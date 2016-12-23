/** @file
  Register names for SATA controllers.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REGS_SATA_H_
#define _REGS_SATA_H_

///
///  SATA Controller Registers (D19:F0)
///
#define PCI_BUS_NUMBER_SATA             0
#define PCI_DEVICE_NUMBER_SATA          18
#define PCI_FUNCTION_NUMBER_SATA        0

#define R_SATA_ID                       0x00       ///< Identifiers
#define B_SATA_ID_DID                   0xFFFF0000 ///< Device ID
#define B_SATA_ID_VID                   0x0000FFFF ///< Vendor ID
#define V_SATA_VENDOR_ID                V_INTEL_VENDOR_ID
#define V_SATA_DEVICE_ID_BXTP_AHCI      0x5AE3 ///< Broxton AHCI Mode (Ports 0 and 1)

#define R_SATA_COMMAND                  0x04  ///< Command
#define B_SATA_COMMAND_INT_DIS          BIT10 ///< Interrupt Disable
#define B_SATA_COMMAND_FBE              BIT9  ///< Fast Back-to-back Enable
#define B_SATA_COMMAND_SERR_EN          BIT8  ///< SERR# Enable
#define B_SATA_COMMAND_WCC              BIT7  ///< Wait Cycle Enable
#define B_SATA_COMMAND_PER              BIT6  ///< Parity Error Response Enable
#define B_SATA_COMMAND_VPS              BIT5  ///< VGA Palette Snooping Enable
#define B_SATA_COMMAND_PMWE             BIT4  ///< Memory Write and Invalidate Enable
#define B_SATA_COMMAND_SCE              BIT3  ///< Special Cycle Enable
#define B_SATA_COMMAND_BME              BIT2  ///< Bus Master Enable
#define B_SATA_COMMAND_MSE              BIT1  ///< Memory Space Enable
#define B_SATA_COMMAND_IOSE             BIT0  ///< I/O Space Enable
#define R_SATA_CC                       0x0A  ///< Class Code
#define B_SATA_CC_BCC                   0xFF00 ///< Base Class Code
#define B_SATA_CC_SCC                   0x00FF ///< Sub Class Code
#define V_SATA_CC_SCC_IDE               0x01
#define V_SATA_CC_SCC_AHCI              0x06
#define V_SATA_CC_SCC_RAID              0x04
#define R_SATA_AHCI_BAR                 0x24
#define B_SATA_AHCI_BAR_BA              0xFFFFF800
#define V_SATA_AHCI_BAR_LENGTH          0x800
#define N_SATA_AHCI_BAR_ALIGNMENT       11
#define V_SATA_AHCI_BAR_LENGTH_64K      0x10000
#define N_SATA_AHCI_BAR_ALIGNMENT_64K   16
#define B_SATA_AHCI_BAR_PF              BIT3
#define B_SATA_AHCI_BAR_TP              (BIT2 | BIT1)
#define B_SATA_AHCI_BAR_RTE             BIT0
#define R_SATA_PID                      0x70
#define B_SATA_PID_NEXT                 0xFF00
#define V_SATA_PID_NEXT_0               0xB000
#define V_SATA_PID_NEXT_1               0xA800
#define B_SATA_PID_CID                  0x00FF
#define R_SATA_SS                       0x2C  ///< Sub System Identifiers
#define B_SATA_SS_SSID                  0xFFFF0000 ///< Subsystem ID
#define B_SATA_SS_SSVID                 0x0000FFFF ///< Subsystem Vendor ID
#define R_SATA_PC                       0x72
#define S_SATA_PC                       2
#define B_SATA_PC_PME                   (BIT15 | BIT14 | BIT13 | BIT12 | BIT11)
#define V_SATA_PC_PME_0                 0x0000
#define V_SATA_PC_PME_1                 0x4000
#define B_SATA_PC_D2_SUP                BIT10
#define B_SATA_PC_D1_SUP                BIT9
#define B_SATA_PC_AUX_CUR               (BIT8 | BIT7 | BIT6)
#define B_SATA_PC_DSI                   BIT5
#define B_SATA_PC_PME_CLK               BIT3
#define B_SATA_PC_VER                   (BIT2 | BIT1 | BIT0)
#define R_SATA_PMCS                     0x74
#define B_SATA_PMCS_PMES                BIT15
#define B_SATA_PMCS_PMEE                BIT8
#define B_SATA_PMCS_NSFRST              BIT3
#define V_SATA_PMCS_NSFRST_1            0x01
#define V_SATA_PMCS_NSFRST_0            0x00
#define B_SATA_PMCS_PS                  (BIT1 | BIT0)
#define V_SATA_PMCS_PS_3                0x03
#define V_SATA_PMCS_PS_0                0x00
#define R_SATA_MID                      0x80
#define B_SATA_MID_NEXT                 0xFF00
#define B_SATA_MID_CID                  0x00FF
#define R_SATA_MC                       0x82
#define B_SATA_MC_C64                   BIT7
#define B_SATA_MC_MME                   (BIT6 | BIT5 | BIT4)
#define V_SATA_MC_MME_4                 0x04
#define V_SATA_MC_MME_2                 0x02
#define V_SATA_MC_MME_1                 0x01
#define V_SATA_MC_MME_0                 0x00
#define B_SATA_MC_MMC                   (BIT3 | BIT2 | BIT1)
#define V_SATA_MC_MMC_4                 0x04
#define V_SATA_MC_MMC_0                 0x00
#define B_SATA_MC_MSIE                  BIT0
#define V_SATA_MC_MSIE_1                0x01
#define V_SATA_MC_MSIE_0                0x00
#define R_SATA_MD                       0x88
#define B_SATA_MD_MSIMD                 0xFFFF

#define R_SATA_MAP                   0x90
#define B_SATA_MAP_SPD               (BIT18 | BIT17 | BIT16)
#define N_SATA_MAP_SPD               16
#define B_SATA_MAP_SPD2              BIT18
#define B_SATA_MAP_SPD1              BIT17
#define B_SATA_MAP_SPD0              BIT16
#define B_SATA_MAP_PCD                0xFF
#define B_SATA_MAP_PORT7_PCD          BIT7
#define B_SATA_MAP_PORT6_PCD          BIT6
#define B_SATA_MAP_PORT5_PCD          BIT5
#define B_SATA_MAP_PORT4_PCD          BIT4
#define B_SATA_MAP_PORT3_PCD          BIT3
#define B_SATA_MAP_PORT2_PCD          BIT2
#define B_SATA_MAP_PORT1_PCD          BIT1
#define B_SATA_MAP_PORT0_PCD          BIT0

#define R_SATA_SATAGC                   0x9C
#define B_SATA_SATAGC_SMS_MASK        BIT16
#define N_SATA_SATAGC_SMS_MASK        16
#define V_SATA_SATAGC_SMS_AHCI        0x0
#define V_SATA_SATAGC_SMS_RAID        0x1
#define B_SATA_SATAGC_AIE               BIT7
#define B_SATA_SATAGC_AIES              BIT6
#define B_SATA_SATAGC_MSS               (BIT4 | BIT3)
#define V_SATA_SATAGC_MSS_8K            0x2
#define N_SATA_SATAGC_MSS               3
#define B_SATA_SATAGC_ASSEL             (BIT2 | BIT1 | BIT0)
#define V_SATA_SATAGC_ASSEL_64K         0x3

#define R_SATA_SIRI                     0xA0
#define R_SATA_STRD                     0xA4

#define R_SATA_SIR_70                   0x70
#define R_SATA_SIR_80                   0x80
#define R_SATA_SIR_88                   0x88
#define R_SATA_SIR_90                   0x90
#define R_SATA_SIR_9C                   0x9C
#define R_SATA_SIR_A0                   0xA0
#define R_SATA_SIR_A4                   0xA4
#define R_SATA_SIR_A8                   0xA8
#define R_SATA_SIR_D0                   0xD0
#define R_SATA_SIR_D4                   0xD4
#define B_SATA_STRD_DTA                 0xFFFFFFFF
#define R_SATA_CR0                      0xA8
#define B_SATA_CR0_MAJREV               0x00F00000
#define B_SATA_CR0_MINREV               0x000F0000
#define B_SATA_CR0_NEXT                 0x0000FF00
#define B_SATA_CR0_CAP                  0x000000FF
#define R_SATA_CR1                      0xAC
#define B_SATA_CR1_BAROFST              0xFFF0
#define B_SATA_CR1_BARLOC               0x000F
#define R_SATA_FLR_CID                  0xB0
#define B_SATA_FLR_CID_NEXT             0xFF00
#define B_SATA_FLR_CID                  0x00FF
#define V_SATA_FLR_CID_1                0x0009
#define V_SATA_FLR_CID_0                0x0013
#define R_SATA_FLR_CLV                  0xB2
#define B_SATA_FLR_CLV_FLRC_FLRCSSEL_0  BIT9
#define B_SATA_FLR_CLV_TXPC_FLRCSSEL_0  BIT8
#define B_SATA_FLR_CLV_VSCID_FLRCSSEL_0 0x00FF
#define B_SATA_FLR_CLV_VSCID_FLRCSSEL_1 0x00FF
#define V_SATA_FLR_CLV_VSCID_FLRCSSEL   0x0006
#define R_SATA_FLRC                     0xB4
#define B_SATA_FLRC_TXP                 BIT8
#define B_SATA_FLRC_INITFLR             BIT0
#define R_SATA_SP                       0xC0
#define B_SATA_SP                       0xFFFFFFFF
#define R_SATA_MXID                     0xD0
#define N_SATA_MXID_NEXT                8

#define R_SATA_BFCS                     0xE0
#define B_SATA_BFCS_P7BFI               BIT17
#define B_SATA_BFCS_P6BFI               BIT16
#define B_SATA_BFCS_P5BFI               BIT15
#define B_SATA_BFCS_P4BFI               BIT14
#define B_SATA_BFCS_P3BFI               BIT13
#define B_SATA_BFCS_P2BFI               BIT12
#define B_SATA_BFCS_P2BFS               BIT11
#define B_SATA_BFCS_P2BFF               BIT10
#define B_SATA_BFCS_P1BFI               BIT9
#define B_SATA_BFCS_P0BFI               BIT8
#define B_SATA_BFCS_BIST_FIS_T          BIT7
#define B_SATA_BFCS_BIST_FIS_A          BIT6
#define B_SATA_BFCS_BIST_FIS_S          BIT5
#define B_SATA_BFCS_BIST_FIS_L          BIT4
#define B_SATA_BFCS_BIST_FIS_F          BIT3
#define B_SATA_BFCS_BIST_FIS_P          BIT2
#define R_SATA_BFTD1                    0xE4
#define B_SATA_BFTD1                    0xFFFFFFFF
#define R_SATA_BFTD2                    0xE8
#define B_SATA_BFTD2                    0xFFFFFFFF

#define R_SATA_VS_CAP                   0xA4
#define B_SATA_VS_CAP_NRMBE             BIT0                            ///< NVM Remap Memory BAR Enable
#define B_SATA_VS_CAP_MSL               0x1FFE                          ///< Memory Space Limit
#define N_SATA_VS_CAP_MSL               1
#define V_SATA_VS_CAP_MSL               0x1EF                           ///< Memory Space Limit Field Value
#define B_SATA_VS_CAP_NRMO              0xFFF0000                       ///< NVM Remapped Memory Offset
#define N_SATA_VS_CAP_NRMO              16
#define V_SATA_VS_CAP_NRMO              0x10                            ///< NVM Remapped Memory Offset Field Value

///
/// Memory AHCI BAR Area Related Registers
///
#define R_SATA_AHCI_CAP                 0x0   ///< HBA Capabilities
#define B_SATA_AHCI_CAP_S64A            BIT31 ///< Supports 64-bit Addressing
#define B_SATA_AHCI_CAP_SCQA            BIT30 ///< Support Native Command Queuing Acceleration
#define B_SATA_AHCI_CAP_SSNTF           BIT29 ///< Supports SNotification Register
#define B_SATA_AHCI_CAP_SIS             BIT28 ///< Supports Mechanical Presence (Formerly Interlock Switch)
#define B_SATA_AHCI_CAP_SSS             BIT27 ///< Supports Staggered Spin-up
#define B_SATA_AHCI_CAP_SALP            BIT26 ///< Supports Aggressive Link Power Management
#define B_SATA_AHCI_CAP_SAL             BIT25 ///< Supports Activity LED
#define B_SATA_AHCI_CAP_SCLO            BIT24 ///< Supports Command List Override
#define B_SATA_AHCI_CAP_ISS_MASK        (BIT23 | BIT22 | BIT21 | BIT20) ///< Interface Speed Support
#define N_SATA_AHCI_CAP_ISS             20    ///< Interface Speed Support
#define V_SATA_AHCI_CAP_ISS_6_0_G       0x03
#define V_SATA_AHCI_CAP_ISS_3_0_G       0x02  ///< Gen 2 (3.0 Gbps)
#define V_SATA_AHCI_CAP_ISS_1_5_G       0x01  ///< Gen 1 (1.5 Gbps)
#define B_SATA_AHCI_CAP_SNZO            BIT19 ///< Supports Non-Zero DMA Offsets
#define B_SATA_AHCI_CAP_SAM             BIT18 ///< Supports AHCI mode only
#define B_SATA_AHCI_CAP_PMS             BIT17 ///< Supports Port Multiplier

#define B_SATA_AHCI_CAP_PMD             BIT15 ///< PIO Multiple DRQ Block
#define B_SATA_AHCI_CAP_SSC             BIT14 ///< Slumber Slate Capable
#define B_SATA_AHCI_CAP_PSC             BIT13 ///< Partial State Capable
#define B_SATA_AHCI_CAP_NCS             0x1F00 ///< Indicating Support for 32 slots
#define B_SATA_AHCI_CAP_CCCS            BIT7  ///< Command Completion Coalescing Supported
#define B_SATA_AHCI_CAP_EMS             BIT6  ///< Enclosure Management Supported
#define B_SATA_AHCI_CAP_SXS             BIT5  ///< Supports External SATA
#define B_SATA_AHCI_CAP_NPS             0x001F

#define R_SATA_AHCI_GHC                 0x04  ///< Global HBA Control
#define B_SATA_AHCI_GHC_AE              BIT31 ///< AHCI Enable
#define B_SATA_AHCI_GHC_PITO            0xFF00
#define B_SATA_AHCI_GHC_MRSM            BIT2  ///< MSI Revert to Single Message
#define B_SATA_AHCI_GHC_IE              BIT1  ///< Interrupt Enable
#define B_SATA_AHCI_GHC_HR              BIT0  ///< HBA Reset

#define R_SATA_AHCI_IS                  0x08  ///< Interrupt Status Register
#define B_SATA_AHCI_IS_PORT5            BIT5  ///< Interrupt Pending Status Port 5
#define B_SATA_AHCI_IS_PORT4            BIT4  ///< Interrupt Pending Status Port 4
#define B_SATA_AHCI_IS_PORT3            BIT3  ///< Interrupt Pending Status Port 3
#define B_SATA_AHCI_IS_PORT2            BIT2  ///< Interrupt Pending Status Port 2
#define B_SATA_AHCI_IS_PORT1            BIT1  ///< Interrupt Pending Status Port 1
#define B_SATA_AHCI_IS_PORT0            BIT0  ///< Interrupt Pending Status Port 0

#define R_SATA_AHCI_PI                  0x0C  ///< Ports Implemented
#define B_SATA_PORT_MASK                0x03
#define B_SATA_PORT5_IMPLEMENTED        BIT5  ///< Port 5 Implemented
#define B_SATA_PORT4_IMPLEMENTED        BIT4  ///< Port 4 Implemented
#define B_SATA_PORT3_IMPLEMENTED        BIT3  ///< Port 3 Implemented
#define B_SATA_PORT2_IMPLEMENTED        BIT2  ///< Port 2 Implemented
#define B_SATA_PORT1_IMPLEMENTED        BIT1  ///< Port 1 Implemented
#define B_SATA_PORT0_IMPLEMENTED        BIT0  ///< Port 0 Implemented

#define R_SATA_AHCI_VS                  0x10  ///< AHCI Version
#define B_SATA_AHCI_VS_MJR              0xFFFF0000 ///< Major Version Number
#define B_SATA_AHCI_VS_MNR              0x0000FFFF ///< Minor Version Number

#define R_SATA_AHCI_EM_LOC              0x1C  ///< Enclosure Management Location
#define B_SATA_AHCI_EM_LOC_OFST         0xFFFF0000 ///< Offset
#define B_SATA_AHCI_EM_LOC_SZ           0x0000FFFF ///< Buffer Size

#define R_SATA_AHCI_EM_CTRL             0x20  ///< Enclosure Management Control
#define B_SATA_AHCI_EM_CTRL_ATTR_PM     BIT27 ///< Port Multiplier Support
#define B_SATA_AHCI_EM_CTRL_ATTR_ALHD   BIT26 ///< Activity LED Hardware Driven
#define B_SATA_AHCI_EM_CTRL_ATTR_XMT    BIT25 ///< Transmit Only
#define B_SATA_AHCI_EM_CTRL_ATTR_SMB    BIT24 ///< Single Message Buffer
#define B_SATA_AHCI_EM_CTRL_SUPP_SGPIO  BIT19 ///< SGPIO Enclosure Management Messages
#define B_SATA_AHCI_EM_CTRL_SUPP_SES2   BIT18 ///< SES-2 Enclosure Management Messages
#define B_SATA_AHCI_EM_CTRL_SUPP_SAFTE  BIT17 ///< SAF-TE Enclosure Management Messages
#define B_SATA_AHCI_EM_CTRL_SUPP_LED    BIT16 ///< LED Message Types
#define B_SATA_AHCI_EM_CTRL_RST         BIT9  ///< Reset
#define B_SATA_AHCI_EM_CTRL_CTL_TM      BIT8  ///< Transmit Message
#define B_SATA_AHCI_EM_CTRL_STS_MR      BIT0  ///< Message Received

#define R_SATA_AHCI_CAP2                0x24  ///< HBA Capabilities Extended
#define B_SATA_AHCI_CAP2_DESO           BIT5
#define B_SATA_AHCI_CAP2_SADM           BIT4
#define B_SATA_AHCI_CAP2_SDS            BIT3
#define B_SATA_AHCI_CAP2_APST           BIT2  ///< Automatic Partial to Slumber Transitions
#define B_SATA_AHCI_CAP2_BOH            BIT0  ///< BIOS / OS Handoff (Not Supported)

#define R_SATA_AHCI_VSP                 0xA0  ///< Vendor Specific
#define B_SATA_AHCI_VSP_SFMS            BIT6  ///< Software Feature Mask Supported
#define B_SATA_AHCI_VSP_PFS             BIT5  ///< Premium Features Supported
#define B_SATA_AHCI_VSP_PT              BIT4  ///< Platform Type
#define B_SATA_AHCI_VSP_SRPIR           BIT3  ///< Supports RAID Platform ID Reporting

#define R_SATA_AHCI_VSCAP               0xA4  ///< Vendor Specific Capabilities Register
#define B_SATA_AHCI_VSCAP_PNRRO         0xFFFF0000 ///< PCIe NAND Remapped Register Offset
#define B_SATA_AHCI_VSCAP_MSL           0x00000FFE
#define B_SATA_AHCI_VSCAP_PNABRE        BIT0  ///< PCIe NAND AHCI BAR Remapped Enable

#define R_SATA_AHCI_RPID                0xC0  ///< RAID Platform ID
#define B_SATA_AHCI_RPID_OFST           0xFFFF0000 ///< Offset
#define B_SATA_AHCI_RPID_RPID           0x0000FFFF ///< RAID Platform ID

#define R_SATA_AHCI_PFB                 0xC4  ///< Premium Feature Block
#define B_SATA_AHCI_PFB_SEA             BIT1  ///< Supports Email Alert
#define B_SATA_AHCI_PFB_SOI             BIT0  ///< Supports OEM IOCTL

#define R_SATA_AHCI_SFM                 0xC8  ///< SW Feature Mask
#define B_SATA_AHCI_SFM_OUND            (BIT11 | BIT10) ///< OROM UI Normal Delay
#define B_SATA_AHCI_SFM_SRT             BIT9  ///< Smart Response Technology
#define B_SATA_AHCI_SFM_IROES           BIT8  ///< IRRT Only on ESATA
#define B_SATA_AHCI_SFM_LEDL            BIT7  ///< LED Locate
#define B_SATA_AHCI_SFM_HDDLK           BIT6  ///< HDD Unlock
#define B_SATA_AHCI_SFM_OROMUNB         BIT5  ///< OROM UI and Banner
#define B_SATA_AHCI_SFM_IRRT            BIT4  ///< IRRT
#define B_SATA_AHCI_SFM_R5E             BIT3  ///< R5 Enable
#define B_SATA_AHCI_SFM_R10E            BIT2  ///< R10 Enable
#define B_SATA_AHCI_SFM_R1E             BIT1  ///< R1 Enable
#define B_SATA_AHCI_SFM_R0E             BIT0  ///< R0 Enable
#define B_SATA_AHCI_SFM_LOWBYTES        0x1FF

#define R_SATA_AHCI_P0CLB               0x100 ///< Port 0 Command List Base Address
#define R_SATA_AHCI_P1CLB               0x180 ///< Port 1 Command List Base Address
#define B_SATA_AHCI_PXCLB               0xFFFFFC00  ///< Command List Base Address

#define R_SATA_AHCI_P0CLBU              0x104 ///< Port 0 Command List Base Address Upper 32-bits
#define R_SATA_AHCI_P1CLBU              0x184 ///< Port 1 Command List Base Address Upper 32-bits
#define B_SATA_AHCI_PXCLBU              0xFFFFFFFF ///< Command List Base Address Upper

#define R_SATA_AHCI_P0FB                0x108 ///< Port 0 FIS Base Address
#define R_SATA_AHCI_P1FB                0x188 ///< Port 1 FIS Base Address
#define B_SATA_AHCI_PXFB                0xFFFFFF00 ///< FIS Base Address

#define R_SATA_AHCI_P0FBU               0x10C ///< Port 0 FIS Base Address Upper 32-bits
#define R_SATA_AHCI_P1FBU               0x18C ///< Port 1 FIS Base Address Upper 32-bits
#define B_SATA_AHCI_PXFBU               0xFFFFFFFF ///< FIS Base Address Upper

#define R_SATA_AHCI_P0IS                0x110 ///< Port 0 Interrupt Status
#define R_SATA_AHCI_P1IS                0x190 ///< Port 1 Interrupt Status
#define B_SATA_AHCI_PXIS_CPDS           BIT31 ///< Cold Presence Detect Status
#define B_SATA_AHCI_PXIS_TFES           BIT30 ///< Task File Error Status
#define B_SATA_AHCI_PXIS_HBFS           BIT29 ///< Host Bus Fatal Error Status
#define B_SATA_AHCI_PXIS_HBDS           BIT28 ///< Host Bus Data Error Status
#define B_SATA_AHCI_PXIS_IFS            BIT27 ///< Interface Fatal Error Status
#define B_SATA_AHCI_PXIS_INFS           BIT26 ///< Interface Non-Fatal Error Status
#define B_SATA_AHCI_PXIS_OFS            BIT24 ///< Overflow Status
#define B_SATA_AHCI_PXIS_IPMS           BIT23 ///< Incorrect Port Multiplier Status
#define B_SATA_AHCI_PXIS_PRCS           BIT22 ///< PhyRdy Change Status
#define B_SATA_AHCI_PXIS_DMPS           BIT7  ///< Device Mechanical Presence Status (Formerly Interlock Switch)
#define B_SATA_AHCI_PXIS_PCS            BIT6  ///< Port Connect Change Status
#define B_SATA_AHCI_PXIS_DPS            BIT5  ///< Descriptor Processed
#define B_SATA_AHCI_PXIS_UFS            BIT4  ///< Unknown FIS Interrupt
#define B_SATA_AHCI_PXIS_SDBS           BIT3  ///< Set Device Bits Interrupt
#define B_SATA_AHCI_PXIS_DSS            BIT2  ///< DMA Setup FIS Interrupt
#define B_SATA_AHCI_PXIS_PSS            BIT1  ///< PIO Setup FIS Interrupt
#define B_SATA_AHCI_PXIS_DHRS           BIT0  ///< Device to Host Register FIS Interrupt

#define R_SATA_AHCI_P0IE                0x114 ///< Port 0 Interrupt Enable
#define R_SATA_AHCI_P1IE                0x194 ///< Port 1 Interrupt Enable
#define B_SATA_AHCI_PXIE_CPDE           BIT31 ///< Cold Presence Detect Enable
#define B_SATA_AHCI_PXIE_TFEE           BIT30 ///< Task File Error Enable
#define B_SATA_AHCI_PXIE_HBFE           BIT29 ///< Host Bus Fatal Error Enable
#define B_SATA_AHCI_PXIE_HBDE           BIT28 ///< Host Bus Data Error Enable
#define B_SATA_AHCI_PXIE_IFE            BIT27 ///< Interface Fatal Error Enable
#define B_SATA_AHCI_PXIE_INFE           BIT26 ///< Interface Non-Fatal Error Enable
#define B_SATA_AHCI_PXIE_OFE            BIT24 ///< Overflow Enable
#define B_SATA_AHCI_PXIE_IPME           BIT23 ///< Incorrect Port Multiplier Enable
#define B_SATA_AHCI_PXIE_PRCE           BIT22 ///< PhyRdy Change Interrupt Enable
#define B_SATA_AHCI_PXIE_DIE            BIT7  ///< Device Mechanical Enable (Formerly Interlock Switch)
#define B_SATA_AHCI_PXIE_PCE            BIT6  ///< Port Change Interrupt Enable
#define B_SATA_AHCI_PXIE_DPE            BIT5  ///< Descriptor Processed Interrupt Enable
#define B_SATA_AHCI_PXIE_UFIE           BIT4  ///< Unknown FIS Interrupt Enable
#define B_SATA_AHCI_PXIE_SDBE           BIT3  ///< Set Device Bits FIS Interrupt Enable
#define B_SATA_AHCI_PXIE_DSE            BIT2  ///< DMA Setup FIS Interrupt Enable
#define B_SATA_AHCI_PXIE_PSE            BIT1  ///< PIO Setup FIS Interrupt Enable
#define B_SATA_AHCI_PXIE_DHRE           BIT0  ///< Device to Host Register FIS Interrupt Enable

#define R_SATA_AHCI_P0CMD               0x118 ///< Port 0 Command
#define R_SATA_AHCI_P1CMD               0x198 ///< Port 1 Command
#define B_SATA_AHCI_PxCMD_ICC           (BIT31 | BIT30 | BIT29 | BIT28) ///< Interface Communication Control
#define B_SATA_AHCI_PxCMD_MASK          (BIT27 | BIT26 | BIT21 | BIT22 | BIT19 | BIT18)
#define B_SATA_AHCI_PxCMD_ASP           BIT27 ///< Aggressive Slumber Partial
#define B_SATA_AHCI_PxCMD_ALPE          BIT26 ///< Aggressive Link Power Management Enable
#define B_SATA_AHCI_PxCMD_DLAE          BIT25 ///< Drive LED on ATAPI Enable
#define B_SATA_AHCI_PxCMD_ATAPI         BIT24 ///< Device is ATAPI
#define B_SATA_AHCI_PxCMD_APSTE         BIT23 ///< Automatic Partial to Slumber Transitions Enable

#define B_SATA_AHCI_PxCMD_ESP           BIT21 ///< External SATA Port
#define B_SATA_AHCI_PxCMD_CPD           BIT20 ///< Cold Presence Detection
#define B_SATA_AHCI_PxCMD_MPSP          BIT19 ///< Mechanical Presence Switch Attached to Port
#define B_SATA_AHCI_PxCMD_HPCP          BIT18 ///< Hot Plug Capable Port

#define B_SATA_AHCI_PxCMD_CR            BIT15 ///< Command List Running
#define B_SATA_AHCI_PxCMD_FR            BIT14 ///< FIS Receive Running
#define B_SATA_AHCI_PxCMD_MPSS          BIT13 ///< Mechanical Presence Switch State (Formerly Interlock Switch)
#define B_SATA_AHCI_PxCMD_CCS           0x00001F00 ///< Current Command Slot
#define B_SATA_AHCI_PxCMD_FRE           BIT4  ///< FIS Receive Enable
#define B_SATA_AHCI_PxCMD_CLO           BIT3  ///< Command List Overide
#define B_SATA_AHCI_PxCMD_POD           BIT2  ///< Power On Device
#define B_SATA_AHCI_PxCMD_SUD           BIT1  ///< Spin-Up Device
#define B_SATA_AHCI_PxCMD_ST            BIT0  ///< Start

#define R_SATA_AHCI_P0TFD               0x120 ///< Port 0 Task File Data
#define R_SATA_AHCI_P1TFD               0x1A0 ///< Port 1 Task File Data
#define B_SATA_AHCI_PXTFD_ERR           0x0000FF00 ///< Error
#define B_SATA_AHCI_PXTFD_STS           0x000000FF ///< Status
#define B_SATA_AHCI_PXTFD_STS_BSY       BIT7  ///< Status Busy
#define B_SATA_AHCI_PXTFD_STS_DRQ       BIT3  ///< Status DRQ
#define B_SATA_AHCI_PXTFD_STS_ERR       BIT0  ///< Status Error

#define R_SATA_AHCI_P0SIG               0x124 ///< Port 0 Signature
#define R_SATA_AHCI_P1SIG               0x1A4 ///< Port 1 Signature
#define B_SATA_AHCI_PXSIG_LBA_HR        0xFF000000
#define B_SATA_AHCI_PXSIG_LBA_MR        0x00FF0000
#define B_SATA_AHCI_PXSIG_LBA_LR        0x0000FF00
#define B_SATA_AHCI_PXSIG_SCR           0x000000FF

#define R_SATA_AHCI_P0SSTS              0x128 ///< Port 0 Serial ATA Status
#define R_SATA_AHCI_P1SSTS              0x1A8 ///< Port 1 Serial ATA Status
#define B_SATA_AHCI_PXSSTS_IPM          0x00000F00 ///< Interface Power Management
#define B_SATA_AHCI_PXSSTS_IPM_0        0x00000000
#define B_SATA_AHCI_PXSSTS_IPM_1        0x00000100
#define B_SATA_AHCI_PXSSTS_IPM_2        0x00000200
#define B_SATA_AHCI_PXSSTS_IPM_6        0x00000600
#define B_SATA_AHCI_PXSSTS_SPD          0x000000F0 ///< Current Interface Speed
#define B_SATA_AHCI_PXSSTS_SPD_0        0x00000000
#define B_SATA_AHCI_PXSSTS_SPD_1        0x00000010
#define B_SATA_AHCI_PXSSTS_SPD_2        0x00000020
#define B_SATA_AHCI_PXSSTS_SPD_3        0x00000030
#define B_SATA_AHCI_PXSSTS_DET          0x0000000F ///< Device Detection
#define B_SATA_AHCI_PXSSTS_DET_0        0x00000000
#define B_SATA_AHCI_PXSSTS_DET_1        0x00000001
#define B_SATA_AHCI_PXSSTS_DET_3        0x00000003
#define B_SATA_AHCI_PXSSTS_DET_4        0x00000004

#define R_SATA_AHCI_P0SCTL              0x12C ///< Port 0 Serial ATA Control
#define R_SATA_AHCI_P1SCTL              0x1AC ///< Port 1 Serial ATA Control
#define B_SATA_AHCI_PXSCTL_IPM          0x00000F00 ///< Interface Power Management Transitions Allowed
#define V_SATA_AHCI_PXSCTL_IPM_0        0x00000000
#define V_SATA_AHCI_PXSCTL_IPM_1        0x00000100
#define V_SATA_AHCI_PXSCTL_IPM_2        0x00000200
#define V_SATA_AHCI_PXSCTL_IPM_3        0x00000300
#define B_SATA_AHCI_PXSCTL_SPD          0x000000F0 ///< Speed Allowed
#define V_SATA_AHCI_PXSCTL_SPD_0        0x00000000
#define V_SATA_AHCI_PXSCTL_SPD_1        0x00000010
#define V_SATA_AHCI_PXSCTL_SPD_2        0x00000020
#define V_SATA_AHCI_PXSCTL_SPD_3        0x00000030
#define B_SATA_AHCI_PXSCTL_DET          0x0000000F ///< Device Detection Initialization
#define V_SATA_AHCI_PXSCTL_DET_0        0x00000000
#define V_SATA_AHCI_PXSCTL_DET_1        0x00000001
#define V_SATA_AHCI_PXSCTL_DET_4        0x00000004

#define R_SATA_AHCI_P0SERR              0x130 ///< Port 0 Serial ATA Error
#define R_SATA_AHCI_P1SERR              0x1B0 ///< Port 1 Serial ATA Error
#define B_SATA_AHCI_PXSERR_DIAG         0xFFFF0000 ///< Diagnostics
#define B_SATA_AHCI_PXSERR_ERR          0x0000FFFF ///< Error
#define B_SATA_AHCI_PXSERR_EXCHG        BIT26
#define B_SATA_AHCI_PXSERR_UN_FIS_TYPE  BIT25
#define B_SATA_AHCI_PXSERR_TRSTE_24     BIT24
#define B_SATA_AHCI_PXSERR_TRSTE_23     BIT23
#define B_SATA_AHCI_PXSERR_HANDSHAKE    BIT22
#define B_SATA_AHCI_PXSERR_CRC_ERROR    BIT21
#define B_SATA_AHCI_PXSERR_10B8B_DECERR BIT19
#define B_SATA_AHCI_PXSERR_COMM_WAKE    BIT18
#define B_SATA_AHCI_PXSERR_PHY_ERROR    BIT17
#define B_SATA_AHCI_PXSERR_PHY_RDY_CHG  BIT16
#define B_SATA_AHCI_PXSERR_INTRNAL_ERR  BIT11
#define B_SATA_AHCI_PXSERR_PROTOCOL_ERR BIT10
#define B_SATA_AHCI_PXSERR_PCDIE        BIT9
#define B_SATA_AHCI_PXSERR_TDIE         BIT8
#define B_SATA_AHCI_PXSERR_RCE          BIT1
#define B_SATA_AHCI_PXSERR_RDIE         BIT0

#define R_SATA_AHCI_P0SACT              0x134 ///< Port 0 Serial ATA Active
#define R_SATA_AHCI_P1SACT              0x1B4 ///< Port 1 Serial ATA Active
#define B_SATA_AHCI_PXSACT_DS           0xFFFFFFFF

#define R_SATA_AHCI_P0CI                0x138 ///< Port 0 Commands Issued
#define R_SATA_AHCI_P1CI                0x1B8 ///< Port 1 Commands Issued
#define B_SATA_AHCI_PXCI                0xFFFFFFFF

#define R_SATA_AHCI_P0DEVSLP            0x144 ///< Port [0-5] Device Sleep
#define R_SATA_AHCI_P1DEVSLP            0x1C4 ///< Port [0-5] Device Sleep

#define B_SATA_AHCI_PxDEVSLP_DSP        BIT1
#define B_SATA_AHCI_PxDEVSLP_ADSE       BIT0
#define B_SATA_AHCI_PxDEVSLP_DITO_MASK  0x01FF8000
#define V_SATA_AHCI_PxDEVSLP_DITO_625   0x01388000
#define B_SATA_AHCI_PxDEVSLP_DM_MASK    0x1E000000
#define V_SATA_AHCI_PxDEVSLP_DM_16      0x1E000000

#define R_SATA_AHCI_EM_MF               0x580 ///< Enclosure Management Message Format
#define B_SATA_AHCI_EM_MF_MTYPE         0x0F000000 ///< Message Type
#define B_SATA_AHCI_EM_MF_DSIZE         0x00FF0000 ///< Data Size
#define B_SATA_AHCI_EM_MF_MSIZE         0x0000FF00 ///< Message Size

#define R_SATA_AHCI_EM_LED              0x584 ///< Enclosure Management LED
#define B_SATA_AHCI_EM_LED_VAL          0xFFFF0000
#define B_SATA_AHCI_EM_LED_PM           0x0000FF00
#define B_SATA_AHCI_EM_LED_HBA          0x000000FF

///
/// Macros of capabilities for SATA controller which are used by SATA controller driver
///
///
///
/// Define the individual capabilities of each SATA controller
///
#define SATA_MAX_CONTROLLERS            1     ///< Max SATA controllers number supported
#define SATA_MAX_DEVICES                2     ///< Max SATA devices number of single SATA channel
#define IDE_MAX_CHANNELS                2     ///< Max IDE channels number of single SATA controller
#define IDE_MAX_DEVICES                 2     ///< Max IDE devices number of single SATA channel
#define AHCI_MAX_PORTS                  2     ///< Max number of SATA ports
#define IDE_MAX_PORTS                   2     ///< Max number of IDE ports

///
/// GPIOS_14 SATA0GP is the SATA port 0 reset pin.
///
#define GPIO_SATA_PORT0_RESET           14
///
/// GPIOS_15 SATA1GP is the SATA port 1 reset pin.
///
#define GPIO_SATA_PORT1_RESET           15

#endif

