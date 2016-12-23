/** @file
  SC Extended Reset Protocol.

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_EXTENDED_RESET_H_
#define _EFI_EXTENDED_RESET_H_

#define EFI_EXTENDED_RESET_PROTOCOL_GUID \
  { \
    0xf0bbfca0, 0x684e, 0x48b3, 0xba, 0xe2, 0x6c, 0x84, 0xb8, 0x9e, 0x53, 0x39 \
  }
extern EFI_GUID                                 gEfiExtendedResetProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_EXTENDED_RESET_PROTOCOL EFI_SC_EXTENDED_RESET_PROTOCOL;

//
// Related Definitions
//
//
// SC Extended Reset Types
//
typedef struct {
  UINT8  PowerCycle : 1;
  UINT8  GlobalReset : 1;
  UINT8  SusPwrDnAck : 1;

  UINT8  RsvdBits : 5;
} SC_EXTENDED_RESET_TYPES;

typedef
EFI_STATUS
(EFIAPI *EFI_SC_EXTENDED_RESET) (
  IN     EFI_SC_EXTENDED_RESET_PROTOCOL   * This,
  IN     SC_EXTENDED_RESET_TYPES          ScExtendedResetTypes
  );

struct _EFI_EXTENDED_RESET_PROTOCOL {
  EFI_SC_EXTENDED_RESET  Reset;
};

#endif

