/** @file
  Processor Power Management initialization code. This code determines current
  user configuration and modifies and loads ASL as well as initializing chipset
  and processor features to enable the proper power management.

  Acronyms:
    PPM: Processor Power Management
    TM:  Thermal Monitor
    IST: Intel(R) Speedstep technology
    HT:  Hyper-Threading Technology

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtInit.h"
#include "PowerMgmtCommon.h"
#include <Library/MmPciLib.h>
#include <Protocol/CpuInfo.h>

///
/// Global variables
///

///
/// Power Managment policy configurations
///
GLOBAL_REMOVE_IF_UNREFERENCED POWER_MGMT_CONFIG      *mPowerMgmtConfig    = NULL;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_CPUID_REGISTER     mCpuid01             = { 0, 0, 0, 0 }; // CPUID 01 values

///
/// Values for FVID table calculate.
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mTurboBusRatio             = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mMaxBusRatio               = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mMinBusRatio               = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mProcessorFlavor           = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mBspBootRatio              = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mPackageTdp                = 0;                        ///< Processor TDP value in MSR_PACKAGE_POWER_SKU.
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mPackageTdpWatt            = 0;                        ///< Processor TDP value in Watts.
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mCpuConfigTdpBootRatio     = 0;                        ///< Config TDP Boot settings
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mCustomPowerUnit           = 1;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mCpuCacheSize              = 0;                        ///< Cache Size in KB
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mCpuPolicyRevision         = 0;

///
/// Fractional part of Processor Power Unit in Watts. (i.e. Unit is 1/mProcessorPowerUnit)
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mProcessorPowerUnit         = 0;

///
/// Maximum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mPackageMaxPower           = 0;

///
/// Minimum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mPackageMinPower           = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                        mControllableTdpEnable     = 0;     ///< Controllable TDP enable/Disable
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                        mRatioLimitProgrammble     = 0;     ///< Programmable Ratio Limit
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                        mTdpLimitProgrammble       = 0;     ///< Porgrammable TDP Limit
GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA_PROTOCOL *gCpuGlobalNvsAreaProtocol = NULL;  ///< CPU GlobalNvs Protocol
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MP_SERVICES_PROTOCOL     *mMpService                = NULL;

///
/// FVID Table Information
/// Default FVID table
/// One header field plus states
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT16     mNumberOfStates            = 0;
GLOBAL_REMOVE_IF_UNREFERENCED FVID_TABLE *mFvidPointer              = NULL;

///
/// Globals to support updating ACPI Tables
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_TABLE_PROTOCOL     *mAcpiTable                = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mCpu0IstTable             = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mApIstTable               = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mCpu0CstTable             = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mApCstTable               = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mCpuSsdtTable             = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mCpu0TstTable             = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_ACPI_DESCRIPTION_HEADER *mApTstTable               = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_INIT_DATA_HOB           *mCpuInitDataHob           = NULL;

/**
  Initialize the power management support.
  This function will do boot time configuration:
  Detect HW capabilities and SW configuration
  Initialize HW and software state (primarily MSR and ACPI tables)

  @param[in] ImageHandle     Pointer to the loaded image protocol for this driver
  @param[in] SystemTable     Pointer to the EFI System Table

  @retval    EFI_SUCCESS     The driver installes/initialized correctly.
  @retval                    Driver will ASSERT in debug builds on error.
                             PPM functionality is considered critical for mobile systems.

**/
EFI_STATUS
EFIAPI
InitPowerManagement (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  VOID                             *Hob;

  Handle            = NULL;
  //
  // Get CPU Init Data Hob
  //
  Hob = GetFirstGuidHob (&gCpuInitDataHobGuid);
  if (Hob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return EFI_NOT_FOUND;
  }
  mCpuInitDataHob = (CPU_INIT_DATA_HOB *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));

  //
  // Initialize the Global pointer for Power Managment Policy
  //
  mPowerMgmtConfig     = (POWER_MGMT_CONFIG *) (UINTN) mCpuInitDataHob->PowerMgmtConfig;
  mFvidPointer         = (FVID_TABLE *) (UINTN) mCpuInitDataHob->FvidTable;

  //
  // Initialize Power management Global variables
  //
  InitPowerManagementGlobalVariables ();

  //
  // Initialize CPU Power management code (Patch and install CPU ACPI tables, save S3 boot script info)
  //
  Status = InitPpmDxe ();
  ASSERT_EFI_ERROR (Status);

  //
  // Install the PowerMgmtInitDone Protocol so that PowerMgmtS3 driver can load to save the MSRs for S3 resume
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gPowerMgmtInitDoneProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  Initializes the platform power management global variable.
  This must be called prior to any of the functions being used.

**/
VOID
InitPowerManagementGlobalVariables (
  VOID
  )
{
  EFI_STATUS   Status;

  //
  // Locate CPU GlobalNvs Protocol.
  //
  Status = gBS->LocateProtocol (
                  &gCpuGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &gCpuGlobalNvsAreaProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CPU GloableNvs protocol not found"));
  }
  ASSERT_EFI_ERROR (Status);
  if (gCpuGlobalNvsAreaProtocol != NULL) {
    gCpuGlobalNvsAreaProtocol->Area->Cpuid = GetCpuFamily () | GetCpuStepping ();
  }

  //
  // Locate MP service protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &mMpService
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate ACPI table protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTable);
  ASSERT_EFI_ERROR (Status);

  return;
}


/**
  Initialize the processor power management based on hardware capabilities
  and user configuration settings.

  @retval  EFI_SUCCESS      On success
  @retval  Appropiate failure code on error

**/
EFI_STATUS
InitPpmDxe (
  VOID
  )
{
  EFI_STATUS   Status;
#if (ENBDT_PF_ENABLE == 1)
  UINT8        Data8;
#endif
  UINTN        MaximumNumberOfCpus;
  UINTN        NumberOfEnabledCpus;

  Status      = EFI_SUCCESS;

  //
  // Update Fadt table for C State support.
  //
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) {
    ConfigureFadtCStates ();
  }

  Status = InitCpuAcpiTable ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Update Logical Processor Count
  //
  mMpService->GetNumberOfProcessors (
                mMpService,
                &MaximumNumberOfCpus,
                &NumberOfEnabledCpus
                );
  gCpuGlobalNvsAreaProtocol->Area->LogicalProcessorCount = (UINT8) MaximumNumberOfCpus;

  //
  // Save the SW SMI number to trigger SMI to restore the MSRs when resuming from S3
  //
#if (ENBDT_PF_ENABLE == 1)
  Data8 = mPowerMgmtConfig->S3RestoreMsrSwSmiNumber;
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint8,
    (UINTN) (R_PCH_APM_CNT),
    1,
    &Data8
    );
#endif

  return Status;
}

