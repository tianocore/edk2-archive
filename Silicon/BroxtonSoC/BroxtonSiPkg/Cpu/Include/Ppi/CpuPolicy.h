/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_POLICY_PPI_H_
#define _CPU_POLICY_PPI_H_

#include <ConfigBlock.h>
#include <CpuAccess.h>
#include <CpuDataStruct.h>
#include <Ppi/BiosGuardConfig.h>
#include <Ppi/CpuConfigPreMem.h>
#include <Ppi/CpuConfig.h>
#include <Ppi/CpuOverclockingConfig.h>
#include <Ppi/PowerMgmtConfig.h>

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSiCpuPolicyPpiGuid;

#pragma pack(1)

//
// SI_CPU_POLICY_PPI revisions
//
#define SI_CPU_POLICY_PPI_REVISION  1

/**
  The PPI allows the platform code to publish a set of configuration information that the
  CPU drivers will use to configure the processor in the PEI phase.
  This Policy PPI needs to be initialized for CPU configuration.
  @note The PPI has to be published before processor PEIMs are dispatched.

**/
typedef struct {
  CONFIG_BLOCK_TABLE_HEADER  ConfigBlockTableHeader;
} SI_CPU_POLICY_PPI;

#pragma pack()
#endif

