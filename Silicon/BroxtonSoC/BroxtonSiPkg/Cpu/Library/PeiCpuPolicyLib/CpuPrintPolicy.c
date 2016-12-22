/** @file
  This file is CpuPrintPolicy library.

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
  Print whole CPU_POLICY_PPI and serial out in PostMem.

  @param[in]  SiCpuPolicyPpi       The RC Policy PPI instance

**/
VOID
CpuPrintPolicyPpi (
  IN  SI_CPU_POLICY_PPI       *SiCpuPolicyPpi
  )
{
#ifdef EFI_DEBUG
  EFI_STATUS              Status;
  UINTN                   Index;
  CPU_CONFIG              *CpuConfig;
  POWER_MGMT_CONFIG       *PowerMgmtConfig;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicyPpi, &gCpuConfigGuid,(VOID *) &CpuConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, " Get config block for GUIId = %g \n", &gCpuConfigGuid));

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER*)SiCpuPolicyPpi, &gPowerMgmtConfigGuid,(VOID *)&PowerMgmtConfig);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, " Get config block for GUIId = %g \n", &gPowerMgmtConfigGuid));

  DEBUG ((DEBUG_INFO, "\n------------------------ SiCpuPolicyPpi Print Begin in PostMem-----------------\n"));
  //
  // CPU_CONFIG
  //
  DEBUG ((DEBUG_INFO, " CPU:: HyperThreading : 0x%X\n", CpuConfig->HyperThreading));
  DEBUG ((DEBUG_INFO, " CPU:: VmxEnable : 0x%X\n", CpuConfig->VmxEnable));
  DEBUG ((DEBUG_INFO, " CPU:: ProcTraceMemSize : 0x%X\n", CpuConfig->ProcTraceMemSize));
  DEBUG ((DEBUG_INFO, " CPU:: ProcTraceEnable : 0x%X\n", CpuConfig->ProcTraceEnable));
  DEBUG ((DEBUG_INFO, " CPU:: ActiveProcessorCores : 0x%X\n", CpuConfig->ActiveProcessorCores));
  DEBUG ((DEBUG_INFO, " CPU:: Core1 : 0x%X\n", CpuConfig->DisableCore1));
  DEBUG ((DEBUG_INFO, " CPU:: Core2 : 0x%X\n", CpuConfig->DisableCore2));
  DEBUG ((DEBUG_INFO, " CPU:: Core3 : 0x%X\n", CpuConfig->DisableCore3));
  //
  // POWER_MGMT_CONFIG
  //
  DEBUG ((DEBUG_INFO, " PPM:: TccActivationOffset : 0x%X\n", PowerMgmtConfig->TccActivationOffset));
  DEBUG ((DEBUG_INFO, " PPM:: RatioLimit[4] : 0x%X , 0x%X , 0x%X , 0x%X \n",  PowerMgmtConfig->RatioLimit[0], \
          PowerMgmtConfig->RatioLimit[1], \
          PowerMgmtConfig->RatioLimit[2], \
          PowerMgmtConfig->RatioLimit[3]));
  DEBUG ((DEBUG_INFO, " S3RestoreMsrSwSmiNumber : %x\n", PowerMgmtConfig->S3RestoreMsrSwSmiNumber));
  DEBUG ((DEBUG_INFO, "\n Ppm Lock Enables... \n"));
  DEBUG ((DEBUG_INFO, " PmgCstCfgCtrlLock : %x\n", PowerMgmtConfig->PmgCstCfgCtrlLock));
  DEBUG ((DEBUG_INFO, " ProcHotLock : %x\n", PowerMgmtConfig->ProcHotLock));
  DEBUG ((DEBUG_INFO, "\n PowerMgmtConfig... \n"));
  DEBUG ((DEBUG_INFO, " Eist : %x\n", PowerMgmtConfig->Eist));
  DEBUG ((DEBUG_INFO, " Cx : %x\n", PowerMgmtConfig->Cx));
  DEBUG ((DEBUG_INFO, " C1e : %x\n", PowerMgmtConfig->C1e));
  DEBUG ((DEBUG_INFO, " C1Autodemotion : %x\n", PowerMgmtConfig->C1AutoDemotion));
  DEBUG ((DEBUG_INFO, " C3AutoDemotion : %x\n", PowerMgmtConfig->C3AutoDemotion));
  DEBUG ((DEBUG_INFO, " C1Undemotion : %x\n", PowerMgmtConfig->C1UnDemotion));
  DEBUG ((DEBUG_INFO, " C3UnDemotion : %x\n", PowerMgmtConfig->C3UnDemotion));
  DEBUG ((DEBUG_INFO, " PkgCstateUndemotion : %x\n", PowerMgmtConfig->PkgCStateUnDemotion));
  DEBUG ((DEBUG_INFO, " PkgCState Demotion : %x\n", PowerMgmtConfig->PkgCStateDemotion));
  DEBUG ((DEBUG_INFO, " TurboMode : %x\n", PowerMgmtConfig->TurboMode));
  DEBUG ((DEBUG_INFO, " PowerLimit2 : %x\n", PowerMgmtConfig->PowerLimit2));
  DEBUG ((DEBUG_INFO, " PkgCStateLimit : %x\n", PowerMgmtConfig->PkgCStateLimit));
  DEBUG ((DEBUG_INFO, " TimedMwait : %x\n", PowerMgmtConfig->TimedMwait));

  DEBUG ((DEBUG_INFO, "\n Turbo settings... \n"));
  DEBUG ((DEBUG_INFO, " PowerLimit1 Enable : %x\n", PowerMgmtConfig->PowerLimit1Enable));
  DEBUG ((DEBUG_INFO, " PowerLimit1 Clamp Mode : %x\n", PowerMgmtConfig->PowerLimit1ClampEnable));
  DEBUG ((DEBUG_INFO, " PowerLimit1 : %x\n", PowerMgmtConfig->PowerLimit1));
  DEBUG ((DEBUG_INFO, " Custom PowerLimit1 : %x\n", PowerMgmtConfig->CustomPowerLimit1));
  DEBUG ((DEBUG_INFO, " PowerLimit1Time : %x\n", PowerMgmtConfig->PowerLimit1Time));
  DEBUG ((DEBUG_INFO, " Custom PowerLimit1Time : %x\n", PowerMgmtConfig->CustomPowerLimit1Time));
  DEBUG ((DEBUG_INFO, " PowerLimit2 : %x\n", PowerMgmtConfig->PowerLimit2));
  DEBUG ((DEBUG_INFO, " PowerLimit3 : %x\n", PowerMgmtConfig->PowerLimit3));
  DEBUG ((DEBUG_INFO, " PowerLimit3Time : %x\n", PowerMgmtConfig->PowerLimit3Time));
  DEBUG ((DEBUG_INFO, " PowerLimit3DutyCycle : %x\n", PowerMgmtConfig->PowerLimit3DutyCycle));
  DEBUG ((DEBUG_INFO, " PowerLimit3Lock : %x\n", PowerMgmtConfig->PowerLimit3Lock));
  DEBUG ((DEBUG_INFO, " PowerLimit4 : %x\n", PowerMgmtConfig->PowerLimit4));
  DEBUG ((DEBUG_INFO, " PowerLimit4Lock : %x\n", PowerMgmtConfig->PowerLimit4Lock));
  DEBUG ((DEBUG_INFO, " TurboPowerLimitLock : %x\n", PowerMgmtConfig->TurboPowerLimitLock));
  DEBUG ((DEBUG_INFO, " VidNumber : %x\n", PowerMgmtConfig->NumberOfEntries));
  DEBUG ((DEBUG_INFO, " VidCpuid : %x\n", PowerMgmtConfig->Cpuid));
  DEBUG ((DEBUG_INFO, " VidMaxRatio : %x\n", PowerMgmtConfig->MaxRatio));
  for (Index = 0; Index < MAX_CUSTOM_RATIO_TABLE_ENTRIES; Index++) {
    DEBUG ((DEBUG_INFO, " StateRatio[%d] : %x\n", Index, PowerMgmtConfig->StateRatio[Index]));
  }
  DEBUG ((DEBUG_INFO, " BiProcHot : %x\n", PowerMgmtConfig->BiProcHot));
  DEBUG ((DEBUG_INFO, " DisableProcHotOut : %x\n", PowerMgmtConfig->DisableProcHotOut));
  DEBUG ((DEBUG_INFO, " ProcHotResponse : %x\n", PowerMgmtConfig->ProcHotResponse));
  DEBUG ((DEBUG_INFO, " TStates : %x\n", PowerMgmtConfig->TStates));
  DEBUG ((DEBUG_INFO, " AutoThermalReporting : %x\n", PowerMgmtConfig->AutoThermalReporting));
  DEBUG ((DEBUG_INFO, " ThermalMonitor : %x\n", PowerMgmtConfig->ThermalMonitor));

  DEBUG ((DEBUG_INFO, "\n------------------------ SiCpuPolicyPpi Print End -----------------\n\n"));
#endif
}

