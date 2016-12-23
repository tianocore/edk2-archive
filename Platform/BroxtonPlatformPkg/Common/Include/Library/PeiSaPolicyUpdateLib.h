/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SA_POLICY_UPDATE_LIB_H_
#define _PEI_SA_POLICY_UPDATE_LIB_H_

/**
  This function performs SA PEI Policy initialzation.

  @param[in, out]  SiSaPolicyPpi    The SA Policy PPI instance

  @retval          EFI_SUCCESS      The PPI is installed and initialized.

**/
EFI_STATUS
EFIAPI
UpdatePeiSaPolicy (
  IN OUT   SI_SA_POLICY_PPI  *SiSaPolicyPpi
  );
#endif

