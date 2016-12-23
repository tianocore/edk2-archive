/** @file
  Instance of Fsp Policy Initialization Library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>

/**
  Performs FSP PEI Policy Pre-memory initialization.

  @param[in] FspHeader            Pointer to FSP infomation header.
  @param[in] FspInitParam         Pointer to FSP initialization parameters.

**/
VOID
EFIAPI
FspPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS            Status;

  //
  // SI Pei Fsp Policy Initialization
  //
  Status = PeiFspSiPolicyInitPreMem (FspmUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - SI Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // SC Pei Fsp Policy Initialization
  //
  Status = PeiFspScPolicyInitPreMem (FspmUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - SC Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInitPreMem (FspmUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - CPU Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInitPreMem (FspmUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - SystemAgent Pei Fsp Policy in Pre-Memory Initialization fail, Status = %r\n", Status));
  }

}

/**
  Performs FSP PEI Policy initialization.

  @param[in, out] FspsUpd       Pointer UPD data region

**/
VOID
EFIAPI
FspPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS            Status;

  //
  // SC Pei Fsp Policy Initialization
  //
  Status = PeiFspScPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - SC Pei Fsp Policy iInitialization fail, Status = %r\n", Status));
  }

  //
  // SystemAgent Pei Fsp Policy Initialization
  //
  Status = PeiFspSaPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - SystemAgent Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }

  //
  // Cpu Pei Fsp Policy Initialization
  //
  Status = PeiFspCpuPolicyInit (FspsUpd);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - CPU Pei Fsp Policy Initialization fail, Status = %r\n", Status));
  }
}

