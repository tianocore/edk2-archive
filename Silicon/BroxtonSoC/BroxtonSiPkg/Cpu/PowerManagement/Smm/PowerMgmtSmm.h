/** @file
  Header file for PowerMgmt Smm Driver.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POWER_MGMT_SMM_H_
#define _POWER_MGMT_SMM_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/HobLib.h>
#include <Library/CpuPlatformLib.h>
#include <Private/CpuInitDataHob.h>
#include <Private/Library/CpuCommonLib.h>
#include <CpuAccess.h>
#include <ScAccess.h>
#include <Private/PowerMgmtNvsStruct.h>

#define PPM_WAIT_PERIOD 15

extern POWER_MGMT_CONFIG                *PowerMgmtConfig;

typedef struct _EFI_MSR_VALUES {
  UINT16  Index;
  UINT64  Value;
  UINT64  BitMask;
  BOOLEAN RestoreFlag;
} EFI_MSR_VALUES;

/**
  Save processor MSR runtime settings for S3.

  @retval   EFI_SUCCESS            Processor MSR setting is saved.

**/
EFI_STATUS
S3SaveMsr (
  VOID
  );

/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in, out] Procedure     The function to be run.
  @param[in, out] Buffer        Pointer to a parameter buffer.

  @retval         EFI_SUCCESS

**/
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  );

/**
  This function will restore MSR settings.

  This function must be MP safe.

  @param[in, out] Buffer        Unused

**/
VOID
EFIAPI
ApSafeRestoreMsr (
  IN OUT VOID *Buffer
  );

/**
  Restore processor MSR runtime settings for S3.

  @param[in]  DispatchHandle    The handle of this callback, obtained when registering
  @param[in]  DispatchContex    Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

**/
VOID
S3RestoreMsr (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext
  );

/**
  Digital Thermal Sensor (DTS) SMM driver function.

  @param[in] ImageHandle         Image handle for this driver image

  @retval EFI_SUCCESS            Driver initialization completed successfully
  @retval EFI_OUT_OF_RESOURCES   Error when allocating required memory buffer.

**/
EFI_STATUS
EFIAPI
InstallDigitalThermalSensor (
  VOID
  );
#endif

