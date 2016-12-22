/** @file
  Prototype of the CpuPolicy library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_POLICY_LIB_H_
#define _CPU_POLICY_LIB_H_

#include <Ppi/CpuPolicy.h>
#include <Library/ConfigBlockLib.h>
#include <ConfigBlock.h>

/**
  Print whole CPU_POLICY_PPI and serial out.

  @param[in]  SiCpuPolicyPpi             The RC Policy PPI instance

**/
VOID
CpuPrintPolicyPpi (
  IN  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi
  );

/**
  CreateCpuConfigBlocks creates the Config Blocks for CPU Policy.

  @param[in, out] SiCpuPolicyPpi                The pointer to get CPU Policy PPI instance

  @retval         EFI_SUCCESS                   The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
CreateCpuConfigBlocks (
  IN OUT  SI_CPU_POLICY_PPI          **SiCpuPolicyPpi
  );

/**
  CpuInstallPolicyPpi installs SiCpuPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiCpuPolicyPpi                The pointer to PEI Cpu Policy PPI instance

  @retval    EFI_SUCCESS                   The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
CpuInstallPolicyPpi (
  IN  SI_CPU_POLICY_PPI                 *SiCpuPolicyPpi
  );

#endif // _PEI_CPU_POLICY_LIB_H_

