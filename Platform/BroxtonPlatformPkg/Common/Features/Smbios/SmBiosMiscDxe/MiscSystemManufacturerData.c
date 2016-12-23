/** @file
  Static data of System manufacturer information.
  System manufacturer information is Misc. subclass type 3 and SMBIOS type 1.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"

//
// Static (possibly build generated) System Manufacturer data.
//
MISC_SMBIOS_TABLE_DATA(EFI_MISC_SYSTEM_MANUFACTURER_DATA, MiscSystemManufacturer) = {
  STRING_TOKEN(STR_MISC_SYSTEM_MANUFACTURER),  // SystemManufactrurer
  STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME),  // SystemProductName
  STRING_TOKEN(STR_MISC_SYSTEM_VERSION),       // SystemVersion
  STRING_TOKEN(STR_MISC_SYSTEM_SERIAL_NUMBER), // SystemSerialNumber
  {                                            // SystemUuid
    0xa5000288, 0x6462, 0x4524, 0x98, 0x6a, 0x9b, 0x77, 0x37, 0xe3, 0x15, 0xcf
  },
  EfiSystemWakeupTypePowerSwitch  // SystemWakeupType
};

