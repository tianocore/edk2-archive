/** @file
  Prototype of the PeiScPolicy library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SC_POLICY_LIBRARY_H_
#define _PEI_SC_POLICY_LIBRARY_H_

#include <Ppi/ScPolicy.h>
#include <Ppi/ScPolicyPreMem.h>

/**
  Print whole SC_PREMEM_POLICY_PPI and serial out.

  @param[in]  ScPreMemPolicyPpi     The RC Policy PPI instance

**/
VOID
EFIAPI
ScPreMemPrintPolicyPpi (
  IN  SC_PREMEM_POLICY_PPI          *ScPreMemPolicyPpi
  );

/**
  Print whole SC_POLICY_PPI and serial out.

  @param[in]   ScPolicy             The SC Policy Ppi instance

**/
VOID
EFIAPI
ScPrintPolicyPpi (
  IN  SC_POLICY_PPI           *ScPolicy
  );

/**
  CreatePreMemConfigBlocks generates the config blocks of SC Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] ScPreMemPolicyPpi             The pointer to get SC PREMEM Policy PPI instance

  @retval     EFI_SUCCESS                   The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
ScCreatePreMemConfigBlocks (
  OUT  SC_PREMEM_POLICY_PPI            **ScPreMemPolicyPpi
  );

/**
  ScInstallPreMemPolicyPpi installs ScPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] ScPreMemPolicyPpi             The pointer to SC PREMEM Policy PPI instance

  @retval    EFI_SUCCESS                   The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
ScInstallPreMemPolicyPpi (
  IN  SC_PREMEM_POLICY_PPI             *ScPreMemPolicyPpi
  );

/**
  Get SC config block table total size.

  @retval   Size of SC config block table

**/
UINT32
EFIAPI
ScGetConfigBlockTotalSize (
  VOID
  );

/**
  ScCreateConfigBlocks generates the config blocks of SC Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] ScPolicyPpi                   The pointer to get SC Policy PPI instance

  @retval     EFI_SUCCESS                   The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
ScCreateConfigBlocks (
  OUT  SC_POLICY_PPI            **ScPolicyPpi
  );

/**
  ScInstallPolicyPpi installs ScPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] ScPolicyPpi                   The pointer to SC Policy PPI instance

  @retval    EFI_SUCCESS                   The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
ScInstallPolicyPpi (
  IN  SC_POLICY_PPI             *ScPolicyPpi
  );

#endif // _PEI_SC_POLICY_LIBRARY_H_

