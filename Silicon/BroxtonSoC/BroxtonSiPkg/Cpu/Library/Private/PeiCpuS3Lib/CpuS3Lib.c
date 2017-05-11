/** @file
  Cpu S3 library running on S3 resume paths.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Private/Library/CpuS3Lib.h>
#include "CpuS3.h"
#include <Library/CpuPolicyLib.h>
#include <Library/PostCodeLib.h>

extern EFI_GUID gSmramCpuDataHeaderGuid;
extern EFI_GUID gPeiAcpiCpuDataGuid;

typedef VOID (*S3_AP_PROCEDURE) (
  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  UINT64               *MtrrValues
  );

/**
  This function handles CPU S3 resume task at the end of PEI

  @param[in] PeiServices     Pointer to PEI Services Table.
  @param[in] NotifyDesc      Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi             Pointer to the PPI data associated with this function.

  @retval    EFI_STATUS      Always return EFI_SUCCESS

**/
STATIC
EFI_STATUS
CpuS3ResumeAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  );


STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpuS3ResumeNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  CpuS3ResumeAtEndOfPei
};


/**
  Restore all MSR settings

  @param[in]  ScriptTable     Contain the MSR settings that will be restored.

**/
VOID
InitializeFeatures (
  IN MP_CPU_S3_SCRIPT_DATA *ScriptTable
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;
  UINT32              ApicId;
  UINT8               SkipMsr;

  //
  // Restore all the MSRs for processor
  //
  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  ApicId = (CpuidRegisters.RegEbx >> 24);

  while (ScriptTable->MsrIndex != 0) {
    if (ApicId == ScriptTable->ApicId) {
      SkipMsr = 0;
      if ((ScriptTable->MsrIndex == MSR_PMG_CST_CONFIG) && (AsmReadMsr64 (MSR_PMG_CST_CONFIG) & B_CST_CONTROL_LOCK)) {
        SkipMsr = 1;
      }
      if ((ScriptTable->MsrIndex == MSR_IA32_FEATURE_CONTROL) && (AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL) & B_MSR_IA32_FEATURE_CONTROL_LOCK)) {
        SkipMsr = 1;
      }
      if (SkipMsr == 0) {
        AsmWriteMsr64 (ScriptTable->MsrIndex, ScriptTable->MsrValue);
      }
    }
    ScriptTable++;
  }
}


/**
  Restore all MSR settings with debug message output

  @param[in]  ScriptTable     Script table contain all MSR settings that will be restored

**/
VOID
InitializeFeaturesLog (
  IN MP_CPU_S3_SCRIPT_DATA *ScriptTable
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;
  UINT32              ApicId;
  BOOLEAN             SkipMsr;

  PostCode (0xC41);

  //
  // Restore all the MSRs for processor
  //
  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  ApicId = (CpuidRegisters.RegEbx >> 24);

  while (ScriptTable->MsrIndex != 0) {
    if (ApicId == ScriptTable->ApicId) {
      DEBUG ((DEBUG_INFO, "MSR Index - %x, MSR value - %x\n", ScriptTable->MsrIndex, ScriptTable->MsrValue));
      SkipMsr = FALSE;
      if ((ScriptTable->MsrIndex == MSR_PMG_CST_CONFIG) && (AsmReadMsr64 (MSR_PMG_CST_CONFIG) & B_CST_CONTROL_LOCK)) {
        SkipMsr = TRUE;
      }
      if ((ScriptTable->MsrIndex == MSR_IA32_FEATURE_CONTROL) && (AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL) & B_MSR_IA32_FEATURE_CONTROL_LOCK)) {
        SkipMsr = TRUE;
      }
      if (ScriptTable->MsrIndex == MSR_IA32_DEBUG_INTERFACE) {
        //
        // Debug interface is supported if CPUID (EAX=1): ECX[11] = 1
        //
        if ((CpuidRegisters.RegEcx & BIT11) && (AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE) & B_DEBUG_INTERFACE_LOCK)) {
            SkipMsr = TRUE;
        }
      }
      if (!SkipMsr) {
        AsmWriteMsr64 (ScriptTable->MsrIndex, ScriptTable->MsrValue);
      }
    }
    ScriptTable++;
  }
  PostCode (0xC44);
}


/**
  AP initialization

  @param[in]  ExchangeInfo    Pointer to the exchange info buffer for output.
  @param[in]  MtrrValues      Buffer contains MTRR settings

**/
VOID
MPRendezvousProcedure (
  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  UINT64               *MtrrValues
  )
{
  EFI_STATUS              Status;
  UINT32                  FailedRevision;
  ACPI_CPU_DATA           *AcpiCpuData;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;
  EFI_CPUID_REGISTER      CpuidRegisters;
  CPU_CONFIG              *CpuConfig;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) ExchangeInfo->SiCpuPolicyPpi, &gCpuConfigGuid , (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );

  //
  // Init XMM support on all APs
  //
  XmmInit ();

  //
  // Switch AP speed to BSP speed
  //
  if ((CpuidRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_EIST) != 0) {
    AsmWriteMsr64 (MSR_IA32_PERF_CTRL, ExchangeInfo->CpuPerfCtrlValue);
  }

  AcpiCpuData = (ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress);

  ProgramXApic (FALSE);

  InitializeFeatures (ExchangeInfo->S3BootScriptTable);

  InterlockedIncrement (&(ExchangeInfo->SerializeLock));
  while (ExchangeInfo->SerializeLock < AcpiCpuData->NumberOfCpus) {
    CpuPause ();
  }

  InitializeMicrocode (
    ExchangeInfo,
    (CPU_MICROCODE_HEADER *) (UINTN) CpuConfig->MicrocodePatchAddress,
    &FailedRevision
    );

  ProcessorsPrefetcherInitialization (
    CpuConfig->MlcStreamerPrefetcher,
    CpuConfig->MlcSpatialPrefetcher
    );

  //
  // wait till all CPUs done the Microcode Load
  //
  while (ExchangeInfo->McuLoadCount < AcpiCpuData->NumberOfCpus) {
    CpuPause ();
  }

  MpMtrrSynchUp (MtrrValues);

  InterlockedIncrement (&(ExchangeInfo->FinishedCount));

  //
  // Sempahore check loop executed in memory
  //
  (*ExchangeInfo->SemaphoreCheck) (&ExchangeInfo->FinishedCount);

  InterlockedIncrement (&(ExchangeInfo->WakeupCount));

  //
  // Restore the MTRR programmed before OS boot
  //
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  SetMtrrRegisters ((EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);

  while (ExchangeInfo->WakeupCount < AcpiCpuData->NumberOfCpus - 1) {
    CpuPause ();
  }

  InterlockedIncrement (&(ExchangeInfo->FinishedCount));
}


/**
  Wake up all the application processors

  @param[in] PeiServices     Indirect reference to the PEI Services Table
  @param[in] AcpiCpuData     Pointer to ACPI_CPU_DATA structure
  @param[in] MtrrValues      Pointer to a buffer which stored MTRR settings

  @retval    EFI_SUCCESS     APs are successfully waked up

**/
EFI_STATUS
S3WakeUpAps (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  ACPI_CPU_DATA                *AcpiCpuData,
  UINT64                       *MtrrValues,
  S3_AP_PROCEDURE              Function
  )
{
  EFI_PHYSICAL_ADDRESS    WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO    *ExchangeInfo;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;

  PostCode (0xC45);
  WakeUpBuffer = AcpiCpuData->WakeUpBuffer;
  CopyMem ((VOID *) (UINTN) WakeUpBuffer, S3AsmGetAddressMap (), 0x1000 - 0x200);

  ExchangeInfo                        = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));

  ExchangeInfo->Lock                  = 0;
  ExchangeInfo->StackStart            = (UINTN) AcpiCpuData->StackAddress;
  ExchangeInfo->StackSize             = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction            = (UINT32) Function;
  ExchangeInfo->BufferStart           = (UINT32) WakeUpBuffer;
  ExchangeInfo->PmodeOffset           = (UINT32) (S3AsmGetPmodeOffset ());
  ExchangeInfo->SemaphoreCheck        = (VOID (*)(UINT32 *)) (S3AsmGetSemaphoreCheckOffset () + (UINT32) WakeUpBuffer);
  ExchangeInfo->AcpiCpuDataAddress    = (UINT32) AcpiCpuData;
  ExchangeInfo->MtrrValuesAddress     = (UINT32) MtrrValues;
  ExchangeInfo->FinishedCount         = (UINT32) 0;
  ExchangeInfo->SerializeLock         = (UINT32) 0;
  ExchangeInfo->StartState            = (UINT32) 0;

  CpuS3DataPtr                        = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  ExchangeInfo->S3BootScriptTable     = (MP_CPU_S3_SCRIPT_DATA *) (UINTN) CpuS3DataPtr->S3BootScriptTable;
  ExchangeInfo->VirtualWireMode       = CpuS3DataPtr->VirtualWireMode;
  ExchangeInfo->PeiServices           = PeiServices;
  ExchangeInfo->CpuPerfCtrlValue      = AsmReadMsr64 (MSR_IA32_PERF_CTRL);

  ExchangeInfo->SiCpuPolicyPpi  = NULL;
  PeiServicesLocatePpi (
    &gSiCpuPolicyPpiGuid,
    0,
    NULL,
    (VOID **) &(ExchangeInfo->SiCpuPolicyPpi)
    );

  CopyMem (
    (VOID *) (UINTN) &ExchangeInfo->GdtrProfile,
    (VOID *) (UINTN) AcpiCpuData->GdtrProfile,
    sizeof (IA32_DESCRIPTOR)
    );
  CopyMem (
    (VOID *) (UINTN) &ExchangeInfo->IdtrProfile,
    (VOID *) (UINTN) AcpiCpuData->IdtrProfile,
    sizeof (IA32_DESCRIPTOR)
    );

  DEBUG ((DEBUG_INFO, "Cpu S3 Bootscript at %08X\n", (UINT32) ExchangeInfo->S3BootScriptTable));

  //
  // Don't touch MPCPU private data
  // Here we use ExchangeInfo instead
  //
  //
  // Send INIT IPI - SIPI to all APs
  //
  SendInterrupt (
    BROADCAST_MODE_ALL_EXCLUDING_SELF,
    0,
    0,
    DELIVERY_MODE_INIT,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  MicroSecondDelay (10 * STALL_ONE_MILLI_SECOND); //< 10ms

  SendInterrupt (
    BROADCAST_MODE_ALL_EXCLUDING_SELF,
    0,
    (UINT32) RShiftU64 (WakeUpBuffer, 12),
    DELIVERY_MODE_SIPI,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  MicroSecondDelay (200 * STALL_ONE_MICRO_SECOND); //< 200us

  SendInterrupt (
    BROADCAST_MODE_ALL_EXCLUDING_SELF,
    0,
    (UINT32) RShiftU64 (WakeUpBuffer, 12),
    DELIVERY_MODE_SIPI,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  MicroSecondDelay (200 * STALL_ONE_MICRO_SECOND); //< 200us
  PostCode (0xC48);

  return EFI_SUCCESS;
}


/**
  This routine is used to search SMRAM and get SmramCpuData point.

  @param[in] PeiServices    PEI services global pointer
  @param[in] SmmAccessPpi   SmmAccess PPI instance

  @retval    SmramCpuData   The pointer of CPU information in SMRAM.

**/
STATIC
SMRAM_CPU_DATA *
GetSmmCpuData (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN PEI_SMM_ACCESS_PPI        *SmmAccessPpi
  )
{
  EFI_SMRAM_DESCRIPTOR  *SmramRanges;
  UINTN                 SmramRangeCount;
  UINTN                 Size;
  EFI_STATUS            Status;
  UINT32                Address;
  SMRAM_CPU_DATA        *SmramCpuData;

  //
  // Get all SMRAM range
  //
  Size    = 0;
  Status  = SmmAccessPpi->GetCapabilities ((EFI_PEI_SERVICES **) PeiServices, SmmAccessPpi, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  Status = PeiServicesAllocatePool (
             Size,
             (VOID **) &SmramRanges
             );
  ASSERT_EFI_ERROR (Status);

  Status = SmmAccessPpi->GetCapabilities ((EFI_PEI_SERVICES **) PeiServices, SmmAccessPpi, &Size, SmramRanges);
  ASSERT_EFI_ERROR (Status);

  Size /= sizeof (*SmramRanges);
  SmramRangeCount = Size;

  //
  // We assume TSEG is the last range of SMRAM in SmramRanges
  //
  SmramRanges += SmramRangeCount - 1;

  DEBUG ((DEBUG_INFO, "TsegBase - %x\n", SmramRanges->CpuStart));
  DEBUG ((DEBUG_INFO, "TsegTop  - %x\n", SmramRanges->CpuStart + SmramRanges->PhysicalSize));

  //
  // Search SMRAM on page alignment for the SMMNVS signature
  //
  for (Address = (UINT32) (SmramRanges->CpuStart + SmramRanges->PhysicalSize - EFI_PAGE_SIZE);
       Address >= (UINT32) SmramRanges->CpuStart;
       Address -= EFI_PAGE_SIZE
       ) {
    SmramCpuData = (SMRAM_CPU_DATA *) (UINTN) Address;
    if (CompareGuid (&SmramCpuData->HeaderGuid, &gSmramCpuDataHeaderGuid)) {
      return SmramCpuData;
    }
  }

  ASSERT (FALSE);

  return NULL;
}

/**
  This routine is restore the CPU information from SMRAM to original reserved memory region.

  @param[in] PeiServices    PEI services global pointer

  @retval    AcpiCpuData    The pointer of CPU information in reserved memory.

**/
ACPI_CPU_DATA *
RestoreSmramCpuData (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  PEI_SMM_ACCESS_PPI      *SmmAccessPpi;
  SMRAM_CPU_DATA          *SmramCpuData;
  EFI_STATUS              Status;
  ACPI_CPU_DATA           *AcpiCpuData;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;
  IA32_DESCRIPTOR         *Idtr;
  IA32_DESCRIPTOR         *Gdtr;
  UINTN                   Index;

  Status = PeiServicesLocatePpi (
             &gPeiSmmAccessPpiGuid,
             0,
             NULL,
             (VOID **) &SmmAccessPpi
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Open all SMM regions
  //
  Index = 0;
  do {
    Status = SmmAccessPpi->Open ((EFI_PEI_SERVICES **) PeiServices, SmmAccessPpi, Index);
    Index++;
  } while (!EFI_ERROR (Status));

  SmramCpuData = GetSmmCpuData ((CONST EFI_PEI_SERVICES **) PeiServices, SmmAccessPpi);
  if (SmramCpuData == NULL) {
    ASSERT (FALSE);
    return NULL;
  }
  DEBUG ((DEBUG_INFO, "CpuS3 SmramCpuData                         - 0x%x \n", SmramCpuData));
  DEBUG ((DEBUG_INFO, "SmramCpuData->GdtrProfileSize              - %x\n", SmramCpuData->GdtrProfileSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->GdtSize                      - %x\n", SmramCpuData->GdtSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->IdtrProfileSize              - %x\n", SmramCpuData->IdtrProfileSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->IdtSize                      - %x\n", SmramCpuData->IdtSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->CpuPrivateDataSize           - %x\n", SmramCpuData->CpuPrivateDataSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->S3BootScriptTableSize        - %x\n", SmramCpuData->S3BootScriptTableSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->S3BspMtrrTableSize           - %x\n", SmramCpuData->S3BspMtrrTableSize));
  DEBUG ((DEBUG_INFO, "SmramCpuData->GdtrProfileOffset            - %x\n", SmramCpuData->GdtrProfileOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->GdtOffset                    - %x\n", SmramCpuData->GdtOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->IdtrProfileOffset            - %x\n", SmramCpuData->IdtrProfileOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->IdtOffset                    - %x\n", SmramCpuData->IdtOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->CpuPrivateDataOffset         - %x\n", SmramCpuData->CpuPrivateDataOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->S3BootScriptTableOffset      - %x\n", SmramCpuData->S3BootScriptTableOffset));
  DEBUG ((DEBUG_INFO, "SmramCpuData->S3BspMtrrTableOffset         - %x\n", SmramCpuData->S3BspMtrrTableOffset));

  //
  // Start restore data to NVS
  //
  AcpiCpuData = (ACPI_CPU_DATA *) (UINTN) SmramCpuData->AcpiCpuPointer;
  CopyMem (AcpiCpuData, &SmramCpuData->AcpiCpuData, sizeof (ACPI_CPU_DATA));

  CopyMem (
    (VOID *) (UINTN) AcpiCpuData->GdtrProfile,
    (UINT8 *) SmramCpuData + SmramCpuData->GdtrProfileOffset,
    SmramCpuData->GdtrProfileSize
    );
  Gdtr = (IA32_DESCRIPTOR *) (UINTN) AcpiCpuData->GdtrProfile;
  CopyMem (
    (VOID *) (UINTN) Gdtr->Base,
    (UINT8 *) SmramCpuData + SmramCpuData->GdtOffset,
    SmramCpuData->GdtSize
    );
  CopyMem (
    (VOID *) (UINTN) AcpiCpuData->IdtrProfile,
    (UINT8 *) SmramCpuData + SmramCpuData->IdtrProfileOffset,
    SmramCpuData->IdtrProfileSize
    );
  Idtr = (IA32_DESCRIPTOR *) (UINTN) AcpiCpuData->IdtrProfile;
  CopyMem (
    (VOID *) (UINTN) Idtr->Base,
    (UINT8 *) SmramCpuData + SmramCpuData->IdtOffset,
    SmramCpuData->IdtSize
    );
  CopyMem (
    (VOID *) (UINTN) AcpiCpuData->CpuPrivateData,
    (UINT8 *) SmramCpuData + SmramCpuData->CpuPrivateDataOffset,
    SmramCpuData->CpuPrivateDataSize
    );
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  CopyMem (
    (VOID *) (UINTN) CpuS3DataPtr->S3BootScriptTable,
    (UINT8 *) SmramCpuData + SmramCpuData->S3BootScriptTableOffset,
    SmramCpuData->S3BootScriptTableSize
    );
  CopyMem (
    (VOID *) (UINTN) CpuS3DataPtr->S3BspMtrrTable,
    (UINT8 *) SmramCpuData + SmramCpuData->S3BspMtrrTableOffset,
    SmramCpuData->S3BspMtrrTableSize
    );
  //
  // Close all SMM regions
  //
  Index = 0;
  do {
    Status = SmmAccessPpi->Close ((EFI_PEI_SERVICES **) PeiServices, SmmAccessPpi, Index);
    Index++;
  } while (!EFI_ERROR (Status));

  return AcpiCpuData;
}


/**
  Cpu initialization called during S3 resume to take care
  of CPU related activities in PEI phase

  @param[in] PeiServices      Indirect reference to the PEI Services Table

  @retval    EFI_SUCCESS      Multiple processors are intialized successfully.

**/
EFI_STATUS
S3InitializeCpu (
  IN  EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS              Status;
  ACPI_CPU_DATA           *AcpiCpuData;
  UINT64                  *MtrrValues;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;
  UINTN                   VariableMtrrNumber;
  EFI_PHYSICAL_ADDRESS    WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO    *ExchangeInfo;
  UINT32                  FailedRevision;
  VOID                    *Hob;
  EFI_BOOT_MODE           BootMode;
  CPU_CONFIG              *CpuConfig;

  Status = PeiServicesGetBootMode (&BootMode);
  DEBUG ((DEBUG_INFO, "CPU: BootMode = %X\n", BootMode));
  if ((Status == EFI_SUCCESS) && (BootMode != BOOT_ON_S3_RESUME)) {
    DEBUG ((DEBUG_INFO,"CPU: Normal Boot\n"));
    return EFI_SUCCESS;
  }

  DEBUG((DEBUG_INFO, "S3InitializeCpu Start \n"));
  PostCode (0xC40);

  //
  // Restore ACPI Nvs data from SMRAM
  //
  AcpiCpuData = RestoreSmramCpuData ((CONST EFI_PEI_SERVICES **) PeiServices);
  if (AcpiCpuData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((DEBUG_INFO, "CpuS3 RestoreSmramCpuData - 0x%x \n", AcpiCpuData));

  AcpiCpuData->S3BootPath = TRUE;

  CpuS3DataPtr        = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;

  VariableMtrrNumber  = (UINTN) ((UINT64) AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT) * 2;
  Status = PeiServicesAllocatePool (
             (FixedMtrrNumber + MtrrDefTypeNumber + VariableMtrrNumber) * sizeof (UINT64),
             (VOID **) &MtrrValues
             );
  ASSERT_EFI_ERROR (Status);
  ReadMtrrRegisters (MtrrValues);

  WakeUpBuffer                = AcpiCpuData->WakeUpBuffer;
  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));
  ExchangeInfo->WakeupCount   = (UINT32) 0;
  ExchangeInfo->FinishedCount = (UINT32) 0;
  ExchangeInfo->SerializeLock = (UINT32) 0;

  //
  // Restore features for BSP
  //
  InitializeFeaturesLog ((MP_CPU_S3_SCRIPT_DATA *) CpuS3DataPtr->S3BootScriptTable);

  //
  // Restore AP configuration
  //
  S3WakeUpAps ((CONST EFI_PEI_SERVICES **)PeiServices, AcpiCpuData, MtrrValues, MPRendezvousProcedure);

  //
  // Program XApic register
  //
  ProgramXApic (
    TRUE
    );

  InterlockedIncrement (&(ExchangeInfo->SerializeLock));
  while (ExchangeInfo->SerializeLock < AcpiCpuData->NumberOfCpus) {
    CpuPause ();
  }

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) ExchangeInfo->SiCpuPolicyPpi, &gCpuConfigGuid , (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "Initialize Microcode Start \n"));
  PostCode (0xC49);
  InitializeMicrocode (
    ExchangeInfo,
    (CPU_MICROCODE_HEADER *) (UINTN) CpuConfig->MicrocodePatchAddress,
    &FailedRevision
    );

  DEBUG ((DEBUG_INFO, "BuildGuid Data Hob Start \n"));
  PostCode (0xC4D);
  //
  // Save acpi cpu data into one hob, it will be used by a callback when End of Pei Signal installed.
  //
  Hob = BuildGuidDataHob (
          &gPeiAcpiCpuDataGuid,
          (VOID *) (UINTN) AcpiCpuData,
          (UINTN) sizeof (ACPI_CPU_DATA)
          );
  ASSERT (Hob != NULL);
  DEBUG ((DEBUG_INFO, "CPU S3: Register notification to be trigerred at End of Pei event\n"));
  Status = PeiServicesNotifyPpi (&mCpuS3ResumeNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  //
  // Wait for all APs to complete
  //
  while (ExchangeInfo->FinishedCount < AcpiCpuData->NumberOfCpus - 1) {
    CpuPause ();
  }

  DEBUG ((DEBUG_INFO, "S3InitializeCpu Done \n"));
  PostCode (0xC5F);

  return EFI_SUCCESS;
}


/**
  This function handles CPU S3 resume task at the end of PEI

  @param[in] PeiServices     Pointer to PEI Services Table.
  @param[in] NotifyDesc      Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi             Pointer to the PPI data associated with this function.

  @retval    EFI_STATUS      Always return EFI_SUCCESS

**/
STATIC
EFI_STATUS
CpuS3ResumeAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  ACPI_CPU_DATA           *AcpiCpuData;
  EFI_PHYSICAL_ADDRESS    WakeUpBuffer;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;
  MP_CPU_EXCHANGE_INFO    *ExchangeInfo;
  VOID                    *Hob;

  DEBUG ((DEBUG_INFO, "Cpu S3 callback Entry\n"));

  PostCode (0xC51);

  //
  // Find the saved acpi cpu data from HOB.
  //
  AcpiCpuData = NULL;
  Hob = GetFirstGuidHob (&gPeiAcpiCpuDataGuid);
  if (Hob != NULL) {
    AcpiCpuData = (ACPI_CPU_DATA *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));
    ASSERT (AcpiCpuData != NULL);
    if (AcpiCpuData == NULL) {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  WakeUpBuffer                = AcpiCpuData->WakeUpBuffer;
  ExchangeInfo                = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + (0x1000 - 0x200));

  //
  // Have APs to continue the task - Restore S3BspMtrrTable
  //
  ExchangeInfo->WakeupCount   = (UINT32) 0;
  ExchangeInfo->FinishedCount = (UINT32) 0;

  //
  // Set MTRR to the final values
  // Do not do it too early so as to avoid performance penalty
  //
  CpuS3DataPtr = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;

#ifdef EFI_DEBUG
  ShowMtrrRegisters ((EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);
#endif

  SetMtrrRegisters ((EFI_MTRR_VALUES *) CpuS3DataPtr->S3BspMtrrTable);

  MicroSecondDelay (1 * STALL_ONE_MILLI_SECOND); //< 1ms

  while (ExchangeInfo->FinishedCount < AcpiCpuData->NumberOfCpus - 1) {
    CpuPause ();
  }
  PostCode (0xC54);

  return EFI_SUCCESS;
}

