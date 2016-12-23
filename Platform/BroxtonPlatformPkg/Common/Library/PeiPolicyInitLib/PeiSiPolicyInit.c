/** @file
  This file is SampleCode for Intel Silicon PEI Policy initialzation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSiPolicyInit.h"

/**
  This function performs Silicon Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
PeiSiPolicyInit (
  IN UINT8               FirmwareConfiguration
  )
{
  EFI_STATUS             Status;
  SI_POLICY_PPI          *SiPolicyPpi;

  //
  // Call SiCreatePolicyDefaults to initialize Silicon Policy structure
  // and get all Intel default policy settings.
  //
  Status = SiCreatePolicyDefaults (&SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Update and override all platform related and customized settings below.
  //
  UpdatePeiSiPolicy (SiPolicyPpi);

  //
  // Install SiPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiInstallPolicyPpi (SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

