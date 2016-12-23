/** @file
  Variable Cache Library implementation file.
  This library builds a variable cache HOB consumed by the variable driver.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <VariableNvmStorageFormat.h>
#include <Library/VariableNvmStorageLib.h>
#include <Library/PeiVariableCacheLib.h>

EFI_STATUS
EFIAPI
InitializeVariableCacheStore (
  IN VARIABLE_STORE_HEADER  *VariableCacheStore
  )
{
  if (VariableCacheStore == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the variable store
  //
  CopyMem (&VariableCacheStore->Signature, &gEfiVariableGuid, sizeof (EFI_GUID));

  VariableCacheStore->Format = VARIABLE_STORE_FORMATTED;
  VariableCacheStore->State  = VAR_IN_DELETED_TRANSITION;
  VariableCacheStore->Size   = 0;

  return EFI_SUCCESS;
}


//
// Copy the NVM variable to a new variable in the cache store
//
EFI_STATUS
EFIAPI
CopyNvmVariablesToCacheStore (
  IN  VARIABLE_STORE_HEADER    *VariableStoreHeader,
  IN  VARIABLE_NVM_STORE_INFO  *VariableNvmStoreInfo
  )
{
  VARIABLE_HEADER      *VariableHeader    = NULL;
  VARIABLE_NVM_HEADER  *VariableNvmHeader = NULL;
  CHAR16               *VariableNamePtr   = NULL;
  UINT8                *VariableDataPtr   = NULL;
  UINTN                *MrcDataPtr        = NULL;

  VariableHeader    = (VARIABLE_HEADER *) HEADER_ALIGN (VariableStoreHeader + 1);
  VariableNvmHeader = GetStartPointer (VariableNvmStoreInfo->VariableStoreHeader);

  DEBUG ((EFI_D_INFO, "++> Copying variables from the pre-memory location to the cache HOB...\n"));
  DEBUG ((EFI_D_INFO, " Starting pointers: VariableHeader at %p. VariableNvmHeader at %p.\n", VariableHeader, VariableNvmHeader));

  while (IsValidVariableHeader (VariableNvmHeader, GetEndPointer (VariableNvmStoreInfo->VariableStoreHeader))) {

    VariableHeader->StartId    = VARIABLE_DATA;
    VariableHeader->State      = VAR_ADDED;
    VariableHeader->Attributes = VariableNvmHeader->Attributes;
    VariableHeader->NameSize   = VariableNvmHeader->NameSize;
    VariableHeader->DataSize   = VariableNvmHeader->DataSize;

    VariableNamePtr = (CHAR16 *) ((UINTN) VariableHeader + ((VariableNvmStoreInfo->AuthFlag) ? sizeof (AUTHENTICATED_VARIABLE_HEADER) : sizeof (VARIABLE_HEADER)));
    VariableDataPtr = (UINT8 *) ((UINTN) VariableNamePtr + VariableHeader->NameSize + GET_PAD_SIZE (VariableHeader->NameSize));

    DEBUG ((EFI_D_INFO, "  VariableNamePtr is %p. VariableDataPtr is %p.\n", VariableNamePtr, VariableDataPtr));

    //
    // Capture the MRC configuration data address
    //
    if ((StrCmp (GetVariableNamePtr (VariableNvmHeader, VariableNvmStoreInfo->AuthFlag), L"MemoryConfig") == 0
      || StrCmp (GetVariableNamePtr (VariableNvmHeader, VariableNvmStoreInfo->AuthFlag), L"MemoryBootData") == 0)
      && CompareGuid (&VariableNvmHeader->VendorGuid, &gEfiMemoryConfigVariableGuid)) {

      VariableHeader->DataSize = sizeof (UINTN);
      MrcDataPtr = (UINTN *) VariableDataPtr;
      *MrcDataPtr = (UINTN) ((UINTN) VariableNvmStoreInfo->VariableStoreHeader + GetVariableDataPtr (VariableNvmHeader, VariableNvmStoreInfo));

      DEBUG ((EFI_D_INFO, "  Found memory config data in variable store (in SRAM) at address 0x%x. Storing address in cache store...\n", *MrcDataPtr));
    } else {
      DEBUG ((EFI_D_INFO, "  Variable data pointer = 0x%x.\n", ((UINTN) VariableNvmStoreInfo->VariableStoreHeader + GetVariableDataPtr (VariableNvmHeader, VariableNvmStoreInfo))));
      CopyMem (VariableDataPtr,
               (UINT8 *) ((UINTN) VariableNvmStoreInfo->VariableStoreHeader + GetVariableDataPtr (VariableNvmHeader, VariableNvmStoreInfo)),
               VariableNvmHeader->DataSize
               );
    }

    //
    // Copy the variable GUID and name
    //
    CopyMem (&VariableHeader->VendorGuid, &VariableNvmHeader->VendorGuid, sizeof (EFI_GUID));
    CopyMem (VariableNamePtr, GetVariableNamePtr (VariableNvmHeader, VariableNvmStoreInfo->AuthFlag), VariableNvmHeader->NameSize);

    DEBUG ((EFI_D_INFO, "  Variable name is %s and variable GUID is %g.\n", VariableNamePtr, &VariableHeader->VendorGuid));

    //
    // Output debug data for the variable just copied
    //
    DEBUG ((EFI_D_INFO, "  Variable name is %d bytes.\n", VariableHeader->NameSize));
    DEBUG ((EFI_D_INFO, "  Variable data is %d bytes.\n", VariableHeader->DataSize));

    VariableHeader    = (VARIABLE_HEADER *) HEADER_ALIGN ((UINTN) VariableDataPtr + VariableHeader->DataSize + GET_PAD_SIZE (VariableHeader->DataSize));
    VariableNvmHeader = GetNextVariablePtr (VariableNvmStoreInfo, VariableNvmHeader);
  }

  ASSERT ((UINTN) VariableHeader > (UINTN) VariableStoreHeader);
  VariableStoreHeader->Size = (UINT32) ((UINTN) VariableHeader - (UINTN) VariableStoreHeader);

  DEBUG ((EFI_D_INFO, " Final VariableStoreHeader size = %d bytes.\n", VariableStoreHeader->Size));
  return EFI_SUCCESS;
}


UINT32
EFIAPI
GetNewVariableStoreSize (
  IN  VARIABLE_NVM_STORE_INFO  *VariableNvmStoreInfo
  )
{
  UINT32               VariableStoreSize  = 0;
  VARIABLE_NVM_HEADER  *VariableNvmHeader = GetStartPointer (VariableNvmStoreInfo->VariableStoreHeader);

  DEBUG ((EFI_D_INFO, "Start pointer = %p\n", VariableNvmHeader));

  VariableStoreSize = VariableNvmStoreInfo->VariableStoreHeader->Size;
  while (IsValidVariableHeader (VariableNvmHeader, GetEndPointer (VariableNvmStoreInfo->VariableStoreHeader))) {
    if ((StrCmp (GetVariableNamePtr (VariableNvmHeader, VariableNvmStoreInfo->AuthFlag), L"MemoryConfig") == 0
      || StrCmp (GetVariableNamePtr (VariableNvmHeader, VariableNvmStoreInfo->AuthFlag), L"MemoryBootData") == 0)
      && CompareGuid (&VariableNvmHeader->VendorGuid, &gEfiMemoryConfigVariableGuid)) {
      VariableStoreSize -= VariableNvmHeader->DataSize;
      VariableStoreSize += sizeof (UINTN);
      break;
    }
    VariableNvmHeader = GetNextVariablePtr (VariableNvmStoreInfo, VariableNvmHeader);
  }

  return VariableStoreSize;
}


/**
  Creates the PEI variable cache HOB.

  @param[out]  MemoryConfigData       A pointer to the memory training data.
                                      This function will modify the pointer to point to the training data.

  @retval      EFI_SUCCESS            The PEI variable cache was successfully created.

**/
EFI_STATUS
EFIAPI
CreateVariableCacheHob (
  VOID
  )
{
  EFI_STATUS                       Status;
  VARIABLE_STORE_HEADER            *VariableStoreHeader;
  VARIABLE_NVM_STORE_INFO          VariableNvmStoreInfo;
  UINT32                           TotalVariableFileSize = 0;
  EFI_HOB_GUID_TYPE                *PreMemoryVariableLocationGuidHob = NULL;
  PRE_MEMORY_VARIABLE_LOCATION_HOB *PreMemoryLocationHob             = NULL;

  if (GetFirstGuidHob (&gPeiVariableCacheHobGuid) != NULL) {
    //
    // Variable cache HOB already created
    //
    return EFI_SUCCESS;
  }

  // The variable store (at a memory address) to cache is passed via PreMemoryVariableLocationHob.
  //
  PreMemoryVariableLocationGuidHob = GetFirstGuidHob (&gPreMemoryVariableLocationHobGuid);

  if (PreMemoryVariableLocationGuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "Pre-memory variable location HOB not found. Could not build the variable cache.\n"));
    return EFI_NOT_FOUND;
  }

  PreMemoryLocationHob = (PRE_MEMORY_VARIABLE_LOCATION_HOB *) (VOID *) GET_GUID_HOB_DATA (PreMemoryVariableLocationGuidHob);

  if (PreMemoryLocationHob == NULL) {
    DEBUG ((EFI_D_ERROR, "Pre-memory variable data could be not found. Could not build the variable cache.\n"));
    return EFI_NOT_FOUND;
  }

  VariableNvmStoreInfo.VariableStoreHeader = PreMemoryLocationHob->VariableDataPtr;
  Status = IsAuthenticatedVariableStore (PreMemoryLocationHob->VariableDataPtr, &VariableNvmStoreInfo.AuthFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TotalVariableFileSize = GetNewVariableStoreSize (&VariableNvmStoreInfo);
  DEBUG ((EFI_D_INFO, "Total variable store size in pre-memory file = %d bytes.\n", TotalVariableFileSize));

  //
  // Ensure the PEI variable cache size does not exceed the platform defined limit
  //
  if (TotalVariableFileSize > PcdGet32 (PcdVariableCacheMaximumSize)) {
    DEBUG ((EFI_D_ERROR, "The space required for the variable cache exceeds the maximum cache size specified. Actual = 0x%x Maximum = 0x%x\n",
      TotalVariableFileSize,
      PcdGet32 (PcdVariableCacheMaximumSize)));

    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create the variable cache HOB
  //
  VariableStoreHeader = BuildGuidHob (&gPeiVariableCacheHobGuid, TotalVariableFileSize);
  if (VariableStoreHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (VariableStoreHeader, TotalVariableFileSize);

  DEBUG ((EFI_D_INFO, "VariableCacheStore HOB is allocated at 0x%x\n", VariableStoreHeader));

  InitializeVariableCacheStore (VariableStoreHeader);

  Status = CopyNvmVariablesToCacheStore (VariableStoreHeader, &VariableNvmStoreInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "Variable cache created successfully\n"));

  return EFI_SUCCESS;
}

