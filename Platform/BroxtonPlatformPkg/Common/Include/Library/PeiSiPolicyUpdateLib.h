/** @file
  Header file for PEI SiPolicyUpdate Library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SI_POLICY_UPDATE_LIB_H_
#define _PEI_SI_POLICY_UPDATE_LIB_H_

/**
  This function performs Silicon PEI Policy initialzation.

  @param[in, out] SiPolicy        The Silicon Policy PPI instance

  @retval    EFI_SUCCESS          The function completed successfully

**/
EFI_STATUS
EFIAPI
UpdatePeiSiPolicy (
  IN OUT SI_POLICY_PPI *SiPolicy
  );
#endif

