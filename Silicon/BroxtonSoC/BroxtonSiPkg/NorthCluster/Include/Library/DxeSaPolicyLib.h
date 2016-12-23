/** @file
  GPrototype of the DxeSaPolicyLib library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_SA_POLICY_LIB_H_
#define _DXE_SA_POLICY_LIB_H_

#include <Protocol/SaPolicy.h>
#include <Library/ConfigBlockLib.h>

/**
  Creates the default setting of SA Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] DxeSaPolicy           The pointer to get SA Policy protocol instance.

  @retval         EFI_SUCCESS           The policy default is initialized.
  @retval         Others                Internal error when create default SA policy.

**/
EFI_STATUS
EFIAPI
SaCreatePolicyDefaults (
  IN OUT  SA_POLICY_PROTOCOL      *DxeSaPolicy
  );

/**
  Creates the Config Blocks for SA DXE Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] SiSaPolicyPpi          The pointer to get SI/SA Policy PPI instance.

  @retval         EFI_SUCCESS            The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
CreateSaDxeConfigBlocks(
  IN OUT  SA_POLICY_PROTOCOL       **SaPolicy
  );

/**
  Install protocol for SA Policy.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] DxeSaPolicy                The pointer to SA Policy Protocol instance.

  @retval    EFI_SUCCESS                The policy is installed.
  @retval    Others                     Internal error when install protocol.

**/
EFI_STATUS
EFIAPI
SaInstallPolicyProtocol (
  IN  SA_POLICY_PROTOCOL *DxeSaPolicy
  );

/**
  This function prints the SA DXE phase policy.

  @param[in]  DxeSaPolicy              The pointer to SA Policy Protocol instance.

**/
VOID
SaPrintPolicyProtocol (
  IN  SA_POLICY_PROTOCOL      *SaPolicy
 );

#endif // _DXE_SA_POLICY_LIB_H_

