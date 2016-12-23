/** @file
  Protocol to communicate with ICH TCO.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _TCO_RESET_H_
#define _TCO_RESET_H_

#define EFI_TCO_RESET_PROTOCOL_GUID  \
  {0xa6a79162, 0xe325, 0x4c30, 0xbc, 0xc3, 0x59, 0x37, 0x30, 0x64, 0xef, 0xb3}

typedef struct _EFI_TCO_RESET_PROTOCOL  EFI_TCO_RESET_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_TCO_RESET_PROTOCOL_ENABLE_TCO_RESET) (
  IN      UINT32            *RcrbGcsSaveValue
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCO_RESET_PROTOCOL_DISABLE_TCO_RESET) (
  OUT     UINT32    RcrbGcsRestoreValue
  );

typedef struct _EFI_TCO_RESET_PROTOCOL {
  EFI_TCO_RESET_PROTOCOL_ENABLE_TCO_RESET       EnableTcoReset;
  EFI_TCO_RESET_PROTOCOL_DISABLE_TCO_RESET      DisableTcoReset;
} EFI_TCO_RESET_PROTOCOL;

extern EFI_GUID gEfiTcoResetProtocolGuid;

#endif

