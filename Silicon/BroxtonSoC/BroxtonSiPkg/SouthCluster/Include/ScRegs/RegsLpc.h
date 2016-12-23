/** @file
  Register names for SC LPC/eSPI device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_REGS_LPC_H_
#define _SC_REGS_LPC_H_

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

//
// BXT-P LPC Device IDs
//
#define V_SC_LPC_DEVICE_ID_BXT_P_0                0x5AE8          ///< BXT-P SKU

#define V_PCH_LPC_RID_0                           0x00
#define V_PCH_LPC_RID_1                           0x01
#define V_PCH_LPC_RID_9                           0x09
#define R_PCH_LPC_SERIRQ_CNT                      0x64
#define B_PCH_LPC_SERIRQ_CNT_SIRQEN               0x80
#define B_PCH_LPC_SERIRQ_CNT_SIRQMD               0x40
#define B_PCH_LPC_SERIRQ_CNT_SIRQSZ               0x3C
#define N_PCH_LPC_SERIRQ_CNT_SIRQSZ               2
#define B_PCH_LPC_SERIRQ_CNT_SFPW                 0x03
#define N_PCH_LPC_SERIRQ_CNT_SFPW                 0
#define V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK            0x00
#define V_PCH_LPC_SERIRQ_CNT_SFPW_6CLK            0x01
#define V_PCH_LPC_SERIRQ_CNT_SFPW_8CLK            0x02
#define R_PCH_LPC_IOD                             0x80
#define B_PCH_LPC_IOD_FDD                         0x1000
#define N_PCH_LPC_IOD_FDD                         12
#define V_PCH_LPC_IOD_FDD_3F0                     0
#define V_PCH_LPC_IOD_FDD_370                     1
#define B_PCH_LPC_IOD_LPT                         0x0300
#define N_PCH_LPC_IOD_LPT                         8
#define V_PCH_LPC_IOD_LPT_378                     0
#define V_PCH_LPC_IOD_LPT_278                     1
#define V_PCH_LPC_IOD_LPT_3BC                     2
#define B_PCH_LPC_IOD_COMB                        0x0070
#define N_PCH_LPC_IOD_COMB                        4
#define V_PCH_LPC_IOD_COMB_3F8                    0
#define V_PCH_LPC_IOD_COMB_2F8                    1
#define V_PCH_LPC_IOD_COMB_220                    2
#define V_PCH_LPC_IOD_COMB_228                    3
#define V_PCH_LPC_IOD_COMB_238                    4
#define V_PCH_LPC_IOD_COMB_2E8                    5
#define V_PCH_LPC_IOD_COMB_338                    6
#define V_PCH_LPC_IOD_COMB_3E8                    7
#define B_PCH_LPC_IOD_COMA                        0x0007
#define N_PCH_LPC_IOD_COMA                        0
#define V_PCH_LPC_IOD_COMA_3F8                    0
#define V_PCH_LPC_IOD_COMA_2F8                    1
#define V_PCH_LPC_IOD_COMA_220                    2
#define V_PCH_LPC_IOD_COMA_228                    3
#define V_PCH_LPC_IOD_COMA_238                    4
#define V_PCH_LPC_IOD_COMA_2E8                    5
#define V_PCH_LPC_IOD_COMA_338                    6
#define V_PCH_LPC_IOD_COMA_3E8                    7
#define R_PCH_LPC_IOE                             0x82
#define B_PCH_LPC_IOE_ME2                         BIT13
#define B_PCH_LPC_IOE_SE                          BIT12
#define B_PCH_LPC_IOE_ME1                         BIT11
#define B_PCH_LPC_IOE_KE                          BIT10
#define B_PCH_LPC_IOE_HGE                         BIT9
#define B_PCH_LPC_IOE_LGE                         BIT8
#define B_PCH_LPC_IOE_FDE                         BIT3
#define B_PCH_LPC_IOE_PPE                         BIT2
#define B_PCH_LPC_IOE_CBE                         BIT1
#define B_PCH_LPC_IOE_CAE                         BIT0
#define R_PCH_LPC_GEN1_DEC                        0x84
#define R_PCH_LPC_GEN2_DEC                        0x88
#define R_PCH_LPC_GEN3_DEC                        0x8C
#define R_PCH_LPC_GEN4_DEC                        0x90
#define B_PCH_LPC_GENX_DEC_IODRA                  0x00FC0000
#define B_PCH_LPC_GENX_DEC_IOBAR                  0x0000FFFC
#define B_PCH_LPC_GENX_DEC_EN                     0x00000001
#define R_PCH_LPC_ULKMC                           0x94
#define B_PCH_LPC_ULKMC_SMIBYENDPS                BIT15
#define B_PCH_LPC_ULKMC_TRAPBY64W                 BIT11
#define B_PCH_LPC_ULKMC_TRAPBY64R                 BIT10
#define B_PCH_LPC_ULKMC_TRAPBY60W                 BIT9
#define B_PCH_LPC_ULKMC_TRAPBY60R                 BIT8
#define B_PCH_LPC_ULKMC_SMIATENDPS                BIT7
#define B_PCH_LPC_ULKMC_PSTATE                    BIT6
#define B_PCH_LPC_ULKMC_A20PASSEN                 BIT5
#define B_PCH_LPC_ULKMC_USBSMIEN                  BIT4
#define B_PCH_LPC_ULKMC_64WEN                     BIT3
#define B_PCH_LPC_ULKMC_64REN                     BIT2
#define B_PCH_LPC_ULKMC_60WEN                     BIT1
#define B_PCH_LPC_ULKMC_60REN                     BIT0
#define R_PCH_LPC_LGMR                            0x98
#define B_PCH_LPC_LGMR_MA                         0xFFFF0000
#define B_PCH_LPC_LGMR_LMRD_EN                    BIT0

#define R_PCH_LPC_FWH_BIOS_SEL                    0xD0
#define B_PCH_LPC_FWH_BIOS_SEL_F8                 0xF0000000
#define B_PCH_LPC_FWH_BIOS_SEL_F0                 0x0F000000
#define B_PCH_LPC_FWH_BIOS_SEL_E8                 0x00F00000
#define B_PCH_LPC_FWH_BIOS_SEL_E0                 0x000F0000
#define B_PCH_LPC_FWH_BIOS_SEL_D8                 0x0000F000
#define B_PCH_LPC_FWH_BIOS_SEL_D0                 0x00000F00
#define B_PCH_LPC_FWH_BIOS_SEL_C8                 0x000000F0
#define B_PCH_LPC_FWH_BIOS_SEL_C0                 0x0000000F
#define R_PCH_LPC_FWH_BIOS_SEL2                   0xD4
#define B_PCH_LPC_FWH_BIOS_SEL2_70                0xF000
#define B_PCH_LPC_FWH_BIOS_SEL2_60                0x0F00
#define B_PCH_LPC_FWH_BIOS_SEL2_50                0x00F0
#define B_PCH_LPC_FWH_BIOS_SEL2_40                0x000F
#define R_PCH_LPC_BDE                             0xD8                          ///< BIOS decode enable
#define B_PCH_LPC_BDE_F8                          0x8000
#define B_PCH_LPC_BDE_F0                          0x4000
#define B_PCH_LPC_BDE_E8                          0x2000
#define B_PCH_LPC_BDE_E0                          0x1000
#define B_PCH_LPC_BDE_D8                          0x0800
#define B_PCH_LPC_BDE_D0                          0x0400
#define B_PCH_LPC_BDE_C8                          0x0200
#define B_PCH_LPC_BDE_C0                          0x0100
#define B_PCH_LPC_BDE_LEG_F                       0x0080
#define B_PCH_LPC_BDE_LEG_E                       0x0040
#define B_PCH_LPC_BDE_70                          0x0008
#define B_PCH_LPC_BDE_60                          0x0004
#define B_PCH_LPC_BDE_50                          0x0002
#define B_PCH_LPC_BDE_40                          0x0001
#define R_PCH_LPC_PCC                             0xE0
#define B_PCH_LPC_PCC_CLKRUN_EN                   0x0001
#define B_PCH_LPC_FVEC0_USB_PORT_CAP              0x00000C00
#define V_PCH_LPC_FVEC0_USB_14_PORT               0x00000000
#define V_PCH_LPC_FVEC0_USB_12_PORT               0x00000400
#define V_PCH_LPC_FVEC0_USB_10_PORT               0x00000800
#define B_PCH_LPC_FVEC0_SATA_RAID_CAP             0x00000080
#define B_PCH_LPC_FVEC0_SATA_PORT23_CAP           0x00000040
#define B_PCH_LPC_FVEC0_SATA_PORT1_6GB_CAP        0x00000008
#define B_PCH_LPC_FVEC0_SATA_PORT0_6GB_CAP        0x00000004
#define B_PCH_LPC_FVEC0_PCI_CAP                   0x00000002
#define R_PCH_LPC_FVEC1                           0x01
#define B_PCH_LPC_FVEC1_USB_R_CAP                 0x00400000
#define R_PCH_LPC_FVEC2                           0x02
#define B_PCH_LPC_FVEC2_IATT_CAP                  0x00400000
#define V_PCH_LPC_FVEC2_PCIE_PORT78_CAP           0x00200000
#define V_PCH_LPC_FVEC2_PCH_IG_SUPPORT_CAP        0x00020000
#define R_PCH_LPC_FVEC3                           0x03
#define B_PCH_LPC_FVEC3_DCMI_CAP                  0x00002000
#define B_PCH_LPC_FVEC3_NM_CAP                    0x00001000

//
// APM Registers
//
#define R_PCH_APM_CNT                             0xB2
#define R_PCH_APM_STS                             0xB3

#define R_PCH_LPC_BC                              0xDC            ///< Bios Control
#define S_PCH_LPC_BC                              1
#define B_PCH_LPC_BC_BILD                         BIT7            ///< BIOS Interface Lock-Down
#define B_PCH_LPC_BC_BBS                          BIT6            ///< Boot BIOS strap
#define N_PCH_LPC_BC_BBS                          6
#define V_PCH_LPC_BC_BBS_SPI                      0               ///< Boot BIOS strapped to SPI
#define V_PCH_LPC_BC_BBS_LPC                      1               ///< Boot BIOS strapped to LPC
#define B_PCH_LPC_BC_EISS                         BIT5            ///< Enable InSMM.STS
#define B_PCH_LPC_BC_TS                           BIT4            ///< Top Swap
#define B_PCH_LPC_BC_LE                           BIT1            ///< Lock Enable
#define N_PCH_LPC_BC_LE                           1
#define B_PCH_LPC_BC_WPD                          BIT0            ///< Write Protect Disable

#define R_PCH_ESPI_PCBC                           0xDC            ///< Peripheral Channel BIOS Control
#define S_PCH_ESPI_PCBC                           4               ///< Peripheral Channel BIOS Control register size
#define B_PCH_ESPI_PCBC_BWRE                      BIT11           ///< BIOS Write Report Enable
#define N_PCH_ESPI_PCBC_BWRE                      11              ///< BIOS Write Report Enable bit position
#define B_PCH_ESPI_PCBC_BWRS                      BIT10           ///< BIOS Write Report Status
#define N_PCH_ESPI_PCBC_BWRS                      10              ///< BIOS Write Report Status bit position
#define B_PCH_ESPI_PCBC_BWPDS                     BIT8            ///< BIOS Write Protect Disable Status
#define N_PCH_ESPI_PCBC_BWPDS                     8               ///< BIOS Write Protect Disable Status bit position
#define B_PCH_ESPI_PCBC_ESPI_EN                   BIT2            ///< eSPI Enable Pin Strap
#define B_PCH_ESPI_PCBC_LE                        BIT1            ///< Lock Enable

//
// Processor interface registers
//
#define R_PCH_NMI_SC                              0x61
#define B_PCH_NMI_SC_SERR_NMI_STS                 BIT7
#define B_PCH_NMI_SC_IOCHK_NMI_STS                BIT6
#define B_PCH_NMI_SC_TMR2_OUT_STS                 BIT5
#define B_PCH_NMI_SC_REF_TOGGLE                   BIT4
#define B_PCH_NMI_SC_IOCHK_NMI_EN                 BIT3
#define B_PCH_NMI_SC_PCI_SERR_EN                  BIT2
#define B_PCH_NMI_SC_SPKR_DAT_EN                  BIT1
#define B_PCH_NMI_SC_TIM_CNT2_EN                  BIT0
#define R_PCH_NMI_EN                              0x70
#define B_PCH_NMI_EN_NMI_EN                       BIT7

//
// Reset Generator I/O Port
//
#define R_PCH_RST_CNT                             0xCF9
#define B_PCH_RST_CNT_FULL_RST                    BIT3
#define B_PCH_RST_CNT_RST_CPU                     BIT2
#define B_PCH_RST_CNT_SYS_RST                     BIT1
#define V_PCH_RST_CNT_FULLRESET                   0x0E
#define V_PCH_RST_CNT_HARDRESET                   0x06
#define V_PCH_RST_CNT_SOFTRESET                   0x04
#define V_PCH_RST_CNT_HARDSTARTSTATE              0x02
#define V_PCH_RST_CNT_SOFTSTARTSTATE              0x00

//
// RTC register
//
#define R_PCH_RTC_INDEX                           0x70
#define R_PCH_RTC_TARGET                          0x71
#define R_PCH_RTC_EXT_INDEX                       0x72
#define R_PCH_RTC_EXT_TARGET                      0x73
#define R_PCH_RTC_REGA                            0x0A
#define B_PCH_RTC_REGA_UIP                        0x80
#define R_PCH_RTC_REGB                            0x0B
#define B_PCH_RTC_REGB_SET                        0x80
#define B_PCH_RTC_REGB_PIE                        0x40
#define B_PCH_RTC_REGB_AIE                        0x20
#define B_PCH_RTC_REGB_UIE                        0x10
#define B_PCH_RTC_REGB_DM                         0x04
#define B_PCH_RTC_REGB_HOURFORM                   0x02
#define R_PCH_RTC_REGC                            0x0C
#define R_PCH_RTC_REGD                            0x0D

//
// Private Configuration Register
// RTC PCRs (PID:RTC)
//
#define R_PCH_PCR_RTC_CONF                        0x3400               ///< RTC Configuration register
#define S_PCH_PCR_RTC_CONF                        4
#define B_PCH_PCR_RTC_CONF_BILD                   BIT31                ///< BIOS Interface Lock-Down
#define B_PCH_PCR_RTC_CONF_HPM_HW_DIS             BIT6                 ///< RTC High Power Mode HW Disable
#define B_PCH_PCR_RTC_CONF_UCMOS_LOCK             BIT4                 ///< Partial Range Lock in Upper 128 Bytes
#define B_PCH_PCR_RTC_CONF_LCMOS_LOCK             BIT3                 ///< Partial Range Lock in Lower 128 Bytes
#define B_PCH_PCR_RTC_CONF_UCMOS_EN               BIT2                 ///< Upper CMOS bank enable
#define R_PCH_PCR_RTC_RTCDCG                      0x3418               ///< RTC Dynamic Clock Gating Control
#define B_PCH_PCR_RTC_RTCDCG_RTCPGCBDCGEN         BIT2                 ///< pgcb_clk (12MHz) Dynamic Clock Gate Enable
#define B_PCH_PCR_RTC_RTCDCG_RTCPCICLKDCGEN       BIT1                 ///< ipciclk_clk (24 MHz) Dynamic Clock Gate Enable
#define B_PCH_PCR_RTC_RTCDCG_RTCROSIDEDCGEN       BIT0                 ///< rosc_side_clk (120 MHz) Dynamic Clock Gate Enable

//
// LPC PCR Registers
//
#define R_PCH_PCR_LPC_GCFD                        0x3418

#endif

