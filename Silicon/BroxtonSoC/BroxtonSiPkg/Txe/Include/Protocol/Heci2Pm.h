/** @file
  EFI HECI2 Power Management Protocol.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_HECI2_PM_PROTOCOL_H
#define _EFI_HECI2_PM_PROTOCOL_H

extern EFI_GUID gEfiHeci2PmProtocolGuid;

typedef
BOOLEAN
(EFIAPI *EFI_HECI2_PM_IS_IDLE) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_HECI2_PM_SET_ACTIVE) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_HECI2_PM_SET_IDLE) (
  VOID
  );

typedef
UINTN
(EFIAPI *EFI_HECI2_PM_GET_HECIBAR) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_HECI2_PM_SET_HECIBAR) (
  UINTN
  );

typedef
BOOLEAN
(EFIAPI *EFI_HECI2_PM_AT_RUNTIME) (
  VOID
  );

typedef struct _EFI_HECI2_PM_PROTOCOL {
  EFI_HECI2_PM_IS_IDLE     IsIdle;
  EFI_HECI2_PM_SET_ACTIVE  SetActive;
  EFI_HECI2_PM_SET_IDLE    SetIdle;
  EFI_HECI2_PM_GET_HECIBAR GetHeciBar;
  EFI_HECI2_PM_SET_HECIBAR SetHeciBar;
  EFI_HECI2_PM_AT_RUNTIME  AtRuntime;
} EFI_HECI2_PM_PROTOCOL;

#endif

