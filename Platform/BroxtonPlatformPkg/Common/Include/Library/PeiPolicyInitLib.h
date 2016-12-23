/** @file
  Header file for the PeiPolicyInit Library.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POLICY_INIT_PEI_LIB_H_
#define _POLICY_INIT_PEI_LIB_H_

/**
  Initialize Pre-Mem Intel PEI Platform Policy.

**/
EFI_STATUS
EFIAPI
PeiPolicyInitPreMem (
  VOID
  );

/**
  Initialize Intel PEI Platform Policy.

  @param[in] PeiServices            General purpose services available to every PEIM.
  @param[in] SystemConfiguration    The pointer to get System Setup

**/
VOID
EFIAPI
PeiPolicyInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );
#endif

