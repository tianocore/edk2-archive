/** @file
  Common header file shared by all source files.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BIOS_RESERVED_MEMORY_H__
#define __BIOS_RESERVED_MEMORY_H__

//
// BIOS reserved memory config
//
#pragma pack(1)

typedef struct {
  UINT8         Pram;
} BIOS_RESERVED_MEMORY_CONFIG;

#pragma pack()

typedef struct _PEI_BIOS_RESERVED_MEMORY_POLICY_PPI PEI_BIOS_RESERVED_MEMORY_POLICY_PPI;

typedef
EFI_STATUS
(EFIAPI *GET_BIOS_RESERVED_MEMORY_POLICY) (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_BIOS_RESERVED_MEMORY_POLICY_PPI  *This,
  IN OUT BIOS_RESERVED_MEMORY_CONFIG      *BiosReservedMemoryPolicy
  );

typedef struct _PEI_BIOS_RESERVED_MEMORY_POLICY_PPI {
  GET_BIOS_RESERVED_MEMORY_POLICY  GetBiosReservedMemoryPolicy;
} PEI_BIOS_RESERVED_MEMORY_POLICY_PPI;

extern EFI_GUID gBiosReservedMemoryPolicyPpiGuid;

#endif //__BIOS_RESERVED_MEMORY_H__

