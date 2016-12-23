/** @file
  Header file for the PeiCpuPolicyInit.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_CPU_POLICY_INIT_H_
#define _PEI_CPU_POLICY_INIT_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CpuPolicyLib.h>
#include <Library/CpuPlatformLib.h>
#include <Guid/SetupVariable.h>
#include <Library/PeiCpuPolicyUpdateLib.h>

//
// Function prototypes
//
/**
  This function performs CPU PEI Policy initialization in Pre-Memory.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).

**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInitPreMem (
  );

/**
  This function performs CPU PEI Policy initialization in Post-Memory.

  @param[in] SystemConfiguration    The pointer to get System Setup

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).

**/
EFI_STATUS
EFIAPI
PeiCpuPolicyInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );
#endif

