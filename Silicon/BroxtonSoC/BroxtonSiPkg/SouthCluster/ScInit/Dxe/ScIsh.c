/** @file
  Initializes SC ISH Devices.

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
#include <ScRegs/RegsIsh.h>
#include "SaAccess.h"

/**
  Hide PCI config space of ISH devices and do any final initialization.

  @param[in] ScPolicy                The SC Policy instance

  @retval    EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
ConfigureIshAtBoot (
  IN SC_POLICY_HOB                *ScPolicy
  )
{
  UINTN                           IshPciMmBase;
  UINT32                          IshMmioBase0;
  UINT32                          IshMmioBase1;
  UINT32                          Buffer32;

  DEBUG ((DEBUG_INFO, "ConfigureIshAtBoot() Start\n"));

  //
  // Initialize Variables
  //
  IshPciMmBase     = 0;
  IshMmioBase0     = 0;
  IshMmioBase1     = 0;
  Buffer32         = 0;

  DEBUG ((DEBUG_INFO, "ConfigureIshAtBoot() End\n"));

  return EFI_SUCCESS;
}

