/** @file
  Register names for USB devices.

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

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REGS_USB_H_
#define _REGS_USB_H_

///
/// USB3 (XHCI) related definitions
///
#define PCI_DEVICE_NUMBER_XHCI          21
#define PCI_FUNCTION_NUMBER_XHCI        0

///
/// XHCI PCI Config Space registers
///
#define R_XHCI_VENDOR_ID                0x00  ///< Vendor ID
#define B_XHCI_VENDOR_ID                0xFFFF

#define R_XHCI_DEVICE_ID                0x02  ///< Device ID
#define B_XHCI_DEVICE_ID                0xFFFF

#define R_XHCI_COMMAND_REGISTER         0x04  ///< Command
#define B_XHCI_COMMAND_ID               BIT10 ///< Interrupt Disable
#define B_XHCI_COMMAND_FBE              BIT9  ///< Fast Back to Back Enable
#define B_XHCI_COMMAND_SERR             BIT8  ///< SERR# Enable
#define B_XHCI_COMMAND_WCC              BIT7  ///< Wait Cycle Control
#define B_XHCI_COMMAND_PER              BIT6  ///< Parity Error Response
#define B_XHCI_COMMAND_VPS              BIT5  ///< VGA Palette Snoop
#define B_XHCI_COMMAND_MWI              BIT4  ///< Memory Write Invalidate
#define B_XHCI_COMMAND_SCE              BIT3  ///< Special Cycle Enable
#define B_XHCI_COMMAND_BME              BIT2  ///< Bus Master Enable
#define B_XHCI_COMMAND_MSE              BIT1  ///< Memory Space Enable

#define R_XHCI_MEM_BASE                 0x10  ///< Memory Base Address
#define B_XHCI_MEM_BASE_BA              0xFFFFFFFFFFFF0000 ///< Base Address
#define V_XHCI_MEM_LENGTH               0x10000 ///< 64 KB of Memory Length
#define N_XHCI_MEM_ALIGN                16    ///< Memory Space Alignment
#define B_XHCI_MEM_BASE_PREF            BIT3  ///< Prefetchable
#define B_XHCI_MEM_BASE_TYPE            (BIT2 | BIT1) ///< Type
#define B_XHCI_MEM_BASE_RTE             BIT0  ///< Resource Type Indicator

#define R_XHCI_SVID                     0x2C
#define B_XHCI_SVID                     0xFFFF

#define R_XHCI_SID                      0x2E
#define B_XHCI_SID                      0xFFFF

#define R_XHCI_INT_LN                   0x3C  ///< Interrupt Line
#define B_XHCI_INT_LN                   0xFF  ///< Interrupt Line Mask

#define R_XHCI_INT_PN                   0x3D  ///< Interrupt Pin
#define B_XHCI_INT_PN                   0xFF  ///< Interrupt Line Mask
#define V_XHCI_INT_PN                   BIT0  ///< Interrupt Pin Value (INTA)

#define R_XHCI_XHCC1                    0x40
#define B_XHCI_XHCC1_ACCTRL             BIT31
#define B_XHCI_XHCC1_RMTASERR           BIT24
#define B_XHCI_XHCC1_URD                BIT23
#define B_XHCI_XHCC1_URRE               BIT22
#define B_XHCI_XHCC1_IIL1E              (BIT21 | BIT20 | BIT19)
#define V_XHCI_XHCC1_IIL1E_DIS          0
#define V_XHCI_XHCC1_IIL1E_32           (BIT19)
#define V_XHCI_XHCC1_IIL1E_64           (BIT20)
#define V_XHCI_XHCC1_IIL1E_128          (BIT20 | BIT19)
#define V_XHCI_XHCC1_IIL1E_256          (BIT21)
#define V_XHCI_XHCC1_IIL1E_512          (BIT21 | BIT19)
#define V_XHCI_XHCC1_IIL1E_1024         (BIT21 | BIT20)
#define V_XHCI_XHCC1_IIL1E_131072       (BIT21 | BIT20 | BIT19)
#define B_XHCI_XHCC1_XHCIL1E            BIT18 ///< XHC Initiated L1 Enable
#define B_XHCI_XHCC1_D3IL1E             BIT17 ///< D3 Initiated L1 Enable
#define B_XHCI_XHCC1_UNPPA              (BIT16 | BIT15 | BIT14 | BIT13 | BIT12) ///< Periodic Complete Pre Wake Time
#define B_XHCI_XHCC1_SWAXHCI            BIT11 ///< SW Assisted xHC Idle
#define B_XHCI_XHCC1_L23HRAWC           (BIT10 | BIT9 | BIT8) ///< L23 to Host Reset Acknowledge Wait Count
#define V_XHCI_XHCC1_L23HRAWC_DIS       0
#define V_XHCI_XHCC1_L23HRAWC_128       (BIT8)
#define V_XHCI_XHCC1_L23HRAWC_256       (BIT9)
#define V_XHCI_XHCC1_L23HRAWC_512       (BIT9 | BIT8)
#define V_XHCI_XHCC1_L23HRAWC_1024      (BIT10)
#define V_XHCI_XHCC1_L23HRAWC_2048      (BIT10 | BIT8)
#define V_XHCI_XHCC1_L23HRAWC_4096      (BIT10 | BIT9)
#define V_XHCI_XHCC1_L23HRAWC_131072    (BIT10 | BIT9 | BIT8)
#define B_XHCI_XHCC1_UTAGCP             (BIT7 | BIT6) ///< Upstream Type Arbiter Grant Count Posted
#define B_XHCI_XHCC1_UDAGCNP            (BIT5 | BIT4) ///< Upstream Type Arbiter Grant Count Non Posted
#define B_XHCI_XHCC1_UDAGCCP            (BIT3 | BIT2) ///< Upstream Type Arbiter Grant Count Completion
#define B_XHCI_XHCC1_UDAGC              (BIT1 | BIT0) ///< Upstream Type Arbiter Grant Count

#define R_XHCI_XHCC2                    0x44  ///< XHC System Bus Configuration 2
#define B_XHCI_XHCC2_OCCFDONE           BIT31 ///< OC Configuration Done
#define B_XHCI_XHCC2_DREQBCC            BIT25 ///< DMA Request Boundary Crossing Control
#define B_XHCI_XHCC2_IDMARRSC           (BIT24 | BIT23 | BIT22) ///< IDMA Read Request Size Control
#define B_XHCI_XHCC2_XHCUPRDROE         BIT21 ///< XHC Upstream Read Relaxed Ordering Enable
#define B_XHCI_XHCC2_IOSFSRAD           BIT20 ///< IOSF Sideband Register Access Disable
#define B_XHCI_XHCC2_UNPPA              0xFC000 ///< Upstream Non-Posted Pre-Allocation
#define B_XHCI_XHCC2_SWAXHCIP           (BIT13 | BIT12) ///< SW Assisted xHC Idle Policy
#define B_XHCI_XHCC2_RAWDD              BIT11 ///< MMIO Read After MMIO Write Delay Disable
#define B_XHCI_XHCC2_WAWDE              BIT10 ///< MMIO Write After MMIO Write Delay Enable
#define B_XHCI_XHCC2_SWACXIHB           (BIT9 | BIT8) ///< SW Assisted Cx Inhibit
#define B_XHCI_XHCC2_SWADMIL1IHB        (BIT7 | BIT6) ///< SW Assisted DMI L1 Inhibit
#define B_XHCI_XHCC2_L1FP2CGWC          (BIT5 | BIT4 | BIT3) ///< L1 Force P2 clock Gating Wait Count
#define V_XHCI_XHCC2_L1FP2CGWC_DIS      0
#define V_XHCI_XHCC2_L1FP2CGWC_128      (BIT3)
#define V_XHCI_XHCC2_L1FP2CGWC_256      (BIT4)
#define V_XHCI_XHCC2_L1FP2CGWC_512      (BIT4 | BIT3)
#define V_XHCI_XHCC2_L1FP2CGWC_1024     (BIT5)
#define V_XHCI_XHCC2_L1FP2CGWC_2048     (BIT5 | BIT3)
#define V_XHCI_XHCC2_L1FP2CGWC_4096     (BIT5 | BIT4)
#define V_XHCI_XHCC2_L1FP2CGWC_131072   (BIT5 | BIT4 | BIT3)
#define B_XHCI_XHCC2_RDREQSZCTRL        (BIT2 | BIT1 | BIT0) ///< Read Request Size Control
#define V_XHCI_XHCC2_RDREQSZCTRL_128    0
#define V_XHCI_XHCC2_RDREQSZCTRL_256    (BIT0)
#define V_XHCI_XHCC2_RDREQSZCTRL_512    (BIT1)
#define V_XHCI_XHCC2_RDREQSZCTRL_64     (BIT2 | BIT1 | BIT0)

#define R_XHCI_XHCLKGTEN                0x50  ///< Clock Gating
#define B_XHCI_XHCLKGTEN_NUEFBCGPS      BIT28 ///< Naking USB2.0 EPs for Backbone Clock Gating and PLL Shutdown
#define B_XHCI_XHCLKGTEN_SRAMPGTEN      BIT27 ///< SRAM Power Gate Enable
#define B_XHCI_XHCLKGTEN_SSLSE          BIT26 ///< SS Link PLL Shutdown Enable
#define B_XHCI_XHCLKGTEN_USB2PLLSE      BIT25 ///< USB2 PLL Shutdown Enable
#define B_XHCI_XHCLKGTEN_IOSFSTCGE      BIT24 ///< IOSF Sideband Trunk Clock Gating Enable
#define B_XHCI_XHCLKGTEN_HSTCGE         (BIT23 | BIT22 | BIT21 | BIT20) ///< HS Backbone PXP Trunk Clock Gate Enable
#define B_XHCI_XHCLKGTEN_SSTCGE         (BIT19 | BIT18 | BIT17 | BIT16) ///< SS Backbone PXP Trunk Clock Gate Enable
#define B_XHCI_XHCLKGTEN_XHCIGEU3S      BIT15 ///< XHC Ignore_EU3S
#define B_XHCI_XHCLKGTEN_XHCFTCLKSE     BIT14 ///< XHC Frame Timer Clock Shutdown Enable
#define B_XHCI_XHCLKGTEN_XHCBBTCGIPISO  BIT13 ///< XHC Backbone PXP Trunk Clock Gate In Presence of ISOCH EP
#define B_XHCI_XHCLKGTEN_XHCHSTCGU2NRWE BIT12 ///< XHC HS Backbone PXP Trunk Clock Gate U2 non RWE
#define B_XHCI_XHCLKGTEN_XHCUSB2PLLSDLE (BIT11 | BIT10) ///< XHC USB2 PLL Shutdown Lx Enable
#define B_XHCI_XHCLKGTEN_HSPLLSUE       (BIT9 | BIT8) ///< HS Backbone PXP PLL Shutdown Ux Enable
#define B_XHCI_XHCLKGTEN_SSPLLSUE       (BIT7 | BIT6 | BIT5) ///< SS backbone PXP PLL Shutdown Ux Enable
#define B_XHCI_XHCLKGTEN_XHCBLCGE       BIT4  ///< XHC Backbone Local Clock Gating Enable
#define B_XHCI_XHCLKGTEN_HSLTCGE        BIT3  ///< HS Link Trunk Clock Gating Enable
#define B_XHCI_XHCLKGTEN_SSLTCGE        BIT2  ///< SS Link Trunk Clock Gating Enable
#define B_XHCI_XHCLKGTEN_IOSFBTCGE      BIT1  ///< IOSF Backbone Trunk Clock Gating Enable
#define B_XHCI_XHCLKGTEN_IOSFGBLCGE     BIT0  ///< IOSF Gasket Backbone Local Clock Gating Enable

#define R_XHCI_USB_RELNUM               0x60
#define B_XHCI_USB_RELNUM               0xFF

#define R_XHCI_FL_ADJ                   0x61
#define B_XHCI_FL_ADJ                   0x3F

#define R_XHCI_PWR_CAPID                0x70
#define B_XHCI_PWR_CAPID                0xFF

#define R_XHCI_NXT_PTR1                 0x71
#define B_XHCI_NXT_PTR1                 0xFF

#define R_XHCI_PWR_CAP                  0x72
#define B_XHCI_PWR_CAP_PME_SUP          0xF800
#define B_XHCI_PWR_CAP_D2_SUP           BIT10
#define B_XHCI_PWR_CAP_D1_SUP           BIT9
#define B_XHCI_PWR_CAP_AUX_CUR          (BIT8 | BIT7 | BIT6)
#define B_XHCI_PWR_CAP_DSI              BIT5
#define B_XHCI_PWR_CAP_PME_CLK          BIT3
#define B_XHCI_PWR_CAP_VER              (BIT2 | BIT1 | BIT0)

#define R_XHCI_PWR_CNTL_STS             0x74
#define B_XHCI_PWR_CNTL_STS_PME_STS     BIT15
#define B_XHCI_PWR_CNTL_STS_DATASCL     (BIT14 | BIT13)
#define B_XHCI_PWR_CNTL_STS_DATASEL     (BIT12 | BIT11 | BIT10 | BIT9)
#define B_XHCI_PWR_CNTL_STS_PME_EN      BIT8
#define B_XHCI_PWR_CNTL_STS_PWR_STS     (BIT1 | BIT0)
#define V_XHCI_PWR_CNTL_STS_PWR_STS_D3  (BIT1 | BIT0)

#define R_XHCI_MSI_CAPID                0x80
#define B_XHCI_MSI_CAPID                0xFF

#define R_XHCI_NXT_PTR2                 0x81
#define B_XHCI_NXT_PTR2                 0xFF

#define R_XHCI_MSI_MCTL                 0x82
#define B_XHCI_MSI_MCTL_MSIENABLE       BIT0

#define V_XHCI_MSI_NEXT                 0x90  ///< Pointer to DevIdle capability structure

#define R_XHCI_PCE                      0xA2  ///< Power control enable
#define B_XHCI_PCE_HAE                  BIT5///<HAE: Hardware Autonomous Enable
#define B_XHCI_PCE_SE                   BIT3///<SE: Sleep Enable
#define B_XHCI_PCE_D3HE                 BIT2///<D3HE: D3-Hot Enable:
#define B_XHCI_PCE_I3E                  BIT1///<I3E: I3 Enable
#define B_XHCI_PCE_SPE                  BIT0 ///<SPE: Software PowerGate Enable

#define R_XHCI_HSCFG1                   0xAC  ///<High Speed Configuration 1
#define B_XHCI_HSCFG1_UTMI_SUSPEND_CG   BIT19 ///<UTMI Gasket Local Suspended Port 480/60/48MHz Clock Gating Enable
#define B_XHCI_HSCFG1_UTMI_SPEED_CG     BIT18 ///<UTMI Gasket Local Speed Port 480/60/48MHz Clock Gating Enable

#define R_XHCI_HSCFG2                   0xA4  ///<High Speed Configuration 2
#define B_XHCI_HSCFG2_HSAAIM            BIT15 ///<HS ASYNC Active IN Mask (HSAAIM):
#define B_XHCI_HSCFG2_HSOAAPEPM         BIT14 ///<HS OUT ASYNC Active Polling EP Mask (HSOAAPEPM):
#define B_XHCI_HSCFG2_HSIAAPEPM         BIT13 ///<HS IN ASYNC Active Polling EP Mask (HSIAAPEPM):
#define B_XHCI_HSCFG2_HSIIPAPC          (BIT12|BIT11) ///<HS INTR IN Periodic Active Policy Control (HSIIPAPC):
#define B_XHCI_HSCFG2_HSIIPANEPT        (BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4)  ///< HS INTR IN Periodic Active Num of EP Threshold(HSIIPANEPT)
#define B_XHCI_HSCFG2_HSIIPASIT         (BIT3 | BIT2 | BIT1 | BIT0)                        ///< HS INTR IN Periodic Active Service Interval Threshold (HSIIPASIT)

#define R_XHCI_SSCFG1                   0xA8  ///<High Speed Configuration 1
#define B_XHCI_SSCFG1_LFPS              BIT17 ///<LFPS Power Management Enable
#define B_XHCI_SSCFG1_PHY_U3            BIT14 ///<USB3 PHY Power Gate Enable for U2

#define R_XHCI_U2OCM                    0xB0
#define R_XHCI_U3OCM                    0xD0
#define V_XHCI_NUMBER_OF_OC_PINS        2

//
// Extended Capability Registers
//
#define R_XHCI_USB2PDO                  0x84F8
#define B_XHCI_BXT_USB2PDO_MASK         0x07
#define B_XHCI_BXTP_USB2PDO_MASK        0xFF
#define B_XHCI_USB2PDO_DIS_PORT0        BIT0

#define R_XHCI_USB3PDO                  0x84FC
#define B_XHCI_BXT_USB3PDO_MASK         0x03
#define B_XHCI_BXTP_USB3PDO_MASK        0x3F
#define B_XHCI_USB3PDO_DIS_PORT0        BIT0

///
/// xHCI MMIO registers
///
#define R_XHCI_MEM_DUAL_ROLE_CFG0       0x80D8
#define B_XHCI_EN_PIPE4_SYNC            BIT30 ///< enable/disable PIPE 4.1 synchronous phystatus
#define R_XHCI_MEM_DUAL_ROLE_CFG1       0x80DC

///
/// 0x00 - 0x1F - Capability Registers
///
#define R_XHCI_CAPLENGTH                0x00  ///< Capability Registers Length

#define R_XHCI_HCIVERSION               0x02  ///< Host Controller Interface Version Number

#define R_XHCI_HCSPARAMS1               0x04  ///< Structural Parameters 1
#define B_XHCI_HCSPARAMS1_MAXPORTS      0xFF000000 ///< Number of Ports
#define B_XHCI_HCSPARAMS1_MAXINTRS      0x7FF00 ///< Number of Interrupters
#define B_XHCI_HCSPARAMS1_MAXSLOTS      0xFF  ///< Number of Device Slots

#define R_XHCI_HCSPARAMS2               0x08  ///< Structural Parameters 2
#define B_XHCI_HCSPARAMS2_MSB           0xF8000000 ///< Max Scratchpad Buffers
#define B_XHCI_HCSPARAMS2_ERSTMAX       0xF0  ///< Event Ring Segment Table Max
#define B_XHCI_HCSPARAMS2_IST           0x0F  ///< Isochronous Scheduling Threshold

#define R_XHCI_HCSPARAMS3               0x0C  ///< Structural Parameters 3
#define B_XHCI_HCSPARAMS3_U2DEL         0xFFFF0000 ///< U2 Device Exit Latency
#define B_XHCI_HCSPARAMS3_U1DEL         0x000000FF ///< U1 Device Exit Latency

#define R_XHCI_HCCPARAMS                0x10  ///< Capability Parameters
#define B_XHCI_HCCPARAMS_XECP           0xFFFF0000 ///< xHCI Extended Capabilities Pointer
#define B_XHCI_HCCPARAMS_MAXPSASIZE     (BIT15 | BIT14 | BIT13 | BIT12) ///< Maximum Primary Stream Array Size
#define B_XHCI_HCCPARAMS_CFC            BIT11  ///< Contiguous Frame ID Capability
#define B_XHCI_HCCPARAMS_PAE            BIT8  ///< Parst All Event Data
#define B_XHCI_HCCPARAMS_NSS            BIT7  ///< No Secondary SID Support
#define B_XHCI_HCCPARAMS_LTC            BIT6  ///< Latency Tolerance Messaging Capability
#define B_XHCI_HCCPARAMS_LHRC           BIT5  ///< Light HC Reset Capability
#define B_XHCI_HCCPARAMS_PIND           BIT4  ///< Port Indicators
#define B_XHCI_HCCPARAMS_PPC            BIT3  ///< Port Power Control
#define B_XHCI_HCCPARAMS_CSZ            BIT2  ///< Context Size
#define B_XHCI_HCCPARAMS_BNC            BIT1  ///< BW Negotiation Capability
#define B_XHCI_HCCPARAMS_AC64           BIT0  ///< 64-bit Addressing Capability

#define R_XHCI_DBOFF                    0x14  ///< Doorbell Offset
#define B_XHCI_DBOFF_DBAO               0xFFFFFFFC ///< Doorbell Array Offset

#define R_XHCI_RTSOFF                   0x18  ///< Runtime Register Space Offset
#define B_XHCI_RTSOFF_RTRSO             0xFFFFFFE0 ///< Runtime Register Space Offset

///
/// 0x80 - 0xBF - Operational Registers
///
#define R_XHCI_USBCMD                   0x80  ///< USB Command
#define B_XHCI_USBCMD_EU3S              BIT11 ///< Enable U3 MFINDEX Stop
#define B_XHCI_USBCMD_EWE               BIT10 ///< Enable Wrap Event
#define B_XHCI_USBCMD_CRS               BIT9  ///< Controller Restore State
#define B_XHCI_USBCMD_CSS               BIT8  ///< Controller Save State
#define B_XHCI_USBCMD_LHCRST            BIT7  ///< Light Host Controller Reset
#define B_XHCI_USBCMD_HSEE              BIT3  ///< Host System Error Enable
#define B_XHCI_USBCMD_INTE              BIT2  ///< Interrupter Enable
#define B_XHCI_USBCMD_HCRST             BIT1  ///< Host Controller Reset
#define B_XHCI_USBCMD_RS                BIT0  ///< Run/Stop

#define R_XHCI_USBSTS                   0x84  ///< USB Status
#define B_XHCI_USBSTS_HCE               BIT12 ///< Host Controller Error
#define B_XHCI_USBSTS_CNR               BIT11 ///< Controller Not Ready
#define B_XHCI_USBSTS_SRE               BIT10 ///< Save / Restore Error
#define B_XHCI_USBSTS_RSS               BIT9  ///< Restore State Status
#define B_XHCI_USBSTS_SSS               BIT8  ///< Save State Status
#define B_XHCI_USBSTS_PCD               BIT4  ///< Port Change Detect
#define B_XHCI_USBSTS_EINT              BIT3  ///< Event Interrupt
#define B_XHCI_USBSTS_HSE               BIT2  ///< Host System Error
#define B_XHCI_USBSTS_HCH               BIT0  ///< HC Halted

///
/// 0x480 - 0x5CF - Port Status and Control Registers
///
#define R_XHCI_PORTSC01USB2              0x480
#define R_XHCI_PORTSC02USB2              0x490
#define R_XHCI_PORTSC03USB2              0x4A0
#define R_XHCI_PORTSC04USB2              0x4B0
#define R_XHCI_PORTSC05USB2              0x4C0
#define R_XHCI_PORTSC06USB2              0x4D0
#define R_XHCI_PORTSC07USB2              0x4E0
#define R_XHCI_PORTSC08USB2              0x4F0
#define B_XHCI_PORTSCXUSB2_WPR          BIT31 ///< Warm Port Reset
#define B_XHCI_PORTSCXUSB2_DR           BIT30 ///< Device Removable
#define B_XHCI_PORTSCXUSB2_WOE          BIT27 ///< Wake on Over-Current Enable
#define B_XHCI_PORTSCXUSB2_WDE          BIT26 ///< Wake on Disconnect Enable
#define B_XHCI_PORTSCXUSB2_WCE          BIT25 ///< Wake on Connect Enable
#define B_XHCI_PORTSCXUSB2_CAS          BIT24 ///< Cold Attach Status
#define B_XHCI_PORTSCXUSB2_CEC          BIT23 ///< Port Config Error Change
#define B_XHCI_PORTSCXUSB2_PLC          BIT22 ///< Port Link State Change
#define B_XHCI_PORTSCXUSB2_PRC          BIT21 ///< Port Reset Change
#define B_XHCI_PORTSCXUSB2_OCC          BIT20 ///< Over-current Change
#define B_XHCI_PORTSCXUSB2_WRC          BIT19 ///< Warm Port Reset Change
#define B_XHCI_PORTSCXUSB2_PEC          BIT18 ///< Port Enabled Disabled Change
#define B_XHCI_PORTSCXUSB2_CSC          BIT17 ///< Connect Status Change
#define B_XHCI_PORTSCXUSB2_LWS          BIT16 ///< Port Link State Write Strobe
#define B_XHCI_PORTSCXUSB2_PIC          (BIT15 | BIT14) ///< Port Indicator Control
#define B_XHCI_PORTSCXUSB2_PS           (BIT13 | BIT12 | BIT11 | BIT10) ///< Port Speed
#define B_XHCI_PORTSCXUSB2_PP           BIT9  ///< Port Power
#define B_XHCI_PORTSCXUSB2_PLS          (BIT8 | BIT7 | BIT6 | BIT5) ///< Port Link State
#define B_XHCI_PORTSCXUSB2_PR           BIT4  ///< Port Reset
#define B_XHCI_PORTSCXUSB2_OCA          BIT3  ///< Over-Current Active
#define B_XHCI_PORTSCXUSB2_PED          BIT1  ///< Port Enabled Disabled
#define B_XHCI_PORTSCXUSB2_CCS          BIT0  ///< Current Connect Status

#define R_BXT_XHCI_PORTSC1USB3          0x4C0
#define R_BXT_XHCI_PORTSC2USB3          0x4D0
#define R_BXT_XHCI_PORTSC3USB3          0x4E0
#define R_BXT_XHCI_PORTSC4USB3          0x4F0
#define R_BXTP_XHCI_PORTSC1USB3         0x500
#define R_BXTP_XHCI_PORTSC2USB3         0x510
#define R_BXTP_XHCI_PORTSC3USB3         0x520
#define R_BXTP_XHCI_PORTSC4USB3         0x530
#define R_BXTP_XHCI_PORTSC5USB3         0x540
#define R_BXTP_XHCI_PORTSC6USB3         0x550
#define R_BXTP_XHCI_PORTSC7USB3         0x560
#define B_XHCI_PORTSCXUSB3_WPR          BIT31  ///</ Warm Port Reset
#define B_XHCI_PORTSCXUSB3_CEC          BIT23  ///</ Port Config Error Change
#define B_XHCI_PORTSCXUSB3_PLC          BIT22  ///</ Port Link State Change
#define B_XHCI_PORTSCXUSB3_PRC          BIT21  ///</ Port Reset Change
#define B_XHCI_PORTSCXUSB3_OCC          BIT20  ///</ Over-current Chang
#define B_XHCI_PORTSCXUSB3_WRC          BIT19  ///</ Warm Port Reset Change
#define B_XHCI_PORTSCXUSB3_PEC          BIT18  ///</ Port Enabled Disabled Change
#define B_XHCI_PORTSCXUSB3_CSC          BIT17  ///</ Connect Status Change
#define B_XHCI_PORTSCXUSB3_PP           BIT9   ///</ Port Power
#define B_XHCI_PORTSCXUSB3_PR           BIT4   ///</ Port Reset
#define B_XHCI_PORTSCXUSB3_PED          BIT1   ///</ Port Enabled / Disabled

#define R_XHCI_XECP_CMDM_CTRL_REG1      0x818C  ///< XECP_CMDM_CTRL_REG1 - Command Manager Control 1
#define B_XHCI_XECP_MAX_EP              BIT20   ///<enable cause a Configure Endpoint Command to fail if the number of active EPs post configuration exceeds the maximum number of EPs available in cache
#define B_XHCI_XECP_TSP                 BIT16   ///<clearing of split state if TSP=1
#define B_XHCI_XECP_CLR_CTX_ENSLOT      BIT8    ///<clearing other context during an enable slot command.

#define R_XHCI_XECP_CMDM_CTRL_REG2      0x8190  ///< XECP_CMDM_CTRL_REG2 - Command Manager Control 2
#define B_XHCI_XECP_FORCE_BURST_SIZE    BIT14    ///<Force the default burst size when clearing context

#define R_XHCI_XECP_CMDM_CTRL_REG3      0x8194  ///< XECP_CMDM_CTRL_REG3 - Command Manager Control 3
#define B_XHCI_XECP_STOP_EP             BIT25   ///<Setting this field will enable the fix for a stream issue related to Stop EP recovery

#define R_XHCI_XECP_SUPP_USB3_2         0x8028

#define R_XHCI_PMCTRL                   0x80A4  ///<Power Management Control
#define B_XHCI_PMCTRL_ASYNC_PME_SRC     BIT31   ///<Async PME Source Enable
#define B_XHCI_PMCTRL_LEGACY_PME_SRC    BIT30   ///<Legacy PME Source Enable
#define B_XHCI_PMCTRL_RESET_PG          BIT29   ///<Reset Warn Power Gate Trigger Disable
#define B_XHCI_PMCTRL_XELFPSRTC         BIT25   ///<XELFPSRTC (Enable LFPS Filtering on RTC)
#define B_XHCI_PMCTRL_XMPHYSPGDD0I2     BIT24   ///<XMPHYSPGDD0I2 (ModPhy Sus Well Power Gate Disable for D0I2)
#define B_XHCI_PMCTRL_XMPHYSPGDD0I3     BIT23   ///<XMPHYSPGDD0I3 (ModPhy Sus Well Power Gate Disable for D0I3)
#define B_XHCI_PMCTRL_XMPHYSPGDRTD3     BIT22   ///<XMPHYSPGDRTD3 (ModPhy Sus Well Power Gate Disable for RTD3)
#define B_XHCI_PMCTRL_SSALDE            BIT16   ///<SS AON LFPS Detector Enable
#define B_XHCI_PMCTRL_LFPS_THRESHOLD    (0xFF00)   ///<SS U3 LFPS Detection Threshold bit15:8
#define B_XHCI_PMCTRL_FPS_OFFTIME       (0x00F0)   ///<SS U3 LFPS Periodic Sampling OFF Time Control bit7:4
#define B_XHCI_PMCTRL_LFPS_SRC          BIT3   ///<PS3 LFPS Source Select
#define B_XHCI_PMCTRL_APGE              BIT2   ///<XHCI Engine Autonomous Power Gate Exit Reset Policy

#define R_XHCI_PGCBCTRL                   0x80A8  ///<PGCB Control
#define B_XHCI_PGCBCTRL_RESET_PREP_DIS     BIT24  ///<Reset Prep override disable

#define R_XHCI_AUX_CTRL_REG1            0x80E0  ///< AUX_CTRL_REG1 - AUX Power Management Control
#define B_XHCI_AUX_ISOLATION            BIT22   ///<masking related to isolation missing between gated and ungated domain that will cause an immediate wake out of power gating
#define B_XHCI_AUX_PORT_CG              BIT16   ///<USB3 port clock gating
#define B_XHCI_AUX_CG                   BIT9    ///<When set to '1' disable core clock gating based on low power state entered
#define B_XHCI_AUX_P2                   BIT6    ///<enable P2 overwrite P1 when PCIe core has indicated the transition from P0 to P1.

#define R_XHCI_HOST_CTRL_SCH_REG        0x8094  ///< HOST_CTRL_SCH_REG - Host Control Scheduler
#define B_XHCI_HOST_CTRL_DIS_ASYNC      BIT23   ///<Disable async. scheduling while periodic active to same port
#define B_XHCI_HOST_CTRL_EN_EP_PPL      BIT22   ///<Setting this bit enables pipelining of multiple OUT EPs
#define B_XHCI_HOST_CTRL_DIS_SCH_PKT    BIT21   ///<Scheduler: Enable Stop serving packets to disabled port
#define B_XHCI_HOST_CTRL_DIS_PKT_CHK    BIT14   ///<TTE: Disable checking of missed microframes
#define B_XHCI_HOST_CTRL_EN_1P_EP_PPL   BIT6   ///<Setting this bit enables pipelining of multiple OUT EPs on the same port. This will mainly help boost the performance for 1 port multiple OUT EPs test case.

#define R_XHCI_HOST_CTRL_IDMA_REG        0x809C  ///< HOST_CTRL_SCH_REG - Host Control IN DMA Register
#define B_XHCI_HOST_CTRL_IDMA_HCRST_WDT   BIT31   ///<Enable the WDT for HC reset flow

#define R_XHCI_HOST_CTRL_PORT_LINK_REG  0x80EC  ///< HOST_CTRL_PORT_LINK_REG - SuperSpeed Port Link Control
#define B_XHCI_HOST_CTRL_EN_TS_EXIT     BIT19   ///<enable TS receive to complete U1/U2/U3 exit LFPS handshake
#define B_XHCI_HOST_CTRL_PORT_INIT_TIMEOUT   BIT17///<specifies the port initialization timeout value 1:20us-21us 0:19us-20us
#define B_XHCI_DIS_LINK_CM              BIT0    ///<0: Enable link compliance mode 1: Disable link compliance mode

#define R_XHCI_USB2_LINK_MGR_CTRL_REG1  0x80F0  ///< USB2_LINK_MGR_CTRL_REG1 - USB2 Port Link Control 1, 2, 3, 4
#define B_XHCI_USB2_LINK_L1_EXIT        BIT20   ///<Mode for extended L1 Exit recovery delay

#define R_XHCI_USB2_LINK_MGR_CTRL_REG1_CONTROL4   0x80FC ///<USB2_LINK_MGR_CTRL_REG1 - USB2 Port Link Control 1, 2, 3, 4
#define B_XHCI_USB2_LINK_PRV_L1_ENTRY   BIT25 ///<(bit121) Chicken bit to enable periodic_prewake fix to prevent L1 entry if in U0, or wake from L1 if already in U2.

#define R_XHCI_HOST_CTRL_TRM_REG2       0x8110  ///< HOST_CTRL_TRM_REG2 - Host Controller Transfer Manager Control 2
#define B_XHCI_HOST_CTRL_MAX_BURST_CHK  BIT20   ///<TRM can check the credit returned from remote device to not excced its max burst size
#define B_XHCI_HOST_CTRL_TRF_PIPE       BIT11   ///<Enable the host to transfer to the prime-pipe state
#define B_XHCI_HOST_CTRL_REDEEM         BIT2    ///<enable the credit redeem when a port is in NC state

#define R_XHCI_AUX_CTRL_REG2            0x8154  ///< AUX_CTRL_REG2 - Aux PM Control Register 2
#define B_XHCI_AUX2_L1P2_EXIT           BIT31 ///<This bit disables the dependency on Wake Enables defined in PORTSC for L1P2 exit when in D0
#define B_XHCI_AUX2_P2_D3HOT            BIT21 ///<disables p2 overwrite due to the D3HOT where PCIe core enters the L1
#define B_XHCI_AUX2_PHY_P3              BIT13 ///<enables PHY P3 mode in U2.

#define R_XHCI_AUX_CLOCK_CTRL_REG       0x816C  ///< xHCI Aux Clock Control Register
#define B_XHCI_USB3_PELCG               BIT19 ///<enables gating of the SOSC trunk to the XHCI engine and link in the PARUSB3 partition.
#define B_XHCI_USB3_PFTTCG              BIT18 ///<USB3 Partition Frame Timer trunk gating Enable
#define B_XHCI_USB2_LPCG                BIT17 ///<USB2 link partition clock gating enable
#define B_XHCI_USB2_USBIP_12_5HZ_CG     BIT16 ///<USB2/USHIP 12.5 MHz partition clock gating enable
#define B_XHCI_USB3_PORT_ACCG           BIT14 ///<USB3 Port Aux/Core clock gating enable
#define B_XHCI_AUXCG_RXDET_TIMER        (BIT13 | BIT12) ///<Rx Detect Timer when port Aux Clock is Gated 0x0:      100ms;   0x1:      12ms;   Others:    Reserved;
#define B_XHCI_HOST_U2_RES_BMODCG       (BIT11 | BIT8) ///<U2 Residency Before ModPHY Clock Gating
#define B_XHCI_EACG                     BIT5 ///< XHCI Engine Aux clock gating enable
#define B_XHCI_APBCG                    BIT4 ///<XHCI Aux PM block clock gating enable
#define B_XHCI_ACTCG                    BIT3 ///<USB3 Aux Clock Trunk Gating Enable
#define B_XHCI_PORT_APCG                BIT2 ///< USB3 Port Aux/Port clock gating enable
#define B_XHCI_PPACG_IN_U2              BIT1 ///< USB3 PHY port Aux clock gating enable in U2
#define B_XHCI_PPACG_IN_DUD             BIT0 ///<USB3 PHY port Aux clock gating enable in Disconnected, U3 or Disabled

#define R_XHCI_HOST_IF_PWR_CTRL_REG0    0x8140  ///< HOST_IF_PWR_CTRL_REG0 - Power Scheduler Control 0
#define B_XHCI_HOST_EIH                 (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24) ///< Engine Idle Hysteresis (EIH), This register controls the min. idle span that has to be observed from the engine idle indicators before the power state flags (xhc_*_idle) will indicate a 1 (TBD units)
#define B_XHCI_HOST_AW                  (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12)///<Advance Wake (AW):This register controls the time before the next scheduled transaction where the periodic_active & periodic_active_hs_in will assert.
#define B_XHCI_HOST_MID                 (BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)///<Min. Idle Duration (MID):The sum of this register plus the Advance Wake form to a Total Idle time.

#define R_XHCI_HOST_IF_PWR_CTRL_REG1    0x8144  ///< HOST_IF_PWR_CTRL_REG1 - Power Scheduler Control 1
#define B_XHCI_HSII                     BIT8 ///<HS Interrupt IN Alarm (HSII)

#define R_XHCI_LATENCY_TOLERANCE_PARAMETERS_LTV_CONTROL                 0x8174  ///< xHCI Latency Tolerance Parameters - LTV Control
#define B_XHCI_XLTRE                    BIT24 ///<XHCI LTR Enable (XLTRE) This bit must be set to enable LTV messaging from XHCI to the PMC.
#define B_XHCI_USB2_PORT_L0_LTV         (BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)///< USB2 Port L0 LTV

#define R_XHCI_LATENCY_TOLERANCE_PARAMETERS_HIGH_IDLE_TIME_CONTROL      0x817C  ///< xHC Latency Tolerance Parameters - High Idle Time Control
#define B_XHCI_MHIT                     (BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16) ///<Minimum High Idle Time (MHIT)This is the minimum schedule idle time that must be available before a 'High' LTR value can be indicated.
#define B_XHCI_HIWL                     (BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0) ///<High Idle Wake Latency (HIWL) This is the latency to access memory from the High Idle Latency state.

#define R_XHCI_LATENCY_TOLERANCE_PARAMETERS_MEDIUM_IDLE_TIME_CONTROL    0x8180  ///< xHC Latency Tolerance Parameters - Medium Idle Time Control
#define B_XHCI_MMIT                     (BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16) ///<Minimum Medium Idle Time (MMIT)This is the minimum schedule idle time that must be available before a 'Medium' LTR value can be indicated.
#define B_XHCI_MIWL                     (BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0) ///<Medium Idle Wake Latency (MIWL)This is the latency to access memory from the Medium Idle Latency state.

#define R_XHCI_LATENCY_TOLERANCE_PARAMETERS_LOW_IDLE_TIME_CONTROL       0x8184  ///< xHC Latency Tolerance Parameters - Low Idle Time Control
#define B_XHCI_MLIT                     (BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16) ///<Minimum Low Idle Time (MLIT)This is the minimum schedule idle time that must be available before a 'Low' LTR value can be indicated.
#define B_XHCI_LIWL                     (BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0) ///<Low Idle Wake Latency (LIWL) This is the latency to access memory from the Low Idle Latency state.

#define R_XHCI_USB2_PHY_POWER_MANAGEMENT_CONTROL                        0x8164  ///< USB2 PHY Power Management Control
#define B_XHCI_CMAI                     BIT7 ///< Enable Command Manager Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_TTEAI                    BIT6 ///<Enable TTE  Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_IDMAAI                   BIT5 ///<Enable IDMA Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_ODMAAI                   BIT4 ///<Enable ODMA Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_TMAI                     BIT3 ///<Enable  Transfer Manager  Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_SAI                      BIT2 ///<Enable  Scheduler  Active indication for Tx/Rx Bias circuit HS Phy PM Policy
#define B_XHCI_RX_BIAS_CHT_DIS          BIT1 ///<Enable Rx Bias ckt disable
#define B_XHCI_TX_BIAS_CHT_DIS          BIT0 ///<Enable Tx Bias ckt disable

#define R_XHCI_HOST_CONTROLLER_MISC_REG 0x80B0
#define B_XHCI_LATE_FID_CHK_DIS         BIT24 ///< This register disables the Late FID Check performed when starting an ISOCH stream.
#define B_XHCI_LATE_FID_TTE_DIS         BIT23 /// < Late FID TTE  Count Adjust Disable
#define B_XHCI_EXTRA_UFRAME             (BIT18|BIT17|BIT16) ///< This register controls the extra number of uFrames added onto the advancing of late FID check..
#define B_XHCI_SSPE                     (BIT3|BIT2|BIT1|BIT0) ///<This field controls whether SuperSpeed capability is enabled  for a given USB3 port.

#define R_XHCI_HOST_CONTROLLER_MISC2REG 0x80B4
#define B_XHCI_FRAME_TIM_SEL            BIT5   ///<Frame Timer Select
#define B_XHCI_WARM_PORT_RESET_ON_DISC_PORT_DIS    BIT2   ///<Disable Warm Port Reset on Disconnected Port

#define R_XHCI_HOST_CONTROLLER_SSPE     0x80B8
#define B_XHCI_ENCLCCS                  BIT30 ///< Enables clearing of CCS on HCRESET

#define R_XHCI_HOST_CTRL_BW_MAX_REG     0x8128 ///<Max BW control Reg 4
#define V_XHCI_HOST_CTRL_BW_MAX_REG_TT  0xFFF  ///<TT Max BW Units

#define R_XHCI_PULLDOWN_DISABLE_CONTROL 0x8198 ///<Each bit corresponds to a USB2 port indexed by the bit number.When set, allow the pulldown on D+ or D- (as appropriate) to be disabled when the port is connected and in L2.
#define B_XHCI_PULLDOWN_DISABLE_CONTROL 0xFFFFFFFF

#define R_XHCI_THROTTLE_CONTROL         0x819C
#define B_XHCI_SSIC_TTUM                BIT20 ///<SSIC Thermal Throttle Ux Mapping,Controls if U1 or U2 is forced upon the start of thermal throttle OFF period.
#define B_XHCI_USB3_TTUM                BIT16 ///<USB3 Thermal Throttle Ux Mapping,Controls if U1 or U2 is forced upon the start of thermal throttle OFF period.
#define B_XHCI_FORCE_L1                 BIT14 ///<Enable  Force L1 when throttled.
#define B_XHCI_INTERRUPT_THROTTLING_DIS BIT13 ///<Disable Interrupt Throttling
#define B_XHCI_ISOCHRONOUS_THROT_DIS    BIT12 ///<Disable Isochronous Throttling
#define B_XHCI_T1_ACTION                (BIT11 | BIT10 | BIT9 | BIT8) ///<T1 Action, bus intervals to be idle for async traffic out of the 16 interval master period; from 0 to 15.
#define B_XHCI_T2_ACITON                (BIT7 | BIT6 | BIT5 | BIT4) ///<T2 Action, # bus intervals to be idle for async traffic out of the 16 interval master period; from 0 to 15.
#define B_XHCI_T3_ACTION                (BIT3 | BIT2 | BIT1 | BIT0) ///<T3 Action, # bus intervals to be idle for async traffic out of the 16 interval master period; from 0 to 15.

#define R_XHCI_LFPS_PM_CONTROL          0x81A0 ///< LFPS Power Management in U3 Enable, This field allows xHC to turn off LFPS Receiver when the port is in U3.
#define B_XHCI_LFPS_PM_CONTROL          0xFFFFFFFF

#define R_XHCI_THROTT2                  0x81B4
#define B_XHCI_TTFLA                    (BIT3 | BIT2 | BIT1 | BIT0) ///< Thermal Throttle Force LPM Accept Enable

#define R_XHCI_LFPS_ON_COUNT            0x81B8
#define B_XHCI_XLFPSONCNTSSIC           (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10) ///< This time would describe the number of clocks SSIC LFPS will remain ON.
#define B_XHCI_XLFPSONCNTSS             (BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0) ///< This time would describe the number of clocks LFPS will remain ON.

#define R_XHCI_D0I2_CONTROL             0x81BC
#define B_XHCI_USB2_BTB_BTO_HANDLING  BIT31 ///<USB2 Back to Back BTO Handling Enable
#define B_XHCI_D0I2_MIN_RESIDENCY       (BIT29|BIT28|BIT27|BIT26) ///<This field controls the minimum time that we must stay in D0i2 to ensure that the entry sequence has settled before we attempt to exit.
#define B_XHCI_D0I2_ENTRY_HYSTER_TIMER  (BIT25|BIT24|BIT23|BIT22) ///<This field allows for a hysteresis timer to be implemented specifically for D0i2.  This will allow for D0i2 entry to be controlled independently from the timer used for D0i3 and D3.
#define B_XHCI_D0I2_ACT_PERIODIC_EP_DIS BIT21 ///< This field allows the xHC to control how aggressive it enters D0i2 in the presence of active Periodic EP's.
#define B_XHCI_MSI_D0I2_PWT             (BIT20|BIT19|BIT18|BIT17|BIT16) ///<This is the latency that is expected to be incurred to exit the D0i2 state.This wake latency is the latency to be added to the tracked D0i2 wake by the MSI module.
#define B_XHCI_MSI_IDLE_THRESHOLD       (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4) ///<his field allows the xHC to control how aggressive it enters D0i2 in the presence of pending MSI.  This field is valid only if Pending MSI Disable is "0", allowing D0i2 in the presence of pending MSI's.
#define B_XHCI_PENDING_MSI_DIS      (BIT3) ///<This field allows the xHC to disable D0i2 when there are pending MSI's in the event manager.
#define B_XHCI_FRAME_TIMER_RUN_DIS   (BIT2) ///<This field allows the xHC to disable D0i2 when the frame timer is running.
#define B_XHCI_USB2_L1_DIS              (BIT1) ///<This field allows the xHC to disable D0i2 when USB2 ports are in L1.  This implies that D0i2 will only be triggered when ports are in L2 or deeper.
#define B_XHCI_USB3_L1_DIS              (BIT0) ///<This field allows the xHC to disable D0i2 when USB3 ports are in U2.  This implies that D0i2 will only be triggered when ports are in U3 or deeper.

#define R_XHCI_D0I2_SCH_ALARM_CTRL      0x81C0
#define B_XHCI_D0I2_IDLE_TIME           0x1FFF0000 ///<bit28:16 This is the minimum schedule idle time that must be available before D0i2 can be allowed.
#define B_XHCI_D0I2_WAKE_LATENCY        0x1FFF ///<bit12:0 This is the latency that is expected to be incurred to exit the D0i2 state.

#define R_XHCI_USB2_PM_CTRL             0x81C4
#define B_XHCI_U2PSPGPSCBP              BIT11 ///<USB2 PHY SUS Power Gate PORTSC Block Policy:This controls the policy for blocking PORTSC Updates while the USB2 PHY SUS Well is power gated.
#define B_XHCI_U2PSPGEHC                (BIT10|BIT9|BIT8) ///<USB2 PHY SUS Well Power Gate Entry Hysteresis Count:This controls the amount of hysteresis time the controller will enforce after detecting the USB2 PHY SUS Power Gate entry condition.
#define B_XHCI_U2PSUSPGP                (BIT3|BIT2) ///<USB2 PHY SUS Well Power Gate Policy: This field controls when to enable the USB2 PHY SUS Well Power Gating when the proper conditions are met.
#define B_XHCI_U2PSUSPGP_Shadow     (BIT1|BIT0) ///<Shadow of USB2 PHY SUS Well Power Gate Policy: This filed is reserved but is required to shadow bits 3:2 due to a HW bug where some logic is using bit 1:0 instead of 3:2
#define R_XHCI_STRAP2                   0x8420  ///< USB3 Mode Strap
#define R_XHCI_SSIC_CFG_2_PORT1         0x880C
#define B_XHCI_SSIC_PORT_UNUSED         BIT31
#define B_XHCI_PROG_DONE                BIT30
#define B_XHCI_NUM_OF_MK0               (BIT29|BIT28|BIT27|BIT26)
#define B_XHCI_DISABLE_SCRAMBLING       BIT25
#define B_XHCI_RETRAIN_TIME             (BIT24|BIT23|BIT22|BIT21)
#define B_XHCI_PHY_RESET_TIME           (BIT20|BIT19|BIT18|BIT17|BIT16)
#define B_XHCI_LRST_TIME                0xFF00  ///<bit15:8
#define B_XHCI_ACTIVATE_LRST_TIME       0xFF  ///<bit7:0

#define R_XHCI_SSIC_CFG_2_PORT2         0x883C

#define R_XDCI_POW_PG_CONF              0xA0
#define B_XDCI_POW_PG_CONF_D3HEN        BIT18   ///< D3-Hot Enable
#define B_XDCI_POW_PG_CONF_DEVIDLEN     BIT17   ///< DEVIDLE Enable


#define R_OTG_BAR0                      0x10  ///< BAR 0
#define B_OTG_BAR0_BA                   0xFFE00000 ///< Base Address
#define V_OTG_BAR0_SIZE                 0x200000
#define N_OTG_BAR0_ALIGNMENT            21
#define B_OTG_BAR0_PREF                 BIT3  ///< Prefetchable
#define B_OTG_BAR0_ADDRNG               (BIT2 | BIT1) ///< Address Range
#define B_OTG_BAR0_SPTYP                BIT0  ///< Space Type (Memory)
#define R_OTG_GEN_INPUT_REGRW           0xC0
#define B_OTG_GEN_INPUT_REGRW_CPSU3     (BIT11 | BIT10) ///< Current Power State u3pmu
#define B_OTG_GEN_INPUT_REGRW_CPSU2     (BIT9 | BIT8) ///< Current Power State u2pmu

#endif

