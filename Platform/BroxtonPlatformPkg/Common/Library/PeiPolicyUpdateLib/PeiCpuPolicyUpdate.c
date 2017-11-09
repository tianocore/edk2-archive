/** @file
  This file is SampleCode of the library for Intel CPU PEI Policy Update initialization.

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiCpuPolicyUpdate.h>
#include <Library/CpuPolicyLib.h>
#include <Library/SteppingLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>
#include <Platform.h>


/**
  This function performs CPU PEI Policy initialization.

  @param[in] SiCpuPolicyPpi           The Cpu Policy PPI instance
  @param[in] SystemConfiguration      The pointer to get System Setup

  @retval    EFI_SUCCESS              The PPI is installed and initialized.
  @retval    EFI ERRORS               The PPI is not successfully installed.
  @retval    EFI_OUT_OF_RESOURCES     Do not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
UpdatePeiCpuPolicy (
  IN OUT  SI_CPU_POLICY_PPI      *SiCpuPolicyPpi,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  )
{
  EFI_STATUS                  Status;
  CPU_CONFIG                  *CpuConfig;
  POWER_MGMT_CONFIG           *PowerMgmtConfig;
  EFI_PEI_HOB_POINTERS        Hob;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gCpuConfigGuid, (VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, " Get config block for CpuConfig \n"));

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gPowerMgmtConfigGuid, (VOID *) &PowerMgmtConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, " Get config block for PowerMgmtConfig \n"));

  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);

  CpuConfig->ActiveProcessorCores        = SystemConfiguration->ActiveProcessorCores;
  CpuConfig->DisableCore1                = SystemConfiguration->Core1;
  CpuConfig->DisableCore2                = SystemConfiguration->Core2;
  CpuConfig->DisableCore3                = SystemConfiguration->Core3;
  CpuConfig->VmxEnable                   = SystemConfiguration->ProcessorVmxEnable;
  CpuConfig->ProcTraceMemSize            = SystemConfiguration->ProcTraceMemSize;
  CpuConfig->ProcTraceEnable             = SystemConfiguration->ProcTraceEnable;
  CpuConfig->EnableDts                   = SystemConfiguration->EnableDigitalThermalSensor;
  PowerMgmtConfig->Eist                  = SystemConfiguration->EnableGv;
  PowerMgmtConfig->BootPState            = SystemConfiguration->BootPState;
  PowerMgmtConfig->Cx                    = SystemConfiguration->EnableCx;
  PowerMgmtConfig->C1e                   = SystemConfiguration->EnableCxe;
  PowerMgmtConfig->BiProcHot             = SystemConfiguration->EnableProcHot;
  PowerMgmtConfig->PkgCStateLimit        = (MAX_PKG_C_STATE) PcdGet8 (PcdMaxPkgCState);
  PowerMgmtConfig->AutoThermalReporting  = SystemConfiguration->AutoThermalReporting;

  if (BxtStepping () >= BxtPB1) {
    PowerMgmtConfig->PmgCstCfgCtrIoMwaitRedirection  = SOC_DEVICE_ENABLE;
  } else {
    PowerMgmtConfig->PmgCstCfgCtrIoMwaitRedirection  = SOC_DEVICE_DISABLE;
  }

  //
  //If Auto is selected, disable Monitor Mwait for B1 and enable for other stepping
  //
  if (SystemConfiguration->MonitorMwaitEnable == 2) {
    if (BxtStepping () >= BxtPB1) {
      CpuConfig->MonitorMwaitEnable                      = SOC_DEVICE_DISABLE;
    } else {
      CpuConfig->MonitorMwaitEnable                      = SOC_DEVICE_ENABLE;
    }
  } else {
    CpuConfig->MonitorMwaitEnable                      = SystemConfiguration->MonitorMwaitEnable;
  }


  if (SystemConfiguration->EnableCx) {
    //
    // Clear C1 & C3 Auto demotion policy
    //
    PowerMgmtConfig->C1AutoDemotion = SOC_DEVICE_DISABLE;
    PowerMgmtConfig->C3AutoDemotion = SOC_DEVICE_DISABLE;

    switch (SystemConfiguration->CStateAutoDemotion) {
      case 0:
        //
        // Disable C1 and C3 Auto-demotion
        //
        break;

      case 1:
        //
        // Enable C3/C6/C7 Auto-demotion to C1
        //
        PowerMgmtConfig->C1AutoDemotion = SOC_DEVICE_ENABLE;
        break;

      case 2:
        //
        // Enable C6/C7 Auto-demotion to C3
        //
        PowerMgmtConfig->C3AutoDemotion = SOC_DEVICE_ENABLE;
        break;

      default:
      case 3:
        //
        // Enable C6/C7 Auto-demotion to C1 and C3
        //
        PowerMgmtConfig->C1AutoDemotion = SOC_DEVICE_ENABLE;
        PowerMgmtConfig->C3AutoDemotion = SOC_DEVICE_ENABLE;
        break;
    }
    //
    // Configure Un-demotion.
    //
    PowerMgmtConfig->C1UnDemotion = SOC_DEVICE_DISABLE;
    PowerMgmtConfig->C3UnDemotion = SOC_DEVICE_DISABLE;

    switch (SystemConfiguration->CStateUnDemotion) {
      case 0:
        //
        // Disable C1 and C3 Un-demotion
        //
        break;

      case 1:
        //
        // Enable C1 Un-demotion
        //
        PowerMgmtConfig->C1UnDemotion = SOC_DEVICE_ENABLE;
        break;

      case 2:
        //
        // Enable C3 Un-demotion
        //
        PowerMgmtConfig->C3UnDemotion = SOC_DEVICE_ENABLE;
        break;

      case 3:
        //
        // Enable C1 and C3 Un-demotion
        //
        PowerMgmtConfig->C1UnDemotion = SOC_DEVICE_ENABLE;
        PowerMgmtConfig->C3UnDemotion = SOC_DEVICE_ENABLE;
        break;

      default:
        break;
    }
    switch (SystemConfiguration->MaxCoreCState) {
      case 0:
        PowerMgmtConfig->UnlimitedCstate = SOC_DEVICE_ENABLE;
        break;

      case 1:
        PowerMgmtConfig->EnableC1 = SOC_DEVICE_ENABLE;
        break;

      case 2:
        PowerMgmtConfig->EnableC3 = SOC_DEVICE_ENABLE;
        break;

      case 3:
        PowerMgmtConfig->EnableC6 = SOC_DEVICE_ENABLE;
        break;

      case 4:
        PowerMgmtConfig->EnableC7 = SOC_DEVICE_ENABLE;
        break;

      case 5:
        PowerMgmtConfig->EnableC8 = SOC_DEVICE_ENABLE;
        break;

      case 6:
        PowerMgmtConfig->EnableC9 = SOC_DEVICE_ENABLE;
        break;

      case 7:
        PowerMgmtConfig->EnableC10 = SOC_DEVICE_ENABLE;
        break;

      case 8:
        PowerMgmtConfig->EnableCCx = SOC_DEVICE_ENABLE;
        break;

      default:
        break;
    }

    //
    // Pkg C-state Demotion/Un Demotion
    //
    PowerMgmtConfig->PkgCStateDemotion   = SystemConfiguration->PkgCStateDemotion;
    PowerMgmtConfig->PkgCStateUnDemotion = SystemConfiguration->PkgCStateUnDemotion;
  }

  if (BxtStepping() == BxtA0) {
    PowerMgmtConfig->TurboMode  = SOC_DEVICE_DISABLE;
  } else {
    PowerMgmtConfig->TurboMode  = SystemConfiguration->TurboModeEnable;
  }

  PowerMgmtConfig->PowerLimit1Enable       = SystemConfiguration->PowerLimit1Enable;
  PowerMgmtConfig->PowerLimit1ClampEnable  = SystemConfiguration->PowerLimit1Clamp;
  PowerMgmtConfig->CustomPowerLimit1Time   = SystemConfiguration->PowerLimit1Time;
  PowerMgmtConfig->CustomPowerLimit1       = SystemConfiguration->PowerLimit1;

  return EFI_SUCCESS;
}

