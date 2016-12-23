/** @file
  This is the SMM driver for saving and restoring the powermanagement related MSRs.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtSmm.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED CPU_CONFIG                       *mCpuConfig;
GLOBAL_REMOVE_IF_UNREFERENCED POWER_MGMT_CONFIG                *mPowerMgmtConfig;

/**
  Initialize the SMM power management Handler.

  @param[in] ImageHandle     Pointer to the loaded image protocol for this driver
  @param[in] SystemTable     Pointer to the EFI System Table

  @retval    EFI_SUCCESS     The driver installes/initialized correctly.
  @retval    EFI_NOT_FOUND   CPU Data HOB not available.

**/
EFI_STATUS
EFIAPI
PowerMgmtSmmEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

  EFI_STATUS                        Status;
  EFI_HANDLE                        SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch;
  VOID                              *Hob;
  CPU_INIT_DATA_HOB                 *mCpuInitDataHob;

  SwHandle = 0;
  DEBUG ((DEBUG_INFO, " PpmSmmEntryPoint Started\n"));

  //
  // Get CPU Init Data Hob
  //
  Hob = GetFirstGuidHob (&gCpuInitDataHobGuid);
  if (Hob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return EFI_NOT_FOUND;
  }
  mCpuInitDataHob = (CPU_INIT_DATA_HOB *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));
  mCpuConfig       = (CPU_CONFIG *) (UINTN) mCpuInitDataHob->CpuConfig;
  mPowerMgmtConfig = (POWER_MGMT_CONFIG *) (UINTN)mCpuInitDataHob->PowerMgmtConfig;

  //
  // Locate the ICH SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**) &SwDispatch
                    );

  //
  // Register ACPI S3 MSR restore handler
  //
  SwContext.SwSmiInputValue = mPowerMgmtConfig->S3RestoreMsrSwSmiNumber;

  Status = SwDispatch->Register (
                         SwDispatch,
                         (EFI_SMM_HANDLER_ENTRY_POINT2) S3RestoreMsr,
                         &SwContext,
                         &SwHandle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Install Digital Thermal Sensor
  //
  if (mCpuConfig->EnableDts > 0) {
    DEBUG ((DEBUG_INFO, " Install Digital Thermal Sensor \n"));
    InstallDigitalThermalSensor ();
  }

  //
  // Save MSRs for S3 Resume.
  //
  DEBUG ((DEBUG_INFO, " Saving Processor MSR for S3 Resume \n"));
  S3SaveMsr ();

  return EFI_SUCCESS;
}

