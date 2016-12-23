/** @file
  SEC Chipset Lib implementation.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/PerformanceLib.h>
#include <Library/TimerLib.h>
#include <SeCState.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>
#include <SeCAccess.h>
#include <IndustryStandard/Pci22.h>
#include <Library/MmPciLib.h>
#include <Library/PcdLib.h>


/**
  Enable/Disable SEC devices

  @param[in] WhichDevice       Select of SEC device
  @param[in] DeviceFuncCtrl    Function control

  @retval    VOID              None

**/
VOID
SeCDeviceControl (
  IN  SEC_DEVICE                   WhichDevice,
  IN  SEC_DEVICE_FUNC_CTRL         DeviceFuncCtrl
  )
{
  UINT32                            PmcBase;
  UINT32                            FuncDisableReg;

  PmcBase         = PMC_BASE_ADDRESS;
  FuncDisableReg  = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);

if (Disabled == DeviceFuncCtrl) {
  switch (WhichDevice) {
    case HECI1:
      MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), FuncDisableReg | BIT27);
      DEBUG ((DEBUG_INFO, "HECI1 Device disabled\n"));
      break;
    case HECI2:
      MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), FuncDisableReg | BIT26);
      DEBUG ((DEBUG_INFO, "HECI2 Device disabled\n"));
      break;
    case HECI3:
      MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), FuncDisableReg | BIT25);
      DEBUG ((DEBUG_INFO, "HECI3 Device disabled\n"));
      break;
    case FTPM:
      MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), (FuncDisableReg | BIT27| BIT26 | BIT25));
      DEBUG ((DEBUG_INFO, "FTPM Device disabled\n"));
      break;
    default:
      break;
  }
  } else {
    switch (WhichDevice) {
      case HECI1:
        MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), (FuncDisableReg & (~BIT27)));
        DEBUG ((DEBUG_INFO, "HECI1 Device Enabled\n"));
        break;
      case HECI2:
        MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), (FuncDisableReg& (~BIT26)));
        DEBUG ((DEBUG_INFO, "HECI2 Device Enabled\n"));
        break;
      case HECI3:
        MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), (FuncDisableReg& (~ BIT25)));
        DEBUG ((DEBUG_INFO, "HECI3 Device Enabled\n"));
        break;
      case FTPM:
        MmioWrite32 ((PmcBase + R_PMC_FUNC_DIS), (FuncDisableReg & (~(BIT27| BIT26 | BIT25))));
        DEBUG ((DEBUG_INFO, "FTPM Device Enabled\n"));
        break;
      default:
        break;
    }
  }
}

