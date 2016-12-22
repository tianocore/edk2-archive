/** @file
  Header file for the PeiCpuPolicyLib library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_CPU_POLICY_LIBRARY_H_
#define _PEI_CPU_POLICY_LIBRARY_H_

#include <PiPei.h>
#include <CpuAccess.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/CpuPolicy.h>
#include <Library/CpuPolicyLib.h>
#include <Library/CpuPlatformLib.h>

#define SMM_FROM_SMBASE_DRIVER            0x55
#define SW_SMI_FROM_SMMBASE               SMM_FROM_SMBASE_DRIVER

//
// TXT configuration defines
//
#define TXT_SINIT_MEMORY_SIZE         0x50000
#define TXT_HEAP_MEMORY_SIZE          0xE0000

#define TXT_LCP_PD_BASE               0x0         ///< Platform default LCP
#define TXT_LCP_PD_SIZE               0x0         ///< Platform default LCP
#define TXT_TGA_MEMORY_SIZE           0x0         ///< Maximum 512K of TGA memory (aperture)

#endif // _PEI_CPU_POLICY_LIBRARY_H_

