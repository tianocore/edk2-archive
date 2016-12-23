/** @file
  Header file for the PeiScPolicy library.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SC_POLICY_LIB_H_
#define _PEI_SC_POLICY_LIB_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiScPolicyLib.h>
#include <Library/ScPlatformLib.h>
#include <Ppi/ScPolicy.h>
#include <ScAccess.h>
#include <SaAccess.h>
#include <Library/SideBandLib.h>
#include <ConfigBlock.h>
#include <Library/ConfigBlockLib.h>

#define SC_SMBUS_BASE_ADDRESS 0xEFA0
#define SC_HPET_BASE_ADDRESS  0xFED00000
#define SW_SMI_BIOS_LOCK      0xA9

typedef
VOID
(*LOAD_DEFAULT_FUNCTION) (
  IN VOID   *ConfigBlockPointer
  );

typedef struct {
  EFI_GUID               *Guid;
  UINT16                 Size;
  UINT8                  Revision;
  LOAD_DEFAULT_FUNCTION  LoadDefault;
} IP_BLOCK_ENTRY;

#endif // _PEI_SC_POLICY_LIB_H_

