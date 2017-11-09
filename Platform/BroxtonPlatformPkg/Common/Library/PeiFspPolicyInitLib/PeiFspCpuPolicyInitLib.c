/** @file
  Implementation of Fsp PCH Policy Initialization.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>
#include <Ppi/CpuPolicy.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/CpuPolicyLib.h>


/**
  Performs FSP CPU PEI Policy initialization.

  @param[in, out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspCpuPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                Status;
  SI_CPU_POLICY_PPI         *SiCpuPolicyPpi;

  //
  // Locate SiCpuPolicyPpi
  //
  SiCpuPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiCpuPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiCpuPolicyPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  Performs FSP CPU PEI Policy post memory initialization.

  @param[in, out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspCpuPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  SYSTEM_CONFIGURATION             *SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINTN                            VariableSize = 0;
  EFI_STATUS                       Status;

  DEBUG ((DEBUG_INFO, "Update FspsUpd from setup option...\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  if (Status == EFI_SUCCESS) {
    FspsUpd->FspsConfig.ActiveProcessorCores  = SystemConfiguration->ActiveProcessorCores;
    FspsUpd->FspsConfig.DisableCore1          = SystemConfiguration->Core1;
    FspsUpd->FspsConfig.DisableCore2          = SystemConfiguration->Core2;
    FspsUpd->FspsConfig.DisableCore3          = SystemConfiguration->Core3;
    FspsUpd->FspsConfig.VmxEnable             = SystemConfiguration->ProcessorVmxEnable;
    FspsUpd->FspsConfig.ProcTraceMemSize      = SystemConfiguration->ProcTraceMemSize;
    FspsUpd->FspsConfig.ProcTraceEnable       = SystemConfiguration->ProcTraceEnable;
    FspsUpd->FspsConfig.Eist                  = SystemConfiguration->EnableGv;
    FspsUpd->FspsConfig.BootPState            = SystemConfiguration->BootPState;
    FspsUpd->FspsConfig.EnableCx              = SystemConfiguration->EnableCx;
    FspsUpd->FspsConfig.C1e                   = SystemConfiguration->EnableCxe;
    FspsUpd->FspsConfig.BiProcHot             = SystemConfiguration->EnableProcHot;
    FspsUpd->FspsConfig.PkgCStateLimit        = (MAX_PKG_C_STATE) PcdGet8 (PcdMaxPkgCState);
    FspsUpd->FspsConfig.EnableCx              = SystemConfiguration->EnableCx;
    FspsUpd->FspsConfig.CStateAutoDemotion    = SystemConfiguration->CStateAutoDemotion;
    FspsUpd->FspsConfig.CStateUnDemotion      = SystemConfiguration->CStateUnDemotion;
    FspsUpd->FspsConfig.MaxCoreCState         = SystemConfiguration->MaxCoreCState;
    FspsUpd->FspsConfig.PkgCStateDemotion     = SystemConfiguration->PkgCStateDemotion;
    FspsUpd->FspsConfig.PkgCStateUnDemotion   = SystemConfiguration->PkgCStateUnDemotion;
    FspsUpd->FspsConfig.TurboMode             = SystemConfiguration->TurboModeEnable;
  }

  return EFI_SUCCESS;
}

