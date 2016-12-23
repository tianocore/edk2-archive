/** @file
  Common header file shared by all source files.
  This file includes package header files, library classes and protocol, PPI & GUID definitions.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __STALL_HEADER_H__
#define __STALL_HEADER_H__

//
//Function Prototypes only - please do not add #includes here
//
/**
  This function provides a blocking stall for reset at least the given number of microseconds
  stipulated in the final argument.

  @param[in]  PeiServices      General purpose services available to every PEIM.
  @param[in]  This             Pointer to the local data for the interface.
  @param[in]  Microseconds     Number of microseconds for which to stall.

  @retval     EFI_SUCCESS      The function provided at least the required stall.

**/
EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN CONST EFI_PEI_STALL_PPI  *This,
  IN UINTN                    Microseconds
  );
#endif

