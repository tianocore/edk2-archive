/** @file
  Header file of PMIC hardware definition.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PMIC_LIB_H_
#define _PMIC_LIB_H_

typedef enum {
  REG_OVERRIDE    = 0,
  REG_AND         = 1,
  REG_OR          = 2,
  REG_CLEAR       = 3
} RegAction;

typedef struct RegInit {
  UINT8     baseAddress;
  UINT8     regoffset;
  RegAction action;
  UINT8     mask;
  UINT8     value;
} RegInit_st;

typedef enum {
  DIR_INPUT  = 0,
  DIR_OUTPUT = 1,
} GpioAttrib;

typedef struct Gpio {
  UINT8       *PinName;
  UINT8       OutputReg;
  UINT8       InputReg;
  GpioAttrib  Direction;
  UINT8       IsIntr;       //is it an interrupt
  UINT8       GPOCtrlVal;
  UINT8       GPICtrlVal;
} GpioCfg_st;

typedef enum {
  PMIC_TYPE_NONE       = 0,
  PMIC_TYPE_CC         = 1,
  PMIC_TYPE_CC_ROHM    = 2,
  PMIC_TYPE_CC_DIALOG  = 3,
  PMIC_TYPE_DC         = 4,
  PMIC_TYPE_DC_XPOWERS = 5,
  PMIC_TYPE_DC_TI      = 6,
  PMIC_TYPE_PC         = 7,
  PMIC_TYPE_PC_XPOWERS = 8,
  PMIC_TYPE_WC         = 9,
  PMIC_TYPE_DUMMY      = 0xFE,
} PMIC_TYPE;


#define PMIC_I2C_BUSNO        4     // I2C4 is used. index from 0

//
// Separated registers into two pages: page 0 (accessible through I2C bus address 0x5E) and page 1 (accessible through I2C bus address 0x6E).
// Page 0 is for OTP. Assigned addresses to registers on page 1.
//
#define PMIC_PAGE_0_I2C_ADDR  0x5E
#define PMIC_PAGE_1_I2C_ADDR  0x6E


#define INTERRUPT_EN             1
#define INTERRUPT_DIS            0

#define PMIC_DEV_ROHM            0x1F
#define PMIC_DEV_DIALOG          0x00
#define PMIC_DEV_DIALOG_1        0x2B
#define PMIC_DEV_MOUNTVILLE      0x01
#define PMIC_DEV_PENNYCOVE       0x00
#define PMIC_DEV_DC_XPOWERS      0x41

#define PMIC_REVID_B0            0xB0

#define PMIC_ROHM_REVID_A1       0xA1
#define PMIC_ROHM_REVID_A2       0xA2
#define PMIC_ROHM_REVID_B0       0xB0
#define PMIC_ROHM_REVID_B1       0xB1
#define PMIC_ROHM_REVID_B3       0xB3

#define PMIC_DIALOG_REVID_A0     0x0A
#define PMIC_DIALOG_REVID_B0     0xB0
#define PMIC_DIALOG_REVID_C0     0xC0
#define CHG_BQ24261_STS_CTRL_REG0         0x00
#define CHG_BQ24261_STS_CTRL_BOOOST       BIT6
#define CHG_BQ24261_CTRL_REG1             0x01
#define CHG_BQ24261_IN_LIMIT_MASK         (BIT6|BIT5|BIT4)
#define CHG_BQ24261_VENDOR_ID     0x03
#define CHG_VENDOR_ID             0x46
#define CHG_BQ25892_WDT           0x07
#define CHG_BQ25892_CTRL          0x03

/**
  Reads an 8-bit PMIC register.

  Reads the 8-bit PMIC register specified by Register.
  The 8-bit read value is returned.

  @param[in]  BaseAddress       IPC operation address for target PMIC device.
  @param[in]  Register          The PMIC register to read.

  @retval     UINT8             The value read.

**/
UINT8
EFIAPI
PmicRead8 (
  IN UINT8                     BaseAddress,
  IN UINT8                     Register
  );

/**
  Writes an 8-bit PMIC register with a 8-bit value.

  Writes the 8-bit PMIC register specified by Register with the value specified
  by Value and return the operation status.

  @param[in]  BaseAddress       IPC operation address for target PMIC device.
  @param[in]  Register          The PMIC register to write.
  @param[in]  Value             The value to write to the PMIC register.

  @retval     EFI_SUCCESS       Write bytes to PMIC device successfully
  @retval     Others            Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicWrite8 (
  IN UINT8                     BaseAddress,
  IN UINT8                     Register,
  IN UINT8                     Value
  );

/**
  Initialize PMIC thermal detection capability.

  @retval EFI_SUCCESS          Initialize thermal detection successfully.
  @retval Others                Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicThermInit (
  VOID
  );

/**
  Initialize PMIC GPIO pin.

  Initialize PMIC GPIO pin in order to get boardid/memcfgid/.. etc later

  @param[in] PlatformInfo        Platform information with GPIO setting.

  @retval    EFI_SUCCESS         Initialize GPIO pin successfully.
  @retval    Others              Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicGpioInit (
  IN VOID    *PlatformInfo
  );

/**
  Initializes PMIC device.

  @retval EFI_SUCCESS           Initialize PMIC successfully.
  @retval Others                Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicIntrInit (
  VOID
  );

/**
  Initializes Burst Control Unit (BCU) hardware.

  The BCU has several different knobs (input stimuli configuration, interrupts,
  and output actions) that allow the Software and Firmware to change the BCU response
  behavior once these events occur. These BCU control knobs allow for a change in the
  BCU behavior in responding to these triggers and can be completely customized as
  necessary by the specific system implementation.

  @retval EFI_SUCCESS            Initializes BCU hardware successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicBcuInit (
  VOID
  );

/**
  Initializes other miscellaneous functions on PMIC device.

  @retval EFI_SUCCESS           Initializes miscellaneous functions successfully.
  @retval Others                Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicMiscInit (
  VOID
  );

/**
  Turn on or off VBUS for OTG

  @param[in]  Enable             TRUE : turn on VBUS
                                 FALSE: turn off VBUS

  @retval EFI_SUCCESS            Turn on/off VBUS successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicVbusControl (
  IN BOOLEAN    Enable
  );

/**
  Turn on or off 5V VBUS for USB2/3 HOST

  @param[in]  Enable             TRUE : turn on V5P0S
                                 FALSE: turn off V5P0S

  @retval EFI_SUCCESS            Turn on/off V5P0S successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicVhostControl (
  IN BOOLEAN    Enable
  );

/**
  Get PMIC Vendor ID and Device ID

  @param[in, out] VenId          Vendor ID
  @param[in, out] DevId          Device ID

  @retval         EFI_SUCCESS    Get Vendor ID and Device ID successfully.
  @retval         Others         Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicGetDevID (
  IN OUT UINT8    *VenId,
  IN OUT UINT8    *DevId
  );

/**
  Get PMIC Stepping ID

  @retval PMIC STEPPING.

**/
UINT8
EFIAPI
PmicStepping(VOID);

/**
  Get PMIC device instance

  @retval PMIC_TYPE             Device type is defined in PMIC_TYPE.

**/
PMIC_TYPE
EFIAPI
PmicGetDeviceType (
  VOID
  );

/**
  Get battery voltage.

  @retval UINT16                ADC result for battery voltage.

**/
UINT16
EFIAPI
PmicGetVBAT (
  VOID
  );

/**
  Get battery capacity.

  @retval UINT16               Remaining percentage of battery capacity.

**/
UINT16
EFIAPI
PmicGetBatteryCap (
  VOID
  );

/**
  AC/DC Adapter Detection Status

  @retval TRUE                  Connected
  @retval FALSE                 Disconnected

**/
BOOLEAN
EFIAPI
PmicIsACOn (
  VOID
  );

/**
  Get power button status

  @retval TRUE                  Power button pressed.
  @retval FALSE                 Power button released.

**/
BOOLEAN
EFIAPI
PmicIsPwrBtnPressed (
  VOID
  );

/**
  Set VDDQ to 1.35V for DDR3L

  @retval EFI_SUCCESS           Succeed to Set VDDQ
  @retval Others                Filed to Set VDDQ

**/
EFI_STATUS
EFIAPI
PmicSetVDDQ (
  VOID
  );

/**
  Probe to find the correct PMIC object.

  After probling, g_pmic_obj points to correct PMIC object
  This routine is invoked when library is loaded .

  @retval TRUE                  There is one PMIC object found.
  @retval FALSE                 No PMIC object found.

**/
BOOLEAN
EFIAPI
PmicProbe (
  VOID
  );

/**
  Get previous shutdown root cause.

  @param[in, out]  ShutdownCause   The data to store shutdown root cause.

  @retval EFI_SUCCESS              Get shutdown root cause successfully.
  @retval EFI_INVALID_PARAMETER    ShutdownCause is NULL.

**/
EFI_STATUS
EFIAPI
PmicGetShutdownCause (
  IN OUT UINT32    *ShutdownCause
  );

/**
  Get previous reset root cause

  @param[in, out]  ResetSrc       The data to store reset root cause

  @retval EFI_SUCCESS             Get reset root cause successfully.
  @retval EFI_INVALID_PARAMETER   ResetSrc is NULL.

**/
EFI_STATUS
EFIAPI
PmicGetResetSrc (
  IN OUT    UINT32 *ResetSrc
  );

/**
  Get the cause of system wake event.

  @param[in, out]  WakeCause     The data to store the cause of wake event.

  @retval EFI_SUCCESS            Get wake cause successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicGetWakeCause (
  IN OUT UINT32    *WakeCause
  );

/**
  Get power source detection result.

  @param[in, out]  PowerSrcIrq    The data to the cause of wake event.

  @retval EFI_SUCCESS             Get power source successfully.
  @retval Others                  Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicGetPwrSrcIrq (
  IN OUT UINT32    *PowerSrcIrq
  );

/**
  Battery Detection Status

  @retval TRUE                  Connected
  @retval FALSE                 Disconnected

**/
BOOLEAN
EFIAPI
PmicIsBatOn (
  VOID
  );

/**
  VBUS Detection Status

  It can be used to detect whether USB charger is connected.

  @retval TRUE                 Connected
  @retval FALSE                Disconnected

**/
BOOLEAN
EFIAPI
PmicVbusStatus (
  VOID
  );

/**
  Clear previous shutdown root cause.

  @retval EFI_SUCCESS            Clear shutdown root cause successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicClearShutdownCause (
  VOID
  );

/**
  Clear the cause of system wake event.

  @retval EFI_SUCCESS            Clear wake cause successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicClearWakeCause (
  VOID
  );

/**
  Clear the cause of system reset.

  @retval EFI_SUCCESS           Clear reset cause successfully.
  @retval Others                Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicClearResetSrc (
  VOID
  );

/**
  Disable the capability to shutdown platform using power button.

  @param[out] ButtonHoldTime     If the power button is disabled successfully,
                                 this contains the time in seconds need to hold power button to shutdown platform.

  @retval     EFI_SUCCESS        Succeed to disable power button
  @retval     Others             Failed to disable power button

**/
EFI_STATUS
EFIAPI
PmicDisablePowerButton (
  OUT UINT8    *ButtonHoldTime
  );

/**
  Enable the capability to shutdown platform using power button.

  @param[in] ButtonHoldTime     Time in seconds to shut down the platform if power button is enabled and hold

  @retval    EFI_SUCCESS        Succeed to enable power button
  @retval    Others             Filed to enable power button

**/
EFI_STATUS
EFIAPI
PmicEnablePowerButton (
  IN UINT8    ButtonHoldTime
  );

/**
  Read charger's register provided by PMIC.

  @param[in]  Offset            The charger's register to read.
  @param[out] Value             The value read.

  @retval     EFI_SUCCESS       Read charger's register successfully.
  @retval     Others            Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicChargerRead (
  IN  UINT8     Offset,
  OUT UINT8     *Value
  );

/**
  Write charger's register provided by PMIC.

  @param[in] Offset             The charger's register to write.
  @param[in] Value              The value written.

  @retval    EFI_SUCCESS        Write charger's register successfully.
  @retval    Others             Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicChargerWrite (
  IN UINT8    Offset,
  IN UINT8    Value
  );

EFI_STATUS
EFIAPI
WcPmicChargerRead (
  IN UINT8     Offset,
  OUT UINT8    *Value
  );

EFI_STATUS
EFIAPI
WcPmicChargerWrite (
  IN UINT8    Offset,
  IN UINT8    Value
  );

/**
  Detect charger type and charger presence.

  @param[out] ChargerPresent     TRUE : Charger present.
                                 FALSE: Charger not present.
  @param[out] ChargerType        Charger type - SDP/DCP/CDP... etc.

  @retval EFI_SUCCESS            Detect charger type and charger presence successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicDetectCharger (
  OUT BOOLEAN    *ChargerPresent,
  OUT UINT8      *ChargerType
  );

/**
  Controls external USB PHY reset

  @param[in] Enable              TRUE : OUTOFRESET - external PHY is out of reset
                                 FALSE: INRESET - external PHY is in reset

  @retval EFI_SUCCESS            Controls external USB PHY successfully.
  @retval Others                 Status depends on each PMIC operation.

**/
EFI_STATUS
EFIAPI
PmicUSBSwitchControl (
  IN BOOLEAN    Enable
  );

/**
  USB ID Detection Status

  @retval UINT8                 Value depends on each PMIC operation.

**/
UINT8
EFIAPI
PmicUSBIDStatus (
  VOID
  );

#endif

