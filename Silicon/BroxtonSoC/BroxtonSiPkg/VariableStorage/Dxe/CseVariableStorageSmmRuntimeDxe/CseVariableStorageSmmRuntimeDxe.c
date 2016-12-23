/** @file
  Implements CSE Variable Storage Services and installs
  an instance of the VariableStorage Runtime DXE protocol.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CseVariableStorage.h"
#include <Library/SmmServicesTableLib.h>

/**
  Provide SMM functionality of CSE NVM variable storage services.

  @param[in]  ImageHandle  The image handle.
  @param[in]  SystemTable  The system table.

  @retval     EFI_SUCCESS  The protocol was installed successfully.
  @retval     Others       Protocol could not be installed.

**/
EFI_STATUS
EFIAPI
CseVariableStorageSmmRuntimeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle             = NULL;
  EFI_HOB_GUID_TYPE   *FdoEnabledGuidHob = NULL;

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

  if (FdoEnabledGuidHob == NULL) {
    Status = CseVariableStorageCommonInitialize ();

    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gVariableStorageProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mCseVariableStorageProtocol
                    );

  return Status;
}

