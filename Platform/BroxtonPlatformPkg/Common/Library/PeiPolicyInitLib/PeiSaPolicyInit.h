/** @file
  Header file for the SaPolicyInitPei PEIM.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_POLICY_INIT_PEI_H_
#define _SA_POLICY_INIT_PEI_H_

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiSaPolicyLib.h>
#include <Ppi/SaPolicy.h>
#include <Library/PeiSaPolicyUpdateLib.h>

//
// Functions
//
/**
  <b>This PEIM performs SA PEI Policy initialzation</b> \n
  - <b>Introduction</b> \n
    System Agent PEIM behavior can be controlled by platform policy without modifying reference code directly.
    Platform policy PPI is initialized with default settings in this funciton.
    Some MRC functions will be initialized in this PEIM to provide the capability for customization.
    This policy PPI has to be initialized prior to System Agent initialization PEIM and MRC execution.

  - @pre
    - PEI_READ_ONLY_VARIABLE_PPI

  - @result
    SI_SA_POLICY_PPI will be installed successfully and ready for System Agent reference code use.

  - <b>Porting Recommendations</b> \n
    Policy should be initialized basing on platform design or user selection (like BIOS Setup Menu)

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' varaible.

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the PPI.
  @retval     EFI ERRORS             The PPI is not successfully installed.

**/
EFI_STATUS
PeiSaPolicyInit (
  IN UINT8                     FirmwareConfiguration
  );
#endif

