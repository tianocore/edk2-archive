/** @file
  This file is SampleCode for Intel CPU PEI Policy initialzation.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiCpuPolicyInit.h"

/**
  This function performs CPU PEI Policy initialization in Pre-Memory.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).

**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInitPreMem (
  )
{
  EFI_STATUS                        Status;
  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi;

  //
  // Call CpuCreatePolicyDefaults to initialize platform policy structure
  // and get all Intel default policy settings.
  //
  Status = CreateCpuConfigBlocks (&SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Update Pre-Mem Policy related policies.
  //
  UpdatePeiCpuPolicyPreMem (SiCpuPolicyPpi);

  //
  // Install SiCpuPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = CpuInstallPolicyPpi (SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This function performs CPU PEI Policy initialization in Post-Memory.

  @param[in]  SystemConfiguration   The pointer to get System Setup

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).

**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  )
{
  EFI_STATUS                        Status;
  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi;

  //
  // Call CreateCpuConfigBlocks to initialize platform policy structure
  // and get all Intel default policy settings.
  //
  Status = CreateCpuConfigBlocks (&SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  UpdatePeiCpuPolicy (SiCpuPolicyPpi, SystemConfiguration);

  //
  // Install SiCpuPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = CpuInstallPolicyPpi (SiCpuPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

