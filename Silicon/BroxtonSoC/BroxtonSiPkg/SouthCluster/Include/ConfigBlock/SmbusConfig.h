/** @file
  Smbus policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMBUS_CONFIG_H_
#define _SMBUS_CONFIG_H_

#define SMBUS_CONFIG_REVISION 1

extern EFI_GUID gSmbusConfigGuid;

#pragma pack (push,1)

#define SC_MAX_SMBUS_RESERVED_ADDRESS 128

typedef struct {
  CONFIG_BLOCK_HEADER  Header;         ///< Config Block Header
  UINT32               Enable             :  1;
  UINT32               ArpEnable          :  1;      ///< Enable SMBus ARP support, <b>0: Disable</b>; 1: Enable.
  UINT32               DynamicPowerGating :  1;      ///< <b>(Test)</b> <b>Disable</b> or Enable Smbus dynamic power gating.
  UINT32               RsvdBits0          : 29;      ///< Reserved bits
  UINT16               SmbusIoBase;                  ///< SMBUS Base Address (IO space). Default is <b>0xEFA0</b>.
  UINT8                Rsvd0;                        ///< Reserved bytes
  UINT8                NumRsvdSmbusAddresses;        ///< The number of elements in the RsvdSmbusAddressTable.
  UINT8                RsvdSmbusAddressTable[SC_MAX_SMBUS_RESERVED_ADDRESS];
  UINT32               SpdRsvd              :  2;
  UINT32               SvRsvdbits           : 30;
} SC_SMBUS_CONFIG;

#pragma pack (pop)

#endif // _SMBUS_CONFIG_H_

