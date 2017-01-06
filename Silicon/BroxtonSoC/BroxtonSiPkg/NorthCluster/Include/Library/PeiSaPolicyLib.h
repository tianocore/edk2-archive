/** @file
  Prototype of the PeiSaPolicy library.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SA_POLICY_LIB_H_
#define _PEI_SA_POLICY_LIB_H_

#include <Library/ConfigBlockLib.h>
#include <Library/BaseMemoryLib.h>
#include <SaRegs.h>



/**
  Creates the Config Blocks for SA Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] SiSaPolicyPpi          The pointer to get SI/SA Policy PPI instance.

  @retval         EFI_SUCCESS            The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
CreateConfigBlocks (
  IN OUT  SI_SA_POLICY_PPI          **SiSaPolicyPpi
  );


/**
  SaInstallPreMemPolicyPpi installs SaPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SaPreMemPolicyPpi         The pointer to SA PREMEM Policy PPI instance.

  @retval    EFI_SUCCESS               The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES      Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SaInstallPreMemPolicyPpi (
  IN  SI_SA_POLICY_PPI             *SaPolicyPpi
  );

#endif // _PEI_SA_POLICY_LIBRARY_H_

