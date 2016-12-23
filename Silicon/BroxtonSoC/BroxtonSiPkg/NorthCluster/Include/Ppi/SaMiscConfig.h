/** @file
  Policy details for miscellaneous configuration in System Agent.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_MISC_CONFIG_H_
#define _SA_MISC_CONFIG_H_

#pragma pack(1)

#define SA_MISC_CONFIG_REVISION 1

extern EFI_GUID gSaMiscConfigGuid;

#ifndef MAX_SOCKETS
#define MAX_SOCKETS 4
#endif

typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-23 Config Block Header
  UINT8                SpdAddressTable[MAX_SOCKETS];
  UINT8                UserBd;
  UINT8                PlatformType;
  UINT8                FastBoot;
  UINT8                DynSR;
} SA_MISC_CONFIG;

#pragma pack()

#endif // _SA_MISC_CONFIG_H_

