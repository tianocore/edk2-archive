/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_SA_POLICY_UPDATE_LIB_H_
#define _DXE_SA_POLICY_UPDATE_LIB_H_

/**
  Get data for platform policy from setup options.

  @param[in] SaPolicy                 The pointer to get SA Policy protocol instance
  @param[in] SystemConfiguration      The pointer to get System Setup

  @retval    EFI_SUCCESS              Operation success.

**/
EFI_STATUS
EFIAPI
UpdateDxeSaPolicy(
  IN OUT  SA_POLICY_PROTOCOL *SaPolicy,
  IN SYSTEM_CONFIGURATION    *SystemConfiguration
);

#endif

