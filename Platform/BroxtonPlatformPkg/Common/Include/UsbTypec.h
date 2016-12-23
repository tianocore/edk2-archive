/** @file
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _USB_TYPEC_
#define _USB_TYPEC_

#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>

#include <Library/I2CLib.h>
#include <Guid/SetupVariable.h>
#include <Library/PmicLib.h>
#include <Library/GpioLib.h>

#include <ScRegs/RegsUsb.h>
#include <ScRegs/RegsPcu.h>
#include <PlatformBaseAddresses.h>
#include <ScAccess.h>


typedef enum _CurrentState {
  Reset,
  Disabled,
  ErrorRecovery,
  UnAttcahedSink,
  AttachWaitSink,
  AttachedSink,
  UnattachedSource,
  AttachWaitSource,
  AttachedSource,
  TrySource,
  TryWaitSink,
  AudioAccessory,
  DebugAccessory,
  UnAttachedAccessory,
  AttachWaitAccessory,
  PoweredAccessory,
  UnSuppportedAccessory,
} CurrentState;

typedef enum _CC_Termination {
  CCUnKnown = -1,
  CCOpen = -1,
  CCTermRa = 0,
  CCTermRd,
  CCTermRdDef,
  CCTermRd1p5A,
  CCTermRd3p0A,
} CC_Termination;

typedef enum _ToggleState {
  TogRunning,
  TogSetToSrcCC1,
  TogSetToSrcCC2,
  TogSetToUfp,
  TogNotDefined,
  TogSetToSnkCC1,
  TogSetToSnkCC2,
  TogSetToAudioAccCC12,
} ToggleState;


///PD related DS

#define BXT_TYPEC_REG_CONTROL1   0x07

#define BXT_TYPEC_REG_CONTROL2   0x08
#define BXT_TYPEC_REG_CONTROL3   0x09
#define BXT_TYPEC_PERCMP_SEL_CRTL  0x02

#define BXT_PERICOMM_MUX_I2C_BUS_NO 4
#define BXT_PERICOMM_MUX_SLAVE_ADDR 0x54
///PD Related DS-End

typedef union _IDREG {
  UINT8 IDReg;
  struct {
    UINT8 MINREV   : 3;
    UINT8 MAJREV   : 3;
    UINT8 VENID    : 2;
  } IDRegBit;
} IDREG;

typedef union _USBCCTRL1_Reg {
  UINT8 USBCCTRL1_RegB;
  struct {
    UINT8 Modes        : 3;
    UINT8 Cur_Src      : 2;
    UINT8 DRP_Toggle   : 3;
  } USBCCTRL1_RegBit;
} USBCCTRL1_Reg;

typedef union _USBCCTRL2_Reg {
  UINT8 USBCCTRL2_RegB;
  struct {
    UINT8 UNATT_SNK   : 1;
    UINT8 UNATT_SRC   : 1;
    UINT8 DIS_ST      : 1;
    UINT8 ERR_REC     : 1;
    UINT8 ATT_SNK     : 1;
    UINT8 ATT_SRC     : 1;
    UINT8 TRY_SRC     : 1;
    UINT8 UNSUP_ACC   : 1;
  } USBCCTRL2_RegBit;
} USBCCTRL2_Reg;

typedef union _USBCCTRL3_Reg {
  UINT8 USBCCTRL3_RegB;
  struct {
    UINT8 DET_DIS      : 1;
    UINT8 PD_DIS       : 1;
    UINT8 RESETPHY     : 1;
    UINT8 ATE_TESTMODE : 1;
    UINT8 LOOPBACK     : 1;
    UINT8 IGNORE_VBUS  : 1;
    UINT8 Reserved     : 2;
  } USBCCTRL3_RegBit;
} USBCCTRL3_Reg;

typedef union _CCCTRL_Reg {
  UINT8 CCCTRL_RegB;
  struct {
    UINT8 PU_EN1    : 1;
    UINT8 VCONN_EN1 : 1;
    UINT8 TX_EN1    : 1;
    UINT8 PDWN_EN1  : 1;
    UINT8 CDET_EN1  : 1;
    UINT8 RDET_EN1  : 1;
    UINT8 ADC_EN1   : 1;
    UINT8 VBUSOK    : 1;
  } CCCTRL_RegBit;
} CCCTRL_Reg;

typedef union _CCSEL_Reg {
  UINT8 CCSEL_RegB;
  struct {
    UINT8 Reserved        : 4;
    UINT8 VCONN_SWAP_OFF  : 1;
    UINT8 VCONN_SWAP_ON   : 1;
    UINT8 CCSEL           : 2;
  } CCSEL_RegBit;
} CCSEL_Reg;

typedef union _USBCSTATUS1_Reg {
  UINT8 USBCSTATUS1_RegB;
  struct {
    UINT8 TYPEC_RSLT   : 4;
    UINT8 CC_ORIENT    : 2;
    UINT8 TYPEC_DET    : 2;
  } USBCSTATUS1_RegBit;
} USBCSTATUS1_Reg;

typedef union _USBCSTATUS2_Reg {
  UINT8 USBCSTATUS2_RegB;
  struct {
    UINT8 SHORT        : 1;
    UINT8 OVRTEMP      : 1;
    UINT8 ADC_DONE1    : 1;
    UINT8 ADC_DONE2    : 1;
    UINT8 PD_ALLOWED   : 1;
    UINT8 VBUS_REQUEST : 1;
    UINT8 Reserved     : 2;
  } USBCSTATUS2_Regbit;
} USBCSTATUS2_Reg;

typedef union _USBCSTATUS3_Reg {
  UINT8 USBCSTATUS3_RegB;
  struct {
    UINT8 TYPEC_STATE   : 5;
    UINT8 Reserved      : 2;
    UINT8 TYPEC_ACTIVE  : 1;
  } USBCSTATUS3_RegBit;
} USBCSTATUS3_Reg;

typedef union _CCCMP_Reg {
  UINT8 CCCMP_RegB;
  struct {
    UINT8 DET_RA    : 1;
    UINT8 DET_RD    : 1;
    UINT8 DET_DEF   : 1;
    UINT8 DET_1P5A  : 1;
    UINT8 DET_3A    : 1;
    UINT8 Reserved  : 3;
  } CCCMP_RegBit;
} CCCMP_Reg;

typedef union _CCSTATUS_Reg {
  UINT8 CCSTATUS_RegB;
  struct {
    UINT8 SNK_RP      : 1;
    UINT8 PWRDEF_SNK  : 1;
    UINT8 PWR1P5A_SNK : 1;
    UINT8 PWR3A_SNK   : 1;
    UINT8 SRC_RP      : 1;
    UINT8 SRC_RX      : 2;
    UINT8 Reserved    : 1;
  } CCSTATUS_RegBit;
} CCSTATUS_Reg;

typedef union _USBCIRQ1_Reg {
  UINT8 USBCIRQ1_RegB;
  struct {
    UINT8 SHORT       : 1;
    UINT8 OVRTEMP     : 1;
    UINT8 ADCDONE1    : 1;
    UINT8 ADCDONE2    : 1;
    UINT8 Reserved    : 4;
  } USBCIRQ1_RegBit;
} USBCIRQ1_Reg;

typedef union _USBCIRQ2_Reg {
  UINT8 USBCIRQ2_RegB;
  struct {
    UINT8 Reserved    : 2;
    UINT8 TX_FAIL     : 1;
    UINT8 TX_SUCCESS  : 1;
    UINT8 RX_CR       : 1;
    UINT8 RX_HR       : 1;
    UINT8 RX_PD       : 1;
    UINT8 CC_CHANGE   : 1;
  } USBCIRQ2_RegBit;
} USBCIRQ2_Reg;

typedef union _MUSBCIRQ1_Reg {
  UINT8 MUSBCIRQ1_RegB;
  struct {
    UINT8 MSHORT      : 1;
    UINT8 MOVRTEMP    : 1;
    UINT8 MADCDONE1   : 1;
    UINT8 MADCDONE2   : 1;
    UINT8 Reserved    : 4;
  } MUSBCIRQ1_RegBit;
} MUSBCIRQ1_Reg;

typedef union _MUSBCIRQ2_Reg {
  UINT8 MUSBCIRQ2_RegB;
  struct {
    UINT8 Reserved    : 2;
    UINT8 MTX_FAIL    : 1;
    UINT8 MTX_SUCCESS : 1;
    UINT8 MRX_CR      : 1;
    UINT8 MRX_HR      : 1;
    UINT8 MRX_PD      : 1;
    UINT8 MCC_CHANGE  : 1;
  } MUSBCIRQ2_RegBit;
} MUSBCIRQ2_Reg;

typedef union _PDCFG1_Reg {
  UINT8 PDCFG1_RegB;
  struct {
    UINT8 ID_WA      : 1;
    UINT8 Reserved   : 2;
    UINT8 ID_FILL    : 1;
  } PDCFG1_RegBit;
} PDCFG1_Reg;

typedef union _PDCFG2_Reg {
  UINT8 PDCFG2_RegB;
  struct {
    UINT8 SOP_RX    : 5;
    UINT8 Reserved  : 3;
  } PDCFG2_RegBit;
} PDCFG2_Reg;

typedef union _PDCFG3_Reg {
  UINT8 PDCFG3_RegB;
  struct {
    UINT8 PWRROLE    : 1;
    UINT8 DATAROLE   : 1;
    UINT8 SR_SOP0    : 2;
    UINT8 SR_SOP1    : 2;
    UINT8 SR_SOP2    : 2;
  } PDCFG3_RegBit;
} PDCFG3_Reg;

typedef union _PDSTATUS_Reg {
  UINT8 PDSTATUS_RegB;
  struct {
    UINT8 TXRSLT    : 3;
    UINT8 RXRSLT    : 3;
    UINT8 Reserved  : 2;
  } PDSTATUS_RegBit;
} PDSTATUS_Reg;

typedef union _RXSTATUS_Reg {
  UINT8 RXSTATUS_RegB;
  struct {
    UINT8 RXCLEAR    : 1;
    UINT8 Reserved   : 5;
    UINT8 RXOVERRUN  : 1;
    UINT8 RXDATA     : 1;
  } RXSTATUS_RegBit;
} RXSTATUS_Reg;

typedef union _RXINFO_Reg {
  UINT8 RXINFO_RegB;
  struct {
    UINT8 RXSOP      : 3;
    UINT8 RXBYTES    : 5;
  } RXINFO_RegBit;
} RXINFO_Reg;

typedef union _TXCMD_Reg {
  UINT8 TXCMD_RegB;
  struct {
    UINT8 TXBUF_RDY   : 1;
    UINT8 TX_START    : 1;
    UINT8 Reserved    : 3;
    UINT8 TX_CMD      : 3;
  } TXCMD_RegBit;
} TXCMD_Reg;

typedef union _TXINFO_Reg {
  UINT8 TXINFO_RegB;
  struct {
    UINT8 TX_SOP      : 3;
    UINT8 TX_RETRIES  : 3;
    UINT8 Reserved    : 2;
  } TXINFO_RegBit;
} TXINFO_Reg;

typedef union _AFE_ADC1_Reg {
  UINT8 AFE_ADC1_RegB;
  struct {
    UINT8 ADC1         : 6;
    UINT8 ADCSTART1    : 1;
    UINT8 Reserved     : 1;
  } AFE_ADC1_RegBit;
} AFE_ADC1_Reg;

typedef union _AFE_PWRSTATE_Reg {
  UINT8 AFE_PWRSTATE_RegB;
  struct {
    UINT8 POWER        : 4;
    UINT8 Reserved     : 4;
  } AFE_PWRSTATE_RegBit;
} AFE_PWRSTATE_Reg;

typedef union _Register_1 {
  UINT8 DeviceID;
  struct {
    UINT8 RevisionID   : 4;
    UINT8 VersionID    : 4;
  } DeviceIDBit;
} Register_1;

typedef union _Register_2 {
  UINT8 Switches0;
  struct {
    UINT8 PDWN1      : 1;
    UINT8 PDWN2      : 1;
    UINT8 MEAS_CC1   : 1;
    UINT8 MEAS_CC2   : 1;
    UINT8 VCONN_CC1  : 1;
    UINT8 VCONN_CC2  : 1;
    UINT8 PU_EN1     : 1;
    UINT8 PU_EN2     : 1;
  } Switches0Bit;
} Register_2;

typedef union _Register_3 {
  UINT8 Switches1;
  struct {
    UINT8 TXCC1       : 1;
    UINT8 TXCC2       : 1;
    UINT8 AUTO_CRC    : 1;
    UINT8 RESERVED    : 1;
    UINT8 DATAROLE    : 1;
    UINT8 SPECREV1_0  : 2;
    UINT8 POWERROLE   : 1;
  } Switches1Bit;
} Register_3;

typedef union _Register_4 {
  UINT8 Measure;
  struct {
    UINT8 MDAC5_0      : 6;
    UINT8 MEAS_VBUS    : 1;
    UINT8 RESERVED     : 1;
  } MeasureBit;
} Register_4;

typedef union _Register_5 {
  UINT8 Slice;
  struct {
    UINT8 SDAC5_0      : 6;
    UINT8 SDAC_HYS1_2  : 2;
  } SliceBit;
} Register_5;

typedef union _Register_6 {
  UINT8 Control0;
  struct {
    UINT8 TX_START     : 1;
    UINT8 AUTO_PRE     : 1;
    UINT8 HOST_CUR1_0  : 2;
    UINT8 RESERVED1    : 1;
    UINT8 INT_MASK     : 1;
    UINT8 TX_FLUSH     : 1;
    UINT8 RESERVED     : 1;
  } Control0Bit;
} Register_6;

typedef union _Register_7 {
  UINT8 Control1;
  struct {
    UINT8 ENSOP2_1     : 2;
    UINT8 RX_FLUSH     : 1;
    UINT8 RESERVED1    : 1;
    UINT8 BIST_MODE2   : 1;
    UINT8 ENSOP1DB     : 1;
    UINT8 ENSOP2DB     : 1;
    UINT8 RESERVED     : 1;
  } Control1Bit;
} Register_7;

typedef union _Register_8 {
  UINT8 Control2;
  struct {
    UINT8 TOGGLE           : 1;
    UINT8 MODE1_0          : 2;
    UINT8 WAKE_EN          : 1;
    UINT8 RESERVED         : 1;
    UINT8 TOG_RD_ONLY      : 1;
    UINT8 TOG_SAVE_PWR2_1  : 2;
  } Control2Bit;
} Register_8;

typedef union _Register_9 {
  UINT8 Control3;
  struct {
    UINT8 AUTO_RETRY      : 1;
    UINT8 N_RETRIES1_0    : 2;
    UINT8 AUTO_SOFTRESET  : 1;
    UINT8 AUTO_HARDRESET  : 1;
    UINT8 RESERVED1       : 1;
    UINT8 SEND_HARD_RESET : 1;
    UINT8 RESERVED        : 1;
  } Control3Bit;
} Register_9;

typedef union _Register_A {
  UINT8 Mask1;
  struct {
    UINT8 M_BC_LVL     : 1;
    UINT8 M_COLLISION  : 1;
    UINT8 M_WAKE       : 1;
    UINT8 M_ALERT      : 1;
    UINT8 M_CRC_CHK    : 1;
    UINT8 M_COMP_CHNG  : 1;
    UINT8 M_ACTIVITY   : 1;
    UINT8 M_VBUSOK     : 1;
  } Mask1Bit;
} Register_A;

typedef union _Register_B {
  UINT8 Power;
  struct {
    UINT8 PWR0      : 1;
    UINT8 PWR1      : 1;
    UINT8 PWR2      : 1;
    UINT8 PWR3      : 1;
    UINT8 RESERVED  : 4;
  } PowerBit;
  struct {
    UINT8 PWR3_0    : 4;
    UINT8 RESERVED  : 4;
  } PowerBits;
} Register_B;

typedef union _Register_C {
  UINT8 Reset;
  struct {
    UINT8 SW_RES     : 1;
    UINT8 PD_RESET   : 1;
    UINT8 RESERVED   : 6;
  } ResetBit;
} Register_C;

typedef union _Register_D {
  UINT8 OcpPreg;
  struct {
    UINT8 OCP_CUR2_1 : 3;
    UINT8 OCP_RANGE  : 1;
    UINT8 RESERVED   : 4;
  } OcpPregBit;
} Register_D;

typedef union _Register_E {
  UINT8 MaskA;
  struct {
    UINT8 M_HARDRST    : 1;
    UINT8 M_SOFTRST    : 1;
    UINT8 M_TXSENT     : 1;
    UINT8 M_HARDSENT   : 1;
    UINT8 M_RETRYFAIL  : 1;
    UINT8 M_SOFTFAIL   : 1;
    UINT8 M_TOGDONE    : 1;
    UINT8 M_OCP_TEMP   : 1;
  } MaskABit;
} Register_E;

typedef union _Register_F {
  UINT8 MaskB;
  struct {
    UINT8 M_GCRCSENT   : 1;
    UINT8 M_RESERVED   : 7;
  } MaskBBit;
} Register_F;

typedef union _Register_3C {
  UINT8 Status0A;
  struct {
    UINT8 HARDRST    : 1;
    UINT8 SOFTRST    : 1;
    UINT8 POWER2     : 1;
    UINT8 POWER3     : 1;
    UINT8 RETRYFAIL  : 1;
    UINT8 SOFTFAIL   : 1;
    UINT8 RESERVED   : 2;
  } Status0ABit;
} Register_3C;

typedef union _Register_3D {
  UINT8 Status1A;
  struct {
    UINT8 RXSOP      : 1;
    UINT8 RXSOP1DB   : 1;
    UINT8 RXSOP2DB   : 1;
    UINT8 TOGSS3_1   : 3;
    UINT8 RESERVED   : 2;
  } Status1ABit;
} Register_3D;

typedef union _Register_3E {
  UINT8 InterruptA;
  struct {
    UINT8 I_HARDRST    : 1;
    UINT8 I_SOFTRST    : 1;
    UINT8 I_TXSENT     : 1;
    UINT8 I_HARDSENT   : 1;
    UINT8 I_RETRYFAIL  : 1;
    UINT8 I_SOFTFAIL   : 1;
    UINT8 I_TOGDONE    : 1;
    UINT8 I_OCP_TEMP   : 1;
  } InterruptABit;
} Register_3E;

typedef union _Register_3F {
  UINT8 InterruptB;
  struct {
    UINT8 I_GCRCSENT   : 1;
    UINT8 I_RESERVED   : 7;
  } InterruptBBit;
} Register_3F;

typedef union _Register_40 {
  UINT8 Status0;
  struct {
    UINT8 BC_LVL0   : 1;
    UINT8 BC_LVL1   : 1;
    UINT8 WAKE      : 1;
    UINT8 ALERT     : 1;
    UINT8 CRC_CHK   : 1;
    UINT8 COMP      : 1;
    UINT8 ACTIVITY  : 1;
    UINT8 VBUSOK    : 1;
  } Status0Bit;
  struct {
    UINT8 BC_LVL    : 2;
    UINT8 WAKE      : 1;
    UINT8 ALERT     : 1;
    UINT8 CRC_CHK   : 1;
    UINT8 COMP      : 1;
    UINT8 ACTIVITY  : 1;
    UINT8 VBUSOK    : 1;
  } Status0Bits;
} Register_40;

typedef union _Register_41 {
  UINT8 Status1;
  struct {
    UINT8 OCP       : 1;
    UINT8 OVRTEMP   : 1;
    UINT8 TX_FULL   : 1;
    UINT8 TX_EMPTY  : 1;
    UINT8 RX_FULL   : 1;
    UINT8 RX_EMPTY  : 1;
    UINT8 RXSOP1    : 1;
    UINT8 RXSOP2    : 1;
  } Status1Bit;
} Register_41;

typedef union _Register_42 {
  UINT8 Interrupt;
  struct {
    UINT8 I_BC_LVL     : 1;
    UINT8 I_COLLISION  : 1;
    UINT8 I_WAKE       : 1;
    UINT8 I_ALERT      : 1;
    UINT8 I_CRC_CHK    : 1;
    UINT8 I_COMP_CHNG  : 1;
    UINT8 I_ACTIVITY   : 1;
    UINT8 I_VBUSOK     : 1;
  } InterruptBit;
} Register_42;


typedef struct _PrSwapStat {
  BOOLEAN InProgress;
  BOOLEAN Done;
  BOOLEAN PrSwapSent;
  BOOLEAN PrSwapReceived;
  BOOLEAN IsSource;
  BOOLEAN AcceptSent;
  BOOLEAN AcceptReceived;
  BOOLEAN PsRdySent;
  BOOLEAN PsRdyReceived;
  BOOLEAN SendCaps;
  BOOLEAN IsPowerAcc;
} PrSwapStat;


//Misc

CC_Termination
EFIAPI
Detect_CC();

CC_Termination
EFIAPI
Detect_CCLine();

//
// Delay
//
#define DELAY_BETWEEN_INSTRUCTION_35000   35000
#define WC_PMIC_READ_INSTRUCTION_DELAY    10
#define WC_PMIC_WRITE_INSTRUCTION_DELAY   100
#define DELAY_30_MSEC 30000
#define DELAY_100_MICRO_SEC 100
#define DELAY_1_MSEC        1000


typedef union _MessageHeader {
  UINT16 MessHeader;
  UINT8  Byte[2];
  struct {
    UINT16 MessageType  : 4;
    UINT16 Reserved1    : 1;
    UINT16 PortDataRole : 1;
    UINT16 SpecRevision : 2;
    UINT16 PortPowerRole: 1;
    UINT16 MessageID    : 3;
    UINT16 NoOfDataObjects : 3;
    UINT16 Reserved2    : 1;
  } MessageBits;
} MessageHeader, *pMessageHeader;

typedef union _FixedSupplyPDO {
  unsigned int DataPDO;
  unsigned char Byte[4];
  struct {
    unsigned MaxCurrentIn10mA        : 10;
    unsigned VoltageIn50mV           : 10;
    unsigned PeakCurrent             : 2;
    unsigned Reserved                : 3;
    unsigned DataRoleSwap            : 1;
    unsigned UsbCommunicationCapable : 1;
    unsigned ExternallyPowered       : 1;
    unsigned USBSuspend              : 1;
    unsigned DualRolePower           : 1;
    unsigned FixedSupply             : 2;
  } DataBits;
} FixedSupplyPDO, *pFixedSupplyPDO;

typedef union _FixedRequestPDO {
  UINT32 ReqPDO;
  UINT8  Byte[4];
  struct {
    UINT32 MaxOperatingCurrentIn10mA : 10;
    UINT32 OperatingCurrentIn10mA    : 10;
    UINT32 Reserved1                 : 4;
    UINT32 NoUSBSuspend              : 1;
    UINT32 USBCommCapable            : 1;
    UINT32 CapabilityMismatch        : 1;
    UINT32 GiveBackFlag              : 1;
    UINT32 ObjectPosition            : 3;
    UINT32 Reserved                  : 1;
  } RequestBits;
} FixedRequestPDO, *pFixedRequestPDO;

typedef enum _ControlMessageType {
  GoodCrc=1,
  GoToMin,
  Accept,
  Reject,
  Ping,
  Ps_Rdy,
  Get_Source_Cap,
  Get_Sink_Cap,
  Dr_Swap,
  Pr_Swap,
  Vconn_Swap,
  Wait,
  Soft_Reset,
} ControlMessageType;

typedef enum _DataMessageType {
  Src_Caps=1,
  Request,
  Bist,
  Sink_Caps,
  Vendor_Defined=0xF,
} DataMessageType;

#endif

