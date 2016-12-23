/** @file
  Defines common functions used in CseVariableStorage.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _CSE_VARIABLE_STORAGE_H_
#define _CSE_VARIABLE_STORAGE_H_

#include "CseVariableStorageSmmRuntimeDxe.h"
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/VariableNvmStorageLib.h>

extern CSE_VARIABLE_FILE_INFO        *mCseVariableFileInfo[];
extern VARIABLE_STORAGE_PROTOCOL      mCseVariableStorageProtocol;

/**
  Performs common initialization needed for this module.

  @param  None

  @retval EFI_SUCCESS  The module was initialized successfully.
  @retval Others       The module could not be initialized.

**/
EFI_STATUS
EFIAPI
CseVariableStorageCommonInitialize (
  VOID
  );

/**
  Update the CSE NVM variable with the supplied data. These are the same
  arguments as the EFI Variable services.

  @param[in]      VariableName    Name of variable.
  @param[in]      VendorGuid      Guid of variable.
  @param[in]      Data            Variable data.
  @param[in]      DataSize        Size of data. 0 means delete.
  @param[in]      Attributes      Attributes of the variable.
  @param[in, out] IndexVariable   The variable found in the header region corresponding to this variable.

  @retval         EFI_SUCCESS     The update operation is success.
  @retval         Others          The variable update operation failed.

**/
EFI_STATUS
UpdateVariable (
  IN       CHAR16                      *VariableName,
  IN       EFI_GUID                    *VendorGuid,
  IN       VOID                        *Data,
  IN       UINTN                       DataSize,
  IN       UINT32                      Attributes,
  IN OUT   VARIABLE_NVM_POINTER_TRACK  *IndexVariable,
  IN       UINT32                      KeyIndex        OPTIONAL,
  IN       UINT64                      MonotonicCount  OPTIONAL,
  IN       EFI_TIME                    *TimeStamp      OPTIONAL
  );

#endif

