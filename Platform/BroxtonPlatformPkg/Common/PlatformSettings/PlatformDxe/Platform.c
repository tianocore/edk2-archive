/** @file
  Platform Initialization Driver.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include "Platform.h"
#include "Logo.h"

#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Library/SideBandLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformSecureDefaultsLib.h>
#include <Library/SteppingLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/UsbPolicy.h>
#include <Protocol/TpmMp.h>
#include <Protocol/SeCPlatformPolicy.h>
#include <Protocol/SeCOperation.h>
#include <Protocol/GlobalNvsArea.h>
#include <Guid/DataHubRecords.h>
#include <Guid/PciLanInfo.h>
#include <Guid/BxtVariable.h>
#include <Guid/EventGroup.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <PlatformBootMode.h>
#include <Library/HeciMsgLib.h>
#include <Ppi/SiPolicyPpi.h>
#include <Library/GpioLib.h>

#if (ENBDT_PF_ENABLE == 1) //BXTP
  #include <Library/PlatformCmosLib.h>
#endif

extern EFI_GUID gEfiBootMediaHobGuid;

EFI_GUID                                  mPlatformDriverGuid = EFI_PLATFORM_DRIVER_GUID;
SYSTEM_CONFIGURATION                      mSystemConfiguration;
UINT32                                    mSetupVariableAttributes;
EFI_HANDLE                                mImageHandle;
BOOLEAN                                   mMfgMode = FALSE;
UINT32                                    mPlatformBootMode = PLATFORM_NORMAL_MODE;
EFI_PLATFORM_INFO_HOB                     mPlatformInfo;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EVENT   mEndOfDxeEvent;
EFI_EVENT                                 mReadyToBootEvent;
UINT32                                    mSubsystemVidDid;
UINTN                                     mPciLanCount = 0;
VOID                                      *mPciLanInfo = NULL;
EFI_USB_POLICY_PROTOCOL                   mUsbPolicyData = {0};
BOOLEAN                                   mPMCReadyToBootFirstTime = TRUE;
EFI_BOOT_SCRIPT_SAVE_PROTOCOL             *mBootScriptSave;

BXT_GPIO_PAD_INIT  SdcardGpio[] =
{ //
  //                   Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L, Inverted,GPI_ROUT,IOSstae, IOSTerm,   MMIO_Offset, Community
  //
  BXT_GPIO_PAD_CONF (L"GPIO_177 SDCARD_CD_B",       M0,     GPI   , GPIO_D,  NA    ,   Edge    ,Wake_Disabled, P_20K_H,    NA   ,    NA  ,TxDRxE ,  NA    ,   0x04a8 ,     SOUTHWEST) //SD Card Wake Int
};

VOID
EFIAPI
PMCReadyToBoot (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  EFI_STATUS                    Status;
  UINT32                        PciBar0RegOffset;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
  EFI_GUID                      SSRAMBASEGuid = { 0x9cfa1ece, 0x4488, 0x49be, { 0x9a, 0x4b, 0xe9, 0xb5, 0x11, 0x82, 0x65, 0x77 } };
  UINT32                        Data32;

  if (mPMCReadyToBootFirstTime == FALSE) {
    return;
  }
  mPMCReadyToBootFirstTime = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &GlobalNvsArea
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Save PMC IPC1 Bar0 (PcdPmcIpc1BaseAddress0) to GNVS and size is 0x2000(8KB).
    //
    PciBar0RegOffset = (UINT32) MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_PMC_IPC1, PCI_FUNCTION_NUMBER_PMC_IPC1, R_PMC_IPC1_BASE);
    DEBUG ((DEBUG_INFO, " IPC1 BAR0 MMIO = %x \n ", (MmioRead32 (PciBar0RegOffset) & 0xFFFFE000)));
    GlobalNvsArea->Area->IPCBar0Address = (MmioRead32 (PciBar0RegOffset) & 0xFFFFE000);
    GlobalNvsArea->Area->IPCBar0Length = 0x2000;
    //
    // Save PMC IPC1 Bar1 (PcdPmcIpc1BaseAddress0) to GNVS and size is 0x2000(8KB).
    //
    PciBar0RegOffset = (UINT32) MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_PMC_IPC1, (PCI_FUNCTION_NUMBER_PMC_SSRAM), 0x10);
    DEBUG ((DEBUG_INFO, "  SSRAM MMIO Bar = %x \n ", MmioRead32 (PciBar0RegOffset) & 0xFFFFFFF0));
    GlobalNvsArea->Area->SSRAMBar0Address = MmioRead32 (PciBar0RegOffset) & 0xFFFFFFF0;
    GlobalNvsArea->Area->SSRAMBar0Length = 0x2000;

    Status = gRT->SetVariable (
                    L"SSRAMBASE",
                    &SSRAMBASEGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (UINT32),
                    &(GlobalNvsArea->Area->SSRAMBar0Address)
                    );

    SideBandAndThenOr32 (
      0xC6,
      0x0300 + 0x001C,
      0xFFFFFFFF,
      (BIT1 | BIT0)
      );

    SideBandAndThenOr32 (
      0xC6,
      0x0300 + 0x0038,
      0xFFFFFFFF,
      BIT0
      );

    //
    // Switch the PMC SSRAM to ACPI mode.
    //
    SideBandAndThenOr32 (
      0x95,
      0x208,
      0xFFFFFFFF,
      BIT0 | B_PMC_IOSF2OCP_PCICFGCTRL3_BAR1_DISABLE3
      );

    //
    // Program PMC ACPI IRQ.
    //
    Data32 = SideBandRead32 (0x95, 0x204);
    Data32 |= BIT1;
    Data32 |= 0x00028000;
    SideBandWrite32 (0x95, 0x204, Data32);

    Data32 = SideBandRead32 (0x95, 0x204);
    DEBUG ((DEBUG_INFO, "  PMC Interrupt value= %x \n ", Data32));

  }

  PciBar0RegOffset = (UINT32)MmPciAddress(0, DEFAULT_PCI_BUS_NUMBER_SC, 0, 0, 0x48);
  PciBar0RegOffset = (MmioRead32(PciBar0RegOffset) & 0xFFFF7000);
  GlobalNvsArea->Area->IPCBIOSMailBoxData = (UINT32)PciBar0RegOffset + 0x7000 + 0x80;
  GlobalNvsArea->Area->IPCBIOSMailBoxInterface = (UINT32)PciBar0RegOffset + 0x7000 + 0x84;
  DEBUG((DEBUG_INFO, "  BIOS MAIL-BOX Data= %x \n ", GlobalNvsArea->Area->IPCBIOSMailBoxData));
  DEBUG((DEBUG_INFO, "  BIOS MAIL-BOX Interface= %x \n ", GlobalNvsArea->Area->IPCBIOSMailBoxInterface));

}


VOID
EFIAPI
InitPciDevPME (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
#ifdef SATA_SUPPORT
  //
  // Program SATA PME_EN
  //
  SataPciCfg32Or (R_SATA_PMCS, B_SATA_PMCS_PMEE);
#endif
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

  @param[in]  Event     EFI Event Handle
  @param[in]  Context   Pointer to Notify Context

  @retval     None

**/
VOID
EFIAPI
EnableAcpiCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT8                  OutputValue;
  UINT32                 SmiEn;
  UINT16                 Pm1Cnt;
  UINT16                 AcpiBaseAddr;

  DEBUG ((DEBUG_INFO, "EnableAcpiCallback ++ (DXE)\n"));

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Disable SW SMI Timer
  //
  SmiEn = IoRead32 (AcpiBaseAddr + R_SMI_EN);
  SmiEn &= ~B_SMI_STS_SWSMI_TMR;
  IoWrite32 (AcpiBaseAddr + R_SMI_EN, SmiEn);

  //
  // Disable PM sources except power button
  //
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_EN, B_ACPI_PM1_EN_PWRBTN);

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
  Pm1Cnt = IoRead16 (AcpiBaseAddr + R_ACPI_PM1_CNT);
  Pm1Cnt |= B_ACPI_PM1_CNT_SCI_EN;
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, Pm1Cnt);
}

VOID
PlatformScInitBeforeBoot (
  VOID
  )
{
  //
  // Saved SPI Opcode menu to fix EFI variable unable to write after S3 resume.
  //
  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (SPI_BASE_ADDRESS + (R_SPI_OPMENU0)),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + (R_SPI_OPMENU0))
  );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (SPI_BASE_ADDRESS + (R_SPI_OPMENU1)),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + (R_SPI_OPMENU1))
  );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint16,
    (UINTN) (SPI_BASE_ADDRESS + R_SPI_OPTYPE),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + R_SPI_OPTYPE)
  );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint16,
    (UINTN) (SPI_BASE_ADDRESS + R_SPI_PREOP),
    1,
    (VOID *) (UINTN) (SPI_BASE_ADDRESS + R_SPI_PREOP)
  );

  return;
}


/**
  Saves UART2 Base Address To Restore on S3 Resume Flows

  @param[in]  Event     A pointer to the Event that triggered the callback.
  @param[in]  Context   A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ConfigureUart2OnReadyToBoot (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  EFI_STATUS              Status;
  UINTN                   Uart2BaseAddressRegister = 0;

  //
  // Save the UART2 BAR As It Will Need Restored Manually
  // On S3 Resume to Match the Value Written to the DBG2 ACPI Table
  //

  Uart2BaseAddressRegister = MmPciAddress (
                               0,
                               DEFAULT_PCI_BUS_NUMBER_SC,
                               PCI_DEVICE_NUMBER_LPSS_HSUART,
                               PCI_FUNCTION_NUMBER_LPSS_HSUART2,
                               R_LPSS_IO_BAR
                               );

  mSystemConfiguration.Uart2KernelDebugBaseAddress = (UINT32) (MmioRead32 (Uart2BaseAddressRegister) & B_LPSS_IO_BAR_BA);

  Status = gRT->SetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  mSetupVariableAttributes,
                  sizeof (SYSTEM_CONFIGURATION),
                  &mSystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);
}


#if (ENBDT_PF_ENABLE == 1) //BXTP
/**
  This function handles PlatformInit task at the end of DXE

  @param[in]  Event     The Event this notify function registered to.
  @param[in]  Context   Pointer to the context data registered to the
                        Event.

**/
VOID
EFIAPI
EndofDxeCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
}
#endif //BXTP


VOID
EFIAPI
ReadyToBootFunction (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  //
  // Switch SD Card detect pin from native to GPI so OS can detect card insertion
  // events.
  //
  DEBUG ((EFI_D_INFO, "%a(#%d) - Programming SD Card detect pin as GPI...\n", __FUNCTION__, __LINE__));

  GpioPadConfigTable (sizeof (SdcardGpio) / sizeof (SdcardGpio[0]), SdcardGpio);

  if (mPciLanInfo != NULL) {
    //
    // save LAN info to a variable
    //
    gRT->SetVariable (
           L"PciLanInfo",
           &gEfiPciLanInfoGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           mPciLanCount * sizeof (PCI_LAN_INFO),
           mPciLanInfo
           );

    gBS->FreePool (mPciLanInfo);
    mPciLanInfo = NULL;
  }

  //
  // Set RTC century in case RTC core driver failed to initialize it.
  //
  AdjustRtcCentury ();

  return;
}


/**
  Disable SecureBoot Configuration

**/
VOID
EFIAPI
DisableSecureBootCfg (
  VOID
  )
{
  EFI_STATUS                          Status;
  CHAR16                              *MfgTest = L"NULL";
  UINT8                               SecureBootCfg;
  UINTN                               DataSize;

  //
  // create MFGTEST with attribute of NV,BS & RT ,Default value = NULL , This parameter is reserved for future USE.
  //
  Status = gRT->SetVariable (
                  L"MFGTEST",
                  &gMfgModeVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (MfgTest),
                  MfgTest
                  );

  DataSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_ENABLE_NAME,
                  &gEfiSecureBootEnableDisableGuid,
                  NULL,
                  &DataSize,
                  &SecureBootCfg
                  );

  if (SecureBootCfg) {
    SecureBootCfg = 0x00; //Disable SecureBoot
    Status = gRT->SetVariable (
                    EFI_SECURE_BOOT_ENABLE_NAME,
                    &gEfiSecureBootEnableDisableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UINT8),
                    &SecureBootCfg
                    );

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
}


/**
  Initializes manufacturing and config mode setting.

**/
VOID
InitMfgAndConfigModeStateVar (
  VOID
  )
{
  EFI_PLATFORM_SETUP_ID           *BootModeBuffer;
  VOID                            *HobList;

  HobList = GetFirstGuidHob (&gEfiPlatformBootModeGuid);
  if (HobList != NULL) {
    BootModeBuffer = GET_GUID_HOB_DATA (HobList);
    //
    // Check if in Manufacturing mode
    //
    if (!CompareMem (&BootModeBuffer->SetupName, MANUFACTURE_SETUP_NAME, StrSize (MANUFACTURE_SETUP_NAME))) {
      mMfgMode = TRUE;
      DisableSecureBootCfg ();
    }
  }
}


/**
  Initializes platform boot mode setting.

**/
VOID
InitPlatformBootMode (
  VOID
  )
{
  EFI_PLATFORM_SETUP_ID           *BootModeBuffer;
  VOID                            *HobList;

  HobList = GetFirstGuidHob (&gEfiPlatformBootModeGuid);

  if (HobList != NULL) {
    BootModeBuffer = GET_GUID_HOB_DATA (HobList);
    mPlatformBootMode = BootModeBuffer->PlatformBootMode;
  }

}


VOID
InitPlatformUsbPolicy (
  VOID
  )
{
  EFI_HANDLE              Handle;
  EFI_STATUS              Status;

  Handle = NULL;

  mUsbPolicyData.Version                       = (UINT8) USB_POLICY_PROTOCOL_REVISION_2;
  mUsbPolicyData.UsbOperationMode              = HIGH_SPEED;
  mUsbPolicyData.LegacyKBEnable                = LEGACY_KB_EN;
  mUsbPolicyData.LegacyMSEnable                = LEGACY_MS_EN;
  mUsbPolicyData.LegacyUsbEnable               = mSystemConfiguration.UsbLegacy;
  mUsbPolicyData.CodeBase                      = ICBD_CODE_BASE;

  //
  // Some chipset need Period smi, 0 = LEGACY_PERIOD_UN_SUPP
  //
  mUsbPolicyData.USBPeriodSupport      = LEGACY_PERIOD_UN_SUPP;

  //
  // Some platform need legacy free, 0 = LEGACY_FREE_UN_SUPP
  //
  mUsbPolicyData.LegacyFreeSupport     = LEGACY_FREE_UN_SUPP;

  //
  // Set Code base , TIANO_CODE_BASE =0x01, ICBD =0x00
  //
  mUsbPolicyData.CodeBase    = (UINT8) ICBD_CODE_BASE;


  //
  // Set for reduce usb post time
  //
  mUsbPolicyData.UsbTimeTue               = 0x00;
  mUsbPolicyData.InternelHubExist         = 0x01;
  mUsbPolicyData.EnumWaitPortStableStall  = 100;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gUsbPolicyGuid,
                  EFI_NATIVE_INTERFACE,
                  &mUsbPolicyData
                  );
  ASSERT_EFI_ERROR (Status);

}


VOID
InitSeC (
  VOID
  )
{
  EFI_STATUS                Status;
  DXE_SEC_POLICY_PROTOCOL   *SeCPlatformPolicy;
  EFI_PEI_HOB_POINTERS      HobPtr;
  MBP_CURRENT_BOOT_MEDIA    *BootMediaData;
  SEC_OPERATION_PROTOCOL    *SeCOp;

  DEBUG ((DEBUG_INFO, "InitSeC  ++\n"));

  Status = gBS->LocateProtocol (&gDxePlatformSeCPolicyGuid, NULL, (VOID **) &SeCPlatformPolicy);

  if (EFI_ERROR (Status)) {
    return;
  }

  SeCPlatformPolicy->SeCConfig.EndOfPostEnabled = mSystemConfiguration.SeCEOPEnable;
  DEBUG ((DEBUG_INFO, "InitSeC mDxePlatformSeCPolicy->SeCConfig.EndOfPostEnabled %x %x\n", SeCPlatformPolicy->SeCConfig.EndOfPostEnabled,mSystemConfiguration.SeCEOPEnable));

  //
  // Get Boot Media Hob, save to SystemConfigData for next S3 cycle.
  //
  DEBUG ((DEBUG_INFO, "CSE Boot Device record is %x from SystemConfigData\n", mSystemConfiguration.CseBootDevice));
  HobPtr.Guid   = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  BootMediaData = (MBP_CURRENT_BOOT_MEDIA *) GET_GUID_HOB_DATA (HobPtr.Guid);

  if (mSystemConfiguration.CseBootDevice != BootMediaData->PhysicalData) {
      mSystemConfiguration.CseBootDevice= (UINT8) BootMediaData->PhysicalData;
      DEBUG ((DEBUG_INFO, "Current CSE Boot Device is %x. Update to SystemConfigData\n", BootMediaData->PhysicalData));

      Status = gRT->SetVariable (
                      L"Setup",
                      &gEfiSetupVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
                      sizeof (SYSTEM_CONFIGURATION),
                      &mSystemConfiguration
                      );
  }

  Status = gBS->LocateProtocol (
                  &gEfiSeCOperationProtocolGuid,
                  NULL,
                  (VOID **) &SeCOp
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = SeCOp->PerformSeCOperation (SEC_OP_CHECK_UNCONFIG);
  Status = SeCOp->PerformSeCOperation (SEC_OP_CHECK_HMRFPO);

}


VOID
InitPlatformResolution (
  VOID
  )
{
  PANEL_RESOLUTION PanelResolution[] = {
    {0, 0,     0},     // Native Mode, Find the highest resolution which GOP supports.
    {1, 640,   480},
    {2, 800,   600},
    {3, 1024,  768},
    {4, 1280,  1024},
    {5, 1366,  768},
    {6, 1680,  1050},
    {7, 1920,  1200},
    {8, 1280,  800}
  };

  //
  // Set the Panel Resolution.
  //
  PcdSet32S (PcdVideoHorizontalResolution, PanelResolution[mSystemConfiguration.IgdFlatPanel].HorizontalResolution);
  PcdSet32S (PcdVideoVerticalResolution, PanelResolution[mSystemConfiguration.IgdFlatPanel].VerticalResolution);
}

VOID
OverrideSdCardPresence (
  VOID
  )
{
  UINT32 PciP2sbBar0RegOffset;
  UINT32 P2sbMmioBar;
  UINT32 Gpio177PadConfigDW0RegAdd;
  UINT32 Gpio177RxState;

  PciP2sbBar0RegOffset = (UINT32) MmPciAddress (
                                    0,
                                    DEFAULT_PCI_BUS_NUMBER_SC,
                                    PCI_DEVICE_NUMBER_P2SB,
                                    PCI_FUNCTION_NUMBER_P2SB,
                                    R_P2SB_BASE
                                    );
  //
  // Read back P2SB MMIO BAR Base Addr
  //
  P2sbMmioBar = MmioRead32 (PciP2sbBar0RegOffset);
  if (P2sbMmioBar == 0xFFFFFFFF) {
    //
    // P2SB has been hidden, read it from Pcd
    //
    P2sbMmioBar = PcdGet32 (PcdP2SBBaseAddress);
  } else {
    P2sbMmioBar &= B_P2SB_BAR_BA;
  }

  Gpio177PadConfigDW0RegAdd = P2SB_MMIO_ADDR (P2sbMmioBar, SOUTHWEST, 0x5D0);
  Gpio177RxState = MmioRead32(Gpio177PadConfigDW0RegAdd) & BIT1;
  DEBUG ((DEBUG_INFO, "Gpio177PadConfigDW0RegAdd: 0x%X\n", Gpio177PadConfigDW0RegAdd));
  DEBUG ((DEBUG_INFO, "Gpio177RxState: 0x%X\n", Gpio177RxState));

  if (Gpio177RxState == 0x00) {
    SideBandAndThenOr32 (
      0xD6,
      0x0600 + 0x08,
      0xFFFFFFFF,
      BIT5
      );
  };
}

/**
  This is the standard EFI driver point for the Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param[in]  ImageHandle     Handle for the image of this driver.
  @param[in]  SystemTable     Pointer to the EFI System Table.

  @retval     EFI_SUCCESS     Policy decisions set.

**/
EFI_STATUS
EFIAPI
InitializePlatform (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  UINTN                               VarSize;
  EFI_EVENT                           EfiExitBootServicesEvent;
  EFI_HOB_GUID_TYPE                   *FdoEnabledGuidHob = NULL;
  EFI_PLATFORM_INFO_HOB               *PlatformInfoHob;
  EFI_PEI_HOB_POINTERS                Hob;
  EFI_EVENT                           EfiShellEvent = NULL;
  VOID                                *mEfiShellProtocolRegistration;

  mImageHandle = ImageHandle;

  VarSize = sizeof (EFI_PLATFORM_INFO_HOB);
  Status = gRT->GetVariable(
                  L"PlatformInfo",
                  &gEfiBxtVariableGuid,
                  NULL,
                  &VarSize,
                  &mPlatformInfo
                  );

  InitMfgAndConfigModeStateVar (); // Initialize Product Board ID variable
  InitPlatformBootMode ();

  VarSize = 0;
  Status = gRT->GetVariable (
                  L"db",
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &VarSize,
                  NULL
                  );

  VarSize = 0;
  Status = gRT->GetVariable (
                  L"dbx",
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &VarSize,
                  NULL
                  );


  VarSize = 0;
  Status = gRT->GetVariable (
                  L"KEK",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  NULL
                  );

  VarSize = 0;
  Status = gRT->GetVariable (
                  L"PK",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  NULL
                  );

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  &mSetupVariableAttributes,
                  &VarSize,
                  &mSystemConfiguration
                  );

  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfoHob = GET_GUID_HOB_DATA (Hob.Raw);

  mSystemConfiguration.BoardId = PlatformInfoHob->BoardId;

  if (mSystemConfiguration.PlatformSettingEn == 0) {

    mSystemConfiguration.PlatformSettingEn = 1;
  }

  mSystemConfiguration.PmicSetupDefault = 1;
  mSystemConfiguration.MaxPkgCState = PcdGet8 (PcdMaxPkgCState);


  Status = gRT->SetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  mSetupVariableAttributes,
                  VarSize,
                  &mSystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ReadyToBootFunction,
             NULL,
             &mReadyToBootEvent
             );

  //
  // Create a ReadyToBoot Event to run the PME init process
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             InitPciDevPME,
             NULL,
             &mReadyToBootEvent
             );

  //
  // Configure UART2 at ReadyToBoot if kernel debug is enabled (setup for S3 resume)
  //
  if (GetBxtSeries () == BxtP && mSystemConfiguration.OsDbgEnable) {
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               ConfigureUart2OnReadyToBoot,
               NULL,
               &mReadyToBootEvent
               );
  }

  //
  // Initialize BXT Platform Policy
  // SiPolicy is consumed by HSTI Silicon driver
  //
  InitSiPolicy ();

  ReportStatusCodeEx (
    EFI_PROGRESS_CODE,
    EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_DXE_NB_INIT,
    0,
    &gEfiCallerIdGuid,
    NULL,
    NULL,
    0
  );
  InitPlatformSaPolicy (&mSystemConfiguration);

  //
  // Dis-arm RTC Alarm Interrupt
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, IoRead8 (PCAT_RTC_DATA_REGISTER) & ~B_RTC_ALARM_INT_ENABLE);

  InitPlatformIdePolicy ();
  InitPlatformUsbPolicy ();
  InitSioPlatformPolicy ();
  InitializeClockRouting ();
  InitTcoReset ();

  InitPlatformResolution ();
  //
  // Install PCI Bus Driver Hook
  //
  PciBusDriverHook ();
  PlatformScInitBeforeBoot ();
  InitSeC ();

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);
  if (FdoEnabledGuidHob != NULL) {
  }

#if (ENBDT_PF_ENABLE == 1) //BXTP

#ifdef SENSOR_INFO_VAR_SUPPORT
  InitializeSensorInfoVariable (); // Initialize Sensor Info variable
#endif
  //
  // Performing PlatformInitEndOfDxe after the gEfiEndOfDxeEventGroup is signaled.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EndofDxeCallback,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &mEndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);
#endif //  #if (ENBDT_PF_ENABLE == 1) //BXTP

  //
  // IPC1 and SRAM update before boot
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PMCReadyToBoot,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &EfiExitBootServicesEvent
                  );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  EnableAcpiCallback,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &EfiExitBootServicesEvent
                  );


  OverrideSdCardPresence();

  //
  //QR code showing under EFI Shell
  //
  if (PcdGetBool(PcdLogoDisplay)) {
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EfiShellProtocolCallback,
                  NULL,
                  &EfiShellEvent
                  );
  Status = gBS->RegisterProtocolNotify (
                  &gEfiShellProtocolGuid,
                  EfiShellEvent,
                  &mEfiShellProtocolRegistration
                  );
  }
  return EFI_SUCCESS;
}

