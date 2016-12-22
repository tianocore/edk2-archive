/** @file
  This file is PeiSiPolicyLib library creates default settings of RC
  Policy and installs RC Policy PPI.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSiPolicyLibrary.h"

/**
  Print whole SI_POLICY_PPI and serial out.

  @param[in]  SiPolicyPpi           The RC Policy PPI instance.

**/
VOID
EFIAPI
SiPrintPolicyPpi (
  IN  SI_POLICY_PPI          *SiPolicyPpi
  )
{
#ifdef EFI_DEBUG
  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy Start ------------------------\n"));
  DEBUG ((DEBUG_INFO, " Revision= %x\n", SiPolicyPpi->Revision));
  DEBUG ((DEBUG_INFO, " EcPresent= %x\n", SiPolicyPpi->EcPresent));
  DEBUG ((DEBUG_INFO, " TempPciBusMin= %x\n", SiPolicyPpi->TempPciBusMin));
  DEBUG ((DEBUG_INFO, " TempPciBusMax= %x\n", SiPolicyPpi->TempPciBusMax));
  DEBUG ((DEBUG_INFO, " TempMemBaseAddr= %x\n", SiPolicyPpi->TempMemBaseAddr));
  DEBUG ((DEBUG_INFO, " TempMemSize= %x\n", SiPolicyPpi->TempMemSize));
  DEBUG ((DEBUG_INFO, " TempIoBaseAddr= %x\n", SiPolicyPpi->TempIoBaseAddr));
  DEBUG ((DEBUG_INFO, " TempIoSize= %x\n", SiPolicyPpi->TempIoSize));
  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy End --------------------------\n"));
#endif // EFI_DEBUG
}

