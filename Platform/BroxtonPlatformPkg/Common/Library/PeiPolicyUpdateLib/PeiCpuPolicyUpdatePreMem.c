/** @file
  This file is SampleCode of the library for Intel CPU PEI Policy initialization.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiCpuPolicyUpdate.h>

/**
  This function performs CPU PEI Policy initialization in Pre-memory.

  @param[in] SiCpuPolicyPpi           The Cpu Policy PPI instance

  @retval    EFI_SUCCESS              The PPI is installed and initialized.
  @retval    EFI ERRORS               The PPI is not successfully installed.
  @retval    EFI_OUT_OF_RESOURCES     Do not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicyPreMem (
  IN OUT  SI_CPU_POLICY_PPI *SiCpuPolicyPpi
  )
{
  return EFI_SUCCESS;
}

