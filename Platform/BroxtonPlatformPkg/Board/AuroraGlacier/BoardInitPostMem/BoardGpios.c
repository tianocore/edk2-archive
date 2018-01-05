/** @file
  Gpio setting for multiplatform.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardGpios.h"
#include <Library/GpioLib.h>
#include <Library/SteppingLib.h>


/**
  Returns the Correct GPIO table for Mobile/Desktop respectively.
  Before call it, make sure PlatformInfoHob->BoardId&PlatformFlavor is get correctly.

  @param[in] PeiServices         General purpose services available to every PEIM.
  @param[in] PlatformInfoHob     PlatformInfoHob pointer with PlatformFlavor specified.

  @retval    EFI_SUCCESS         The function completed successfully.
  @retval    EFI_DEVICE_ERROR    KSC fails to respond.

**/
EFI_STATUS
AuroraMultiPlatformGpioTableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  )
{
  DEBUG ((DEBUG_INFO, "AuroraMultiPlatformGpioTableInit()...\n"));
  DEBUG ((DEBUG_INFO, "PlatformInfoHob->BoardId: 0x%02X\n", PlatformInfoHob->BoardId));

  //
  // Select/modify the GPIO initialization data based on the Board ID.
  //
  switch (PlatformInfoHob->BoardId) {
    case BOARD_ID_AURORA:
      PlatformInfoHob->PlatformGpioSetting_SW = &mAuroraGpioInitData_SW[0];
      PlatformInfoHob->PlatformGpioSetting_W = &mAuroraGpioInitData_W[0];
      PlatformInfoHob->PlatformGpioSetting_NW = &mAuroraGpioInitData_NW[0];
      PlatformInfoHob->PlatformGpioSetting_N = &mAuroraGpioInitData_N[0];
      break;
    default:
      PlatformInfoHob->PlatformGpioSetting_SW = &mAuroraGpioInitData_SW[0];
      PlatformInfoHob->PlatformGpioSetting_W = &mAuroraGpioInitData_W[0];
      PlatformInfoHob->PlatformGpioSetting_NW = &mAuroraGpioInitData_NW[0];
      PlatformInfoHob->PlatformGpioSetting_N = &mAuroraGpioInitData_N[0];
      break;
  }

  return EFI_SUCCESS;
}


/**
  Set GPIO Lock for security.

**/
VOID
AuroraSetGpioPadCfgLock (
  VOID
  )
{
  UINT32 Data32;

  Data32 = 0;

  //
  // JTAG
  //
  GpioLockPadCfg (N_TCK);
  GpioLockPadCfg (N_TRST_B);
  GpioLockPadCfg (N_TMS);
  GpioLockPadCfg (N_TDI);
  GpioLockPadCfg (N_TDO);

  //
  // Power
  //
  GpioLockPadCfg (NW_PMIC_THERMTRIP_B);
  GpioLockPadCfg (NW_PROCHOT_B);

  //
  // Touch
  //
  GpioLockPadCfg (NW_GPIO_118);
  GpioLockPadCfg (NW_GPIO_119);
  GpioLockPadCfg (NW_GPIO_120);
  GpioLockPadCfg (NW_GPIO_121);
  GpioLockPadCfg (NW_GPIO_122);
  GpioLockPadCfg (NW_GPIO_123);

  //
  // SPI
  //
  GpioLockPadCfg (NW_GPIO_97);
  GpioLockPadCfg (NW_GPIO_98);
  GpioLockPadCfg (NW_GPIO_99);
  GpioLockPadCfg (NW_GPIO_100);
  GpioLockPadCfg (NW_GPIO_101);
  GpioLockPadCfg (NW_GPIO_102);
  GpioLockPadCfg (NW_GPIO_103);
  GpioLockPadCfg (NW_FST_SPI_CLK_FB);

  //
  // SMBus
  // Set SMBus GPIO PAD_CFG.PADRSTCFG to Powergood
  //
  Data32 = GpioPadRead (SW_SMB_ALERTB);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_ALERTB, Data32);

  Data32 = GpioPadRead (SW_SMB_CLK);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_CLK, Data32);

  Data32 = GpioPadRead (SW_SMB_DATA);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_DATA, Data32);

  GpioLockPadCfg (SW_SMB_ALERTB);
  GpioLockPadCfg (SW_SMB_CLK);
  GpioLockPadCfg (SW_SMB_DATA);
}


/**
  Returns the Correct GPIO table for Mobile/Desktop respectively.
  Before call it, make sure PlatformInfoHob->BoardId&PlatformFlavor is get correctly.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] PlatformInfoHob      PlatformInfoHob pointer with PlatformFlavor specified.

  @retval    EFI_SUCCESS          The function completed successfully.
  @retval    EFI_DEVICE_ERROR     KSC fails to respond.

**/
EFI_STATUS
AuroraMultiPlatformGpioProgram (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  )
{
  UINTN                             VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  SYSTEM_CONFIGURATION              SystemConfiguration;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  ZeroMem (&SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));

 (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gEfiPeiReadOnlyVariable2PpiGuid,
                    0,
                    NULL,
                    (VOID **) &VariableServices
                     );

  VariableServices->GetVariable (
                      VariableServices,
                      PLATFORM_SETUP_VARIABLE_NAME,
                      &gEfiSetupVariableGuid,
                      NULL,
                      &VariableSize,
                      &SystemConfiguration
                      );

  DEBUG ((DEBUG_INFO, "AuroraMultiPlatformGpioProgram()...\n"));

  switch (PlatformInfoHob->BoardId) {
    case BOARD_ID_AURORA:
      //
      // PAD programming
      //
      DEBUG ((DEBUG_INFO, "PAD programming, Board ID: 0x%X\n", PlatformInfoHob->BoardId));
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_N) / sizeof (mAuroraGpioInitData_N[0]), PlatformInfoHob->PlatformGpioSetting_N);
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_NW) / sizeof (mAuroraGpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_W) / sizeof (mAuroraGpioInitData_W[0]), PlatformInfoHob->PlatformGpioSetting_W);
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_SW) / sizeof (mAuroraGpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);

      if (SystemConfiguration.ScIshEnabled == 0) {
        DEBUG ((DEBUG_INFO, "Switch ISH_I2C0 & ISH_I2C1 to LPSS_I2C5 and LPSS I2C6. \n" ));
        GpioPadConfigTable (sizeof (mAuroraGpioInitData_LPSS_I2C) / sizeof (mAuroraGpioInitData_LPSS_I2C[0]), mAuroraGpioInitData_LPSS_I2C);
      }
      break;
    default:
    //
    // PAD programming
    //
    GpioPadConfigTable (sizeof (mAuroraGpioInitData_N) / sizeof (mAuroraGpioInitData_N[0]), PlatformInfoHob->PlatformGpioSetting_N);
    GpioPadConfigTable (sizeof (mAuroraGpioInitData_NW) / sizeof (mAuroraGpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
    GpioPadConfigTable (sizeof (mAuroraGpioInitData_W) / sizeof (mAuroraGpioInitData_W[0]), PlatformInfoHob->PlatformGpioSetting_W);
    GpioPadConfigTable (sizeof (mAuroraGpioInitData_SW) / sizeof (mAuroraGpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);

    if (SystemConfiguration.TDO == 2) {  //Auto
      if (BxtA0 == BxtStepping()) {
        DEBUG ((DEBUG_INFO, " BxtA0 TDO disable\n" ));
      }
    } else if (SystemConfiguration.TDO == 0) { // Disable
      DEBUG ((DEBUG_INFO, " Setup option to disable TDO\n" ));
    }

    if (SystemConfiguration.ScHdAudioIoBufferOwnership == 3) {
      DEBUG ((DEBUG_INFO, "HD Audio IO Buffer Ownership is I2S. Change GPIO pin settings for it. \n" ));
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_Audio_SSP6) / sizeof (mAuroraGpioInitData_Audio_SSP6[0]), mAuroraGpioInitData_Audio_SSP6);
    }

    if (SystemConfiguration.PcieRootPortEn[4] == FALSE) {
      DEBUG ((DEBUG_INFO, "Onboard LAN disable. \n" ));
      GpioPadConfigTable (sizeof (AuroraLomDisableGpio) / sizeof (AuroraLomDisableGpio[0]), AuroraLomDisableGpio);
    }

    if (SystemConfiguration.EPIEnable == 1) {
      DEBUG ((DEBUG_INFO, "Overriding GPIO 191 for EPI\n"));
      GpioPadConfigTable (sizeof (mAuroraGpioInitData_EPI_Override) / sizeof (mAuroraGpioInitData_EPI_Override[0]), mAuroraGpioInitData_EPI_Override);
    }
    if (SystemConfiguration.GpioLock == TRUE) {
      AuroraSetGpioPadCfgLock ();
    }
    DEBUG ((DEBUG_INFO, "No board ID available for this board ....\n"));
    break;
  }

  //
  // Dump Community registers
  //
  DumpGpioCommunityRegisters (NORTH);
  DumpGpioCommunityRegisters (NORTHWEST);
  DumpGpioCommunityRegisters (WEST);
  DumpGpioCommunityRegisters (SOUTHWEST);

  switch (PlatformInfoHob->BoardId) {
    case BOARD_ID_AURORA:
      //
      // PAD programming
      //
      DEBUG ((DEBUG_INFO, "Dump Community pad registers, Board ID: 0x%X\n", PlatformInfoHob->BoardId));
      DumpGpioPadTable (sizeof (mAuroraGpioInitData_N) / sizeof (mAuroraGpioInitData_N[0]), PlatformInfoHob->PlatformGpioSetting_N);
      DumpGpioPadTable (sizeof (mAuroraGpioInitData_NW) / sizeof (mAuroraGpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
      DumpGpioPadTable (sizeof (mAuroraGpioInitData_W) / sizeof (mAuroraGpioInitData_W[0]), PlatformInfoHob->PlatformGpioSetting_W);
      DumpGpioPadTable (sizeof (mAuroraGpioInitData_SW) / sizeof (mAuroraGpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);
      break;
    default:
    //
    // Dump Community pad registers
    //
    DumpGpioPadTable (sizeof (mAuroraGpioInitData_N) / sizeof (mAuroraGpioInitData_N[0]), PlatformInfoHob->PlatformGpioSetting_N);
    DumpGpioPadTable (sizeof (mAuroraGpioInitData_NW) / sizeof (mAuroraGpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
    DumpGpioPadTable (sizeof (mAuroraGpioInitData_W) / sizeof (mAuroraGpioInitData_W[0]), PlatformInfoHob->PlatformGpioSetting_W);
    DumpGpioPadTable (sizeof (mAuroraGpioInitData_SW) / sizeof (mAuroraGpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);

    break;
  }

  return EFI_SUCCESS;
}

