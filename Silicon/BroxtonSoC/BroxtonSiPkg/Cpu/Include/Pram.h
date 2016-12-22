/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PRAM_H_
#define _PRAM_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi20.h>
#include "Platform.h"
#include <Uefi.h>

#define EFI_ACPI_PRAM_BASE_ADDRESS_TABLE_SIGNATURE 0x4D415250

#define EFI_ACPI_OEM_PRAM_REVISION  0x00000001

#define EFI_ACPI_PRAM_BASE_ADDRESS_TABLE_REVISION  0x01

#pragma pack(1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER  Header;
  EFI_PHYSICAL_ADDRESS         PramBaseAddress;
  UINT32                       PramSize;
} EFI_ACPI_PRAM_BASE_ADDRESS_TABLE;

#pragma pack()

#endif //

