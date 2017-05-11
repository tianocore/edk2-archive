/** @file
  PEI CSE Variable Storage Selector Library.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CseVariableStorageSelectorLib.h>
#include <Library/DebugLib.h>

/**
  Returns the CSE NVM file used for the requested variable.

  @param[in] VariableName         Name of the variable.
  @param[in] VendorGuid           Guid of the variable.
  @param[in] CseVariableFileInfo  An array of pointers to CSE
                                  variable file information.

  @return  The type of CSE NVM file used for this variable.

**/
CSE_VARIABLE_FILE_TYPE
EFIAPI
GetCseVariableStoreFileType (
  IN  CONST CHAR16                 *VariableName,
  IN  CONST EFI_GUID               *VendorGuid,
  IN  CSE_VARIABLE_FILE_INFO       **CseVariableFileInfo
  )
{
  CSE_VARIABLE_FILE_TYPE    Type;

  if (VariableName[0] == 0) {
    //
    // Return the first available CSE file store
    //
    for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
      if (CseVariableFileInfo[Type]->FileEnabled) {
        return Type;
      }
    }
    //
    // There should always be at least one CSE file store enabled
    //
    ASSERT (FALSE);
    return (CSE_VARIABLE_FILE_TYPE) 0;
  } else if ((StrCmp (VariableName, L"Setup") == 0) &&
    CompareGuid (VendorGuid, &gEfiSetupVariableGuid)) {
    //
    // Setup variable is stored in the MRC training data file
    // if it is enabled.
    //
    return (CseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileEnabled ?
                                                                   CseVariableFileTypePreMemoryFile :
                                                                   CseVariableFileTypePrimaryIndexFile
                                                                   );
  } else if ((StrCmp (VariableName, L"MemoryConfig") == 0 || StrCmp (VariableName, L"MemoryBootData") == 0) &&
    CompareGuid (VendorGuid, &gEfiMemoryConfigVariableGuid)) {
    //
    // Memory configuration data is preferred to be stored in
    // the MRC training data file.
    //
    return (CseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileEnabled ?
                                                                   CseVariableFileTypePreMemoryFile :
                                                                   CseVariableFileTypePrimaryIndexFile
                                                                   );
} else if ((StrCmp (VariableName, L"AcpiGlobalVariable") == 0) &&
    CompareGuid (VendorGuid, &gEfiAcpiVariableCompatiblityGuid)) {
    return (CseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileEnabled ?
                                                                   CseVariableFileTypePreMemoryFile :
                                                                   CseVariableFileTypePrimaryIndexFile
                                                                   );
  } else {
    //
    // All other variables are stored in individual CSE files
    //
    return CseVariableFileTypePrimaryIndexFile;
  }
}


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
  )
{
  if (CseFileOffset == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (CseVariableFileType) {
    case CseVariableFileTypePreMemoryFile:
      *CseFileOffset = CSE_PRE_MEMORY_FILE_STORE_OFFSET;
      break;

    case CseVariableFileTypePrimaryIndexFile:
      *CseFileOffset = CSE_PRIMARY_NVM_FILE_STORE_OFFSET;
      break;

    default:
      return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  Returns the HECI protocol used to access the CSE NVM file.

  @param[in]    HeciProtocol            A pointer to the HECI2 protocol if it is active.
                                        If HECI1 is active, the value is NULL.

  @return   CSE_VARIABLE_HECI_PROTOCOL  The HECI protocol to currently use.

**/
CSE_VARIABLE_HECI_PROTOCOL
EFIAPI
GetCseHeciProtocol (
  OUT EFI_HECI_PROTOCOL  **Heci2Protocol
  )
{
  *Heci2Protocol = NULL;

  return CseVariableHeci1Protocol;
}


/**
  Returns the HECI protocol used to access the CSE NVM file based on a given varaible.

  @param[in] VariableName                Name of the variable.
  @param[in] VendorGuid                  Guid of the variable.

  @return    CSE_VARIABLE_HECI_PROTOCOL  The HECI protocol to currently use.

**/
CSE_VARIABLE_HECI_PROTOCOL
EFIAPI
GetCseVariableHeciProtocol (
  IN CONST CHAR16             *VariableName,
  IN CONST EFI_GUID           *VendorGuid
  )
{
  return CseVariableHeci1Protocol;
}

