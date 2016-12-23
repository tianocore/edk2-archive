/** @file
  Header file for the Platform Configuration Update library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_PLATFORM_CONFIG_UPDATE_LIB_H_
#define _PEI_PLATFORM_CONFIG_UPDATE_LIB_H_

#include <PiPei.h>
#include <Guid/SetupVariable.h>

/**
  Updates Setup values from PlatformInfoHob and platform policies.

  @param  PreDefaultSetupData   A pointer to the setup data prior to being
                                placed in the default data HOB.

  @retval EFI_SUCCESS           The Setup data was updated successfully.

**/
EFI_STATUS
UpdateSetupDataValues (
  SYSTEM_CONFIGURATION     *CachedSetupData
  );

#endif

