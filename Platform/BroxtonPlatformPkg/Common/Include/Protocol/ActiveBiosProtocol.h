/** @file
  This protocol is used to report and control what BIOS is mapped to the
  BIOS address space anchored at 4GB boundary.

  This protocol is EFI compatible.

  E.G. For current generation ICH, the 4GB-16MB to 4GB range can be mapped
  to PCI, SPI, or FWH.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_ACTIVE_BIOS_PROTOCOL_H_
#define _EFI_ACTIVE_BIOS_PROTOCOL_H_

//
// Define the  protocol GUID
//
#define EFI_ACTIVE_BIOS_PROTOCOL_GUID  \
  { 0xebbe2d1b, 0x1647, 0x4bda, {0xab, 0x9a, 0x78, 0x63, 0xe3, 0x96, 0xd4, 0x1a} }

typedef struct _EFI_ACTIVE_BIOS_PROTOCOL EFI_ACTIVE_BIOS_PROTOCOL;

//
// Protocol definitions
//
typedef enum {
  ActiveBiosStateSpi,
  ActiveBiosStatePci,
  ActiveBiosStateLpc,
  ActiveBiosStateMax
} EFI_ACTIVE_BIOS_STATE;

typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE) (
  IN EFI_ACTIVE_BIOS_PROTOCOL     *This,
  IN EFI_ACTIVE_BIOS_STATE        DesiredState,
  IN UINTN                        Key
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE) (
  IN     EFI_ACTIVE_BIOS_PROTOCOL   *This,
  IN     BOOLEAN                    Lock,
  IN OUT UINTN                      *Key
  );

//
// Protocol definition
//
// Note that some functions are optional.  This means that they may be NULL.
// Caller is required to verify that an optional function is defined by checking
// that the value is not NULL.
//
struct _EFI_ACTIVE_BIOS_PROTOCOL {
  EFI_ACTIVE_BIOS_STATE                       State;
  EFI_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE       SetState;
  EFI_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE      LockState;
};

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiActiveBiosProtocolGuid;

#endif

