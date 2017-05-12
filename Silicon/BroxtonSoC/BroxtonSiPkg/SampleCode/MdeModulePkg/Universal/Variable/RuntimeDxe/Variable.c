/** @file
  The common variable operation routines shared by DXE_RUNTIME variable
  module and DXE_SMM variable module.

  Caution: This module requires additional review when modified.
  This driver will have external input - variable data. They may be input in SMM mode.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  VariableServiceGetNextVariableName () and VariableServiceQueryVariableInfo() are external API.
  They need check input parameter.

  VariableServiceGetVariable() and VariableServiceSetVariable() are external API
  to receive datasize and data buffer. The size should be checked carefully.

  VariableServiceSetVariable() should also check authenticate data to avoid buffer overflow,
  integer overflow. It should also check attribute to avoid authentication bypass.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Variable.h"

VARIABLE_MODULE_GLOBAL  *mVariableModuleGlobal;

///
/// A flag which indicates whether all variables should be treated as volatile
///
BOOLEAN               mForceVolatileVariable;

///
/// Define a memory cache that improves the search performance for a variable.
///
VARIABLE_STORE_HEADER  *mNvVariableCache      = NULL;

///
/// The memory entry used for variable statistics data.
///
VARIABLE_INFO_ENTRY    *gVariableInfo         = NULL;

///
/// The flag to indicate whether the platform has left the DXE phase of execution.
///
BOOLEAN                mEndOfDxe              = FALSE;

///
/// Buffer for temporary storage of variable contents when copying to NV cache
///
VOID                  *mVariableDataBuffer    = NULL;

///
/// It indicates the var check request source.
/// In the implementation, DXE is regarded as untrusted, and SMM is trusted.
///
VAR_CHECK_REQUEST_SOURCE mRequestSource       = VarCheckFromUntrusted;
CHAR16                   mVariableNameBuffer[MAX_VARIABLE_NAME_SIZE];

//
// It will record the current boot error flag before EndOfDxe.
//
VAR_ERROR_FLAG         mCurrentBootVarErrFlag = VAR_ERROR_FLAG_NO_ERROR;

VARIABLE_ENTRY_PROPERTY mVariableEntryProperty[] = {
  {
    &gEdkiiVarErrorFlagGuid,
    VAR_ERROR_FLAG_NAME,
    {
      VAR_CHECK_VARIABLE_PROPERTY_REVISION,
      VAR_CHECK_VARIABLE_PROPERTY_READ_ONLY,
      VARIABLE_ATTRIBUTE_NV_BS_RT,
      sizeof (VAR_ERROR_FLAG),
      sizeof (VAR_ERROR_FLAG)
    }
  },
};

AUTH_VAR_LIB_CONTEXT_IN mAuthContextIn = {
  AUTH_VAR_LIB_CONTEXT_IN_STRUCT_VERSION,
  //
  // StructSize, TO BE FILLED
  //
  0,
  //
  // MaxAuthVariableSize, TO BE FILLED
  //
  0,
  VariableExLibFindVariable,
  VariableExLibFindNextVariable,
  VariableExLibUpdateVariable,
  VariableExLibGetScratchBuffer,
  VariableExLibCheckRemainingSpaceForConsistency,
  VariableExLibAtRuntime,
};

AUTH_VAR_LIB_CONTEXT_OUT mAuthContextOut;


/**
  SecureBoot Hook for auth variable update.

  @param[in] VariableName                 Name of Variable to be found.
  @param[in] VendorGuid                   Variable vendor GUID.

**/
VOID
EFIAPI
SecureBootHook (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid
  );


/**
  Initialization for MOR Lock Control.

  @retval EFI_SUCEESS     MorLock initialization success.
  @retval Others          Some error occurs.

**/
EFI_STATUS
MorLockInit (
  VOID
  );


/**
  This service is an MOR/MorLock checker handler for the SetVariable().

  @param[in]  VariableName           The name of the vendor's variable, as a
                                     Null-Terminated Unicode String
  @param[in]  VendorGuid             Unify identifier for vendor.
  @param[in]  Attributes             Point to memory location to return the attributes of variable. If the point
                                     is NULL, the parameter would be ignored.
  @param[in]  DataSize               The size in bytes of Data-Buffer.
  @param[in]  Data                   Point to the content of the variable.

  @retval     EFI_SUCCESS            The MOR/MorLock check pass, and Variable driver can store the variable data.
  @retval     EFI_INVALID_PARAMETER  The MOR/MorLock data or data size or attributes is not allowed for MOR variable.
  @retval     EFI_ACCESS_DENIED      The MOR/MorLock is locked.
  @retval     EFI_ALREADY_STARTED    The MorLock variable is handled inside this function.
                                     Variable driver can just return EFI_SUCCESS.

**/
EFI_STATUS
SetVariableCheckHandlerMor (
  IN CHAR16     *VariableName,
  IN EFI_GUID   *VendorGuid,
  IN UINT32     Attributes,
  IN UINTN      DataSize,
  IN VOID       *Data
  );


/**
  Routine used to track statistical information about variable usage.
  The data is stored in the EFI system table so it can be accessed later.
  VariableInfo.efi can dump out the table. Only Boot Services variable
  accesses are tracked by this code. The PcdVariableCollectStatistics
  build flag controls if this feature is enabled.

  A read that hits in the cache will have Read and Cache true for
  the transaction. Data is allocated by this routine, but never
  freed.

  @param[in] VariableName   Name of the Variable to track.
  @param[in] VendorGuid     Guid of the Variable to track.
  @param[in] Volatile       TRUE if volatile FALSE if non-volatile.
  @param[in] Read           TRUE if GetVariable() was called.
  @param[in] Write          TRUE if SetVariable() was called.
  @param[in] Delete         TRUE if deleted via SetVariable().
  @param[in] Cache          TRUE for a cache hit.

**/
VOID
UpdateVariableInfo (
  IN  CHAR16                  *VariableName,
  IN  EFI_GUID                *VendorGuid,
  IN  BOOLEAN                 Volatile,
  IN  BOOLEAN                 Read,
  IN  BOOLEAN                 Write,
  IN  BOOLEAN                 Delete,
  IN  BOOLEAN                 Cache
  )
{
  VARIABLE_INFO_ENTRY   *Entry;

  if (FeaturePcdGet (PcdVariableCollectStatistics)) {

    if (AtRuntime ()) {
      //
      // Don't collect statistics at runtime.
      //
      return;
    }

    if (gVariableInfo == NULL) {
      //
      // On the first call allocate a entry and place a pointer to it in
      // the EFI System Table.
      //
      gVariableInfo = AllocateZeroPool (sizeof (VARIABLE_INFO_ENTRY));

      if (gVariableInfo == NULL) {
        ASSERT (gVariableInfo != NULL);
        return;
      }

      CopyGuid (&gVariableInfo->VendorGuid, VendorGuid);
      gVariableInfo->Name = AllocateZeroPool (StrSize (VariableName));

      if (gVariableInfo->Name == NULL) {
        ASSERT (gVariableInfo->Name != NULL);
        return;
      }

      StrCpyS (gVariableInfo->Name, StrSize (VariableName) / sizeof (CHAR16), VariableName);
      gVariableInfo->Volatile = Volatile;
    }


    for (Entry = gVariableInfo; Entry != NULL; Entry = Entry->Next) {
      if (CompareGuid (VendorGuid, &Entry->VendorGuid)) {
        if (StrCmp (VariableName, Entry->Name) == 0) {
          if (Read) {
            Entry->ReadCount++;
          }
          if (Write) {
            Entry->WriteCount++;
          }
          if (Delete) {
            Entry->DeleteCount++;
          }
          if (Cache) {
            Entry->CacheCount++;
          }
          return;
        }
      }

      if (Entry->Next == NULL) {
        //
        // If the entry is not in the table add it.
        // Next iteration of the loop will fill in the data.
        //
        Entry->Next = AllocateZeroPool (sizeof (VARIABLE_INFO_ENTRY));

        if (Entry->Next == NULL) {
          ASSERT (Entry->Next != NULL);
          return;
        }

        CopyGuid (&Entry->Next->VendorGuid, VendorGuid);
        Entry->Next->Name = AllocateZeroPool (StrSize (VariableName));
        ASSERT (Entry->Next->Name != NULL);
        StrCpyS (Entry->Next->Name, StrSize (VariableName) /sizeof (CHAR16), VariableName);
        Entry->Next->Volatile = Volatile;
      }

    }
  }
}


/**
  This code checks if variable header is valid or not.

  @param[in] Variable           Pointer to the Variable Header.
  @param[in] VariableStoreEnd   Pointer to the Variable Store End.

  @retval    TRUE               Variable header is valid.
  @retval    FALSE              Variable header is not valid.

**/
BOOLEAN
IsValidVariableHeader (
  IN  VARIABLE_HEADER       *Variable,
  IN  VARIABLE_HEADER       *VariableStoreEnd
  )
{
  if ((Variable == NULL) || (Variable >= VariableStoreEnd) || (Variable->StartId != VARIABLE_DATA)) {
    //
    // Variable is NULL or has reached the end of variable store,
    // or the StartId is not correct.
    //
    return FALSE;
  }

  return TRUE;
}


/**
  This function writes data to the FWH at the correct LBA even if the LBAs
  are fragmented.

  @param[in] Global                  Pointer to VARAIBLE_GLOBAL structure.
  @param[in] Volatile                Point out the Variable is Volatile or Non-Volatile.
  @param[in] SetByIndex              TRUE if target pointer is given as index.
                                     FALSE if target pointer is absolute.
  @param[in] DataPtrIndex            Pointer to the Data from the end of VARIABLE_STORE_HEADER
                                     structure.
  @param[in] DataSize                Size of data to be written.
  @param[in] Buffer                  Pointer to the buffer from which data is written.

  @retval    EFI_INVALID_PARAMETER   Parameters not valid.
  @retval    EFI_SUCCESS             Variable store successfully updated.

**/
EFI_STATUS
UpdateVariableStore (
  IN  VARIABLE_GLOBAL                     *Global,
  IN  BOOLEAN                             Volatile,
  IN  BOOLEAN                             SetByIndex,
  IN  UINTN                               DataPtrIndex,
  IN  UINT32                              DataSize,
  IN  UINT8                               *Buffer
  )
{
  VARIABLE_STORE_HEADER       *VolatileBase;
  EFI_PHYSICAL_ADDRESS        DataPtr;

  DataPtr     = DataPtrIndex;
  //
  // Check if the Data is Volatile.
  //
  if (!Volatile) {
    //
    // Data Pointer should point to the actual Address where data is to be
    // written.
    //
    if (SetByIndex) {
      DataPtr += (UINTN) mNvVariableCache;
    }

    if ((DataPtr + DataSize) >= ((UINTN) ((UINT8 *) mNvVariableCache + mNvVariableCache->Size))) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // Data Pointer should point to the actual Address where data is to be
    // written.
    //
    VolatileBase = (VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->VariableGlobal.VolatileVariableBase);
    if (SetByIndex) {
      DataPtr += mVariableModuleGlobal->VariableGlobal.VolatileVariableBase;
    }

    if ((DataPtr + DataSize) >= ((UINTN) ((UINT8 *) VolatileBase + VolatileBase->Size))) {
      return EFI_INVALID_PARAMETER;
    }
  }
  CopyMem ((UINT8 *) (UINTN) DataPtr, Buffer, DataSize);

  return EFI_SUCCESS;
}


/**
  This code gets the current status of Variable Store.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @retval    EfiRaw          Variable store status is raw.
  @retval    EfiValid        Variable store status is valid.
  @retval    EfiInvalid      Variable store status is invalid.

**/
VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN VARIABLE_STORE_HEADER *VarStoreHeader
  )
{
  if ((CompareGuid (&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
       CompareGuid (&VarStoreHeader->Signature, &gEfiVariableGuid)) &&
      VarStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
      VarStoreHeader->State == VARIABLE_STORE_HEALTHY
      ) {

    return EfiValid;
  } else if (((UINT32 *) (&VarStoreHeader->Signature))[0] == 0xffffffff &&
             ((UINT32 *) (&VarStoreHeader->Signature))[1] == 0xffffffff &&
             ((UINT32 *) (&VarStoreHeader->Signature))[2] == 0xffffffff &&
             ((UINT32 *) (&VarStoreHeader->Signature))[3] == 0xffffffff &&
             VarStoreHeader->Size == 0xffffffff &&
             VarStoreHeader->Format == 0xff &&
             VarStoreHeader->State == 0xff
          ) {
    return EfiRaw;
  } else {
    return EfiInvalid;
  }
}


/**
  This code gets the size of variable header.

  @return Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  VOID
  )
{
  UINTN Value;

  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    Value = sizeof (AUTHENTICATED_VARIABLE_HEADER);
  } else {
    Value = sizeof (VARIABLE_HEADER);
  }

  return Value;
}


/**
  This code gets the size of name of variable.

  @param[in] Variable        Pointer to the Variable Header.

  @return    UINTN           Size of variable in bytes.

**/
UINTN
NameSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->NameSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
        Variable->DataSize == (UINT32) (-1) ||
        Variable->NameSize == (UINT32) (-1) ||
        Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->NameSize;
  }
}


/**
  This code sets the size of name of variable.

  @param[in] Variable   Pointer to the Variable Header.
  @param[in] NameSize   Name size to set.

**/
VOID
SetNameSizeOfVariable (
  IN VARIABLE_HEADER    *Variable,
  IN UINTN              NameSize
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    AuthVariable->NameSize = (UINT32) NameSize;
  } else {
    Variable->NameSize = (UINT32) NameSize;
  }
}


/**
  This code gets the size of variable data.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Size of variable in bytes.

**/
UINTN
DataSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->DataSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
        Variable->DataSize == (UINT32) (-1) ||
        Variable->NameSize == (UINT32) (-1) ||
        Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->DataSize;
  }
}


/**
  This code sets the size of variable data.

  @param[in] Variable   Pointer to the Variable Header.
  @param[in] DataSize   Data size to set.

**/
VOID
SetDataSizeOfVariable (
  IN VARIABLE_HEADER    *Variable,
  IN UINTN              DataSize
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    AuthVariable->DataSize = (UINT32) DataSize;
  } else {
    Variable->DataSize = (UINT32) DataSize;
  }
}


/**
  This code gets the pointer to the variable name.

  @param Variable        Pointer to the Variable Header.

  @return Pointer to Variable Name which is Unicode encoding.

**/
CHAR16 *
GetVariableNamePtr (
  IN  VARIABLE_HEADER   *Variable
  )
{
  return (CHAR16 *) ((UINTN) Variable + GetVariableHeaderSize ());
}


/**
  This code gets the pointer to the variable guid.

  @param Variable   Pointer to the Variable Header.

  @return A EFI_GUID* pointer to Vendor Guid.

**/
EFI_GUID *
GetVendorGuidPtr (
  IN VARIABLE_HEADER    *Variable
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    return &AuthVariable->VendorGuid;
  } else {
    return &Variable->VendorGuid;
  }
}


/**
  This code gets the pointer to the variable data.

  @param Variable        Pointer to the Variable Header.

  @return Pointer to Variable Data.

**/
UINT8 *
GetVariableDataPtr (
  IN  VARIABLE_HEADER   *Variable
  )
{
  UINTN Value;

  //
  // Be careful about pad size for alignment.
  //
  Value =  (UINTN) GetVariableNamePtr (Variable);
  Value += NameSizeOfVariable (Variable);
  Value += GET_PAD_SIZE (NameSizeOfVariable (Variable));

  return (UINT8 *) Value;
}


/**
  This code gets the variable data offset related to variable header.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Variable Data offset.

**/
UINTN
GetVariableDataOffset (
  IN  VARIABLE_HEADER   *Variable
  )
{
  UINTN Value;

  //
  // Be careful about pad size for alignment
  //
  Value = GetVariableHeaderSize ();
  Value += NameSizeOfVariable (Variable);
  Value += GET_PAD_SIZE (NameSizeOfVariable (Variable));

  return Value;
}


/**
  This code gets the pointer to the next variable header.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Pointer to next variable header.

**/
VARIABLE_HEADER *
GetNextVariablePtr (
  IN  VARIABLE_HEADER   *Variable
  )
{
  UINTN Value;

  Value =  (UINTN) GetVariableDataPtr (Variable);
  Value += DataSizeOfVariable (Variable);
  Value += GET_PAD_SIZE (DataSizeOfVariable (Variable));

  //
  // Be careful about pad size for alignment.
  //
  return (VARIABLE_HEADER *) HEADER_ALIGN (Value);
}


/**
  Gets the pointer to the first variable header in given variable store area.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return    Pointer to the first variable header.

**/
VARIABLE_HEADER *
GetStartPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  //
  // The end of variable store.
  //
  return (VARIABLE_HEADER *) HEADER_ALIGN (VarStoreHeader + 1);
}


/**
  Gets the pointer to the end of the variable storage area.

  This function gets pointer to the end of the variable storage
  area, according to the input variable store header.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return    Pointer to the end of the variable storage area.

**/
VARIABLE_HEADER *
GetEndPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  //
  // The end of variable store
  //
  return (VARIABLE_HEADER *) HEADER_ALIGN ((UINTN) VarStoreHeader + VarStoreHeader->Size);
}


/**
  Gets the VARIABLE_STORAGE_PROTOCOL for the NV Storage that should store
  a variable with the given name and GUID

  @param[in]   VariableName                Name of the variable to be found.
  @param[in]   VendorGuid                  Vendor GUID to be found.
  @param[out]  VariableStorageProtocol     The found VARIABLE_STORAGE_PROTOCOL

  @retval      EFI_INVALID_PARAMETER       If VariableName is an empty string, or
                                           VendorGuid is NULL.
  @retval      EFI_SUCCESS                 VARIABLE_STORAGE_PROTOCOL successfully found.
  @retval      EFI_NOT_FOUND               VARIABLE_STORAGE_PROTOCOL not found

**/
EFI_STATUS
GetVariableStorageProtocol (
  IN  CHAR16                      *VariableName,
  IN  EFI_GUID                    *VendorGuid,
  OUT VARIABLE_STORAGE_PROTOCOL   **VariableStorageProtocol
  )
{
  EFI_GUID                    VariableStorageId;
  EFI_GUID                    InstanceGuid;
  VARIABLE_STORAGE_PROTOCOL   *CurrentInstance;
  UINTN                       Index;
  EFI_STATUS                  Status;

  if (VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *VariableStorageProtocol = NULL;
  ZeroMem ((VOID *) &VariableStorageId, sizeof (EFI_GUID));
  Status = GetVariableStorageId (VariableName, VendorGuid, TRUE, &VariableStorageId);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for ( Index = 0;
        Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
        Index++) {
    CurrentInstance = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
    ZeroMem ((VOID *) &InstanceGuid, sizeof (EFI_GUID));
    Status = CurrentInstance->GetId (&InstanceGuid);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (CompareGuid (&VariableStorageId, &InstanceGuid)) {
      *VariableStorageProtocol = CurrentInstance;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}


/**
  Record variable error flag.

  @param[in] Flag               Variable error flag to record.
  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Guid of variable.
  @param[in] Attributes         Attributes of the variable.
  @param[in] VariableSize       Size of the variable.

**/
VOID
RecordVarErrorFlag (
  IN VAR_ERROR_FLAG         Flag,
  IN CHAR16                 *VariableName,
  IN EFI_GUID               *VendorGuid,
  IN UINT32                 Attributes,
  IN UINTN                  VariableSize
  )
{
  EFI_STATUS                  Status;
  VARIABLE_POINTER_TRACK      Variable;
  VAR_ERROR_FLAG              *VarErrFlag;
  VAR_ERROR_FLAG              TempFlag;
  VARIABLE_STORAGE_PROTOCOL   *VariableStorageProtocol;

  DEBUG_CODE (
    DEBUG ((EFI_D_ERROR, "  Variable Driver: RecordVarErrorFlag (0x%02x) %s:%g - 0x%08x - 0x%x\n", Flag, VariableName, VendorGuid, Attributes, VariableSize));
    if (Flag == VAR_ERROR_FLAG_SYSTEM_ERROR) {
      if (AtRuntime ()) {
        DEBUG ((EFI_D_ERROR, "  Variable Driver: CommonRuntimeVariableSpace = 0x%x - CommonVariableTotalSize = 0x%x\n", mVariableModuleGlobal->CommonRuntimeVariableSpace, mVariableModuleGlobal->CommonVariableTotalSize));
      } else {
        DEBUG ((EFI_D_ERROR, "  Variable Driver: CommonVariableSpace = 0x%x - CommonVariableTotalSize = 0x%x\n", mVariableModuleGlobal->CommonVariableSpace, mVariableModuleGlobal->CommonVariableTotalSize));
      }
    } else {
      DEBUG ((EFI_D_ERROR, "  Variable Driver: CommonMaxUserVariableSpace = 0x%x - CommonUserVariableTotalSize = 0x%x\n", mVariableModuleGlobal->CommonMaxUserVariableSpace, mVariableModuleGlobal->CommonUserVariableTotalSize));
    }
  );

  if (!mEndOfDxe) {
    //
    // Before EndOfDxe, just record the current boot variable error flag to local variable,
    // and leave the variable error flag in NV flash as the last boot variable error flag.
    // After EndOfDxe in InitializeVarErrorFlag (), the variable error flag in NV flash
    // will be initialized to this local current boot variable error flag.
    //
    mCurrentBootVarErrFlag &= Flag;
    return;
  }

  //
  // Record error flag (it should have be initialized).
  //
  Status = FindVariable (
             VAR_ERROR_FLAG_NAME,
             &gEdkiiVarErrorFlagGuid,
             &Variable,
             &mVariableModuleGlobal->VariableGlobal,
             FALSE
             );
  if (!EFI_ERROR (Status)) {
    VarErrFlag = (VAR_ERROR_FLAG *) GetVariableDataPtr (Variable.CurrPtr);
    TempFlag = *VarErrFlag;
    TempFlag &= Flag;
    if (TempFlag == *VarErrFlag) {
      return;
    }
    VariableStorageProtocol = NULL;
    Status = GetVariableStorageProtocol (
               VAR_ERROR_FLAG_NAME,
               &gEdkiiVarErrorFlagGuid,
               &VariableStorageProtocol
               );
    if (!EFI_ERROR (Status) && VariableStorageProtocol != NULL) {
      //
      // Update the data in NV
      //
      if (VariableStorageProtocol->WriteServiceIsReady (
                                        VariableStorageProtocol)) {
        Status = VariableStorageProtocol->SetVariable (
                                            VariableStorageProtocol,
                                            VAR_ERROR_FLAG_NAME,
                                            &gEdkiiVarErrorFlagGuid,
                                            VARIABLE_ATTRIBUTE_NV_BS_RT,
                                            sizeof (TempFlag),
                                            &TempFlag,
                                            AtRuntime (),
                                            0,
                                            0,
                                            NULL
                                            );
        if (!EFI_ERROR (Status)) {
          //
          // Update the data in NV cache.
          //
          *VarErrFlag = TempFlag;
        }
      }
    }
  }
}


/**
  Initialize variable error flag.

  Before EndOfDxe, the variable indicates the last boot variable error flag,
  then it means the last boot variable error flag must be got before EndOfDxe.
  After EndOfDxe, the variable indicates the current boot variable error flag,
  then it means the current boot variable error flag must be got after EndOfDxe.

**/
VOID
InitializeVarErrorFlag (
  VOID
  )
{
  EFI_STATUS                Status;
  VARIABLE_POINTER_TRACK    Variable;
  VAR_ERROR_FLAG            Flag;
  VAR_ERROR_FLAG            VarErrFlag;

  if (!mEndOfDxe) {
    return;
  }

  Flag = mCurrentBootVarErrFlag;
  DEBUG ((EFI_D_INFO, "  Variable Driver: Initialize variable error flag (%02x)\n", Flag));

  Status = FindVariable (
             VAR_ERROR_FLAG_NAME,
             &gEdkiiVarErrorFlagGuid,
             &Variable,
             &mVariableModuleGlobal->VariableGlobal,
             FALSE
             );
  if (!EFI_ERROR (Status)) {
    VarErrFlag = *((VAR_ERROR_FLAG *) GetVariableDataPtr (Variable.CurrPtr));
    if (VarErrFlag == Flag) {
      return;
    }
  }

  UpdateVariable (
    VAR_ERROR_FLAG_NAME,
    &gEdkiiVarErrorFlagGuid,
    &Flag,
    sizeof (Flag),
    VARIABLE_ATTRIBUTE_NV_BS_RT,
    0,
    0,
    &Variable,
    NULL
    );
}


/**
  Is user variable?

  @param[in] Variable      Pointer to variable header.

  @retval    TRUE          User variable.
  @retval    FALSE         System variable.

**/
BOOLEAN
IsUserVariable (
  IN VARIABLE_HEADER    *Variable
  )
{
  VAR_CHECK_VARIABLE_PROPERTY   Property;

  //
  // Only after End Of Dxe, the variables belong to system variable are fixed.
  // If PcdMaxUserNvStorageVariableSize is 0, it means user variable share the same NV storage with system variable,
  // then no need to check if the variable is user variable or not specially.
  //
  if (mEndOfDxe && (mVariableModuleGlobal->CommonMaxUserVariableSpace != mVariableModuleGlobal->CommonVariableSpace)) {
    if (VarCheckLibVariablePropertyGet (GetVariableNamePtr (Variable), GetVendorGuidPtr (Variable), &Property) == EFI_NOT_FOUND) {
      return TRUE;
    }
  }
  return FALSE;
}


/**
  Calculate common user variable total size.

**/
VOID
CalculateCommonUserVariableTotalSize (
  VOID
  )
{
  VARIABLE_HEADER               *Variable;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VariableSize;
  VAR_CHECK_VARIABLE_PROPERTY   Property;

  //
  // Only after End Of Dxe, the variables belong to system variable are fixed.
  // If PcdMaxUserNvStorageVariableSize is 0, it means user variable share the same NV storage with system variable,
  // then no need to calculate the common user variable total size specially.
  //
  if (mEndOfDxe && (mVariableModuleGlobal->CommonMaxUserVariableSpace != mVariableModuleGlobal->CommonVariableSpace)) {
    Variable = GetStartPointer (mNvVariableCache);
    while (IsValidVariableHeader (Variable, GetEndPointer (mNvVariableCache))) {
      NextVariable = GetNextVariablePtr (Variable);
      VariableSize = (UINTN) NextVariable - (UINTN) Variable;
      if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        if (VarCheckLibVariablePropertyGet (GetVariableNamePtr (Variable), GetVendorGuidPtr (Variable), &Property) == EFI_NOT_FOUND) {
          //
          // No property, it is user variable.
          //
          mVariableModuleGlobal->CommonUserVariableTotalSize += VariableSize;
        }
      }

      Variable = NextVariable;
    }
  }
}


/**
  Initialize variable quota.

**/
VOID
InitializeVariableQuota (
  VOID
  )
{
  if (!mEndOfDxe) {
    return;
  }

  InitializeVarErrorFlag ();
  CalculateCommonUserVariableTotalSize ();
}


/**
  Find the variable in the specified variable store.

  @param[in]       VariableName        Name of the variable to be found
  @param[in]       VendorGuid          Vendor GUID to be found.
  @param[in]       IgnoreRtCheck       Ignore EFI_VARIABLE_RUNTIME_ACCESS attribute
                                       check at runtime when searching variable.
  @param[in, out]  PtrTrack            Variable Track Pointer structure that contains Variable Information.

  @retval          EFI_SUCCESS         Variable found successfully
  @retval          EFI_NOT_FOUND       Variable not found
**/
EFI_STATUS
FindVariableEx (
  IN     CHAR16                  *VariableName,
  IN     EFI_GUID                *VendorGuid,
  IN     BOOLEAN                 IgnoreRtCheck,
  IN OUT VARIABLE_POINTER_TRACK  *PtrTrack
  )
{
  VARIABLE_HEADER                *InDeletedVariable;
  VOID                           *Point;

  PtrTrack->InDeletedTransitionPtr = NULL;

  //
  // Find the variable by walk through HOB, volatile and non-volatile variable store.
  //
  InDeletedVariable  = NULL;

  for ( PtrTrack->CurrPtr = PtrTrack->StartPtr
      ; IsValidVariableHeader (PtrTrack->CurrPtr, PtrTrack->EndPtr)
      ; PtrTrack->CurrPtr = GetNextVariablePtr (PtrTrack->CurrPtr)
      ) {
    if (PtrTrack->CurrPtr->State == VAR_ADDED ||
        PtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)
       ) {
      if (IgnoreRtCheck || !AtRuntime () || ((PtrTrack->CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) != 0)) {
        if (VariableName[0] == 0) {
          if (PtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
            InDeletedVariable   = PtrTrack->CurrPtr;
          } else {
            PtrTrack->InDeletedTransitionPtr = InDeletedVariable;
            return EFI_SUCCESS;
          }
        } else {
          if (CompareGuid (VendorGuid, GetVendorGuidPtr (PtrTrack->CurrPtr))) {
            Point = (VOID *) GetVariableNamePtr (PtrTrack->CurrPtr);

            ASSERT (NameSizeOfVariable (PtrTrack->CurrPtr) != 0);
            if (CompareMem (VariableName, Point, NameSizeOfVariable (PtrTrack->CurrPtr)) == 0) {
              if (PtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
                InDeletedVariable     = PtrTrack->CurrPtr;
              } else {
                PtrTrack->InDeletedTransitionPtr = InDeletedVariable;
                return EFI_SUCCESS;
              }
            }
          }
        }
      }
    }
  }

  PtrTrack->CurrPtr = InDeletedVariable;
  return (PtrTrack->CurrPtr  == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}


/**
  Variable store garbage collection and reclaim operation.

  @param[in]      VariableBase            Base address of variable store.
  @param[out]     LastVariableOffset      Offset of last variable.
  @param[in]      IsVolatile              The variable store is volatile or not;
                                          if it is non-volatile, need FTW.
  @param[in, out] UpdatingPtrTrack        Pointer to updating variable pointer track structure.
  @param[in]      NewVariable             Pointer to new variable.
  @param[in]      NewVariableSize         New variable size.

  @return         EFI_SUCCESS             Reclaim operation has finished successfully.
  @return         EFI_OUT_OF_RESOURCES    No enough memory resources or variable space.
  @return         Others                  Unexpect error happened during reclaim operation.

**/
EFI_STATUS
Reclaim (
  IN     EFI_PHYSICAL_ADDRESS         VariableBase,
  OUT    UINTN                        *LastVariableOffset,
  IN     BOOLEAN                      IsVolatile,
  IN OUT VARIABLE_POINTER_TRACK       *UpdatingPtrTrack,
  IN     VARIABLE_HEADER              *NewVariable,
  IN     UINTN                        NewVariableSize
  )
{
  VARIABLE_HEADER               *Variable;
  VARIABLE_HEADER               *AddedVariable;
  VARIABLE_HEADER               *NextVariable;
  VARIABLE_HEADER               *NextAddedVariable;
  VARIABLE_STORE_HEADER         *VariableStoreHeader;
  UINT8                         *ValidBuffer;
  UINTN                         MaximumBufferSize;
  UINTN                         VariableSize;
  UINTN                         NameSize;
  UINT8                         *CurrPtr;
  VOID                          *Point0;
  VOID                          *Point1;
  BOOLEAN                       FoundAdded;
  EFI_STATUS                    Status;
  UINTN                         CommonVariableTotalSize;
  UINTN                         CommonUserVariableTotalSize;
  UINTN                         HwErrVariableTotalSize;
  VARIABLE_HEADER               *UpdatingVariable;
  VARIABLE_HEADER               *UpdatingInDeletedTransition;
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;
  VARIABLE_STORAGE_PROTOCOL     *VariableStorageProtocol;
  VARIABLE_POINTER_TRACK        VarErrFlagPtr;

  Status = EFI_SUCCESS;
  VariableSize = 0;
  UpdatingVariable            = NULL;
  UpdatingInDeletedTransition = NULL;

  if (UpdatingPtrTrack != NULL) {
    UpdatingVariable = UpdatingPtrTrack->CurrPtr;
    UpdatingInDeletedTransition = UpdatingPtrTrack->InDeletedTransitionPtr;
  }

  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) VariableBase);

  CommonVariableTotalSize     = 0;
  CommonUserVariableTotalSize = 0;
  HwErrVariableTotalSize      = 0;

  if (IsVolatile || mForceVolatileVariable) {
    //
    // Start Pointers for the variable.
    //
    Variable          = GetStartPointer (VariableStoreHeader);
    MaximumBufferSize = sizeof (VARIABLE_STORE_HEADER);

    while (IsValidVariableHeader (Variable, GetEndPointer (VariableStoreHeader))) {
      NextVariable = GetNextVariablePtr (Variable);
      if ((Variable->State == VAR_ADDED || Variable->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) &&
          Variable != UpdatingVariable &&
          Variable != UpdatingInDeletedTransition
         ) {
        VariableSize = (UINTN) NextVariable - (UINTN) Variable;
        MaximumBufferSize += VariableSize;
      }

      Variable = NextVariable;
    }

    if (NewVariable != NULL) {
      //
      // Add the new variable size.
      //
      MaximumBufferSize += NewVariableSize;
    }

    //
    // Reserve the 1 Bytes with Oxff to identify the
    // end of the variable buffer.
    //
    MaximumBufferSize += 1;
    ValidBuffer = AllocatePool (MaximumBufferSize);
    if (ValidBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SetMem (ValidBuffer, MaximumBufferSize, 0xff);

    //
    // Copy variable store header.
    //
    CopyMem (ValidBuffer, VariableStoreHeader, sizeof (VARIABLE_STORE_HEADER));
    CurrPtr = (UINT8 *) GetStartPointer ((VARIABLE_STORE_HEADER *) ValidBuffer);

    //
    // Reinstall all ADDED variables as long as they are not identical to Updating Variable.
    //
    Variable = GetStartPointer (VariableStoreHeader);
    while (IsValidVariableHeader (Variable, GetEndPointer (VariableStoreHeader))) {
      NextVariable = GetNextVariablePtr (Variable);
      if (Variable != UpdatingVariable && Variable->State == VAR_ADDED) {
        VariableSize = (UINTN) NextVariable - (UINTN) Variable;
        CopyMem (CurrPtr, (UINT8 *) Variable, VariableSize);
        CurrPtr += VariableSize;
        if ((!IsVolatile) && ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD)) {
          HwErrVariableTotalSize += VariableSize;
        } else if ((!IsVolatile) && ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD)) {
          CommonVariableTotalSize += VariableSize;
          if (IsUserVariable (Variable)) {
            CommonUserVariableTotalSize += VariableSize;
          }
        }
      }
      Variable = NextVariable;
    }

    //
    // Reinstall all in delete transition variables.
    //
    Variable = GetStartPointer (VariableStoreHeader);
    while (IsValidVariableHeader (Variable, GetEndPointer (VariableStoreHeader))) {
      NextVariable = GetNextVariablePtr (Variable);
      if (Variable != UpdatingVariable && Variable != UpdatingInDeletedTransition && Variable->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
        //
        // Buffer has cached all ADDED variable.
        // Per IN_DELETED variable, we have to guarantee that
        // no ADDED one in previous buffer.
        //
        FoundAdded = FALSE;
        AddedVariable = GetStartPointer ((VARIABLE_STORE_HEADER *) ValidBuffer);
        while (IsValidVariableHeader (AddedVariable, GetEndPointer ((VARIABLE_STORE_HEADER *) ValidBuffer))) {
          NextAddedVariable = GetNextVariablePtr (AddedVariable);
          NameSize = NameSizeOfVariable (AddedVariable);
          if (CompareGuid (GetVendorGuidPtr (AddedVariable), GetVendorGuidPtr (Variable)) &&
              NameSize == NameSizeOfVariable (Variable)
            ) {
            Point0 = (VOID *) GetVariableNamePtr (AddedVariable);
            Point1 = (VOID *) GetVariableNamePtr (Variable);
            if (CompareMem (Point0, Point1, NameSize) == 0) {
              FoundAdded = TRUE;
              break;
            }
          }
          AddedVariable = NextAddedVariable;
        }
        if (!FoundAdded) {
          //
          // Promote VAR_IN_DELETED_TRANSITION to VAR_ADDED.
          //
          VariableSize = (UINTN) NextVariable - (UINTN) Variable;
          CopyMem (CurrPtr, (UINT8 *) Variable, VariableSize);
          ((VARIABLE_HEADER *) CurrPtr)->State = VAR_ADDED;
          CurrPtr += VariableSize;
          if ((!IsVolatile) && ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD)) {
            HwErrVariableTotalSize += VariableSize;
          } else if ((!IsVolatile) && ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD)) {
            CommonVariableTotalSize += VariableSize;
            if (IsUserVariable (Variable)) {
              CommonUserVariableTotalSize += VariableSize;
            }
          }
        }
      }

      Variable = NextVariable;
    }
  } else {
    //
    // If we are reclaiming the NV cache, just clear it out. The cache will be rebuilt from NV as needed
    //
    ValidBuffer           = (UINT8 *) mNvVariableCache;
    VarErrFlagPtr.CurrPtr = NULL;

    //
    // Compute the changes to NV storage usage
    //
    HwErrVariableTotalSize      = mVariableModuleGlobal->HwErrVariableTotalSize;
    CommonVariableTotalSize     = mVariableModuleGlobal->CommonVariableTotalSize;
    CommonUserVariableTotalSize = mVariableModuleGlobal->CommonUserVariableTotalSize;
    if (NewVariable != NULL && UpdatingVariable != NULL) {
      NextVariable = GetNextVariablePtr (UpdatingPtrTrack->CurrPtr);
      VariableSize = (UINTN) NextVariable - (UINTN) UpdatingPtrTrack->CurrPtr;
      if ((UpdatingPtrTrack->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        HwErrVariableTotalSize -= VariableSize;
      } else if ((UpdatingPtrTrack->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        CommonVariableTotalSize -= VariableSize;
        if (IsUserVariable (UpdatingPtrTrack->CurrPtr)) {
          CommonUserVariableTotalSize -= VariableSize;
        }
      }
    }

    //
    // Retain VarErrorFlag in the NV cache
    //
    if (mEndOfDxe) {
      VarErrFlagPtr.StartPtr = GetStartPointer (VariableStoreHeader);
      VarErrFlagPtr.EndPtr   = GetEndPointer   (VariableStoreHeader);
      VarErrFlagPtr.Volatile = FALSE;
      if (IsValidVariableHeader (VarErrFlagPtr.StartPtr, VarErrFlagPtr.EndPtr)) {
        Status = FindVariableEx (
                  VAR_ERROR_FLAG_NAME,
                  &gEdkiiVarErrorFlagGuid,
                  TRUE,
                  &VarErrFlagPtr
                  );
        if (!EFI_ERROR (Status) && VarErrFlagPtr.CurrPtr != NULL) {
          NextVariable = GetNextVariablePtr (VarErrFlagPtr.CurrPtr);
          VariableSize = (UINTN) NextVariable - (UINTN) VarErrFlagPtr.CurrPtr;
        } else {
          VarErrFlagPtr.CurrPtr = NULL;
        }
      }
    }

    //
    // Delete all existing data
    //
    Point0            = (VOID *) GetStartPointer (VariableStoreHeader);
    Point1            = (VOID *) GetEndPointer (VariableStoreHeader);
    CurrPtr           = (UINT8 *) Point0;
    if (VarErrFlagPtr.CurrPtr != NULL && IsValidVariableHeader (Point0, Point1)) {
      CopyMem (Point0, VarErrFlagPtr.CurrPtr, VariableSize);
      Point0 = (UINT8 *) GetNextVariablePtr ((VARIABLE_HEADER *) Point0);
    }

    if (Point1 > Point0) {
      MaximumBufferSize = (UINTN) Point1 - (UINTN) Point0;
      SetMem (Point0, MaximumBufferSize, 0xff);
    }
  }

  //
  // Install the new variable if it is not NULL.
  //
  if (NewVariable != NULL) {
    if ((UINTN) (CurrPtr - ValidBuffer) + NewVariableSize > VariableStoreHeader->Size) {
      //
      // No enough space to store the new variable.
      //
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    if (!IsVolatile) {
      if ((NewVariable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        HwErrVariableTotalSize += NewVariableSize;
      } else if ((NewVariable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        CommonVariableTotalSize += NewVariableSize;
        if (IsUserVariable (NewVariable)) {
          CommonUserVariableTotalSize += NewVariableSize;
        }
      }
      if ((HwErrVariableTotalSize > PcdGet32 (PcdHwErrStorageSize)) ||
          (CommonVariableTotalSize > mVariableModuleGlobal->CommonVariableSpace) ||
          (CommonUserVariableTotalSize > mVariableModuleGlobal->CommonMaxUserVariableSpace)) {
        //
        // No enough space to store the new variable by NV or NV+HR attribute.
        //
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      //
      // Update the data in NV
      //
      if (!mForceVolatileVariable) {
        VariableStorageProtocol = NULL;
        Status = GetVariableStorageProtocol (
                  GetVariableNamePtr (NewVariable),
                  GetVendorGuidPtr (NewVariable),
                  &VariableStorageProtocol
                  );
        if (!EFI_ERROR (Status) && VariableStorageProtocol != NULL) {
          if (VariableStorageProtocol->WriteServiceIsReady (VariableStorageProtocol)) {
            if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
              AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) NewVariable;
              Status = VariableStorageProtocol->SetVariable (
                                                  VariableStorageProtocol,
                                                  GetVariableNamePtr (NewVariable),
                                                  GetVendorGuidPtr (NewVariable),
                                                  NewVariable->Attributes,
                                                  DataSizeOfVariable (NewVariable),
                                                  GetVariableDataPtr (NewVariable),
                                                  AtRuntime (),
                                                  AuthVariable->PubKeyIndex,
                                                  ReadUnaligned64 (&(AuthVariable->MonotonicCount)),
                                                  &AuthVariable->TimeStamp
                                                  );
            } else {
              Status = VariableStorageProtocol->SetVariable (
                                                  VariableStorageProtocol,
                                                  GetVariableNamePtr (NewVariable),
                                                  GetVendorGuidPtr (NewVariable),
                                                  NewVariable->Attributes,
                                                  DataSizeOfVariable (NewVariable),
                                                  GetVariableDataPtr (NewVariable),
                                                  AtRuntime (),
                                                  0,
                                                  0,
                                                  NULL
                                                  );
            }
            if (EFI_ERROR (Status)) {
              goto Done;
            }
          } else {
            Status = EFI_NOT_AVAILABLE_YET;
            goto Done;
          }
        } else {
          if (!EFI_ERROR (Status)) {
            Status = EFI_NOT_FOUND;
          }
          goto Done;
        }
      }
    }

    CopyMem (CurrPtr, (UINT8 *) NewVariable, NewVariableSize);
    ((VARIABLE_HEADER *) CurrPtr)->State = VAR_ADDED;
    if (UpdatingVariable != NULL) {
      UpdatingPtrTrack->CurrPtr = (VARIABLE_HEADER *)((UINTN)UpdatingPtrTrack->StartPtr + ((UINTN)CurrPtr - (UINTN)GetStartPointer ((VARIABLE_STORE_HEADER *) ValidBuffer)));
      UpdatingPtrTrack->InDeletedTransitionPtr = NULL;
    }
    CurrPtr += NewVariableSize;
  }

  *LastVariableOffset = (UINTN) (CurrPtr - ValidBuffer);
  if (IsVolatile) {
    //
    // If volatile variable store, just copy valid buffer.
    //
    SetMem ((UINT8 *) (UINTN) VariableBase, VariableStoreHeader->Size, 0xff);
    CopyMem ((UINT8 *) (UINTN) VariableBase, ValidBuffer, (UINTN) (CurrPtr - ValidBuffer));
    Status  = EFI_SUCCESS;
  } else {
    //
    // If non-volatile variable store, update NV storage usage
    //
    mVariableModuleGlobal->HwErrVariableTotalSize       = HwErrVariableTotalSize;
    mVariableModuleGlobal->CommonVariableTotalSize      = CommonVariableTotalSize;
    mVariableModuleGlobal->CommonUserVariableTotalSize  = CommonUserVariableTotalSize;
  }

Done:
  if (IsVolatile || mForceVolatileVariable) {
    FreePool (ValidBuffer);
  }

  return Status;
}


/**
  Get index from supported language codes according to language string.

  This code is used to get corresponding index in supported language codes. It can handle
  RFC4646 and ISO639 language tags.
  In ISO639 language tags, take 3-characters as a delimitation to find matched string and calculate the index.
  In RFC4646 language tags, take semicolon as a delimitation to find matched string and calculate the index.

  For example:
    SupportedLang  = "engfraengfra"
    Lang           = "eng"
    Iso639Language = TRUE
  The return value is "0".
  Another example:
    SupportedLang  = "en;fr;en-US;fr-FR"
    Lang           = "fr-FR"
    Iso639Language = FALSE
  The return value is "3".

  @param[in]  SupportedLang               Platform supported language codes.
  @param[in]  Lang                        Configured language.
  @param[in]  Iso639Language              A bool value to signify if the handler is operated on ISO639 or RFC4646.

  @retval     The index of language in the language codes.

**/
UINTN
GetIndexFromSupportedLangCodes (
  IN  CHAR8            *SupportedLang,
  IN  CHAR8            *Lang,
  IN  BOOLEAN          Iso639Language
  )
{
  UINTN    Index;
  UINTN    CompareLength;
  UINTN    LanguageLength;

  if (Iso639Language) {
    CompareLength = ISO_639_2_ENTRY_SIZE;
    for (Index = 0; Index < AsciiStrLen (SupportedLang); Index += CompareLength) {
      if (AsciiStrnCmp (Lang, SupportedLang + Index, CompareLength) == 0) {
        //
        // Successfully find the index of Lang string in SupportedLang string.
        //
        Index = Index / CompareLength;
        return Index;
      }
    }
    ASSERT (FALSE);
    return 0;
  } else {
    //
    // Compare RFC4646 language code
    //
    Index = 0;
    for (LanguageLength = 0; Lang[LanguageLength] != '\0'; LanguageLength++);

    for (Index = 0; *SupportedLang != '\0'; Index++, SupportedLang += CompareLength) {
      //
      // Skip ';' characters in SupportedLang
      //
      for (; *SupportedLang != '\0' && *SupportedLang == ';'; SupportedLang++);
      //
      // Determine the length of the next language code in SupportedLang
      //
      for (CompareLength = 0; SupportedLang[CompareLength] != '\0' && SupportedLang[CompareLength] != ';'; CompareLength++);

      if ((CompareLength == LanguageLength) &&
          (AsciiStrnCmp (Lang, SupportedLang, CompareLength) == 0)) {
        //
        // Successfully find the index of Lang string in SupportedLang string.
        //
        return Index;
      }
    }
    ASSERT (FALSE);
    return 0;
  }
}


/**
  Get language string from supported language codes according to index.

  This code is used to get corresponding language strings in supported language codes. It can handle
  RFC4646 and ISO639 language tags.
  In ISO639 language tags, take 3-characters as a delimitation. Find language string according to the index.
  In RFC4646 language tags, take semicolon as a delimitation. Find language string according to the index.

  For example:
    SupportedLang  = "engfraengfra"
    Index          = "1"
    Iso639Language = TRUE
  The return value is "fra".
  Another example:
    SupportedLang  = "en;fr;en-US;fr-FR"
    Index          = "1"
    Iso639Language = FALSE
  The return value is "fr".

  @param[in]  SupportedLang               Platform supported language codes.
  @param[in]  Index                       The index in supported language codes.
  @param[in]  Iso639Language              A bool value to signify if the handler is operated on ISO639 or RFC4646.

  @retval     The language string in the language codes.

**/
CHAR8 *
GetLangFromSupportedLangCodes (
  IN  CHAR8            *SupportedLang,
  IN  UINTN            Index,
  IN  BOOLEAN          Iso639Language
  )
{
  UINTN    SubIndex;
  UINTN    CompareLength;
  CHAR8    *Supported;

  SubIndex  = 0;
  Supported = SupportedLang;
  if (Iso639Language) {
    //
    // According to the index of Lang string in SupportedLang string to get the language.
    // This code will be invoked in RUNTIME, therefore there is not a memory allocate/free operation.
    // In driver entry, it pre-allocates a runtime attribute memory to accommodate this string.
    //
    CompareLength = ISO_639_2_ENTRY_SIZE;
    mVariableModuleGlobal->Lang[CompareLength] = '\0';
    return CopyMem (mVariableModuleGlobal->Lang, SupportedLang + Index * CompareLength, CompareLength);

  } else {
    while (TRUE) {
      //
      // Take semicolon as delimitation, sequentially traverse supported language codes.
      //
      for (CompareLength = 0; *Supported != ';' && *Supported != '\0'; CompareLength++) {
        Supported++;
      }
      if ((*Supported == '\0') && (SubIndex != Index)) {
        //
        // Have completed the traverse, but not find corrsponding string.
        // This case is not allowed to happen.
        //
        ASSERT(FALSE);
        return NULL;
      }
      if (SubIndex == Index) {
        //
        // According to the index of Lang string in SupportedLang string to get the language.
        // As this code will be invoked in RUNTIME, therefore there is not memory allocate/free operation.
        // In driver entry, it pre-allocates a runtime attribute memory to accommodate this string.
        //
        mVariableModuleGlobal->PlatformLang[CompareLength] = '\0';
        return CopyMem (mVariableModuleGlobal->PlatformLang, Supported - CompareLength, CompareLength);
      }
      SubIndex++;

      //
      // Skip ';' characters in Supported
      //
      for (; *Supported != '\0' && *Supported == ';'; Supported++);
    }
  }
}


/**
  Returns a pointer to an allocated buffer that contains the best matching language
  from a set of supported languages.

  This function supports both ISO 639-2 and RFC 4646 language codes, but language
  code types may not be mixed in a single call to this function. This function
  supports a variable argument list that allows the caller to pass in a prioritized
  list of language codes to test against all the language codes in SupportedLanguages.

  If SupportedLanguages is NULL, then ASSERT().

  @param[in]  SupportedLanguages  A pointer to a Null-terminated ASCII string that
                                  contains a set of language codes in the format
                                  specified by Iso639Language.
  @param[in]  Iso639Language      If TRUE, then all language codes are assumed to be
                                  in ISO 639-2 format.  If FALSE, then all language
                                  codes are assumed to be in RFC 4646 language format
  @param[in]  ...                 A variable argument list that contains pointers to
                                  Null-terminated ASCII strings that contain one or more
                                  language codes in the format specified by Iso639Language.
                                  The first language code from each of these language
                                  code lists is used to determine if it is an exact or
                                  close match to any of the language codes in
                                  SupportedLanguages.  Close matches only apply to RFC 4646
                                  language codes, and the matching algorithm from RFC 4647
                                  is used to determine if a close match is present.  If
                                  an exact or close match is found, then the matching
                                  language code from SupportedLanguages is returned.  If
                                  no matches are found, then the next variable argument
                                  parameter is evaluated.  The variable argument list
                                  is terminated by a NULL.

  @retval     NULL                The best matching language could not be found in SupportedLanguages.
  @retval     NULL                There are not enough resources available to return the best matching
                                  language.
  @retval     Other               A pointer to a Null-terminated ASCII string that is the best matching
                                  language in SupportedLanguages.

**/
CHAR8 *
EFIAPI
VariableGetBestLanguage (
  IN CONST CHAR8  *SupportedLanguages,
  IN BOOLEAN      Iso639Language,
  ...
  )
{
  VA_LIST      Args;
  CHAR8        *Language;
  UINTN        CompareLength;
  UINTN        LanguageLength;
  CONST CHAR8  *Supported;
  CHAR8        *Buffer;

  if (SupportedLanguages == NULL) {
    return NULL;
  }

  VA_START (Args, Iso639Language);
  while ((Language = VA_ARG (Args, CHAR8 *)) != NULL) {
    //
    // Default to ISO 639-2 mode
    //
    CompareLength  = 3;
    LanguageLength = MIN (3, AsciiStrLen (Language));

    //
    // If in RFC 4646 mode, then determine the length of the first RFC 4646 language code in Language
    //
    if (!Iso639Language) {
      for (LanguageLength = 0; Language[LanguageLength] != 0 && Language[LanguageLength] != ';'; LanguageLength++);
    }

    //
    // Trim back the length of Language used until it is empty
    //
    while (LanguageLength > 0) {
      //
      // Loop through all language codes in SupportedLanguages
      //
      for (Supported = SupportedLanguages; *Supported != '\0'; Supported += CompareLength) {
        //
        // In RFC 4646 mode, then Loop through all language codes in SupportedLanguages
        //
        if (!Iso639Language) {
          //
          // Skip ';' characters in Supported
          //
          for (; *Supported != '\0' && *Supported == ';'; Supported++);
          //
          // Determine the length of the next language code in Supported
          //
          for (CompareLength = 0; Supported[CompareLength] != 0 && Supported[CompareLength] != ';'; CompareLength++);
          //
          // If Language is longer than the Supported, then skip to the next language
          //
          if (LanguageLength > CompareLength) {
            continue;
          }
        }
        //
        // See if the first LanguageLength characters in Supported match Language
        //
        if (AsciiStrnCmp (Supported, Language, LanguageLength) == 0) {
          VA_END (Args);

          Buffer = Iso639Language ? mVariableModuleGlobal->Lang : mVariableModuleGlobal->PlatformLang;
          Buffer[CompareLength] = '\0';
          return CopyMem (Buffer, Supported, CompareLength);
        }
      }

      if (Iso639Language) {
        //
        // If ISO 639 mode, then each language can only be tested once
        //
        LanguageLength = 0;
      } else {
        //
        // If RFC 4646 mode, then trim Language from the right to the next '-' character
        //
        for (LanguageLength--; LanguageLength > 0 && Language[LanguageLength] != '-'; LanguageLength--);
      }
    }
  }
  VA_END (Args);

  //
  // No matches were found
  //
  return NULL;
}


/**
  This function is to check if the remaining variable space is enough to set
  all Variables from argument list successfully. The purpose of the check
  is to keep the consistency of the Variables to be in variable storage.

  Note: Variables are assumed to be in same storage.
  The set sequence of Variables will be same with the sequence of VariableEntry from argument list,
  so follow the argument sequence to check the Variables.

  @param[in] Attributes         Variable attributes for Variable entries.
  @param[in] Marker             VA_LIST style variable argument list.
                                The variable argument list with type VARIABLE_ENTRY_CONSISTENCY *.
                                A NULL terminates the list. The VariableSize of
                                VARIABLE_ENTRY_CONSISTENCY is the variable data size as input.
                                It will be changed to variable total size as output.

  @retval    TRUE               Have enough variable space to set the Variables successfully.
  @retval    FALSE              No enough variable space to set the Variables successfully.

**/
BOOLEAN
EFIAPI
CheckRemainingSpaceForConsistencyInternal (
  IN UINT32                     Attributes,
  IN VA_LIST                    Marker
  )
{
  EFI_STATUS                    Status;
  VA_LIST                       Args;
  VARIABLE_ENTRY_CONSISTENCY    *VariableEntry;
  UINT64                        MaximumVariableStorageSize;
  UINT64                        RemainingVariableStorageSize;
  UINT64                        MaximumVariableSize;
  UINTN                         TotalNeededSize;
  UINTN                         OriginalVarSize;
  VARIABLE_STORE_HEADER         *VariableStoreHeader;
  VARIABLE_POINTER_TRACK        VariablePtrTrack;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VarNameSize;
  UINTN                         VarDataSize;

  //
  // Non-Volatile related.
  //
  VariableStoreHeader = mNvVariableCache;

  Status = VariableServiceQueryVariableInfoInternal (
             Attributes,
             &MaximumVariableStorageSize,
             &RemainingVariableStorageSize,
             &MaximumVariableSize
             );
  ASSERT_EFI_ERROR (Status);

  TotalNeededSize = 0;
  Args = Marker;
  VariableEntry = VA_ARG (Args, VARIABLE_ENTRY_CONSISTENCY *);
  while (VariableEntry != NULL) {
    //
    // Calculate variable total size.
    //
    VarNameSize  = StrSize (VariableEntry->Name);
    VarNameSize += GET_PAD_SIZE (VarNameSize);
    VarDataSize  = VariableEntry->VariableSize;
    VarDataSize += GET_PAD_SIZE (VarDataSize);
    VariableEntry->VariableSize = HEADER_ALIGN (GetVariableHeaderSize () + VarNameSize + VarDataSize);

    TotalNeededSize += VariableEntry->VariableSize;
    VariableEntry = VA_ARG (Args, VARIABLE_ENTRY_CONSISTENCY *);
  }

  if (RemainingVariableStorageSize >= TotalNeededSize) {
    //
    // Already have enough space.
    //
    return TRUE;
  } else if (AtRuntime ()) {
    //
    // At runtime, no reclaim.
    // The original variable space of Variables can't be reused.
    //
    return FALSE;
  }

  Args = Marker;
  VariableEntry = VA_ARG (Args, VARIABLE_ENTRY_CONSISTENCY *);
  while (VariableEntry != NULL) {
    //
    // Check if Variable[Index] has been present and get its size.
    //
    OriginalVarSize = 0;
    VariablePtrTrack.StartPtr = GetStartPointer (VariableStoreHeader);
    VariablePtrTrack.EndPtr   = GetEndPointer   (VariableStoreHeader);
    Status = FindVariableEx (
               VariableEntry->Name,
               VariableEntry->Guid,
               FALSE,
               &VariablePtrTrack
               );
    if (!EFI_ERROR (Status)) {
      //
      // Get size of Variable[Index].
      //
      NextVariable = GetNextVariablePtr (VariablePtrTrack.CurrPtr);
      OriginalVarSize = (UINTN) NextVariable - (UINTN) VariablePtrTrack.CurrPtr;
      //
      // Add the original size of Variable[Index] to remaining variable storage size.
      //
      RemainingVariableStorageSize += OriginalVarSize;
    }
    if (VariableEntry->VariableSize > RemainingVariableStorageSize) {
      //
      // No enough space for Variable[Index].
      //
      return FALSE;
    }
    //
    // Sub the (new) size of Variable[Index] from remaining variable storage size.
    //
    RemainingVariableStorageSize -= VariableEntry->VariableSize;
    VariableEntry = VA_ARG (Args, VARIABLE_ENTRY_CONSISTENCY *);
  }

  return TRUE;
}


/**
  This function is to check if the remaining variable space is enough to set
  all Variables from argument list successfully. The purpose of the check
  is to keep the consistency of the Variables to be in variable storage.

  Note: Variables are assumed to be in same storage.
  The set sequence of Variables will be same with the sequence of VariableEntry from argument list,
  so follow the argument sequence to check the Variables.

  @param[in] Attributes         Variable attributes for Variable entries.
  @param ...                    The variable argument list with type VARIABLE_ENTRY_CONSISTENCY *.
                                A NULL terminates the list. The VariableSize of
                                VARIABLE_ENTRY_CONSISTENCY is the variable data size as input.
                                It will be changed to variable total size as output.

  @retval    TRUE               Have enough variable space to set the Variables successfully.
  @retval    FALSE              No enough variable space to set the Variables successfully.

**/
BOOLEAN
EFIAPI
CheckRemainingSpaceForConsistency (
  IN UINT32                     Attributes,
  ...
  )
{
  VA_LIST Marker;
  BOOLEAN Return;

  VA_START (Marker, Attributes);

  Return = CheckRemainingSpaceForConsistencyInternal (Attributes, Marker);

  VA_END (Marker);

  return Return;
}


/**
  Hook the operations in PlatformLangCodes, LangCodes, PlatformLang and Lang.

  When setting Lang/LangCodes, simultaneously update PlatformLang/PlatformLangCodes.

  According to UEFI spec, PlatformLangCodes/LangCodes are only set once in firmware initialization,
  and are read-only. Therefore, in variable driver, only store the original value for other use.

  @param[in] VariableName          Name of variable.
  @param[in] Data                  Variable data.
  @param[in] DataSize              Size of data. 0 means delete.

  @retval    EFI_SUCCESS           The update operation is successful or ignored.
  @retval    EFI_WRITE_PROTECTED   Update PlatformLangCodes/LangCodes at runtime.
  @retval    EFI_OUT_OF_RESOURCES  No enough variable space to do the update operation.
  @retval    Others                Other errors happened during the update operation.

**/
EFI_STATUS
AutoUpdateLangVariable (
  IN  CHAR16             *VariableName,
  IN  VOID               *Data,
  IN  UINTN              DataSize
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINT32                      Attributes;
  VARIABLE_POINTER_TRACK      Variable;
  BOOLEAN                     SetLanguageCodes;
  VARIABLE_ENTRY_CONSISTENCY  VariableEntry[2];
  CHAR8                       *BestLang;
  CHAR8                       *BestPlatformLang;

  BestLang         = NULL;
  BestPlatformLang = NULL;

  //
  // Don't do updates for delete operation
  //
  if (DataSize == 0) {
    return EFI_SUCCESS;
  }

  SetLanguageCodes = FALSE;

  if (StrCmp (VariableName, EFI_PLATFORM_LANG_CODES_VARIABLE_NAME) == 0) {
    //
    // PlatformLangCodes is a volatile variable, so it can not be updated at runtime.
    //
    if (AtRuntime ()) {
      return EFI_WRITE_PROTECTED;
    }

    SetLanguageCodes = TRUE;

    //
    // According to UEFI spec, PlatformLangCodes is only set once in firmware initialization, and is read-only
    // Therefore, in variable driver, only store the original value for other use.
    //
    if (mVariableModuleGlobal->PlatformLangCodes != NULL) {
      FreePool (mVariableModuleGlobal->PlatformLangCodes);
    }
    mVariableModuleGlobal->PlatformLangCodes = AllocateRuntimeCopyPool (DataSize, Data);
    ASSERT (mVariableModuleGlobal->PlatformLangCodes != NULL);

    //
    // PlatformLang holds a single language from PlatformLangCodes,
    // so the size of PlatformLangCodes is enough for the PlatformLang.
    //
    if (mVariableModuleGlobal->PlatformLang != NULL) {
      FreePool (mVariableModuleGlobal->PlatformLang);
    }
    mVariableModuleGlobal->PlatformLang = AllocateRuntimePool (DataSize);
    ASSERT (mVariableModuleGlobal->PlatformLang != NULL);

  } else if (StrCmp (VariableName, EFI_LANG_CODES_VARIABLE_NAME) == 0) {
    //
    // LangCodes is a volatile variable, so it can not be updated at runtime.
    //
    if (AtRuntime ()) {
      return EFI_WRITE_PROTECTED;
    }

    SetLanguageCodes = TRUE;

    //
    // According to UEFI spec, LangCodes is only set once in firmware initialization, and is read-only
    // Therefore, in variable driver, only store the original value for other use.
    //
    if (mVariableModuleGlobal->LangCodes != NULL) {
      FreePool (mVariableModuleGlobal->LangCodes);
    }
    mVariableModuleGlobal->LangCodes = AllocateRuntimeCopyPool (DataSize, Data);
    ASSERT (mVariableModuleGlobal->LangCodes != NULL);
  }

  if (SetLanguageCodes
      && (mVariableModuleGlobal->PlatformLangCodes != NULL)
      && (mVariableModuleGlobal->LangCodes != NULL)) {
    //
    // Update Lang if PlatformLang is already set
    // Update PlatformLang if Lang is already set
    //
    Status = FindVariable (EFI_PLATFORM_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, FALSE);
    if (!EFI_ERROR (Status)) {
      //
      // Update Lang
      //
      VariableName = EFI_PLATFORM_LANG_VARIABLE_NAME;
      Data         = GetVariableDataPtr (Variable.CurrPtr);
      DataSize     = DataSizeOfVariable (Variable.CurrPtr);
    } else {
      Status = FindVariable (EFI_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, FALSE);
      if (!EFI_ERROR (Status)) {
        //
        // Update PlatformLang
        //
        VariableName = EFI_LANG_VARIABLE_NAME;
        Data         = GetVariableDataPtr (Variable.CurrPtr);
        DataSize     = DataSizeOfVariable (Variable.CurrPtr);
      } else {
        //
        // Neither PlatformLang nor Lang is set, directly return
        //
        return EFI_SUCCESS;
      }
    }
  }

  Status = EFI_SUCCESS;

  //
  // According to UEFI spec, "Lang" and "PlatformLang" is NV|BS|RT attributions.
  //
  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;

  if (StrCmp (VariableName, EFI_PLATFORM_LANG_VARIABLE_NAME) == 0) {
    //
    // Update Lang when PlatformLangCodes/LangCodes were set.
    //
    if ((mVariableModuleGlobal->PlatformLangCodes != NULL) && (mVariableModuleGlobal->LangCodes != NULL)) {
      //
      // When setting PlatformLang, firstly get most matched language string from supported language codes.
      //
      BestPlatformLang = VariableGetBestLanguage (mVariableModuleGlobal->PlatformLangCodes, FALSE, Data, NULL);
      if (BestPlatformLang != NULL) {
        //
        // Get the corresponding index in language codes.
        //
        Index = GetIndexFromSupportedLangCodes (mVariableModuleGlobal->PlatformLangCodes, BestPlatformLang, FALSE);

        //
        // Get the corresponding ISO639 language tag according to RFC4646 language tag.
        //
        BestLang = GetLangFromSupportedLangCodes (mVariableModuleGlobal->LangCodes, Index, TRUE);

        //
        // Check the variable space for both Lang and PlatformLang variable.
        //
        VariableEntry[0].VariableSize = ISO_639_2_ENTRY_SIZE + 1;
        VariableEntry[0].Guid = &gEfiGlobalVariableGuid;
        VariableEntry[0].Name = EFI_LANG_VARIABLE_NAME;

        VariableEntry[1].VariableSize = AsciiStrSize (BestPlatformLang);
        VariableEntry[1].Guid = &gEfiGlobalVariableGuid;
        VariableEntry[1].Name = EFI_PLATFORM_LANG_VARIABLE_NAME;
        if (!CheckRemainingSpaceForConsistency (VARIABLE_ATTRIBUTE_NV_BS_RT, &VariableEntry[0], &VariableEntry[1], NULL)) {
          //
          // No enough variable space to set both Lang and PlatformLang successfully.
          //
          Status = EFI_OUT_OF_RESOURCES;
        } else {
          //
          // Successfully convert PlatformLang to Lang, and set the BestLang value into Lang variable simultaneously.
          //
          FindVariable (EFI_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, FALSE);

          Status = UpdateVariable (EFI_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, BestLang,
                                   ISO_639_2_ENTRY_SIZE + 1, Attributes, 0, 0, &Variable, NULL);
        }

        DEBUG ((EFI_D_INFO, "  Variable Driver: Auto Update PlatformLang, PlatformLang:%a, Lang:%a Status: %r\n", BestPlatformLang, BestLang, Status));
      }
    }

  } else if (StrCmp (VariableName, EFI_LANG_VARIABLE_NAME) == 0) {
    //
    // Update PlatformLang when PlatformLangCodes/LangCodes were set.
    //
    if ((mVariableModuleGlobal->PlatformLangCodes != NULL) && (mVariableModuleGlobal->LangCodes != NULL)) {
      //
      // When setting Lang, firstly get most matched language string from supported language codes.
      //
      BestLang = VariableGetBestLanguage (mVariableModuleGlobal->LangCodes, TRUE, Data, NULL);
      if (BestLang != NULL) {
        //
        // Get the corresponding index in language codes.
        //
        Index = GetIndexFromSupportedLangCodes (mVariableModuleGlobal->LangCodes, BestLang, TRUE);

        //
        // Get the corresponding RFC4646 language tag according to ISO639 language tag.
        //
        BestPlatformLang = GetLangFromSupportedLangCodes (mVariableModuleGlobal->PlatformLangCodes, Index, FALSE);

        if (BestPlatformLang == NULL) {
          ASSERT (BestPlatformLang != NULL);
          return EFI_OUT_OF_RESOURCES;
        }

        //
        // Check the variable space for both PlatformLang and Lang variable.
        //
        VariableEntry[0].VariableSize = AsciiStrSize (BestPlatformLang);
        VariableEntry[0].Guid = &gEfiGlobalVariableGuid;
        VariableEntry[0].Name = EFI_PLATFORM_LANG_VARIABLE_NAME;

        VariableEntry[1].VariableSize = ISO_639_2_ENTRY_SIZE + 1;
        VariableEntry[1].Guid = &gEfiGlobalVariableGuid;
        VariableEntry[1].Name = EFI_LANG_VARIABLE_NAME;
        if (!CheckRemainingSpaceForConsistency (VARIABLE_ATTRIBUTE_NV_BS_RT, &VariableEntry[0], &VariableEntry[1], NULL)) {
          //
          // No enough variable space to set both PlatformLang and Lang successfully.
          //
          Status = EFI_OUT_OF_RESOURCES;
        } else {
          //
          // Successfully convert Lang to PlatformLang, and set the BestPlatformLang value into PlatformLang variable simultaneously.
          //
          FindVariable (EFI_PLATFORM_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, FALSE);

          Status = UpdateVariable (EFI_PLATFORM_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, BestPlatformLang,
                                   AsciiStrSize (BestPlatformLang), Attributes, 0, 0, &Variable, NULL);
        }

        DEBUG ((EFI_D_INFO, "  Variable Driver: Auto Update Lang, Lang:%a, PlatformLang:%a Status: %r\n", BestLang, BestPlatformLang, Status));
      }
    }
  }

  if (SetLanguageCodes) {
    //
    // Continue to set PlatformLangCodes or LangCodes.
    //
    return EFI_SUCCESS;
  } else {
    return Status;
  }
}


/**
  Compare two EFI_TIME data.

  @param[in] FirstTime           A pointer to the first EFI_TIME data.
  @param[in] SecondTime          A pointer to the second EFI_TIME data.

  @retval    TRUE                The FirstTime is not later than the SecondTime.
  @retval    FALSE               The FirstTime is later than the SecondTime.

**/
BOOLEAN
VariableCompareTimeStampInternal (
  IN EFI_TIME               *FirstTime,
  IN EFI_TIME               *SecondTime
  )
{
  if (FirstTime->Year != SecondTime->Year) {
    return (BOOLEAN) (FirstTime->Year < SecondTime->Year);
  } else if (FirstTime->Month != SecondTime->Month) {
    return (BOOLEAN) (FirstTime->Month < SecondTime->Month);
  } else if (FirstTime->Day != SecondTime->Day) {
    return (BOOLEAN) (FirstTime->Day < SecondTime->Day);
  } else if (FirstTime->Hour != SecondTime->Hour) {
    return (BOOLEAN) (FirstTime->Hour < SecondTime->Hour);
  } else if (FirstTime->Minute != SecondTime->Minute) {
    return (BOOLEAN) (FirstTime->Minute < SecondTime->Minute);
  }

  return (BOOLEAN) (FirstTime->Second <= SecondTime->Second);
}


/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in] VariableName          Name of variable.
  @param[in] VendorGuid            Guid of variable.
  @param[in] Data                  Variable data.
  @param[in] DataSize              Size of data. 0 means delete.
  @param[in] Attributes            Attributes of the variable.
  @param[in] KeyIndex              Index of associated public key.
  @param[in] MonotonicCount        Value of associated monotonic count.
  @param[in, out] CacheVariable    The variable information which is used to keep track of variable usage.
  @param[in] TimeStamp             Value of associated TimeStamp.
  @param[in] OnlyUpdateNvCache     TRUE if only the NV cache should be written to, not the VARIABLE_STORAGE_PROTOCOLs

  @retval    EFI_SUCCESS           The update operation is success.
  @retval    EFI_OUT_OF_RESOURCES  Variable region is full, can not write other data into this region.

**/
EFI_STATUS
UpdateVariableInternal (
  IN      CHAR16                      *VariableName,
  IN      EFI_GUID                    *VendorGuid,
  IN      VOID                        *Data,
  IN      UINTN                       DataSize,
  IN      UINT32                      Attributes      OPTIONAL,
  IN      UINT32                      KeyIndex        OPTIONAL,
  IN      UINT64                      MonotonicCount  OPTIONAL,
  IN OUT  VARIABLE_POINTER_TRACK      *CacheVariable,
  IN      EFI_TIME                    *TimeStamp      OPTIONAL,
  IN      BOOLEAN                     OnlyUpdateNvCache
  )
{
  EFI_STATUS                          Status;
  VARIABLE_HEADER                     *NextVariable;
  UINTN                               ScratchSize;
  UINTN                               MaxDataSize;
  UINTN                               VarNameOffset;
  UINTN                               VarDataOffset;
  UINTN                               VarNameSize;
  UINTN                               VarSize;
  BOOLEAN                             Volatile;
  UINT8                               State;
  VARIABLE_POINTER_TRACK              *Variable;
  VARIABLE_HEADER                     *LastVariable;
  VARIABLE_HEADER                     *OldVariable;
  UINTN                               OldVariableSize;
  UINTN                               CacheOffset;
  UINT8                               *BufferForMerge;
  UINTN                               MergedBufSize;
  BOOLEAN                             DataReady;
  UINTN                               DataOffset;
  BOOLEAN                             IsCommonVariable;
  BOOLEAN                             IsCommonUserVariable;
  AUTHENTICATED_VARIABLE_HEADER       *AuthVariable;
  EFI_PHYSICAL_ADDRESS                HobVariableBase;
  VARIABLE_STORAGE_PROTOCOL           *VariableStorageProtocol;

  if (!mVariableModuleGlobal->WriteServiceReady) {
    //
    // NV Variable writes are not ready, so the EFI_VARIABLE_WRITE_ARCH_PROTOCOL is not installed.
    //
    if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
      //
      // Trying to update NV variable prior to the installation of EFI_VARIABLE_WRITE_ARCH_PROTOCOL
      //
      DEBUG ((EFI_D_ERROR, "  Variable Driver: Update NV variable before EFI_VARIABLE_WRITE_ARCH_PROTOCOL ready - %r\n", EFI_NOT_AVAILABLE_YET));
      return EFI_NOT_AVAILABLE_YET;
    } else if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
      //
      // Trying to update volatile authenticated variable prior to the installation of EFI_VARIABLE_WRITE_ARCH_PROTOCOL
      // The authenticated variable perhaps is not initialized, just return here.
      //
      DEBUG ((EFI_D_ERROR, "  Variable Driver: Update AUTH variable before EFI_VARIABLE_WRITE_ARCH_PROTOCOL ready - %r\n", EFI_NOT_AVAILABLE_YET));
      return EFI_NOT_AVAILABLE_YET;
    }
  }

  DEBUG ((DEBUG_INFO, "+-+-> Variable Driver: UpdateVariable.\n  Variable Name: %s.\n  Guid:  %g.\n", VariableName, VendorGuid));

  //
  // Check if CacheVariable points to the variable in variable HOB.
  // If yes, let CacheVariable points to the variable in NV variable cache.
  //
  if ((CacheVariable->CurrPtr != NULL) &&
      (mVariableModuleGlobal->VariableGlobal.HobVariableBase != 0) &&
      (CacheVariable->StartPtr == GetStartPointer ((VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.HobVariableBase))
     ) {
    HobVariableBase                                       = mVariableModuleGlobal->VariableGlobal.HobVariableBase;
    mVariableModuleGlobal->VariableGlobal.HobVariableBase = 0;
    Status = FindVariable (VariableName, VendorGuid, CacheVariable, &mVariableModuleGlobal->VariableGlobal, FALSE);
    mVariableModuleGlobal->VariableGlobal.HobVariableBase = HobVariableBase;
    if (CacheVariable->CurrPtr == NULL || EFI_ERROR (Status)) {
      //
      // There is no matched variable in NV variable cache.
      //
      if ((((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) && (DataSize == 0)) || (Attributes == 0)) {
        //
        // It is to delete variable,
        // go to delete this variable in variable HOB and
        // try to flush other variables from HOB to flash.
        //
        FlushHobVariableToFlash (VariableName, VendorGuid);
        return EFI_SUCCESS;
      }
    }
  }

  Variable = CacheVariable;

  //
  // Tricky part: Use scratch data area at the end of volatile variable store
  // as a temporary storage.
  //
  NextVariable = GetEndPointer ((VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->VariableGlobal.VolatileVariableBase));
  ScratchSize = mVariableModuleGlobal->ScratchBufferSize;
  SetMem (NextVariable, ScratchSize, 0xff);
  DataReady = FALSE;

  if (Variable->CurrPtr != NULL) {
    DEBUG ((DEBUG_INFO, "  Variable Driver: Updating an existing variable (found in the cache).\n"));
    //
    // Update/Delete existing variable.
    //
    if (AtRuntime ()) {
      //
      // If AtRuntime and the variable is Volatile and Runtime Access,
      // the volatile is ReadOnly, and SetVariable should be aborted and
      // return EFI_WRITE_PROTECTED.
      //
      if (Variable->Volatile) {
        Status = EFI_WRITE_PROTECTED;
        goto Done;
      }
      //
      // Only variable that have NV attributes can be updated/deleted in Runtime.
      //
      if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Only variable that have RT attributes can be updated/deleted in Runtime.
      //
      if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
    }

    //
    // Setting a data variable with no access, or zero DataSize attributes
    // causes it to be deleted.
    // When the EFI_VARIABLE_APPEND_WRITE attribute is set, DataSize of zero will
    // not delete the variable.
    //
    if ((((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) && (DataSize == 0)) ||
         ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0)) {
      DEBUG ((DEBUG_INFO, "  Variable Driver: Variable is being deleted.\n"));
      if (Variable->InDeletedTransitionPtr != NULL) {
        //
        // Both ADDED and IN_DELETED_TRANSITION variable are present,
        // set IN_DELETED_TRANSITION one to DELETED state first.
        //
        Variable->InDeletedTransitionPtr->State &= VAR_DELETED;
      }
      Variable->CurrPtr->State &= VAR_DELETED;
      Status                    = EFI_SUCCESS;
      if (!Variable->Volatile && !OnlyUpdateNvCache) {
        DEBUG ((DEBUG_INFO, "  Variable Driver: Variable is being deleted from NV storage.\n"));
        //
        // Delete the variable from the NV storage
        //
        Status = GetVariableStorageProtocol (
                  VariableName,
                  VendorGuid,
                  &VariableStorageProtocol
                  );
        if (!EFI_ERROR (Status)) {
          if (VariableStorageProtocol == NULL) {
            ASSERT (VariableStorageProtocol != NULL);
            return EFI_NOT_FOUND;
          }
          Status = VariableStorageProtocol->SetVariable (
                                              VariableStorageProtocol,
                                              VariableName,
                                              VendorGuid,
                                              Attributes,
                                              0,
                                              NULL,
                                              AtRuntime (),
                                              0,
                                              0,
                                              TimeStamp
                                              );
          DEBUG ((DEBUG_INFO, "  Variable Driver: Value returned from storage protocol = %r.\n", Status));
        }
        OldVariable = GetNextVariablePtr (Variable->CurrPtr);
        OldVariableSize = (UINTN) OldVariable - (UINTN) Variable->CurrPtr;
        if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
          mVariableModuleGlobal->HwErrVariableTotalSize -= OldVariableSize;
        } else if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
          mVariableModuleGlobal->CommonVariableTotalSize -= OldVariableSize;
          if (IsUserVariable (Variable->CurrPtr)) {
            mVariableModuleGlobal->CommonUserVariableTotalSize -= OldVariableSize;
          }
        }
      }
      if (!EFI_ERROR (Status)) {
        UpdateVariableInfo (VariableName, VendorGuid, Variable->Volatile, FALSE, FALSE, TRUE, FALSE);
        FlushHobVariableToFlash (VariableName, VendorGuid);
      }
      goto Done;
    }
    //
    // If the variable is marked valid, and the same data has been passed in,
    // then return to the caller immediately.
    //
    if (DataSizeOfVariable (Variable->CurrPtr) == DataSize &&
        (CompareMem (Data, GetVariableDataPtr (Variable->CurrPtr), DataSize) == 0) &&
        ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) &&
        (TimeStamp == NULL)) {
      //
      // Variable content unchanged and no need to update timestamp, just return.
      //
      UpdateVariableInfo (VariableName, VendorGuid, Variable->Volatile, FALSE, TRUE, FALSE, FALSE);
      Status = EFI_SUCCESS;
      goto Done;
    } else if ((Variable->CurrPtr->State == VAR_ADDED) ||
               (Variable->CurrPtr->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION))) {

      //
      // EFI_VARIABLE_APPEND_WRITE attribute only effects for existing variable.
      //
      if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {
        //
        // NOTE: From 0 to DataOffset of NextVariable is reserved for Variable Header and Name.
        // From DataOffset of NextVariable is to save the existing variable data.
        //
        DataOffset = GetVariableDataOffset (Variable->CurrPtr);
        BufferForMerge = (UINT8 *) ((UINTN) NextVariable + DataOffset);
        CopyMem (BufferForMerge, (UINT8 *) ((UINTN) Variable->CurrPtr + DataOffset), DataSizeOfVariable (Variable->CurrPtr));

        //
        // Set Max Common/Auth Variable Data Size as default MaxDataSize.
        // Max Harware error record variable data size is different from common/auth variable.
        //
        if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
          MaxDataSize = mVariableModuleGlobal->MaxAuthVariableSize - DataOffset;
        } else {
          MaxDataSize = mVariableModuleGlobal->MaxVariableSize - DataOffset;
        }
        if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
          MaxDataSize = PcdGet32 (PcdMaxHardwareErrorVariableSize) - DataOffset;
        }

        if (DataSizeOfVariable (Variable->CurrPtr) + DataSize > MaxDataSize) {
          //
          // Existing data size + new data size exceed maximum variable size limitation.
          //
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }
        //
        // Append the new data to the end of existing data.
        //
        CopyMem ((UINT8*) ((UINTN) BufferForMerge + DataSizeOfVariable (Variable->CurrPtr)), Data, DataSize);
        MergedBufSize = DataSizeOfVariable (Variable->CurrPtr) + DataSize;

        //
        // BufferForMerge(from DataOffset of NextVariable) has included the merged existing and new data.
        //
        Data      = BufferForMerge;
        DataSize  = MergedBufSize;
        DataReady = TRUE;
      }

      //
      // Mark the old variable as in delete transition.
      //
      Variable->CurrPtr->State &= VAR_IN_DELETED_TRANSITION;
    }
  } else {
    DEBUG ((DEBUG_INFO, "  Variable Driver: New variable being written.\n"));
    //
    // Not found existing variable. Create a new variable.
    //

    if ((DataSize == 0) && ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0)) {
      Status = EFI_SUCCESS;
      goto Done;
    }

    //
    // Make sure we are trying to create a new variable.
    // Setting a data variable with zero DataSize or no access attributes means to delete it.
    //
    if (DataSize == 0 || (Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }

    //
    // Only variable have NV|RT attribute can be created in Runtime.
    //
    if (AtRuntime () &&
        (((Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) || ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0))) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
  }

  //
  // Function part - create a new variable and copy the data.
  // Both update a variable and create a variable will come here.
  //
  NextVariable->StartId     = VARIABLE_DATA;
  //
  // NextVariable->State = VAR_ADDED;
  //
  NextVariable->Reserved        = 0;
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) NextVariable;
    AuthVariable->PubKeyIndex    = KeyIndex;
    AuthVariable->MonotonicCount = MonotonicCount;
    ZeroMem (&AuthVariable->TimeStamp, sizeof (EFI_TIME));

    if (((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) &&
        (TimeStamp != NULL)) {
      if ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) {
        CopyMem (&AuthVariable->TimeStamp, TimeStamp, sizeof (EFI_TIME));
      } else {
        //
        // In the case when the EFI_VARIABLE_APPEND_WRITE attribute is set, only
        // when the new TimeStamp value is later than the current timestamp associated
        // with the variable, we need associate the new timestamp with the updated value.
        //
        if (Variable->CurrPtr != NULL) {
          if (VariableCompareTimeStampInternal (&(((AUTHENTICATED_VARIABLE_HEADER *) Variable->CurrPtr)->TimeStamp), TimeStamp)) {
            CopyMem (&AuthVariable->TimeStamp, TimeStamp, sizeof (EFI_TIME));
          }
        }
      }
    }
  }

  //
  // The EFI_VARIABLE_APPEND_WRITE attribute will never be set in the returned
  // Attributes bitmask parameter of a GetVariable() call.
  //
  NextVariable->Attributes  = Attributes & (~EFI_VARIABLE_APPEND_WRITE);

  VarNameOffset                 = GetVariableHeaderSize ();
  VarNameSize                   = StrSize (VariableName);
  CopyMem (
    (UINT8 *) ((UINTN) NextVariable + VarNameOffset),
    VariableName,
    VarNameSize
    );
  VarDataOffset = VarNameOffset + VarNameSize + GET_PAD_SIZE (VarNameSize);

  //
  // If DataReady is TRUE, it means the variable data has been saved into
  // NextVariable during EFI_VARIABLE_APPEND_WRITE operation preparation.
  //
  if (!DataReady) {
    CopyMem (
      (UINT8 *) ((UINTN) NextVariable + VarDataOffset),
      Data,
      DataSize
      );
  }

  CopyMem (GetVendorGuidPtr (NextVariable), VendorGuid, sizeof (EFI_GUID));
  //
  // There will be pad bytes after Data, the NextVariable->NameSize and
  // NextVariable->DataSize should not include pad size so that variable
  // service can get actual size in GetVariable.
  //
  SetNameSizeOfVariable (NextVariable, VarNameSize);
  SetDataSizeOfVariable (NextVariable, DataSize);

  //
  // The actual size of the variable that stores in storage should
  // include pad size.
  //
  VarSize = VarDataOffset + DataSize + GET_PAD_SIZE (DataSize);
  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
    //
    // Create a nonvolatile variable.
    //
    Volatile = FALSE;

    IsCommonVariable = FALSE;
    IsCommonUserVariable = FALSE;
    LastVariable  = GetStartPointer (mNvVariableCache);
    while (IsValidVariableHeader (LastVariable, GetEndPointer (mNvVariableCache))) {
      LastVariable = GetNextVariablePtr (LastVariable);
    }
    CacheOffset = (UINTN) LastVariable - (UINTN) mNvVariableCache;
    if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == 0) {
      IsCommonVariable = TRUE;
      IsCommonUserVariable = IsUserVariable (NextVariable);
    }
    if ((((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != 0)
      && ((VarSize + mVariableModuleGlobal->HwErrVariableTotalSize) > PcdGet32 (PcdHwErrStorageSize)))
      || (IsCommonVariable && ((VarSize + mVariableModuleGlobal->CommonVariableTotalSize) > mVariableModuleGlobal->CommonVariableSpace))
      || (IsCommonVariable && AtRuntime () && ((VarSize + mVariableModuleGlobal->CommonVariableTotalSize) > mVariableModuleGlobal->CommonRuntimeVariableSpace))
      || (IsCommonUserVariable && ((VarSize + mVariableModuleGlobal->CommonUserVariableTotalSize) > mVariableModuleGlobal->CommonMaxUserVariableSpace))
      || (UINT32) (VarSize + CacheOffset) > mNvVariableCache->Size) {
      if (AtRuntime ()) {
        if (IsCommonUserVariable && ((VarSize + mVariableModuleGlobal->CommonUserVariableTotalSize) > mVariableModuleGlobal->CommonMaxUserVariableSpace)) {
          RecordVarErrorFlag (VAR_ERROR_FLAG_USER_ERROR, VariableName, VendorGuid, Attributes, VarSize);
        }
        if (IsCommonVariable && ((VarSize + mVariableModuleGlobal->CommonVariableTotalSize) > mVariableModuleGlobal->CommonRuntimeVariableSpace)) {
          RecordVarErrorFlag (VAR_ERROR_FLAG_SYSTEM_ERROR, VariableName, VendorGuid, Attributes, VarSize);
        }
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      //
      // Perform garbage collection & reclaim operation, and integrate the new variable at the same time.
      //
      Status = Reclaim (
                 (EFI_PHYSICAL_ADDRESS) (UINTN) mNvVariableCache,
                 &CacheOffset,
                 FALSE,
                 Variable,
                 NextVariable,
                 HEADER_ALIGN (VarSize)
                 );
      if (!EFI_ERROR (Status)) {
        //
        // The new variable has been integrated successfully during reclaiming.
        //
        if (Variable->CurrPtr != NULL) {
          CacheVariable->CurrPtr = (VARIABLE_HEADER *)((UINTN) CacheVariable->StartPtr + ((UINTN) Variable->CurrPtr - (UINTN) Variable->StartPtr));
          CacheVariable->InDeletedTransitionPtr = NULL;
        }
        UpdateVariableInfo (VariableName, VendorGuid, FALSE, FALSE, TRUE, FALSE, FALSE);
        FlushHobVariableToFlash (VariableName, VendorGuid);
      } else {
        if (IsCommonUserVariable && ((VarSize + mVariableModuleGlobal->CommonUserVariableTotalSize) > mVariableModuleGlobal->CommonMaxUserVariableSpace)) {
          RecordVarErrorFlag (VAR_ERROR_FLAG_USER_ERROR, VariableName, VendorGuid, Attributes, VarSize);
        }
        if (IsCommonVariable && ((VarSize + mVariableModuleGlobal->CommonVariableTotalSize) > mVariableModuleGlobal->CommonVariableSpace)) {
          RecordVarErrorFlag (VAR_ERROR_FLAG_SYSTEM_ERROR, VariableName, VendorGuid, Attributes, VarSize);
        }
      }
      goto Done;
    }
    //
    // Write the variable to NV
    //
    if (!OnlyUpdateNvCache) {
      Status = GetVariableStorageProtocol (
                VariableName,
                VendorGuid,
                &VariableStorageProtocol
                );
      if (EFI_ERROR (Status) || VariableStorageProtocol == NULL) {
        goto Done;
      }
      Status = VariableStorageProtocol->SetVariable (
                                          VariableStorageProtocol,
                                          VariableName,
                                          VendorGuid,
                                          Attributes,
                                          DataSize,
                                          Data,
                                          AtRuntime (),
                                          KeyIndex,
                                          MonotonicCount,
                                          TimeStamp
                                          );
      DEBUG ((DEBUG_INFO, "  Variable Driver: Variable was written to NV via the storage protocol. Status = %r.\n", Status));
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != 0) {
        mVariableModuleGlobal->HwErrVariableTotalSize += HEADER_ALIGN (VarSize);
      } else {
        mVariableModuleGlobal->CommonVariableTotalSize += HEADER_ALIGN (VarSize);
        if (IsCommonUserVariable) {
          mVariableModuleGlobal->CommonUserVariableTotalSize += HEADER_ALIGN (VarSize);
        }
      }
    }
    //
    // Update the NV Cache
    //
    NextVariable->State = VAR_ADDED;
    Status = UpdateVariableStore (
               &mVariableModuleGlobal->VariableGlobal,
               FALSE,
               TRUE,
               CacheOffset,
               (UINT32) VarSize,
               (UINT8 *) NextVariable
               );
    DEBUG ((DEBUG_INFO, "  Variable Driver: Wrote the variable to the NV cache in UpdateVariable().\n"));
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  } else {
    //
    // Create a volatile variable.
    //
    Volatile = TRUE;

    DEBUG ((DEBUG_INFO, "  Variable Driver: Creating a volatile variable.\n"));

    if ((UINT32) (VarSize + mVariableModuleGlobal->VolatileLastVariableOffset) >
        ((VARIABLE_STORE_HEADER *) ((UINTN) (mVariableModuleGlobal->VariableGlobal.VolatileVariableBase)))->Size) {
      //
      // Perform garbage collection & reclaim operation, and integrate the new variable at the same time.
      //
      Status = Reclaim (
                 mVariableModuleGlobal->VariableGlobal.VolatileVariableBase,
                 &mVariableModuleGlobal->VolatileLastVariableOffset,
                 TRUE,
                 Variable,
                 NextVariable,
                 HEADER_ALIGN (VarSize)
                 );
      if (!EFI_ERROR (Status)) {
        //
        // The new variable has been integrated successfully during reclaiming.
        //
        if (Variable->CurrPtr != NULL) {
          CacheVariable->CurrPtr = (VARIABLE_HEADER *) ((UINTN) CacheVariable->StartPtr + ((UINTN) Variable->CurrPtr - (UINTN) Variable->StartPtr));
          CacheVariable->InDeletedTransitionPtr = NULL;
        }
        UpdateVariableInfo (VariableName, VendorGuid, TRUE, FALSE, TRUE, FALSE, FALSE);
      }
      goto Done;
    }

    NextVariable->State = VAR_ADDED;
    Status = UpdateVariableStore (
               &mVariableModuleGlobal->VariableGlobal,
               TRUE,
               TRUE,
               mVariableModuleGlobal->VolatileLastVariableOffset,
               (UINT32) VarSize,
               (UINT8 *) NextVariable
               );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    mVariableModuleGlobal->VolatileLastVariableOffset += HEADER_ALIGN (VarSize);
  }

  //
  // Mark the old variable as deleted.
  //
  if (!EFI_ERROR (Status) && Variable->CurrPtr != NULL) {
    //
    // Reduce the current NV storage usage counts by the old variable's size
    //
    if (!OnlyUpdateNvCache) {
      OldVariable = GetNextVariablePtr (Variable->CurrPtr);
      OldVariableSize = (UINTN) OldVariable - (UINTN) Variable->CurrPtr;
      if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        mVariableModuleGlobal->HwErrVariableTotalSize -= OldVariableSize;
      } else if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        mVariableModuleGlobal->CommonVariableTotalSize -= OldVariableSize;
        if (IsUserVariable (Variable->CurrPtr)) {
          mVariableModuleGlobal->CommonUserVariableTotalSize -= OldVariableSize;
        }
      }
    }
    if (Variable->InDeletedTransitionPtr != NULL) {
      //
      // Both ADDED and IN_DELETED_TRANSITION old variable are present,
      // set IN_DELETED_TRANSITION one to DELETED state first.
      //
      State = Variable->InDeletedTransitionPtr->State;
      State &= VAR_DELETED;
      Status = UpdateVariableStore (
                 &mVariableModuleGlobal->VariableGlobal,
                 Variable->Volatile,
                 FALSE,
                 (UINTN) &Variable->InDeletedTransitionPtr->State,
                 sizeof (UINT8),
                 &State
                 );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }

    State = Variable->CurrPtr->State;
    State &= VAR_DELETED;

    Status = UpdateVariableStore (
               &mVariableModuleGlobal->VariableGlobal,
               Variable->Volatile,
               FALSE,
               (UINTN) &Variable->CurrPtr->State,
               sizeof (UINT8),
               &State
               );
  }

  if (!EFI_ERROR (Status)) {
    UpdateVariableInfo (VariableName, VendorGuid, Volatile, FALSE, TRUE, FALSE, FALSE);
    if (!Volatile) {
      FlushHobVariableToFlash (VariableName, VendorGuid);
    }
  }

Done:
  return Status;
}


/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in]      VariableName          Name of variable.
  @param[in]      VendorGuid            Guid of variable.
  @param[in]      Data                  Variable data.
  @param[in]      DataSize              Size of data. 0 means delete.
  @param[in]      Attributes            Attributes of the variable.
  @param[in]      KeyIndex              Index of associated public key.
  @param[in]      MonotonicCount        Value of associated monotonic count.
  @param[in, out] CacheVariable         The variable information which is used to keep track of variable usage.
  @param[in]      TimeStamp             Value of associated TimeStamp.

  @retval         EFI_SUCCESS           The update operation is success.
  @retval         EFI_OUT_OF_RESOURCES  Variable region is full, can not write other data into this region.

**/
EFI_STATUS
UpdateVariable (
  IN      CHAR16                      *VariableName,
  IN      EFI_GUID                    *VendorGuid,
  IN      VOID                        *Data,
  IN      UINTN                       DataSize,
  IN      UINT32                      Attributes      OPTIONAL,
  IN      UINT32                      KeyIndex        OPTIONAL,
  IN      UINT64                      MonotonicCount  OPTIONAL,
  IN OUT  VARIABLE_POINTER_TRACK      *CacheVariable,
  IN      EFI_TIME                    *TimeStamp      OPTIONAL
  )
{
  return UpdateVariableInternal (
           VariableName,
           VendorGuid,
           Data,
           DataSize,
           Attributes,
           KeyIndex,
           MonotonicCount,
           CacheVariable,
           TimeStamp,
           mForceVolatileVariable
           );
}

/**
  Finds the first variable from the first NV storage.

  @param[out]  PtrTrack               VARIABLE_POINTER_TRACK structure for output,
                                      including the range searched and the target position.
  @param[in]   Global                 Pointer to VARIABLE_GLOBAL structure, including
                                      base of volatile variable storage area, base of
                                      NV variable storage area, and a lock.
  @param[in]   IgnoreRtCheck          Ignore EFI_VARIABLE_RUNTIME_ACCESS attribute
                                      check at runtime when searching variable.

  @retval      EFI_INVALID_PARAMETER  If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval      EFI_SUCCESS            Variable successfully found.
  @retval      EFI_NOT_FOUND          Variable not found.

**/
EFI_STATUS
FindFirstNvVariable (
  OUT VARIABLE_POINTER_TRACK  *PtrTrack,
  IN  VARIABLE_GLOBAL         *Global,
  IN  BOOLEAN                 IgnoreRtCheck
  )
{
  VARIABLE_STORAGE_PROTOCOL   *VariableStorage;
  UINTN                       VariableNameBufferSize;
  UINT32                      VariableAttributes;
  EFI_GUID                    TempGuid;
  EFI_STATUS                  Status;

  if (mVariableModuleGlobal->VariableGlobal.VariableStoresCount <= 0) {
    return EFI_NOT_FOUND;
  }
  VariableNameBufferSize = sizeof (mVariableNameBuffer);
  ZeroMem ((VOID *) &mVariableNameBuffer[0], VariableNameBufferSize);
  ZeroMem ((VOID *) &TempGuid, sizeof (EFI_GUID));
  VariableStorage = mVariableModuleGlobal->VariableGlobal.VariableStores[0];
  Status = VariableStorage->GetNextVariableName (
                              VariableStorage,
                              &VariableNameBufferSize,
                              &mVariableNameBuffer[0],
                              &TempGuid,
                              &VariableAttributes
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "VariableStorageProtocol->GetNextVariableName status %r\n", Status));
    ASSERT (Status != EFI_BUFFER_TOO_SMALL);
    return Status;
  }
  return FindVariable (&mVariableNameBuffer[0], &TempGuid, PtrTrack, Global, IgnoreRtCheck);
}


/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  If IgnoreRtCheck is TRUE, then we ignore the EFI_VARIABLE_RUNTIME_ACCESS attribute check
  at runtime when searching existing variable, only VariableName and VendorGuid are compared.
  Otherwise, variables without EFI_VARIABLE_RUNTIME_ACCESS are not visible at runtime.

  @param[in]   VariableName           Name of the variable to be found.
  @param[in]   VendorGuid             Vendor GUID to be found.
  @param[out]  PtrTrack               VARIABLE_POINTER_TRACK structure for output,
                                      including the range searched and the target position.
  @param[in]   Global                 Pointer to VARIABLE_GLOBAL structure, including
                                      base of volatile variable storage area, base of
                                      NV variable storage area, and a lock.
  @param[in]   IgnoreRtCheck          Ignore EFI_VARIABLE_RUNTIME_ACCESS attribute
                                      check at runtime when searching variable.

  @retval      EFI_INVALID_PARAMETER  If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval      EFI_SUCCESS            Variable successfully found.
  @retval      EFI_NOT_FOUND          Variable not found.

**/
EFI_STATUS
FindVariable (
  IN  CHAR16                  *VariableName,
  IN  EFI_GUID                *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack,
  IN  VARIABLE_GLOBAL         *Global,
  IN  BOOLEAN                 IgnoreRtCheck
  )
{
  EFI_STATUS                  Status;
  EFI_STATUS                  Status2;
  VARIABLE_STORE_HEADER       *VariableStoreHeader[VariableStoreTypeMax];
  EFI_GUID                    VariableStorageId;
  EFI_GUID                    InstanceGuid;
  VARIABLE_STORAGE_PROTOCOL   *VariableStorageProtocol;
  VARIABLE_STORAGE_PROTOCOL   *CorrectVariableStorageProtocol;
  UINTN                       Index;
  VARIABLE_STORE_TYPE         Type;
  EFI_TIME                    TimeStamp;
  UINT64                      MonotonicCount;
  UINTN                       DataSize;
  UINT32                      Attributes;
  UINT32                      KeyIndex;
  BOOLEAN                     FailedRtCheck;

  FailedRtCheck = FALSE;
  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // 0: Volatile, 1: HOB, 2: Non-Volatile Cache.
  // The index and attributes mapping must be kept in this order as RuntimeServiceGetNextVariableName
  // make use of this mapping to implement search algorithm.
  //
  VariableStoreHeader[VariableStoreTypeVolatile] = (VARIABLE_STORE_HEADER *) (UINTN) Global->VolatileVariableBase;
  VariableStoreHeader[VariableStoreTypeHob]      = (VARIABLE_STORE_HEADER *) (UINTN) Global->HobVariableBase;
  VariableStoreHeader[VariableStoreTypeNvCache]  = mNvVariableCache;

  DEBUG ((DEBUG_INFO, "+-+-> Variable Driver: FindVariable.\n  Variable Name: %s.\n  Guid:  %g.\n", VariableName, VendorGuid));

  //
  // Find the variable by walk through HOB, volatile and non-volatile variable store.
  //
  for (Type = (VARIABLE_STORE_TYPE) 0; Type < VariableStoreTypeMax; Type++) {
    if (VariableStoreHeader[Type] == NULL) {
      continue;
    }

    PtrTrack->StartPtr = GetStartPointer (VariableStoreHeader[Type]);
    PtrTrack->EndPtr   = GetEndPointer   (VariableStoreHeader[Type]);
    PtrTrack->Volatile = (BOOLEAN) (Type == VariableStoreTypeVolatile);

    Status = FindVariableEx (VariableName, VendorGuid, TRUE, PtrTrack);
    if (!EFI_ERROR (Status)) {
      FailedRtCheck = FALSE;
      if (!IgnoreRtCheck &&
          ((PtrTrack->CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) &&
          AtRuntime ()) {
        FailedRtCheck = TRUE;
        continue;
      }
      DEBUG ((DEBUG_INFO, "Variable Driver: Found the variable in store type %d before going to protocols.\n", Type));
      return Status;
    }
  }
  if (FailedRtCheck) {
    PtrTrack->CurrPtr                 = NULL;
    PtrTrack->InDeletedTransitionPtr  = NULL;
    return EFI_NOT_FOUND;
  }
  if (mForceVolatileVariable) {
    return EFI_NOT_FOUND;
  }

  //
  // If VariableName is an empty string get the first variable from the first NV storage
  //
  if (VariableName[0] == 0) {
    return FindFirstNvVariable (PtrTrack, Global, IgnoreRtCheck);
  }
  //
  // Search the VARIABLE_STORAGE_PROTOCOLs
  // first, try the protocol instance which the VariableStorageSelectorLib suggests
  //
  ASSERT (mVariableDataBuffer != NULL);
  if (mVariableDataBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CorrectVariableStorageProtocol = NULL;
  ZeroMem ((VOID *) &VariableStorageId, sizeof (EFI_GUID));
  Status = GetVariableStorageId (VariableName, VendorGuid, TRUE, &VariableStorageId);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for ( Index = 0;
        Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
        Index++) {
    VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
    ZeroMem ((VOID *) &InstanceGuid, sizeof (EFI_GUID));
    Status = VariableStorageProtocol->GetId (&InstanceGuid);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (CompareGuid (&VariableStorageId, &InstanceGuid)) {
      CorrectVariableStorageProtocol = VariableStorageProtocol;
      DataSize = mVariableModuleGlobal->ScratchBufferSize;
      ZeroMem (mVariableDataBuffer, DataSize);
      Status = VariableStorageProtocol->GetAuthenticatedVariable (
                                          VariableStorageProtocol,
                                          VariableName,
                                          VendorGuid,
                                          &Attributes,
                                          &DataSize,
                                          mVariableDataBuffer,
                                          &KeyIndex,
                                          &MonotonicCount,
                                          &TimeStamp
                                          );
      DEBUG ((DEBUG_INFO, "  Variable Driver: Variable storage protocol GetAuthenticatedVariable status = %r.\n", Status));

      if (!EFI_ERROR (Status)) {
        goto UpdateNvCache;
      }
      ASSERT (Status != EFI_BUFFER_TOO_SMALL);
      if (Status != EFI_NOT_FOUND) {
        return Status;
      }
      break;
    }
  }
  //
  // As a fallback, try searching the remaining VARIABLE_STORAGE_PROTOCOLs even
  // though the variable shouldn't actually be stored in there
  //
  for ( Index = 0;
        Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
        Index++) {
    VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
    ZeroMem ((VOID *) &InstanceGuid, sizeof (EFI_GUID));
    Status = VariableStorageProtocol->GetId (&InstanceGuid);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (!CompareGuid (&VariableStorageId, &VariableStorageId)) {
      DataSize = mVariableModuleGlobal->ScratchBufferSize;
      ZeroMem (mVariableDataBuffer, DataSize);
      Status = VariableStorageProtocol->GetAuthenticatedVariable (
                                          VariableStorageProtocol,
                                          VariableName,
                                          VendorGuid,
                                          &Attributes,
                                          &DataSize,
                                          mVariableDataBuffer,
                                          &KeyIndex,
                                          &MonotonicCount,
                                          &TimeStamp
                                          );
      if (!EFI_ERROR (Status)) {
        //
        // The variable isn't being stored in the correct VariableStorage,
        // attempt to move it to the correct VariableStorage
        //
        DEBUG ((
          DEBUG_INFO,
          "Variable %s is not being stored in the correct NV storage!\n",
          VariableName
          ));
        DEBUG ((
            DEBUG_INFO,
            "Expected StorageId = %g, Actual StorageId = %g\n",
            &VariableStorageId,
            &VariableStorageId
            ));
        if (CorrectVariableStorageProtocol != NULL) {
          if (CorrectVariableStorageProtocol->WriteServiceIsReady (
                                                CorrectVariableStorageProtocol) &&
              VariableStorageProtocol->WriteServiceIsReady (
                                        VariableStorageProtocol)) {
            Status2 = CorrectVariableStorageProtocol->SetVariable (
                                                        CorrectVariableStorageProtocol,
                                                        VariableName,
                                                        VendorGuid,
                                                        Attributes,
                                                        DataSize,
                                                        mVariableDataBuffer,
                                                        AtRuntime (),
                                                        KeyIndex,
                                                        MonotonicCount,
                                                        &TimeStamp
                                                        );
            if (EFI_ERROR (Status2)) {
              DEBUG ((DEBUG_INFO, "Failed to copy variable to correct VariableStorage!\n"));
              goto UpdateNvCache;
            }
            //
            // Delete the redundant copy that is incorrectly stored
            //
            Status2 = VariableStorageProtocol->SetVariable (
                                                 VariableStorageProtocol,
                                                 VariableName,
                                                 VendorGuid,
                                                 Attributes,
                                                 0,
                                                 NULL,
                                                 AtRuntime (),
                                                 0,
                                                 0,
                                                 &TimeStamp
                                                 );
            if (EFI_ERROR (Status2)) {
              DEBUG ((DEBUG_INFO, "  Variable Driver: Failed to delete redundant copy of variable in the incorrect VariableStorage!\n"));
            }
            DEBUG ((DEBUG_INFO, "  Variable Driver: Variable has been moved to the correct VariableStorage.\n"));
          } else {
            DEBUG ((DEBUG_INFO, "  Variable Driver: VariableStorage is not ready to write, unable to move variable.\n"));
          }
        } else {
          DEBUG ((DEBUG_INFO, "  Variable Driver: Expected VariableStorage does not exist!\n"));
        }
        goto UpdateNvCache;
      }
      ASSERT (Status != EFI_BUFFER_TOO_SMALL);
      if (Status != EFI_NOT_FOUND) {
        return Status;
      }
    }
  }
  return EFI_NOT_FOUND;
UpdateNvCache:
  DEBUG ((DEBUG_INFO, "  Variable Driver: Updating the cache for this variable.\n"));

  PtrTrack->CurrPtr                 = NULL;
  PtrTrack->InDeletedTransitionPtr  = NULL;
  PtrTrack->StartPtr                = GetStartPointer (mNvVariableCache);
  PtrTrack->EndPtr                  = GetEndPointer   (mNvVariableCache);
  PtrTrack->Volatile                = FALSE;
  Status = UpdateVariableInternal (
             VariableName,
             VendorGuid,
             mVariableDataBuffer,
             DataSize,
             Attributes,
             KeyIndex,
             MonotonicCount,
             PtrTrack,
             &TimeStamp,
             TRUE
             );
  DEBUG ((DEBUG_INFO, "  Variable Driver: UpdateVariable status = %r.\n", Status));
  if (!EFI_ERROR (Status)) {
    PtrTrack->StartPtr = GetStartPointer (mNvVariableCache);
    PtrTrack->EndPtr   = GetEndPointer   (mNvVariableCache);
    PtrTrack->Volatile = FALSE;
    Status = FindVariableEx (VariableName, VendorGuid, IgnoreRtCheck, PtrTrack);
  }
  return Status;
}


/**
  This code finds variable in storage blocks (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize is external input.
  This function will do basic validation, before parse the data.

  @param[in]      VariableName               Name of Variable to be found.
  @param[in]      VendorGuid                 Variable vendor GUID.
  @param[out]     Attributes                 Attribute value of the variable found.
  @param[in, out] DataSize                   Size of Data found. If size is less than the
                                             data, this value contains the required size.
  @param[out]     Data                       Data pointer.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VarDataSize;

  if (VariableName == NULL || VendorGuid == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AcquireLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  Status = FindVariable (VariableName, VendorGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, FALSE);
  if (Variable.CurrPtr == NULL || EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Get data size
  //
  VarDataSize = DataSizeOfVariable (Variable.CurrPtr);
  ASSERT (VarDataSize != 0);

  if (*DataSize >= VarDataSize) {
    if (Data == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }

    CopyMem (Data, GetVariableDataPtr (Variable.CurrPtr), VarDataSize);
    if (Attributes != NULL) {
      *Attributes = Variable.CurrPtr->Attributes;
    }

    *DataSize = VarDataSize;
    UpdateVariableInfo (VariableName, VendorGuid, Variable.Volatile, TRUE, FALSE, FALSE, FALSE);

    Status = EFI_SUCCESS;
    goto Done;
  } else {
    *DataSize = VarDataSize;
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

Done:
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);
  return Status;
}


/**
  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in]  VariableName      Pointer to variable name.
  @param[in]  VendorGuid        Variable Vendor Guid.
  @param[out] VariablePtr       Pointer to variable header address.

  @return     EFI_SUCCESS       Find the specified variable.
  @return     EFI_NOT_FOUND     Not found.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextInMemoryVariableInternal (
  IN  CHAR16                *VariableName,
  IN  EFI_GUID              *VendorGuid,
  OUT VARIABLE_HEADER       **VariablePtr,
  OUT BOOLEAN               *CurrentVariableInMemory
  )
{
  VARIABLE_STORE_TYPE     Type;
  VARIABLE_STORE_TYPE     MaxSearch;
  VARIABLE_POINTER_TRACK  Variable;
  VARIABLE_POINTER_TRACK  VariablePtrTrack;
  EFI_STATUS              Status;
  VARIABLE_STORE_HEADER   *VariableStoreHeader[VariableStoreTypeMax];

  *CurrentVariableInMemory = FALSE;
  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // 0: Volatile, 1: HOB
  // The index and attributes mapping must be kept in this order to implement
  // the variable enumeration search algorithm.
  //
  VariableStoreHeader[VariableStoreTypeVolatile] = (VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.VolatileVariableBase;
  VariableStoreHeader[VariableStoreTypeHob]      = (VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.HobVariableBase;
  VariableStoreHeader[VariableStoreTypeNvCache]  = mNvVariableCache;
  if (mForceVolatileVariable) {
    MaxSearch = VariableStoreTypeNvCache;
  } else {
    MaxSearch = VariableStoreTypeHob;
  }

  //
  // Find the current variable by walk through HOB and volatile variable store.
  //
  Variable.CurrPtr  = NULL;
  Status            = EFI_NOT_FOUND;
  for (Type = (VARIABLE_STORE_TYPE) 0; Type <= MaxSearch; Type++) {
    if (VariableStoreHeader[Type] == NULL) {
      continue;
    }

    Variable.StartPtr = GetStartPointer (VariableStoreHeader[Type]);
    Variable.EndPtr   = GetEndPointer   (VariableStoreHeader[Type]);
    Variable.Volatile = (BOOLEAN) (Type == VariableStoreTypeVolatile);

    Status = FindVariableEx (VariableName, VendorGuid, FALSE, &Variable);
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  if (Variable.CurrPtr == NULL || EFI_ERROR (Status)) {
    goto Done;
  }
  *CurrentVariableInMemory = TRUE;

  if (VariableName[0] != 0) {
    //
    // If variable name is not NULL, get next variable.
    //
    Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
  }

  while (TRUE) {
    //
    // Switch from Volatile to HOB
    //
    while (!IsValidVariableHeader (Variable.CurrPtr, Variable.EndPtr)) {
      //
      // Find current storage index
      //
      for (Type = (VARIABLE_STORE_TYPE) 0; Type <= VariableStoreTypeHob; Type++) {
        if ((VariableStoreHeader[Type] != NULL) && (Variable.StartPtr == GetStartPointer (VariableStoreHeader[Type]))) {
          break;
        }
      }
      ASSERT (Type <= VariableStoreTypeHob);
      //
      // Switch to next storage
      //
      for (Type++; Type < VariableStoreTypeMax; Type++) {
        if (VariableStoreHeader[Type] != NULL) {
          break;
        }
      }
      //
      // Capture the case that
      // 1. current storage is the last one, or
      // 2. no further storage
      //
      if (Type > VariableStoreTypeHob) {
        Status = EFI_NOT_FOUND;
        goto Done;
      }
      Variable.StartPtr = GetStartPointer (VariableStoreHeader[Type]);
      Variable.EndPtr   = GetEndPointer   (VariableStoreHeader[Type]);
      Variable.CurrPtr  = Variable.StartPtr;
    }

    //
    // Variable is found
    //
    if (Variable.CurrPtr->State == VAR_ADDED || Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      if (!AtRuntime () || ((Variable.CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) != 0)) {
        if (Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          //
          // If it is a IN_DELETED_TRANSITION variable,
          // and there is also a same ADDED one at the same time,
          // don't return it.
          //
          VariablePtrTrack.StartPtr = Variable.StartPtr;
          VariablePtrTrack.EndPtr = Variable.EndPtr;
          Status = FindVariableEx (
                     GetVariableNamePtr (Variable.CurrPtr),
                     GetVendorGuidPtr (Variable.CurrPtr),
                     FALSE,
                     &VariablePtrTrack
                     );
          if (!EFI_ERROR (Status) && VariablePtrTrack.CurrPtr->State == VAR_ADDED) {
            Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
            continue;
          }
        }

        *VariablePtr = Variable.CurrPtr;
        Status = EFI_SUCCESS;
        goto Done;
      }
    }

    Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
  }

Done:
  return Status;
}


/**
  Determines if a variable exists in the default HOB

  @param[in]  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param[in]  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                    VariableGuid and VariableName must be unique.

  @retval     TRUE                  The variable exists in the HOBs
  @retval     FALSE                 The variable does not exist in the HOBs

**/
BOOLEAN
EFIAPI
VariableExistsInHob (
  IN CONST  CHAR16                          *VariableName,
  IN CONST  EFI_GUID                        *VariableGuid
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;

  if (mVariableModuleGlobal->VariableGlobal.HobVariableBase == 0) {
    return FALSE;
  }
  Variable.CurrPtr  = NULL;
  Variable.StartPtr = GetStartPointer ((VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.HobVariableBase);
  Variable.EndPtr   = GetEndPointer   ((VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.HobVariableBase);
  Variable.Volatile = FALSE;

  Status = FindVariableEx ((CHAR16 *) VariableName, (EFI_GUID *) VariableGuid, FALSE, &Variable);
  if ((Variable.CurrPtr != NULL) && (!EFI_ERROR (Status))) {
    return TRUE;
  } else {
    if (Status == EFI_NOT_FOUND) {
      return FALSE;
    }
    ASSERT_EFI_ERROR (Status);
    return FALSE;
  }
}


/**
  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in, out] VariableNameSize   Size of the variable name.
  @param[in, out] VariableName       Pointer to variable name.
  @param[in, out] VendorGuid         Variable Vendor Guid.

  @return         EFI_SUCCESS        Find the specified variable.
  @return         EFI_NOT_FOUND      Not found.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextVariableInternal (
  IN OUT UINTN              *VariableNameSize,
  IN OUT CHAR16             *VariableName,
  IN OUT EFI_GUID           *VariableGuid
  )
{
  EFI_GUID                    VariableStorageId;
  EFI_GUID                    InstanceGuid;
  VARIABLE_STORAGE_PROTOCOL   *VariableStorage;
  VARIABLE_HEADER             *VariablePtr;
  UINTN                       Index;
  UINTN                       VarNameSize;
  UINT32                      VarAttributes;
  UINTN                       CallerVariableNameBufferSize;
  EFI_STATUS                  Status;
  BOOLEAN                     SearchComplete;
  BOOLEAN                     CurrentVariableInMemory;

  CallerVariableNameBufferSize =  *VariableNameSize;

  //
  // Check the volatile and HOB variables first
  //
  Status = VariableServiceGetNextInMemoryVariableInternal (
             VariableName,
             VariableGuid,
             &VariablePtr,
             &CurrentVariableInMemory
             );
  if (!EFI_ERROR (Status)) {
    VarNameSize = NameSizeOfVariable (VariablePtr);
    ASSERT (VarNameSize != 0);
    if (VarNameSize <= *VariableNameSize) {
      CopyMem (VariableName, GetVariableNamePtr (VariablePtr), VarNameSize);
      CopyMem (VariableGuid, GetVendorGuidPtr (VariablePtr), sizeof (EFI_GUID));
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_BUFFER_TOO_SMALL;
    }

    *VariableNameSize = VarNameSize;
    return Status;
  } else if (Status != EFI_NOT_FOUND) {
    DEBUG ((DEBUG_INFO, "VariableServiceGetNextInMemoryVariableInternal status %r\n", Status));
    return Status;
  }

  if (mForceVolatileVariable) {
    return EFI_NOT_FOUND;
  }

  //
  // If VariableName is an empty string or we reached the end of the volatile
  // and HOB variables, get the first variable from the first NV storage
  //
  if (VariableName[0] == 0 || (Status == EFI_NOT_FOUND && CurrentVariableInMemory)) {
    if (mVariableModuleGlobal->VariableGlobal.VariableStoresCount <= 0) {
      return EFI_NOT_FOUND;
    }
    ZeroMem ((VOID *) VariableName, *VariableNameSize);
    ZeroMem ((VOID *) VariableGuid, sizeof (VariableGuid));
    VariableStorage = mVariableModuleGlobal->VariableGlobal.VariableStores[0];
    Status = VariableStorage->GetNextVariableName (
                                VariableStorage,
                                VariableNameSize,
                                VariableName,
                                VariableGuid,
                                &VarAttributes
                                );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "VariableStorageProtocol->GetNextVariableName status %r\n", Status));
      return Status;
    } else {
      //
      // Don't return this variable if we are at runtime and the variable's attributes do not include
      // the EFI_VARIABLE_RUNTIME_ACCESS flag. If this is true, advance to the next variable
      //
      if (((VarAttributes & EFI_VARIABLE_RUNTIME_ACCESS) != 0) || !AtRuntime ()) {
        return Status;
      }
    }
  }

  SearchComplete = FALSE;
  while (!SearchComplete) {
    Status = GetVariableStorageId (VariableName, VariableGuid, TRUE, &VariableStorageId);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    VarNameSize = CallerVariableNameBufferSize;

    for ( Index = 0;
          Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
          Index++) {
      VariableStorage = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
      ZeroMem ((VOID *) &InstanceGuid, sizeof (EFI_GUID));
      Status = VariableStorage->GetId (&InstanceGuid);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (CompareGuid (&VariableStorageId, &InstanceGuid)) {
        Status = VariableStorage->GetNextVariableName (
                                    VariableStorage,
                                    &VarNameSize,
                                    VariableName,
                                    VariableGuid,
                                    &VarAttributes
                                    );
        if (!EFI_ERROR (Status)) {
          if (VariableExistsInHob (VariableName, VariableGuid)) {
            //
            // Don't return this variable if there is a HOB variable that overrides it
            // advance to the next variable
            //
            break;
          }
          if (((VarAttributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) && AtRuntime ()) {
            //
            // Don't return this variable if we are at runtime and the variable's attributes do not include
            // the EFI_VARIABLE_RUNTIME_ACCESS flag. If this is true, advance to the next variable
            //
            break;
          }
          goto Done;
        } else if (Status == EFI_NOT_FOUND) {
          //
          // If we reached the end of the variables in the current NV storage
          // get the first variable in the next NV storage
          //
          if ((Index + 1) < mVariableModuleGlobal->VariableGlobal.VariableStoresCount) {
            VarNameSize = CallerVariableNameBufferSize;
            VariableStorage = mVariableModuleGlobal->VariableGlobal.VariableStores[Index + 1];

            ZeroMem ((VOID *) VariableGuid, sizeof (VariableGuid));
            Status = VariableStorage->GetNextVariableName (
                                        VariableStorage,
                                        &VarNameSize,
                                        VariableName,
                                        VariableGuid,
                                        &VarAttributes
                                        );
            if (!EFI_ERROR (Status)) {
              if (VariableExistsInHob (VariableName, VariableGuid)) {
                //
                // Don't return this variable if there is a HOB variable that overrides it
                // advance to the next variable
                //
                break;
              }
              if (((VarAttributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) && AtRuntime ()) {
                //
                // Don't return this variable if we are at runtime and the variable's attributes do not include
                // the EFI_VARIABLE_RUNTIME_ACCESS flag. If this is true, advance to the next variable
                //
                break;
              }
            } else {
              DEBUG ((DEBUG_ERROR, "VariableStorageProtocol->GetNextVariableName status %r\n", Status));
              return Status;
            }
            DEBUG ((DEBUG_INFO, "Variable.c: Status returned from the variable storage protocol is %r\n", Status));
            goto Done;
          } else {
            // This is the last variable
            SearchComplete = TRUE;
            break;
          }
        } else {
          DEBUG ((DEBUG_ERROR, "VariableStorageProtocol->GetNextVariableName status %r\n", Status));
          return Status;
        }
      }
    }
  }

  return EFI_NOT_FOUND;

Done:
  *VariableNameSize = VarNameSize;

  if (CallerVariableNameBufferSize < VarNameSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  return EFI_SUCCESS;
}


/**
  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in, out] VariableNameSize           Size of the variable name.
  @param[in, out] VariableName               Pointer to variable name.
  @param[in, out] VendorGuid                 Variable Vendor Guid.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  UINTN                   VariableNameBufferSize;
  EFI_STATUS              Status;

  if (VariableNameSize == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AcquireLockOnlyAtBootTime(&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  VariableNameBufferSize = sizeof (mVariableNameBuffer);
  ZeroMem ((VOID *) &mVariableNameBuffer[0], VariableNameBufferSize);
  StrCpyS (&mVariableNameBuffer[0], VariableNameBufferSize, VariableName);

  Status = VariableServiceGetNextVariableInternal (&VariableNameBufferSize, &mVariableNameBuffer[0], VendorGuid);
  ASSERT (Status != EFI_BUFFER_TOO_SMALL);

  if (!EFI_ERROR (Status)) {
    if (VariableNameBufferSize > *VariableNameSize) {
      *VariableNameSize = VariableNameBufferSize;
      Status = EFI_BUFFER_TOO_SMALL;
    } else {
      StrCpyS (VariableName, *VariableNameSize, &mVariableNameBuffer[0]);
      *VariableNameSize = VariableNameBufferSize;
    }
  }

  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  return Status;
}


/**
  This code sets variable in storage blocks (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in] VariableName                     Name of Variable to be found.
  @param[in] VendorGuid                       Variable vendor GUID.
  @param[in] Attributes                       Attribute value of the variable found
  @param[in] DataSize                         Size of Data found. If size is less than the
                                              data, this value contains the required size.
  @param[in] Data                             Data pointer.

  @return    EFI_INVALID_PARAMETER            Invalid parameter.
  @return    EFI_SUCCESS                      Set successfully.
  @return    EFI_OUT_OF_RESOURCES             Resource not enough to set variable.
  @return    EFI_NOT_FOUND                    Not found.
  @return    EFI_WRITE_PROTECTED              Variable is read-only.

**/
EFI_STATUS
EFIAPI
VariableServiceSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )
{
  VARIABLE_POINTER_TRACK              Variable;
  EFI_STATUS                          Status;
  UINTN                               PayloadSize;

  //
  // Check input parameters.
  //
  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check for the reserverd bit in the variable attributes.
  //
  if ((Attributes & (~EFI_VARIABLE_ATTRIBUTES_MASK)) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Make sure that if the runtime bit is set, the boot service bit is also set.
  //
  if ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == EFI_VARIABLE_RUNTIME_ACCESS) {
    return EFI_INVALID_PARAMETER;
  } else if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
    if (!mVariableModuleGlobal->VariableGlobal.AuthSupport) {
      //
      // Authenticated variable writes are not supported.
      //
      return EFI_INVALID_PARAMETER;
    }
  } else if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != 0) {
    if (PcdGet32 (PcdHwErrStorageSize) == 0) {
      //
      // Harware error record variables are not supported on this platform.
      //
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS and EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
  // cannot both be set.
  //
  if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS)
     && ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) {
    if (DataSize < AUTHINFO_SIZE) {
      //
      // User attempted to write to a Authenticated Variable without AuthInfo.
      //
      return EFI_SECURITY_VIOLATION;
    }
    PayloadSize = DataSize - AUTHINFO_SIZE;
  } else if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) {
    //
    // Sanity check for EFI_VARIABLE_AUTHENTICATION_2 descriptor.
    //
    if (DataSize < OFFSET_OF_AUTHINFO2_CERT_DATA ||
      ((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->AuthInfo.Hdr.dwLength > DataSize - (OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo)) ||
      ((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->AuthInfo.Hdr.dwLength < OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData)) {
      return EFI_SECURITY_VIOLATION;
    }
    PayloadSize = DataSize - AUTHINFO2_SIZE (Data);
  } else {
    PayloadSize = DataSize;
  }

  if ((UINTN)(~0) - PayloadSize < StrSize(VariableName)){
    //
    // Prevent whole variable size overflow
    //
    return EFI_INVALID_PARAMETER;
  }

  //
  //  The size of the VariableName, including the Unicode Null in bytes plus
  //  the DataSize is limited to maximum size of PcdGet32 (PcdMaxHardwareErrorVariableSize)
  //  bytes for HwErrRec#### variable.
  //
  if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    if (StrSize (VariableName) + PayloadSize > PcdGet32 (PcdMaxHardwareErrorVariableSize) - GetVariableHeaderSize ()) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    //  The size of the VariableName, including the Unicode Null in bytes plus
    //  the DataSize is limited to maximum size of Max(Auth)VariableSize bytes.
    //
    if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
      if (StrSize (VariableName) + PayloadSize > mVariableModuleGlobal->MaxAuthVariableSize - GetVariableHeaderSize ()) {
        return EFI_INVALID_PARAMETER;
      }
    } else {
      if (StrSize (VariableName) + PayloadSize > mVariableModuleGlobal->MaxVariableSize - GetVariableHeaderSize ()) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }

  //
  // Special Handling for MOR Lock variable.
  //
  Status = SetVariableCheckHandlerMor (VariableName, VendorGuid, Attributes, PayloadSize, (VOID *) ((UINTN) Data + DataSize - PayloadSize));
  if (Status == EFI_ALREADY_STARTED) {
    //
    // EFI_ALREADY_STARTED means the SetVariable() action is handled inside of SetVariableCheckHandlerMor().
    // Variable driver can just return SUCCESS.
    //
    return EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = VarCheckLibSetVariableCheck (VariableName, VendorGuid, Attributes, PayloadSize, (VOID *) ((UINTN) Data + DataSize - PayloadSize), mRequestSource);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AcquireLockOnlyAtBootTime(&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  //
  // Consider reentrant in MCA/INIT/NMI. It needs be reupdated.
  //
  InterlockedIncrement (&mVariableModuleGlobal->VariableGlobal.ReentrantState);

  //
  // Check whether the input variable is already existed.
  //
  Status = FindVariable (VariableName, VendorGuid, &Variable, &mVariableModuleGlobal->VariableGlobal, TRUE);
  if (!EFI_ERROR (Status)) {
    if (((Variable.CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) && AtRuntime ()) {
      Status = EFI_WRITE_PROTECTED;
      goto Done;
    }
    if (Attributes != 0 && (Attributes & (~EFI_VARIABLE_APPEND_WRITE)) != Variable.CurrPtr->Attributes) {
      //
      // If a preexisting variable is rewritten with different attributes, SetVariable() shall not
      // modify the variable and shall return EFI_INVALID_PARAMETER. Two exceptions to this rule:
      // 1. No access attributes specified
      // 2. The only attribute differing is EFI_VARIABLE_APPEND_WRITE
      //
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((EFI_D_INFO, "  Variable Driver: Rewritten a preexisting variable(0x%08x) with different attributes(0x%08x) - %g:%s\n", Variable.CurrPtr->Attributes, Attributes, VendorGuid, VariableName));
      goto Done;
    }
  }

  Status = VarCheckLibSetVariableCheck (VariableName, VendorGuid, Attributes, PayloadSize, (VOID *) ((UINTN) Data + DataSize - PayloadSize), mRequestSource);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!FeaturePcdGet (PcdUefiVariableDefaultLangDeprecate)) {
    //
    // Hook the operation of setting PlatformLangCodes/PlatformLang and LangCodes/Lang.
    //
    Status = AutoUpdateLangVariable (VariableName, Data, DataSize);
    if (EFI_ERROR (Status)) {
      //
      // The auto update operation failed, directly return to avoid inconsistency between PlatformLang and Lang.
      //
      goto Done;
    }
  }

  if (mVariableModuleGlobal->VariableGlobal.AuthSupport) {
    Status = AuthVariableLibProcessVariable (VariableName, VendorGuid, Data, DataSize, Attributes);
  } else {
    Status = UpdateVariable (VariableName, VendorGuid, Data, DataSize, Attributes, 0, 0, &Variable, NULL);
  }

Done:
  InterlockedDecrement (&mVariableModuleGlobal->VariableGlobal.ReentrantState);
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  if (!AtRuntime ()) {
    if (!EFI_ERROR (Status)) {
      SecureBootHook (
        VariableName,
        VendorGuid
        );
    }
  }

  return Status;
}


/**
  This code returns information about the EFI variables.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in]  Attributes                     Attributes bitmask to specify the type of variables
                                             on which to return information.
  @param[out] MaximumVariableStorageSize     Pointer to the maximum size of the storage space available
                                             for the EFI variables associated with the attributes specified.
  @param[out] RemainingVariableStorageSize   Pointer to the remaining size of the storage space available
                                             for EFI variables associated with the attributes specified.
  @param[out] MaximumVariableSize            Pointer to the maximum size of an individual EFI variables
                                             associated with the attributes specified.

  @return     EFI_SUCCESS                    Query successfully.

**/
EFI_STATUS
EFIAPI
VariableServiceQueryVariableInfoInternal (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  )
{
  VARIABLE_HEADER             *Variable;
  VARIABLE_HEADER             *NextVariable;
  UINT64                      VariableSize;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  UINT64                      CommonVariableTotalSize;
  UINT64                      HwErrVariableTotalSize;
  VARIABLE_STORAGE_PROTOCOL   *VariableStorageProtocol;
  UINTN                       Index;
  UINT32                      VariableStoreSize;
  UINT32                      VspCommonVariablesTotalSize;
  UINT32                      VspHwErrVariablesTotalSize;
  EFI_STATUS                  Status;
  VARIABLE_POINTER_TRACK      VariablePtrTrack;

  CommonVariableTotalSize = 0;
  HwErrVariableTotalSize = 0;

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    //
    // Query is Volatile related.
    //
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->VariableGlobal.VolatileVariableBase);
  } else {
    //
    // Query is Non-Volatile related.
    //
    VariableStoreHeader = mNvVariableCache;
  }

  //
  // Now let's fill *MaximumVariableStorageSize *RemainingVariableStorageSize
  // with the storage size (excluding the storage header size).
  //
  *MaximumVariableStorageSize   = VariableStoreHeader->Size - sizeof (VARIABLE_STORE_HEADER);

  //
  // Harware error record variable needs larger size.
  //
  if ((Attributes & (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_HARDWARE_ERROR_RECORD)) == (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_HARDWARE_ERROR_RECORD)) {
    *MaximumVariableStorageSize = PcdGet32 (PcdHwErrStorageSize);
    *MaximumVariableSize = PcdGet32 (PcdMaxHardwareErrorVariableSize) - GetVariableHeaderSize ();
  } else {
    if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
      if (AtRuntime ()) {
        *MaximumVariableStorageSize = mVariableModuleGlobal->CommonRuntimeVariableSpace;
      } else {
        *MaximumVariableStorageSize = mVariableModuleGlobal->CommonVariableSpace;
      }
    }

    //
    // Let *MaximumVariableSize be Max(Auth)VariableSize with the exception of the variable header size.
    //
    if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
      *MaximumVariableSize = mVariableModuleGlobal->MaxAuthVariableSize - GetVariableHeaderSize ();
    } else {
      *MaximumVariableSize = mVariableModuleGlobal->MaxVariableSize - GetVariableHeaderSize ();
    }
  }

  //
  // Point to the starting address of the variables.
  //
  Variable = GetStartPointer (VariableStoreHeader);

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    //
    // For Volatile related, walk through the variable store.
    //
    while (IsValidVariableHeader (Variable, GetEndPointer (VariableStoreHeader))) {
      NextVariable = GetNextVariablePtr (Variable);
      VariableSize = (UINT64) (UINTN) NextVariable - (UINT64) (UINTN) Variable;

      if (AtRuntime ()) {
        //
        // We don't take the state of the variables in mind
        // when calculating RemainingVariableStorageSize,
        // since the space occupied by variables not marked with
        // VAR_ADDED is not allowed to be reclaimed in Runtime.
        //
        if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
          HwErrVariableTotalSize += VariableSize;
        } else {
          CommonVariableTotalSize += VariableSize;
        }
      } else {
        //
        // Only care about Variables with State VAR_ADDED, because
        // the space not marked as VAR_ADDED is reclaimable now.
        //
        if (Variable->State == VAR_ADDED) {
          if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
            HwErrVariableTotalSize += VariableSize;
          } else {
            CommonVariableTotalSize += VariableSize;
          }
        } else if (Variable->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          //
          // If it is a IN_DELETED_TRANSITION variable,
          // and there is not also a same ADDED one at the same time,
          // this IN_DELETED_TRANSITION variable is valid.
          //
          VariablePtrTrack.StartPtr = GetStartPointer (VariableStoreHeader);
          VariablePtrTrack.EndPtr   = GetEndPointer   (VariableStoreHeader);
          Status = FindVariableEx (
                    GetVariableNamePtr (Variable),
                    GetVendorGuidPtr (Variable),
                    FALSE,
                    &VariablePtrTrack
                    );
          if (!EFI_ERROR (Status) && VariablePtrTrack.CurrPtr->State != VAR_ADDED) {
            if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
              HwErrVariableTotalSize += VariableSize;
            } else {
              CommonVariableTotalSize += VariableSize;
            }
          }
        }
      }

      //
      // Go to the next one.
      //
      Variable = NextVariable;
    }
  } else {
    //
    // For Non Volatile related, call GetStorageUsage() on the VARIABLE_STORAGE_PROTOCOLs
    //
    for ( Index = 0;
          Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
          Index++) {
      VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
      Status = VariableStorageProtocol->GetStorageUsage (
                                          VariableStorageProtocol,
                                          AtRuntime (),
                                          &VariableStoreSize,
                                          &VspCommonVariablesTotalSize,
                                          &VspHwErrVariablesTotalSize
                                          );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      CommonVariableTotalSize  += VspCommonVariablesTotalSize;
      HwErrVariableTotalSize   += VspHwErrVariablesTotalSize;
    }
  }

  if ((Attributes  & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD){
    *RemainingVariableStorageSize = *MaximumVariableStorageSize - HwErrVariableTotalSize;
  } else {
    if (*MaximumVariableStorageSize < CommonVariableTotalSize) {
      *RemainingVariableStorageSize = 0;
    } else {
      *RemainingVariableStorageSize = *MaximumVariableStorageSize - CommonVariableTotalSize;
    }
  }

  if (*RemainingVariableStorageSize < GetVariableHeaderSize ()) {
    *MaximumVariableSize = 0;
  } else if ((*RemainingVariableStorageSize - GetVariableHeaderSize ()) < *MaximumVariableSize) {
    *MaximumVariableSize = *RemainingVariableStorageSize - GetVariableHeaderSize ();
  }

  return EFI_SUCCESS;
}


/**
  This code returns information about the EFI variables.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in]  Attributes                    Attributes bitmask to specify the type of variables
                                            on which to return information.
  @param[out] MaximumVariableStorageSize    Pointer to the maximum size of the storage space available
                                            for the EFI variables associated with the attributes specified.
  @param[out] RemainingVariableStorageSize  Pointer to the remaining size of the storage space available
                                            for EFI variables associated with the attributes specified.
  @param[out] MaximumVariableSize           Pointer to the maximum size of an individual EFI variables
                                            associated with the attributes specified.

  @return     EFI_INVALID_PARAMETER         An invalid combination of attribute bits was supplied.
  @return     EFI_SUCCESS                   Query successfully.
  @return     EFI_UNSUPPORTED               The attribute is not supported on this platform.

**/
EFI_STATUS
EFIAPI
VariableServiceQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  )
{
  EFI_STATUS             Status;

  if(MaximumVariableStorageSize == NULL || RemainingVariableStorageSize == NULL || MaximumVariableSize == NULL || Attributes == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_ATTRIBUTES_MASK) == 0) {
    //
    // Make sure the Attributes combination is supported by the platform.
    //
    return EFI_UNSUPPORTED;
  } else if ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == EFI_VARIABLE_RUNTIME_ACCESS) {
    //
    // Make sure if runtime bit is set, boot service bit is set also.
    //
    return EFI_INVALID_PARAMETER;
  } else if (AtRuntime () && ((Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0)) {
    //
    // Make sure RT Attribute is set if we are in Runtime phase.
    //
    return EFI_INVALID_PARAMETER;
  } else if ((Attributes & (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_HARDWARE_ERROR_RECORD)) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    //
    // Make sure Hw Attribute is set with NV.
    //
    return EFI_INVALID_PARAMETER;
  } else if ((Attributes & VARIABLE_ATTRIBUTE_AT_AW) != 0) {
    if (!mVariableModuleGlobal->VariableGlobal.AuthSupport) {
      //
      // Not support authenticated variable write.
      //
      return EFI_UNSUPPORTED;
    }
  } else if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != 0) {
    if (PcdGet32 (PcdHwErrStorageSize) == 0) {
      //
      // Not support harware error record variable variable.
      //
      return EFI_UNSUPPORTED;
    }
  }

  AcquireLockOnlyAtBootTime(&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  Status = VariableServiceQueryVariableInfoInternal (
             Attributes,
             MaximumVariableStorageSize,
             RemainingVariableStorageSize,
             MaximumVariableSize
             );

  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);
  return Status;
}


/**
  This function reclaims variable storage if free size is below the threshold.

  Caution: This function may be invoked at SMM mode.
  Care must be taken to make sure not security issue.

**/
VOID
ReclaimForOS(
  VOID
  )
{
  EFI_STATUS                     Status;
  UINTN                          RemainingCommonRuntimeVariableSpace;
  UINTN                          RemainingHwErrVariableSpace;
  UINTN                          CacheOffset;
  STATIC BOOLEAN                 Reclaimed;

  //
  // This function will be called only once at EndOfDxe or ReadyToBoot event.
  //
  if (Reclaimed) {
    return;
  }
  Reclaimed = TRUE;

  Status  = EFI_SUCCESS;

  if (mVariableModuleGlobal->CommonRuntimeVariableSpace < mVariableModuleGlobal->CommonVariableTotalSize) {
    RemainingCommonRuntimeVariableSpace = 0;
  } else {
    RemainingCommonRuntimeVariableSpace = mVariableModuleGlobal->CommonRuntimeVariableSpace - mVariableModuleGlobal->CommonVariableTotalSize;
  }

  RemainingHwErrVariableSpace = PcdGet32 (PcdHwErrStorageSize) - mVariableModuleGlobal->HwErrVariableTotalSize;

  //
  // Check if the free area is below a threshold.
  //
  if (((RemainingCommonRuntimeVariableSpace < mVariableModuleGlobal->MaxVariableSize) ||
       (RemainingCommonRuntimeVariableSpace < mVariableModuleGlobal->MaxAuthVariableSize)) ||
      ((PcdGet32 (PcdHwErrStorageSize) != 0) &&
       (RemainingHwErrVariableSpace < PcdGet32 (PcdMaxHardwareErrorVariableSize)))) {
    Status = Reclaim (
               (EFI_PHYSICAL_ADDRESS) (UINTN) mNvVariableCache,
               &CacheOffset,
               FALSE,
               NULL,
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
  }
}


/**
  Get non-volatile maximum variable size.

  @return Non-volatile maximum variable size.

**/
UINTN
GetNonVolatileMaxVariableSize (
  VOID
  )
{
  if (PcdGet32 (PcdHwErrStorageSize) != 0) {
    return MAX (MAX (PcdGet32 (PcdMaxVariableSize), PcdGet32 (PcdMaxAuthVariableSize)),
                PcdGet32 (PcdMaxHardwareErrorVariableSize));
  } else {
    return MAX (PcdGet32 (PcdMaxVariableSize), PcdGet32 (PcdMaxAuthVariableSize));
  }
}


/**
  A callback invoked by each VARIABLE_STORAGE_PROTOCOL to indicate to the core variable driver that
  SetVariable() is ready for use on that VARIABLE_STORAGE_PROTOCOL

  @retval     EFI_SUCCESS                    Change to WriteServiceIsReady() status was processed successfully.

**/
EFI_STATUS
EFIAPI
VariableStorageWriteServiceReadyCallback (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  VARIABLE_STORAGE_PROTOCOL     *VariableStorageProtocol;
  BOOLEAN                       WriteServiceReady;

  Status            = EFI_SUCCESS;
  WriteServiceReady = TRUE;
  for ( Index = 0;
        Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
        Index++) {
    VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
    if (!VariableStorageProtocol->WriteServiceIsReady (VariableStorageProtocol)) {
      WriteServiceReady = FALSE;
      break;
    }
  }
  if (WriteServiceReady && !mVariableModuleGlobal->WriteServiceReady) {
    mVariableModuleGlobal->WriteServiceReady = TRUE;
    Status = VariableWriteServiceInitialize ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "  Variable Driver: Variable write service initialization failed. Status = %r\n", Status));
    } else {
      //
      // Install the Variable Write Architectural protocol
      //
      InstallVariableWriteReady ();
    }
  }
  return Status;
}


/**
  Init non-volatile variable store.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.
  @retval EFI_VOLUME_CORRUPTED  Variable Store or Firmware Volume for Variable Store is corrupted.

**/
EFI_STATUS
InitNonVolatileVariableStore (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  VARIABLE_STORAGE_PROTOCOL     *VariableStorageProtocol;
  UINT32                        NvStoreTotalSize;
  UINT32                        VariableStoreSize;
  UINT32                        CommonVariablesTotalSize;
  UINT32                        HwErrVariablesTotalSize;
  UINT32                        HwErrStorageSize;
  UINT32                        MaxUserNvVariableSpaceSize;
  UINT32                        BoottimeReservedNvVariableSpaceSize;
  BOOLEAN                       AuthSupported;

  //
  // Get the total NV storage size from all VARIABLE_STORAGE_PROTOCOLs, and the amount of space
  // used
  //
  NvStoreTotalSize                                  = 0;
  mVariableModuleGlobal->CommonVariableTotalSize    = 0;
  mVariableModuleGlobal->HwErrVariableTotalSize     = 0;
  mVariableModuleGlobal->VariableGlobal.AuthFormat  = TRUE;
  mVariableModuleGlobal->WriteServiceReady          = FALSE;

  if (!mForceVolatileVariable) {
    for ( Index = 0;
          Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
          Index++) {
      VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
      Status = VariableStorageProtocol->GetStorageUsage (
                                          VariableStorageProtocol,
                                          AtRuntime (),
                                          &VariableStoreSize,
                                          &CommonVariablesTotalSize,
                                          &HwErrVariablesTotalSize
                                          );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      DEBUG ((EFI_D_INFO, "+-+-> Variable Driver: Data from StorageUsage VariableStorageProtocol[%d]:\n  VariableStoreSize = %d bytes.\n  CommonVariablesTotalSize = %d bytes.  HwErrVariablesTotalSize = %d bytes.\n",
              Index, VariableStoreSize, CommonVariablesTotalSize, HwErrVariablesTotalSize));

      NvStoreTotalSize                                += VariableStoreSize;
      mVariableModuleGlobal->CommonVariableTotalSize  += CommonVariablesTotalSize;
      mVariableModuleGlobal->HwErrVariableTotalSize   += HwErrVariablesTotalSize;

      //
      // Determine authenticated variables support. If all of the
      // VARIABLE_STORAGE_PROTOCOLs support authenticated variables, enable it. If
      // any of the VARIABLE_STORAGE_PROTOCOLs do not support it disable it globally.
      //
      if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
        AuthSupported = FALSE;
        Status        = VariableStorageProtocol->GetAuthenticatedSupport (
                                                  VariableStorageProtocol,
                                                  &AuthSupported
                                                  );
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR (Status)) {
          return Status;
        }
        if (!AuthSupported) {
          mVariableModuleGlobal->VariableGlobal.AuthFormat = FALSE;
        }
      }
    }
  } else {
    NvStoreTotalSize                                = DEFAULT_NV_STORE_SIZE;
    mVariableModuleGlobal->CommonVariableTotalSize  = 0;
    mVariableModuleGlobal->HwErrVariableTotalSize   = 0;
  }

  DEBUG ((EFI_D_INFO, "+-+-> Variable Driver: Total data from StorageUsage:\n  NvStoreTotalSize = %d bytes.\n  CommonVariablesTotalSize = %d bytes.  HwErrVariablesTotalSize = %d bytes.\n",
            NvStoreTotalSize, mVariableModuleGlobal->CommonVariableTotalSize, mVariableModuleGlobal->HwErrVariableTotalSize));

  //
  // Allocate NV Storage Cache and initialize it to all 1's (like an erased FV)
  //
  // The NV Storage Cache allows the variable services to cache any NV variable
  // returned from the VARIABLE_STORAGE_PROTOCOLs so that subsequent reads will
  // be returned from memory and not require additional NV access
  //
  NvStoreTotalSize = ALIGN_VALUE (NvStoreTotalSize, sizeof (UINT32));
  mNvVariableCache = (VARIABLE_STORE_HEADER *) AllocateRuntimePool (NvStoreTotalSize);
  if (mNvVariableCache == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem32 ((VOID *) mNvVariableCache, NvStoreTotalSize, (UINT32) 0xFFFFFFFF);
  //
  // Initialize the VARIABLE_STORE_HEADER for the NV Storage Cache
  //
  ZeroMem ((VOID *) mNvVariableCache, sizeof (VARIABLE_STORE_HEADER));
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    CopyMem (&mNvVariableCache->Signature, &gEfiAuthenticatedVariableGuid, sizeof (EFI_GUID));
  } else {
    CopyMem (&mNvVariableCache->Signature, &gEfiVariableGuid, sizeof (EFI_GUID));
  }
  mNvVariableCache->Size    = NvStoreTotalSize;
  mNvVariableCache->Format  = VARIABLE_STORE_FORMATTED;
  mNvVariableCache->State   = VARIABLE_STORE_HEALTHY;
  //
  // Initialize mVariableModuleGlobal
  //
  HwErrStorageSize = PcdGet32 (PcdHwErrStorageSize);
  MaxUserNvVariableSpaceSize = PcdGet32 (PcdMaxUserNvVariableSpaceSize);
  BoottimeReservedNvVariableSpaceSize = PcdGet32 (PcdBoottimeReservedNvVariableSpaceSize);
  //
  // Note that in EdkII variable driver implementation, Hardware Error Record type variable
  // is stored with common variable in the same NV region. So the platform integrator should
  // ensure that the value of PcdHwErrStorageSize is less than the value of
  // (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)).
  //
  ASSERT (HwErrStorageSize < (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)));
  //
  // Ensure that the value of PcdMaxUserNvVariableSpaceSize is less than the value of
  // (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)) - PcdGet32 (PcdHwErrStorageSize).
  //
  ASSERT (MaxUserNvVariableSpaceSize < (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER) - HwErrStorageSize));
  //
  // Ensure that the value of PcdBoottimeReservedNvVariableSpaceSize is less than the value of
  // (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)) - PcdGet32 (PcdHwErrStorageSize).
  //
  ASSERT (BoottimeReservedNvVariableSpaceSize < (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER) - HwErrStorageSize));

  mVariableModuleGlobal->CommonVariableSpace = ((UINTN) NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER) - HwErrStorageSize);
  mVariableModuleGlobal->CommonMaxUserVariableSpace = ((MaxUserNvVariableSpaceSize != 0) ? MaxUserNvVariableSpaceSize : mVariableModuleGlobal->CommonVariableSpace);
  mVariableModuleGlobal->CommonRuntimeVariableSpace = mVariableModuleGlobal->CommonVariableSpace - BoottimeReservedNvVariableSpaceSize;

  DEBUG ((EFI_D_INFO, "  Variable Driver: Variable driver common space: 0x%x 0x%x 0x%x\n", mVariableModuleGlobal->CommonVariableSpace, mVariableModuleGlobal->CommonMaxUserVariableSpace, mVariableModuleGlobal->CommonRuntimeVariableSpace));

  //
  // The max NV variable size should be < (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)).
  //
  ASSERT (GetNonVolatileMaxVariableSize () < (NvStoreTotalSize - sizeof (VARIABLE_STORE_HEADER)));

  mVariableModuleGlobal->MaxVariableSize = PcdGet32 (PcdMaxVariableSize);
  mVariableModuleGlobal->MaxAuthVariableSize = ((PcdGet32 (PcdMaxAuthVariableSize) != 0) ? PcdGet32 (PcdMaxAuthVariableSize) : mVariableModuleGlobal->MaxVariableSize);

  return EFI_SUCCESS;
}


/**
  Flush the HOB variable to flash.

  @param[in] VariableName       Name of variable has been updated or deleted.
  @param[in] VendorGuid         Guid of variable has been updated or deleted.

**/
VOID
FlushHobVariableToFlash (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid
  )
{
  EFI_STATUS                    Status;
  VARIABLE_STORE_HEADER         *VariableStoreHeader;
  VARIABLE_HEADER               *Variable;
  VOID                          *VariableData;
  VARIABLE_POINTER_TRACK        VariablePtrTrack;
  BOOLEAN                       ErrorFlag;

  ErrorFlag = FALSE;

  //
  // Flush the HOB variable to flash.
  //
  if (mVariableModuleGlobal->VariableGlobal.HobVariableBase != 0) {
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableGlobal.HobVariableBase;
    //
    // Set HobVariableBase to 0, it can avoid SetVariable to call back.
    //
    mVariableModuleGlobal->VariableGlobal.HobVariableBase = 0;
    for ( Variable = GetStartPointer (VariableStoreHeader)
        ; IsValidVariableHeader (Variable, GetEndPointer (VariableStoreHeader))
        ; Variable = GetNextVariablePtr (Variable)
        ) {
      if (Variable->State != VAR_ADDED) {
        //
        // The HOB variable has been set to DELETED state in local.
        //
        continue;
      }
      ASSERT ((Variable->Attributes & EFI_VARIABLE_NON_VOLATILE) != 0);
      if (VendorGuid == NULL || VariableName == NULL ||
          !CompareGuid (VendorGuid, GetVendorGuidPtr (Variable)) ||
          StrCmp (VariableName, GetVariableNamePtr (Variable)) != 0) {
        VariableData = GetVariableDataPtr (Variable);
        FindVariable (GetVariableNamePtr (Variable), GetVendorGuidPtr (Variable), &VariablePtrTrack, &mVariableModuleGlobal->VariableGlobal, FALSE);
        Status = UpdateVariable (
                   GetVariableNamePtr (Variable),
                   GetVendorGuidPtr (Variable),
                   VariableData,
                   DataSizeOfVariable (Variable),
                   Variable->Attributes,
                   0,
                   0,
                   &VariablePtrTrack,
                   NULL
                 );
        DEBUG ((EFI_D_INFO, "  Variable Driver: Flushed the HOB variable to flash: %g %s %r\n", GetVendorGuidPtr (Variable), GetVariableNamePtr (Variable), Status));
      } else {
        //
        // The updated or deleted variable is matched with this HOB variable.
        // Don't break here because we will try to set other HOB variables
        // since this variable could be set successfully.
        //
        Status = EFI_SUCCESS;
      }
      if (!EFI_ERROR (Status)) {
        //
        // If set variable successful, or the updated or deleted variable is matched with the HOB variable,
        // set the HOB variable to DELETED state in local.
        //
        DEBUG ((EFI_D_INFO, "  Variable Driver: Set the HOB variable to DELETED state in local: %g %s\n", GetVendorGuidPtr (Variable), GetVariableNamePtr (Variable)));
        Variable->State &= VAR_DELETED;
      } else {
        ErrorFlag = TRUE;
      }
    }
    if (ErrorFlag) {
      //
      // We still have HOB variable(s) not flushed in flash.
      //
      mVariableModuleGlobal->VariableGlobal.HobVariableBase = (EFI_PHYSICAL_ADDRESS) (UINTN) VariableStoreHeader;
    } else {
      //
      // All HOB variables have been flushed in flash.
      //
      DEBUG ((EFI_D_INFO, "  Variable Driver: All HOB variables have been flushed in flash.\n"));
      if (!AtRuntime ()) {
        FreePool ((VOID *) VariableStoreHeader);
      }
    }
  }

}


/**
  Initializes variable write service after FTW was ready.

  @retval EFI_SUCCESS          Function successfully executed.
  @retval Others               Fail to initialize the variable service.

**/
EFI_STATUS
VariableWriteServiceInitialize (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  VARIABLE_ENTRY_PROPERTY         *VariableEntry;

  AcquireLockOnlyAtBootTime(&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);

  FlushHobVariableToFlash (NULL, NULL);

  Status = EFI_SUCCESS;
  ZeroMem (&mAuthContextOut, sizeof (mAuthContextOut));
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    //
    // Authenticated variable initialize.
    //
    mAuthContextIn.StructSize = sizeof (AUTH_VAR_LIB_CONTEXT_IN);
    mAuthContextIn.MaxAuthVariableSize = mVariableModuleGlobal->MaxAuthVariableSize - GetVariableHeaderSize ();
    Status = AuthVariableLibInitialize (&mAuthContextIn, &mAuthContextOut);
    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "  Variable Driver: Variable driver will work with auth variable support!\n"));
      mVariableModuleGlobal->VariableGlobal.AuthSupport = TRUE;
      if (mAuthContextOut.AuthVarEntry != NULL) {
        for (Index = 0; Index < mAuthContextOut.AuthVarEntryCount; Index++) {
          VariableEntry = &mAuthContextOut.AuthVarEntry[Index];
          Status = VarCheckLibVariablePropertySet (
                     VariableEntry->Name,
                     VariableEntry->Guid,
                     &VariableEntry->VariableProperty
                     );
          ASSERT_EFI_ERROR (Status);
        }
      }
    } else if (Status == EFI_UNSUPPORTED) {
      DEBUG ((EFI_D_INFO, "  Variable Driver: NOTICE - AuthVariableLibInitialize() returns %r!\n", Status));
      DEBUG ((EFI_D_INFO, "  Variable Driver: Will continue to work without auth variable support!\n"));
      mVariableModuleGlobal->VariableGlobal.AuthSupport = FALSE;
      Status = EFI_SUCCESS;
    }
  }

  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < sizeof (mVariableEntryProperty) / sizeof (mVariableEntryProperty[0]); Index++) {
      VariableEntry = &mVariableEntryProperty[Index];
      Status = VarCheckLibVariablePropertySet (VariableEntry->Name, VariableEntry->Guid, &VariableEntry->VariableProperty);
      ASSERT_EFI_ERROR (Status);
    }
  }

  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock);
  //
  // Initialize MOR Lock variable.
  //
  MorLockInit();

  return Status;
}


/**
  Initializes variable store area for non-volatile and volatile variable.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.

**/
EFI_STATUS
VariableCommonInitialize (
  VOID
  )
{
  EFI_STATUS                      Status;
  VARIABLE_STORE_HEADER           *VolatileVariableStore;
  VARIABLE_STORE_HEADER           *VariableStoreHeader;
  UINT64                          VariableStoreLength;
  UINTN                           ScratchSize;
  EFI_HOB_GUID_TYPE               *GuidHob;
  EFI_GUID                        *VariableGuid;
  UINTN                           Index;
  VARIABLE_STORAGE_PROTOCOL       *VariableStorageProtocol;

  InitializeLock (&mVariableModuleGlobal->VariableGlobal.VariableServicesLock, TPL_NOTIFY);

  //
  // Allocate memory for volatile variable store, note that there is a scratch space to store scratch data.
  //
  ScratchSize = GetNonVolatileMaxVariableSize ();
  mVariableModuleGlobal->ScratchBufferSize = ScratchSize;
  VolatileVariableStore = AllocateRuntimePool (PcdGet32 (PcdVariableStoreSize) + ScratchSize);
  if (VolatileVariableStore == NULL) {
    FreePool (mVariableModuleGlobal);
    return EFI_OUT_OF_RESOURCES;
  }
  mVariableDataBuffer = AllocateRuntimePool (ScratchSize);
  if (mVariableDataBuffer == NULL) {
    FreePool (mVariableModuleGlobal);
    FreePool (VolatileVariableStore);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Init non-volatile variable store.
  //
  Status = InitNonVolatileVariableStore ();
  if (EFI_ERROR (Status)) {
    FreePool (mVariableModuleGlobal);
    FreePool (mVariableDataBuffer);
    FreePool (VolatileVariableStore);
    return Status;
  }

  //
  // mVariableModuleGlobal->VariableGlobal.AuthFormat
  // has been initialized in InitNonVolatileVariableStore().
  //
  if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
    DEBUG ((EFI_D_INFO, "  Variable Driver: Will work with auth variable format!\n"));
    //
    // Set AuthSupport to FALSE first, VariableWriteServiceInitialize() will initialize it.
    //
    mVariableModuleGlobal->VariableGlobal.AuthSupport = FALSE;
    VariableGuid = &gEfiAuthenticatedVariableGuid;
  } else {
    DEBUG ((EFI_D_INFO, "  Variable Driver: Will work without auth variable support!\n"));
    mVariableModuleGlobal->VariableGlobal.AuthSupport = FALSE;
    VariableGuid = &gEfiVariableGuid;
  }

  //
  // Get HOB variable store.
  //
  GuidHob = GetFirstGuidHob (VariableGuid);
  if (GuidHob != NULL) {
    VariableStoreHeader = GET_GUID_HOB_DATA (GuidHob);
    VariableStoreLength = (UINT64) (GuidHob->Header.HobLength - sizeof (EFI_HOB_GUID_TYPE));
    if (GetVariableStoreStatus (VariableStoreHeader) == EfiValid) {
      mVariableModuleGlobal->VariableGlobal.HobVariableBase = (EFI_PHYSICAL_ADDRESS) (UINTN) AllocateRuntimeCopyPool ((UINTN) VariableStoreLength, (VOID *) VariableStoreHeader);
      if (mVariableModuleGlobal->VariableGlobal.HobVariableBase == 0) {
        FreePool (mNvVariableCache);
        FreePool (mVariableModuleGlobal);
        FreePool (mVariableDataBuffer);
        FreePool (VolatileVariableStore);
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      DEBUG ((EFI_D_ERROR, "HOB Variable Store header is corrupted!\n"));
    }
  }

  SetMem (VolatileVariableStore, PcdGet32 (PcdVariableStoreSize) + ScratchSize, 0xff);

  //
  // Initialize Variable Specific Data.
  //
  mVariableModuleGlobal->VariableGlobal.VolatileVariableBase = (EFI_PHYSICAL_ADDRESS) (UINTN) VolatileVariableStore;
  mVariableModuleGlobal->VolatileLastVariableOffset = (UINTN) GetStartPointer (VolatileVariableStore) - (UINTN) VolatileVariableStore;

  CopyGuid (&VolatileVariableStore->Signature, VariableGuid);
  VolatileVariableStore->Size        = PcdGet32 (PcdVariableStoreSize);
  VolatileVariableStore->Format      = VARIABLE_STORE_FORMATTED;
  VolatileVariableStore->State       = VARIABLE_STORE_HEALTHY;
  VolatileVariableStore->Reserved    = 0;
  VolatileVariableStore->Reserved1   = 0;

  //
  // Setup the callback to determine when to enable variable writes
  //
  if (!mForceVolatileVariable) {
    for ( Index = 0;
          Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
          Index++) {
      VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
      VariableStorageProtocol->RegisterWriteServiceReadyCallback (
                                VariableStorageProtocol,
                                VariableStorageWriteServiceReadyCallback
                                );
    }
  } else {
    mVariableModuleGlobal->WriteServiceReady = TRUE;
    Status = VariableWriteServiceInitialize ();

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "  Variable Driver: Variable write service initialization failed. Status = %r\n", Status));
    } else {
      //
      // Install the Variable Write Architectural protocol
      //
      InstallVariableWriteReady ();
    }
  }

  return EFI_SUCCESS;
}

