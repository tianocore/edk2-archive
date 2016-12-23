/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include <Protocol/SaPolicy.h>
#include <Protocol/GlobalNvsArea.h>
#include <Library/DxeSaPolicyLib.h>
#include <Library/DxeSaPolicyUpdateLib.h>
#include <Library/ConfigBlockLib.h>

VOID
InitPlatformSaPolicy (
  IN SYSTEM_CONFIGURATION         *SystemConfiguration
  )
{
  EFI_STATUS                      Status;
  SA_POLICY_PROTOCOL              *SaPolicy;

  //
  // Call CreateSaDxeConfigBlocks to initialize SA DXE policy structure
  // and get all Intel default policy settings.
  //
  Status = CreateSaDxeConfigBlocks (&SaPolicy);
  DEBUG ((DEBUG_INFO, "SaPolicy->TableHeader.NumberOfBlocks = 0x%x\n ", SaPolicy->TableHeader.NumberOfBlocks));
  ASSERT_EFI_ERROR (Status);

  UpdateDxeSaPolicy (SaPolicy, SystemConfiguration);

  Status = SaInstallPolicyProtocol (SaPolicy);
  ASSERT_EFI_ERROR (Status);
}

