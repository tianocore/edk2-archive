/** @file
  Header file for the PeiSiPolicyInit.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_INIT_PEI_H_
#define _SI_POLICY_INIT_PEI_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SiPolicyLib.h>
#include <Library/PeiSiPolicyUpdateLib.h>

//
// Functions
//
/**
  This function performs Silicon Policy initialzation.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
PeiSiPolicyInit (
  IN UINT8               FirmwareConfiguration
  );

#endif // _SI_POLICY_INIT_PEI_H_

