/** @file
  Cpu driver, which initializes CPU and implements CPU Architecture
  Protocol as defined in Framework specification.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/MpService.h>
#include <Protocol/CpuGlobalNvsArea.h>
#include <Private/CpuInitDataHob.h>
#include "CpuInitDxe.h"
#include "Exception.h"
#include <Private/Library/CpuCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ConfigBlockLib.h>

//
// Private GUID for BIOS Guard initializes
//
extern EFI_GUID gBiosGuardHobGuid;

GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA_PROTOCOL           CpuGlobalNvsAreaProtocol;


#ifdef SLE_FLAG
#define SAMPLE_TICK_COUNT 5
#else
#define SAMPLE_TICK_COUNT 1000
#endif //SLE_FLAG

extern UINT64                   mValidMtrrAddressMask;
extern UINT64                   mValidMtrrBitsMask;
extern UINT8                    CpuInitDxeStrings[];
extern UINT8                    mDefaultMemoryType;
CONST CHAR8 *DefaultVersion = "Intel(R) Genuine processor";

GLOBAL_REMOVE_IF_UNREFERENCED VOID                             *mSmmBaseRegistration;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_METRONOME_ARCH_PROTOCOL      *mMetronome;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                          mIsFlushingGCD = TRUE;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                            mSmmbaseSwSmiNumber;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                          mVariableMtrrChanged;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                          mFixedMtrrChanged;
GLOBAL_REMOVE_IF_UNREFERENCED UINT64                           mCpuFrequency = 0;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_EVENT                        gReadyToBootEvent;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_INTERRUPT_HANDLER        mExternalVectorTable[0x100];
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                          mInterruptState = FALSE;

//
// The Cpu Init Data Hob
//
GLOBAL_REMOVE_IF_UNREFERENCED CPU_INIT_DATA_HOB                *mCpuInitDataHob = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_CONFIG                       *CpuConfig       = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED POWER_MGMT_CONFIG                *PowerMgmtConfig = NULL;


GLOBAL_REMOVE_IF_UNREFERENCED CPU_INFO_PROTOCOL                *mCpuInfo;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            mCommonFeatures;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                            mSiliconFeatures;

//
// The Cpu Architectural Protocol that this Driver produces
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPU_ARCH_PROTOCOL gCpu = {
  FlushCpuDataCache,
  EnableInterrupt,
  DisableInterrupt,
  CpuGetInterruptState,
  Init,
  RegisterInterruptHandler,
  GetTimerValue,
  SetMemoryAttributes,
  1, ///< NumberOfTimers
  4, ///< DmaBufferAlignment
};

/**
  Flush CPU data cache. If the instruction cache is fully coherent
  with all DMA operations then function can just return EFI_SUCCESS.

  @param[in] This                  Protocol instance structure
  @param[in] Start                 Physical address to start flushing from.
  @param[in] Length                Number of bytes to flush. Round up to chipset
                                   granularity.
  @param[in] FlushType             Specifies the type of flush operation to perform.

  @retval    EFI_SUCCESS           If cache was flushed
  @retval    EFI_UNSUPPORTED       If flush type is not supported.
  @retval    EFI_DEVICE_ERROR      If requested range could not be flushed.

**/
EFI_STATUS
EFIAPI
FlushCpuDataCache (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_PHYSICAL_ADDRESS  Start,
  IN UINT64                Length,
  IN EFI_CPU_FLUSH_TYPE    FlushType
  )
{
  if (FlushType == EfiCpuFlushTypeWriteBackInvalidate) {
    AsmWbinvd ();
    return EFI_SUCCESS;
  } else if (FlushType == EfiCpuFlushTypeInvalidate) {
    AsmInvd ();
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}


/**
  Enables CPU interrupts.

  @param[in] This                  Protocol instance structure

  @retval    EFI_SUCCESS           If interrupts were enabled in the CPU.

**/
EFI_STATUS
EFIAPI
EnableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL *This
  )
{
  EnableInterrupts ();
  mInterruptState = TRUE;

  return EFI_SUCCESS;
}


/**
  Disables CPU interrupts.

  @param[in] This                  Protocol instance structure

  @retval    EFI_SUCCESS           If interrupts were disabled in the CPU.

**/
EFI_STATUS
EFIAPI
DisableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL *This
  )
{
  DisableInterrupts ();
  mInterruptState = FALSE;

  return EFI_SUCCESS;
}


/**
  Return the state of interrupts.

  @param[in]  This                    Protocol instance structure
  @param[out] State                   Pointer to the CPU's current interrupt state

  @retval     EFI_SUCCESS             If interrupts were disabled in the CPU.
  @retval     EFI_INVALID_PARAMETER   State is NULL.

**/
EFI_STATUS
EFIAPI
CpuGetInterruptState (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  OUT BOOLEAN              *State
  )
{
  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *State = mInterruptState;

  return EFI_SUCCESS;
}


/**
  Generates an INIT to the CPU

  @param[in] This                 Protocol instance structure
  @param[in] InitType             Type of CPU INIT to perform

  @retval    EFI_SUCCESS          If CPU INIT occurred. This value should never be seen
  @retval    EFI_DEVICE_ERROR     If CPU INIT failed.
  @retval    EFI_UNSUPPORTED      Requested type of CPU INIT not supported.

**/
EFI_STATUS
EFIAPI
Init (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_CPU_INIT_TYPE     InitType
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Registers a function to be called from the CPU interrupt handler.

  @param[in] This                   Protocol instance structure
  @param[in] InterruptType          Defines which interrupt to hook.
                                    IA-32 valid range is 0x00 through 0xFF
  @param[in] InterruptHandler       A pointer to a function of type EFI_CPU_INTERRUPT_HANDLER
                                    that is called when a processor interrupt occurs.
                                    A null pointer is an error condition.

  @retval    EFI_SUCCESS            If handler installed or uninstalled.
  @retval    EFI_ALREADY_STARTED    InterruptHandler is not NULL, and a handler for
                                    InterruptType was previously installed
  @retval    EFI_INVALID_PARAMETER  InterruptHandler is NULL, and a handler for
                                    InterruptType was not previously installed.
  @retval    EFI_UNSUPPORTED        The interrupt specified by InterruptType is not supported.

**/
EFI_STATUS
EFIAPI
RegisterInterruptHandler (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_EXCEPTION_TYPE        InterruptType,
  IN EFI_CPU_INTERRUPT_HANDLER InterruptHandler
  )
{
  BOOLEAN   State;

  if (InterruptType < 0 || InterruptType > 0xff) {
    return EFI_UNSUPPORTED;
  }

  if (InterruptHandler == NULL && mExternalVectorTable[InterruptType] == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (InterruptHandler != NULL && mExternalVectorTable[InterruptType] != NULL) {
    return EFI_ALREADY_STARTED;
  }

  //
  // State is stored with the current interrupt state from CPU Arch Prot.
  // Keep interrupts disabled until we finish registering.
  //
  (This->GetInterruptState) (This, &State);
  if (State) {
    This->DisableInterrupt (This);
  }

  if (InterruptHandler != NULL) {
    SetInterruptDescriptorTableHandlerAddress ((UINTN)InterruptType);
  } else {
    //
    // Restore the original IDT handler address if InterruptHandler is NULL.
    //
    RestoreInterruptDescriptorTableHandlerAddress ((UINTN) InterruptType);
  }

  mExternalVectorTable[InterruptType] = InterruptHandler;

  if (State) {
    This->EnableInterrupt (This);
  }

  return EFI_SUCCESS;
}


/**
  Returns a timer value from one of the CPU's internal timers. There is no
  inherent time interval between ticks but is a function of the CPU frequency.

  @param[in]  This                    Protocol instance structure.
  @param[in]  TimerIndex              Specifies which CPU timer is requested.
  @param[out] TimerValue              Pointer to the returned timer value.
  @param[out] TimerPeriod             A pointer to the amount of time that passes in femtoseconds (10-15) for each
                                      increment of TimerValue. If TimerValue does not increment at a predictable
                                      rate, then 0 is returned. The amount of time that has passed between two calls to
                                      GetTimerValue() can be calculated with the formula
                                      (TimerValue2 - TimerValue1) * TimerPeriod. This parameter is optional and may be NULL.

  @retval     EFI_SUCCESS             If the CPU timer count was returned.
  @retval     EFI_UNSUPPORTED         If the CPU does not have any readable timers.
  @retval     EFI_DEVICE_ERROR        If an error occurred while reading the timer.
  @retval     EFI_INVALID_PARAMETER   TimerIndex is not valid or TimerValue is NULL.

**/
EFI_STATUS
EFIAPI
GetTimerValue (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN UINT32                TimerIndex,
  OUT UINT64               *TimerValue,
  OUT UINT64 *TimerPeriod  OPTIONAL
  )
{
  UINT64 Actual;

  if (TimerValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (TimerIndex != 0) {
    return EFI_INVALID_PARAMETER;
  }

  *TimerValue = AsmReadTsc ();

  if (TimerPeriod != NULL) {
    GetActualFrequency (mMetronome, &Actual);
    *TimerPeriod = DivU64x32 (1000000000, (UINT32) Actual);
  }

  return EFI_SUCCESS;
}


/**
  Set memory cacheability attributes for given range of memeory

  @param[in] This                   Protocol instance structure
  @param[in] BaseAddress            Specifies the start address of the memory range
  @param[in] Length                 Specifies the length of the memory range
  @param[in] Attributes             The memory cacheability for the memory range

  @retval    EFI_SUCCESS            If the cacheability of that memory range is set successfully
  @retval    EFI_UNSUPPORTED        If the desired operation cannot be done
  @retval    EFI_INVALID_PARAMETER  The input parameter is not correct, such as Length = 0

**/
EFI_STATUS
EFIAPI
SetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN UINT64                Length,
  IN UINT64                Attributes
  )
{
  EFI_STATUS                Status;
  UINT64                    TempQword;
  UINT32                    MsrNum;
  UINTN                     MtrrNumber;
  BOOLEAN                   Positive;
  BOOLEAN                   OverLap;
  EFI_MP_SERVICES_PROTOCOL  *MpService;
  EFI_STATUS                Status1;
  UINT32                    VariableMtrrLimit;

  mFixedMtrrChanged     = FALSE;
  mVariableMtrrChanged  = FALSE;

  VariableMtrrLimit     = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  if (mIsFlushingGCD) {
    return EFI_SUCCESS;
  }

  TempQword = 0;

  //
  // Check for invalid parameter
  //
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BaseAddress & ~mValidMtrrAddressMask) != 0 || (Length & ~mValidMtrrAddressMask) != 0) {
    return EFI_UNSUPPORTED;
  }

  switch (Attributes) {
    case EFI_MEMORY_UC:
      Attributes = CACHE_UNCACHEABLE;
      break;

    case EFI_MEMORY_WC:
      Attributes = CACHE_WRITECOMBINING;
      break;

    case EFI_MEMORY_WT:
      Attributes = CACHE_WRITETHROUGH;
      break;

    case EFI_MEMORY_WP:
      Attributes = CACHE_WRITEPROTECTED;
      break;

    case EFI_MEMORY_WB:
      Attributes = CACHE_WRITEBACK;
      break;

    default:
      return EFI_UNSUPPORTED;
  }

  //
  // Check if Fixed MTRR
  //
  Status = EFI_SUCCESS;
  while ((BaseAddress < (1 << 20)) && (Length > 0) && Status == EFI_SUCCESS) {
    Status = CalculateFixedMtrr (Attributes, &BaseAddress, &Length);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (mFixedMtrrChanged) {
    ProgramFixedMtrr ();
  }

  if (Length == 0) {
    //
    // Just Fixed MTRR. NO need to go through Variable MTRR
    //
    goto Done;
  }

  //
  // since mem below 1m will be override by fixed mtrr, we can set it to 0 to save mtrr.
  //
  if (BaseAddress == 0x100000) {
    BaseAddress = 0;
    Length += 0x100000;
  }

  //
  // Check overlap
  //
  GetMemoryAttribute ();
  OverLap = CheckMemoryAttributeOverlap (BaseAddress, BaseAddress + Length - 1);
  if (OverLap) {
    Status = CombineMemoryAttribute (Attributes, &BaseAddress, &Length);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    if (Length == 0) {
      //
      // combine successfully
      //
      Status = EFI_SUCCESS;
      goto Done;
    }
  } else {
    if (Attributes == mDefaultMemoryType) {
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  //
  // Program Variable MTRRs
  //
  if (mUsedMtrr >= VariableMtrrLimit) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // Find first unused MTRR
  //
  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {
    if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) == 0) {
      break;
    }
  }

  TempQword = Length;
  if (TempQword == Power2MaxMemory (TempQword)) {
    ProgramVariableMtrr (
      MsrNum,
      BaseAddress,
      Length,
      Attributes
      );
  } else {
    GetDirection (TempQword, &MtrrNumber, &Positive);
    if ((mUsedMtrr + MtrrNumber) > VariableMtrrLimit) {
      goto Done;
    }
    if (!Positive) {
      Length = Power2MaxMemory (LShiftU64 (TempQword, 1));
      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        Attributes
        );
      BaseAddress += TempQword;
      TempQword   = Length - TempQword;
      Attributes  = CACHE_UNCACHEABLE;
    }
    do {
      //
      // Find unused MTRR
      //
      for (; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {
        if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) == 0) {
          break;
        }
      }
      Length = Power2MaxMemory (TempQword);
      ProgramVariableMtrr (
        MsrNum,
        BaseAddress,
        Length,
        Attributes
        );
      BaseAddress += Length;
      TempQword -= Length;
    } while (TempQword);
  }

Done:
  Status1 = gBS->LocateProtocol (
                   &gEfiMpServiceProtocolGuid,
                   NULL,
                   (VOID **) &MpService
                   );

  if (!EFI_ERROR (Status1)) {
    if (mVariableMtrrChanged || mFixedMtrrChanged) {
      //
      // PERF_START (NULL, L"CacheSync", NULL, 0);
      //
      ReadMtrrRegisters ();
      Status1 = MpService->StartupAllAPs (
                             MpService,
                             MpMtrrSynchUp,
                             FALSE,
                             NULL,
                             0,
                             NULL,
                             NULL
                             );
      //
      // PERF_END (NULL, L"CacheSync", NULL, 0);
      //
    }
  }

  return Status;
}


VOID
EFIAPI
MpIATrust (
  IN VOID *Buffer
  )
{
  UINT64    Data;

  Data = AsmReadMsr64 (0x120);
  Data |= BIT6;
  AsmWriteMsr64 (0x120, Data);
}


EFI_STATUS
EFIAPI
SetUntrustedModeExitBootServicesEvent (
  VOID
  )
{
  EFI_MP_SERVICES_PROTOCOL *MpService;
  EFI_STATUS               Status;

  DEBUG((EFI_D_INFO, "Setting Untrusted Mode\n"));

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );

  if (!EFI_ERROR (Status)) {
    MpIATrust (NULL);
    Status = MpService->StartupAllAPs (
                          MpService,
                          MpIATrust,
                          FALSE,
                          NULL,
                          0,
                          NULL,
                          NULL
                          );
  }

  return Status;
}


VOID
EFIAPI
CpuExitBootServicesCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS    Status;

  //
  // Change Boot Personality At End of Boot (Set Untrusted Mode)
  //
  Status = SetUntrustedModeExitBootServicesEvent ();
  ASSERT_EFI_ERROR (Status);
}


/**
  Initialize the state information for the CPU Architectural Protocol

  @param[in]  ImageHandle             Image handle of the loaded driver
  @param[in]  SystemTable             Pointer to the System Table

  @retval     EFI_SUCCESS             thread can be successfully created
  @retval     EFI_OUT_OF_RESOURCES    cannot allocate protocol data structure
  @retval     EFI_DEVICE_ERROR        cannot create the thread

**/
EFI_STATUS
EFIAPI
InitializeCpu (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        NewHandle1;
  EFI_EVENT         ExitBootServiceEvent;
  VOID              *Hob;

  //
  // Initialize the Global Descriptor Table
  //
  InitializeSelectors ();

  //
  // Setup Cache attributes and Interrupt Tables
  //
  PrepareMemory ();

  //
  // Initialize Exception Handlers
  //
  InitializeException (&gCpu);

  //
  // Install CPU Architectural Protocol
  //
  NewHandle1  = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle1,
                  &gEfiCpuArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gCpu
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Refresh memory space attributes according to MTRRs
  //
  Status          = RefreshGcdMemoryAttributes ();
  mIsFlushingGCD  = FALSE;
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get CPU Init Data Hob
  //
  Hob = GetFirstGuidHob (&gCpuInitDataHobGuid);
  if (Hob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return EFI_NOT_FOUND;
  }
  mCpuInitDataHob    = (CPU_INIT_DATA_HOB *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));
  CpuConfig          = (CPU_CONFIG *) (UINTN) mCpuInitDataHob->CpuConfig;
  PowerMgmtConfig    = (POWER_MGMT_CONFIG *) (UINTN) mCpuInitDataHob->PowerMgmtConfig;


  mSiliconFeatures   = mCpuInitDataHob->SiliconInfo;

  //
  // Initialize the global SmmBase SWSMI number
  //
  mSmmbaseSwSmiNumber = CpuConfig->SmmbaseSwSmiNumber;

  Status  = gBS->LocateProtocol (&gEfiMetronomeArchProtocolGuid, NULL, (VOID **) &mMetronome);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Initialize DxeCpuInfo protocol instance and gather CPU information
  //
  Status = InitCpuInfo ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to initialize DxeCpuInfo\n"));
  }

  //
  // Initialize MP Support if necessary
  //
  Status = InitializeMpSupport ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to initialize MPs\n"));
  }

  //
  // Register the CPU ExitBootServices callback function
  //
  Status = gBS->CreateEvent(
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  CpuExitBootServicesCallback,
                  NULL,
                  &ExitBootServiceEvent
                  );

  //
  // Get CPU Global NVS protocol pointer
  //
  CpuGlobalNvsAreaProtocol.Area = (CPU_GLOBAL_NVS_AREA *) (UINTN) mCpuInitDataHob->CpuGnvsPointer;
  CpuGlobalNvsAreaProtocol.Area->DtsAcpiEnable  = 0;

  //
  // Install Cpu Power management GlobalNVS Area protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gCpuGlobalNvsAreaProtocolGuid,
                  &CpuGlobalNvsAreaProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  Returns the actual CPU core frequency in MHz.

  @param[in]  Metronome               Metronome protocol
  @param[out] Frequency               Pointer to the CPU core frequency

  @retval     EFI_SUCCESS             If the frequency is returned successfully
  @retval     EFI_INVALID_PARAMETER   If the input parameter is wrong

**/
EFI_STATUS
GetActualFrequency (
  IN EFI_METRONOME_ARCH_PROTOCOL *Metronome,
  OUT UINT64                     *Frequency
  )
{
  UINT64      BeginValue;
  UINT64      EndValue;
  UINT64      TotalValue;
  UINT32      TickCount;
  BOOLEAN     InterruptState;
  EFI_STATUS  Status;

  if (Metronome == NULL || Frequency == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mCpuFrequency == 0) {
    //
    // In order to calculate the actual CPU frequency, we keep track of the CPU Tsc value (which
    // increases by 1 for every cycle) for a know period of time. The Metronome is not accurate
    // for the 1st tick, so I choose to wait for 1000 ticks, thus the error can be control to be
    // lower than 1%.
    //
    TickCount = SAMPLE_TICK_COUNT;
    CpuGetInterruptState (&gCpu, &InterruptState);
    if (InterruptState) {
      DisableInterrupt (&gCpu);
    }
    //
    // In DxeCis-0.91 specs.
    // Metronome->WaitForTick is possible for interrupt processing,
    // or exception processing to interrupt the execution of the WaitForTick() function.
    // Depending on the hardware source for the ticks, it is possible for a tick to be missed.
    // This function cannot guarantee that ticks will not be missed.
    //
    while (TRUE) {
      BeginValue  = AsmReadTsc ();
      Status      = Metronome->WaitForTick (Metronome, TickCount);
      EndValue    = AsmReadTsc ();
      if (!EFI_ERROR (Status)) {
        TotalValue = EndValue - BeginValue;
        break;
      }
    }

    if (InterruptState) {
      EnableInterrupt (&gCpu);
    }

    mCpuFrequency = MultU64x32 (TotalValue, 10);
    mCpuFrequency = DivU64x32 (mCpuFrequency, Metronome->TickPeriod * TickCount);
  }

  *Frequency = mCpuFrequency;

  return EFI_SUCCESS;
}


/**
  Initialize CPU info.

  @retval  EFI_SUCCESS     successfully prepared.

**/
EFI_STATUS
InitCpuInfo (
  VOID
  )
{
  CACHE_DESCRIPTOR_INFO *CacheInfo;
  CHAR8                 *BrandString;
  EFI_CPUID_REGISTER    CpuidRegs;
  UINT32                CpuSignature;
  UINT8                 CacheInfoCount;
  UINT16                CachePartitions;
  UINT16                CacheLineSize;
  UINT32                CacheNumberofSets;
  UINT8                 ThreadsPerCore;
  UINT64                MsrData;
  UINT8                 Index;
  EFI_HANDLE            Handle;
  UINT16                MaxEnabledThreadsPerCore;
  UINT16                MaxEnabledCoresPerDie;
  UINT16                MaxDiesPerPackage;
  UINT16                MaxPackages;
  UINT64                Frequency;
  Handle                = NULL;

  //
  // Install CPU info protocol
  //
  mCpuInfo                             = AllocateZeroPool (sizeof (CPU_INFO_PROTOCOL));
  if (mCpuInfo== NULL) {
    DEBUG ((EFI_D_ERROR, "mCpuInfo is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  mCpuInfo->Revision                   = CPU_INFO_PROTOCOL_REVISION;
  mCpuInfo->CpuCommonFeatures          = mCommonFeatures | (mSiliconFeatures << 10);

  mCpuInfo->CpuInfo                    = AllocateZeroPool (sizeof (CPU_INFO));
  if (mCpuInfo->CpuInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "mCpuInfo->CpuInfo is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mCpuInfo->CpuInfo->BrandString       = AllocateZeroPool (49 * sizeof (CHAR8));
  if (mCpuInfo->CpuInfo->BrandString == NULL) {
    DEBUG ((EFI_D_ERROR, "mCpuInfo->CpuInfo->BrandString is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mCpuInfo->SmramCpuInfo               = AllocateZeroPool (sizeof (SMRAM_CPU_INFO));
  if (mCpuInfo->SmramCpuInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "mCpuInfo->SmramCpuInfo is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get Cache Descriptors.
  //
  CacheInfoCount = 0;
  do {
    AsmCpuidEx (CPUID_FUNCTION_4, CacheInfoCount, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
    CacheInfoCount++;
  } while (((UINT8) CpuidRegs.RegEax & 0x0F)!=0);

  DEBUG ((DEBUG_INFO, "CacheInfoCount = %x\n", CacheInfoCount));
  CacheInfoCount--;
  mCpuInfo->CpuInfo->CacheInfo          = AllocateZeroPool (CacheInfoCount * sizeof (CACHE_DESCRIPTOR_INFO));
  if (mCpuInfo->CpuInfo->CacheInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "mCpuInfo->CpuInfo->CacheInfo is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  mCpuInfo->CpuInfo->MaxCacheSupported  = CacheInfoCount;
  mCpuInfo->CpuInfo->SmmbaseSwSmiNumber = mSmmbaseSwSmiNumber;

  BrandString = mCpuInfo->CpuInfo->BrandString;
  CacheInfo   = mCpuInfo->CpuInfo->CacheInfo;

  //
  // Get Brand string
  //
  AsmCpuid (CPUID_BRAND_STRING1, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
  if (CpuidRegs.RegEax != 0) {
    *(UINT32 *) BrandString = CpuidRegs.RegEax; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEbx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEcx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEdx; BrandString +=4;

    AsmCpuid (CPUID_BRAND_STRING2, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
    *(UINT32 *) BrandString = CpuidRegs.RegEax; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEbx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEcx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEdx; BrandString +=4;

    AsmCpuid (CPUID_BRAND_STRING3, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
    *(UINT32 *) BrandString = CpuidRegs.RegEax; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEbx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEcx; BrandString +=4;
    *(UINT32 *) BrandString = CpuidRegs.RegEdx; BrandString +=4;
    *BrandString = '\0';
  } else {
    CopyMem (BrandString, DefaultVersion, AsciiStrLen (DefaultVersion));
  }
  //
  // Remove leading spaces. After removing leading spaces, the Brand String can not be freed. However, it should never be freed.
  //
  while (*mCpuInfo->CpuInfo->BrandString == ' ') {
    ++mCpuInfo->CpuInfo->BrandString;
   }

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
  // Gather CPU info
  //
  AsmCpuid (CPUID_VERSION_INFO, &CpuSignature, NULL, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
  mCpuInfo->CpuInfo->CpuSignature               = CpuSignature;
  mCpuInfo->CpuInfo->Features                   = LShiftU64 (CpuidRegs.RegEcx, 32) + CpuidRegs.RegEdx;

  AsmCpuidEx (CPUID_CORE_TOPOLOGY, 0, NULL, &CpuidRegs.RegEbx, NULL, NULL);
  mCpuInfo->CpuInfo->NumSupportedThreadsPerCore = (UINT8) CpuidRegs.RegEbx;
  ThreadsPerCore = (UINT8) CpuidRegs.RegEbx;

  AsmCpuidEx (CPUID_CORE_TOPOLOGY, 1, NULL, &CpuidRegs.RegEbx, NULL, NULL);
  mCpuInfo->CpuInfo->NumSupportedCores          = (UINT8) (CpuidRegs.RegEbx / ThreadsPerCore);

  MsrData = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  mCpuInfo->CpuInfo->NumCores                   = (UINT8) MaxEnabledCoresPerDie;
  mCpuInfo->CpuInfo->NumHts                     = (UINT8) MaxEnabledThreadsPerCore;
  mCpuInfo->CpuInfo->IntendedFreq               = (10000 * (((UINT32) AsmReadMsr64 (MSR_PLATFORM_INFO) >> 8) & 0xFF)) /100;
  GetActualFrequency (mMetronome, &Frequency);
  //
  //  Units in MHz
  //
  mCpuInfo->CpuInfo->ActualFreq                 = (UINT32) Frequency;
  mCpuInfo->CpuInfo->Voltage                    = 0;

  CacheInfoCount = mCpuInfo->CpuInfo->MaxCacheSupported;
  for (Index=0; Index <= CacheInfoCount; Index++) {
    AsmCpuidEx (CPUID_FUNCTION_4, Index, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);
    CacheInfo[Index].Type=(UINT8) (((UINT8) CpuidRegs.RegEax) & CPU_CACHE_TYPE_MASK);
    CacheInfo[Index].Level=(UINT8) ((((UINT8) CpuidRegs.RegEax) >> 5) & CPU_CACHE_LEVEL_MASK);
    CacheInfo[Index].Associativity= (UINT16) (((CpuidRegs.RegEbx >> 22) & CPU_CACHE_ASSOCIATIVITY_MASK)+ 1);
    //
    // Determine Cache Size in Bytes = (Associativity) * (Partitions + 1) * (Line_Size + 1) * (Sets + 1)= (EBX[31:22] + 1) * (EBX[21:12] + 1) * (EBX[11:0] + 1) * (ECX + 1)
    //
    CachePartitions = (UINT16) (((CpuidRegs.RegEbx >> 12)& CPU_CACHE_PARTITION_MASK)+ 1 );
    CacheLineSize =(UINT16) (((UINT16) CpuidRegs.RegEbx & CPU_CACHE_LINE_SIZE_MASK)+ 1);
    CacheNumberofSets = CpuidRegs.RegEcx + 1;
    CacheInfo[Index].Size = (UINT32) ((CacheInfo[Index].Associativity *  CachePartitions * CacheLineSize * CacheNumberofSets) / 1024);
  }

  gBS->InstallMultipleProtocolInterfaces (
         &Handle,
         &gCpuInfoProtocolGuid,
         mCpuInfo,
         NULL
         );

  return EFI_SUCCESS;
}

