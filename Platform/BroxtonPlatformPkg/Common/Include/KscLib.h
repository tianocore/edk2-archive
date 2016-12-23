/** @file
  KSC library functions and definitions.

  This library provides basic KSC interface.  It is deemed simple enough and uses in
  so few cases that there is not currently benefit to implementing a protocol.
  If more consumers are added, it may be benefitial to implement as a protocol.

  There may be different libraries for different environments (PEI, BS, RT, SMM).
  Make sure you meet the requirements for the library (protocol dependencies, use
  restrictions, etc).

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _KSC_LIB_H_
#define _KSC_LIB_H_

//
// Include files
//
#include <Library/BaseLib.h>

//
// Timeout if KSC command/data fails
//
#define KSC_TIME_OUT    0x20000

//
// The Keyboard and System management Controller (KSC) implements a standard 8042 keyboard
// controller interface at ports 0x60/0x64 and a ACPI compliant system management controller
// at ports 0x62/0x66. Port 0x66 is the command and status port, port 0x62 is the data port.
//
#define KSC_D_PORT        0x62
#define KSC_C_PORT        0x66

//
// Status Port 0x62
//
#define KSC_S_OVR_TMP     0x80    // Current CPU temperature exceeds the threshold
#define KSC_S_SMI_EVT     0x40    // SMI event is pending
#define KSC_S_SCI_EVT     0x20    // SCI event is pending
#define KSC_S_BURST       0x10    // KSC is in burst mode or normal mode
#define KSC_S_CMD         0x08    // Byte in data register is command/data
#define KSC_S_IGN         0x04    // Ignored
#define KSC_S_IBF         0x02    // Input buffer is full/empty
#define KSC_S_OBF         0x01    // Output buffer is full/empty

//
// KSC commands that are issued to the KSC through the command port (0x66).
// New commands and command parameters should only be written by the host when IBF=0.
// Data read from the KSC data port is valid only when OBF=1.
//
#define KSC_C_SMI_NOTIFY_ENABLE         0x04    // Enable SMI notifications to the host
#define KSC_C_SMI_NOTIFY_DISABLE        0x05    // SMI notifications are disabled and pending notifications cleared
#define KSC_C_QUERY_SYS_STATUS          0x06    // Returns 1 byte of information about the system status
#define KSC_B_SYS_STATUS_FAN            0x40    // Fan status (1 = ON)
#define KSC_B_SYS_STATUS_DOCK           0x20    // Dock status (1 = Docked)
#define KSC_B_SYS_STATUS_AC             0x10    // AC power (1 = AC powered)
#define KSC_B_SYS_STATUS_THERMAL        0x0F    // CPU thermal state (0 ~ 9)
#define KSC_C_QUERY_BOARD_ID_BIT        0x0A    // Board Id 8 or 16 bit
#define KSC_B_BOARD_ID_BIT_16           0x80    // Bit7 =1 means 16 bit implementation
#define KSC_C_FAB_ID                    0x0D    // Get the board fab ID in the lower 3 bits
#define KSC_C_SYSTEM_POWER_OFF          0x22    // Turn off the system power
#define KSC_C_LAN_ON                    0x46    // Turn on the power to LAN through EC/KSC
#define KSC_C_LAN_OFF                   0x47    // Turn off the power to LAN through EC/KSC
#define KSC_C_GET_TEMP                  0x50    // Returns the CPU temperature as read from the SMBus thermal sensor.
#define KSC_C_SET_CTEMP                 0x58    // The next byte written to the data port will be the shutdown temperature
#define KSC_EC_SMBUS_EN                 0x60    // EC SMBus thermal monitoring Enable cmd
#define KSC_EC_SMBUS_DIS                0x61    // EC SMBus thermal monitoring Disable cmd
#define KSC_TS_ON_DIMM_EN               0x6B    // TS-on-DIMM thermal monitoring enable command
#define KSC_TS_ON_DIMM_DIS              0x6C    // TS-on-DIMM thermal monitoring disable command
#define KSC_C_SMBUS_MSG_LENGTH          0x6D    // SMBus block read buffer length
#define KSC_C_SMBUS_PEC_EN              0x6E    // SMBus Packet Error Checking (PEC) Enable command.
#define KSC_C_SMBUS_PEC_DIS             0x76    // SMBus Packet Error Checking (PEC) Disable command.
#define KSC_C_EC_SMBUS_HIGH_SPEED       0x75    // EC SMBus high speed mode command
#define KSC_EC_SMBUS_WRITE_EN           0x68    // EC SMBus Write Enable cmd
#define KSC_EC_SMBUS_WRITE_DIS          0x69    // EC SMBus Write Disable cmd
#define KSC_C_SMI_QUERY                 0x70    // The host reads the data port to retrieve the notifications
#define KSC_C_SMI_TIMER                 0x71    // Commands the KSC to generate a periodic SMI to the host
#define KSC_C_SMI_HOTKEY                0x72    // Get the scan code of hotkey pressed (CTRL + ALT + SHIFT + key)
#define KSC_C_READ_MEM                  0x80    // Read the KSC memory
#define KSC_C_WRITE_MEM                 0x81    // Write the KSC memory
#define KSC_C_DOCK_STATUS               0x8A    // Get the dock status
#define KSC_B_DOCK_STATUS_ATTACH        0x01    // Dock status (1 = Attach)
#define KSC_C_KSC_REVISION              0x90    // Get the revision for the KSC
#define KSC_C_SMI_INJECT                0xBA    // The next byte written to the data port will generate an immediate SMI
#define KSC_C_SMI_DISABLE               0xBC    // SMI generation by the KSC is disabled
#define KSC_C_SMI_ENABLE                0xBD    // SMI generation by the KSC is enabled
#define KSC_C_ACPI_ENABLE               0xAA    // Enable ACPI mode
#define KSC_C_ACPI_DISABLE              0xAB    // Disable ACPI mode

//
// KSC commands that are only valid if the EC has ACPI mode enabled.
// Note that capacity and voltage are 16 bit values, thus you need to read them from
// ACPI space with two reads (little Endian).
//
#define KSC_VIRTUAL_BAT_STATUS                48      // Status of the virtual battery (present)
#define KSC_VIRTUAL_BAT_PRESENT_MASK          0x10    // Bit 4 is the indicator

#define KSC_REAL_BAT1_STATUS                  50      // Status of the first real battery (present, charging)
#define KSC_REAL_BAT1_REMAINING_CAPACITY      89      // Remaining capacity in mWh
#define KSC_REAL_BAT1_RESOLUTION_VOLTAGE      93      // Full resolution voltage in mV

#define KSC_REAL_BAT2_STATUS                  54      // Status of the second real battery (present, charging)
#define KSC_REAL_BAT2_REMAINING_CAPACITY      99      // Remaining capacity in mWh
#define KSC_REAL_BAT2_RESOLUTION_VOLTAGE      103     // Full resolution voltage in mV

#define KSC_REAL_BAT_PRESENT_MASK             0x8     // Bit 3 is the indicator
#define KSC_REAL_BAT_CHARGING_MASK            0x1     // Bit 1 is the indicator

//
// SMI notification code table, read through command KSC_C_SMI_QUERY
//
#define KSC_N_SMI_NULL                  0x00    // Null marks the end of the SMI notification queue
#define KSC_N_SMI_HOTKEY                0x20    // Hotkey pressed SMI
#define KSC_N_SMI_ACINSERTION           0x30    // AC insertion SMI
#define KSC_N_SMI_ACREMOVAL             0x31    // AC removal SMI
#define KSC_N_SMI_PWRSW                 0x32    // Power switch press SMI
#define KSC_N_SMI_LID                   0x33    // Lid switch change SMI
#define KSC_N_SMI_VB                    0x34    // Virtual battery switch change SMI
#define KSC_N_SMI_THERM_0               0x60    // Thermal state 0 SMI
#define KSC_N_SMI_THERM_1               0x61    // Thermal state 1 SMI
#define KSC_N_SMI_THERM_2               0x62    // Thermal state 2 SMI
#define KSC_N_SMI_THERM_3               0x63    // Thermal state 3 SMI
#define KSC_N_SMI_THERM_4               0x64    // Thermal state 4 SMI
#define KSC_N_SMI_THERM_5               0x65    // Thermal state 5 SMI
#define KSC_N_SMI_THERM_6               0x66    // Thermal state 6 SMI
#define KSC_N_SMI_THERM_7               0x67    // Thermal state 7 SMI
#define KSC_N_SMI_THERM_8               0x68    // Thermal state 8 SMI
#define KSC_N_SMI_DOCKED                0x70    // Dock complete SMI
#define KSC_N_SMI_UNDOCKED              0x71    // Undock complete SMI
#define KSC_N_SMI_UNDOCKREQUEST         0x72    // Undocking request SMI
#define KSC_N_SMI_TIMER                 0x80    // Timer wakeup SMI

//
// Hotkey scan code (CTRL + ALT + SHIFT + key)
//
#define KSC_HK_ESC      0x01          // ESC
#define KSC_HK_1        0x02          // 1 !
#define KSC_HK_2        0x03          // 2 @
#define KSC_HK_3        0x04          // 3 #
#define KSC_HK_4        0x05          // 4 $
#define KSC_HK_5        0x06          // 5 %
#define KSC_HK_6        0x07          // 6 ^
#define KSC_HK_7        0x08          // 7 &
#define KSC_HK_8        0x09          // 8 *
#define KSC_HK_9        0x0A          // 9 (
#define KSC_HK_0        0x0B          // 0 )
#define KSC_HK_MINUS    0x0C          // - _
#define KSC_HK_ADD      0x0D          // = +
#define KSC_HK_F1       0x3B          // F1
#define KSC_HK_F2       0x3C          // F2
#define KSC_HK_F3       0x3D          // F3
#define KSC_HK_F4       0x3E          // F4
#define KSC_HK_F5       0x3F          // F5
#define KSC_HK_F6       0x40          // F6
#define KSC_HK_F7       0x41          // F7
#define KSC_HK_F8       0x42          // F8
#define KSC_HK_F9       0x43          // F9
#define KSC_HK_F10      0x44          // F10
#define KSC_HK_F11      0x57          // F11
#define KSC_HK_F12      0x58          // F12

//
// Function declarations
//
/**
  This function initializes the KSC library.
  It must be called before using any of the other KSC library functions.

  @param   None.

  @retval  EFI_SUCCESS      KscLib is successfully initialized.

**/
EFI_STATUS
InitializeKscLib (
  VOID
  );

/**
  Send a command to the Keyboard System Controller.

  @param  Command    Command byte to send

  @retval  EFI_SUCCESS         Command success
  @retval  EFI_TIMEOUT         Command timeout
  @retval  Other               Command failed

**/
EFI_STATUS
SendKscCommand (
  UINT8   Command
  );

/**
  Sends data to Keyboard System Controller.

  @param   Data               Data byte to send

  @retval EFI_SUCCESS         Success
  @retval EFI_TIMEOUT         Timeout
  @retval Other               Failed

**/
EFI_STATUS
SendKscData (
  UINT8   Data
  );

/**
  Receives data from Keyboard System Controller.

  @param   Data                Data byte received

  @retval  EFI_SUCCESS         Read success
  @retval  EFI_TIMEOUT         Read timeout
  @retval  Other               Read failed

**/
EFI_STATUS
ReceiveKscData (
  UINT8   *Data
  );

/**
  Receives status from Keyboard System Controller.

  @param   Status              Status byte to receive

  @retval  EFI_SUCCESS         Success
  @retval  Other               Failed

**/
EFI_STATUS
ReceiveKscStatus (
  UINT8   *KscStatus
  );

#endif

