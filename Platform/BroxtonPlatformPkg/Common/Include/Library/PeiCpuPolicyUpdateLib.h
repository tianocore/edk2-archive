/** @file
  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_CPU_POLICY_UPDATE_LIB_H_
#define _PEI_CPU_POLICY_UPDATE_LIB_H_

/**
  This function performs CPU PEI Policy initialzation in PreMem.

  @param[in] SiCpuPolicyPpi          The Cpu Policy PPI instance

  @retval    EFI_SUCCESS             The PPI is installed and initialized.
  @retval    EFI ERRORS              The PPI is not successfully installed.
  @retval    EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicyPreMem (
  IN OUT  SI_CPU_POLICY_PPI *SiCpuPolicyPpi
  );

/**
  This function performs CPU PEI Policy initialzation in Post-memory.

  @param[in] SiCpuPolicyPpi          The Cpu Policy PPI instance
  @param[in] SystemConfiguration     The pointer to get System Setup

  @retval    EFI_SUCCESS             The PPI is installed and initialized.
  @retval    EFI ERRORS              The PPI is not successfully installed.
  @retval    EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicy (
  IN OUT  SI_CPU_POLICY_PPI  *SiCpuPolicyPpi,
  IN SYSTEM_CONFIGURATION    *SystemConfiguration
  );
#endif

