/** @file
  This is the driver that initializes the Intel SC devices

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInit.h"

/**
  Update ASL object before Boot

  @param[in] ScPolicy              The SC Policy instance

  @retval    EFI_STATUS
  @retval    EFI_NOT_READY         The Acpi protocols are not ready.

**/
EFI_STATUS
UpdateNvsArea (
  IN     SC_POLICY_HOB         *ScPolicy
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        RpDev;
  UINTN                        RpFun;
  UINT32                       Data32;
  EFI_GLOBAL_NVS_AREA_PROTOCOL *GlobalNvsArea;
  BXT_SERIES                   BxtSeries;
  UINT8                        MaxPciePortNum;
  SC_PCIE_CONFIG               *PcieRpConfig;

  Status = GetConfigBlock ((VOID *) ScPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);

  Status = EFI_SUCCESS;
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &GlobalNvsArea
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Update ASL PCIE port address according to root port device and function
  //
  BxtSeries = GetBxtSeries ();
  if ((BxtSeries == BxtP) || (BxtSeries == Bxt1)) {
    MaxPciePortNum = GetScMaxPciePortNum ();
    for (Index = 0; Index < MaxPciePortNum; Index++) {
      Status = GetScPcieRpDevFun (Index, &RpDev, &RpFun);
      ASSERT_EFI_ERROR (Status);

      Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
      GlobalNvsArea->Area->RpAddress[Index] = Data32;
      //
      // Update Maximum Snoop Latency and Maximum No-Snoop Latency values for PCIE
      //
      GlobalNvsArea->Area->PcieLtrMaxSnoopLatency[Index]   = PcieRpConfig->RootPort[Index].LtrMaxSnoopLatency;
      GlobalNvsArea->Area->PcieLtrMaxNoSnoopLatency[Index] = PcieRpConfig->RootPort[Index].LtrMaxNoSnoopLatency;
    }
  }

  return Status;
}

