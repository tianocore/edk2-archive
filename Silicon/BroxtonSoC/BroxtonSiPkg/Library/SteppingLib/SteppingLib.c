/** @file
  This file contains routines that get Soc Stepping.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/Pci30.h>
#include <SaAccess.h>
#include <ScRegs/RegsPmc.h>
#include <Library/SteppingLib.h>
#include <Library/MmPciLib.h>

/**
  Return SOC series type

  @retval  BXT_SERIES      SOC series type

**/
BXT_SERIES
EFIAPI
GetBxtSeries (
  VOID
  )
{
  UINTN   McD0Base;
  UINT16  VenId;
  UINT16  DevId;

  McD0Base = MmPciBase (
               SA_MC_BUS,
               SA_MC_DEV,
               SA_MC_FUN
               );

  VenId = MmioRead16 (McD0Base + PCI_VENDOR_ID_OFFSET);
  DevId = MmioRead16 (McD0Base + PCI_DEVICE_ID_OFFSET);
  if (VenId == V_SA_MC_VID) {
    switch (DevId) {
      case V_SA_MC_DID0:
        return Bxt;
        break;
      case V_SA_MC_DID1:
        return Bxt1;
        break;
      case V_SA_MC_DID3:
        return BxtP;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT Series.\n"));
        return BxtSeriesMax;
        break;
    }
  }
  return BxtSeriesMax;
}


/**
  This procedure will get Soc Stepping

  @retval   Soc Stepping

**/
BXT_STEPPING
EFIAPI
BxtStepping (
  VOID
  )
{
  BXT_SERIES  BxtSeries;
  UINT8       RevId;
  UINTN       McD0Base;

  McD0Base = MmPciBase (
               SA_MC_BUS,
               SA_MC_DEV,
               SA_MC_FUN
               );
  RevId = MmioRead8 (McD0Base + PCI_REVISION_ID_OFFSET);
  BxtSeries = GetBxtSeries ();

  if (BxtSeries == BxtP) {
    switch (RevId) {
      case V_SA_MC_RID_3:
        DEBUG ((DEBUG_INFO, "BXTP-A0 detected!\n"));
        return BxtPA0;
        break;
      case V_SA_MC_RID_9:
        DEBUG ((DEBUG_INFO, "BXTP-B0 detected!\n"));
        return BxtPB0;
        break;
      case V_SA_MC_RID_A:
        DEBUG ((DEBUG_INFO, "BXTP-B1 detected!\n"));
        return BxtPB1;
        break;
      case V_SA_MC_RID_B:
        DEBUG ((DEBUG_INFO, "BXTP-B2 detected!\n"));
        return BxtPB2;
        break;
      case V_SA_MC_RID_C:
        DEBUG ((DEBUG_INFO, "BXTP-E0 detected!\n"));
        return BxtPE0;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT-P stepping.\n"));
        return BxtSteppingMax;
        break;
    }
  } else if (BxtSeries == Bxt1) {
    switch (RevId) {
      case V_SA_MC_RID_6:
        DEBUG ((DEBUG_INFO, "BXT-B0 detected!\n"));
        return BxtB0;
        break;
      case V_SA_MC_RID_7:
        DEBUG ((DEBUG_INFO, "BXT-B1 detected!\n"));
        return BxtB1;
        break;
      case V_SA_MC_RID_8:
        DEBUG ((DEBUG_INFO, "BXT-B2 detected!\n"));
        return BxtB2;
        break;
      case V_SA_MC_RID_C:
        DEBUG ((DEBUG_INFO, "BXT-C0 detected!\n"));
        return BxtC0;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT1 stepping.\n"));
        return BxtSteppingMax;
        break;
    }
  } else if (BxtSeries == Bxt) {
    switch (RevId) {
      case V_SA_MC_RID_0:
        DEBUG ((DEBUG_INFO, "BXT-A0 detected!\n"));
        return BxtA0;
        break;
      case V_SA_MC_RID_1:
        DEBUG ((DEBUG_INFO, "BXT-A1 detected!\n"));
        return BxtA1;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT stepping.\n"));
        return BxtSteppingMax;
        break;
    }
  }

  return BxtSteppingMax;
}



