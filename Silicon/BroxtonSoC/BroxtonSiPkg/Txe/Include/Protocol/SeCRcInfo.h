/** @file
  This file defines the SEC RC Info Protocol.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_RC_INFO_H_
#define _SEC_RC_INFO_H_

//
// Define SEC RC INFO protocol GUID
//
#define EFI_SEC_RC_INFO_PROTOCOL_GUID \
  { \
    0x11fbfdfb, 0x10d2, 0x43e6, 0xb5, 0xb1, 0xb4, 0x38, 0x6e, 0xdc, 0xcb, 0x9a \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gEfiSeCRcInfoProtocolGuid;

//
// Revision 1:  Original version
//
#define SEC_RC_INFO_PROTOCOL_REVISION_1  1
#define SEC_RC_VERSION                   0x00050000

typedef union _RC_VERSION {
  UINT32  Data;
  struct {
    UINT32  RcBuildNo : 8;
    UINT32  RcRevision : 8;
    UINT32  RcMinorVersion : 8;
    UINT32  RcMajorVersion : 8;
  } Fields;
} RC_VERSION;

///
/// This interface defines the SEC RC Info Protocol.
///
typedef struct _EFI_SEC_RC_INFO_PROTOCOL {
  UINT8       Revision;
  RC_VERSION  RCVersion;
} EFI_SEC_RC_INFO_PROTOCOL;
#endif

