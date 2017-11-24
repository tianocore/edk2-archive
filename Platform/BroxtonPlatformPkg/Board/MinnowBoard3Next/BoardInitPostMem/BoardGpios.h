/** @file
  GPIO setting for Broxton.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MINNOW3_NEXT_BOARDGPIOS_H_
#define _MINNOW3_NEXT_BOARDGPIOS_H_

#include <PiPei.h>
#include"ChipsetAccess.h"
#include"PlatformBaseAddresses.h"
#include "BoardInitMiscs.h"
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo_Aplk.h>
#include <Library/GpioLib.h>

/**
GPIO input pin interrupt type configuration:

Interrupt type    GPI Route               Host SW                 Enable/Status                                 Comment
 GPI               None                    GPIO Driver Mode        GPI Interrupt Status\Enable                   GPIO driver to handle it.
 Direct IRQ        GPIROUTIOXAPIC          ACPI Mode                                                             IRQ number is fixed to each GPIO pin in N and NW communities.
 SCI/GPE           GPIROUTSCI              ACPI Mode               GPI General Purpose Events Status\Enable      SCI is not supported in BXT A0. The reason is because the PMC lacks the ACPI registers and status tunneling. This will be fixed in derivatives.
 SMI               GPIROUTSMI              ACPI Mode               SMI Status\Enable                             Don't enable SMI for BXT0. It is currently unsupported by the PMC.
 NMI               GPIROUTNMI              ACPI Mode                                                             Not supported on BXT.

Interrupt trigger type             Configuration                  Comment
 Rising edge                        Edge+No_invert
 Falling edge                       Edge+Invert
 Both edge                          BothEdge+Invert
 Level high                         Level+No_invert                Direct IRQ pin mostly use this config.Wake pin MUST use it.
 Level low                          Level+Invert

HostSw:
 * All GPIO pins which are 'M0' PMode, have to set HostSw to GPIO_D, indicating GPIO driver owns it.
 * Others, such as Native function(M1,M2,M3..) and SCI/SMI/NMI/Direct IRQ, need to set it to ACPI_D or NA.
 * Default is ACPI_D for NA

IOSstate:
 * For interrupt or wake pin, need to set it to TxDRxE.

Wake_Enabled:
 * It is for direct IRQ only.

**/

//
// North Community
//
BXT_GPIO_PAD_INIT  mMinnow3Next_GpioInitData_N[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,     MMIO_Offset  , Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_0",                   M0   ,    GPO   , GPIO_D,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0000, NORTH), // GP_DDC_EN
  BXT_GPIO_PAD_CONF(L"GPIO_1",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0008, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_2",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0010, NORTH), // GPIO9
  BXT_GPIO_PAD_CONF(L"GPIO_3",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0018, NORTH), // GPIO10
  BXT_GPIO_PAD_CONF(L"GPIO_4",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0020, NORTH), // GPIO11
  BXT_GPIO_PAD_CONF(L"GPIO_5",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE , Inverted,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0028, NORTH), // BC_SCI_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_6",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE , Inverted,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0030, NORTH), // LID#
  BXT_GPIO_PAD_CONF(L"GPIO_7",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE , Inverted,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0038, NORTH), // SLEEP#
  BXT_GPIO_PAD_CONF(L"GPIO_8",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0040, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_9",                   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0048, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_10",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0050, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_11",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0058, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_12",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0060, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_13",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0068, NORTH), // GPIO7
  BXT_GPIO_PAD_CONF(L"GPIO_14",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0070, NORTH), // GPIO8
  BXT_GPIO_PAD_CONF(L"GPIO_15",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0078, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_16",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0080, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_17",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0088, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_18",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0090, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_19",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0098, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_20",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A0, NORTH), // CLEAR_BACKUP
  BXT_GPIO_PAD_CONF(L"GPIO_21",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A8, NORTH), // FORCE_RECOV#
  BXT_GPIO_PAD_CONF(L"GPIO_22",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00B0, NORTH), // BOOT_SEL2#
  BXT_GPIO_PAD_CONF(L"GPIO_23",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00B8, NORTH), // BOOT_SEL1#
  BXT_GPIO_PAD_CONF(L"GPIO_24",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00C0, NORTH), // BOOT_SEL0#
  BXT_GPIO_PAD_CONF(L"GPIO_25",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00C8, NORTH), // TEST#
  BXT_GPIO_PAD_CONF(L"GPIO_26",                  M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00D0, NORTH), // SATA_LED#
  BXT_GPIO_PAD_CONF(L"GPIO_27",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00D8, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_28",                  M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00E0, NORTH), // USB2_EN
  BXT_GPIO_PAD_CONF(L"GPIO_29",                  M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00E8, NORTH), // USB_DRIVE_BUS_1V8
  BXT_GPIO_PAD_CONF(L"GPIO_30",                  M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00F0, NORTH), // USB1_EN
  BXT_GPIO_PAD_CONF(L"GPIO_31",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00F8, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_32",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0100, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_33",                  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0108, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_34 PWM0",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0110, NORTH), // STRAP_GPIO_34
  BXT_GPIO_PAD_CONF(L"GPIO_35 PWM1",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0118, NORTH), // STRAP_GPIO_35
  BXT_GPIO_PAD_CONF(L"GPIO_36 PWM2",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0120, NORTH), // STRAP_GPIO_36
  BXT_GPIO_PAD_CONF(L"GPIO_37 PWM3",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0128, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_38 LPSS_UART0_RXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0130, NORTH), // SER0_RX
  BXT_GPIO_PAD_CONF(L"GPIO_39 LPSS_UART0_TXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0138, NORTH), // SER0_TX
  BXT_GPIO_PAD_CONF(L"GPIO_40 LPSS_UART0_CTS_B", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0140, NORTH), // SER0_RTS#
  BXT_GPIO_PAD_CONF(L"GPIO_41 LPSS_UART0_CTS_B", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0148, NORTH), // SER0_CTS#
  BXT_GPIO_PAD_CONF(L"GPIO_42 LPSS_UART1_RXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0150, NORTH), // SER1_RX
  BXT_GPIO_PAD_CONF(L"GPIO_43 LPSS_UART1_TXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0158, NORTH), // SER1_TX
  BXT_GPIO_PAD_CONF(L"GPIO_44 LPSS_UART1_RTS_B", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0160, NORTH), // SER1_RTS#
  BXT_GPIO_PAD_CONF(L"GPIO_45 LPSS_UART1_CTS_B", M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0168, NORTH), // PWRBTN_BC_OUT_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_46 LPSS_UART2_RXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0170, NORTH), // SER2_RX
  BXT_GPIO_PAD_CONF(L"GPIO_47 LPSS_UART2_TXD",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0178, NORTH), // SER2_TX
  BXT_GPIO_PAD_CONF(L"GPIO_48 LPSS_UART2_RTS_B", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0180, NORTH), // SER2_RTS#
  BXT_GPIO_PAD_CONF(L"GPIO_49 LPSS_UART2_CTS_B", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0188, NORTH), // SER2_CTS#
  BXT_GPIO_PAD_CONF(L"GPIO_62 GP_CAMERASB00",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0190, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_63 GP_CAMERASB01",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0198, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_64 GP_CAMERASB02",    M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01A0, NORTH), // CAM0_RST#
  BXT_GPIO_PAD_CONF(L"GPIO_65 GP_CAMERASB03",    M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01A8, NORTH), // CAM1_RST#
  BXT_GPIO_PAD_CONF(L"GPIO_66 GP_CAMERASB04",    M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01B0, NORTH), // CAM0_PWR#
  BXT_GPIO_PAD_CONF(L"GPIO_67 GP_CAMERASB05",    M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01B8, NORTH), // CAM1_PWR#
  BXT_GPIO_PAD_CONF(L"GPIO_68 GP_CAMERASB06",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01C0, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_69 GP_CAMERASB07",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01C8, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_70 GP_CAMERASB08",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01D0, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_71 GP_CAMERASB09",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01D8, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_72 GP_CAMERASB10",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01E0, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_73 GP_CAMERASB11",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01E8, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"TCK",                      M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01F0, NORTH), // XDP_TCK
  BXT_GPIO_PAD_CONF(L"TRST_B",                   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01F8, NORTH), // XDP_TRST#
  BXT_GPIO_PAD_CONF(L"TMS",                      M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0200, NORTH), // XDP_TMS
  BXT_GPIO_PAD_CONF(L"TDI",                      M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0208, NORTH), // XDP_TDI
  BXT_GPIO_PAD_CONF(L"CX_PMODE",                 M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0210, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"CX_PREQ_B",                M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0218, NORTH), // XDP_PREQ_BUF#
  BXT_GPIO_PAD_CONF(L"JTAGX",                    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0220, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"CX_PRDY_B",                M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0228, NORTH), // XDP_PRDY#
  BXT_GPIO_PAD_CONF(L"TDO",                      M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0230, NORTH), // XDP_TDO
  BXT_GPIO_PAD_CONF(L"GPIO_216 CNV_BRI_DT",      M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0238, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_217 CNV_BRI_RSP",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0240, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_218 CNV_RGI_DT",      M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0248, NORTH), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_219 CNV_RGI_RSP",     M1   ,    NA    ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0250, NORTH), // EMMC_RST#
  BXT_GPIO_PAD_CONF(L"SVID0_ALERT_B",            M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0258, NORTH), // SVID_ALERT#
  BXT_GPIO_PAD_CONF(L"SVID0_DATA",               M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0260, NORTH), // SVID_DATA
  BXT_GPIO_PAD_CONF(L"SVID0_CLK",                M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0268, NORTH), // SVID_CLK
};

//
// North West Community
//
BXT_GPIO_PAD_INIT  mMinnow3Next_GpioInitData_NW [] =
{
  //
  //                 Group Pin#:  pad_name,     PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger, Wake_Enabled, Term_H_L,Inverted,GPI_ROUT,IOSstae,  IOSTerm,     MMIO_Offset    ,  Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_187 HV_DDI0_DDC_SDA", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   SAME, GPIO_PADBAR+0x0000, NORTHWEST), // DDI0_DDCDATA
  BXT_GPIO_PAD_CONF(L"GPIO_188 HV_DDI0_DDC_SCL", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   SAME, GPIO_PADBAR+0x0008, NORTHWEST), // DDI0_DDCCLK
  BXT_GPIO_PAD_CONF(L"GPIO_189 HV_DDI1_DDC_SDA", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   SAME, GPIO_PADBAR+0x0010, NORTHWEST), // DDI1_DDCDATA
  BXT_GPIO_PAD_CONF(L"GPIO_190 HV_DDI1_DDC_SCL", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   SAME, GPIO_PADBAR+0x0018, NORTHWEST), // DDI1_DDCCLK
  BXT_GPIO_PAD_CONF(L"GPIO_191 DBI_SDA",         M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0020, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_192 DBI_SCL",         M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0028, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_193 PANEL0_VDDEN",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA, D0RxDRx0I,   SAME, GPIO_PADBAR+0x0030, NORTHWEST), // PNL0_VDDEN
  BXT_GPIO_PAD_CONF(L"GPIO_194 PANEL0_BKLTEN",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA, D0RxDRx0I,   SAME, GPIO_PADBAR+0x0038, NORTHWEST), // PNL0_BKLTEN
  BXT_GPIO_PAD_CONF(L"GPIO_195 PANEL0_BKLTCTL",  M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA, D0RxDRx0I,   SAME, GPIO_PADBAR+0x0040, NORTHWEST), // PNL0_BKLCTL
  BXT_GPIO_PAD_CONF(L"GPIO_196 PANEL1_VDDEN",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0048, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_197 PANEL1_BKLTEN",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0050, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_198 PANEL1_BKLTCTL",  M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0058, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_199 DBI_CSX",         M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0060, NORTHWEST), // DDI1_HPD#
  BXT_GPIO_PAD_CONF(L"GPIO_200 DBI_RESX",        M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0068, NORTHWEST), // DDI0_HPD#
  BXT_GPIO_PAD_CONF(L"GPIO_201 GP_INTD_DSI_TE1", M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0070, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_202 GP_INTD_DSI_TE2", M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0078, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_203 USB_OC0_B",       M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0080, NORTHWEST), // USB_OC0#
  BXT_GPIO_PAD_CONF(L"GPIO_204 USB_OC1_B",       M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0088, NORTHWEST), // USB_OC1#
  BXT_GPIO_PAD_CONF(L"PMC_SPI_FS0",              M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0090, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMC_SPI_FS1",              M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,Last_Value,   SAME, GPIO_PADBAR+0x0098, NORTHWEST), // eDP_HPD#
  BXT_GPIO_PAD_CONF(L"PMC_SPI_FS2",              M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMC_SPI_RXD",              M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A8, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMC_SPI_TXD",              M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00B0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMC_SPI_CLK",              M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00B8, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMIC_PWRGOOD",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00C0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMIC_RESET_B",             M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00C8, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_213 PMIC_SDWN_B",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00D0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_214 PMIC_BCUDISW2",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00D8, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_215 PMIC_BCUDISCRIT", M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00E0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PMIC_THERMTRIP_B",         M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,Last_Value,   SAME, GPIO_PADBAR+0x00E8, NORTHWEST), // THERMTRIP
  BXT_GPIO_PAD_CONF(L"PMIC_STDBY",               M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00F0, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"PROCHOT_B",                M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x00F8, NORTHWEST), // PROCHOT#
  BXT_GPIO_PAD_CONF(L"PMIC_I2C_SCL",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0100, NORTHWEST), // SMB_CLK_PMIC
  BXT_GPIO_PAD_CONF(L"PMIC_I2C_SDA",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0108, NORTHWEST), // SMB_DAT_PMIC
  BXT_GPIO_PAD_CONF(L"GPIO_74 AVS_I2S1_MCLK",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0110, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_75 AVS_I2S1_BCLK",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0118, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_76 AVS_I2S1_WS_SYNC", M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0120, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_77 AVS_I2S1_SDI",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0128, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_78 AVS_I2S1_SDO",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0130, NORTHWEST), // STRAP_GPIO_78
  BXT_GPIO_PAD_CONF(L"GPIO_79 AVS_M_CLK_A1",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0138, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_80 AVS_M_CLK_B1",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0140, NORTHWEST), // HWCONF6
  BXT_GPIO_PAD_CONF(L"GPIO_81 AVS_M_DATA_1",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0148, NORTHWEST), // HWCONF7
  BXT_GPIO_PAD_CONF(L"GPIO_82 AVS_M_CLK_AB2",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0150, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_83 AVS_M_DATA_2",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0158, NORTHWEST), // HWCONF8
  BXT_GPIO_PAD_CONF(L"GPIO_84 AVS_I2S2_MCLK",    M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0160, NORTHWEST), // AUDIO_MCK_HDA_RST#
  BXT_GPIO_PAD_CONF(L"GPIO_85 AVS_I2S2_BCLK",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0168, NORTHWEST), // I2S0_CK
  BXT_GPIO_PAD_CONF(L"GPIO_86 AVS_I2S2_WS_SYNC", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0170, NORTHWEST), // I2S0_LRCK
  BXT_GPIO_PAD_CONF(L"GPIO_87 AVS_I2S2_SDI",     M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,    TxDRxE,   EnPd, GPIO_PADBAR+0x0178, NORTHWEST), // I2S0_SDIN
  BXT_GPIO_PAD_CONF(L"GPIO_88 AVS_I2S2_SDO",     M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0180, NORTHWEST), // I2S0_SDOUT
  BXT_GPIO_PAD_CONF(L"GPIO_89 AVS_I2S3_BCLK",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0188, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_90 AVS_I2S3_WS_SYNC", M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0190, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_91 AVS_I2S3_SDI",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0198, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_92 AVS_I2S3_SDO",     M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x01A0, NORTHWEST), // STRAP_GPIO_92
  BXT_GPIO_PAD_CONF(L"GPIO_97 FST_SPI_CS0_B",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01A8, NORTHWEST), // FST_SPI_CS0#
  BXT_GPIO_PAD_CONF(L"GPIO_98 FST_SPI_CS1_B",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01B0, NORTHWEST), // FST_SPI_CS1#
  BXT_GPIO_PAD_CONF(L"GPIO_99 FST_SPI_MOSI_IO0", M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01B8, NORTHWEST), // FST_SPI_MOSI
  BXT_GPIO_PAD_CONF(L"GPIO_100 FST_SPI_MISO_IO1",M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01C0, NORTHWEST), // FST_SPI_MISO
  BXT_GPIO_PAD_CONF(L"GPIO_101 FST_SPI_IO2",     M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01C8, NORTHWEST), // FST_SPI_WP
  BXT_GPIO_PAD_CONF(L"GPIO_102 FST_SPI_IO3",     M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01D0, NORTHWEST), // FST_SPI_HOLD
  BXT_GPIO_PAD_CONF(L"GPIO_103 FST_SPI_CLK",     M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, Native_control,NA,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01D8, NORTHWEST), // FST_SPI_CLK
  BXT_GPIO_PAD_CONF(L"FST_SPI_CLK_FB",           M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x01E0, NORTHWEST), //
  BXT_GPIO_PAD_CONF(L"GPIO_104 GP_SSP_0_CLK",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x01E8, NORTHWEST), // SIO_SPI_SCLK
  BXT_GPIO_PAD_CONF(L"GPIO_105 GP_SSP_0_FS0",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x01F0, NORTHWEST), // SIO_SPI_CS0#
  BXT_GPIO_PAD_CONF(L"GPIO_106 GP_SSP_0_FS1",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x01F8, NORTHWEST), // SIO_SPI_CS1#
  BXT_GPIO_PAD_CONF(L"GPIO_109 GP_SSP_0_RXD",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0200, NORTHWEST), // SIO_SPI_MISO
  BXT_GPIO_PAD_CONF(L"GPIO_110 GP_SSP_0_TXD",    M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0208, NORTHWEST), // SIO_SPI_MOSI
  BXT_GPIO_PAD_CONF(L"GPIO_111 GP_SSP_1_CLK",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0210, NORTHWEST), // STRAP_GPIO_111
  BXT_GPIO_PAD_CONF(L"GPIO_112 GP_SSP_1_FS0",    M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0218, NORTHWEST), // SER3_RX
  BXT_GPIO_PAD_CONF(L"GPIO_113 GP_SSP_1_FS1",    M2   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x0220, NORTHWEST), // SER3_TX
  BXT_GPIO_PAD_CONF(L"GPIO_116 GP_SSP_1_RXD",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0228, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_117 GP_SSP_1_TXD",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0230, NORTHWEST), // STRAP_GPIO_117
  BXT_GPIO_PAD_CONF(L"GPIO_118 GP_SSP_2_CLK",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0238, NORTHWEST), // STRAP_GPIO_118
  BXT_GPIO_PAD_CONF(L"GPIO_119 GP_SSP_2_FS0",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0240, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_120 GP_SSP_2_FS1",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0248, NORTHWEST), // STRAP_GPIO_120
  BXT_GPIO_PAD_CONF(L"GPIO_121 GP_SSP_2_FS2",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0250, NORTHWEST), // STRAP_GPIO_121
  BXT_GPIO_PAD_CONF(L"GPIO_122 GP_SSP_2_RXD",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0258, NORTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_123 GP_SSP_2_TXD",    M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0260, NORTHWEST), // STRAP_GPIO_123
};

//
// West Community
//
BXT_GPIO_PAD_INIT  mMinnow3Next_GpioInitData_W [] =
{
  //
  //                   Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L, Inverted,GPI_ROUT,IOSstae, IOSTerm,     MMIO_Offset    , Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_124 LPSS_I2C0_SDA",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0000, WEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_125 LPSS_I2C0_SCL",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0008, WEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_126 LPSS_I2C1_SDA",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA, D1RxDRx1I,   EnPu, GPIO_PADBAR+0x0010, WEST), // GP_DDCDATA
  BXT_GPIO_PAD_CONF(L"GPIO_127 LPSS_I2C1_SCL",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA, D1RxDRx1I,   EnPu, GPIO_PADBAR+0x0018, WEST), // GP_DDCCLK
  BXT_GPIO_PAD_CONF(L"GPIO_128 LPSS_I2C2_SDA",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0020, WEST), // HWCONF0
  BXT_GPIO_PAD_CONF(L"GPIO_129 LPSS_I2C2_SCL",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0028, WEST), // HWCONF3
  BXT_GPIO_PAD_CONF(L"GPIO_130 LPSS_I2C3_SDA",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0030, WEST), // HWCONF2
  BXT_GPIO_PAD_CONF(L"GPIO_131 LPSS_I2C3_SCL",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0038, WEST), // HWCONF1
  BXT_GPIO_PAD_CONF(L"GPIO_132 LPSS_I2C4_SDA",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA, D1RxDRx1I,   EnPu, GPIO_PADBAR+0x0040, WEST), // I2C_CAM0_DAT
  BXT_GPIO_PAD_CONF(L"GPIO_133 LPSS_I2C4_SCL",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_1K_H ,    NA   ,    NA, D1RxDRx1I,   EnPu, GPIO_PADBAR+0x0048, WEST), // I2C_CAM0_CK
  BXT_GPIO_PAD_CONF(L"GPIO_134 LPSS_I2C5_SDA",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0050, WEST), // I2C_CAM1_DAT
  BXT_GPIO_PAD_CONF(L"GPIO_135 LPSS_I2C5_SCL",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0058, WEST), // I2C_CAM1_CK
  BXT_GPIO_PAD_CONF(L"GPIO_136 LPSS_I2C6_SDA",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0060, WEST), // I2C_GP_DAT
  BXT_GPIO_PAD_CONF(L"GPIO_137 LPSS_I2C6_SCL",   M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x0068, WEST), // I2C_GP_CK
  BXT_GPIO_PAD_CONF(L"GPIO_138 LPSS_I2C7_SDA",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0070, WEST), // HWCONF5
  BXT_GPIO_PAD_CONF(L"GPIO_139 LPSS_I2C7_SCL",   M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0078, WEST), // HWCONF4
  BXT_GPIO_PAD_CONF(L"GPIO_146 ISH_GPIO_0",      M3   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0080, WEST), // HDA_CK
  BXT_GPIO_PAD_CONF(L"GPIO_147 ISH_GPIO_1",      M3   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0088, WEST), // HDA_SYNC
  BXT_GPIO_PAD_CONF(L"GPIO_148 ISH_GPIO_2",      M3   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0090, WEST), // HDA_SDI
  BXT_GPIO_PAD_CONF(L"GPIO_149 ISH_GPIO_3",      M3   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0098, WEST), // HDA_SDO
  BXT_GPIO_PAD_CONF(L"GPIO_150 ISH_GPIO_4",      M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A0, WEST), // USB5_EN
  BXT_GPIO_PAD_CONF(L"GPIO_151 ISH_GPIO_5",      M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00A8, WEST), // USB4_EN
  BXT_GPIO_PAD_CONF(L"GPIO_152 ISH_GPIO_6",      M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00B0, WEST), // USB3_EN
  BXT_GPIO_PAD_CONF(L"GPIO_153 ISH_GPIO_7",      M0   ,    GPO   ,  NA   ,  HI    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x00B8, WEST), // WD_ALERT_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_154 ISH_GPIO_8",      M0   ,    GPO   , GPIO_D,  LO    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x00C0, WEST), // WDTRIG_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_155 ISH_GPIO_9",      M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00C8, WEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_209 PCIE_CLKREQ0_B",  M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x00D0, WEST), // GND
  BXT_GPIO_PAD_CONF(L"GPIO_210 PCIE_CLKREQ1_B",  M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x00D8, WEST), // GND
  BXT_GPIO_PAD_CONF(L"GPIO_211 PCIE_CLKREQ2_B",  M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x00E0, WEST), // GND
  BXT_GPIO_PAD_CONF(L"GPIO_212 PCIE_CLKREQ3_B",  M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx0I,   EnPd, GPIO_PADBAR+0x00E8, WEST), // GND
  BXT_GPIO_PAD_CONF(L"OSC_CLK_OUT_0",            M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,     NA   ,     NA, GPIO_PADBAR+0x00F0, WEST), // CAM_MCK
  BXT_GPIO_PAD_CONF(L"OSC_CLK_OUT_1",            M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x00F8, WEST), // NC
  BXT_GPIO_PAD_CONF(L"OSC_CLK_OUT_2",            M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0100, WEST), // NC
  BXT_GPIO_PAD_CONF(L"OSC_CLK_OUT_3",            M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0108, WEST), // NC
  BXT_GPIO_PAD_CONF(L"OSC_CLK_OUT_4",            M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0110, WEST), // NC
  BXT_GPIO_PAD_CONF(L"PMU_AC_PRESENT",           M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0118, WEST), // ACPRESENT
  BXT_GPIO_PAD_CONF(L"PMU_BATLOW_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0120, WEST), // BATLOW_3V3#
  BXT_GPIO_PAD_CONF(L"PMU_PLTRST_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0128, WEST), // PLTRST#
  BXT_GPIO_PAD_CONF(L"PMU_PWRBTN_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0130, WEST), // PWRBTN_BC_OUT#
  BXT_GPIO_PAD_CONF(L"PMU_RESETBUTTON_B",        M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0138, WEST), // RSTBTN_BC_OUT#
  BXT_GPIO_PAD_CONF(L"PMU_SLP_S0_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0140, WEST), // SLP_SOIX#
  BXT_GPIO_PAD_CONF(L"PMU_SLP_S3_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0148, WEST), // SLP_S3#
  BXT_GPIO_PAD_CONF(L"PMU_SLP_S4_B",             M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0150, WEST), // SLP_S4#
  BXT_GPIO_PAD_CONF(L"PMU_SUSCLK",               M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0158, WEST), // SUSCLK
  BXT_GPIO_PAD_CONF(L"PMU_WAKE_B",               M0   ,    GPI   ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0160, WEST), // NC
  BXT_GPIO_PAD_CONF(L"SUS_STAT_B",               M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0168, WEST), // SUS_STAT#
  BXT_GPIO_PAD_CONF(L"SUSPWRDNACK",              M1   ,    NA    ,  NA   ,  NA    ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0170, WEST), // SUSPWRDNACK
};

 //
 // South West Community
 //
BXT_GPIO_PAD_INIT  mMinnow3Next_GpioInitData_SW[]=
{
  //
  //                  Group Pin#:  pad_name,       PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,Wake_Enabled, Term_H_L,Inverted,GPI_ROUT,IOSstae,  IOSTerm,   MMIO_Offset     ,  Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_205 PCIE_WAKE0_B",      M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0000, SOUTHWEST), // PCIE_WAKE_LAN_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_206 PCIE_WAKE1_B",      M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0008, SOUTHWEST), // PCIE_WAKE_Q7_1V8#
  BXT_GPIO_PAD_CONF(L"GPIO_207 PCIE_WAKE2_B",      M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0010, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_208 PCIE_WAKE3_B",      M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0018, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_156 EMMC0_CLK",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA, D0RxDRx0I,   SAME, GPIO_PADBAR+0x0020, SOUTHWEST), // EMMC_CLK
  BXT_GPIO_PAD_CONF(L"GPIO_157 EMMC0_D0",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0028, SOUTHWEST), // EMMC_DAT0
  BXT_GPIO_PAD_CONF(L"GPIO_158 EMMC0_D1",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0030, SOUTHWEST), // EMMC_DAT1
  BXT_GPIO_PAD_CONF(L"GPIO_159 EMMC0_D2",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0038, SOUTHWEST), // EMMC_DAT2
  BXT_GPIO_PAD_CONF(L"GPIO_160 EMMC0_D3",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0040, SOUTHWEST), // EMMC_DAT3
  BXT_GPIO_PAD_CONF(L"GPIO_161 EMMC0_D4",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0048, SOUTHWEST), // EMMC_DAT4
  BXT_GPIO_PAD_CONF(L"GPIO_162 EMMC0_D5",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0050, SOUTHWEST), // EMMC_DAT5
  BXT_GPIO_PAD_CONF(L"GPIO_163 EMMC0_D6",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0058, SOUTHWEST), // EMMC_DAT6
  BXT_GPIO_PAD_CONF(L"GPIO_164 EMMC0_D7",          M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0060, SOUTHWEST), // EMMC_DAT7
  BXT_GPIO_PAD_CONF(L"GPIO_165 EMMC0_CMD",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x0068, SOUTHWEST), // EMMC_CMD
  BXT_GPIO_PAD_CONF(L"GPIO_166 SDIO_CLK",          M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0070, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_167 SDIO_D0",           M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0078, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_168 SDIO_D1",           M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0080, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_169 SDIO_D2",           M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0088, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_170 SDIO_D3",           M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0090, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_171 SDIO_CMD",          M0   ,    GPI   ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,     NA   ,DisPuPd, GPIO_PADBAR+0x0098, SOUTHWEST), // NC
  BXT_GPIO_PAD_CONF(L"GPIO_172 SDCARD_CLK",        M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x00A0, SOUTHWEST), // SDIO_CLK
  BXT_GPIO_PAD_CONF(L"GPIO_179 SDCARD_CLK_FB",     M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,        NA,     NA, GPIO_PADBAR+0x00A8, SOUTHWEST), //
  BXT_GPIO_PAD_CONF(L"GPIO_173 SDCARD_D0",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x00B0, SOUTHWEST), // SDIO_D0
  BXT_GPIO_PAD_CONF(L"GPIO_174 SDCARD_D1",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x00B8, SOUTHWEST), // SDIO_D1
  BXT_GPIO_PAD_CONF(L"GPIO_175 SDCARD_D2",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x00C0, SOUTHWEST), // SDIO_D2
  BXT_GPIO_PAD_CONF(L"GPIO_176 SDCARD_D3",         M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,   SAME, GPIO_PADBAR+0x00C8, SOUTHWEST), // SDIO_D3
  BXT_GPIO_PAD_CONF(L"GPIO_177 SDCARD_CD_B",       M0   ,    GPI   , GPIO_D,  NA     ,   Edge     , Wake_Disabled, P_NONE ,    NA   ,    NA,    TxDRxE,     NA, GPIO_PADBAR+0x00D0, SOUTHWEST), // SDIO_1V8_CD#
  BXT_GPIO_PAD_CONF(L"GPIO_178 SDCARD_CMD",        M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x00D8, SOUTHWEST), // SDIO_CMD
  BXT_GPIO_PAD_CONF(L"GPIO_186 SDCARD_LVL_WP",     M0   ,    GPI   , GPIO_D,  NA     ,   Edge     , Wake_Disabled, P_20K_L, Inverted,    NA,Last_Value,   SAME, GPIO_PADBAR+0x00E0, SOUTHWEST), // SDIO_1V8_WP
  BXT_GPIO_PAD_CONF(L"GPIO_182 EMMC0_STROBE",      M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,   HizRx0I,   SAME, GPIO_PADBAR+0x00E8, SOUTHWEST), // EMMC_STROBE
  BXT_GPIO_PAD_CONF(L"GPIO_183 SDIO_PWR_DOWN_B",   M0   ,    GPO   , GPIO_D,  LO     ,   NA       , Wake_Disabled, P_20K_L,    NA   ,    NA,     NA   ,   EnPd, GPIO_PADBAR+0x00F0, SOUTHWEST), // SDIO_PWR_EN_1V8
  BXT_GPIO_PAD_CONF(L"SMB_ALERTB",                 M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x00F8, SOUTHWEST), // SMB_ALERT#
  BXT_GPIO_PAD_CONF(L"LPC_ILB_SERIRQ",             M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,IOS_Masked,   SAME, GPIO_PADBAR+0x0110, SOUTHWEST), // SERIRQ
  BXT_GPIO_PAD_CONF(L"LPC_CLKOUT0",                M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0118, SOUTHWEST), // LPC_CLKOUT0
  BXT_GPIO_PAD_CONF(L"LPC_CLKOUT1",                M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_NONE ,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0120, SOUTHWEST), // LPC_CLKOUT1
  BXT_GPIO_PAD_CONF(L"LPC_AD0",                    M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0128, SOUTHWEST), // LPC_AD0
  BXT_GPIO_PAD_CONF(L"LPC_AD1",                    M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0130, SOUTHWEST), // LPC_AD1
  BXT_GPIO_PAD_CONF(L"LPC_AD2",                    M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0138, SOUTHWEST), // LPC_AD2
  BXT_GPIO_PAD_CONF(L"LPC_AD3",                    M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0140, SOUTHWEST), // LPC_AD3
  BXT_GPIO_PAD_CONF(L"LPC_CLKRUNB",                M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0148, SOUTHWEST), // LPC_CLKRUN_SOC#
  BXT_GPIO_PAD_CONF(L"LPC_FRAMEB",                 M1   ,    NA    ,  NA   ,  NA     ,   NA       , Wake_Disabled, P_20K_H,    NA   ,    NA,   HizRx1I,DisPuPd, GPIO_PADBAR+0x0150, SOUTHWEST), // LPC_FRAME#
};

BXT_GPIO_PAD_INIT  mMinnow3Next_GpioInitData_FAB2[] =
{
  //
  //                  Group Pin#:  pad_name,    PMode,GPIO_Config,HostSw,GPO_STATE,INT_Trigger,  Wake_Enabled ,Term_H_L,Inverted, GPI_ROUT, IOSstae, IOSTerm,     MMIO_Offset  ,Community
  //
  BXT_GPIO_PAD_CONF(L"GPIO_6",                   M0   ,    GPI   ,GPIO_D,   NA    ,   Level    , Wake_Disabled, P_20K_L,   NA    ,IOAPIC,    TxDRxE,    NA,  GPIO_PADBAR+0x0030,  NORTH),     // LID#
};

#endif

