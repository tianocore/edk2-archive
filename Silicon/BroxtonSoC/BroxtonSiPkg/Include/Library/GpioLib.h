/** @file
  This library provides the GPIO library definitions.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _GPIO_LIB_H_
#define _GPIO_LIB_H_

#include "BxtPGpioLib.h"

// GPIO Community Port ID definition
#define GPIO_SOUTHWEST_COMMUNITY     0xC0
#define GPIO_SOUTH_COMMUNITY         0xC2
#define GPIO_NORTHWEST_COMMUNITY     0xC4
#define GPIO_NORTH_COMMUNITY         0xC5
#define GPIO_WEST_COMMUNITY          0xC7

//
// GPIO Community Port ID definition
//
#define GPIO_MMIO_OFFSET_SW          0xC0
#define GPIO_MMIO_OFFSET_S           0xC2
#define GPIO_MMIO_OFFSET_NW          0xC4
#define GPIO_MMIO_OFFSET_N           0xC5
#define GPIO_MMIO_OFFSET_W           0xC7

#define BXT_GPIO_PAD_CONF0_OFFSET    0x0
#define BXT_GPIO_PAD_CONF1_OFFSET    0x4
#define NA                           0xFF

#define ALL_COMMUNITY_GPIO_PIN_NUM   248

// GPIO pad DW0 MASK
#define PAD_DW0_MASK  0xF961E0FE           ///< bit 27, 24,  22, 21, 16:13, 7:1 are RO, mask bit [31:28] as they are modified as usual, special operation for them if need.
// GPIO pad DW1 MASK
#define PAD_DW1_MASK  0xFFFC00FF           ///< bit [17:14],[13:10],[9:8]  are R/W

//
// BXTM GPIO Groups
// Use below for functions from GPIO Lib which
// require GpioGroup as argument
//
#define GPIO_BXTM_GROUP_0  0x0100 ///< SOUTH     GPIO pin  0 ~ 19 belong to BXTM GROUP0
#define GPIO_BXTM_GROUP_1  0x0101 ///< SOUTHWEST GPIO pin  0 ~ 23 belong to BXTM GROUP1
#define GPIO_BXTM_GROUP_2  0x0102 ///< WEST      GPIO pin  0 ~ 23 belong to BXTM GROUP2
#define GPIO_BXTM_GROUP_4  0x0104 ///< NORTHWEST GPIO pin 21 ~ 23 belong to BXTM GROUP4
#define GPIO_BXTM_GROUP_5  0x0105 ///< NORTH     GPIO pin 32 ~ 55 belong to BXTM GROUP8
#define GPIO_BXTM_GROUP_6  0x0106 ///< NORTHWEST GPIO pin 64 ~ 71 belong to BXTM GROUP6
#define GPIO_BXTM_GROUP_7  0x0107 ///< NORTH     GPIO pin  0 ~ 23 belong to BXTM GROUP7
#define GPIO_BXTM_GROUP_8  0x0108 ///< NORTH     GPIO pin 32 ~ 55 belong to BXTM GROUP8
#define GPIO_BXTM_GROUP_9  0x0109 ///< NORTH     GPIO pin 64 ~ 73 belong to BXTM GROUP9

//
// BXTP GPIO Groups
// Use below for functions from GPIO Lib which
// require GpioGroup as argument
//
#define GPIO_BXTP_GROUP_0  0x0200 ///< SOUTHWEST GPIO pin  0 ~ 31 belong to BXTP GROUP0
#define GPIO_BXTP_GROUP_1  0x0201 ///< SOUTHWEST GPIO pin 32 ~ 42 belong to BXTP GROUP1
#define GPIO_BXTP_GROUP_2  0x0202 ///< WEST      GPIO pin  0 ~ 25 belong to BXTP GROUP2
#define GPIO_BXTP_GROUP_4  0x0204 ///< NORTHWEST GPIO pin  0 ~ 17 belong to BXTP GROUP4
#define GPIO_BXTP_GROUP_5  0x0205 ///< NORTHWEST GPIO pin 32 ~ 63 belong to BXTP GROUP5
#define GPIO_BXTP_GROUP_6  0x0206 ///< NORTHWEST GPIO pin 64 ~ 76 belong to BXTP GROUP6
#define GPIO_BXTP_GROUP_7  0x0207 ///< NORTH     GPIO pin  0 ~ 31 belong to BXTP GROUP7
#define GPIO_BXTP_GROUP_8  0x0208 ///< NORTH     GPIO pin 32 ~ 61 belong to BXTP GROUP8

//
// For any GpioGroup usage in code use GPIO_GROUP type
//
typedef UINT32 GPIO_GROUP;

#define GPIO_GET_GROUP_INDEX(Group)           (Group & 0xFF)

#define R_GPIO_MISCCFG          0x10         ///< Miscellaneous Configuration
#define B_GPIO_MISCCFG_GPE0_DW2 (BIT19 | BIT18 | BIT17 | BIT16)
#define N_GPIO_MISCCFG_GPE0_DW2 16
#define B_GPIO_MISCCFG_GPE0_DW1 (BIT15 | BIT14 | BIT13 | BIT12)
#define N_GPIO_MISCCFG_GPE0_DW1 12
#define B_GPIO_MISCCFG_GPE0_DW0 (BIT11 | BIT10 | BIT9 | BIT8)
#define N_GPIO_MISCCFG_GPE0_DW0 8
#define B_GPIP_MISCCFG          (BIT0|BIT1)  ///< Enable participation in dynamic local & partition clock gating

#define PAD_OWNERSHIP_0       0x20
#define V_PAD_OWNERSHIP_MASK  0x03
#define V_PAD_OWNERSHIP_HOST  0x00
#define HOSTSW_OWNERSHIP_0    0x80
#define GPI_IS_0              0x100
#define GPI_IE_0              0x110
#define GPI_GPE_STS_0         0x120
#define GPI_GPE_EN_0          0x130
#define GPI_SMI_STS_0         0x140
#define GPI_SMI_EN_0          0x150
#define EVOUTEN_0             0x210
#define EVMAP_0               0x220

#define EVENT_MUX_SIZE        16 ///< MUX is 16:1 on Broxton
#define WAKE_EVENT_NUM        16 ///< 16 direct IRQ wake events are supported on Broxton in North and Northwest communities.

typedef enum {
  Native = 0xff,
  GPIO   = 0,       ///< Native, no need to set PAD_VALUE
  GPI    = 1,       ///< GPI, input only in PAD_VALUE
  GPO    = 2,       ///< GPO, output only in PAD_VALUE
  HI_Z   = 3,
} GPIO_En;

typedef enum {
  LO = 0,
  HI = 1,
} GPO_D4;

/*
  Mapping to CONF0 bit 27:24
  Note: Assume "Direct Irq En" is not set, unless specially notified
*/
typedef enum {
  P_NONE  = 0,        ///< Pull None
  P_5K_L  = 2,        ///< Pull Down  5K
  P_20K_L = 4,        ///< Pull Down  20K
  P_1K_H  = 9,        ///< Pull Up 1K
  P_2K_H  = 11,       ///< Pull Up 2K
  P_20K_H = 12,       ///< Pull Up 20K
  P_5K_H  = 10,       ///< Pull Up  5K
  P_1K2K_H  = 13,     ///< Pull Up  1K
  Native_control = 15
} PULL_TYPE;

typedef enum {
  M0 = 0,
  M1,
  M2,
  M3,
  M4,
  M5
} ModeList;

typedef enum {
  Level    = 0,
  Edge     = 1,
  INT_DIS  = 2,
  BothEdge = 3,
} INT_Type;

typedef enum {
  SOUTH     = GPIO_SOUTH_COMMUNITY,
  WEST      = GPIO_WEST_COMMUNITY,
  NORTHWEST = GPIO_NORTHWEST_COMMUNITY,
  SOUTHWEST = GPIO_SOUTHWEST_COMMUNITY,
  NORTH     = GPIO_NORTH_COMMUNITY
} coms;

typedef enum {
  IOAPIC = 8 ,
  SCI    = 4,
  SMI    = 2,
  NMI    = 1,
  NONE   = 0,
} GPIO_Route;

/*
  InvertRxTx 7:4
  0 - No Inversion
  1 - Inversion
  [0] RX Enable
  [1] TX Enable
  [2] RX Data
  [3] TX Data
*/
typedef enum {
  No_invert = 0,
  Inverted  = 1,
} InvertRX;

typedef enum {
  ACPI_D = 0, // ACPI driver
  GPIO_D = 1, // GPIO driver
} HOSTSW;

/*
  I/O Standby State
*/
typedef enum {
  Last_Value = 0,  ///< 0 = Latch last value driven on TX, TX Enable and RX Enable
  D0RxDRx0I  = 1,  ///< 1 = Drive 0 with RX disabled and RX drive 0 internally
  D0RxDRx1I  = 2,  ///< 2 = Drive 0 with RX disabled and RX drive 1 internally
  D1RxDRx0I  = 3,  ///< 3 = Drive 1 with RX disabled and RX drive 0 internally
  D1RxDRx1I  = 4,  ///< 4 = Drive 1 with RX disabled and RX drive 1 internally
  D0RxE      = 5,  ///< 5 = Drive 0 with RX enabled
  D1RxE      = 6,  ///< 6 = Drive 1 with RX enabled
  HizRx0I    = 7,  ///< 7 = Hi-Z with RX drive 0 internally
  HizRx1I    = 8,  ///< 8 = Hi-Z with RX drive 1 internally
  TxDRxE     = 9,  ///< 9 = TX Disabled and RX Enabled (i.e. wake or interrupt)
  IOS_Masked = 15, ///< 15 = IO Standby signal is masked for this pad.
} IOSSTATE;

/*
  I/O Standby Termination
*/
typedef enum {
  SAME    = 0,  ///< 0 = Same as functional pull
  DisPuPd = 1,  ///< 1 = Disable Pullup and Pulldown
  EnPd    = 2,  ///< 2 = Enable Pulldown
  EnPu    = 3,  ///< 3 = Enable Pullup
} IOSTERM;

/*
  Wake enable or disable for direct IRQ GPIO pin
*/
typedef enum {
  Wake_Disabled = FALSE,
  Wake_Enabled  = TRUE,
} WAKE_CAPABLE;

typedef enum
{
  Host_Gpio_Mode = 0,
  CSME_Gpio_Mode = 1,
  ISH_Gpio_Mode = 2,
  IE_Gpio_mode = 3,
} GPIO_PAD_OWNERSHIP;

typedef union {
  UINT32 padCnf0;
  struct {
    UINT32 GPIOTxState:1; ///< 0      GPIO TX State
    UINT32 GPIORxState:1; ///< 1      GPIO RX State, RO
    UINT32 Reserved1:6;   ///< 2-7    Reserved, RO
    UINT32 GPIORxTxDis:2; ///< 8-9    GPIO RX Disable[9], GPIO TX Disable[8]
    UINT32 PMode:3;       ///< 10-12  Pad Mode, 0h = GPIO Controller controls the Pad; 1h = Native Function 1, if applicable, controls the pad; 2h = Native Function 2, if applicable, controls the pad, etc.
    UINT32 Reserved2:4;   ///< 13-16  Reserved, RO
    UINT32 GPIRout:4;     ///< 17-20  Route to IOxAPIC[20], SCI[19], SMI[18], NMI[17]
    UINT32 RXTXEnCfg:2;   ///< 21-22  RX/TX Enable Config (RXTXEnCfg) RO
    UINT32 RXINV:1;       ///< 23     RX Invert, 0 = No inversion; 1 = Inversion
    UINT32 PreGfRXSel:1;  ///< 24     Pre Glitch Filter Stage RX Pad State Select,  RO, not support in BXT
    UINT32 RxEvCfg:2;     ///< 25-26  0h = Level 1h  = Edge (RxInv=0 for rising edge; 1 for falling edge), 2h  = Disabled ,3h  =  Either rising edge or falling edge
    UINT32 Reserved3:1;   ///< 27     RO
    UINT32 RXRAW1:1;      ///< 28     Override the RX to 1
    UINT32 RXPadStSel:1;  ///< 29     RX Pad State Select
    UINT32 PadRstCfg:2;   ///< 30-31  Pad Reset Config
  } r;
} BXT_CONF_PAD0;

typedef union {
  UINT32 padCnf1;
  struct {
    UINT32 IntSel:7;      ///< 0-6    Interrupt Select. RO
    UINT32 Reserved:1;    ///< 7      Reserved.
    UINT32 IOSTerm:2;     ///< 8-9    I/O Standby Termination (IOSTerm) RW
    UINT32 Term:4;        ///< 10-13  Termination,
                          ///<        0 000: none;0 010: 5k wpd;0 100: 20k wpd;1 000: none;1 001: 1k wpu;1 011: 2k wpu;1 010: 5k wpu;
                          ///<        1 100: 20k wpu;1 101: 1k & 2k wpu;1 111: (optional) Native controller selected by Pad Mode controls the Termination
    UINT32 IOSState:4;    ///< 14-17  I/O Standby State, I/O Standby is not implemented in BXT, RW
    UINT32 CFIOPadCfg:14; ///< 18-31  For BXT, this is done thru Family Register if necessary. RO
  } r;
} BXT_CONF_PAD1;

typedef struct {
  BXT_CONF_PAD0  padConfg0;
  BXT_CONF_PAD1  padConfg1;
  UINT8          Community;
  UINT16         MMIO_ADDRESS;
  BOOLEAN        HostSw;
  BOOLEAN        WakeEnabled; ///< Wake enable for direct IRQ pin
  CHAR16         *pad_name;   ///< GPIO Pin Name for debug purpose
} BXT_GPIO_PAD_INIT;

typedef struct {
  UINT32         CommAndOffset;
  CHAR16*        pad_name;
} BXT_GPIO_PAD_TABLE;

typedef struct {
  UINT32         CommAndOffset;
  BXT_CONF_PAD0  padConfg0;
  BXT_CONF_PAD1  padConfg1;
  UINT8          HostSw;
  BOOLEAN        WakeEnabled;
} BXT_GPIO_CONFIG;

typedef union {
  UINT32 DW;
  struct {
    UINT32 Community:     8;
    UINT32 Offset:        12;
    UINT32 HostSw:        1;
    UINT32 DirectIrqWake: 1;
    UINT32 Reserved0:     10;
  } r;
} GPIO_CONFIG_SMIP_HALF0;

typedef union {
  UINT32 DW;
  struct {
    UINT32 padConfig0: 20; ///< GPIO Pad DW0's R/W bits: [0], [8:12], [17:26], [28:31]
    UINT32 padConfig1: 10; ///< GPIO pad DW1's R/W bits:[8:17]
    UINT32 Reserved0:  2;
  } r;
} GPIO_CONFIG_SMIP_HALF1;

typedef struct {
  GPIO_CONFIG_SMIP_HALF0 Half0;
  GPIO_CONFIG_SMIP_HALF1 Half1;
} GPIO_CONFIG_SMIP;

// product all items of BXT_GPIO_PAD_INIT
#define BXT_GPIO_PAD_CONF(pad_name, PMode, GPIO_Config, HostSw, GPO_STATE, INT_Trigger, WakeEnabled, Term_H_L, Inverted, GPI_ROUT, IOSState, IOSTerm, MMIO_Offset, Community ) { \
    {((((INT_Trigger) != NA) ? ((UINT32) INT_Trigger << 25) : (UINT32)INT_DIS << 25 )  | (((Inverted) != NA) ? ((UINT32) Inverted << 23) : (UINT32)No_invert << 23) | (((GPI_ROUT) != NA) ? ((UINT32) GPI_ROUT << 17) : (UINT32)NONE << 17) | ((UINT32) PMode << 10) |  (((GPIO_Config) != NA) ? ((UINT32) GPIO_Config << 8 ): (UINT32)0 << 8) | (((GPO_STATE) == HI ) ? HI : LO))}, \
    {(((IOSState != NA) ? ((UINT32) IOSState << 14 ) : Last_Value) | (((Term_H_L) != NA) ? ((UINT32) Term_H_L << 10) : P_NONE) | ((IOSTerm != NA) ? ((UINT32) IOSTerm << 8) : SAME))}, \
        Community, \
        (MMIO_Offset != NA) ? (MMIO_Offset) : 0, \
        ((HostSw == NA)||(HostSw == ACPI_D))? ACPI_D : GPIO_D,\
        WakeEnabled,\
        pad_name \
        }


#define SMIP_Config0_Convert(SmipPadConfig0) ( \
          (((UINT32)SmipPadConfig0 & (BIT16 | BIT17 | BIT18 | BIT19)) << 12) | \
          (((UINT32)SmipPadConfig0 & (0xFFC0)) << 11) | \
          (((UINT32)SmipPadConfig0 & (BIT1 | BIT2 | BIT3 |BIT4 | BIT5))  << 7) | \
          ((UINT32)SmipPadConfig0 & (BIT0)) \
        )

#define SMIP_Config1_Convert(SmipPadConfig1) ( \
          ((UINT32)SmipPadConfig1 & 0x3FF) << 8 \
        )

/**
  Common interface to write an 32-bit GPIO register.

  @param[in]  Community         Community Port ID definition.
                                SouthWest Community:     0xC0
                                South Community    :     0xC2
                                NorthWest Community:     0xC4
                                North Community    :     0xC5
                                West Community     :     0xC7
  @param[in]  Offset            The GPIO register to write.
  @param[in]  Value             The value to write.

  @retval     EFI_SUCCESS       Write GPIO register successfully.

**/
EFI_STATUS
GpioWrite (
  IN UINT8     Community,
  IN UINT16    Offset,
  IN UINT32    Value
  );

/**
  Common interface to read an 32-bit GPIO register.

  @param[in]  Community         Community Port ID definition.
                                SouthWest Community:     0xC0
                                South Community    :     0xC2
                                NorthWest Community:     0xC4
                                North Community    :     0xC5
                                West Community     :     0xC7
  @param[in]  Offset            The GPIO register to read.

  @retval     UINT32            The 32-bit value read from side band.

**/
UINT32
GpioRead (
  IN UINT8     Community,
  IN UINT16    Offset
  );

/**
  Common interface to read an 32-bit specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadRead (N_GPIO_20)

  @param[in]  CommAndOffset       It contains GPIO community and offset.

  @retval     UINT32              The 32-bit value read from GpioRead operation.

**/
UINT32
GpioPadRead (
  IN UINT32    CommAndOffset
  );

/**
  Common interface to write an 32-bit to a specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadWrite (N_GPIO_20, 0xXXXXXXXX)

  @param[in]  CommAndOffset   It contains GPIO community and offset.
  @param[in]  Value           The value to write.

  @retval     EFI_SUCCESS     Write to a specific GPIO pad register successfully.
  @retval     Others          Status depends on GpioWrite operation.

**/
EFI_STATUS
GpioPadWrite(
  IN UINT32    CommAndOffset,
  IN UINT32    Value
  );

/**
  Set GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num      GPIO Pin Number to configure.
  @param[in]  Gpio_Conf_Data    Data array of default setting for each GPIO communities.

**/
VOID
GpioPadConfigTable (
  IN UINT32               Gpio_Pin_Num,
  IN BXT_GPIO_PAD_INIT    *Gpio_Conf_Data
  );

/**
  Program SMIP GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num           GPIO Pin Number to configure.
  @param[in]  Gpio_Smip_Conf_Data    Data array from SMIP setting for each GPIO communities.

**/
VOID
GpioSmipPadConfigTable (
  IN UINT32                    Gpio_Pin_Num,
  IN GPIO_CONFIG_SMIP*         Gpio_Smip_Conf_Data
  );

/**
  Set GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Conf_Data        The structure contains GPIO community and pad 0/1 value.

**/
VOID
GpioPadConfigItem (
  IN BXT_GPIO_CONFIG    Gpio_Conf_Data
  );

/**
  This procedure will get GPIO pad input inversion value

  @param[in]  GpioPad                GPIO pad
  @param[out] InvertState            GPIO inversion state
                                     0: No input inversion, 1: Inverted input

  @retval     EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
GpioGetInputInversion (
  IN  UINT32                   GpioPad,
  OUT UINT32                   *InvertState
  );

/**
  This procedure will set GPIO pad input inversion value

  @param[in] GpioPad               GPIO pad
  @param[in] Value                 GPIO inversion value

  @retval    EFI_SUCCESS           Set GPIO pad input inversion value successfully.
  @retval    Others                Status depends on GpioPadWrite operation.

**/
EFI_STATUS
GpioSetInputInversion (
  IN UINT32                   GpioPad,
  IN UINT64                   Value
  );

/**
  This procedure is used to clear SMI STS for a specified Pad

  @param[in]  GpioPad              GPIO pad

  @retval     EFI_SUCCESS          The function completed successfully.
  @retval     Others               Status depends on GpioWrite operation.

**/
EFI_STATUS
GpioClearGpiSmiSts (
  IN UINT32                   GpioPad
  );

/**
  This procedure is used by ScSmiDispatcher and will clear
  all GPI SMI Status bits

  @retval   EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
GpioClearAllGpiSmiSts (
  VOID
  );

/**
  This procedure is used by PchSmiDispatcher and will return information
  needed to register GPI SMI. Relation between Index and GpioPad number is:
  Index = GpioGroup + 24 * GpioPad

  @param[in]  Index                   GPI SMI number
  @param[out] GpiSmiBitOffset         GPI SMI bit position within GpiSmi Registers
  @param[out] GpiSmiEnRegAddress      Address of GPI SMI Enable register
  @param[out] GpiSmiStsRegAddress     Address of GPI SMI status register

  @retval     EFI_SUCCESS             The function completed successfully
  @retval     EFI_INVALID_PARAMETER   Invalid group or pad number

**/

EFI_STATUS
GpioGetPadAndSmiRegs (
  IN UINT32            Index,
  OUT UINT8            *GpiSmiBitOffset,
  OUT UINT32           *GpiSmiEnRegAddress,
  OUT UINT32           *GpiSmiStsRegAddress
  );

/**
  This procedure will set GPIO output level

  @param[in] GpioPad               GPIO pad
  @param[in] Value                 Output value
                                   0: OutputLow, 1: OutputHigh

  @retval    EFI_SUCCESS           The function completed successfully.
  @retval    Others                Status depends on GpioPadWrite operation.

**/
EFI_STATUS
GpioSetOutputValue (
  IN UINT32                   GpioPad,
  IN UINT32                   Value
  );

/**
  This procedure will get GPIO input level

  @param[in]  GpioPad              GPIO pad
  @param[out] InputVal             GPIO Input value
                                   0: InputLow, 1: InpuHigh

  @retval     EFI_SUCCESS          The function completed successfully.
  @retval     Others               Status depends on GpioPadRead operation.

**/
EFI_STATUS
GpioGetInputValue (
  IN UINT32                    GpioPad,
  OUT UINT32                   *InputVal
  );

/**
  This procedure is used to register GPI SMI dispatch function.

  @param[in]  GpioPad       GPIO pad
  @param[out] GpiNum        GPI number

  @retval     EFI_SUCCESS   The function completed successfully.

**/
EFI_STATUS
GpioGetGpiSmiNum (
  IN  UINT32  GpioPad,
  OUT UINT64  *GpiNum
  );

/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
GpioLockPadCfg (
  IN UINT32    GpioPad
  );

/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
GpioLockPadCfgTx (
  IN UINT32    GpioPad
  );

/**
  This procedure will set Group to GPE mapping.

  @param[in]  GroupToGpeDw0           GPIO group to be mapped to GPE_DW0
  @param[in]  GroupToGpeDw1           GPIO group to be mapped to GPE_DW1
  @param[in]  GroupToGpeDw2           GPIO group to be mapped to GPE_DW2

  @retval     EFI_SUCCESS             The function completed successfully.
  @retval     EFI_INVALID_PARAMETER   Invalid group or pad number.

**/
EFI_STATUS
GpioSetGroupToGpeDwX (
  IN GPIO_GROUP                GroupToGpeDw0,
  IN GPIO_GROUP                GroupToGpeDw1,
  IN GPIO_GROUP                GroupToGpeDw2
  );

VOID
PrintGpioPadTitle(
  VOID
  );

VOID
DumpGpioPadRegisters(
  IN UINT32 CommAndOffset,
  IN UINT16 *PadName
  );

VOID
DumpGpioCommunityRegisters(
  IN UINT8 community
  );

VOID
DumpGpioPadTable(
  IN UINT32              Gpio_Pin_Size,
  IN BXT_GPIO_PAD_INIT*  Gpio_Conf_Data
  );

#endif

