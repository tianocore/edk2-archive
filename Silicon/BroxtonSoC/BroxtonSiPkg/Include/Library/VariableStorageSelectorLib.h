/** @file
  Variable Storage Selector Library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VARIABLE_STORAGE_SELECTOR_LIB_H_
#define _VARIABLE_STORAGE_SELECTOR_LIB_H_

#include <Uefi.h>

/**
  Gets the variable storage instance ID for the variable storage instance
  that is used to store a given variable

  @param[in]  VariableName          A pointer to a null-terminated string that is
                                    the variable's name.
  @param[in]  VariableGuid          A pointer to an EFI_GUID that is the variable's
                                    GUID. The combination of VariableGuid and
                                    VariableName must be unique.
  @param[in]  AtDxe                 True if in DXE. False if in PEI.
  @param[out] VariableStorageId     The ID for the variable storage instance that
                                    stores a given variable

  @retval     EFI_SUCCESS           Variable storage instance id was retrieved

**/
EFI_STATUS
EFIAPI
GetVariableStorageId (
  IN  CONST  CHAR16       *VariableName,
  IN  CONST  EFI_GUID     *VendorGuid,
  IN         BOOLEAN      AtDxe,
  OUT        EFI_GUID     *VariableStorageId
  );

#endif

