/** @file
  Header file for the Dxe Runtime PCI library.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_RUNTIME_PCI_LIB_PCIEXPRESS_H_
#define _DXE_RUNTIME_PCI_LIB_PCIEXPRESS_H_


/**
  Constructor for Pci library. Register VirtualAddressNotifyEvent() notify function
  It will ASSERT() if that operation fails

  @param[in] None

  @retval    EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
EFIAPI
PciLibConstructor (
  VOID
  );

/**
  Register memory space
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If SmPciLibAddressMapIndex) > PCI_LIB_ADDRESS_MAP_MAX_ITEM, then ASSERT().

  @param[in] Address                 Starting address of the memory space
  @param[in] Length                  Length of the memory space

  @retval    EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
EFIAPI
PciLibRegisterMemory (
  IN UINTN   Address,
  IN UINTN   Length
  );

#endif

