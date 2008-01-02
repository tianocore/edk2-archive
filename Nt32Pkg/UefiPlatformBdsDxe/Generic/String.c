/** @file

Copyright (c) 2004 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  String.c

Abstract:

  String support


**/

#include "Bds.h"
#include "Language.h"
#include "FrontPage.h"

EFI_HII_HANDLE gStringPackHandle;

EFI_GUID mBdsStringPackGuid = {
  0x7bac95d3, 0xddf, 0x42f3, 0x9e, 0x24, 0x7c, 0x64, 0x49, 0x40, 0x37, 0x9a
};


/**
  Initialize HII global accessor for string support

  None

  @retval EFI_SUCCESS  String support initialize success.

**/
EFI_STATUS
InitializeStringSupport (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   DriverHandle;
  EFI_HII_PACKAGE_LIST_HEADER  *PackageList;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &gHiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Create driver handle used by HII database
  //
  Status = HiiLibCreateHiiDriverHandle (&DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PackageList = PreparePackageList (1, &mBdsStringPackGuid, &BdsStrings);
  ASSERT (PackageList != NULL);

  Status = gHiiDatabase->NewPackageList (
                           gHiiDatabase,
                           PackageList,
                           DriverHandle,
                           &gStringPackHandle
                           );

  gBS->FreePool (PackageList);
  return Status;
}


/**
  Get string by string id from HII Interface

  @param  Id           String ID.

  @retval *            String from ID.
  @retval NULL         If error occurs.

**/
CHAR16 *
GetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  CHAR16 *String;

  String = NULL;
  GetStringFromHandle (gStringPackHandle, Id, &String);

  return String;
}
