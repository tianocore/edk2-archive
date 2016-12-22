/** @file
  Header file for the PeiSiPolicyLib library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SI_POLICY_LIBRARY_H_
#define _PEI_SI_POLICY_LIBRARY_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/SiPolicyPpi.h>
#include <Library/SiPolicyLib.h>

#define TEMP_MEM_BASE_ADDRESS 0xFE600000
#define TEMP_IO_BASE_ADDRESS  0xD000

//
// IO/MMIO resource limits
//
#define TEMP_MEM_SIZE         0x200000
#define TEMP_IO_SIZE          0x10

#endif // _PEI_SI_POLICY_LIBRARY_H_

