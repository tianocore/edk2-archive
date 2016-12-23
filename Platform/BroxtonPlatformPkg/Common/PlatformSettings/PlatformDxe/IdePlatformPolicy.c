/** @file
  Platform IDE init driver's policy according to setup variable.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include "Protocol/PlatformIdeInit.h"

EFI_PLATFORM_IDE_INIT_PROTOCOL mPlatformIdeInit = {
  TRUE
};


/**
  Updates the feature policies according to the setup variable.

**/
VOID
InitPlatformIdePolicy (
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  if ( mSystemConfiguration.SmartMode ) {
    mPlatformIdeInit.SmartMode = FALSE;
  }

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiPlatformIdeInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPlatformIdeInit
                  );
  ASSERT_EFI_ERROR (Status);
}

