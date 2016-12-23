/** @file
  This file is SampleCode for Intel PEI Platform Policy initialzation.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiPolicyInit.h"

/**
  Initialize Intel PEI Platform Policy

  @param[in] PeiServices            General purpose services available to every PEIM.
  @param[in] FirmwareConfiguration  It uses to skip specific policy init that depends
                                    on the 'FirmwareConfiguration' varaible.

**/
VOID
EFIAPI
PeiPolicyInit (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  )
{
  EFI_STATUS                   Status;

  DEBUG ((DEBUG_INFO, "PeiPolicyInit - Start\n"));

  //
  // CPU PEI Policy Initialization
  //
  Status = PeiCpuPolicyInit (SystemConfiguration);
  DEBUG ((DEBUG_INFO, "CPU PEI Policy Initialization Done in Post-Memory\n"));
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "PeiPolicyInit - End\n"));
}

