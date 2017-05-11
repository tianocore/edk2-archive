/** @file
  This is a generic template for a child of the IchSmm driver.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmmPlatform.h"
#include <Library/SideBandLib.h>
#include <Library/ReportStatusCodeLib.h>
#define PROGRESS_CODE_S3_SUSPEND_START _gPcd_FixedAtBuild_PcdProgressCodeS3SuspendStart

#define NMI_REG_OFFSET 0x3330
#define NMI_NOW BIT0
#define NMI2SMI_EN BIT2


//
// Local variables
//
typedef struct {
  UINT8     Device;
  UINT8     Function;
} EFI_PCI_BUS_MASTER;

EFI_PCI_BUS_MASTER  mPciBm[] = {
  { PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_1 },
};


BOARD_AA_NUMBER_DECODE DefectiveBoardIdTable [] = {
  {"E76523", 302},  //BLKD510MO
  {"E76525", 301},  //LAD510MO
  {"E67982", 303}   //LAD510MOV
};
UINTN DefectiveBoardIdTableSize = sizeof (DefectiveBoardIdTable) / sizeof (BOARD_AA_NUMBER_DECODE);

EFI_SMM_SYSTEM_TABLE2                    *mSmst;
UINT16                                  mAcpiBaseAddr;
SYSTEM_CONFIGURATION                    mSystemConfiguration;
EFI_SMM_VARIABLE_PROTOCOL               *mSmmVariable;
EFI_GLOBAL_NVS_AREA_PROTOCOL            *mGlobalNvsAreaPtr;
BOOLEAN                                 mSetSmmVariableProtocolSmiAllowed = TRUE;

//
// Variables. Need to initialize this from Setup
//
BOOLEAN                                 mWakeOnLanS5Variable;
BOOLEAN                                 mWakeOnRtcVariable;
BOOLEAN                                 mWakeOnLanVariable;
UINT8                                   mWakeupDay;
UINT8                                   mWakeupHour;
UINT8                                   mWakeupMinute;
UINT8                                   mWakeupSecond;
UINT8                                   mDeepStandby=0;

//
// Use an enum. 0 is Stay Off, 1 is Last State, 2 is Stay On
//
UINT8                                   mAcLossVariable;

static
UINT8 mTco1Sources[] = {
  IchnNmi
};

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

EFI_STATUS
S5SleepWakeOnRtcCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  );


VOID
EnableS5WakeOnRtc ();

EFI_STATUS
Stall (
  IN UINTN              Microseconds
  );

UINT8
HexToBcd(
  UINT8                 HexValue
  );

UINT8
BcdToHex(
  IN UINT8              BcdValue
  );

EFI_STATUS
EfiSmmGetTime (
  IN OUT EFI_TIME       *Time
  );

VOID
CpuSmmSxWorkAround(
  );


/**
  Initializes the SMM Handler Driver

  @param[in]  ImageHandle     A handle for the image that is initializing this driver.
  @param[in]  SystemTable     A pointer to the EFI system table.

  @retval     None

**/
EFI_STATUS
EFIAPI
InitializePlatformSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  UINT8                                     Index;
  EFI_HANDLE                                Handle;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT     PowerButtonContext;
  EFI_SMM_ICHN_DISPATCH_CONTEXT             IchnContext;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL   *PowerButtonDispatch;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL            *IchnDispatch;
  EFI_SMM_SX_DISPATCH2_PROTOCOL             *SxDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT               EntryDispatchContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
  UINTN                                     VarSize;
  EFI_BOOT_MODE                             BootMode;
  UINT32                                    Data32;
  Handle = NULL;

  //
  //  Locate the Global NVS Protocol.
  //
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &mGlobalNvsAreaPtr
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize global variables
  //
  mSmst = gSmst;

  //
  // Get the ACPI Base Address
  //
  mAcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &mSystemConfiguration
                  );

  if (!EFI_ERROR (Status)) {
    mAcLossVariable = mSystemConfiguration.StateAfterG3;
    mDeepStandby    = mSystemConfiguration.DeepStandby;

    //
    // If LAN is disabled, WOL function should be disabled too.
    //
    if (mSystemConfiguration.Lan == 0x01) {
      mWakeOnLanS5Variable = mSystemConfiguration.WakeOnLanS5;
    } else {
      mWakeOnLanS5Variable = FALSE;
    }

  mWakeOnLanVariable = mSystemConfiguration.Wol;
    mWakeOnRtcVariable = mSystemConfiguration.WakeOnRtcS5;
  }

  BootMode = GetBootModeHob ();

  //
  // Get the Power Button protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &PowerButtonDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  if (BootMode != BOOT_ON_FLASH_UPDATE) {
    //
    // Register for the power button event
    //
    PowerButtonContext.Phase = EfiPowerButtonEntry;
    Status = PowerButtonDispatch->Register(
                                    PowerButtonDispatch,
                                    (EFI_SMM_HANDLER_ENTRY_POINT2)PowerButtonCallback,
                                    &PowerButtonContext,
                                    &Handle
                                    );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Get the Sx dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SxDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register entry phase call back function
  //
  EntryDispatchContext.Type  = SxS3;
  EntryDispatchContext.Phase = SxEntry;

  Status = SxDispatch->Register (
                         SxDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)SxSleepEntryCallBack,
                         &EntryDispatchContext,
                         &Handle
                         );

  EntryDispatchContext.Type  = SxS4;

  Status = SxDispatch->Register (
                         SxDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)S4S5CallBack,
                         &EntryDispatchContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  EntryDispatchContext.Type  = SxS5;

  Status = SxDispatch->Register (
                         SxDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)S4S5CallBack,
                         &EntryDispatchContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  Status = SxDispatch->Register (
                         SxDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)S5SleepAcLossCallBack,
                         &EntryDispatchContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  //  Get the Sw dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI enable handler
  //
  SwContext.SwSmiInputValue = ACPI_ENABLE;
  Status = SwDispatch->Register (
                         SwDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)EnableAcpiCallback,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI disable handler
  //
  SwContext.SwSmiInputValue = ACPI_DISABLE;
  Status = SwDispatch->Register (
                         SwDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)DisableAcpiCallback,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Register the TPM PTS & SMBS Handler
  //
  SwContext.SwSmiInputValue = 0x5A;

  Status = SwDispatch->Register (
                         SwDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2)TpmPtsSmbsCallback,
                         &SwContext,
                         &Handle
                         );

  ASSERT_EFI_ERROR (Status);

  
  Data32 = IoRead32 (mAcpiBaseAddr + R_SMI_EN);
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint32,
    (mAcpiBaseAddr + R_SMI_EN),
    1,
    &Data32
    );
  
  
  //
  // Get the ICHn protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmIchnDispatchProtocolGuid,
                    NULL,
                    (VOID **) &IchnDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the events that may happen that we do not care.
  // This is true for SMI related to TCO since TCO is enabled by BIOS WP
  //
  for (Index = 0; Index < sizeof (mTco1Sources) / sizeof (UINT8); Index++) {
    IchnContext.Type = mTco1Sources[Index];
    Status = IchnDispatch->Register(
                             IchnDispatch,
                             (EFI_SMM_ICHN_DISPATCH)DummyTco1Callback,
                             &IchnContext,
                             &Handle
                             );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Lock TCO_EN bit.
  //
  IoWrite16 (mAcpiBaseAddr + R_TCO_CNT, IoRead16 (mAcpiBaseAddr + R_TCO_CNT) | B_TCO_CNT_LOCK);

  //
  // Set to power on from G3 dependent on WOL instead of AC Loss variable in order to support WOL from G3 feature.
  //
  //
  // Set wake from G3 dependent on AC Loss variable and Wake On LAN variable.
  // This is because no matter how, if WOL enabled or AC Loss variable not disabled, the board needs to wake from G3 to program the LAN WOL settings.
  // This needs to be done after LAN enable/disable so that the PWR_FLR state clear not impacted the WOL from G3 feature.
  //
  if (mAcLossVariable != 0x00) {
    SetAfterG3On (TRUE);
  } else {
    SetAfterG3On (FALSE);
  }

  return EFI_SUCCESS;
}


VOID
StallInternalFunction(
  IN UINTN              Microseconds
  )
{
  UINTN                 Ticks;
  UINTN                 Counts;
  UINT32                CurrentTick;
  UINT32                OriginalTick;
  UINT64                RemainingTick;

  OriginalTick = IoRead32 (mAcpiBaseAddr + R_ACPI_PM1_TMR);

  CurrentTick = OriginalTick;

  //
  //The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  Counts = Ticks / V_ACPI_PM1_TMR_MAX_VAL;  //The loops needed by timer overflow
  RemainingTick = Ticks % V_ACPI_PM1_TMR_MAX_VAL;  //remaining clocks within one loop

  //
  //not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  //one I/O operation, and maybe generate SMI
  //

  while (Counts != 0) {
    CurrentTick = IoRead32 (mAcpiBaseAddr + R_ACPI_PM1_TMR);
    if (CurrentTick <= OriginalTick) {
      Counts --;
    }
    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick) ) {
    OriginalTick = CurrentTick;
    CurrentTick = IoRead32(mAcpiBaseAddr + R_ACPI_PM1_TMR);
  }
}


/**
  Waits for at least the given number of microseconds.

  @param[in]  Microseconds        Desired length of time to wait

  @retval     EFI_SUCCESS         If the desired amount of time passed.

**/
EFI_STATUS
Stall (
  IN UINTN              Microseconds
  )
{
  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  StallInternalFunction (Microseconds);

  return EFI_SUCCESS;
}


EFI_STATUS
SmmReadyToBootCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     Status;

  if (mSetSmmVariableProtocolSmiAllowed) {
    //
    // It is okay to use gBS->LocateProtocol here because
    // we are still in trusted execution.
    //
    Status = gSmst->SmmLocateProtocol(
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **) &mSmmVariable
                    );

    ASSERT_EFI_ERROR (Status);

    //
    // mSetSmmVariableProtocolSmiAllowed will prevent this function from
    // being executed more than 1 time.
    //
    mSetSmmVariableProtocolSmiAllowed = FALSE;
  }

  return EFI_SUCCESS;
}


/**
  The CallBack function of SxSleep Entry.

  @param[in]  DispatchHandle    The handle of this callback, obtained when registering
  @param[in]  DispatchContext   The predefined context which contained sleep type and phase

  @retval     EFI_SUCCESS       Operation successfully performed

**/
EFI_STATUS
SxSleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS              Status;
  UINT32                  Data32;

  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PROGRESS_CODE_S3_SUSPEND_START);

  Status = SaveRuntimeScriptTable (mSmst);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set GPIO_35 to low
  //
  Data32 = MmioRead32 (PcdGet32 (PcdP2SBBaseAddress) + N_GPIO_35);
  Data32 = Data32 & ~(0x0100);
  Data32 = Data32 & ~(0x1);
  MmioWrite32 (PcdGet32 (PcdP2SBBaseAddress) + N_GPIO_35, Data32);

  //
  // Enable GPIO TIER1 SCI EN bit for LID wake function.
  //
  IoOr32 (mAcpiBaseAddr + R_ACPI_GPE0a_EN, B_ACPI_GPE0a_EN_GPIO_TIER1_SCI_EN);

  //
  // Enable PCIE WAKE EN bit if WOL feature is enabled
  //

  if (mWakeOnLanVariable) {
    IoOr32 (mAcpiBaseAddr + R_ACPI_GPE0a_EN, B_ACPI_GPE0a_EN_PCIE_WAKE0_EN);
    IoOr32 (mAcpiBaseAddr + R_ACPI_GPE0a_EN, B_ACPI_GPE0a_EN_PCIE_WAKE1_EN);
    IoOr32 (mAcpiBaseAddr + R_ACPI_GPE0a_EN, B_ACPI_GPE0a_EN_PCIE_WAKE2_EN);
  }

  //
  // Workaround for S3 wake hang if C State is enabled
  //
  CpuSmmSxWorkAround ();

  return EFI_SUCCESS;
}


VOID
CpuSmmSxWorkAround(
  )
{
  UINT64           MsrValue;

  MsrValue = AsmReadMsr64 (EFI_MSR_PMG_CST_CONFIG);

  if (MsrValue & B_EFI_MSR_PMG_CST_CONFIG_CST_CONTROL_LOCK) {
    //
    // Cannot do anything if the register is locked.
    //
    return;
  }

  if (MsrValue & B_EFI_MSR_PMG_CST_CONFIG_IO_MWAIT_REDIRECTION_ENABLE) {
    //
    // If C State enabled, disable it before going into S3
    // The MSR will be restored back during S3 wake
    //
    MsrValue &= ~B_EFI_MSR_PMG_CST_CONFIG_IO_MWAIT_REDIRECTION_ENABLE;
    AsmWriteMsr64 (EFI_MSR_PMG_CST_CONFIG, MsrValue);
  }
}


VOID
ClearP2PBusMaster(
  )
{
  UINT8             Command;
  UINT8             Index;

  for (Index = 0; Index < sizeof (mPciBm) / sizeof (EFI_PCI_BUS_MASTER); Index++) {
    Command = MmioRead8 (
                MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_SC,
                  mPciBm[Index].Device,
                  mPciBm[Index].Function,
                  PCI_COMMAND_OFFSET
                )
              );
    Command &= ~EFI_PCI_COMMAND_BUS_MASTER;
    MmioWrite8 (
      MmPciAddress (0,
        DEFAULT_PCI_BUS_NUMBER_SC,
        mPciBm[Index].Device,
        mPciBm[Index].Function,
        PCI_COMMAND_OFFSET
      ),
      Command
    );
  }
}


/**
  Set the AC Loss to turn on or off.

  @retval    None

**/
VOID
SetAfterG3On (
  BOOLEAN Enable
  )
{
  UINT8             PmCon1;

  //
  // SC handling portion
  //
  PmCon1 = MmioRead8 (PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1);
  PmCon1 &= ~B_PMC_GEN_PMCON_AFTERG3_EN;

  if (Enable) {
    PmCon1 |= B_PMC_GEN_PMCON_AFTERG3_EN;
  }

  MmioWrite8 (PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1, PmCon1);
}


/**
  When a power button event happens, it shuts off the machine

  @retval    None

**/
EFI_STATUS
PowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{
  //
  // Check what the state to return to after AC Loss. If Last State, then
  // set it to Off.
  //
  UINT16  Data16;

  if (mWakeOnRtcVariable) {
    EnableS5WakeOnRtc ();
  }

  if (mAcLossVariable == 1) {
    SetAfterG3On (TRUE);
  }

  ClearP2PBusMaster ();

  Data16 = (UINT16)(IoRead16(mAcpiBaseAddr + R_ACPI_GPE0a_EN));
  Data16 &= B_ACPI_GPE0a_EN_PCIE_GPE_EN;

  //
  // Clear Sleep SMI Status
  //
  IoWrite16 (
    mAcpiBaseAddr + R_SMI_STS,
    (UINT16) (IoRead16 (mAcpiBaseAddr + R_SMI_STS) | B_SMI_STS_ON_SLP_EN)
    );

  //
  // Clear Sleep Type Enable
  //
  IoWrite16 (
    mAcpiBaseAddr + R_SMI_EN,
    (UINT16) (IoRead16 (mAcpiBaseAddr + R_SMI_EN) & (~B_SMI_EN_ON_SLP_EN))
    );

  //
  // Clear Power Button Status
  //
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_STS, B_ACPI_PM1_STS_PWRBTN);

  //
  // Shut it off now!
  //
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_CNT, V_ACPI_PM1_CNT_S5);
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_CNT, B_ACPI_PM1_CNT_SLP_EN | V_ACPI_PM1_CNT_S5);

  return EFI_SUCCESS;
}


VOID
PmeCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{

}


EFI_STATUS
S5SleepAcLossCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  //
  // Check what the state to return to after AC Loss. If Last State, then
  // set it to Off.
  //
  if (mAcLossVariable == 1) {
    SetAfterG3On (TRUE);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
S4S5CallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  return EFI_SUCCESS;
}


/**
  SMI handler to enable ACPI mode

  Dispatched on reads from APM port with value 0xA0

  Disables the SW SMI Timer.
  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then enabled.

  Disable SW SMI Timer

  Clear all ACPI event status and disable all ACPI events
  Disable PM sources except power button
  Clear status bits

  Disable GPE0 sources
  Clear status bits

  Disable GPE1 sources
  Clear status bits

  Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)

  Enable SCI

  @param[in]   DispatchHandle    EFI Handle
  @param[in]   DispatchContext   Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval      Nothing

**/
EFI_STATUS
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  OUT UINTN                     *CommBufferSize  OPTIONAL
  )
{
  UINT8  OutputValue;
  UINT32 SmiEn;
  UINT16 Pm1Cnt;


  //
  // Disable SW SMI Timer
  //
  SmiEn = IoRead32 (mAcpiBaseAddr + R_SMI_EN);
  SmiEn &= ~B_SMI_STS_SWSMI_TMR;
  IoWrite32 (mAcpiBaseAddr + R_SMI_EN, SmiEn);


  //
  // Disable PM sources except power button
  //
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_EN, B_ACPI_PM1_EN_PWRBTN);

  //
  // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  OutputValue = RTC_ADDRESS_REGISTER_D;
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, OutputValue);
  OutputValue = 0x0;
  OutputValue = IoRead8 (PCAT_RTC_DATA_REGISTER);

  //
  // Enable SCI
  //
  Pm1Cnt = IoRead16 (mAcpiBaseAddr + R_ACPI_PM1_CNT);
  Pm1Cnt |= B_ACPI_PM1_CNT_SCI_EN;
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_CNT, Pm1Cnt);

  return EFI_SUCCESS;
}


/**
  SMI handler to disable ACPI mode

  Dispatched on reads from APM port with value 0xA1

  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then disabled.
  Clear all ACPI event status and disable all ACPI events
  Disable PM sources except power button
  Clear status bits
  Disable GPE0 sources
  Clear status bits
  Disable GPE1 sources
  Clear status bits

  Disable SCI

  @param[in]   DispatchHandle      EFI Handle
  @param[in]   DispatchContext     Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval      Nothing

**/
EFI_STATUS
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  UINT16 Pm1Cnt;

  //
  // Disable SCI
  //
  Pm1Cnt = IoRead16 (mAcpiBaseAddr + R_ACPI_PM1_CNT);
  Pm1Cnt &= ~B_ACPI_PM1_CNT_SCI_EN;
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_CNT, Pm1Cnt);

  return EFI_SUCCESS;
}


VOID
DummyTco1Callback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{
}

UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    DevicePath = NextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}


EFI_STATUS
S5SleepWakeOnRtcCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  EnableS5WakeOnRtc ();

  return EFI_SUCCESS;
}


/**
  Returns:
    Check Alarm interrupt is not set.
    Clear Alarm interrupt.
    Set RTC wake up date and time.
    Enable RTC wake up alarm.
    Enable ICH PM1 EN Bit 10(RTC_EN)

**/
VOID
EnableS5WakeOnRtc (
  )
{
  UINT8             CmosData;
  UINTN             i;
  EFI_STATUS        Status;
  UINTN             VarSize;

  //
  // make sure EFI_SMM_VARIABLE_PROTOCOL is available
  //
  if (!mSmmVariable) {
    return;
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &mSystemConfiguration
                  );

  if (EFI_ERROR (Status) || (!mSystemConfiguration.WakeOnRtcS5)) {
    return;
  }
  mWakeupDay = HexToBcd ((UINT8) mSystemConfiguration.RTCWakeupDate);
  mWakeupHour = HexToBcd ((UINT8) mSystemConfiguration.RTCWakeupTimeHour);
  mWakeupMinute = HexToBcd ((UINT8) mSystemConfiguration.RTCWakeupTimeMinute);
  mWakeupSecond = HexToBcd ((UINT8) mSystemConfiguration.RTCWakeupTimeSecond);

  //
  // Check RTC alarm interrupt is enabled.  If enabled, someone already
  // grabbed RTC alarm.  Just return.
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  if (IoRead8 (PCAT_RTC_DATA_REGISTER) & B_RTC_ALARM_INT_ENABLE) {
    return;
  }

  //
  // Set Date
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
  CmosData = IoRead8 (PCAT_RTC_DATA_REGISTER);
  CmosData &= ~(B_RTC_DATE_ALARM_MASK);
  CmosData |= mWakeupDay ;
  for (i = 0 ; i < 0xffff ; i++) {
    IoWrite8 (PCAT_RTC_DATA_REGISTER, CmosData);
    Stall (1);
    if (((CmosData = IoRead8 (PCAT_RTC_DATA_REGISTER)) & B_RTC_DATE_ALARM_MASK) == mWakeupDay) {
      break;
    }
  }

  //
  // Set Second
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_SECOND_ALARM);
  for (i = 0 ; i < 0xffff ; i++) {
    IoWrite8 (PCAT_RTC_DATA_REGISTER, mWakeupSecond);
    Stall (1);
    if (IoRead8 (PCAT_RTC_DATA_REGISTER) == mWakeupSecond) {
      break;
    }
  }

  //
  // Set Minute
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_MINUTE_ALARM);
  for (i = 0 ; i < 0xffff ; i++){
    IoWrite8 (PCAT_RTC_DATA_REGISTER, mWakeupMinute);
    Stall (1);
    if (IoRead8 (PCAT_RTC_DATA_REGISTER) == mWakeupMinute) {
      break;
    }
  }

  //
  // Set Hour
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_HOUR_ALARM);
  for (i = 0 ; i < 0xffff ; i++) {
    IoWrite8 (PCAT_RTC_DATA_REGISTER, mWakeupHour);
    Stall (1);
    if (IoRead8 (PCAT_RTC_DATA_REGISTER) == mWakeupHour) {
      break;
    }
  }

  //
  // Wait for UIP to arm RTC alarm
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_A);
  while (IoRead8 (PCAT_RTC_DATA_REGISTER) & 0x80);

  //
  // Read RTC register 0C to clear pending RTC interrupts
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_C);
  IoRead8 (PCAT_RTC_DATA_REGISTER);

  //
  // Enable RTC Alarm Interrupt
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, IoRead8(PCAT_RTC_DATA_REGISTER) | B_RTC_ALARM_INT_ENABLE);

  //
  // Clear ICH RTC Status
  //
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_STS, B_ACPI_PM1_STS_RTC);

  //
  // Enable ICH RTC event
  //
  IoWrite16 (mAcpiBaseAddr + R_ACPI_PM1_EN,
              (UINT16) (IoRead16 (mAcpiBaseAddr + R_ACPI_PM1_EN) | B_ACPI_PM1_EN_RTC));
}


UINT8
HexToBcd(
  IN UINT8 HexValue
  )
{
  UINTN   HighByte;
  UINTN   LowByte;

  HighByte    = (UINTN) HexValue / 10;
  LowByte     = (UINTN) HexValue % 10;

  return ((UINT8) (LowByte + (HighByte << 4)));
}


UINT8
BcdToHex(
  IN UINT8 BcdValue
  )
{
  UINTN   HighByte;
  UINTN   LowByte;

  HighByte    = (UINTN) ((BcdValue >> 4) * 10);
  LowByte     = (UINTN) (BcdValue & 0x0F);

  return ((UINT8) (LowByte + HighByte));
}


/**
  TPM Handler

  @param[in]   DispatchHandle    The handle of this callback, obtained when registering
  @param[in]   DispatchContext   Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval      None.

**/
EFI_STATUS
TpmPtsSmbsCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )
{
  UINT8         TpmSetting;

  //
  // Setting MORD
  //
  TpmSetting = EFI_ACPI_TPM_MORD;
  mSmst->SmmIo.Io.Write (&mSmst->SmmIo, SMM_IO_UINT8, 0x72, 1, &TpmSetting);
  TpmSetting = mGlobalNvsAreaPtr->Area->MorData;
  mSmst->SmmIo.Io.Write (&mSmst->SmmIo, SMM_IO_UINT8, 0x73, 1, &TpmSetting);

  return EFI_SUCCESS;
}

