/** @file
  CSE Variable Storage Library.

  Copyright (c) 2016-2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CseVariableStorageLib.h>
#include <Library/CseVariableStorageSelectorLib.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Pi/PiMultiPhase.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>  // Temporary
#include <Library/VariableNvmStorageLib.h>
#include <Protocol/Heci.h>


/**
  Creates variable stores if in DXE and necessary. Loads the variable data region
  from CSE NVM needed to find variables.

  @param[in, out]  CseVariableFileInfo   A pointer to a structure that contains information the CSE files available.
  @param[in]       AtDxeRuntime          Determines how this function should allocate memory.
                                         Caller passes TRUE if in DXE and FALSE for pre-DXE (PEI).

  @retval          EFI_SUCCESS           The variable store was established successfully.
  @retval          Others                The variable store could not be established.

**/
EFI_STATUS
EFIAPI
EstablishAndLoadCseVariableStores (
  IN OUT CSE_VARIABLE_FILE_INFO   **CseVariableFileInfo,
  IN     BOOLEAN                  AtDxeRuntime
  )
{
  EFI_STATUS                 Status;
  CSE_VARIABLE_FILE_TYPE     Type;
  UINTN                      VariableStoreSize               = 0;
  UINTN                      VariableHeaderRegionSize        = 0;
  UINTN                      VariableHeaderRegionBufferSize  = 0;
  UINT32                     CseFileVariableBaseFileOffset   = 0;
  EFI_HOB_GUID_TYPE          *IndexFileDataHob                  = NULL;
  UINT8                      *VariableHeaderRegion              = NULL;
  UINT8                      *VariableHeaderRegionIndexHobData  = NULL;
  VARIABLE_NVM_STORE_HEADER  VariableStoreHeader;

  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    ASSERT (CseVariableFileInfo[Type] != NULL);

    if (CseVariableFileInfo[Type]->FileEnabled && !CseVariableFileInfo[Type]->FileStoreEstablished) {
      Status = GetCseVariableStoreFileOffset (Type, &CseFileVariableBaseFileOffset);

      ASSERT_EFI_ERROR (Status);

      //
      // Attempt to load the index file data from its HOB if available
      //
      IndexFileDataHob = GetFirstGuidHob (CseVariableFileInfo[Type]->IndexFileDataGuid);

      if (IndexFileDataHob != NULL && !AtDxeRuntime) {
        CseVariableFileInfo[Type]->HeaderRegionBase = (EFI_PHYSICAL_ADDRESS) (UINTN) GET_GUID_HOB_DATA (IndexFileDataHob);
        continue;
      }

      if (AtDxeRuntime) {
        if (Type == CseVariableFileTypePreMemoryFile) {
          VariableHeaderRegionBufferSize = CseVariableFileInfo[Type]->FileStoreMaximumSize;
        } else {
          VariableHeaderRegionBufferSize = CSE_MAX_FILE_DATA_SIZE;
        }

        if (IndexFileDataHob == NULL) {
          //
          // Writes an empty variable store to CSE NVM if it does not already exist
          // Returns the size of the CSE NVM variable store
          //
          Status = CreateCseNvmVariableStore (
                     CseVariableFileInfo[Type]->FileName,
                     CseFileVariableBaseFileOffset,
                     &VariableHeaderRegionSize
                     );

          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "Error creating CSE variable store %d\n", (UINT32) Type));
            ASSERT_EFI_ERROR (Status);
            return Status;
          } else {
            DEBUG ((EFI_D_INFO, "Successfully created CSE variable store %d\n", (UINT32) Type));
          }
        } else {
          VariableHeaderRegionSize = GET_GUID_HOB_DATA_SIZE (IndexFileDataHob);
        }
      } else {
        //
        // Check if the variable store exists
        //
        Status = HeciGetNVMFileSize ((UINT8 *)(CseVariableFileInfo[Type]->FileName), &VariableHeaderRegionBufferSize);

        //
        // If there's an error finding the file, do not establish this store as
        // variable stores cannot be created in PEI.
        //
        if (EFI_ERROR (Status) || VariableHeaderRegionBufferSize > CseVariableFileInfo[Type]->FileStoreMaximumSize) {
          continue;
        }
        ASSERT (VariableHeaderRegionBufferSize > 0);

        VariableHeaderRegionSize = VariableHeaderRegionBufferSize;
      }

      if (Type == CseVariableFileTypePreMemoryFile) {
        VariableStoreSize = sizeof (VARIABLE_NVM_STORE_HEADER);

        //
        // Read the variable store header to determine the total variable header region size
        //
        Status = ReadCseNvmFile (
                   CseVariableFileInfo[Type]->FileName,
                   CseFileVariableBaseFileOffset,
                   (UINT8 *) &VariableStoreHeader,
                   &VariableStoreSize,
                   NULL
                   );

        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Error reading the variable header region for CSE variable store %d.\n", (UINT32) Type));
          ASSERT_EFI_ERROR (Status);
          return Status;
        }

        VariableHeaderRegionSize = sizeof (VARIABLE_NVM_STORE_HEADER) \
                                     + VariableStoreHeader.VariableHeaderTotalLength \
                                     + NVM_GET_PAD_SIZE (VariableStoreHeader.VariableHeaderTotalLength);

        if (!AtDxeRuntime) {
          VariableHeaderRegionBufferSize = VariableHeaderRegionSize;
        }
      }
      ASSERT (
        VariableHeaderRegionSize >= sizeof (VARIABLE_NVM_STORE_HEADER) \
        && VariableHeaderRegionSize <= CseVariableFileInfo[Type]->FileStoreMaximumSize
        );

      if (AtDxeRuntime) {
        VariableHeaderRegion = AllocateRuntimePool (VariableHeaderRegionBufferSize);
      } else {
        VariableHeaderRegion = BuildGuidHob (CseVariableFileInfo[Type]->IndexFileDataGuid, VariableHeaderRegionBufferSize);
      }

      if (VariableHeaderRegion == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      DEBUG ((EFI_D_INFO, "Total buffer size for store %d is %d bytes.\n", (UINT32) Type, VariableHeaderRegionBufferSize));

      ZeroMem (VariableHeaderRegion, VariableHeaderRegionBufferSize);

      if (IndexFileDataHob == NULL) {
        //
        // Load the variable header region for this variable store
        //
        Status = ReadCseNvmFile (
                   CseVariableFileInfo[Type]->FileName,
                   CseFileVariableBaseFileOffset,
                   (UINT8 *) VariableHeaderRegion,
                   &VariableHeaderRegionSize,
                   NULL
                   );

        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Error reading the variable header region for CSE variable store %d.\n", (UINT32) Type));

          if (AtDxeRuntime) {
            FreePool (VariableHeaderRegion);
          }

          ASSERT_EFI_ERROR (Status);
          return Status;
        }
      } else {
        VariableHeaderRegionIndexHobData = GET_GUID_HOB_DATA (IndexFileDataHob);

        if (VariableHeaderRegionIndexHobData == NULL) {
          ASSERT (VariableHeaderRegionIndexHobData != NULL);
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (VariableHeaderRegion, VariableHeaderRegionIndexHobData, VariableHeaderRegionSize);
      }

      CseVariableFileInfo[Type]->HeaderRegionBase = (EFI_PHYSICAL_ADDRESS) (UINTN) VariableHeaderRegion;

      DEBUG ((EFI_D_INFO, "Setting file store %d to established. Variable header region base at 0x%x.\n", (UINT32) Type, CseVariableFileInfo[Type]->HeaderRegionBase));

      CseVariableFileInfo[Type]->FileStoreEstablished = TRUE;
    }
  }

  return EFI_SUCCESS;
}


/**
  Initializes the global variable structures for CSE storage modules.

  @param[in,out]  CseVariableFileInfo   A pointer to a structure that contains information the CSE files available.
  @param[in]      AtDxeRuntime          Determines how this function should allocate memory.
                                        Caller passes TRUE if in DXE and FALSE for pre-DXE (PEI).

  @retval         EFI_SUCCESS           The global variables were initialized successfully.
  @retval         EFI_OUT_OF_RESOURCES  Not enough memory resources needed for allocation were available.

**/
EFI_STATUS
EFIAPI
InitializeCseStorageGlobalVariableStructures (
  IN OUT CSE_VARIABLE_FILE_INFO   **CseVariableFileInfo,
  IN     BOOLEAN                  AtDxeRuntime
  )
{
  CHAR8                   FileNamePtr[CSE_MAX_NVM_FILE_NAME_LENGTH];
  CHAR16                  *FileNamePtrUni;
  UINTN                   FileNameSize;
  CSE_VARIABLE_FILE_TYPE  Type;

  FileNamePtrUni = NULL;

  ZeroMem (FileNamePtr, sizeof (FileNamePtr));

  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    if (AtDxeRuntime) {
      CseVariableFileInfo[Type] = AllocateRuntimeZeroPool (sizeof (CSE_VARIABLE_FILE_INFO));
    } else {
      CseVariableFileInfo[Type] = AllocateZeroPool (sizeof (CSE_VARIABLE_FILE_INFO));
    }

    if (CseVariableFileInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Set file enable defaults
  //
  CseVariableFileInfo[CseVariableFileTypePreMemoryFile]->FileEnabled = FALSE;
  CseVariableFileInfo[CseVariableFileTypePrimaryIndexFile]->FileEnabled = TRUE;

  //
  // Populate the remaining fields in the global variable structure
  //
  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    ASSERT (CseVariableFileInfo[Type] != NULL);

    CseVariableFileInfo[Type]->FileName             = NULL;
    CseVariableFileInfo[Type]->HeaderRegionBase     = 0;
    CseVariableFileInfo[Type]->FileStoreEstablished = FALSE;

    if (Type == CseVariableFileTypePreMemoryFile) {
      CseVariableFileInfo[Type]->IndexFileDataGuid = &gCsePreMemoryIndexFileDataHobGuid;
      FileNamePtrUni = (CHAR16 *)(VOID *)PcdGetPtr (PcdEnableCseMrcTrainingDataVariables);

      //
      // A pre-memory variable store file is not required
      //
      if (FileNamePtrUni == NULL || FileNamePtrUni[0] == '\0') {
        DEBUG ((EFI_D_INFO, "The pre-memory file pointer is null - Do not use the pre-memory file.\n"));
        continue;
      }

      CseVariableFileInfo[Type]->FileStoreMaximumSize = CSE_PRE_MEMORY_FILE_MAX_STORE_SIZE;
      CseVariableFileInfo[Type]->FileEnabled = TRUE;
    } else if ((CSE_VARIABLE_FILE_TYPE) Type == CseVariableFileTypePrimaryIndexFile) {
      CseVariableFileInfo[Type]->IndexFileDataGuid = &gCsePrimaryIndexFileDataHobGuid;
      FileNamePtrUni = (CHAR16 *) (VOID *) PcdGetPtr (PcdPrimaryCseNvmStoreIndexFileName);
      CseVariableFileInfo[Type]->FileStoreMaximumSize = CSE_PRIMARY_NVM_MAX_STORE_SIZE;
    }

    ASSERT (FileNamePtrUni != NULL);

    UnicodeStrToAsciiStr (FileNamePtrUni, FileNamePtr);
    FileNameSize = AsciiStrSize (FileNamePtr);
    ASSERT (FileNameSize <= CSE_MAX_NVM_FILE_NAME_LENGTH);

    if (AtDxeRuntime) {
      CseVariableFileInfo[Type]->FileName = AllocateRuntimeCopyPool (FileNameSize, FileNamePtr);
    } else {
      CseVariableFileInfo[Type]->FileName = AllocateCopyPool (FileNameSize, FileNamePtr);
    }

    if (CseVariableFileInfo[Type]->FileName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    DEBUG ((EFI_D_INFO, "Finished initializing global variable structure for CSE file type %d. File name = %a File name size = %d\n", (UINT32)Type, CseVariableFileInfo[Type]->FileName, FileNameSize));
  }

  return EFI_SUCCESS;
}


/**
  Reads data from a CSE NVM file into a specified memory buffer.

  @param[in]  CseFileName            The name of the CSE file used in the read operation.
  @param[in]  FileOffset             The offset to begin reading in the CSE file.
  @param[in]  Data                   A pointer to a buffer that is the destination for the read.
  @param[in]  DataSize               The length in bytes of data to read from the file.
  @param[in]  HeciProtocolSelector   The HECI protocol to use to write the file.
                                     If not specified, the protocol will be determined internally.

  @retval     EFI_SUCCESS            The file was read successfully.
  @retval     Others                 An error occurred reading the file.

**/
EFI_STATUS
EFIAPI
ReadCseNvmFile (
  IN      CHAR8                       *CseFileName,
  IN      UINT32                      FileOffset,
  OUT     UINT8                       *Data,
  OUT     UINTN                       *DataSize,
  IN      CSE_VARIABLE_HECI_PROTOCOL  *HeciProtocolSelector  OPTIONAL
  )
{
  CSE_VARIABLE_HECI_PROTOCOL  HeciProtocolActive;
  EFI_HECI_PROTOCOL           *Heci2Protocol = NULL;

  DEBUG ((EFI_D_INFO, "+++> Reading CSE NVM file.\n  Name = %a\n  Offset = %d\n\n", CseFileName, FileOffset));

  if (CseFileName == NULL || AsciiStrnLenS (CseFileName, (UINTN) CSE_MAX_NVM_FILE_NAME_LENGTH) == (UINTN) CSE_MAX_NVM_FILE_NAME_LENGTH) {
    DEBUG ((EFI_D_ERROR, "Invalid parameter passed, cannot read the CSE file.\n"));
    return EFI_INVALID_PARAMETER;
  }

  HeciProtocolActive = GetCseHeciProtocol (&Heci2Protocol);

  if (HeciProtocolSelector != NULL) {
    HeciProtocolActive = *HeciProtocolSelector;
  }

  if (HeciProtocolActive == CseVariableHeci1Protocol) {
    return HeciReadNVMFile ((UINT8 *)CseFileName, FileOffset, Data, DataSize);
  } else if (Heci2Protocol != NULL) {
    return Heci2ReadNVMFile ((UINT8 *)CseFileName, FileOffset, Data, DataSize, Heci2Protocol);
  } else {
    ASSERT (FALSE);
  }

  return EFI_NOT_READY;
}


/**
  Writes data in a memory buffer to a specified CSE NVM file.

  @param[in]  CseFileName            The name of the CSE file used in the write operation.
  @param[in]  FileOffset             The offset to begin writing in the CSE file.
  @param[in]  Data                   A pointer to a buffer of data to write to the file.
  @param[in]  DataSize               The length in bytes of data to write to the file.
  @param[in]  HeciProtocolSelector   The HECI protocol to use to write the file.
                                     If not specified, the protocol will be determined internally.
  @param[in]  Truncate               If true, the file will be truncated.

  @retval     EFI_SUCCESS            The file was updated successfully.
  @retval     Others                 An error occurred updating the file.

**/

EFI_STATUS
EFIAPI
UpdateCseNvmFile (
  IN      CHAR8                       *CseFileName,
  IN      UINT32                      FileOffset,
  IN      UINT8                       *Data,
  IN      UINTN                       DataSize,
  IN      BOOLEAN                     Truncate,
  IN      CSE_VARIABLE_HECI_PROTOCOL  *HeciProtocolSelector  OPTIONAL
  )
{
  CSE_VARIABLE_HECI_PROTOCOL   HeciProtocolActive;
  EFI_HECI_PROTOCOL            *Heci2Protocol = NULL;
  EFI_STATUS                   Status;

  DEBUG ((EFI_D_INFO, "+++> Updating CSE NVM file.\n  Name = %a\n  Offset = %d\n  DataSize = %d\n\n", CseFileName, FileOffset, DataSize));

  if (CseFileName == NULL ||
    Data == NULL ||
    AsciiStrnLenS (CseFileName, (UINTN) CSE_MAX_NVM_FILE_NAME_LENGTH) == (UINTN) CSE_MAX_NVM_FILE_NAME_LENGTH) {
    DEBUG ((EFI_D_ERROR, "Invalid parameter passed, cannot write to the CSE file.\n"));
    return EFI_INVALID_PARAMETER;
  }

  HeciProtocolActive = GetCseHeciProtocol (&Heci2Protocol);

  if (HeciProtocolSelector != NULL) {
    HeciProtocolActive = *HeciProtocolSelector;
  }

  if (HeciProtocolActive == CseVariableHeci1Protocol) {
    return HeciWriteNVMFile ((UINT8 *)CseFileName, FileOffset, Data, DataSize, Truncate);
  } else if (Heci2Protocol != NULL) {
    Status = Heci2WriteNVMFile ((UINT8 *)CseFileName, FileOffset, Data, DataSize, Truncate);
    return Status;
  } else {
    ASSERT (FALSE);
  }

  return EFI_NOT_READY;
}


/**
  Creates an empty variable store on the CSE NVM file specified.

  @param[in]   StoreFileName   A pointer to the string representing the CSE file name containing this store.
  @param[in]   Offset          The offset from the beginning of the CSE NVM file to the first byte of the store.
  @param[out]  StoreSize       The size of the variable store created / found will be written to this pointer.

  @retval      EFI_SUCCESS     The variable store was created successfully.
  @retval      Others          The variable store could not be created.

**/
EFI_STATUS
EFIAPI
CreateCseNvmVariableStore (
  IN  CHAR8     *StoreFileName,
  IN  UINT32    Offset,
  OUT UINTN     *StoreSize
  )
{
  VARIABLE_NVM_STORE_HEADER  *VariableStoreHeader;
  EFI_STATUS                 Status;

  VariableStoreHeader = NULL;

  if (StoreFileName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if a variable store already exists
  //
  Status = HeciGetNVMFileSize ((UINT8 *)StoreFileName, StoreSize);
  if (Status == EFI_SUCCESS || (Status != EFI_NOT_FOUND && EFI_ERROR (Status))) {
    return Status;
  }
  DEBUG ((EFI_D_INFO, "Could not find an existing variable store. Attempting to create a new store...\n"));

  //
  // Allocate memory for a new variable store
  //
  VariableStoreHeader = AllocateZeroPool (sizeof (VARIABLE_NVM_STORE_HEADER));
  if (VariableStoreHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get an empty variable store
  //
  Status = CreateEmptyVariableStore (VariableStoreHeader);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error creating an empty variable store.\n"));
    FreePool (VariableStoreHeader);
    return Status;
  }

  *StoreSize = (UINTN) VariableStoreHeader->Size;

  //
  // Write the variable store to the base of the store location
  //
  Status = UpdateCseNvmFile (StoreFileName, Offset, (UINT8 *) VariableStoreHeader, *StoreSize, FALSE, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error writing the new variable store to the CSE NVM file.\n"));
    FreePool (VariableStoreHeader);
    return Status;
  }

  FreePool (VariableStoreHeader);

  DEBUG ((EFI_D_INFO, "Successfully wrote the new variable store to the CSE NVM file.\n"));

  return EFI_SUCCESS;
}


/**
  Returns the file name for a CSE data file at the specified offset (file number).

  @param[in]   DataOffset               The file number on CSE NVM (each data file is an "offset")
  @param[out]  CseVariableDataFileName  The CSE file name.

  @retval      EFI_INVALID_PARAMETER    A valid buffer was not passed to fill with the file name.
  @retval      EFI_SUCCESS              File name was successfully created and copied to the buffer.

**/
EFI_STATUS
EFIAPI
BuildCseDataFileName (
  IN  UINT32     DataOffset,
  OUT CHAR8      *CseVariableDataFileName
  )
{
  UINTN    CseVariableDataFileNamePrefixLength;
  UINTN    CseVariableDataFileNumberStrLength;
  CHAR8    CseVariableDataFileNumberStrBuffer[8] = "";

  if (CseVariableDataFileName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CseVariableDataFileNumberStrLength = AsciiSPrint (CseVariableDataFileNumberStrBuffer, sizeof (CseVariableDataFileNumberStrBuffer), "%03d", DataOffset);

  //
  // Form the name of the CSE data file
  //
  CseVariableDataFileNamePrefixLength = AsciiStrnLenS (CSE_PRIMARY_NVM_DATA_FILE_PREFIX, sizeof (CSE_PRIMARY_NVM_DATA_FILE_PREFIX));

  CopyMem (CseVariableDataFileName, CSE_PRIMARY_NVM_DATA_FILE_PREFIX, CseVariableDataFileNamePrefixLength);
  CopyMem (CseVariableDataFileName + CseVariableDataFileNamePrefixLength, CseVariableDataFileNumberStrBuffer, CseVariableDataFileNumberStrLength + 1);

  return EFI_SUCCESS;
}


/**
  Finds deleted variables in the specified CSE NVM variable store.

  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]      VariableName            Name of the variable to be found.
  @param[in]      VendorGuid              Vendor GUID to be found.
  @param[in]      VariableStore           Pointer to a VARIABLE_NVM_STORE_HEADER structure which
                                          is the variable store to search.
  @param[in,out]  VariableTrack Ptr       Pointer to a variable NVM tracking structure which points
                                          current pointer to the variable if found.

  @retval         EFI_INVALID_PARAMETER   If VariableName is not an empty string, while
                                          VendorGuid is NULL.
  @retval         EFI_SUCCESS             Variable successfully found.
  @retval         EFI_NOT_FOUND           Variable not found

**/
EFI_STATUS
EFIAPI
FindDeletedVariable (
  IN  CONST  CHAR16                     *VariableName,
  IN  CONST  EFI_GUID                   *VendorGuid,
  IN         VARIABLE_NVM_STORE_HEADER  *VariableStorePtr,
  IN OUT     VARIABLE_NVM_POINTER_TRACK *VariablePtrTrack
  )
{
  EFI_STATUS               Status;
  VARIABLE_NVM_STORE_INFO  VariableNvmStoreInfo;
  VOID                     *VariableCandidateName;

  VariablePtrTrack->InDeletedTransitionPtr = NULL;

  Status = IsAuthenticatedVariableStore (VariableStorePtr, &VariableNvmStoreInfo.AuthFlag);

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableNvmStoreInfo.VariableStoreHeader = VariableStorePtr;

  //
  // Find the variable by walking through the specified variable store
  //
  for (VariablePtrTrack->CurrPtr = VariablePtrTrack->StartPtr;
       IsValidVariableHeader (VariablePtrTrack->CurrPtr, VariablePtrTrack->EndPtr);
       VariablePtrTrack->CurrPtr = GetNextVariablePtr (&VariableNvmStoreInfo, VariablePtrTrack->CurrPtr)
       ) {
    if (VariablePtrTrack->CurrPtr->State != VAR_ADDED && VariablePtrTrack->CurrPtr->State != (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {

      //
      // A variable name was given to find
      //
      if (CompareGuid (VendorGuid, &VariablePtrTrack->CurrPtr->VendorGuid)) {
        VariableCandidateName = (VOID *)GetVariableNamePtr (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag);

        ASSERT (NameSizeOfVariable (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag) != 0);
        if (CompareMem (
              VariableName,
              VariableCandidateName,
              NameSizeOfVariable (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag)
              ) == 0) {
          DEBUG ((EFI_D_INFO, "Returning with variable found at header address 0x%x\n", VariablePtrTrack->CurrPtr));
          return EFI_SUCCESS;
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Finds the variable in the specified CSE NVM variable store.

  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]      VariableName            Name of the variable to be found.
  @param[in]      VendorGuid              Vendor GUID to be found.
  @param[in]      VariableStore           Pointer to a VARIABLE_NVM_STORE_HEADER structure which
                                          is the variable store to search.
  @param[in,out]  VariableTrack Ptr       Pointer to a variable NVM tracking structure which points
                                          current pointer to the variable if found.

  @retval         EFI_INVALID_PARAMETER   If VariableName is not an empty string, while
                                          VendorGuid is NULL.
  @retval         EFI_SUCCESS             Variable successfully found.
  @retval         EFI_NOT_FOUND           Variable not found

**/
EFI_STATUS
EFIAPI
FindVariableEx (
  IN  CONST  CHAR16                     *VariableName,
  IN  CONST  EFI_GUID                   *VendorGuid,
  IN         VARIABLE_NVM_STORE_HEADER  *VariableStorePtr,
  IN OUT     VARIABLE_NVM_POINTER_TRACK *VariablePtrTrack
  )
{
  EFI_STATUS               Status;
  VARIABLE_NVM_HEADER      *InDeletedTransitionVariable;
  VARIABLE_NVM_STORE_INFO  VariableNvmStoreInfo;
  VOID                     *VariableCandidateName;

  InDeletedTransitionVariable              = NULL;
  VariablePtrTrack->InDeletedTransitionPtr = NULL;

  Status = IsAuthenticatedVariableStore (VariableStorePtr, &VariableNvmStoreInfo.AuthFlag);

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableNvmStoreInfo.VariableStoreHeader = VariableStorePtr;

  //
  // Find the variable by walking through the specified variable store
  //
  for (VariablePtrTrack->CurrPtr = VariablePtrTrack->StartPtr;
       IsValidVariableHeader (VariablePtrTrack->CurrPtr, VariablePtrTrack->EndPtr);
       VariablePtrTrack->CurrPtr = GetNextVariablePtr (&VariableNvmStoreInfo, VariablePtrTrack->CurrPtr)
       ) {
    if (VariablePtrTrack->CurrPtr->State == VAR_ADDED ||
        VariablePtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)
        ) {

      if (VariableName[0] == 0) {
        DEBUG ((EFI_D_INFO, "The variable has been requested to be deleted.\n"));
        //
        // The variable has been requested to be deleted
        //
        if (VariablePtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          InDeletedTransitionVariable = VariablePtrTrack->CurrPtr;
        } else {
          VariablePtrTrack->InDeletedTransitionPtr = InDeletedTransitionVariable;
          return EFI_SUCCESS;
        }
      } else {
        //
        // A variable name was given to find
        //
        if (CompareGuid (VendorGuid, &VariablePtrTrack->CurrPtr->VendorGuid)) {
          VariableCandidateName = (VOID *) GetVariableNamePtr (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag);

          ASSERT (NameSizeOfVariable (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag) != 0);
          if (CompareMem (VariableName, VariableCandidateName,
                          NameSizeOfVariable (VariablePtrTrack->CurrPtr, VariableNvmStoreInfo.AuthFlag)) == 0) {
            if (VariablePtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
              InDeletedTransitionVariable = VariablePtrTrack->CurrPtr;
            } else {
              VariablePtrTrack->InDeletedTransitionPtr = InDeletedTransitionVariable;
              DEBUG ((EFI_D_INFO, "Returning with variable found at header address 0x%x\n", VariablePtrTrack->CurrPtr));
              return EFI_SUCCESS;
            }
          }
        }
      }
    }
  }

  VariablePtrTrack->CurrPtr = InDeletedTransitionVariable;

  return (VariablePtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}


/**
  Finds the variable in CSE NVM.
  This code finds the variable in the CSE NVM variable stores enabled.

  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  VariableName            Name of the variable to be found.
  @param[in]  VendorGuid              Vendor GUID to be found.
  @param[in]  CseVariableFileInfo     A pointer to a structure that contains information the CSE files available.
  @param[out] PtrTrack                VARIABLE_NVM POINTER_TRACK structure for output,
                                      including the range searched and the target position.

  @retval     EFI_INVALID_PARAMETER   If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval     EFI_SUCCESS             Variable successfully found.
  @retval     EFI_NOT_FOUND           Variable not found

**/
EFI_STATUS
EFIAPI
FindVariable (
  IN   CONST CHAR16                      *VariableName,
  IN   CONST EFI_GUID                    *VendorGuid,
  IN    CSE_VARIABLE_FILE_INFO      **CseVariableFileInfo,
  OUT   VARIABLE_NVM_POINTER_TRACK  *VariablePtrTrack
  )
{
  EFI_STATUS                  Status;
  CSE_VARIABLE_FILE_TYPE      Type;
  VARIABLE_NVM_STORE_HEADER   *VariableStoreHeader;

  if (VariableName[0] != 0 && VendorGuid == NULL) {
    DEBUG ((EFI_D_ERROR, "VariableName and VendorGuid passed to FindVariable() are invalid.\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Determine the CSE store file type for this variable
  //
  Type = GetCseVariableStoreFileType (VariableName, VendorGuid, CseVariableFileInfo);

  if (CseVariableFileInfo[Type] == NULL || !CseVariableFileInfo[Type]->FileStoreEstablished) {
    DEBUG ((EFI_D_ERROR, "The variable store needed is not available for this variable. Variable store type = %d\n", (UINT32)Type));
    return EFI_NOT_FOUND;
  }

  VariableStoreHeader = (VARIABLE_NVM_STORE_HEADER *) (UINTN) CseVariableFileInfo[Type]->HeaderRegionBase;

  DEBUG ((EFI_D_INFO, "VariableStoreHeader Region Base in FindVariable() = 0x%x\n", VariableStoreHeader));

  VariablePtrTrack->CurrPtr  = NULL;
  VariablePtrTrack->StartPtr = GetStartPointer (VariableStoreHeader);
  VariablePtrTrack->EndPtr   = GetEndPointer (VariableStoreHeader);

  //
  // Do not search in this variable store if it is empty
  //
  if (VariableStoreHeader->VariableHeaderTotalEntries == 0 || !IsValidVariableHeader (VariablePtrTrack->StartPtr, VariablePtrTrack->EndPtr)) {
    DEBUG ((EFI_D_INFO, "Variable store %d is empty, not searching.\n", (UINT32) Type));
    return EFI_NOT_FOUND;
  }

  //
  // Find the variable by searching in the variable header region of the store
  //
  Status = FindVariableEx (VariableName, VendorGuid, VariableStoreHeader, VariablePtrTrack);
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Found the variable\n"));
    return Status;
  }

  return EFI_NOT_FOUND;
}


/**
  Returns the next available variable in the given CSE variable stores.

  @param[in]  VariableName         Pointer to variable name.
  @param[in]  VendorGuid           Variable Vendor Guid.
  @param[in]  CseVariableFileInfo  A pointer to a structure that contains information the CSE files available.
  @param[out] VariablePtr          Pointer to variable header address.
  @param[out] IsAuthVariable       Indicates if the variable returned is an authenticated variable.

  @return     EFI_SUCCESS          Found the specified variable.
  @return     EFI_NOT_FOUND        Not found.

**/
EFI_STATUS
EFIAPI
GetNextCseVariableName (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VariableGuid,
  IN    CSE_VARIABLE_FILE_INFO  **CseVariableFileInfo,
  OUT  VARIABLE_NVM_HEADER     **VariablePtr,
  OUT  BOOLEAN                 *IsAuthVariable
  )
{
  CSE_VARIABLE_FILE_TYPE      Type;
  EFI_STATUS                  Status;
  VARIABLE_NVM_POINTER_TRACK  Variable;
  VARIABLE_NVM_POINTER_TRACK  VariablePtrTrack;
  VARIABLE_NVM_STORE_INFO     StoreInfo[CseVariableFileTypeMax];

  //
  // In the case of a NULL variable name, FindVariable will return the first
  // variable in the appropriate CSE file store. It is recommended to make
  // a special case for the name NULL that returns the first CSE variable file
  // store type (currently implemented in CseVariableStorageSelectorLib).
  //
  Status = FindVariable (VariableName, VariableGuid, CseVariableFileInfo, &Variable);

  if (EFI_ERROR (Status)) {
    return Status;
  } else if (Variable.CurrPtr == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Initialize the CSE store info structures
  //
  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    if (CseVariableFileInfo[Type] == NULL || !CseVariableFileInfo[Type]->FileStoreEstablished) {
      StoreInfo[Type].AuthFlag = FALSE;
      StoreInfo[Type].VariableStoreHeader = NULL;
    } else {
      StoreInfo[Type].VariableStoreHeader = (VARIABLE_NVM_STORE_HEADER *) (UINTN) CseVariableFileInfo[Type]->HeaderRegionBase;
      Status = IsAuthenticatedVariableStore (StoreInfo[Type].VariableStoreHeader, &StoreInfo[Type].AuthFlag);
      if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
    }
    DEBUG ((EFI_D_INFO,
      "Init StoreInfo[%d] to: AuthFlag=%c, StoreHeader = 0x%x\n",
      (UINT32) Type,
      ((StoreInfo[Type].AuthFlag) ? 'T' : 'F'),
      StoreInfo[Type].VariableStoreHeader)
      );
  }

  //
  // Find which CSE store the current variable is in,
  // and get the next variable if the variable given
  // was valid.
  //
  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    if ((StoreInfo[Type].VariableStoreHeader != NULL) && (Variable.StartPtr == GetStartPointer (StoreInfo[Type].VariableStoreHeader))) {
      DEBUG ((EFI_D_INFO, "Found current variable in CSE file type %d.\n", (UINT32) Type));
      if (VariableName[0] != 0) {
        Variable.CurrPtr = GetNextVariablePtr (&StoreInfo[Type], Variable.CurrPtr);
      }
      break;
    }
  }

  while (TRUE) {
    while (!IsValidVariableHeader (Variable.CurrPtr, Variable.EndPtr)) {
      //
      // Find a new valid CSE variable file store
      //
      for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
        if ((StoreInfo[Type].VariableStoreHeader != NULL) && (Variable.StartPtr == GetStartPointer (StoreInfo[Type].VariableStoreHeader))) {
          DEBUG ((EFI_D_INFO, "New CSE variable file store type is %d.\n", (UINT32) Type));
          break;
        }
      }

      ASSERT (Type < CseVariableFileTypeMax);

      //
      // Switch to the next CSE variable file store
      //
      for (Type++; Type < CseVariableFileTypeMax; Type++) {
        if (StoreInfo[Type].VariableStoreHeader != NULL) {
          DEBUG ((EFI_D_INFO, "Next store type in search is %d. Header at 0x%x\n", (UINT32) Type, StoreInfo[Type].VariableStoreHeader));
          break;
        }
      }

      //
      // Check if the CSE variable file stores available have
      // been exhausted and return not found if so.
      //
      if (Type == CseVariableFileTypeMax) {
        return EFI_NOT_FOUND;
      }

      Variable.StartPtr = GetStartPointer (StoreInfo[Type].VariableStoreHeader);
      Variable.EndPtr   = GetEndPointer   (StoreInfo[Type].VariableStoreHeader);
      Variable.CurrPtr  = Variable.StartPtr;
    }

    if (Type == CseVariableFileTypeMax) {
      return EFI_NOT_FOUND;
    }

    //
    // A valid variable was found
    //
    if (Variable.CurrPtr->State == VAR_ADDED || Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      if (Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
        //
        // If the variable is IN_DELETED_TRANSITION and there is
        // also the same variable with the ADDED attribute, don't
        // return it.
        //
        VariablePtrTrack.StartPtr = Variable.StartPtr;
        VariablePtrTrack.EndPtr   = Variable.EndPtr;

        Status = FindVariableEx (
                   GetVariableNamePtr (Variable.CurrPtr, StoreInfo[Type].AuthFlag),
                   GetVendorGuidPtr (Variable.CurrPtr, StoreInfo[Type].AuthFlag),
                   StoreInfo[Type].VariableStoreHeader,
                   &VariablePtrTrack
                   );
        if (!EFI_ERROR (Status) && VariablePtrTrack.CurrPtr->State == VAR_ADDED) {
          Variable.CurrPtr = GetNextVariablePtr (&StoreInfo[Type], Variable.CurrPtr);
          continue;
        }
      }

      DEBUG ((EFI_D_INFO, "Found next variable. Variable header at 0x%x. StoreInfo for type %d at 0x%x\n", Variable.CurrPtr, (UINT32) Type, &StoreInfo[Type]));

      *VariablePtr = Variable.CurrPtr;
      *IsAuthVariable = StoreInfo[Type].AuthFlag;

      return EFI_SUCCESS;
    }

    Variable.CurrPtr = GetNextVariablePtr (&StoreInfo[Type], Variable.CurrPtr);
  }

  return EFI_SUCCESS;
}


/**
  Retrieves a variable's value using its name and GUID.

  Read the specified variable from the CSE NVM variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  Note: It is the caller's responsibility to extensively validate the input parameters.

  @param[in]       CseVariableFileInfo    A pointer to an array of structures that contains information the CSE files available.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.
  @param[out]      VariableHeader         A pointer to the variable header.

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
GetCseVariable (
  IN        CSE_VARIABLE_FILE_INFO     **CseVariableFileInfo,
  IN  CONST      CHAR16                     *VariableName,
  IN  CONST      EFI_GUID                   *VariableGuid,
  OUT       UINT32                     *Attributes            OPTIONAL,
  IN OUT    UINTN                      *DataSize,
  OUT       VOID                       *Data,
  OUT       VARIABLE_NVM_HEADER        *VariableHeader        OPTIONAL
  )
{
  CHAR8                        CseVariableDataFileName[CSE_MAX_NVM_FILE_NAME_LENGTH];
  CSE_VARIABLE_HECI_PROTOCOL   HeciProtocolSelector;
  CSE_VARIABLE_FILE_TYPE       VariableCseFileType;
  UINTN                        VariableDataSize;
  UINT32                       VariableBaseFileOffset;
  VARIABLE_NVM_STORE_INFO      VariableNvmStoreInfo;
  VARIABLE_NVM_POINTER_TRACK   VariablePtrTrack;
  EFI_STATUS                   Status;

  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VariableCseFileType = GetCseVariableStoreFileType (VariableName, VariableGuid, CseVariableFileInfo);

  DEBUG ((EFI_D_INFO, "FileStoreEstablished = %c\n", (CseVariableFileInfo[VariableCseFileType]->FileStoreEstablished) ? 'T' : 'F'));

  if (CseVariableFileInfo[VariableCseFileType] == NULL || !CseVariableFileInfo[VariableCseFileType]->FileStoreEstablished) {
    DEBUG ((EFI_D_ERROR, "Variable store not available for this variable. Store type = %d\n", (UINT32) VariableCseFileType));
    return EFI_NOT_FOUND;
  }

  VariableNvmStoreInfo.VariableStoreHeader = (VARIABLE_NVM_STORE_HEADER *) (UINTN) CseVariableFileInfo[VariableCseFileType]->HeaderRegionBase;

  Status = IsAuthenticatedVariableStore (VariableNvmStoreInfo.VariableStoreHeader, &VariableNvmStoreInfo.AuthFlag);

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  Status = FindVariable (VariableName, VariableGuid, CseVariableFileInfo, &VariablePtrTrack);
  if (EFI_ERROR (Status) || VariablePtrTrack.CurrPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "Could not find variable or error finding variable.\n"));
    return Status;
  }

  VariableDataSize = DataSizeOfVariable (VariablePtrTrack.CurrPtr, VariableNvmStoreInfo.AuthFlag);

  ASSERT (VariableDataSize != 0);

  if (*DataSize < VariableDataSize) {
    DEBUG ((EFI_D_ERROR, "Buffer passed to GetVariable() is too small.\n"));
    *DataSize = VariableDataSize;
    return EFI_BUFFER_TOO_SMALL;
  } else {
    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }
  }

  *DataSize = VariableDataSize;

  if (Attributes != NULL) {
    *Attributes = VariablePtrTrack.CurrPtr->Attributes;
  }

  Status = GetCseVariableStoreFileOffset (VariableCseFileType, &VariableBaseFileOffset);
  ASSERT_EFI_ERROR (Status);

  if (VariableHeader != NULL) {
    VariableHeader = VariablePtrTrack.CurrPtr;
  }

  //
  // Read the data from the CSE NVM file
  //
  if (VariableCseFileType == CseVariableFileTypePreMemoryFile) {
    DEBUG ((EFI_D_INFO, "Variable is being read from the CSE mrc_trg_data file.\n"));

    Status = ReadCseNvmFile (
               CseVariableFileInfo[VariableCseFileType]->FileName,
               VariableBaseFileOffset + VariablePtrTrack.CurrPtr->DataOffset,
               Data,
               DataSize,
               NULL
               );
    ASSERT (*DataSize == DataSizeOfVariable (VariablePtrTrack.CurrPtr, VariableNvmStoreInfo.AuthFlag));
    return Status;
  } else if (VariableCseFileType == CseVariableFileTypePrimaryIndexFile) {

    DEBUG ((EFI_D_INFO, "Variable is being read from primary CSE NVM storage.\n"));
    ZeroMem (&CseVariableDataFileName[0], CSE_MAX_NVM_FILE_NAME_LENGTH);
    Status = BuildCseDataFileName (VariablePtrTrack.CurrPtr->DataOffset, &CseVariableDataFileName[0]);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Error building the CSE NVM data file name.\n"));
      return Status;
    }
    DEBUG ((EFI_D_INFO, "CSE NVM Data Filename: %a\n", &CseVariableDataFileName[0]));

    HeciProtocolSelector = GetCseVariableHeciProtocol (VariableName, VariableGuid);

    Status = ReadCseNvmFile (&CseVariableDataFileName[0], VariableBaseFileOffset, Data, DataSize, &HeciProtocolSelector);
    ASSERT (*DataSize == DataSizeOfVariable (VariablePtrTrack.CurrPtr, VariableNvmStoreInfo.AuthFlag));

    return Status;
  }

  return EFI_INVALID_PARAMETER;
}

