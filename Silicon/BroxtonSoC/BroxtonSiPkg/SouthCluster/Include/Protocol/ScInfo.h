/** @file
  This file defines the Info Protocol.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_INFO_H_
#define _SC_INFO_H_

#define EFI_INFO_PROTOCOL_GUID \
  { \
    0xd31f0400, 0x7d16, 0x4316, 0xbf, 0x88, 0x60, 0x65, 0x88, 0x3b, 0x40, 0x2b \
  }
extern EFI_GUID                       gEfiInfoProtocolGuid;

///
/// Forward reference for ANSI C compatibility
///
typedef struct _EFI_INFO_PROTOCOL EFI_SC_INFO_PROTOCOL;

#define INFO_PROTOCOL_REVISION_1  1
#define INFO_PROTOCOL_REVISION_2  2

#define SC_RC_VERSION                0x01000000

struct _EFI_INFO_PROTOCOL {
  UINT8   Revision;
  UINT8   BusNumber;
  UINT32  RCVersion;
};

#endif

