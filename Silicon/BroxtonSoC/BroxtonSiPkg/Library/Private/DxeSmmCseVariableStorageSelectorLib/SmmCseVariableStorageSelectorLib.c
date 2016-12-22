/** @file
  SMM CSE Variable Storage Selector Library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CseVariableStorageSelectorLibInternal.h"
#include <Library/SmmServicesTableLib.h>


EFI_STATUS
EFIAPI
CseVariableStorageSelectorSmmEndOfDxeNotification (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  EFI_STATUS  Status;

  Status = gSmst->SmmLocateProtocol (&gEfiHeciSmmProtocolGuid, NULL, (VOID **) &mHeci2Protocol);

  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "HECI2 protocol found at 0x%x.\n", mHeci2Protocol));

  return Status;
}


/**
  The library constructor.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI system table.

  @retval     EFI_SUCCESS       The function always return EFI_SUCCESS for now.
                                It will ASSERT on error for debug version.
  @retval     EFI_ERROR         Please reference LocateProtocol for error code details.

**/
EFI_STATUS
EFIAPI
SmmCseVariableStorageLibInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *SmmEndOfDxeRegistration;

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmEndOfDxeProtocolGuid,
                    CseVariableStorageSelectorSmmEndOfDxeNotification,
                    &SmmEndOfDxeRegistration
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

