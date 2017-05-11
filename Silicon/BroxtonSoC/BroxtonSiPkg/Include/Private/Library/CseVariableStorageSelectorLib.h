/** @file
  CSE Variable Storage Selector Library.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CSE_VARIABLE_STORAGE_SELECTOR_LIB_H_
#define _CSE_VARIABLE_STORAGE_SELECTOR_LIB_H_

#include <Uefi.h>

#include <Library/CseVariableStorageLib.h>
#include <Protocol/Heci.h>

#define CSE_PRIMARY_NVM_FILE_STORE_OFFSET   0

//
// Variable store options provided by CSE (platform specific)
//
typedef enum {
  VariableStoreTypeHostMemoryBuffer,     ///< A variable store that resides in host memory
  VariableStoreTypeNv,                   ///< General NVM provided by CSE
  VariableStoreTypeMax                   ///< End of CSE variable store types
} CSE_VARIABLE_STORE_TYPE;

/**
  Returns the CSE NVM file used for the requested variable.

  @param[in] VariableName         Name of the variable.
  @param[in] VendorGuid           Guid of the variable.
  @param[in] CseVariableFileInfo  An array of pointers to CSE
                                  variable file information.

  @return    The type of CSE NVM file used for this variable.

**/
CSE_VARIABLE_FILE_TYPE
EFIAPI
GetCseVariableStoreFileType (
  IN  CONST CHAR16                 *VariableName,
  IN  CONST EFI_GUID               *VendorGuid,
  IN  CSE_VARIABLE_FILE_INFO       **CseVariableFileInfo
  );

/**
  Returns the base offset for variable services in the CSE NVM file
  specified for this file type.

  @param[in]  CseVariableFileType  The type of the CSE NVM file.
  @param[out] CseFileOffset        The offset to base a variable store
                                   in the CSE file for this variable.

  @retval     EFI_SUCCESS          Offset successfully returned for the variable.
  @retval     Others               An error occurred.

**/
EFI_STATUS
EFIAPI
GetCseVariableStoreFileOffset (
  IN  CSE_VARIABLE_FILE_TYPE  CseVariableFileType,
  OUT UINT32                  *CseFileOffset
  );

/**
  Returns the HECI protocol used to access the CSE NVM file based on the current point in the boot.

  @param[out]    Heci2Protocol          A pointer to the HECI2 protocol if it is active.
                                        If HECI1 is active, the value is NULL.

  @return        CSE_VARIABLE_HECI_PROTOCOL  The HECI protocol to currently use.

**/
CSE_VARIABLE_HECI_PROTOCOL
EFIAPI
GetCseHeciProtocol (
  OUT      EFI_HECI_PROTOCOL  **Heci2Protocol
  );

/**
  Returns the HECI protocol used to access the CSE NVM file based on a given varaible.

  @param[in] VariableName         Name of the variable.
  @param[in] VendorGuid           Guid of the variable.

  @return   CSE_VARIABLE_HECI_PROTOCOL  The HECI protocol to currently use.

**/
CSE_VARIABLE_HECI_PROTOCOL
EFIAPI
GetCseVariableHeciProtocol (
  IN CONST CHAR16             *VariableName,
  IN CONST EFI_GUID           *VendorGuid
  );

#endif

