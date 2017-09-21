/** @file
  Do platform specific PEI stage initializations.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformInit.h"
#include <Library/SteppingLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/PttPTPInstanceGuid.h>
#include <Ppi/Smbus2.h>
#include <Library/PcdLib.h>
#include <Library/HeciMsgLib.h>
#include <Ppi/SeCUma.h>

#ifdef __GNUC__
  #pragma GCC push_options
  #pragma GCC optimize ("O0")
#else
  #pragma optimize ("", off)
#endif


static EFI_PEI_RESET_PPI            mResetPpi = { IchReset };

static EFI_PEI_PPI_DESCRIPTOR       mBoardPostMemInitStartPpi[] = {
  {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gBoardPostMemInitStartGuid,
  NULL
  }
};

static EFI_PEI_PPI_DESCRIPTOR       mPpiList[] = {

  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiPeiMasterBootModePpiGuid,
    NULL
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiTpmDeviceSelectedGuid,
    NULL
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiResetPpiGuid,
    &mResetPpi
  }
};

static EFI_PEI_NOTIFY_DESCRIPTOR    mNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    EndOfPeiPpiNotifyCallback
  }
};

EFI_STATUS
TpmPolicyInit (
  IN SYSTEM_CONFIGURATION     *SystemConfiguration
  )
{
#if FTPM_SUPPORT
  EFI_STATUS                  Status;
  UINTN                       Size;
  BOOLEAN                     IsPttEnabled = TRUE;
  MBP_ME_FW_CAPS              Mbp_Me_Fw_Caps = { 0 };

  DEBUG ((EFI_D_ERROR, "TpmPolicyInit entry.\n"));

  //
  // Get ME FW Capability from MBP table to determine PTT State
  //
  Status = HeciGetMeFwCapability (&Mbp_Me_Fw_Caps);
  if (!EFI_ERROR (Status)) {
    IsPttEnabled = (BOOLEAN) Mbp_Me_Fw_Caps.CurrentFeatures.Fields.PTT;
  }

  if ((IsPttEnabled) && (SystemConfiguration->TPM == TPM_PTT)) {
    if (SystemConfiguration->PttSuppressCommandSend == 1) {
      PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceNoneGuid);
      DEBUG ((DEBUG_ERROR, "BIOS will send no further commands to PTT.\n"));
    } else {
      //
      // Set PcdTpmInstanceGuid to PTT
      //
      Size = sizeof (gTpmDeviceInstanceTpm20PttPtpGuid);
      PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gTpmDeviceInstanceTpm20PttPtpGuid);
      DEBUG ((DEBUG_INFO, "Set PcdTpmInstanceGuid to PTT.\n"));
    }
  } else if (SystemConfiguration->TPM == TPM_DTPM_1_2) {
    //
    // Set PcdTpmInstanceGuid to dTPM 1.2
    //
    Size = sizeof (gEfiTpmDeviceInstanceTpm12Guid);
    PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceTpm12Guid);
    DEBUG ((DEBUG_INFO, "Set PcdTpmInstanceGuid to dTPM 1.2.\n"));
  } else if (SystemConfiguration->TPM == TPM_DTPM_2_0) {
    //
    // Set PcdTpmInstanceGuid to dTPM 2.0
    //
    Size = sizeof (gEfiTpmDeviceInstanceTpm20DtpmGuid);
    PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceTpm20DtpmGuid);
    DEBUG ((DEBUG_INFO, "Set PcdTpmInstanceGuid to dTPM 2.0.\n"));
  } else {
    //
    // Set PcdTpmInstanceGuid to NONE
    //
    Size = sizeof (gEfiTpmDeviceInstanceNoneGuid);
    PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceNoneGuid);
    DEBUG ((DEBUG_INFO, "Set PcdTpmInstanceGuid to NONE.\n"));
  }
#endif

  return EFI_SUCCESS;
}


/**
  This function will power off the system at once.

**/
VOID
PowerOffNow (
  VOID
  )
{
  UINT16  AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);
  DEBUG ((DEBUG_INFO, " PowerOffNow: AcpiBaseAddr = 0x%x\n",AcpiBaseAddr));

  //
  // Set PM Register and Put system to S5
  //
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, V_ACPI_PM1_CNT_S5);
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, V_ACPI_PM1_CNT_S5 + B_ACPI_PM1_CNT_SLP_EN);
}


/**
  This function gets system config.

  @param[in]  PeiServices             An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in]  SystemConfiguration     The pointer to get System Setup


  @retval     EFI_SUCCESS             The system configuration is obtained
  @retval     Other values            The system configuration cannot be obtained

**/
EFI_STATUS
GetSystemConfig (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  OUT      SYSTEM_CONFIGURATION   *SystemConfiguration
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
PWM_Fan_Start (
  VOID
  )
{
  PWMCTRL    PwmCtl = {0};
  UINT32     TimeOut = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  //
  // Program GPIO_37 to enable FAN device
  //
  GpioPadWrite(N_GPIO_37,0x400);

  //
  // Set PWM Base Address
  //
  MmioWrite32 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_PMC_PWM, PCI_FUNCTION_NUMBER_PMC_PWM, R_LPSS_IO_BAR),
    PWM_TEMP_MEM_BASE_ADDRESS
    );

  //
  // Enable PWM Memory decode
  //
  MmioWrite16 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_PMC_PWM, PCI_FUNCTION_NUMBER_PMC_PWM, R_LPSS_IO_STSCMD),
    B_LPSS_IO_STSCMD_BME| B_LPSS_IO_STSCMD_MSE
    );

  MmioWrite32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00, PwmCtl.Raw);

  //
  // Initial Enable or First Activation for PWM
  // 1. Program the Base Unit and On Time Divisor values.
  // 2. Set the Software Update Bit.
  // 3. Enable the PWM Output by setting PWM Enable.
  // 4. Repeat the above steps for the next PWM Module.
  //
  PwmCtl.BITS.Pwm_Base_Unit = 0x15;
  PwmCtl.BITS.Pwm_On_Time_Divisor = 0x80;
  MmioWrite32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00, PwmCtl.Raw);

  PwmCtl.Raw = MmioRead32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00);
  PwmCtl.BITS.Pwm_Sw_Update = 1;
  MmioWrite32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00, PwmCtl.Raw);

  PwmCtl.Raw = MmioRead32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00);
  while (((PwmCtl.Raw & BIT30) == BIT30) && (TimeOut < PWM_TIMEOUT_MAX)) {
    MicroSecondDelay (1);
    PwmCtl.Raw = MmioRead32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00);
    TimeOut ++;
  }

  PwmCtl.Raw = MmioRead32 (PWM_TEMP_MEM_BASE_ADDRESS + 0x0C00);
  PwmCtl.BITS.Pwm_Enable = 1;
  MmioWrite32 (PWM_TEMP_MEM_BASE_ADDRESS+0x0C00, PwmCtl.Raw);

  if (TimeOut > PWM_TIMEOUT_MAX) {
    Status = EFI_TIMEOUT;
  }
  return Status;
}


EFI_STATUS
BXTPolicyInit (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN SYSTEM_CONFIGURATION         *SystemConfiguration
  )
{
  VOID*                           Buffer;
  UINT32                          Size;
  EFI_GUID                        PeiLogoGuid        = { 0x7BB28B99, 0x61BB, 0x11D5, {0x9A, 0x5D, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D} };
  EFI_GUID                        TianmaVbtGuid      = { 0xE08CA6D5, 0x8D02, 0x43ae, {0xAB, 0xB1, 0x95, 0x2C, 0xC7, 0x87, 0xC9, 0x33} };
  VBT_INFO                        VbtInfo;


  DEBUG ((DEBUG_INFO, " BXTPolicyInit: SystemAgent PEI Platform Policy Initialization begin \n"));


  PeiGetSectionFromFv (PeiLogoGuid, &Buffer, &Size);
  if (Buffer == NULL) {
    DEBUG (( DEBUG_ERROR, "Could not locate PeiLogo"));
  }

  //
  // May need a different VBT depending on PanelSel
  //
  PeiGetSectionFromFv (TianmaVbtGuid, &Buffer, &Size);

  if (Buffer == NULL) {
    DEBUG (( DEBUG_ERROR, "Could not locate VBT"));
  }


  //
  // Build the VBT data into HOB for DXE GOP
  //
  VbtInfo.VbtAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)Buffer;
  VbtInfo.VbtSize = Size;
  DEBUG ((DEBUG_INFO, "VbtInfo VbtAddress is 0x%x\n", Buffer));
  DEBUG ((DEBUG_INFO, "VbtInfo VbtSize is 0x%x\n", Size));

  BuildGuidDataHob (
    &gVbtInfoGuid,
    &VbtInfo,
    sizeof (VbtInfo)
    );

  DEBUG ((DEBUG_INFO, " SystemAgent PEI Platform Policy Initialization Done \n"));

  return EFI_SUCCESS;
}

VOID
ConfigurePmicIMON (
  VOID
  )
{
  UINTN   PciD0F0RegBase = 0;
  UINTN   MchBar = 0;
  UINT32  Data;
  UINT16  StallCount;
  UINT64  PkgPwrSKU;

  PciD0F0RegBase  = MmPciAddress (0,SA_MC_BUS,SA_MC_DEV,SA_MC_FUN,0);
  MchBar          = MmioRead32 (PciD0F0RegBase + R_SA_MCHBAR_REG) &~BIT0;
  PkgPwrSKU       = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);

  StallCount = 0;
  while (StallCount < 1000) {
    Data = MmioRead32 (MchBar + R_BIOS_MAILBOX_INTERFACE); 
    if ((Data & BIT31) == BIT31) {
      MicroSecondDelay (1);
    } else {
      break;
    }
    StallCount++;
  }
  MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_DATA), 0xfa0d04a4);
  MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_INTERFACE), 0x8000011d);

  StallCount = 0;
  while (StallCount < 1000) {
    Data = MmioRead32 (MchBar + R_BIOS_MAILBOX_INTERFACE);
    if ((Data & BIT31) == BIT31) {
      MicroSecondDelay (1);
    } else {
      break;
    }
    StallCount++;
  }

  if ((PkgPwrSKU & 0x07FFF) >= 0x0903){
    MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_DATA), 0xe8330466);
    MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_INTERFACE), 0x8000001d);
  } else { 
    MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_DATA), 0xed3303b3);
    MmioWrite32 ( (MchBar + R_BIOS_MAILBOX_INTERFACE), 0x8000001d);
  }
}

/**
  Platform Init PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database

**/
EFI_STATUS
EFIAPI
PlatformInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  SYSTEM_CONFIGURATION        SystemConfiguration;
  EFI_PLATFORM_INFO_HOB       *PlatformInfo;
  EFI_PEI_HOB_POINTERS        Hob;
  BOARD_POST_MEM_INIT_FUNC    BoardPostMemInitFunc;

  DEBUG ((EFI_D_INFO, "PlatformInit EntryPoint\n"));
  //
  // Trigger board post mem init
  //
  Status = PeiServicesInstallPpi (mBoardPostMemInitStartPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Set the some PCI and chipset range as UC
  // And align to 1M at leaset
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  if (PlatformInfo->SsidSvid != 0) {
    PlatformInfo->SsidSvid = 0;
  }


  //
  // Initialize GFX VT-d Base Address
  //
  if (BxtStepping () <= BxtA1) {
    PcdSet32S (PcdVtdGfxBaseAddress, 0xFF000000);
  }

  PWM_Fan_Start ();
  
  ConfigurePmicIMON();

  //
  // Initialize PlatformInfo HOB
  //
  BoardPostMemInitFunc = (BOARD_POST_MEM_INIT_FUNC) (UINTN) PcdGet64 (PcdBoardPostMemInitFunc);
  if (BoardPostMemInitFunc != NULL) {
    BoardPostMemInitFunc (PeiServices, PlatformInfo);
  }
  // MultiPlatformInfoInit(PeiServices, PlatformInfo);

  //
  // Set the new boot mode
  //
  Status = UpdateBootMode (PeiServices, PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  SetPlatformBootMode (PeiServices, PlatformInfo);

  GetSystemConfig (PeiServices, &SystemConfiguration);

  //
  //Update the PlatformInfo Hob according to setup
  //
  PlatformInfo->OsSelection = 3;

  //
  // Initialize all PEI Policy
  //
  PeiPolicyInit (PeiServices, &SystemConfiguration);

  //
  // Do basic SC init
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_PEI_CAR_SB_INIT));
  Status = PlatformScInit (&SystemConfiguration, PeiServices, PlatformInfo->PlatformType);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize BxtPolicy PPI
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_COMPUTING_UNIT_CHIPSET | EFI_CHIPSET_PC_PEI_CAR_NB_INIT));
  Status = BXTPolicyInit (PeiServices, &SystemConfiguration);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize TPM Policy before Initializing Platform PPIs - Tcg2Pei load depends on gEfiTpmDeviceSelectedGuid which will be installed as a part of Platform PPIs
  //
  Status = TpmPolicyInit (&SystemConfiguration);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize platform PPIs
  //
  Status = PeiServicesInstallPpi (&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Set LVDS_BKLT_CTRL to 50%.
  //
  MmPci8 (0, 0, 2, 0, 0xF4) = 128;

  //
  // Initialize platform PPIs
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  //
  // Maybe move to MemoryCallBack.c (which would be called between PreMemPlatInit and PlatInit)
  //
  PlatformInitFinalConfig (PeiServices);

  return Status;
}


EFI_STATUS
EFIAPI
CpuOnlyReset (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
{

#ifdef __GNUC__
  __asm__
  (
   "xorl %ecx, %ecx\n"
   "1:hlt; hlt; hlt\n"
   "jmp 1b\n"
  );
#else
  _asm {
    xor   ecx, ecx
  HltLoop:
    hlt
    hlt
    hlt
    loop  HltLoop
  }
#endif

  //
  // If we get here we need to mark it as a failure.
  //
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32               *Size
  )
{
  EFI_STATUS  Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI          *FvPpi;
  EFI_FV_FILE_INFO                     FvFileInfo;
  PEI_CORE_INSTANCE                    *PrivateData;
  UINTN                                CurrentFv;
  PEI_CORE_FV_HANDLE                   *CoreFvHandle;
  EFI_PEI_FILE_HANDLE                  VbtFileHandle;
  EFI_GUID                             *VbtGuid;
  EFI_COMMON_SECTION_HEADER            *Section;
  CONST EFI_PEI_SERVICES               **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS (PeiServices);

  Status = PeiServicesLocatePpi (
             &gEfiFirmwareFileSystem2Guid,
             0,
             NULL,
             (VOID **) &FvPpi
             );
  ASSERT_EFI_ERROR (Status);

  CurrentFv = PrivateData->CurrentPeimFvCount;
  CoreFvHandle = &(PrivateData->Fv[CurrentFv]);

  Status = FvPpi->FindFileByName (FvPpi, &NameGuid, &CoreFvHandle->FvHandle, &VbtFileHandle);
  if (!EFI_ERROR (Status) && VbtFileHandle != NULL) {

    DEBUG ((DEBUG_INFO, "Find SectionByType \n"));

    Status = FvPpi->FindSectionByType (FvPpi, EFI_SECTION_RAW, VbtFileHandle, (VOID **) &VbtGuid);
    if (!EFI_ERROR (Status)) {

      DEBUG ((DEBUG_INFO, "GetFileInfo \n"));

      Status = FvPpi->GetFileInfo (FvPpi, VbtFileHandle, &FvFileInfo);
      Section = (EFI_COMMON_SECTION_HEADER *)FvFileInfo.Buffer;

      if (IS_SECTION2 (Section)) {
        ASSERT (SECTION2_SIZE (Section) > 0x00FFFFFF);
        *Size = SECTION2_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
        *Address = ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER2));
      } else {
        *Size = SECTION_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER);
        *Address = ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER));
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Install Firmware Volume Hob's once there is main memory

  @param[in] PeiServices      General purpose services available to every PEIM.
  @param[in] NotifyDescriptor Not uesed.
  @param[in] Ppi              Not uesed.

  @retval    EFI_SUCCESS      If the interface could be successfully installed.

**/
EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_PLATFORM_INFO_HOB            *PlatformInfo;
  EFI_PEI_HOB_POINTERS             Hob;

  DEBUG ((EFI_D_INFO, "Platform Init End of PEI signal\n"));

  //
  // Set the some PCI and chipset range as UC
  // And align to 1M at leaset
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);


  DEBUG ((EFI_D_INFO, "Memory TOLM: %X\n", PlatformInfo->MemData.MemTolm));
  DEBUG ((EFI_D_INFO, "PCIE OSBASE: %lX\n", PlatformInfo->PciData.PciExpressBase));
  DEBUG (
    (EFI_D_INFO,
    "PCIE   BASE: %lX     Size : %X\n",
    PlatformInfo->PciData.PciExpressBase,
    PlatformInfo->PciData.PciExpressSize)
    );
  DEBUG (
    (EFI_D_INFO,
    "PCI32  BASE: %X     Limit: %X\n",
    PlatformInfo->PciData.PciResourceMem32Base,
    PlatformInfo->PciData.PciResourceMem32Limit)
    );
  DEBUG (
    (EFI_D_INFO,
    "PCI64  BASE: %lX     Limit: %lX\n",
    PlatformInfo->PciData.PciResourceMem64Base,
    PlatformInfo->PciData.PciResourceMem64Limit)
    );
  DEBUG ((EFI_D_INFO, "UC    START: %lX     End  : %lX\n", PlatformInfo->MemData.MemMir0, PlatformInfo->MemData.MemMir1));

  return EFI_SUCCESS;
}


/**
  Install Firmware Volume Hob's once there is main memory

  @param[in] PeiServices      General purpose services available to every PEIM.
  @param[in] NotifyDescriptor Notify that this module published.
  @param[in] Ppi              PPI that was installed.

  @retval    EFI_SUCCESS      The function completed successfully.

**/
EFI_STATUS
EFIAPI
PlatformInitFinalConfig (
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                  Status;
  EFI_BOOT_MODE               BootMode;
  EFI_CPUID_REGISTER          FeatureInfo;
  UINT8                       CpuAddressWidth;
  UINT16                      Pm1Cnt;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_PLATFORM_INFO_HOB       *PlatformInfo;
  UINT16                      AcpiBaseAddr;

  //
  // Get Platform Info HOB
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **) PeiServices, &BootMode);

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Check if user wants to turn off in PEI phase
  //
  if ((BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_FLASH_UPDATE)) {
    CheckPowerOffNow ();
  } else {
    Pm1Cnt  = IoRead16 (AcpiBaseAddr + R_ACPI_PM1_CNT);
    Pm1Cnt &= ~B_ACPI_PM1_CNT_SLP_TYP;
    IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, Pm1Cnt);
  }

  //
  //  Pulish memory tyoe info
  //
  PublishMemoryTypeInfo ();

  //
  // Work done if on a S3 resume
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  //
  // Add HOP entries for reserved MMIO ranges so that DXE will know about them.
  // Note: this really only need to be done for addresses that are outside the upper 16MB.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PMC_BASE_ADDRESS,
    0x1000
    );
  DEBUG ((EFI_D_INFO, "PmcBase            : 0x%x\n", PMC_BASE_ADDRESS));

  //
  // Spi BAR needs to be set to SPI_BASE_ADDRESS before it can be read..
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    SPI_BASE_ADDRESS,
    0x1000
    );
  DEBUG ((EFI_D_INFO, "SpiBase            : 0x%x\n", SPI_BASE_ADDRESS));

  //
  // Local APIC
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    LOCAL_APIC_BASE_ADDRESS,
    0x1000
  );
  DEBUG ((EFI_D_INFO, "LOCAL_APIC_BASE_ADDRESS : 0x%x\n", LOCAL_APIC_BASE_ADDRESS));

  //
  // IO APIC
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    IO_APIC_BASE_ADDRESS,
    0x1000
  );
  DEBUG ((EFI_D_INFO, "IO_APIC_ADDRESS    : 0x%x\n", IO_APIC_BASE_ADDRESS));

  //
  // Adding the PCIE Express area to the memory table as type 2 memory.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PlatformInfo->PciData.PciExpressBase,
    PlatformInfo->PciData.PciExpressSize
    );
  DEBUG ((EFI_D_INFO, "PciExpressBase     : 0x%x\n", PlatformInfo->PciData.PciExpressBase));

  //
  // Adding the Flashpart to the memory table as type 2 memory.
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_FIRMWARE_DEVICE,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdFlashBaseAddress),
    PcdGet32(PcdFlashSize)
    );
  DEBUG ((EFI_D_INFO, "FLASH_BASE_ADDRESS : 0x%x\n", PcdGet32(PcdFlashBaseAddress)));

  //
  // P2SB (BXT) 16MB
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdP2SBBaseAddress),
    0x1000000
  );

  //
  // PMC IPC (BXT) 8KB and 4KB
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdPmcIpc1BaseAddress0),
    0x2000
  );

    BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdPmcIpc1BaseAddress1),
    0x1000
  );

  //
  // PMC SSRAM (BXT) 8KB and 4KB
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdPmcSsramBaseAddress0),
    0x2000
    );
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    PcdGet32(PcdPmcSsramBaseAddress1),
    0x1000
    );

  //
  // Create a CPU hand-off information
  //
  CpuAddressWidth = 32;
  AsmCpuid (EFI_CPUID_EXTENDED_FUNCTION, &FeatureInfo.RegEax, &FeatureInfo.RegEbx, &FeatureInfo.RegEcx, &FeatureInfo.RegEdx);
  if (FeatureInfo.RegEax >= EFI_CPUID_VIRT_PHYS_ADDRESS_SIZE) {
    AsmCpuid (EFI_CPUID_VIRT_PHYS_ADDRESS_SIZE, &FeatureInfo.RegEax, &FeatureInfo.RegEbx, &FeatureInfo.RegEcx, &FeatureInfo.RegEdx);
    CpuAddressWidth = (UINT8) (FeatureInfo.RegEax & 0xFF);
  }

  BuildCpuHob (CpuAddressWidth, 16);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  )
{
  UINT16  *Ptr;
  UINT16  HeaderLength;
  UINT16  Checksum;

  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if ((FwVolHeader->Revision != EFI_FVH_REVISION) ||
      (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
      (FwVolHeader->FvLength == ((UINT64) -1)) ||
      ((FwVolHeader->HeaderLength & 0x01) != 0)
      ) {
    return EFI_NOT_FOUND;
  }
  //
  // Verify the header checksum
  //
  HeaderLength  = (UINT16) (FwVolHeader->HeaderLength / 2);
  Ptr           = (UINT16 *) FwVolHeader;
  Checksum      = 0;
  while (HeaderLength > 0) {
    Checksum = *Ptr++;
    HeaderLength--;
  }

  if (Checksum != 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

#ifdef __GNUC__
#pragma GCC pop_options
#else
#pragma optimize ("", on)
#endif

