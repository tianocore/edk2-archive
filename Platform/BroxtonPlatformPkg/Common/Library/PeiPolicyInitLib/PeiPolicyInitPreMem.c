/** @file
  This file is SampleCode for Intel PEI Platform Policy initialization.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiPolicyInit.h"

/**
  Initialize Intel PEI Platform Policy

**/
EFI_STATUS
EFIAPI
PeiPolicyInitPreMem (
  VOID
  )
{
  EFI_STATUS    Status;

  //
  // CPU PEI Policy Initialization
  //
  Status = PeiCpuPolicyInitPreMem ();
  DEBUG ((DEBUG_INFO, "CPU PEI Policy Initialization Done in Pre-Memory\n"));
  ASSERT_EFI_ERROR (Status);

  return Status;
}

