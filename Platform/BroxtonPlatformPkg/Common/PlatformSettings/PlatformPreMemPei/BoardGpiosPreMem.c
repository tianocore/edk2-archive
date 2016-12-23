/** @file
  Gpio setting for multiplatform before Memory init.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardGpiosPreMem.h"

#include <Guid/SetupVariable.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/GpioLib.h>
#include <Library/DebugLib.h>
#include <Library/SteppingLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <ScRegs/RegsGpio.h>

//
// BXT PreMemory GPIO CONFIGURATION
//

//
// BXT GPIO Settings
//
/**
GPIO input pin interrupt type configuration:
Interrupt type     GPI Route         Host SW               Enable/Status                               Comment
 GPI               None              GPIO Driver Mode      GPI Interrupt Status\Enable                 GPIO driver to handle it
 Direct IRQ        GPIROUTIOXAPIC    ACPI Mode                                                         IRQ number is fixed to each GPIO pin in N and NW communities
 SCI/GPE           GPIROUTSCI        ACPI Mode             GPI General Purpose Events Status\Enable    SCI is not supported in BXT A0. The reason is because the PMC lacks the ACPI registers and status tunneling. This will be fixed in derivatives.
 SMI               GPIROUTSMI        ACPI Mode             SMI Status\Enable                           Don't enable SMI for BXT0. It is currently unsupported by the PMC.
 NMI               GPIROUTNMI        ACPI Mode                                                         Not support on BXT

Interrupt trigger type             Configuration               Comment
 rising edge                        Edge+No_invert
 falling edge                       Edge+Invert
 both edge                          BothEdge+Invert
 level high                         Level+No_invert             Direct IRQ pin mostly use this config.Wake pin MUST use it.
 level low                          Level+Invert

HostSw:
* All GPIO pins which are 'M0' PMode, have to set HostSw to GPIO_D, indicating GPIO driver owns it.
* Others, such as Native function(M1,M2,M3..) and SCI/SMI/NMI/Direct IRQ, need to set it to ACPI_D or NA.

**/

//
// For Apl board
//
BXT_GPIO_PAD_INIT  IshI2cGpio[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled, Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,   MMIO_Offset,    Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_134 LPSS_I2C5_SDA",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_1K_H,    NA   ,    NA, IOS_Masked,  EnPu,  GPIO_PADBAR+0x0050,  WEST),
  BXT_GPIO_PAD_CONF(L"GPIO_135 LPSS_I2C5_SCL",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_1K_H,    NA   ,    NA, IOS_Masked,  EnPu,  GPIO_PADBAR+0x0058,  WEST),
  BXT_GPIO_PAD_CONF(L"GPIO_136 LPSS_I2C6_SDA",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_1K_H,    NA   ,    NA, IOS_Masked,  EnPu,  GPIO_PADBAR+0x0060,  WEST),
  BXT_GPIO_PAD_CONF(L"GPIO_137 LPSS_I2C6_SCL",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_1K_H,    NA   ,    NA, IOS_Masked,  EnPu,  GPIO_PADBAR+0x0068,  WEST),
};

//
// North Peak GPIO settings before memory initialization, as it needs to be enabled before memory init
//
BXT_GPIO_PAD_INIT  NorthPeakGpio[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,     MMIO_Offset,      Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_0",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0000,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_1",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0008,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_2",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0010,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_3",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0018,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_4",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0020,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_5",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0028,  NORTH),//Mux with CSE_PG based on the SW3 switch
  BXT_GPIO_PAD_CONF(L"GPIO_6",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0030,  NORTH),//Mux with DISP1_RST_N based on the SW3 switch
  BXT_GPIO_PAD_CONF(L"GPIO_7",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0038,  NORTH),//Mux with DISP1_TOUCH_INT_N based on the SW3 switch
  BXT_GPIO_PAD_CONF(L"GPIO_8",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,   HizRx0I,   SAME,    GPIO_PADBAR+0x0040,  NORTH),//Mux with DISP1_TOUCH_RST_N based on the SW3 switch
};


BXT_GPIO_PAD_INIT  LpssSpi1Gpio [] =
{
  //
  //                 Group Pin#:  pad_name,     PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger, Wake_Enabled, Term_H_L,Inverted,GPI_ROUT,IOSstae,  IOSTerm,    MMIO_Offset, Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_111 GP_SSP_1_CLK",    M1   ,    HI_Z  , NA   ,   NA    ,   NA      , Wake_Disabled,  P_20K_H,   NA    ,    NA,Last_Value,  SAME,     GPIO_PADBAR+0x0210,  NORTHWEST),//SSP_5_CLK_R
  BXT_GPIO_PAD_CONF(L"GPIO_112 GP_SSP_1_FS0",    M1   ,    HI_Z  , NA   ,   NA    ,   NA      , Wake_Disabled,  P_20K_H,   NA    ,    NA,Last_Value,  SAME,     GPIO_PADBAR+0x0218,  NORTHWEST),//GP_SSP_1_FS0_R post
  BXT_GPIO_PAD_CONF(L"GPIO_113 GP_SSP_1_FS1",    M1   ,    NA    , NA   ,   NA    ,   NA      , Wake_Disabled,  P_20K_H,   NA    ,    NA,Last_Value,  SAME,     GPIO_PADBAR+0x0220,  NORTHWEST),//GP_SSP_1_FS1_R UART
  BXT_GPIO_PAD_CONF(L"GPIO_116 GP_SSP_1_RXD",    M1   ,    HI_Z  , NA   ,   NA    ,   NA      , Wake_Disabled,  P_20K_H,   NA    ,    NA,Last_Value,  SAME,     GPIO_PADBAR+0x0228,  NORTHWEST),//SSP_5_RXD_R
  BXT_GPIO_PAD_CONF(L"GPIO_117 GP_SSP_1_TXD",    M1   ,    HI_Z  , NA   ,   NA    ,   NA      , Wake_Disabled,  P_20K_H,   NA    ,    NA,Last_Value,  SAME,     GPIO_PADBAR+0x0230,  NORTHWEST),//SSP_5_TXD_R
};


//
//  Need to make sure ISH I2C GPIOs are configured before ISH starts running. [HSD 1205461649]
//  ApolloLake
//
BXT_GPIO_PAD_INIT  IshI2cGpio2[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,MMIO_Offset,Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_134 ISH_I2C0_SDA",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_NONE,   NA    ,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR+0x0050,  WEST),
  BXT_GPIO_PAD_CONF(L"GPIO_135 ISH_I2C0_SCL",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_NONE,   NA    ,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR+0x0058,  WEST)
};


//
//  Need to make sure ISH I2C GPIOs are configured before ISH starts running. [HSD 1205461649]
//  ApolloLake
//
BXT_GPIO_PAD_INIT  IshI2cGpio3[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,MMIO_Offset,Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_136 ISH_I2C1_SDA",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_NONE,   NA    ,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR+0x0060,  WEST),
  BXT_GPIO_PAD_CONF(L"GPIO_137 ISH_I2C1_SCL",   M2   ,    NA    , NA   ,   NA    ,   NA      ,  Wake_Disabled, P_NONE,   NA    ,    NA, D0RxDRx0I,  EnPu,  GPIO_PADBAR+0x0068,  WEST)
};


BXT_GPIO_PAD_INIT  LpcGpio [] =
{
  //
  //                   Group Pin#:  pad_name,     PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger, Wake_Enabled, Term_H_L,Inverted,GPI_ROUT,   IOSstae, IOSTerm,    MMIO_Offset, Community
  //
  BXT_GPIO_PAD_CONF(L"LPC_ILB_SERIRQ",             M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,IOS_Masked, SAME   ,GPIO_PADBAR+0x0110 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_CLKOUT0",                M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_NONE  ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0118 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_CLKOUT1",                M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_NONE  ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0120 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_AD0",                    M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0128 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_AD1",                    M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0130 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_AD2",                    M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0138 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_AD3",                    M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0140 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_CLKRUNB",                M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0148 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"LPC_FRAMEB",                 M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,HizRx1I   , DisPuPd,GPIO_PADBAR+0x0150 ,  SOUTHWEST),
};


BXT_GPIO_PAD_INIT  SmbusGpio [] =
{ //
  //                   Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L, Inverted,GPI_ROUT,IOSstae, IOSTerm,   MMIO_Offset,        Community
  //
  BXT_GPIO_PAD_CONF(L"SMB_CLK",                    M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,IOS_Masked, SAME   ,GPIO_PADBAR+0x0100 ,  SOUTHWEST),
  BXT_GPIO_PAD_CONF(L"SMB_DATA",                   M1,     NA    ,   NA ,  NA     ,   NA      ,Wake_Disabled, P_20K_H ,   NA   ,    NA  ,IOS_Masked, SAME   ,GPIO_PADBAR+0x0108 ,  SOUTHWEST),
};


BXT_GPIO_PAD_INIT  UartGpio [] =
{
  //
  //                   Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L, Inverted,GPI_ROUT,IOSstae, IOSTerm,   MMIO_Offset,      Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_46 LPSS_UART2_RXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,   NA    ,    NA,      NA  ,     NA, GPIO_PADBAR+0x0170,  NORTH),
  BXT_GPIO_PAD_CONF(L"GPIO_47 LPSS_UART2_TXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,   NA    ,    NA,      NA  ,     NA, GPIO_PADBAR+0x0178,  NORTH),
};


BXT_GPIO_PAD_INIT  SataGpio [] =
{
  BXT_GPIO_PAD_CONF(L"GPIO_22",                  M0   ,    GPO   , GPIO_D,  HI    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,  HizRx0I ,   SAME, GPIO_PADBAR+0x00B0,  NORTH),//Feature:Power Enable for SATA DIRECT CONNECTOR
};


BXT_GPIO_PAD_INIT  PcieGpio [] =
{
  //
  //                   Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L, Inverted,GPI_ROUT,IOSstae, IOSTerm,   MMIO_Offset, Community
  //
  // slot 1
  //
  BXT_GPIO_PAD_CONF(L"GPIO_152 ISH_GPIO_6",      M0   ,    GPO   , GPIO_D,  LO    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x00B0,  WEST),  // PERST#
  BXT_GPIO_PAD_CONF(L"GPIO_19",                  M0   ,    GPO   , GPIO_D,  HI    ,   NA       , Wake_Disabled, P_20K_H,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0098,  NORTH), // PFET

  //
  // Slot 2
  //
  BXT_GPIO_PAD_CONF(L"GPIO_13",                  M0   ,    GPO   , GPIO_D,  LO    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0068,  NORTH), // PERST#
  BXT_GPIO_PAD_CONF(L"GPIO_17",                  M0   ,    GPO   , GPIO_D,  HI    ,   NA       , Wake_Disabled, P_20K_H,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0088,  NORTH), // PFET

  //
  // NGFF
  //
  BXT_GPIO_PAD_CONF(L"GPIO_15",                  M0   ,    GPO   , GPIO_D,  LO    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0078,  NORTH), // PERST#

  //
  // LAN
  //
  BXT_GPIO_PAD_CONF(L"GPIO_210 PCIE_CLKREQ1_B",  M1   ,    NA    , NA    ,  NA    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x00D8,  WEST),  // CLKREQ#
  BXT_GPIO_PAD_CONF(L"GPIO_37 PWM3",             M0   ,    GPO   , GPIO_D,  LO    ,   NA       , Wake_Disabled, P_20K_L,   NA    ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0128,  NORTH), // PERST#
};


/**
  Set GPIO pins before MRC init per board design.

  @retval   EFI_SUCCESS          The function completed successfully.

**/
EFI_STATUS
MultiPlatformGpioProgramPreMem (
  IN OUT UINT64   *StartTimerTick
  )
{
  // PAD programming
  GpioPadConfigTable (sizeof (IshI2cGpio) / sizeof (IshI2cGpio[0]), IshI2cGpio);
  GpioPadConfigTable (sizeof (NorthPeakGpio) / sizeof (NorthPeakGpio[0]), NorthPeakGpio);
  GpioPadConfigTable (sizeof (LpssSpi1Gpio) / sizeof (LpssSpi1Gpio[0]), LpssSpi1Gpio);

  GpioPadConfigTable (sizeof (PcieGpio) / sizeof (PcieGpio[0]), PcieGpio);
  *StartTimerTick = GetPerformanceCounter ();
  GpioPadConfigTable (sizeof (SataGpio) / sizeof (SataGpio[0]), SataGpio);
  GpioPadConfigTable (sizeof (LpcGpio) / sizeof (LpcGpio[0]), LpcGpio);
  GpioPadConfigTable (sizeof (SmbusGpio) / sizeof (SmbusGpio[0]), SmbusGpio);
  GpioPadConfigTable (sizeof (UartGpio) / sizeof (UartGpio[0]), UartGpio);

  return EFI_SUCCESS;
}


/**
  Set GPIO pins before MRC init per board design.
  Before call it, make sure PlatformInfoHob->BoardId&PlatformFlavor is get correctly.

  @retval  EFI_SUCCESS      The function completed successfully.

**/
EFI_STATUS
MultiPlatformGpioUpdatePreMem (
  VOID
  )
{
  UINTN                           VariableSize;
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINT8                           LpcGpioSize;
  UINT8                           Index;
  UINT32                          PadCfgDw1Offset;
  UINT32                          Data32;

  ZeroMem (&SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));

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
                               &SystemConfiguration
                               );

  if (!EFI_ERROR (Status)) {

    if (SystemConfiguration.IshI2c0PullUp == 0) {
      GpioPadConfigTable (sizeof (IshI2cGpio2) / sizeof (IshI2cGpio2[0]), IshI2cGpio2);
    }

    if (SystemConfiguration.IshI2c1PullUp == 0) {
      GpioPadConfigTable (sizeof (IshI2cGpio3) / sizeof (IshI2cGpio3[0]), IshI2cGpio3);
    }

    if (SystemConfiguration.LowPowerS0Idle == TRUE) {
      LpcGpioSize = sizeof (LpcGpio) / sizeof (LpcGpio[0]);
      //
      // Set Indxe = 1 becasue the first element of LpcGpio[] has already set to IOS_Masked.
      //
      for (Index = 1; Index < LpcGpioSize; Index++) {

        PadCfgDw1Offset = ((LpcGpio[Index].Community << 16) + LpcGpio[Index].MMIO_ADDRESS + BXT_GPIO_PAD_CONF1_OFFSET);
        Data32 = GpioPadRead (PadCfgDw1Offset);
        Data32 &= ~(B_GPIO_IOSSTATE | B_GPIO_IOSTERM);
        Data32 |= ((IOS_Masked << N_GPIO_IOSSTATE) | (SAME << N_GPIO_IOSTERM));
        GpioPadWrite (PadCfgDw1Offset, Data32);
      }
    }
  }

  return EFI_SUCCESS;
}

