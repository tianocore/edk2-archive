/** @file
  Implements CSE Variable Storage Services and installs
  an instance of the VariableStorage Runtime DXE protocol.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CseVariableStorage.h"

//
// Module globals
//
CSE_VARIABLE_FILE_INFO        *mCseVariableFileInfo[CseVariableFileTypeMax];

VARIABLE_STORAGE_PROTOCOL      mCseVariableStorageProtocol = {
  CseVariableStorageGetId,
  CseVariableStorageGetVariable,
  CseVariableStorageGetAuthenticatedVariable,
  CseVariableStorageGetNextVariableName,
  CseVariableStorageGetStorageUsage,
  CseVariableStorageGetAuthenticatedSupport,
  CseVariableStorageSetVariable,
  CseVariableStorageWriteServiceIsReady,
  CseVariableStorageRegisterWriteServiceReadyCallback,
};

BOOLEAN mIsEmmcBoot = FALSE;

/**
  Performs common initialization needed for this module.

  @param  None

  @retval  EFI_SUCCESS   The module was initialized successfully.
  @retval  Others        The module could not be initialized.

**/
EFI_STATUS
EFIAPI
CseVariableStorageCommonInitialize (
  VOID
  )
{
  EFI_STATUS              Status;
  UINT8                   BootMedia;
  EFI_HOB_GUID_TYPE       *GuidHobPtr;
  MBP_CURRENT_BOOT_MEDIA  *BootMediaData;

  //
  // Allocate and initialize the global variable structure
  //
  Status = InitializeCseStorageGlobalVariableStructures (mCseVariableFileInfo, TRUE);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Establish the CSE variable stores
  // Create an enabled variable store if it does not exist
  // Load the variable header data regions needed to find variables
  //
  Status = EstablishAndLoadCseVariableStores (mCseVariableFileInfo, TRUE);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error returned from EstablishAndLoadCseVariableStores()\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((EFI_D_INFO, "  CSE Pre-memory File Enabled: %c\n  CSE Pre-memory Store Established:  %c\n  CSE NVM File Enabled: %c\n  CSE NVM Store Established: %c\n\n",
          (mCseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileEnabled ? 'T' : 'F'),
          (mCseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileStoreEstablished ? 'T' : 'F'),
          (mCseVariableFileInfo[CseVariableFileTypePrimaryIndexFile]->FileEnabled ? 'T' : 'F'),
          (mCseVariableFileInfo[CseVariableFileTypePrimaryIndexFile]->FileStoreEstablished ? 'T' : 'F')
          ));

  GuidHobPtr  = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  if (GuidHobPtr == NULL) {
    DEBUG ((DEBUG_ERROR, "CseVariableStorage Error: Boot Media HOB does not exist!\n"));
    ASSERT (GuidHobPtr != NULL);
    return FALSE;
  }

  BootMediaData = (MBP_CURRENT_BOOT_MEDIA *) GET_GUID_HOB_DATA (GuidHobPtr);
  BootMedia = (UINT8) BootMediaData->PhysicalData;
  if (BootMedia != BOOT_FROM_SPI) {
    mIsEmmcBoot = TRUE;
  }

  return EFI_SUCCESS;
}


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
  )
{
  BOOLEAN                    UpdatingExistingVariable;
  CSE_VARIABLE_FILE_TYPE     VariableCseFileType;
  EFI_STATUS                 Status;
  UINT8                      EmptyBuffer                      = 0;
  UINTN                      NewVariableTotalSize             = 0;
  UINT32                     NewVariableHeaderSize            = 0;
  INT32                      TotalVariableHeaderRemainingSize = 0;
  INT32                      TotalVariableStoreRemainingSize  = 0;
  UINT32                     VariablePaddingSize              = 0;
  UINT32                     VariableStoreBaseOffset          = 0; ///< The offset from the beginning of the file to the variable store
  UINT32                     VariableIndexFileWriteOffset     = 0; ///< The offset from the beginning of the index file to the variable header
  UINT32                     VariableDataWriteOffset          = 0; ///< The offset from the beginning of the data file to the data
  UINT32                     VariableDataFileNumber           = 0; ///< The variable data file number (if data is stored in individual files)
  CHAR8                      CseVariableDataFileName[CSE_MAX_NVM_FILE_NAME_LENGTH];

  AUTHENTICATED_VARIABLE_NVM_HEADER *AuthenticatedVariableHeader = NULL;
  VARIABLE_NVM_HEADER               *NewVariableHeader           = NULL;
  VARIABLE_NVM_STORE_INFO           VariableStoreInfo;
  VARIABLE_NVM_POINTER_TRACK        VariableNvmDeletedVariablePtrTrack;

  VariableNvmDeletedVariablePtrTrack.StartPtr               = NULL;
  VariableNvmDeletedVariablePtrTrack.EndPtr                 = NULL;
  VariableNvmDeletedVariablePtrTrack.CurrPtr                = NULL;
  VariableNvmDeletedVariablePtrTrack.InDeletedTransitionPtr = NULL;

  UpdatingExistingVariable = (BOOLEAN) (IndexVariable->CurrPtr != NULL && IsValidVariableHeader (IndexVariable->CurrPtr, IndexVariable->EndPtr));
  VariableCseFileType      = GetCseVariableStoreFileType (VariableName, VendorGuid, mCseVariableFileInfo);

  if (VariableCseFileType >= CseVariableFileTypeMax) {
    return EFI_UNSUPPORTED;
  }

  if (mCseVariableFileInfo[VariableCseFileType] == NULL) {
    DEBUG ((EFI_D_ERROR, "The global CSE file info variable was not populated for this file type.\n"));
    return EFI_NOT_FOUND;
  }

  VariableStoreInfo.VariableStoreHeader = (VARIABLE_NVM_STORE_HEADER *) (UINTN) mCseVariableFileInfo[VariableCseFileType]->HeaderRegionBase;
  Status = IsAuthenticatedVariableStore (VariableStoreInfo.VariableStoreHeader, &VariableStoreInfo.AuthFlag);

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if (VariableStoreInfo.VariableStoreHeader == NULL) {
    DEBUG ((EFI_D_ERROR, "The variable store could not be found for this file type.\n"));
    return EFI_NOT_FOUND;
  }

  if (!mCseVariableFileInfo[VariableCseFileType]->FileEnabled || !mCseVariableFileInfo[VariableCseFileType]->FileStoreEstablished) {
    DEBUG ((EFI_D_ERROR, "Error: Attempted to set a variable with a CSE file type not enabled.\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check to see if this is a variable that has been previously deleted
  //
  if (!UpdatingExistingVariable && VariableCseFileType != CseVariableFileTypePreMemoryFile) {
    VariableNvmDeletedVariablePtrTrack.StartPtr  = GetStartPointer (VariableStoreInfo.VariableStoreHeader);
    VariableNvmDeletedVariablePtrTrack.EndPtr    = GetEndPointer (VariableStoreInfo.VariableStoreHeader);

    Status = FindDeletedVariable (VariableName, VendorGuid, VariableStoreInfo.VariableStoreHeader, &VariableNvmDeletedVariablePtrTrack);

    if (EFI_ERROR (Status)) {
      VariableNvmDeletedVariablePtrTrack.CurrPtr = NULL;
    }
  }

  //
  // Update the offsets for the CSE file store type
  //
  // Pre-memory file: This is an offset in the file
  // Primary index file: This is the number of the CSE NVM data file
  //
  GetCseVariableStoreFileOffset (VariableCseFileType, &VariableStoreBaseOffset);

  if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
    if (UpdatingExistingVariable) {
      VariableDataWriteOffset = IndexVariable->CurrPtr->DataOffset;
    } else {
      VariableDataWriteOffset = (UINT32) (mCseVariableFileInfo[VariableCseFileType]->FileStoreMaximumSize \
                                  - VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength \
                                  - DataSize
                                  );

      if (VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength == 0) {
        VariableDataWriteOffset--;
      }

      VariablePaddingSize = NVM_GET_PAD_SIZE_N (VariableDataWriteOffset);
      VariableDataWriteOffset -= VariablePaddingSize;
    }
  } else {
    if (UpdatingExistingVariable) {
      VariableDataFileNumber = IndexVariable->CurrPtr->DataOffset;

      if (CSE_PRIMARY_NVM_MAX_DATA_FILES < 1 || VariableDataFileNumber >= CSE_PRIMARY_NVM_MAX_DATA_FILES) {
        DEBUG ((EFI_D_ERROR, "Cannot save new variable - maximum number of CSE NVM files reached.\n"));
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      if (VariableNvmDeletedVariablePtrTrack.CurrPtr != NULL) {
        DEBUG ((EFI_D_INFO, "Recycling CSE NVM variable file %d.\n", VariableNvmDeletedVariablePtrTrack.CurrPtr->DataOffset));
        VariableDataFileNumber = VariableNvmDeletedVariablePtrTrack.CurrPtr->DataOffset;
      } else {
        VariableDataFileNumber = VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalEntries;
      }
    }
  }

  //
  // Calculate the space remaining in the variable header region
  //
  if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
    TotalVariableHeaderRemainingSize = (INT32) (mCseVariableFileInfo[VariableCseFileType]->FileStoreMaximumSize \
                                         - sizeof (VARIABLE_NVM_STORE_HEADER) \
                                         - VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength \
                                         - VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength \
                                         - StrSize (VariableName)
                                         );
  } else {
    TotalVariableHeaderRemainingSize = (INT32) (CSE_MAX_FILE_DATA_SIZE \
                                         - sizeof (VARIABLE_NVM_STORE_HEADER) \
                                         - VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength \
                                         - StrSize (VariableName)
                                         );
  }

  //
  // Calculate the index file offset for the new variable header and ensure there is enough space
  //
  if (UpdatingExistingVariable) {
    ASSERT ((UINTN) IndexVariable->CurrPtr > (UINTN) VariableStoreInfo.VariableStoreHeader);

    VariableIndexFileWriteOffset = NVM_HEADER_ALIGN ((UINT32) (VariableStoreBaseOffset + \
                                     ((UINTN) (IndexVariable->CurrPtr) - \
                                     (UINTN) VariableStoreInfo.VariableStoreHeader))
                                     );
  } else {
    if (TotalVariableHeaderRemainingSize < (INT32) GetVariableHeaderSize (VariableStoreInfo.AuthFlag)) {
      DEBUG ((EFI_D_ERROR, "Cannot save new variable - CSE NVM header region is at maximum capacity.\n"));
      return EFI_OUT_OF_RESOURCES;
    }
    if (VariableNvmDeletedVariablePtrTrack.CurrPtr == NULL) {
      VariableIndexFileWriteOffset = NVM_HEADER_ALIGN ((UINT32) (VariableStoreBaseOffset + \
                                       ((UINTN) GetEndPointer (VariableStoreInfo.VariableStoreHeader) - \
                                       (UINTN) VariableStoreInfo.VariableStoreHeader))
                                       );
    } else {
      VariableIndexFileWriteOffset = NVM_HEADER_ALIGN ((UINT32) (VariableStoreBaseOffset + \
                                       ((UINTN) (VariableNvmDeletedVariablePtrTrack.CurrPtr) - \
                                       (UINTN) VariableStoreInfo.VariableStoreHeader))
                                       );
    }
  }

  //
  // Construct the data file name
  //
  ZeroMem (&CseVariableDataFileName[0], CSE_MAX_NVM_FILE_NAME_LENGTH);
  if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
    //
    // The pre-memory file is a single file that serves as both the index file
    // and the data file. Therefore, the data filename is the index filename.
    //
    CopyMem (&CseVariableDataFileName[0], mCseVariableFileInfo[VariableCseFileType]->FileName, AsciiStrLen (mCseVariableFileInfo[VariableCseFileType]->FileName));
  } else {
    Status = BuildCseDataFileName (VariableDataFileNumber, &CseVariableDataFileName[0]);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  DEBUG ((EFI_D_INFO, "CSE NVM Data Filename: %a\n", &CseVariableDataFileName[0]));

  //
  // Check if the variable is being deleted
  //
  // Note: Setting a variable with no access or zero DataSize attributes
  //       causes it to be deleted.
  //
  if (UpdatingExistingVariable) {
    if (DataSize == 0 || (Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0) {
      DEBUG ((EFI_D_INFO, "Variable has been requested for delete.\n"));

      if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
        DEBUG ((EFI_D_WARN, "WARNING: Variable deletion is not supported in the pre-memory file.\n"));
        return EFI_UNSUPPORTED;
      }

      //
      // Update variable store properties
      //
      VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength -= IndexVariable->CurrPtr->DataSize;

      VariableStoreInfo.VariableStoreHeader->Size = sizeof (VARIABLE_NVM_STORE_HEADER) \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength;

      DEBUG ((EFI_D_INFO, "  CseVariableStorageProtocol: Total data length before delete = %d bytes. Data size of deleted variable = %d bytes.\n",
              VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength,
              IndexVariable->CurrPtr->DataSize
              ));
      IndexVariable->CurrPtr->State &= VAR_DELETED;

      DEBUG ((EFI_D_INFO, "  Offset written in CSE NVM index file = %d.\n", VariableIndexFileWriteOffset));

      //
      // Update the variable store header with the new sizes
      //
      Status = UpdateCseNvmFile (
                 mCseVariableFileInfo[VariableCseFileType]->FileName,
                 VariableStoreBaseOffset,
                 (UINT8 *) VariableStoreInfo.VariableStoreHeader,
                 sizeof (VARIABLE_NVM_STORE_HEADER),
                 FALSE,
                 NULL
                 );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // Update the variable header with the deleted state
      //
      Status = UpdateCseNvmFile (
                 mCseVariableFileInfo[VariableCseFileType]->FileName,
                 VariableIndexFileWriteOffset,
                 (UINT8 *) IndexVariable->CurrPtr,
                 GetVariableHeaderSize (VariableStoreInfo.AuthFlag),
                 FALSE,
                 NULL
                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Failed to put the CSE NVM variable header in the deleted state.\n"));
        return Status;
      }

      //
      // Delete the data file
      //
      Status = UpdateCseNvmFile (
                 &CseVariableDataFileName[0],
                 0,
                 (UINT8 *) &EmptyBuffer,
                 0,
                 TRUE,
                 NULL
                 );

      if (!EFI_ERROR (Status)) {
        DEBUG ((EFI_D_INFO, "Successfully deleted the CSE NVM file.\n"));
      }

      return Status;
    }
  }
  //
  // Note: Do not check if data already exists on NVM to prevent re-write as HECI I/O is the bottleneck.
  //
  if (UpdatingExistingVariable) {
    DEBUG ((EFI_D_INFO, "The variable already exists.\n"));

    if (!IsValidVariableHeader (IndexVariable->CurrPtr, IndexVariable->EndPtr)) {
      DEBUG ((EFI_D_ERROR, "The existing variable header is not valid.\n"));
      return EFI_INVALID_PARAMETER;
    }

    if (VariableStoreInfo.AuthFlag) {
      AuthenticatedVariableHeader = (AUTHENTICATED_VARIABLE_NVM_HEADER *) IndexVariable->CurrPtr;
    }
  } else {
    DEBUG ((EFI_D_INFO, "The variable does not exist yet. Creating a new variable.\n"));

    //
    // Make sure a new variable does not contain invalid attributes
    //
    if (DataSize == 0 || (Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0) {
      DEBUG ((EFI_D_ERROR, "Invalid attributes specified for new variable.\n"));
      return EFI_INVALID_PARAMETER;
    }

    NewVariableHeaderSize = (VariableNvmDeletedVariablePtrTrack.CurrPtr == NULL) ? (UINT32) GetVariableHeaderSize (VariableStoreInfo.AuthFlag) : 0;

    TotalVariableStoreRemainingSize = (mCseVariableFileInfo[VariableCseFileType]->FileStoreMaximumSize - \
                                        sizeof (VARIABLE_NVM_STORE_HEADER) - \
                                        NewVariableHeaderSize - \
                                        VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength - \
                                        VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength
                                        );

    DEBUG ((EFI_D_INFO, "Note: Total remaining size in variable store BEFORE adding the variable %d bytes of maximum %d bytes.\n",
            TotalVariableStoreRemainingSize,
            mCseVariableFileInfo[VariableCseFileType]->FileStoreMaximumSize
            ));

    if (VariableNvmDeletedVariablePtrTrack.CurrPtr == NULL) {
      NewVariableTotalSize = NewVariableHeaderSize + StrSize (VariableName) + DataSize;
    } else {
      NewVariableTotalSize = DataSize;
    }

    DEBUG ((EFI_D_INFO, "New variable total size (header + data): %d bytes.\n", NewVariableTotalSize));

    if (TotalVariableStoreRemainingSize < (INT32) NewVariableTotalSize) {
      DEBUG ((EFI_D_ERROR, "Cannot save new variable - variable would exceed the maximum store size.\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    if (VariableNvmDeletedVariablePtrTrack.CurrPtr != NULL) {
      NewVariableHeader = VariableNvmDeletedVariablePtrTrack.CurrPtr;
      VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength -= ((UINT32)GetVariableHeaderSize (VariableStoreInfo.AuthFlag) +
                                                                            NewVariableHeader->NameSize                               +
                                                                            NVM_GET_PAD_SIZE (NewVariableHeader->NameSize)
                                                                            );
      ZeroMem (
        NewVariableHeader,
        GetVariableHeaderSize (VariableStoreInfo.AuthFlag) +
          NewVariableHeader->NameSize                      +
          NVM_GET_PAD_SIZE (NewVariableHeader->NameSize)
        );
    } else {
      NewVariableHeader = GetEndPointer (VariableStoreInfo.VariableStoreHeader);
      VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalEntries++;
    }

    DEBUG ((EFI_D_INFO, "New variable header (memory) at address 0x%x\n", NewVariableHeader));

    NewVariableHeader->StartId    = VARIABLE_DATA;
    NewVariableHeader->State      = VAR_ADDED;
    NewVariableHeader->Attributes = Attributes;

    NewVariableHeader->DataSize   = (UINT32) DataSize;
    NewVariableHeader->NameSize   = (UINT32) StrSize (VariableName);

    if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
      NewVariableHeader->DataOffset = VariableDataWriteOffset;
    } else {
      NewVariableHeader->DataOffset = VariableDataFileNumber;
    }

    DEBUG ((EFI_D_INFO, "Pre-existing variable header total length = %d bytes\n", VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength));

    CopyMem (
      GetVariableNamePtr (NewVariableHeader, VariableStoreInfo.AuthFlag),
      VariableName,
      NewVariableHeader->NameSize
      );
    CopyMem (&NewVariableHeader->VendorGuid, VendorGuid, sizeof (EFI_GUID));

    if (VariableStoreInfo.AuthFlag) {
      AuthenticatedVariableHeader = (AUTHENTICATED_VARIABLE_NVM_HEADER *) NewVariableHeader;
    }
  }

  if (VariableStoreInfo.AuthFlag && AuthenticatedVariableHeader != NULL) {
    AuthenticatedVariableHeader->MonotonicCount = MonotonicCount;
    AuthenticatedVariableHeader->PubKeyIndex = KeyIndex;

    if (((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) &&
      (TimeStamp != NULL)) {
      ZeroMem (&AuthenticatedVariableHeader->TimeStamp, sizeof (EFI_TIME));

      if ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) {
        CopyMem (&AuthenticatedVariableHeader->TimeStamp, TimeStamp, sizeof (EFI_TIME));
      } else {
        //
        // In the case when the EFI_VARIABLE_APPEND_WRITE attribute is set, only
        // when the new TimeStamp value is later than the current timestamp associated
        // with the variable, we need to associate the new timestamp with the updated value.
        //
        if (VariableNvmCompareTimeStamp (&(((AUTHENTICATED_VARIABLE_HEADER *)IndexVariable->CurrPtr)->TimeStamp), TimeStamp)) {
          CopyMem (&AuthenticatedVariableHeader->TimeStamp, TimeStamp, sizeof (EFI_TIME));
        }
      }
    }
  }

  //
  // At this point, several writes are pending that will almost certainly increase the size
  // of the total variable store. This would be an opportunity to check if a reclaim operation
  // is needed based on the known size increase coming.
  //
  //
  // Primary Index File Flow:
  // ------------------------
  //   If the file exists:
  //     1. Update the variable store accounting fields for the new data size if the size has changed.
  //     2. Update the data size in the variable header for the data being passed in this function call.
  //     2. Write the data out to the existing data file (increasing or decreasing the data in the file).
  //   If the file does not exist:
  //     1. Update the variable store accounting fields for the new variable.
  //     2. Write the variable header out to the end of the existing variable store
  //     3. Write the variable data out to a new data file
  //

  if (UpdatingExistingVariable) {
    //
    // Update the variable's data size to the new size if needed
    //
    DEBUG ((EFI_D_INFO, "Checking to see if the existing variable\'s data size varies from the new data...\n"));
    if (IndexVariable->CurrPtr->DataSize != DataSize) {
      DEBUG ((EFI_D_INFO, "Need to update the size. Updating from %d bytes to %d bytes...\n", IndexVariable->CurrPtr->DataSize, DataSize));

      if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
        DEBUG ((EFI_D_WARN, "WARNING: The pre-memory file does not support updates that change variable data size!\n"));
        return EFI_UNSUPPORTED;
      }

      if (IndexVariable->CurrPtr->DataSize < DataSize) {
        VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength += (UINT32)(DataSize - IndexVariable->CurrPtr->DataSize);

      } else {
        VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength -= (UINT32)(IndexVariable->CurrPtr->DataSize - DataSize);
      }

      VariableStoreInfo.VariableStoreHeader->Size = sizeof (VARIABLE_NVM_STORE_HEADER) \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength;

      //
      // The EFI_VARIABLE_APPEND_WRITE attribute will never be set in the returned in the Attributes bitmask.
      //
      IndexVariable->CurrPtr->Attributes = Attributes & (~EFI_VARIABLE_APPEND_WRITE);

      IndexVariable->CurrPtr->DataSize   = (UINT32) DataSize;

      //
      // Update the variable header with the new data size
      //
      Status = UpdateCseNvmFile (
                 mCseVariableFileInfo[VariableCseFileType]->FileName,
                 VariableIndexFileWriteOffset,
                 (UINT8 *) IndexVariable->CurrPtr,
                 GetVariableHeaderSize (VariableStoreInfo.AuthFlag),
                 FALSE,
                 NULL
                 );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // Update the data size in the variable store header
      //
      Status = UpdateCseNvmFile (
                 mCseVariableFileInfo[VariableCseFileType]->FileName,
                 VariableStoreBaseOffset,
                 (UINT8 *) VariableStoreInfo.VariableStoreHeader,
                 sizeof (VARIABLE_NVM_STORE_HEADER),
                 FALSE,
                 NULL
                 );

      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  } else {
    //
    // Write out the new variable header and name to the variable header region
    //
    DEBUG ((EFI_D_INFO, "Writing the variable header to the header region in file since this is a new variable.\n"));

    //
    // The EFI_VARIABLE_APPEND_WRITE attribute will never be set in the returned in the Attributes bitmask.
    //
    NewVariableHeader->Attributes = Attributes & (~EFI_VARIABLE_APPEND_WRITE);

    DEBUG ((EFI_D_INFO, "Writing new variable header to offset 0x%x in the header region.\n", VariableIndexFileWriteOffset));

    Status = UpdateCseNvmFile (
               mCseVariableFileInfo[VariableCseFileType]->FileName,
               VariableIndexFileWriteOffset,
               (UINT8 *) NewVariableHeader,
               GetVariableHeaderSize (VariableStoreInfo.AuthFlag) + NewVariableHeader->NameSize,
               FALSE,
               NULL
               );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Update the variable store properties in the variable store header
    //
    if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
      VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength += (UINT32) (DataSize + VariablePaddingSize);
    } else {
      VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength += (UINT32) DataSize;
    }
    VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength += ((UINT32) GetVariableHeaderSize (VariableStoreInfo.AuthFlag) +
                                                                          NewVariableHeader->NameSize +
                                                                          NVM_GET_PAD_SIZE (NewVariableHeader->NameSize)
                                                                          );

    VariableStoreInfo.VariableStoreHeader->Size = sizeof (VARIABLE_NVM_STORE_HEADER) \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength \
                                                    + VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength;

    DEBUG ((EFI_D_INFO, "Updated the local (memory) variable store for the new variable. Current data:\n"));

    DEBUG ((EFI_D_INFO, "  Total size of variable data %d bytes\n",         VariableStoreInfo.VariableStoreHeader->VariableDataTotalLength));
    DEBUG ((EFI_D_INFO, "  Total size of all variable headers: %d bytes\n", VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalLength));
    DEBUG ((EFI_D_INFO, "  Total number of variable entries: %d\n",         VariableStoreInfo.VariableStoreHeader->VariableHeaderTotalEntries));

    DEBUG ((EFI_D_INFO, "Updating the CSE NVM store file with the update to the local variable store:\n"));

    //
    // Update the variable store header on NVM to reflect the changes
    //
    Status = UpdateCseNvmFile (
               mCseVariableFileInfo[VariableCseFileType]->FileName,
               VariableStoreBaseOffset,
               (UINT8 *) VariableStoreInfo.VariableStoreHeader,
               sizeof (VARIABLE_NVM_STORE_HEADER),
               FALSE,
               NULL
               );
  }


  DEBUG ((EFI_D_INFO, "Writing the variable data to the CSE NVM data file...\n"));

  //
  // Write out the variable data to the data file
  //
  Status = UpdateCseNvmFile (
             &CseVariableDataFileName[0],
             VariableDataWriteOffset,
             (UINT8 *) Data,
             DataSize,
             VariableCseFileType != CseVariableFileTypePreMemoryFile,
             NULL
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Retrieves a protocol instance-specific GUID.

  Returns a unique GUID per VARIABLE_STORAGE_PROTOCOL instance.

  @param[out]      VariableGuid           A pointer to an EFI_GUID that is this protocol instance's GUID.

  @retval          EFI_SUCCESS            The data was returned successfully.
  @retval          EFI_INVALID_PARAMETER  A required parameter is NULL.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetId (
  OUT       EFI_GUID                        *InstanceGuid
  )
{
  if (InstanceGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (InstanceGuid, &gCseVariableStorageProtocolInstanceGuid, sizeof (EFI_GUID));

  return EFI_SUCCESS;
}


/**
  This service retrieves a variable's value using its name and GUID.

  Read the specified variable from the CSE NVM variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  @param[in]       This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.

  @retval          EFI_SUCCESS            The variable was read successfully.
  @retval          EFI_NOT_FOUND          The variable could not be found.
  @retval          EFI_BUFFER_TOO_SMALL   The DataSize is too small for the resulting data.
                                          DataSize is updated with the size required for
                                          the specified variable.
  @retval          EFI_INVALID_PARAMETER  VariableName, VariableGuid, DataSize or Data is NULL.
  @retval          EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetVariable (
  IN CONST  VARIABLE_STORAGE_PROTOCOL  *This,
  IN CONST  CHAR16                     *VariableName,
  IN CONST  EFI_GUID                   *VariableGuid,
  OUT       UINT32                     *Attributes OPTIONAL,
  IN OUT    UINTN                      *DataSize,
  OUT       VOID                       *Data
  )
{
  //
  // Check input parameters
  //
  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((EFI_D_INFO, "Attempting to read CSE NVM variable.\n  Name=\'%ls\'\n  GUID=%g\n\n", \
    VariableName, VariableGuid));

  DEBUG ((EFI_D_INFO, "Preparing to call GetCseVariable()\n"));

  return GetCseVariable (mCseVariableFileInfo, VariableName, VariableGuid, Attributes, DataSize, Data, NULL);
}


/**
  This service retrieves an authenticated variable's value using its name and GUID.

  Read the specified authenticated variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  @param[in]       This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.
  @param[out]      KeyIndex               Index of associated public key in database
  @param[out]      MonotonicCount         Associated monotonic count value to protect against replay attack
  @param[out]      TimeStamp              Associated TimeStamp value to protect against replay attack

  @retval          EFI_SUCCESS            The variable was read successfully.
  @retval          EFI_NOT_FOUND          The variable could not be found.
  @retval          EFI_BUFFER_TOO_SMALL   The DataSize is too small for the resulting data.
                                          DataSize is updated with the size required for
                                          the specified variable.
  @retval          EFI_INVALID_PARAMETER  VariableName, VariableGuid, DataSize or Data is NULL.
  @retval          EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetAuthenticatedVariable (
  IN CONST  VARIABLE_STORAGE_PROTOCOL    *This,
  IN CONST  CHAR16                       *VariableName,
  IN CONST  EFI_GUID                     *VariableGuid,
  OUT       UINT32                       *Attributes,
  IN OUT    UINTN                        *DataSize,
  OUT       VOID                         *Data,
  OUT       UINT32                       *KeyIndex,
  OUT       UINT64                       *MonotonicCount,
  OUT       EFI_TIME                     *TimeStamp
  )
{
  EFI_STATUS                        Status;
  AUTHENTICATED_VARIABLE_NVM_HEADER *AuthenticatedVariableHeader = NULL;

  //
  // Check input parameters
  //
  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL ||
      KeyIndex == NULL || MonotonicCount == NULL || TimeStamp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((EFI_D_INFO, "Attempting to read CSE NVM authenticated variable.\n  Name=\'%ls\'\n  GUID=%g\n\n", \
    VariableName, VariableGuid));

  DEBUG ((EFI_D_INFO, "Preparing to call GetCseVariable()\n"));

  Status = GetCseVariable (mCseVariableFileInfo, VariableName, VariableGuid, Attributes, DataSize, Data, (VARIABLE_NVM_HEADER *) AuthenticatedVariableHeader);

  if (!EFI_ERROR (Status) && AuthenticatedVariableHeader != NULL) {
    *KeyIndex       = AuthenticatedVariableHeader->PubKeyIndex;
    *MonotonicCount = AuthenticatedVariableHeader->MonotonicCount;

    CopyMem (TimeStamp, &AuthenticatedVariableHeader->TimeStamp, sizeof (EFI_TIME));
  }

  return Status;
}


/**
  Return the next variable name and GUID.

  This function is called multiple times to retrieve the VariableName
  and VariableGuid of all variables currently available in the system.
  On each call, the previous results are passed into the interface,
  and, on return, the interface returns the data for the next
  interface. When the entire variable list has been returned,
  EFI_NOT_FOUND is returned.

  @param[in]      This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.

  @param[in, out] VariableNameSize       On entry, points to the size of the buffer pointed to by
                                         VariableName. On return, the size of the variable name buffer.
  @param[in, out] VariableName           On entry, a pointer to a null-terminated string that is the
                                         variable's name. On return, points to the next variable's
                                         null-terminated name string.
  @param[in, out] VariableGuid           On entry, a pointer to an EFI_GUID that is the variable's GUID.
                                         On return, a pointer to the next variable's GUID.
  @param[out]     VariableAttributes     A pointer to the variable attributes.

  @retval         EFI_SUCCESS            The variable was read successfully.
  @retval         EFI_NOT_FOUND          The variable could not be found.
  @retval         EFI_BUFFER_TOO_SMALL   The VariableNameSize is too small for the resulting
                                         data. VariableNameSize is updated with the size
                                         required for the specified variable.
  @retval         EFI_INVALID_PARAMETER  VariableName, VariableGuid or
                                         VariableNameSize is NULL.
  @retval         EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetNextVariableName (
  IN CONST  VARIABLE_STORAGE_PROTOCOL       *This,
  IN OUT    UINTN                           *VariableNameSize,
  IN OUT    CHAR16                          *VariableName,
  IN OUT    EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *VariableAttributes
  )
{
  EFI_STATUS           Status;
  BOOLEAN              IsAuthVariable;
  UINTN                VarNameSize;
  VARIABLE_NVM_HEADER  *VariablePtr;

  if (VariableNameSize == NULL || VariableName == NULL || VariableGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetNextCseVariableName (VariableName, VariableGuid, mCseVariableFileInfo, &VariablePtr, &IsAuthVariable);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarNameSize = NameSizeOfVariable (VariablePtr, IsAuthVariable);
  ASSERT (VarNameSize != 0);

  if (VarNameSize <= *VariableNameSize) {
    CopyMem (VariableName, GetVariableNamePtr (VariablePtr, IsAuthVariable), VarNameSize);
    CopyMem (VariableGuid, GetVendorGuidPtr (VariablePtr, IsAuthVariable), sizeof (EFI_GUID));
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *VariableNameSize = VarNameSize;
  *VariableAttributes = VariablePtr->Attributes;

  return Status;
}


/**
  Returns information on the amount of space available in the variable store. If the amount of data that can be written
  depends on if the platform is in Pre-OS stage or OS stage, the AtRuntime parameter should be used to compute usage.

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  AtRuntime                      TRUE is the platform is in OS Runtime, FALSE if still in Pre-OS stage
  @param[out] VariableStoreSize              The total size of the NV storage. Indicates the maximum amount
                                             of data that can be stored in this NV storage area.
  @param[out] CommonVariablesTotalSize       The total combined size of all the common UEFI variables that are
                                             stored in this NV storage area. Excludes variables with the
                                             EFI_VARIABLE_HARDWARE_ERROR_RECORD attribute set.
  @param[out] HwErrVariablesTotalSize        The total combined size of all the UEFI variables that have the
                                             EFI_VARIABLE_HARDWARE_ERROR_RECORD attribute set and which are
                                             stored in this NV storage area. Excludes all other variables.

  @retval     EFI_INVALID_PARAMETER          Any of the given parameters are NULL
  @retval     EFI_SUCCESS                    Space information returned successfully.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetStorageUsage (
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  IN          BOOLEAN                     AtRuntime,
  OUT         UINT32                      *VariableStoreSize,
  OUT         UINT32                      *CommonVariablesTotalSize,
  OUT         UINT32                      *HwErrVariablesTotalSize
  )
{
  BOOLEAN                    IsAuthVariable;
  CSE_VARIABLE_FILE_TYPE     Type;
  EFI_GUID                   Guid;
  EFI_STATUS                 Status;
  UINT32                     Attributes;
  VARIABLE_NVM_POINTER_TRACK VariablePtrTrack;
  CHAR16  *VariableName         = NULL;
  UINTN   VariableNameSize      = 0;
  UINTN   NewVariableNameSize   = 0;
  UINTN  CseVariableTotalStoreSizeInternal   = 0;
  UINTN  CseCommonVariablesTotalSizeInternal = 0;
  UINTN  CseHwErrVariablesTotalSizeInternal  = 0;

  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    if (mCseVariableFileInfo[Type] != NULL && mCseVariableFileInfo[Type]->FileStoreEstablished) {
      CseVariableTotalStoreSizeInternal += (mCseVariableFileInfo[Type]->FileStoreMaximumSize - sizeof (VARIABLE_NVM_STORE_HEADER));
      DEBUG ((EFI_D_INFO, "Current store maximum size = %d bytes.\n", mCseVariableFileInfo[Type]->FileStoreMaximumSize));
    }

  }
  ASSERT (CseVariableTotalStoreSizeInternal > 0);

  VariableNameSize = sizeof (CHAR16);
  VariableName     = AllocateZeroPool (VariableNameSize);
  if (VariableName == NULL) {
    ASSERT (VariableName != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  while (TRUE) {
    NewVariableNameSize = VariableNameSize;
    Status = CseVariableStorageGetNextVariableName (This, &NewVariableNameSize, VariableName, &Guid, &Attributes);

    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool (VariableNameSize, NewVariableNameSize, VariableName);
      if (VariableName == NULL) {
        ASSERT (VariableName != NULL);
        return EFI_OUT_OF_RESOURCES;
      }
      Status = CseVariableStorageGetNextVariableName (This, &NewVariableNameSize, VariableName, &Guid, &Attributes);
      VariableNameSize = NewVariableNameSize;
    }

    if (Status == EFI_NOT_FOUND) {
      break;
    }
    ASSERT_EFI_ERROR (Status);

    //
    // Get the variable header for this variable
    //
    Status = FindVariable (VariableName, &Guid, mCseVariableFileInfo, &VariablePtrTrack);

    if (EFI_ERROR (Status) || VariablePtrTrack.CurrPtr == NULL) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    Type = GetCseVariableStoreFileType (VariableName, &Guid, mCseVariableFileInfo);

    Status = IsAuthenticatedVariableStore ((VARIABLE_NVM_STORE_HEADER *) (UINTN) mCseVariableFileInfo[Type]->HeaderRegionBase, &IsAuthVariable);

    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    //
    // When the reclaim operation is implemented, need to do a runtime check.
    // At runtime, don't worry about the variable state. At boot time, the
    // state needs to be taken into account because the variables can be reclaimed.
    //
    if ((VariablePtrTrack.CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
      CseHwErrVariablesTotalSizeInternal += GetVariableHeaderSize (IsAuthVariable);
      CseHwErrVariablesTotalSizeInternal += VariablePtrTrack.CurrPtr->NameSize;
      CseHwErrVariablesTotalSizeInternal += VariablePtrTrack.CurrPtr->DataSize;
    } else {
      CseCommonVariablesTotalSizeInternal += GetVariableHeaderSize (IsAuthVariable);
      CseCommonVariablesTotalSizeInternal += VariablePtrTrack.CurrPtr->NameSize;
      CseCommonVariablesTotalSizeInternal += VariablePtrTrack.CurrPtr->DataSize;
    }
  }

  FreePool (VariableName);

  *VariableStoreSize        = (UINT32) CseVariableTotalStoreSizeInternal;
  *CommonVariablesTotalSize = (UINT32) CseCommonVariablesTotalSizeInternal;
  *HwErrVariablesTotalSize  = (UINT32) CseHwErrVariablesTotalSizeInternal;

  return EFI_SUCCESS;
}


/**
  Returns whether this NV storage area supports storing authenticated variables or not

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[out] AuthSupported                  TRUE if this NV storage area can store authenticated variables,
                                             FALSE otherwise

  @retval     EFI_SUCCESS                    AuthSupported was returned successfully.

**/
EFI_STATUS
EFIAPI
CseVariableStorageGetAuthenticatedSupport (
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  OUT         BOOLEAN                     *AuthSupported
  )
{
  *AuthSupported = TRUE;

  return EFI_SUCCESS;
}


/**
  This code sets a variable's value using its name and GUID.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parsing the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in]  This                             A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  VariableName                     Name of Variable to be found.
  @param[in]  VendorGuid                       Variable vendor GUID.
  @param[in]  Attributes                       Attribute value of the variable found
  @param[in]  DataSize                         Size of Data found. If size is less than the
                                               data, this value contains the required size.
  @param[in]  Data                             Data pointer.
  @param[in]  AtRuntime                        TRUE is the platform is in OS Runtime, FALSE if still in Pre-OS stage
  @param[in]  KeyIndex                         If writing an authenticated variable, the public key index
  @param[in]  MonotonicCount                   If writing a monotonic counter authenticated variable, the counter value
  @param[in]  TimeStamp                        If writing a timestamp authenticated variable, the timestamp value

  @retval     EFI_INVALID_PARAMETER            Invalid parameter.
  @retval     EFI_SUCCESS                      Set successfully.
  @retval     EFI_OUT_OF_RESOURCES             Resource not enough to set variable.
  @retval     EFI_NOT_FOUND                    Not found.
  @retval     EFI_WRITE_PROTECTED              Variable is read-only.

**/
EFI_STATUS
EFIAPI
CseVariableStorageSetVariable (
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  IN          CHAR16                      *VariableName,
  IN          EFI_GUID                    *VendorGuid,
  IN          UINT32                      Attributes,
  IN          UINTN                       DataSize,
  IN          VOID                        *Data,
  IN          BOOLEAN                     AtRuntime,
  IN          UINT32                      KeyIndex       OPTIONAL,
  IN          UINT64                      MonotonicCount OPTIONAL,
  IN          EFI_TIME                    *TimeStamp     OPTIONAL
  )
{
  EFI_STATUS                  Status;
  VARIABLE_NVM_POINTER_TRACK  VariablePtrTrack;

  //
  // Check input parameters.
  //
  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (((Attributes & EFI_VARIABLE_NON_VOLATILE) != EFI_VARIABLE_NON_VOLATILE) && (DataSize != 0)) {
    DEBUG ((EFI_D_ERROR, "A volatile variable was passed to the CSE storage protocol.\n"));
    return EFI_INVALID_PARAMETER;
  }


  if (AtRuntime && mIsEmmcBoot) {
    DEBUG ((EFI_D_ERROR, "Performing workaround for eMMC boot issue\n"));
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "Attempting to set CSE NVM variable.\n  Name=\'%ls\'\n  GUID=%g\n  Attributes=0x%x\n\n", \
                      VariableName, *VendorGuid, Attributes));

  if (PcdGet32 (PcdMaxVariableSize) - sizeof (VARIABLE_NVM_HEADER) <= StrSize (VariableName) + DataSize) {
    DEBUG ((EFI_D_ERROR, "The variable being written is larger than the maximum specified variable size.\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if the variable already exists
  //
  Status = FindVariable (VariableName, VendorGuid, mCseVariableFileInfo, &VariablePtrTrack);
  if (!EFI_ERROR (Status)) {
    if (Attributes != 0 && (Attributes & (~EFI_VARIABLE_APPEND_WRITE)) != VariablePtrTrack.CurrPtr->Attributes) {
      DEBUG ((EFI_D_ERROR, "Error: Attributes given do not match the existing variable.\n"));
      DEBUG ((EFI_D_ERROR, "       Attributes given = 0x%x. Existing attributes = 0x%x.\n", Attributes, VariablePtrTrack.CurrPtr->Attributes));
      return EFI_INVALID_PARAMETER;
    }
  } else if (Status != EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "Error occurred finding the variable.\n"));
    return Status;
  }

  DEBUG ((EFI_D_INFO, "VariablePtrTrack->CurrPtr is null = %c\n", (VariablePtrTrack.CurrPtr == NULL ? 'T' : 'F')));

  Status = UpdateVariable (VariableName, VendorGuid, Data, DataSize, Attributes, &VariablePtrTrack, KeyIndex, MonotonicCount, TimeStamp);
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "The variable was updated successfully.\n"));
  }

  return Status;
}


/**
  Returns whether this NV storage area is ready to accept calls to SetVariable() or not

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.

  @retval     TRUE                           The NV storage area is ready to accept calls to SetVariable()
  @retval     FALSE                          The NV storage area is not ready to accept calls to SetVariable()

**/
BOOLEAN
EFIAPI
CseVariableStorageWriteServiceIsReady (
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This
  )
{
  //
  // CSE should be ready to service write commands when this driver is loaded
  //
  return TRUE;
}


/**
  Sets the callback to be invoked when the VARIABLE_STORAGE_PROTOCOL is ready to accept calls to SetVariable()

  The VARIABLE_STORAGE_PROTOCOL is required to invoke the callback as quickly as possible after the core
  variable driver invokes RegisterWriteServiceReadyCallback() to set the callback.

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  CallbackFunction               The callback function

  @retval     EFI_SUCCESS                    The callback function was sucessfully registered

**/
EFI_STATUS
EFIAPI
CseVariableStorageRegisterWriteServiceReadyCallback (
  IN CONST    VARIABLE_STORAGE_PROTOCOL             *This,
  IN VARIABLE_STORAGE_WRITE_SERVICE_READY_CALLBACK  CallbackFunction
  )
{
  //
  // CSE should be ready to service write commands when this driver is loaded
  // immediately invoke the callback function
  //
  return CallbackFunction ();
}

