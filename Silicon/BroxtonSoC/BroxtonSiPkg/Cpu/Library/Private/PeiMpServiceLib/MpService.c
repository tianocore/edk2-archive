/** @file
  PEIM to initialize multi-processor.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Private/Library/CpuCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/MasterBootMode.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include "MpService.h"
#include <Ppi/CpuPolicy.h>
#include <Private/CpuInitDataHob.h>
#include <Features.h>
#include <Library/CpuPolicyLib.h>
#include <Library/PostCodeLib.h>
#include <Library/MtrrLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED MP_CPU_RUNTIME_DATA         *mMpCpuRuntimeData     = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED MP_SYSTEM_DATA              *mMpSystemData         = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED SI_CPU_POLICY_PPI           *mSiCpuPolicyPpi       = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_CONFIG                  *mCpuConfig            = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_CONFIG_PREMEM           *mCpuConfigPreMem      = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED POWER_MGMT_CONFIG           *mPowerMgmtConfig      = NULL;


/**
  This function handles CPU MP service task at the end of PEI

  @param[in] PeiServices     Pointer to PEI Services Table.
  @param[in] NotifyDesc      Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi             Pointer to the PPI data associated with this function.

  @retval    EFI_STATUS      Always return EFI_SUCCESS

**/
STATIC
EFI_STATUS
CpuMpServiceAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpuMpServiceNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  CpuMpServiceAtEndOfPei
};


/**
  Allocate a temporary memory under 1MB for MP Init to perform INIT-SIPI.
  This buffer also provides memory for stack/data to run MP routine.

  @param[in] WakeUpBuffer  - Return buffer location

  @retval EFI_SUCCESS if ok to get a memory under 1MB for MP running.
**/
EFI_STATUS
AllocateWakeUpBuffer (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer
  )
{
  EFI_STATUS    Status;

  Status        = EFI_SUCCESS;
  *WakeUpBuffer = 0x58000;

  return Status;
}


/**
  Prepare Wakeup Buffer and stack for APs.

  @param[in] WakeUpBuffer               Pointer to the address of wakeup buffer for output.
  @param[in] StackAddressStart          Pointer to the stack address of APs for output.
  @param[in] MaximumCPUsForThisSystem   Maximum CPUs in this system.

  @retval    EFI_SUCCESS                Memory successfully prepared for APs.
  @retval    Other                      Error occurred while allocating memory.

**/
EFI_STATUS
PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer,
  OUT VOID                 **StackAddressStart,
  IN UINTN                 MaximumCPUsForThisSystem
  )
{
  EFI_STATUS              Status;

  //
  // Release All APs with a lock and wait for them to retire to rendezvous procedure.
  // We need a page (4KB) of memory for IA-32 to use broadcast APIs, on a temporary basis.
  //
 Status = AllocateWakeUpBuffer (WakeUpBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Claim memory for AP stack
  //
  *StackAddressStart = AllocateRuntimePool (MaximumCPUsForThisSystem * STACK_SIZE_PER_PROC);

  if (*StackAddressStart == NULL) {
    FreePool (WakeUpBuffer);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}


/**
  Prepare GDTR and IDTR for AP

  @param[in] Gdtr          The GDTR profile
  @param[in] Idtr          The IDTR profile

  @retval    EFI_STATUS    Status returned by each sub-routine
  @retval    EFI_SUCCESS   GDTR and IDTR has been prepared for AP

**/
EFI_STATUS
PrepareGdtIdtForAP (
  OUT IA32_DESCRIPTOR *Gdtr,
  OUT IA32_DESCRIPTOR *Idtr
  )
{
  INTERRUPT_GATE_DESCRIPTOR  *IdtForAP;
  SEGMENT_DESCRIPTOR         *GdtForAP;
  IA32_DESCRIPTOR            IdtrForBSP;
  IA32_DESCRIPTOR            GdtrForBSP;
  UINT16                     *MceHandler;

  //
  // Get Global Descriptor Table Register(GDTR) descriptor
  //
  AsmReadGdtr (&GdtrForBSP);

  //
  // Get Interrupt Descriptor Table Register(IDTR) descriptor
  //
  AsmReadIdtr (&IdtrForBSP);

  //
  // Allocate reserved memory for IDT
  //
  IdtForAP = (INTERRUPT_GATE_DESCRIPTOR *) AllocateAlignedRuntimePages (EFI_SIZE_TO_PAGES (IdtrForBSP.Limit + 1), sizeof (INTERRUPT_GATE_DESCRIPTOR));
  if (IdtForAP == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Allocate reserved memory for GDT
  //
  GdtForAP = (SEGMENT_DESCRIPTOR *) AllocateAlignedRuntimePages (EFI_SIZE_TO_PAGES (GdtrForBSP.Limit + 1), sizeof (SEGMENT_DESCRIPTOR));
  if (GdtForAP == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MceHandler = AllocateRuntimePool (SIZE_OF_MCE_HANDLER);
  if (MceHandler == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // MceHandler content: iret (opcode = 0xcf)
  //
  *MceHandler = 0xCF48;

  CopyMem (GdtForAP, (VOID *) GdtrForBSP.Base, GdtrForBSP.Limit + 1);
  CopyMem (IdtForAP, (VOID *) IdtrForBSP.Base, IdtrForBSP.Limit + 1);

  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].Offset15To0   = (UINT16) (UINTN) MceHandler;
  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].Offset31To16  = (UINT16) ((UINTN) MceHandler >> 16);

  //
  // Create Gdtr, IDTR profile
  //
  Gdtr->Base  = (UINTN) GdtForAP;
  Gdtr->Limit = GdtrForBSP.Limit;

  Idtr->Base  = (UINTN) IdtForAP;
  Idtr->Limit = IdtrForBSP.Limit;

  return EFI_SUCCESS;
}


/**
  Initialize CPU Data Hob

  @retval EFI_SUCCESS           The driver installes/initialized correctly.
  @retval EFI_OUT_OF_RESOURCES  Allocation of the hob failed.

**/
EFI_STATUS
InitializeCpuDataHob (
  VOID
  )
{
  CPU_INIT_DATA_HOB      *CpuInitDataHob;
  CPU_CONFIG             *CpuConfig;
  POWER_MGMT_CONFIG      *PowerMgmtConfig;
  VOID                   *Hob;

  PostCode (0xC3B);

  //
  // Initial cpu data into one hob, it will be used by MP CPU DXE.
  //
  CpuInitDataHob = AllocateRuntimeZeroPool (sizeof (CPU_INIT_DATA_HOB));
  if (CpuInitDataHob == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CpuConfig                            = &mMpCpuRuntimeData->CpuConfig;
  PowerMgmtConfig                      = &mMpCpuRuntimeData->PowerMgmtConfig;

  CopyMem (
    (VOID *) (UINTN) PowerMgmtConfig,
    (VOID *) (UINTN) mPowerMgmtConfig,
    sizeof (POWER_MGMT_CONFIG)
    );
  CopyMem (
    (VOID *) (UINTN) CpuConfig,
    (VOID *) (UINTN) mCpuConfig,
    sizeof (CPU_CONFIG)
    );

  CpuInitDataHob->CpuConfig          = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->CpuConfig;
  CpuInitDataHob->PowerMgmtConfig    = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->PowerMgmtConfig;

  CpuInitDataHob->MpData             = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->AcpiCpuData;
  CpuInitDataHob->FvidTable          = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->FvidTable;

  Hob = BuildGuidDataHob (
          &gCpuInitDataHobGuid,
          (VOID *) CpuInitDataHob,
          (UINTN) sizeof (CPU_INIT_DATA_HOB)
          );
  ASSERT (Hob != NULL);

  PostCode (0xC3E);

  return EFI_SUCCESS;
}


/**
  Initialize multi-processor service.

**/
EFI_STATUS
InitializeMpServices (
  IN  SI_CPU_POLICY_PPI     *SiCpuPolicyPpi
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  EFI_BOOT_MODE         BootMode;
  UINT64                *MtrrValues;
  MP_CPU_EXCHANGE_INFO  *ExchangeInfo;
  VOID                  *StackAddressStart;
  EFI_PHYSICAL_ADDRESS  WakeUpBuffer;
  MTRR_SETTINGS         MtrrSetting;
  UINT16                MaxEnabledThreadsPerCore;
  UINT16                MaxEnabledCoresPerDie;
  UINT16                MaxDiesPerPackage;
  UINT16                MaxPackages;
  UINTN                 MaximumCPUsForThisSystem;
  UINT32                ResponseProcessorCount;

  Status = PeiServicesGetBootMode (&BootMode);
  DEBUG ((DEBUG_INFO, "InitializeMpServices: BootMode = %X\n", BootMode));
  if ((Status == EFI_SUCCESS) && (BootMode == BOOT_ON_S3_RESUME)) {
    return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "Set Cpu MP Service Environment entry point\n"));
  PostCode (0xC20);

  //
  // Allocate MP data structure memory.
  //
  mMpCpuRuntimeData = (MP_CPU_RUNTIME_DATA *) AllocateRuntimeZeroPool (sizeof (MP_CPU_RUNTIME_DATA));
  if (mMpCpuRuntimeData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize ACPI_CPU_DATA data
  //
  mMpCpuRuntimeData->AcpiCpuData.GdtrProfile            = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->GdtrProfile;
  mMpCpuRuntimeData->AcpiCpuData.IdtrProfile            = (EFI_PHYSICAL_ADDRESS) (UINTN) &mMpCpuRuntimeData->IdtrProfile;
  mMpCpuRuntimeData->AcpiCpuData.S3BootPath             = FALSE;

  mSiCpuPolicyPpi     = SiCpuPolicyPpi;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gCpuConfigGuid , (VOID *)&mCpuConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gPowerMgmtConfigGuid , (VOID *)&mPowerMgmtConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Save the MTRR registers to global variables
  //
  MtrrValues = mMpCpuRuntimeData->MtrrValues;
  ReadMtrrRegisters (MtrrValues);

  //
  // Get information on enabled threads, cores, dies and package for the CPU(s) on this platform
  //
  GetEnabledCount (
    &MaxEnabledThreadsPerCore,
    &MaxEnabledCoresPerDie,
    &MaxDiesPerPackage,
    &MaxPackages
    );

  //
  // Get the total CPU count
  //
  MaximumCPUsForThisSystem = MaxEnabledThreadsPerCore * MaxEnabledCoresPerDie * MaxDiesPerPackage * MaxPackages;

  //
  // Prepare Wakeup Buffer and Stack for APs
  //
  Status = PrepareMemoryForAPs (
             &WakeUpBuffer,
             &StackAddressStart,
             MaximumCPUsForThisSystem
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Specify memory cacheable type as Write Back Start\n"));

  //
  // Set specify memory cacheable type as Write Back
  //
  Status = MtrrSetMemoryAttributeInMtrrSettings (
             &MtrrSetting,
             0x50000,
             0x10000,
             CacheWriteBack
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Fill MP Data
  //
  Status = FillMpData (
             (UINTN) WakeUpBuffer,
             StackAddressStart,
             MaximumCPUsForThisSystem
             );

  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);

  //
  // Wake up all APs at the first time
  //
  WakeUpAPs (MtrrValues, (EFI_AP_PROCEDURE) InitialMpProcedure);

  //
  // Program XApic register
  //
  ProgramXApic (TRUE);

  //
  // Wait for all APs to complete
  //
  while (ExchangeInfo->FinishedCount < mMpCpuRuntimeData->AcpiCpuData.NumberOfCpus - 1) {
    CpuPause ();
  }

  //
  // Collect all APs BIST status
  //
  for (Index = 1,  ResponseProcessorCount = 1; Index < MAXIMUM_CPU_NUMBER; Index++) {
    if (ExchangeInfo->BistBuffer[Index].Number == 1) {
      ExchangeInfo->BistBuffer[Index].Number = ResponseProcessorCount++;
    }
  }

  //
  // Switch BSP to Lowest Feature Processor (LFP)
  //
  Status = SwitchToLowestFeatureProcess ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register notification for CPU MP service task at the End of PEI
  //
  Status = PeiServicesNotifyPpi (&mCpuMpServiceNotifyDesc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Cpu MP Service End\n"));
  PostCode (0xC9F);

  return EFI_SUCCESS;
}


/**
  Get general MP information

  @param[in] NumberOfCpus            Number of processors
  @param[in] MaximumNumberOfCpus     Max supported number of processors
  @param[in] NumberOfEnabledCpus     Number of processors enabled
  @param[in] RendezvousIntNumber     Number of Rendezvous procedure
  @param[in] RendezvousProcLength    Length of Rendezvous procedure

  @retval    EFI_SUCCESS             Always return success

**/
EFI_STATUS
EFIAPI
GetGeneralMpInfo (
  OUT UINTN                   *NumberOfCpus,
  OUT UINTN                   *MaximumNumberOfCpus,
  OUT UINTN                   *NumberOfEnabledCpus,
  OUT UINTN                   *RendezvousIntNumber,
  OUT UINTN                   *RendezvousProcLength
  )
{
  UINTN               Index;
  CPU_DATA_BLOCK      *CpuData;

  if (NumberOfCpus) {
    *NumberOfCpus = mMpSystemData->NumberOfCpus;
  }

  if (MaximumNumberOfCpus) {
    *MaximumNumberOfCpus = mMpSystemData->MaximumCpusForThisSystem;
  }

  if (RendezvousProcLength) {
    *RendezvousProcLength = RENDEZVOUS_PROC_LENGTH;
  }

  if (RendezvousIntNumber) {
    *RendezvousIntNumber = 0;
  }

  if (NumberOfEnabledCpus) {
    *NumberOfEnabledCpus = mMpSystemData->NumberOfCpus;
    for (Index = 0; Index < mMpSystemData->NumberOfCpus; Index++) {
      CpuData = &mMpSystemData->CpuData[Index];
      if (mMpSystemData->EnableSecondaryCpu) {
        if (CpuData->State != CPU_STATE_DISABLED) {
          (*NumberOfEnabledCpus)++;
        }
      } else {
        if (CpuData->State != CPU_STATE_DISABLED && !mMpSystemData->CpuData[Index].SecondaryCpu) {
          (*NumberOfEnabledCpus)++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Get processor context

  @param[in] CpuNumber                Cpu number
  @param[in] BufferLength             Buffer length
  @param[in] ProcessorContextBuffer   Pointer to the buffer that will be updated

  @retval    EFI_INVALID_PARAMETER    Buffer is NULL or CpuNumber our of range
  @retval    EFI_BUFFER_TOO_SMALL     Buffer too small
  @retval    EFI_SUCCESS              Got processor context successfully

**/
EFI_STATUS
EFIAPI
GetProcessorContext (
  IN UINTN                    CpuNumber,
  IN OUT UINTN                *BufferLength,
  IN OUT EFI_MP_PROC_CONTEXT  *ProcessorContextBuffer
  )
{
  EFI_MP_PROC_CONTEXT  *ProcessorBuffer;
  CPU_DATA_BLOCK       *CpuData;

  if (BufferLength == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferLength < sizeof (EFI_MP_PROC_CONTEXT)) {
    *BufferLength = sizeof (EFI_MP_PROC_CONTEXT);
    return EFI_BUFFER_TOO_SMALL;
  }

  if ((mMpSystemData->NumberOfCpus <= CpuNumber) || (ProcessorContextBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData                   = &mMpSystemData->CpuData[CpuNumber];

  *BufferLength             = sizeof (EFI_MP_PROC_CONTEXT);
  ProcessorBuffer           = ProcessorContextBuffer;

  ProcessorBuffer->ApicID   = CpuData->ApicID;

  ProcessorBuffer->Enabled  = TRUE;
  if (!mMpSystemData->EnableSecondaryCpu) {
    if (CpuData->SecondaryCpu) {
      ProcessorBuffer->Enabled = FALSE;
    }
  }

  if (CpuData->State == CPU_STATE_DISABLED) {
    ProcessorBuffer->Enabled = FALSE;
  }

  if (CpuNumber == mMpSystemData->BSP) {
    ProcessorBuffer->Designation = EfiCpuBSP;
  } else {
    ProcessorBuffer->Designation = EfiCpuAP;
  }

  ProcessorBuffer->Health.Flags       = CpuData->Health;
  ProcessorBuffer->Health.TestStatus  = 0;

  ProcessorBuffer->PackageNumber      = CpuData->PhysicalLocation.Package;
  ProcessorBuffer->NumberOfCores      = CpuData->PhysicalLocation.Core;
  ProcessorBuffer->NumberOfThreads    = CpuData->PhysicalLocation.Thread;

  ProcessorBuffer->ProcessorTestMask  = 0;

  return EFI_SUCCESS;
}


/**
  MP Service to get specified application processor (AP)
  to execute a caller-provided code stream.

  @param[in] Function                The procedure to be assigned to AP.
  @param[in] CpuNumber               Number of the specified processor.
  @param[in] ProcArguments           Argument for Procedure.

  @retval    EFI_INVALID_PARAMETER   Procudure is NULL.
  @retval    EFI_INVALID_PARAMETER   Number of CPU out of range, or it belongs to BSP.
  @retval    EFI_INVALID_PARAMETER   Specified CPU is not idle.
  @retval    EFI_SUCCESS             The AP has finished.
  @retval    EFI_TIMEOUT             Time goes out before the AP has finished.

**/
EFI_STATUS
EFIAPI
StartupThisAP (
  IN EFI_AP_PROCEDURE         Function,
  IN UINTN                    CpuNumber,
  IN OUT VOID *ProcArguments  OPTIONAL
  )
{
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  UINT64                 *MtrrValues;

  WakeUpBuffer = mMpCpuRuntimeData->AcpiCpuData.WakeUpBuffer;
  MtrrValues = mMpCpuRuntimeData->MtrrValues;
  ReadMtrrRegisters (MtrrValues);
  ExchangeInfo                        = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  ExchangeInfo->Lock                  = 0;
  ExchangeInfo->ApFunction            = (UINT32) Function;
  ExchangeInfo->ApArgument            = (UINT32) ProcArguments;
  ExchangeInfo->MtrrValuesAddress     = (UINT32) MtrrValues;
  ExchangeInfo->FinishedCount         = (UINT32) 0;
  ExchangeInfo->SerializeLock         = (UINT32) 0;
  ExchangeInfo->StartState            = (UINT32) 0;
  ExchangeInfo->StackStart            = (UINTN) ((ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress))->StackAddress;

  //
  // Send INIT IPI - SIPI to the AP
  //
  SendInterrupt (
    BROADCAST_MODE_SPECIFY_CPU,
    CpuNumber,
    0,
    DELIVERY_MODE_INIT,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  SendInterrupt (
    BROADCAST_MODE_SPECIFY_CPU,
    CpuNumber,
    (UINT32) RShiftU64 (WakeUpBuffer, 12),
    DELIVERY_MODE_SIPI,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  //
  // Wait for the AP to complete
  //
  while (ExchangeInfo->FinishedCount != 1) {
    CpuPause ();
  }

  return EFI_SUCCESS;
}


/**
  Wake up all the application processors

  @param[in] Function         The procedure to be assigned to AP.
  @param[in] ProcArguments    Argument for Procedure.

  @retval    EFI_SUCCESS      APs are successfully waked up

**/
EFI_STATUS
StartupAllAps (
  IN EFI_AP_PROCEDURE         Function,
  IN OUT VOID *ProcArguments  OPTIONAL
  )
{
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  UINT64                 *MtrrValues;
  UINTN                  NumberOfCpus;

  WakeUpBuffer = mMpCpuRuntimeData->AcpiCpuData.WakeUpBuffer;
  MtrrValues = mMpCpuRuntimeData->MtrrValues;
  ReadMtrrRegisters (MtrrValues);
  ExchangeInfo                        = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  ExchangeInfo->Lock                  = 0;
  ExchangeInfo->ApFunction            = (UINT32) Function;
  ExchangeInfo->ApArgument            = (UINT32) ProcArguments;
  ExchangeInfo->MtrrValuesAddress     = (UINT32) MtrrValues;
  ExchangeInfo->FinishedCount         = (UINT32) 0;
  ExchangeInfo->SerializeLock         = (UINT32) 0;
  ExchangeInfo->StackStart            = (UINTN) ((ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress))->StackAddress;
  NumberOfCpus = ((ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress))->NumberOfCpus;

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
  MicroSecondDelay (10 * STALL_ONE_MILLI_SECOND); ///< 10ms

  SendInterrupt (
    BROADCAST_MODE_ALL_EXCLUDING_SELF,
    0,
    (UINT32) RShiftU64 (WakeUpBuffer, 12),
    DELIVERY_MODE_SIPI,
    TRIGGER_MODE_EDGE,
    TRUE
    );
  MicroSecondDelay (200 * STALL_ONE_MICRO_SECOND); ///< 200us

  SendInterrupt (
    BROADCAST_MODE_ALL_EXCLUDING_SELF,
    0,
    (UINT32) RShiftU64 (WakeUpBuffer, 12),
    DELIVERY_MODE_SIPI,
    TRIGGER_MODE_EDGE,
    TRUE
    );

  //
  // Wait for all APs to complete
  //
  DEBUG ((DEBUG_INFO, "FinishedCount = %x, NumberOfCpus = %x\n", ExchangeInfo->FinishedCount, NumberOfCpus));
  while (ExchangeInfo->FinishedCount < NumberOfCpus - 1) {
    CpuPause ();
  }

  return EFI_SUCCESS;
}


/**
    MP Service to makes the current BSP into an AP and then switches the
    designated AP into the AP. This procedure is usually called after a CPU
    test that has found that BSP is not healthy to continue it's responsbilities.

    @param[in] CpuNumber               The number of the specified AP.
    @param[in] EnableOldBsp            Whether to enable or disable the original BSP.

    @retval    EFI_INVALID_PARAMETER   Number for Specified AP out of range.
    @retval    EFI_INVALID_PARAMETER   Number of specified CPU belongs to BSP.
    @retval    EFI_NOT_READY           Specified AP is not idle.
    @retval    EFI_SUCCESS             BSP successfully switched.

**/
EFI_STATUS
EFIAPI
SwitchBsp (
  IN UINTN                    CpuNumber,
  IN BOOLEAN                  EnableOldBsp
  )
{

  CPU_DATA_BLOCK        *CpuData;
  CPU_STATE             CpuState;
  UINT64                *MtrrValues;

  MtrrValues = mMpCpuRuntimeData->MtrrValues;
  ReadMtrrRegisters (MtrrValues);

  //
  // Check if the specified CPU number is valid
  //
  if (CpuNumber >= mMpSystemData->NumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if the specified CPU is already BSP
  //
  if (CpuNumber == mMpSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMpSystemData->CpuData[CpuNumber];
  if (CpuData->State != CPU_STATE_IDLE) {
    return EFI_NOT_READY;
  }

  //
  // Before send both BSP and AP to a procedure to exchange their roles,
  // interrupt must be disabled. This is because during the exchange role
  // process, 2 CPU may use 1 stack. If interrupt happens, the stack will
  // be corrputed, since interrupt return address will be pushed to stack
  // by hardware.
  //
  DisableInterrupts ();

  //
  // Unprogram virtual wire mode for the old BSP
  //
  ProgramXApic (FALSE);
  SetApicBspBit (FALSE);

  mMpSystemData->BspInfo.State  = CPU_SWITCH_STATE_IDLE;
  mMpSystemData->BspInfo.Lock   = VacantFlag;
  mMpSystemData->ApInfo.State   = CPU_SWITCH_STATE_IDLE;
  mMpSystemData->ApInfo.Lock    = VacantFlag;

  //
  // Need to wakeUp AP (future BSP)
  //
  WakeUpAPs (MtrrValues, (EFI_AP_PROCEDURE) FutureBspProc);

  AsmExchangeRole (&mMpSystemData->BspInfo, &mMpSystemData->ApInfo);

  //
  // The new BSP has come out. Since it carries the register value of the AP, need
  // to pay attention to variable which are stored in registers (due to optimization)
  //
  SetApicBspBit (TRUE);
  ProgramXApic (TRUE);

  EnableInterrupts ();

  CpuData = &mMpSystemData->CpuData[mMpSystemData->BSP];
  while (TRUE) {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuState = CpuData->State;
    AsmReleaseMPLock (&CpuData->StateLock);

    if (CpuState == CPU_STATE_FINISHED) {
      break;
    }
  }

  ChangeCpuState (mMpSystemData->BSP, EnableOldBsp, CPU_CAUSE_NOT_DISABLED);
  mMpSystemData->BSP  = CpuNumber;

  return EFI_SUCCESS;
}


/**
  This procedure enables Or disables APs.

  @param[in] CpuNumber               The number of the specified AP.
  @param[in] NewApState              Indicate new desired AP state
  @param[in] HealthState             If not NULL, it points to the value that specifies
                                     the new health status of the AP.  If it is NULL,
                                     this parameter is ignored.

  @retval    EFI_INVALID_PARAMETER   Input paramters were not correct.
  @retval    EFI_SUCCESS             Function completed successfully.

**/
EFI_STATUS
EFIAPI
EnableDisableAp (
  IN UINTN                      CpuNumber,
  IN BOOLEAN                    NewApState,
  IN EFI_MP_HEALTH *HealthState OPTIONAL
  )
{
  CPU_DATA_BLOCK  *CpuData;

  //
  // Check for valid input parameters.
  //
  if (CpuNumber >= mMpSystemData->NumberOfCpus || CpuNumber == mMpSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMpSystemData->CpuData[CpuNumber];

  if (HealthState != NULL) {
    CopyMem (&CpuData->Health, HealthState, sizeof (EFI_MP_HEALTH));
  }

  return EFI_SUCCESS;
}


/**
  This procedure returns the calling CPU handle.

  @dot
    digraph G {
      subgraph cluster_c0 {
        node [shape = box];
          b1[label="GetApicID ()" fontsize=12 style=filled color=lightblue];
          b2[label="Index + 1" fontsize=12 style=filled color=lightblue];
          b3[label="*CpuNumber = Index" fontsize=12 style=filled color=lightblue];

        node [shape = ellipse];
          e1[label="Start" fontsize=12 style=filled color=lightblue];
          e2[label="End" fontsize=12 style=filled color=lightblue];

        node [shape = diamond,style=filled,color=lightblue];
          d1[label="Index < NumOfCpus" fontsize=12];
          d2[label="Is ApicID equal to\n mMpSystemData->CpuData[Index].ApicID" fontsize=12];

        label = "WhoAmI Flow"; fontsize=15; fontcolor=black; color=lightblue;
        e1 -> b1
        b1 -> d1
        d1 -> d2 [label="Yes" fontsize=9]
        d1 -> b3 [label="No" fontsize=9]
        d2 -> b3 [label="Yes" fontsize=9]
        d2 -> b2 [label="No" fontsize=9]
        b2 -> d1
        b3 -> e2

      }
    }
  @enddot

  @param[out] CpuNumber    The number of the specified AP.

  @retval     EFI_SUCCESS  Function completed successfully.

**/
EFI_STATUS
EFIAPI
WhoAmI (
  OUT UINTN      *CpuNumber
  )
{
  UINTN          ApicID;
  UINTN          NumOfCpus;
  UINTN          Index;

  ApicID    = GetCpuApicId ();
  NumOfCpus = mMpSystemData->NumberOfCpus;

  for (Index = 0; Index < NumOfCpus; Index++) {
    if (ApicID == mMpSystemData->CpuData[Index].ApicID) {
      break;
    }
  }
  *CpuNumber = Index;

  return EFI_SUCCESS;
}


/**
  This function handles CPU MP service task at the end of PEI

  @param[in] PeiServices     Pointer to PEI Services Table.
  @param[in] NotifyDesc      Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi             Pointer to the PPI data associated with this function.

  @retval    EFI_STATUS      Always return EFI_SUCCESS

**/
STATIC
EFI_STATUS
CpuMpServiceAtEndOfPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  EFI_STATUS  Status;

  //
  // Send INIT IPI - to all APs
  //
  Status = SendInterrupt (
             BROADCAST_MODE_ALL_EXCLUDING_SELF,
             0,
             0,
             DELIVERY_MODE_INIT,
             TRIGGER_MODE_EDGE,
             TRUE
             );

  return Status;
}


/**
  AP initialization

**/
VOID
InitialMpProcedure (
  VOID
  )
{
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;
  UINT64                 *MtrrValues;
  EFI_CPUID_REGISTER     CpuidRegisters;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );

  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mMpCpuRuntimeData->AcpiCpuData.WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);

  //
  // Init XMM
  //
  XmmInit ();

  MtrrValues =   (UINT64 *)ExchangeInfo->MtrrValuesAddress;
  ProgramXApic (FALSE);

  MpMtrrSynchUp (MtrrValues);
}


/**
  Get CPU platform features settings to fill MP data.

  @param[in] WakeUpBuffer               The address of wakeup buffer.
  @param[in] StackAddressStart          The start address of APs's stacks.
  @param[in] MaximumCPUsForThisSystem   Maximum CPUs in this system.

  @retval    EFI_SUCCESS                Function successfully executed.
  @retval    Other                      Error occurred while allocating memory.

**/
EFI_STATUS
EFIAPI
FillMpData (
  IN UINTN                WakeUpBuffer,
  IN VOID                 *StackAddressStart,
  IN UINTN                MaximumCPUsForThisSystem
  )
{

  BOOLEAN        HyperThreadingEnabled;

  mMpSystemData = &mMpCpuRuntimeData->MpSystemData;

  //
  // First check if the MP data structures and AP rendezvous routine have been
  // supplied by the PEIMs that executed in early boot stage.
  //
  //
  // Clear the data structure area first.
  //
  ZeroMem (mMpSystemData, sizeof (MP_SYSTEM_DATA));
  HyperThreadingEnabled = FALSE;

  mMpCpuRuntimeData->AcpiCpuData.CpuPrivateData  = (EFI_PHYSICAL_ADDRESS) (UINTN) (&mMpSystemData->S3DataPointer);
  mMpCpuRuntimeData->AcpiCpuData.WakeUpBuffer    = WakeUpBuffer;
  mMpCpuRuntimeData->AcpiCpuData.NumberOfCpus    = MaximumCPUsForThisSystem;
  mMpCpuRuntimeData->AcpiCpuData.APState         = HyperThreadingEnabled;
  mMpCpuRuntimeData->AcpiCpuData.StackAddress    = (EFI_PHYSICAL_ADDRESS) (UINTN) StackAddressStart;

  PrepareGdtIdtForAP (
    (IA32_DESCRIPTOR *) (UINTN) mMpCpuRuntimeData->AcpiCpuData.GdtrProfile,
    (IA32_DESCRIPTOR *) (UINTN) mMpCpuRuntimeData->AcpiCpuData.IdtrProfile
    );

  //
  // First BSP fills and inits all known values, including it's own records.
  //
  mMpSystemData->ApSerializeLock    = VacantFlag;
  mMpSystemData->NumberOfCpus       = MaximumCPUsForThisSystem;
  mMpSystemData->EnableSecondaryCpu = HyperThreadingEnabled;

  mMpSystemData->VmxEnable                        = (BOOLEAN) mCpuConfig->VmxEnable;
  mMpSystemData->TxtEnable                        = (BOOLEAN) mCpuConfig->SmxEnable;
  mMpSystemData->MonitorMwaitEnable               = (BOOLEAN) mCpuConfig->MonitorMwaitEnable;
  mMpSystemData->MachineCheckEnable               = (BOOLEAN) mCpuConfig->MachineCheckEnable;
  mMpSystemData->AesEnable                        = (BOOLEAN) mCpuConfig->AesEnable;
  mMpSystemData->DebugInterfaceEnable             = (BOOLEAN) mCpuConfig->DebugInterfaceEnable;
  mMpSystemData->DebugInterfaceLockEnable         = (BOOLEAN) mCpuConfig->DebugInterfaceLockEnable;

  mMpSystemData->ProcTraceMemSize                 =  mCpuConfig->ProcTraceMemSize;
  mMpSystemData->ProcTraceEnable                  = (BOOLEAN) mCpuConfig->ProcTraceEnable;
  mMpSystemData->ProcTraceOutputScheme            = (UINT8)   mCpuConfig->ProcTraceOutputScheme;
  mMpSystemData->HyperThreadingEnable             = (BOOLEAN) mCpuConfig->HyperThreading;

  mMpSystemData->S3DataPointer.S3BootScriptTable  = (UINT32) (UINTN) mMpSystemData->S3BootScriptTable;
  mMpSystemData->S3DataPointer.S3BspMtrrTable     = (UINT32) (UINTN) mMpSystemData->S3BspMtrrTable;

  mMpSystemData->MaximumCpusForThisSystem = MaximumCPUsForThisSystem;

  mMpSystemData->BSP                      = 0;

  //
  // Collect CPU_DATA_BLOCK for BSP. All APs call ones, too.
  //
  FillInProcessorInformation (TRUE, 0);

  return EFI_SUCCESS;
}


/**
  Get CPU platform features settings to fill MP data.

  @retval   MP_SYSTEM_DATA*         Return MpSystemData pointer.

**/
MP_SYSTEM_DATA *
EFIAPI
GetMpSystemData (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS   WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO   *ExchangeInfo;

  WakeUpBuffer = 0x58000;
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);

  return (MP_SYSTEM_DATA *) (VOID *) ExchangeInfo->MpSystemDataAddress;
}


/**
  This function is called by all processors (both BSP and AP) once and collects MP related data.

  @param[in] BSP             TRUE if the CPU is BSP
  @param[in] BistParam       BIST (build-in self test) data for the processor. This data
                             is only valid for processors that are waked up for the 1ast
                             time in this CPU DXE driver.

  @retval    EFI_SUCCESS     Data for the processor collected and filled in.

**/
EFI_STATUS
FillInProcessorInformation (
  IN BOOLEAN           BSP,
  IN UINT32            BistParam
  )
{
  UINT32                Health;
  UINT32                ApicID;
  CPU_DATA_BLOCK        *CpuData;
  UINT32                BIST;
  UINTN                 CpuNumber;
  ACPI_CPU_DATA         *AcpiCpuData;
  MP_CPU_EXCHANGE_INFO  *ExchangeInfo;

  AcpiCpuData = &mMpCpuRuntimeData->AcpiCpuData;
  ApicID  = GetCpuApicId ();
  BIST    = 0;

  if (BSP) {
    CpuNumber = 0;
    BIST      = BistParam;
  } else {
    ExchangeInfo  = (MP_CPU_EXCHANGE_INFO *) (UINTN) (AcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
    CpuNumber     = ExchangeInfo->BistBuffer[ApicID].Number;
    BIST          = ExchangeInfo->BistBuffer[ApicID].BIST;
  }

  CpuData                 = &mMpSystemData->CpuData[CpuNumber];
  CpuData->SecondaryCpu   = IsSecondaryThread ();
  CpuData->ApicID         = ApicID;
  CpuData->Procedure      = NULL;
  CpuData->Parameter      = NULL;
  CpuData->StateLock      = VacantFlag;
  CpuData->ProcedureLock  = VacantFlag;
  CpuData->State          = CPU_STATE_IDLE;

  Health                  = BIST;
  if (Health > 0) {
    CpuData->State                        = CPU_STATE_DISABLED;
    mMpSystemData->DisableCause[CpuNumber] = CPU_CAUSE_SELFTEST_FAILURE;
  } else {
    mMpSystemData->DisableCause[CpuNumber] = CPU_CAUSE_NOT_DISABLED;
  }

  return EFI_SUCCESS;
}


/**
  Wake up all the application processors

  @param[in] MtrrValues      Pointer to a buffer which stored MTRR settings
  @param[in] Function        Pointer to AP Procedure Function

  @retval    EFI_SUCCESS     APs are successfully waked up

**/
EFI_STATUS
WakeUpAPs (
  UINT64               *MtrrValues,
  EFI_AP_PROCEDURE     Function
  )
{
  EFI_PHYSICAL_ADDRESS    WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO    *ExchangeInfo;
  MP_CPU_S3_DATA_POINTER  *CpuS3DataPtr;
  ACPI_CPU_DATA           *AcpiCpuData;

  AcpiCpuData = &mMpCpuRuntimeData->AcpiCpuData;
  WakeUpBuffer = AcpiCpuData->WakeUpBuffer;
  CopyMem (
    (VOID *) (UINTN) WakeUpBuffer,
    AsmGetAddressMap (),
    MP_CPU_EXCHANGE_INFO_OFFSET
  );
  AsmPatchRendezvousCode ((VOID *) (UINTN) WakeUpBuffer);

  ExchangeInfo                        = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer +  MP_CPU_EXCHANGE_INFO_OFFSET);

  ExchangeInfo->Lock                  = 0;
  ExchangeInfo->StackStart            = (UINTN) AcpiCpuData->StackAddress;
  ExchangeInfo->StackSize             = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction            = (UINT32) Function;
  ExchangeInfo->ApArgument            = 0;
  ExchangeInfo->BufferStart           = (UINT32) WakeUpBuffer;
  ExchangeInfo->PmodeOffset           = (UINT32) (AsmGetPmodeOffset ());
  ExchangeInfo->SemaphoreCheck        = (VOID (*)(UINT32 *)) (AsmGetSemaphoreCheckOffset () + (UINT32) WakeUpBuffer);
  ExchangeInfo->AcpiCpuDataAddress    = (UINT32) AcpiCpuData;
  ExchangeInfo->MtrrValuesAddress     = (UINT32) MtrrValues;
  ExchangeInfo->FinishedCount         = (UINT32) 0;
  ExchangeInfo->SerializeLock         = (UINT32) 0;
  ExchangeInfo->StartState            = (UINT32) 0;

  CpuS3DataPtr                        = (MP_CPU_S3_DATA_POINTER *) (UINTN) AcpiCpuData->CpuPrivateData;
  ExchangeInfo->S3BootScriptTable     = (MP_CPU_S3_SCRIPT_DATA *) (UINTN) CpuS3DataPtr->S3BootScriptTable;
  ExchangeInfo->VirtualWireMode       = CpuS3DataPtr->VirtualWireMode;
  ExchangeInfo->CpuPerfCtrlValue      = AsmReadMsr64 (MSR_IA32_PERF_CTRL);
  ExchangeInfo->McuLoadCount          = 0;
  ExchangeInfo->MpSystemDataAddress   = (UINT32) &mMpCpuRuntimeData->MpSystemData;
  ExchangeInfo->MpServicePpiAddress   = 0;
  ExchangeInfo->SiCpuPolicyPpi        = mSiCpuPolicyPpi;

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

  return EFI_SUCCESS;
}


/**
  Send interrupt to CPU

  @param[in] BroadcastMode            Interrupt broadcast mode
  @param[in] ApicID                   APIC ID for sending interrupt
  @param[in] VectorNumber             Vector number
  @param[in] DeliveryMode             Interrupt delivery mode
  @param[in] TriggerMode              Interrupt trigger mode
  @param[in] Assert                   Interrupt pin polarity

  @retval    EFI_INVALID_PARAMETER    Input parameter not correct
  @retval    EFI_NOT_READY            There was a pending interrupt
  @retval    EFI_SUCCESS              Interrupt sent successfully

**/
EFI_STATUS
SendInterrupt (
  IN UINT32            BroadcastMode,
  IN UINT32            ApicID,
  IN UINT32            VectorNumber,
  IN UINT32            DeliveryMode,
  IN UINT32            TriggerMode,
  IN BOOLEAN           Assert
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  UINT32                IcrLow;
  UINT32                IcrHigh;

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

  switch (BroadcastMode) {
    case BROADCAST_MODE_SPECIFY_CPU:
      IcrHigh = ApicID << 24;
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
  ApicBase    = ApicBaseReg & 0xffffff000ULL;

  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_HIGH_OFFSET) = IcrHigh;
  *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET)  = IcrLow;

  MicroSecondDelay (10 * STALL_ONE_MICRO_SECOND);

  IcrLow = *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);
  if (IcrLow & 0x1000) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}


/**
  Switch BSP to the processor which has least features

  @retval   EFI_STATUS     Status code returned from each sub-routines

**/
EFI_STATUS
EFIAPI
SwitchToLowestFeatureProcess (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               CurrentProcessor;

  Status = WhoAmI (&CurrentProcessor);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_BSP_SELECT
    );

  //
  // Take current BSP as the least feature
  //
  UpdateProcessorInfo (CurrentProcessor, &mMpSystemData->LeastFeatureProcessor);

  return EFI_SUCCESS;
}


/**
  Get processor feature

  @param[in]    Features           Pointer to a buffer which stores feature information

**/
VOID
EFIAPI
GetProcessorFeatures (
  IN UINT32 *Features
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  Features[0] = CpuidRegisters.RegEcx;
  Features[1] = CpuidRegisters.RegEdx;
  AsmCpuid (
    CPUID_EXTENDED_CPU_SIG,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  Features[2] = CpuidRegisters.RegEax;
  Features[3] = CpuidRegisters.RegEbx;
  Features[4] = CpuidRegisters.RegEcx;
  Features[5] = CpuidRegisters.RegEdx;

  return;
}


/**
  Find out the common features supported by all core/threads

**/
VOID
EFIAPI
GetProcessorCommonFeature (
  VOID
  )
{
  UINTN               Index;
  UINT32              Features[MAX_FEATURE_NUM];

  GetProcessorFeatures (Features);
  AsmAcquireMPLock (&mMpSystemData->Lock);

  for (Index = 0; Index < MAX_FEATURE_NUM; Index++) {
    mMpSystemData->LeastFeatureProcessor.Features[Index] &= Features[Index];
  }
  AsmReleaseMPLock (&mMpSystemData->Lock);
}


/**
  Get the processor data with least features

**/
VOID
EFIAPI
GetProcessorWithLeastFeature (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               CurrentProcessor;
  LEAST_FEATURE_PROC  LeastFeatureProcessor;

  Status = WhoAmI (&CurrentProcessor);
  ASSERT_EFI_ERROR (Status);

  GetProcessorFeatures (mMpSystemData->LeastFeatureProcessor.Features);
  LeastFeatureProcessor.FeatureDelta = GetProcessorFeatureDelta (
                                         LeastFeatureProcessor.Features,
                                         mMpSystemData->LeastFeatureProcessor.Features
                                         );

  AsmAcquireMPLock (&mMpSystemData->Lock);
  if (LeastFeatureProcessor.FeatureDelta < mMpSystemData->LeastFeatureProcessor.FeatureDelta) {
    mMpSystemData->LeastFeatureProcessor.FeatureDelta = LeastFeatureProcessor.FeatureDelta;
    UpdateProcessorInfo (CurrentProcessor, &mMpSystemData->LeastFeatureProcessor);
  } else if (LeastFeatureProcessor.FeatureDelta == mMpSystemData->LeastFeatureProcessor.FeatureDelta) {
    UpdateProcessorInfo (CurrentProcessor, &LeastFeatureProcessor);
    if (LeastFeatureProcessor.Version < mMpSystemData->LeastFeatureProcessor.Version) {
      UpdateProcessorInfo (CurrentProcessor, &mMpSystemData->LeastFeatureProcessor);
    } else if (LeastFeatureProcessor.Version == mMpSystemData->LeastFeatureProcessor.Version) {
      if (LeastFeatureProcessor.ApicId < mMpSystemData->LeastFeatureProcessor.ApicId) {
        UpdateProcessorInfo (CurrentProcessor, &mMpSystemData->LeastFeatureProcessor);
      }
    }
  }

  AsmReleaseMPLock (&mMpSystemData->Lock);
}


/**
  Extract CPU detail version infomation

  @param[in] FamilyId     FamilyId, including ExtendedFamilyId
  @param[in] Model        Model, including ExtendedModel
  @param[in] SteppingId   SteppingId
  @param[in] Processor    Processor

**/
VOID
EFIAPI
EfiCpuVersion (
  IN  OUT UINT16  *FamilyId,    OPTIONAL
  IN  OUT UINT8   *Model,       OPTIONAL
  IN  OUT UINT8   *SteppingId,  OPTIONAL
  IN  OUT UINT8   *Processor    OPTIONAL
  )
{
  EFI_CPUID_REGISTER  Register;
  UINT8               TempFamilyId;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &Register.RegEax,
    &Register.RegEbx,
    &Register.RegEcx,
    &Register.RegEdx
    );

  if (SteppingId != NULL) {
    *SteppingId = (UINT8) (Register.RegEax & 0xF);
  }

  if (Processor != NULL) {
    *Processor = (UINT8) ((Register.RegEax >> 12) & 0x3);
  }

  if (Model != NULL || FamilyId != NULL) {
    TempFamilyId = (UINT8) ((Register.RegEax >> 8) & 0xF);

    if (Model != NULL) {
      *Model = (UINT8) ((Register.RegEax >> 4) & 0xF);
      if (TempFamilyId == 0x6 || TempFamilyId == 0xF) {
        *Model |= (Register.RegEax >> 12) & 0xF0;
      }
    }

    if (FamilyId != NULL) {
      *FamilyId = TempFamilyId;
      if (TempFamilyId == 0xF) {
        *FamilyId = *FamilyId + (UINT16) ((Register.RegEax >> 20) & 0xFF);
      }
    }
  }
}


/**
  Update some processor info into LEAST_FEATURE_PROC data structure.

  @param[in] Index                    Indicate which processor calling this routine
  @param[in] LeastFeatureProcessor    The data structure that will be updated

**/
VOID
EFIAPI
UpdateProcessorInfo (
  IN UINTN              Index,
  IN LEAST_FEATURE_PROC *LeastFeatureProcessor
  )
{
  UINT16  FamilyId;
  UINT8   Model;
  UINT8   SteppingId;

  EfiCpuVersion (&FamilyId, &Model, &SteppingId, NULL);
  LeastFeatureProcessor->Index    = Index;
  LeastFeatureProcessor->ApicId   = GetCpuApicId ();
  LeastFeatureProcessor->Version  = EfiMakeCpuVersion (FamilyId, Model, SteppingId);
}


/**
  Get processor feature delta

  @param[in] FeaturesInput     Supported features for input processor
  @param[in] CommonFeatures    Supported features for processor (subset of FeaturesInput)

  @retval The least of processor features

**/
UINT32
EFIAPI
GetProcessorFeatureDelta (
  IN UINT32 *FeaturesInput,
  IN UINT32 *CommonFeatures
  )
{
  UINT32  Delta;
  UINTN   Index;

  //
  // CommonFeatures is the subset of FeaturesInput
  //
  Delta = 0;
  for (Index = 0; Index < MAX_FEATURE_NUM; Index++) {
    Delta += GetBitsNumberOfOne (FeaturesInput[Index] - CommonFeatures[Index]);
  }

  return 0;
}


/**
  Calculate how many bits are one from given number

  @param[in]  Value      Number that will be calculated bits

  @retval    Number of bits

**/
UINT32
EFIAPI
GetBitsNumberOfOne (
  IN UINT32 Value
  )
{
  UINT32  Result;

  Result = 0;
  while (Value) {
    if (Value & 1) {
      Result++;
    }
    Value >>= 1;
  }

  return Result;
}


/**
  Write 64bits MSR with script

  @param[in]  Index          MSR index that will be written
  @param[in]  Value          Value written to MSR

**/
VOID
AsmWriteMsr64WithScript (
  IN UINT32 Index,
  IN UINT64 Value
  )
{
  AsmWriteMsr64 (Index, Value);
  WriteMsr64ToScript (Index, Value);
}

/**
  Write 64bits MSR to script

  @param[in]  Index        MSR index that will be written
  @param[in]  Value        Value written to MSR

**/
VOID
WriteMsr64ToScript (
  IN UINT32 Index,
  IN UINT64 Value
  )
{
  UINTN           TableIndex;
  MP_SYSTEM_DATA  *MpSystemData;

  MpSystemData = GetMpSystemData ();
  if (MpSystemData == NULL) {
    ASSERT (FALSE);
    return;
  }

  //
  // Save it into script
  //
  AsmAcquireMPLock (&MpSystemData->S3BootScriptLock);
  TableIndex = MpSystemData->S3BootScriptCount++;
  AsmReleaseMPLock (&MpSystemData->S3BootScriptLock);

  ASSERT (TableIndex < MAX_CPU_S3_TABLE_SIZE - 1);
  MpSystemData->S3BootScriptTable[TableIndex].ApicId   = GetCpuApicId ();
  MpSystemData->S3BootScriptTable[TableIndex].MsrIndex = Index;
  MpSystemData->S3BootScriptTable[TableIndex].MsrValue = Value;
}


/**
  Set APIC BSP bit

  @param[in] Enable           Enable as BSP or not

  @retval    EFI_SUCCESS      Always return success

**/
EFI_STATUS
SetApicBspBit (
  IN BOOLEAN Enable
  )
{
  UINT64  ApicBaseReg;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);

  if (Enable) {
    ApicBaseReg |= 0x100;
  } else {
    ApicBaseReg &= 0xfffffffffffffe00;
  }

  AsmWriteMsr64 (MSR_IA32_APIC_BASE, ApicBaseReg);

  return EFI_SUCCESS;
}


/**
  Switch current BSP processor to AP

  @param[in]  MPSystemData     Pointer to the data structure containing MP related data

**/
VOID
EFIAPI
FutureBspProc (
  VOID
  )
{
  AsmExchangeRole (&mMpSystemData->ApInfo, &mMpSystemData->BspInfo);
  return;
}


/**
  Change CPU state

  @param[in] CpuNumber       CPU number
  @param[in] NewState        The new state that will be changed to
  @param[in] Cause           Cause

  @retval    EFI_SUCCESS     Always return success

**/
EFI_STATUS
ChangeCpuState (
  IN UINTN                  CpuNumber,
  IN BOOLEAN                NewState,
  IN CPU_STATE_CHANGE_CAUSE Cause
  )
{
  CPU_DATA_BLOCK    *CpuData;

  CpuData = &mMpSystemData->CpuData[CpuNumber];

  mMpSystemData->DisableCause[CpuNumber] = Cause;

  if (!NewState) {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_DISABLED;
    AsmReleaseMPLock (&CpuData->StateLock);

    ReportStatusCode (
      EFI_ERROR_MINOR | EFI_ERROR_CODE,
      EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_DISABLED
      );
  } else {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_IDLE;
    AsmReleaseMPLock (&CpuData->StateLock);
  }

  return EFI_SUCCESS;
}

