/** @file
  Code which support multi-processor.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/CpuInfo.h>
#include "MpCommon.h"
#include "CpuInitDxe.h"
#include "MpService.h"

extern EFI_GUID                         gSmramCpuDataHeaderGuid;
extern MP_SYSTEM_DATA                   *mMPSystemData;
extern EFI_PHYSICAL_ADDRESS             mOriginalBuffer;
extern EFI_PHYSICAL_ADDRESS             mBackupBuffer;
extern EFI_METRONOME_ARCH_PROTOCOL      *mMetronome;
extern CPU_INIT_DATA_HOB                *mCpuInitDataHob;
GLOBAL_REMOVE_IF_UNREFERENCED volatile UINTN                          mSwitchToLegacyRegionCount = 0;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS mLegacyRegion;

/**
  Check if X2APIC is enabled

  @retval  TRUE     If enabled
  @retval  FALSE    If not enabled

**/
BOOLEAN
IsXapicEnabled (
  VOID
  )
{
  UINT64  MsrValue;

  MsrValue = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  if (MsrValue & B_MSR_IA32_APIC_BASE_G_XAPIC) {
    if (MsrValue & B_MSR_IA32_APIC_BASE_M_XAPIC) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}


/**
  Function to get APIC register from MSR or MMIO

  @param[in]  XapicEnabled      x2APIC enabled or not
  @param[in]  MsrIndex          MSR index of APIC register
  @param[in]  MemoryMappedIo    MMIO address for APIC register

  @retval     The value of APIC register

**/
UINT64
ReadApicMsrOrMemory (
  IN BOOLEAN XapicEnabled,
  IN UINT32  MsrIndex,
  IN UINT64  MemoryMappedIo
  )
{
  UINT64  Value;

  if (XapicEnabled) {
    Value = AsmReadMsr64 (MsrIndex);
  } else {
    Value = (UINT64) *(volatile UINT32 *) (UINTN) MemoryMappedIo;
  }

  return Value;
}


/**
  Function to write APIC register by MSR or MMIO

  @param[in]  XapicEnabled      x2APIC enabled or not
  @param[in]  MsrIndex          MSR index of APIC register
  @param[in]  MemoryMappedIo    MMIO address for APIC register
  @param[in]  Value             Value that will be written to APIC register

**/
VOID
WriteApicMsrOrMemory (
  IN BOOLEAN XapicEnabled,
  IN UINT32  MsrIndex,
  IN UINT64  MemoryMappedIo,
  IN UINT64  Value
  )
{
  if (XapicEnabled) {
    AsmWriteMsr64 (MsrIndex, Value);
  } else {
    if (MsrIndex == EXT_XAPIC_ICR) {
      *(volatile UINT32 *) (UINTN) (MemoryMappedIo - APIC_REGISTER_ICR_LOW_OFFSET + APIC_REGISTER_ICR_HIGH_OFFSET) = (UINT32) (Value >> 32);
    }
    *(volatile UINT32 *) (UINTN) MemoryMappedIo = (UINT32) Value;
  }
}


/**
  Send interrupt to CPU

  @param[in]  BroadcastMode            Interrupt broadcast mode
  @param[in]  ApicID                   APIC ID for sending interrupt
  @param[in]  VectorNumber             Vector number
  @param[in]  DeliveryMode             Interrupt delivery mode
  @param[in]  TriggerMode              Interrupt trigger mode
  @param[in]  Assert                   Interrupt pin polarity

  @retval     EFI_INVALID_PARAMETER    Input parameter not correct
  @retval     EFI_NOT_READY            There was a pending interrupt
  @retval     EFI_SUCCESS              Interrupt sent successfully

**/
EFI_STATUS
SendInterrupt (
  IN UINT32  BroadcastMode,
  IN UINT32  ApicID,
  IN UINT32  VectorNumber,
  IN UINT32  DeliveryMode,
  IN UINT32  TriggerMode,
  IN BOOLEAN Assert
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  UINT32                IcrLow;
  UINT32                IcrHigh;
  BOOLEAN               XapicEnabled;

  //
  // Initialze ICR high dword, since P6 family processor needs
  // the destination field to be 0x0F when it is a broadcast
  //
  IcrHigh = 0x0f000000;
  IcrLow  = VectorNumber | (DeliveryMode << 8);

  if (TriggerMode == TRIGGER_MODE_LEVEL) {
    IcrLow |= 0x8000;
  }

  if (Assert) {
    IcrLow |= 0x4000;
  }

  XapicEnabled = IsXapicEnabled ();

  switch (BroadcastMode) {
    case BROADCAST_MODE_SPECIFY_CPU:
      if (XapicEnabled) {
        IcrHigh = (UINT32) ApicID;
      } else {
        IcrHigh = ApicID << 24;
      }
      break;

    case BROADCAST_MODE_ALL_INCLUDING_SELF:
      IcrLow |= 0x80000;
      break;

    case BROADCAST_MODE_ALL_EXCLUDING_SELF:
      IcrLow |= 0xC0000;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000;

  /* If Extended XAPIC Mode is enabled,
     legacy xAPIC is no longer working.
     So, previous MMIO offset must be transferred to MSR offset R/W.
     ----------------------------------------------------------------
     MMIO Offset     MSR Offset     Register Name
     ----------------------------------------------------------------
     300h-310h        830h         Interrupt Command Register [63:0]
                      831h         [Reserved]
     ----------------------------------------------------------------
  */
  WriteApicMsrOrMemory (
    XapicEnabled,
    EXT_XAPIC_ICR,
    ApicBase + APIC_REGISTER_ICR_LOW_OFFSET,
    (((UINT64) IcrHigh << 32) | (UINT64) IcrLow)
    );

  MicroSecondDelay (10);

  IcrLow = (UINT32) ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_ICR, ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);

  if (IcrLow & BIT12) {
    return EFI_NOT_READY;
  }

  MicroSecondDelay (100);

  return EFI_SUCCESS;
}


/**
  Programs Local APIC registers.

  @param[in]  Bsp      Is this BSP?

**/
VOID
ProgramCpuXApic (
  IN BOOLEAN Bsp
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  UINT64                EntryValue;
  BOOLEAN               XapicEnabled;

  ApicBaseReg   = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase      = ApicBaseReg & 0xffffff000;

  XapicEnabled  = IsXapicEnabled ();

  //
  // Program the Spurious Vector entry if XAPIC is enabled
  //
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_SVR, ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
  EntryValue &= 0xFFFFFD0F;
  EntryValue |= 0x10F;
  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_SVR, ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET, EntryValue);

  //
  // Double check if it is BSP
  //
  if (!Bsp) {
    DisableInterrupts ();
  }

  //
  // Program the LINT0 vector entry as EntInt
  //
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT0, ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET);
  if (Bsp) {
    EntryValue &= 0xFFFE00FF;
    EntryValue |= 0x700;
  } else {
    EntryValue |= 0x10000;
  }

  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT0, ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET, EntryValue);

  //
  // Program the LINT1 vector entry as NMI
  //
  EntryValue = ReadApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT1, ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET);
  EntryValue &= 0xFFFE00FF;
  if (Bsp) {
    EntryValue |= 0x400;
  } else {
    EntryValue |= 0x10400;
  }

  WriteApicMsrOrMemory (XapicEnabled, EXT_XAPIC_LVT_LINT1, ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET, EntryValue);

}


/**
  Allocate a temporary memory under 1MB for MP Init to perform INIT-SIPI.
  This buffer also provides memory for stack/data for MP running

  @param[out]  WakeUpBuffer    Return buffer location

  @retval      EFI_SUCCESS     If ok to get a memory under 1MB for MP running.

**/
EFI_STATUS
AllocateWakeUpBuffer (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  for (*WakeUpBuffer = 0x58000; *WakeUpBuffer >= 0x2000; *WakeUpBuffer -= 0x1000) {
    Status = gBS->AllocatePages (AllocateAddress, EfiReservedMemoryType, 1, WakeUpBuffer);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}


/**
  Allocate Reserved Memory

  @param[in]  Size          Memory Size
  @param[in]  Alignment     Alignment size
  @param[out] Pointer       Return memory location

  @retval     EFI_SUCCESS   Allocate a reserved memory successfully

**/
EFI_STATUS
AllocateAlignedReservedMemory (
  IN UINTN Size,
  IN UINTN Alignment,
  OUT VOID **Pointer
  )
{
  EFI_STATUS  Status;
  UINTN       PointerValue;

  Status = AllocateReservedMemoryBelow4G (
             Size + Alignment - 1,
             Pointer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PointerValue  = (UINTN) *Pointer;
  PointerValue  = (PointerValue + Alignment - 1) / Alignment * Alignment;

  *Pointer      = (VOID *) PointerValue;

  return EFI_SUCCESS;
}


/**
  Allocate EfiReservedMemoryType below 4G memory address.

  @param[in]  Size         Size of memory to allocate.
  @param[out] Buffer       Allocated address for output.

  @retval     EFI_SUCCESS  Memory successfully allocated.
  @retval     Other        Other errors occur.

**/
EFI_STATUS
AllocateReservedMemoryBelow4G (
  IN UINTN Size,
  OUT VOID **Buffer
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;

  Pages   = EFI_SIZE_TO_PAGES (Size);
  Address = (EFI_PHYSICAL_ADDRESS) (UINTN) AllocateReservedPages (Pages);
  if (Address != 0) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_OUT_OF_RESOURCES;
  }

  *Buffer = (VOID *) (UINTN) Address;

  return Status;
}


/**
  This function is invoked when SMM_BASE protocol is installed, then we
  allocate SMRAM and save all information there.

**/
VOID
EFIAPI
InitializeSmramDataContent (
  VOID
  )
{
#ifndef SLE_FLAG
  SMRAM_CPU_DATA            SmramCpuDataTemplate;
  UINTN                     LockBoxSize;
  IA32_DESCRIPTOR           *Idtr;
  IA32_DESCRIPTOR           *Gdtr;
  EFI_STATUS                Status;
  EFI_SMM_CONTROL2_PROTOCOL *SmmControl;
  UINT8                     *SmramCpuData;
  UINTN                     ArgBufferSize;
  UINT8                     ArgBuffer;
  CPU_INFO_PROTOCOL         *CpuInfo;

  DEBUG ((DEBUG_INFO, "InitializeSmramDataContent\n"));
  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, (VOID **) &SmmControl);
  ASSERT_EFI_ERROR (Status);

  //
  // Init
  //
  CopyMem (&SmramCpuDataTemplate.HeaderGuid, &gSmramCpuDataHeaderGuid, sizeof (EFI_GUID));
  SmramCpuDataTemplate.AcpiCpuPointer = (EFI_PHYSICAL_ADDRESS) (UINTN) mAcpiCpuData;
  CopyMem (&SmramCpuDataTemplate.AcpiCpuData, mAcpiCpuData, sizeof (ACPI_CPU_DATA));

  //
  // Calculate size
  //
  SmramCpuDataTemplate.GdtrProfileSize        = sizeof (IA32_DESCRIPTOR);
  Gdtr = (IA32_DESCRIPTOR *) (UINTN) mAcpiCpuData->GdtrProfile;
  SmramCpuDataTemplate.GdtSize = Gdtr->Limit + 1;
  SmramCpuDataTemplate.IdtrProfileSize = sizeof (IA32_DESCRIPTOR);
  Idtr = (IA32_DESCRIPTOR *) (UINTN) mAcpiCpuData->GdtrProfile;
  SmramCpuDataTemplate.IdtSize = Idtr->Limit + 1;
  SmramCpuDataTemplate.CpuPrivateDataSize = sizeof (MP_CPU_S3_DATA_POINTER);
  SmramCpuDataTemplate.S3BootScriptTableSize = sizeof (mMPSystemData->S3BootScriptTable);
  SmramCpuDataTemplate.S3BspMtrrTableSize = sizeof (mMPSystemData->S3BspMtrrTable);

  SmramCpuDataTemplate.GdtrProfileOffset    = sizeof (SMRAM_CPU_DATA);
  SmramCpuDataTemplate.GdtOffset            = SmramCpuDataTemplate.GdtrProfileOffset + SmramCpuDataTemplate.GdtrProfileSize;
  SmramCpuDataTemplate.IdtrProfileOffset    = SmramCpuDataTemplate.GdtOffset + SmramCpuDataTemplate.GdtSize;
  SmramCpuDataTemplate.IdtOffset            = SmramCpuDataTemplate.IdtrProfileOffset + SmramCpuDataTemplate.IdtrProfileSize;
  SmramCpuDataTemplate.CpuPrivateDataOffset = SmramCpuDataTemplate.IdtOffset + SmramCpuDataTemplate.IdtSize;
  SmramCpuDataTemplate.S3BootScriptTableOffset = SmramCpuDataTemplate.CpuPrivateDataOffset + SmramCpuDataTemplate.CpuPrivateDataSize;
  SmramCpuDataTemplate.S3BspMtrrTableOffset = SmramCpuDataTemplate.S3BootScriptTableOffset + SmramCpuDataTemplate.S3BootScriptTableSize;

  LockBoxSize = sizeof (SMRAM_CPU_DATA) +
    SmramCpuDataTemplate.GdtrProfileSize +
    SmramCpuDataTemplate.GdtSize +
    SmramCpuDataTemplate.IdtrProfileSize +
    SmramCpuDataTemplate.IdtSize +
    SmramCpuDataTemplate.CpuPrivateDataSize +
    SmramCpuDataTemplate.S3BootScriptTableSize +
    SmramCpuDataTemplate.S3BspMtrrTableSize;

  DEBUG ((DEBUG_INFO, "LockBoxSize                               - %x\n", LockBoxSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.GdtrProfileSize              - %x\n", SmramCpuDataTemplate.GdtrProfileSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.GdtSize                      - %x\n", SmramCpuDataTemplate.GdtSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.IdtrProfileSize              - %x\n", SmramCpuDataTemplate.IdtrProfileSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.IdtSize                      - %x\n", SmramCpuDataTemplate.IdtSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.CpuPrivateDataSize           - %x\n", SmramCpuDataTemplate.CpuPrivateDataSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.S3BootScriptTableSize        - %x\n", SmramCpuDataTemplate.S3BootScriptTableSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.S3BspMtrrTableSize           - %x\n", SmramCpuDataTemplate.S3BspMtrrTableSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData.GdtrProfileOffset            - %x\n", SmramCpuDataTemplate.GdtrProfileOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.GdtOffset                    - %x\n", SmramCpuDataTemplate.GdtOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.IdtrProfileOffset            - %x\n", SmramCpuDataTemplate.IdtrProfileOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.IdtOffset                    - %x\n", SmramCpuDataTemplate.IdtOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.CpuPrivateDataOffset         - %x\n", SmramCpuDataTemplate.CpuPrivateDataOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.S3BootScriptTableOffset      - %x\n", SmramCpuDataTemplate.S3BootScriptTableOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData.S3BspMtrrTableOffset         - %x\n", SmramCpuDataTemplate.S3BspMtrrTableOffset));

  //
  // Allocate Normal Memory
  //
  SmramCpuData = AllocatePool (LockBoxSize);
  ASSERT (SmramCpuData != NULL);

  //
  // Allocate SMRAM
  //
  //
  // Copy data buffer
  //
  CopyMem (SmramCpuData, &SmramCpuDataTemplate, sizeof (SmramCpuDataTemplate));

  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.GdtrProfileOffset,
    (VOID *) (UINTN) mAcpiCpuData->GdtrProfile,
    SmramCpuDataTemplate.GdtrProfileSize
    );
  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.GdtOffset,
    (VOID *) (UINTN) Gdtr->Base,
    SmramCpuDataTemplate.GdtSize
    );
  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.IdtrProfileOffset,
    (VOID *) (UINTN) mAcpiCpuData->IdtrProfile,
    SmramCpuDataTemplate.IdtrProfileSize
    );
  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.IdtOffset,
    (VOID *) (UINTN) Idtr->Base,
    SmramCpuDataTemplate.IdtSize
    );
  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.CpuPrivateDataOffset,
    (VOID *) (UINTN) mAcpiCpuData->CpuPrivateData,
    SmramCpuDataTemplate.CpuPrivateDataSize
    );
  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.S3BootScriptTableOffset,
    (VOID *) (UINTN) mMPSystemData->S3DataPointer.S3BootScriptTable,
    SmramCpuDataTemplate.S3BootScriptTableSize
    );
  //
  // Save Mtrr Register for S3 resume
  //
  SaveBspMtrrForS3 ();

  CopyMem (
    SmramCpuData + SmramCpuDataTemplate.S3BspMtrrTableOffset,
    (VOID *) (UINTN) mMPSystemData->S3DataPointer.S3BspMtrrTable,
    SmramCpuDataTemplate.S3BspMtrrTableSize
    );

  //
  // Locate CPU Info Protocol
  //
  Status = gBS->LocateProtocol (
                  &gCpuInfoProtocolGuid,
                  NULL,
                  (VOID **) &CpuInfo
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Fail to locate CpuInfo protocol would cause S3 resume get error.\n"));
    ASSERT_EFI_ERROR (Status);
    return;
  }

  CpuInfo->SmramCpuInfo->LockBoxData  = (EFI_PHYSICAL_ADDRESS) 0;
  CpuInfo->SmramCpuInfo->SmramCpuData = (EFI_PHYSICAL_ADDRESS) (UINTN) SmramCpuData;
  CpuInfo->SmramCpuInfo->LockBoxSize  = (UINT64) LockBoxSize;
  IoWrite8 (R_PCH_APM_STS, SMM_FROM_CPU_DRIVER_SAVE_INFO);

  //
  // Trigger SMI
  //
  ArgBufferSize = sizeof (ArgBuffer);
  ArgBuffer     = mSmmbaseSwSmiNumber;
  Status        = SmmControl->Trigger (SmmControl, (UINT8 *) &ArgBuffer, (UINT8 *) &ArgBufferSize, FALSE, 0);
  Status        = SmmControl->Clear (SmmControl, 0);
#endif

  return;
}


/**
  This function is invoked when LegacyBios protocol is installed, we must
  allocate reserved memory under 1M for AP.

  @param[in]  Event        The triggered event.
  @param[in]  Context      Context for this event.

**/
VOID
EFIAPI
ReAllocateEbdaMemoryForAP (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_PHYSICAL_ADDRESS      EbdaOld;
  EFI_PHYSICAL_ADDRESS      EbdaNew;
  UINTN                     EbdaSize;
  EFI_STATUS                Status;

  //
  // Check whether this is real LegacyBios notification
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // PLEASE NOTE:
  // For legacy implementation, we have reserved 0x9F000 to 0x9FFFF for S3 usage in CSM,
  // No don't need to allocate it again
  // This range will be used for MpS3 driver and S3Resume driver on S3 boot path
  // The base needs to be aligned to 4K to satisfy the AP vector requirement
  // The original implementation requires 8K from legacy memory form E/F segment,
  // which needs lock/unlock and makes lots of code chipset dependent on S3 boot path
  // Here we just use normal low memory to eliminate the dependency
  // In this case, EBDA will start from 0x9F000 - sizeof (EBDA) in CSM definition
  // CSM EBDA base and memory size in BDA area needs to be consistent with this
  //
  //
  // Get EDBA address/length and turn it into the S3 reserved address
  // The length of this range is limited so we need to keep the real mode code small
  //
  EbdaOld                     = (EFI_PHYSICAL_ADDRESS) (*(UINT16 *) (UINTN) 0x40E) << 4;
  EbdaSize                    = (UINTN) (*((UINT8 *) (UINTN) EbdaOld));
  mLegacyRegion               = EbdaOld + (EbdaSize << 10);
  mLegacyRegion               = (mLegacyRegion - 0x1000) & 0xFFFFF000;
  EbdaNew                     = mLegacyRegion - (EbdaSize << 10);
  (*(UINT16 *) (UINTN) 0x40E) = (UINT16) (EbdaNew >> 4);
  CopyMem ((VOID *) (UINTN) EbdaNew, (VOID *) (UINTN) EbdaOld, EbdaSize << 10);

  //
  // Update 40:13 with the new size of available base memory
  //
  *(UINT16 *) (UINTN) 0x413 = (*(UINT16 *) (UINTN) 0x413) - (UINT16) (((EbdaOld - EbdaNew) >> 10));

  //
  // Free the Wake-up buffer and re-declare it as Reserved Memory
  //
  DEBUG ((DEBUG_INFO, "Legacy region freed before re-allocation:  %X\n", mLegacyRegion));
  Status  = (gBS->FreePages) (mLegacyRegion, 1);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "Allocate and reserve the 4K buffer for Legacy Region\n"));
  Status = (gBS->AllocatePages)(AllocateAddress, EfiReservedMemoryType, 1, &mLegacyRegion);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "mLegacyRegion CSM - %x\n", mLegacyRegion));
}


/**
  This callback function will be executed when EndofDxe event is signaled.
  This function will do the following:
  Allocate memory (wakeup buffer) for AP
  Set all threads to deepest C states
  Initialize SMRAM contents

  @param[in]  Event      The triggered event.
  @param[in]  Context    Context for this event.

**/
VOID
EFIAPI
ReAllocateMemoryForAP (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
#ifdef SLE_FLAG
  STATIC BOOLEAN InitDone = FALSE;
  //
  // Make sure it is invoked only once.
  //
  if (InitDone) {
    return;
  }

  InitDone  = TRUE;

  while (ApRunning ()) {  // SE06
    CpuPause ();
  }

#else
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_STATUS                Status;
  EFI_PHYSICAL_ADDRESS      LegacyRegion;
  MP_CPU_EXCHANGE_INFO      *ExchangeInfo;
  BOOLEAN                   HasCsm;
  STATIC BOOLEAN            InitDone = FALSE;
  CPU_CONFIG                *CpuConfig;

  //
  // Make sure it is invoked only once.
  //
  if (InitDone) {
    return;
  }

  InitDone  = TRUE;

  Status    = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    HasCsm = FALSE;
  } else {
    HasCsm = TRUE;
  }

  while (ApRunning ()) {
    CpuPause ();
  }

  if (HasCsm) {
    LegacyRegion = mLegacyRegion;
    DEBUG ((DEBUG_INFO, "Using LegacyRegion CSM - %x\n", LegacyRegion));
  } else {
    //
    // The BackBuffer is 4k. Allocate 0x2000 bytes from below 640K memory to ensure 4k aligned spaces of 0x1000 bytes,
    // since Alignment argument does not work.
    //
    LegacyRegion = 0x9FFFF;
    Status = gBS->AllocatePages (AllocateMaxAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (0x2000), &LegacyRegion);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "LegacyRegion NonCSM - %x\n", LegacyRegion));
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  //
  // This address should be less than A seg.
  // And it should be aligned to 4K
  //
  ASSERT (!((UINTN) LegacyRegion & 0x0FFF) && ((UINTN) LegacyRegion < 0xA0000));

  mAcpiCpuData->WakeUpBuffer  = (EFI_PHYSICAL_ADDRESS) LegacyRegion;
  mAcpiCpuData->WakeUpBuffer  = (mAcpiCpuData->WakeUpBuffer + 0x0fff) & 0x0fffff000;

  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mBackupBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  ExchangeInfo->BufferStart   = (UINT32) mAcpiCpuData->WakeUpBuffer;
  CopyMem (
    (VOID *) (UINTN) mAcpiCpuData->WakeUpBuffer,
    (VOID *) (UINTN) mBackupBuffer,
    EFI_PAGE_SIZE
    );
  RedirectFarJump ();

  if (HasCsm) {
    Status = LegacyBios->CopyLegacyRegion (
                           LegacyBios,
                           sizeof (MP_CPU_EXCHANGE_INFO),
                           (VOID *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET),
                           (VOID *) (UINTN) (mBackupBuffer + MP_CPU_EXCHANGE_INFO_OFFSET)
                           );
  }

  //
  // Set all APs to deepest C-State before ready to boot for better power saving,
  // if boot to DOS/EFI_SHARE or any operating system that running only single thread.
  //
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  CpuConfig = (CPU_CONFIG *) (UINTN) mCpuInitDataHob->CpuConfig;
  if (CpuConfig->ApHandoffManner != WakeUpApPerHltLoop) {
  } else {
    ExchangeInfo->WakeUpApManner = WakeUpApPerHltLoop;
  }

  //
  // Invoke the InitializeSmram directly, since it is in EndOfDxe event.
  //
  InitializeSmramDataContent ();
#endif  //SLE_FLAG
}


/**
  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, reset AP's wakeup buffer address,
  preparing for S3 usage.

  @param[in]  Event      The triggered event.
  @param[in]  Context    Context for this event.

**/
VOID
EFIAPI
ResetAps (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  return;
}

