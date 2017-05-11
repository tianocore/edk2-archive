/** @file
  Implementation file for the HECI2 Power Management library.

  Copyright (c) 2016-2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/Heci2Pm.h>

EFI_HECI2_PM_PROTOCOL *mHeci2PmProtocol = NULL;

/**
  Returns an instance of the HECI2 Power Management protocol.

  @params[out]  Heci2PmProtocol  The address to a pointer to the HECI2 PM protocol.

  @retval       EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetHeci2PmProtocol (
  OUT EFI_HECI2_PM_PROTOCOL **Heci2PmProtocol
  )
{
  EFI_STATUS   Status;

  if (mHeci2PmProtocol == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiHeci2PmProtocolGuid,
                      NULL,
                      (VOID **)&mHeci2PmProtocol
                      );

    if (EFI_ERROR (Status)) {
      mHeci2PmProtocol = NULL;
      return Status;
    }
  }

  *Heci2PmProtocol = mHeci2PmProtocol;

  return EFI_SUCCESS;
}

