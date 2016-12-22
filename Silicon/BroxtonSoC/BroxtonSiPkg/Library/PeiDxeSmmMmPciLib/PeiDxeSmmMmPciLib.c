/** @file
  This file contains routines that get PCI Express Address.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

/**
  This procedure will get PCIE address

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval    PCIE address

**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
  )
{
  UINTN  PcieAddress;

  ASSERT ((Bus <= 0xFF) && (Device <= 0x1F) && (Function <= 0x7));
  //
  // Use local variable PcieAddress here. This library is for PEI\DXE\SMM
  // When PEI, the module variable can't be updated. Need a local variable to get the pcie base address + offset.
  //
  PcieAddress = (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (UINTN) ((Bus << 20) + (Device << 15) + (Function << 12));

  return PcieAddress;
}

