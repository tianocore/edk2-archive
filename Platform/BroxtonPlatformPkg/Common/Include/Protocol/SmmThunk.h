/** @file
  This file defines SMM Thunk abstraction protocol.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMM_THUNK_H_
#define _SMM_THUNK_H_

#include <Protocol/LegacyBios.h>

typedef struct _EFI_SMM_THUNK_PROTOCOL EFI_SMM_THUNK_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *EFI_SMM_FARCALL86)(
  IN EFI_SMM_THUNK_PROTOCOL          *This,
  IN UINT16                          Segment,
  IN UINT16                          Offset,
  IN OUT EFI_IA32_REGISTER_SET *Regs OPTIONAL,
  IN VOID *Stack                     OPTIONAL,
  IN UINTN                           StackSize
  );

typedef
BOOLEAN
(EFIAPI *EFI_SMM_INTCALL86)(
  IN EFI_SMM_THUNK_PROTOCOL          *This,
  IN UINT16                          Segment,
  IN UINT16                          Offset,
  IN OUT EFI_IA32_REGISTER_SET *Regs OPTIONAL,
  IN VOID *Stack                     OPTIONAL,
  IN UINTN                           StackSize
  );

///
/// This protocol abstracts SMM thunk
///
struct _EFI_SMM_THUNK_PROTOCOL {
  EFI_SMM_FARCALL86 FarCall86;
  EFI_SMM_INTCALL86 IntCall86;
};

extern EFI_GUID gSmmThunkProtocolGuid;

#endif

