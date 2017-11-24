/** @file
  Typedefs for board specific functions that are set as dynamic PCDs to be consumed
  by platform common codes.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOARD_FUNCTIONS_PEI_H
#define _BOARD_FUNCTIONS_PEI_H

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/FspCommonLib.h>
#include <Guid/PlatformInfo_Aplk.h>
#include <Guid/AcpiVariable.h>
#include <Guid/AcpiVariableCompatibility.h>
#include <Library/FspPlatformInfoLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/DramPolicyPpi.h>
#include <FspmUpd.h>
#include <FspsUpd.h>
#include <SmipGenerated.h>
#include <ScPreMemPolicyCommon.h>

typedef
EFI_STATUS
(EFIAPI *UPDATE_FSPM_UPD_FUNC) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  );

typedef
EFI_STATUS
(EFIAPI *DRAM_CREATE_POLICY_DEFAULTS_FUNC) (
  IN  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi,
  OUT DRAM_POLICY_PPI                  **DramPolicyPpi,
  IN  IAFWDramConfig                   *DramConfigData,
  IN  UINTN                            *MrcTrainingDataAddr,
  IN  UINTN                            *MrcBootDataAddr,
  IN  UINT8                            BoardId
  );

typedef
EFI_STATUS
(EFIAPI *UPDATE_PCIE_CONFIG_FUNC) (
  IN  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig
  );

#endif

