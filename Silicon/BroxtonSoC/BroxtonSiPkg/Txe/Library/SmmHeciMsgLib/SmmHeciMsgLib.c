/** @file
  Implementation file for Heci Message functionality.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>

UINT8       *mNonSmmData = NULL;

/**
  Set Read Write Temp memory.

  @param[in]  Address   Temp memory.

**/
VOID
HeciSetReadWriteCache (
  VOID*  Address
  );

/**
  SmmEndOfDxeCallback

  @param[in]  Protocol
  @param[in]  Interface
  @param[in]  Handle

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciMsgLibSmmEndOfDxeNotification (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
HeciMsgLibSmmConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *SmmEndOfDxeRegistration;

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiBootServicesData,
                  EFI_SIZE_TO_PAGES (SIZE_64KB),
                  (EFI_PHYSICAL_ADDRESS *) (UINTN) &mNonSmmData
                  );
  ASSERT_EFI_ERROR (Status);

  if (mNonSmmData == NULL) {
    ASSERT (mNonSmmData != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  HeciSetReadWriteCache (mNonSmmData);

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmEndOfDxeProtocolGuid,
                    HeciMsgLibSmmEndOfDxeNotification,
                    &SmmEndOfDxeRegistration
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

