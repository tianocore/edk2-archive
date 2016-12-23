/** @file
  Initializes SC IOSF Devices

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInit.h"
#include <Protocol/GlobalNvsArea.h>
#include "SaAccess.h"

/**
  Hide PCI config space of Iosf devices and do any final initialization.

  @param[in] ScPolicy               The SC Policy instance

  @retval    EFI_SUCCESS            The function completed successfully

**/
EFI_STATUS
ConfigureIosfAtBoot (
  IN SC_POLICY_HOB              *ScPolicy
  )
{
  return EFI_SUCCESS;
}

