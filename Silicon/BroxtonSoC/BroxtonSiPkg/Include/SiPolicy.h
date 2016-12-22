/** @file
  Intel reference code configuration policies.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_H_
#define _SI_POLICY_H_

/**
  Silicon Policy revision number
  Any backwards compatible changes to this structure will result in an update in the revision number

  Revision 1: Initial version
  Revision 2: Updated version
              Add OsSelection
**/
#define SI_POLICY_REVISION  2

///
/// OS Selection
///
typedef enum {
  SiWindows = 0,
  SiAndroid = 1,
  SiWin7    = 2,
  SiLinux   = 3
} SI_OS_SELECTION;

/**
  The Silicon Policy allows the platform code to publish a set of configuration
  information that the RC drivers will use to configure the silicon hardware.
**/
struct _SI_POLICY {
  /**
    This member specifies the revision of the Silicon Policy. This field is used to indicate backward
    compatible changes to the policy structure. Any such changes to this policy structure will result in an update
    in the revision number.
  **/
  UINT8  Revision;
  ///
  /// Platform specific common policies that used by several silicon components.
  ///
  ///
  /// This member determines the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS
  /// Type 14 - Group Associations structure - item type.
  /// FVI structure uses it as SMBIOS OEM type to provide version information.
  ///
  UINT8   FviSmbiosType;
  ///
  /// Reports if EC is present or not.
  ///
  BOOLEAN EcPresent;
  ///
  /// Temp Bus Number range available to be assigned to each root port and its downstream
  /// devices for initialization of these devices before PCI Bus enumeration.
  ///
  UINT8   TempPciBusMin;
  UINT8   TempPciBusMax;
  ///
  /// Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
  /// Minimum size is 2MB bytes
  ///
  UINT32  TempMemBaseAddr;
  ///
  /// Size of allocated temp MMIO space
  ///
  UINT32  TempMemSize;
  ///
  /// Temporary IO Base Address for PCI devices to be used to initialize IO registers.
  ///
  UINT16  TempIoBaseAddr;
  ///
  /// Size of temporary IO space
  ///
  UINT16 TempIoSize;
  UINT32 OsSelection : 2;
  UINT32 RsvdBits    :30; ///< Reserved bits
  UINT32 Reserved[7];
};

#endif

