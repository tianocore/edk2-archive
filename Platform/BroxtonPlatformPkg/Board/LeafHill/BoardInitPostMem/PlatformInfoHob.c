/** @file
  This file does Multiplatform initialization.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitMiscs.h"


/**
  Returns the Platform Info of the platform from the HOB.

  @param[in] PeiServices            General purpose services available to every PEIM.
  @param[in] PlatformInfoHob        Pointer to the PLATFORM_INFO_HOB Pointer.

  @retval  EFI_SUCCESS              The function completed successfully.
  @retval  EFI_NOT_FOUND            PlatformInfoHob data doesn't exist, use default instead.

**/
EFI_STATUS
LeafHillGetPlatformInfoHob (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO_HOB     **PlatformInfoHob
  )
{
  EFI_PEI_HOB_POINTERS        GuidHob;

  //
  // Find the PlatformInfo HOB
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
    *PlatformInfoHob = GET_GUID_HOB_DATA (GuidHob.Guid);
  }

  //
  // PlatformInfo PEIM should provide this HOB data, if not ASSERT and return error.
  //
  ASSERT_EFI_ERROR (*PlatformInfoHob != NULL);
  if (!(*PlatformInfoHob)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

