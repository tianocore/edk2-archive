/** @file
  Header file for SMBIOS Memory library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMBIOS_MEMORY_LIB_H_
#define _SMBIOS_MEMORY_LIB_H_

/**
  This function will determine memory configuration information from the chipset
  and memory and report the memory configuration info to the DataHub.

  @param[in]  ImageHandle    Handle for the image of this driver.
  @param[in]  SystemTable    Pointer to the EFI System Table.

  @retval     EFI_SUCCESS    If the data is successfully reported.
  @retval     EFI_NOT_FOUND  If the HOB list could not be located.

**/
EFI_STATUS
EFIAPI
SmbiosMemory (
  VOID
  );

#endif

