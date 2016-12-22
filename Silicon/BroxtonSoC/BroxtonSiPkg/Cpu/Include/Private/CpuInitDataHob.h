/** @file
  Struct and GUID definitions for CpuInitDataHob.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#include <Ppi/CpuPolicy.h>
#include <CpuDataStruct.h>

extern EFI_GUID gCpuInitDataHobGuid;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 Revision;
  EFI_PHYSICAL_ADDRESS   CpuConfig;           ///< CPU_CONFIG CPU configuration Policies.
  EFI_PHYSICAL_ADDRESS   PowerMgmtConfig;     ///< POWER_MGMT_CONFIG PPM configuration Policies.
  EFI_PHYSICAL_ADDRESS   SoftwareGuardConfig; ///< SOFTWARE_GUARD_CONFIG SGX configuration Policies.
  EFI_PHYSICAL_ADDRESS   CpuGnvsPointer;      ///< CPU_GLOBAL_NVS_AREA Pointer.
  EFI_PHYSICAL_ADDRESS   MpData;              ///< Points to ACPI_CPU_DATA structure with multiprocessor data.
  EFI_PHYSICAL_ADDRESS   FvidTable;           ///< FVID Table.
  UINT32                 SiliconInfo;         ///< SILICON_INFO data
} CPU_INIT_DATA_HOB;

#endif

