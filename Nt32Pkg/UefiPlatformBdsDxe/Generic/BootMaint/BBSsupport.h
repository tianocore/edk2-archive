/** @file

Copyright (c) 2004 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  BBSsupport.h

Abstract:

  declares interface functions

Revision History


**/

#ifndef _EFI_BDS_BBS_SUPPORT_H
#define _EFI_BDS_BBS_SUPPORT_H

//@MT:#include "Tiano.h"
//@MT:#include "EfiDriverLib.h"
//@MT:#include "EfiPrintLib.h"
//@MT:#include "BdsLib.h"
#include "BootMaint.h"


VOID
BdsBuildLegacyDevNameString (
  IN BBS_TABLE                     *CurBBSEntry,
  IN UINTN                         Index,
  IN UINTN                         BufSize,
  OUT CHAR16                       *BootString
  );

EFI_STATUS
BdsDeleteAllInvalidLegacyBootOptions (
  VOID
  );


/**
  Add the legacy boot options from BBS table if they do not exist.

  None.

  @retval EFI_SUCCESS  The boot options are added successfully or they are already
                       in boot options.
  @retval others       An error occurred when creating legacy boot options.

**/
EFI_STATUS
BdsAddNonExistingLegacyBootOptions (
  VOID
  )
;

EFI_STATUS
BdsUpdateLegacyDevOrder (
  VOID
  );

EFI_STATUS
BdsRefreshBbsTableForBoot (
  IN BDS_COMMON_OPTION        *Entry
  );

#endif
