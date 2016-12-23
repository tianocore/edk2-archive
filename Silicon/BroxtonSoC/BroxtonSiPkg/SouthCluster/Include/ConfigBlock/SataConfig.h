/** @file
  SATA policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SATA_CONFIG_H_
#define _SATA_CONFIG_H_

#define SATA_CONFIG_REVISION 1

extern EFI_GUID gSataConfigGuid;

#pragma pack (push,1)

typedef enum  {
  ScSataModeAhci,
  ScSataModeRaid,
  ScSataModeMax
} SC_SATA_MODE;

typedef enum {
  ScSataSpeedDefault,
  ScSataSpeedGen1,
  ScSataSpeedGen2,
  ScSataSpeedGen3
} SC_SATA_SPEED;

typedef struct {
  UINT32  Enable           :  1;    ///< 0: Disable; <b>1: Enable</b>
  UINT32  HotPlug          :  1;    ///< <b>0: Disable</b>; 1: Enable
  UINT32  InterlockSw      :  1;    ///< <b>0: Disable</b>; 1: Enable
  UINT32  External         :  1;    ///< <b>0: Disable</b>; 1: Enable
  UINT32  SpinUp           :  1;    ///< <b>0: Disable</b>; 1: Enable the COMRESET initialization Sequence to the device
  UINT32  SolidStateDrive  :  1;    ///< <b>0: HDD</b>; 1: SSD
  UINT32  DevSlp           :  1;    ///< <b>0: Disable</b>; 1: Enable DEVSLP on the port
  UINT32  EnableDitoConfig :  1;    ///< <b>0: Disable</b>; 1: Enable DEVSLP Idle Timeout settings (DmVal, DitoVal)
  UINT32  DmVal            :  4;    ///< DITO multiplier. Default is <b>15</b>.
  UINT32  DitoVal          : 10;    ///< DEVSLP Idle Timeout (DITO), Default is <b>625</b>.
  UINT32  Rsvdbits0        : 10;    ///< Reserved fields for future expansion w/o protocol change
} SC_SATA_PORT_CONFIG;

//
// The SC_SATA_CONFIG block describes the expected configuration of the SATA controllers.
//
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                 ///< Config Block Header
  //
  // This member describes whether or not the SATA controllers should be enabled.
  //
  UINT32                Enable          :  1;
  UINT32                TestMode        :  1;   ///< <b>(Test)</b> <b>0: Disable</b>; 1: Allow entrance to the SATA test modes
  UINT32                SalpSupport     :  1;   ///< 0: Disable; <b>1: Enable</b> Aggressive Link Power Management
  UINT32                PwrOptEnable    :  1;   ///< <b>0: Disable</b>; 1: Enable SATA Power Optimizer on SC side.
  UINT32                eSATASpeedLimit    :  1;
  UINT32                Rsvdbits           : 27; ///< Reserved bits
  SC_SATA_MODE          SataMode;
  SC_SATA_SPEED         SpeedLimit;
  //
  // This member configures the features, property, and capability for each SATA port.
  //
  SC_SATA_PORT_CONFIG PortSettings[SC_MAX_SATA_PORTS];
  UINT32                Reserved;               ///< Reserved bytes
  UINT32                Reserved1[3];           ///< Reserved fields for future expansion
} SC_SATA_CONFIG;

#pragma pack (pop)

#endif // _SATA_CONFIG_H_

