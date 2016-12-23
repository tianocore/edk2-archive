/** @file
  This file is PeiScPolicyLib library for printing PREMEM Policy settings.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiScPolicyLibrary.h"

VOID
PrintPciePreMemConfig (
  IN CONST SC_PCIE_PREMEM_CONFIG   *PciePreMemConfig
  )
{
  UINT8 i;

  DEBUG ((DEBUG_INFO, "--- PCIe Config ---\n"));
  for (i = 0; i < GetScMaxPciePortNum (); i++) {
    DEBUG ((DEBUG_INFO, " RootPort[%d] PERST = %x\n", i, PciePreMemConfig->RootPort[i].Perst));
  }
  DEBUG ((DEBUG_INFO, " StartTimerTickerOfPerstAssert = %x\n", i, PciePreMemConfig->StartTimerTickerOfPfetAssert));
}


/**
  Print SC_LPC_PREMEM_CONFIG and serial out.

  @param[in] LpcConfig  Pointer to a SC_LPC_CONFIG that provides the platform setting

**/
VOID
PrintLpcPreMemConfig (
  IN CONST SC_LPC_PREMEM_CONFIG  *LpcConfig
  )
{
  DEBUG ((DEBUG_INFO, "--- LPC Config ---\n"));
  DEBUG ((DEBUG_INFO, " EnhancePort8xhDecoding = %x\n", LpcConfig->EnhancePort8xhDecoding));
}


/**
  Print whole SC_PREMEM_POLICY_PPI and serial out.

  @param[in] ScPreMemPolicyPpi The RC Policy PPI instance

**/
VOID
EFIAPI
ScPreMemPrintPolicyPpi (
  IN  SC_PREMEM_POLICY_PPI *ScPreMemPolicyPpi
  )
{
  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig;
  SC_LPC_PREMEM_CONFIG   *LpcPreMemConfig;
  EFI_STATUS             Status;

  Status = GetConfigBlock ((VOID *) ScPreMemPolicyPpi, &gPcieRpPreMemConfigGuid, (VOID *) &PciePreMemConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPreMemPolicyPpi, &gLpcPreMemConfigGuid, (VOID *) &LpcPreMemConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "--- SC Print PreMem Policy Start ---\n"));
  PrintPciePreMemConfig (PciePreMemConfig);
  PrintLpcPreMemConfig (LpcPreMemConfig);
  DEBUG ((DEBUG_INFO, "--- SC Print PreMem Policy End ---\n"));
}

