/** @file
  This file is SampleCode of the library for Intel Silicon PEI
  Platform Policy initialzation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSiPolicyUpdate.h"
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>

/**
  This function performs Silicon PEI Policy initialzation.

  @param[in] SiPolicy     The Silicon Policy PPI instance

  @retval    EFI_SUCCESS  The function completed successfully

**/
EFI_STATUS
EFIAPI
UpdatePeiSiPolicy (
  IN OUT SI_POLICY_PPI *SiPolicy
  )
{
  UINTN                           VariableSize;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_STATUS                      Status;

  //
  // Update Silicon Policy Config
  //
  //
  // Retrieve Setup variable
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );
  SiPolicy->OsSelection = 3;

  return EFI_SUCCESS;
}

