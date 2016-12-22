/** @file
  This file is PeiCpuPolicy library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiCpuPolicyLibrary.h"

/**
  Print whole SI_CPU_POLICY_PPI and serial out in PreMem.

  @param[in]  SiCpuPolicyPpi       The RC Policy PPI instance

**/
VOID
CpuPrintPolicyPpi (
  IN  SI_CPU_POLICY_PPI       *SiCpuPolicyPpi
  )
{
  EFI_STATUS                  Status;
  CPU_CONFIG_PREMEM           *CpuConfigPreMem;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gCpuConfigPreMemGuid, (VOID *) &CpuConfigPreMem);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, " Get config block for GUID = %g",&gCpuConfigPreMemGuid));
  DEBUG ((DEBUG_INFO, "\n------------------------ SiCpuPolicy Print End -----------------\n\n"));
}

