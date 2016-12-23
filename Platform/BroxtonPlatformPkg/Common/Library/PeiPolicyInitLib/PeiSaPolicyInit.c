/** @file
  This file is SampleCode for Intel SA PEI Policy initialization.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSaPolicyInit.h"

/**
  This PEIM performs SA PEI Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the PPI.
  @retval     EFI ERRORS             The PPI is not successfully installed.

**/
EFI_STATUS
PeiSaPolicyInit (
  IN UINT8                     FirmwareConfiguration
  )
{
  EFI_STATUS               Status;
  SI_SA_POLICY_PPI         *SiSaPolicyPpi;

  //
  // Call SaCreatePolicyDefaults to initialize platform policy structure
  // and get all Intel default policy settings.
  //
  Status = CreateConfigBlocks (&SiSaPolicyPpi);
  DEBUG ((DEBUG_INFO, "SiSaPolicyPpi->TableHeader.NumberOfBlocks = 0x%x\n ", SiSaPolicyPpi->TableHeader.NumberOfBlocks));
  ASSERT_EFI_ERROR (Status);

  UpdatePeiSaPolicy (SiSaPolicyPpi);

  //
  // Install SiSaPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiSaInstallPolicyPpi (SiSaPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

