/** @file
  Implement ReadOnly Variable Services required by PEIM and install
  PEI ReadOnly Varaiable2 PPI. These services operates the non volatile storage space.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Variable.h"

//
// Module globals
//
EFI_PEI_READ_ONLY_VARIABLE2_PPI mVariablePpi = {
  PeiGetVariable,
  PeiGetNextVariableName
};

EFI_PEI_PPI_DESCRIPTOR     mPpiListVariable = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  &mVariablePpi
};


/**
  Provide the functionality of the variable services.

  @param[in]  FileHandle    Handle of the file being invoked.
                            Type EFI_PEI_FILE_HANDLE is defined in FfsFindNextFile().
  @param[in]  PeiServices   General purpose services available to every PEIM.

  @retval     EFI_SUCCESS   If the interface could be successfully installed
  @retval     Others        Returned from PeiServicesInstallPpi()

**/
EFI_STATUS
EFIAPI
PeimInitializeVariableServices (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                                    Status;
  UINTN                                         Instance;
  READ_ONLY_VARIABLE_PRE_MEMORY_DESCRIPTOR_PPI  *PreMemoryDescriptorPpi;
  EFI_PEI_PPI_DESCRIPTOR                        *PreMemoryDescriptorPpiDesc;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI               *OldReadOnlyVariable2Ppi;
  EFI_PEI_PPI_DESCRIPTOR                        *OldDescriptor;
  BOOLEAN                                       InPermanentMemory;

  InPermanentMemory = FALSE;
  Status = PeiServicesRegisterForShadow (FileHandle);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_ALREADY_STARTED) {
      InPermanentMemory = TRUE;
    } else {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  //
  // The PI spec does not require that there only be one instance of
  // EFI_PEI_READ_ONLY_VARIABLE2_PPI.  Therefore one must assume that a
  // scenario exists where there is a platform specific instance of
  // EFI_PEI_READ_ONLY_VARIABLE2_PPI.  Therefore, this driver must keep track
  // of which instance of EFI_PEI_READ_ONLY_VARIABLE2_PPI it installs so that
  // the correct instance is reinstalled after PEI is shadowed to permanent
  // memory.
  //
  if (!InPermanentMemory) {
    //
    // If system is still in Pre-Memory, store the temporary address to the
    // PPI descriptor in a separate unique PPI.  Then when permanent memory is
    // installed, the old PPI instance can be located and reinstalled with the
    // new address
    //
    PreMemoryDescriptorPpi = (READ_ONLY_VARIABLE_PRE_MEMORY_DESCRIPTOR_PPI *)
                              AllocateZeroPool (
                                sizeof (
                                  READ_ONLY_VARIABLE_PRE_MEMORY_DESCRIPTOR_PPI
                                  )
                                );
    PreMemoryDescriptorPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)
                                  AllocateZeroPool (
                                    sizeof (EFI_PEI_PPI_DESCRIPTOR)
                                    );
    if (PreMemoryDescriptorPpiDesc == NULL || PreMemoryDescriptorPpi == NULL) {
      ASSERT (PreMemoryDescriptorPpiDesc != NULL);
      ASSERT (PreMemoryDescriptorPpi != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    PreMemoryDescriptorPpi->PreMemoryDescriptor = &mPpiListVariable;
    PreMemoryDescriptorPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI |
                                        EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    PreMemoryDescriptorPpiDesc->Guid  = &gReadOnlyVariablePreMemoryDescriptorPpiGuid;
    PreMemoryDescriptorPpiDesc->Ppi   = PreMemoryDescriptorPpi;
    Status = PeiServicesInstallPpi (PreMemoryDescriptorPpiDesc);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    //
    // Now that the descriptor address is saved,
    // install EFI_PEI_READ_ONLY_VARIABLE2_PPI
    //
    Status = PeiServicesInstallPpi (&mPpiListVariable);
  } else {
    Status = PeiServicesLocatePpi (
               &gReadOnlyVariablePreMemoryDescriptorPpiGuid,
               0,
               NULL,
               (VOID **) &PreMemoryDescriptorPpi
               );
    if (!EFI_ERROR (Status)) {
      //
      // The previous descriptor pointer has been found, locate the old PPI
      // and reinstall it.
      //
      for (Instance = 0; Status != EFI_NOT_FOUND; Instance++) {
        Status = PeiServicesLocatePpi (
                   &gEfiPeiReadOnlyVariable2PpiGuid,
                   Instance,
                   &OldDescriptor,
                   (VOID **) &OldReadOnlyVariable2Ppi
                   );
        if (!EFI_ERROR (Status)) {
          if (OldDescriptor == PreMemoryDescriptorPpi->PreMemoryDescriptor) {
            Status = PeiServicesReInstallPpi (OldDescriptor, &mPpiListVariable);
            ASSERT_EFI_ERROR (Status);
            return Status;
          }
        } else if (Status != EFI_NOT_FOUND) {
          ASSERT_EFI_ERROR (Status);
          return Status;
        }
      }
      //
      // This implementation of EFI_PEI_READ_ONLY_VARIABLE2_PPI
      // was never installed. Install it for the first time
      //
      Status = PeiServicesInstallPpi (&mPpiListVariable);
    } else {
      //
      // This implementation of EFI_PEI_READ_ONLY_VARIABLE2_PPI
      // was never installed. Install it for the first time
      //
      Status = PeiServicesInstallPpi (&mPpiListVariable);
    }
  }

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Gets the pointer to the first variable header in given variable store area.

  @param[in] VarStoreHeader   Pointer to the Variable Store Header.

  @return    Pointer to the first variable header

**/
VARIABLE_HEADER *
GetStartPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  //
  // The end of variable store
  //
  return (VARIABLE_HEADER *) HEADER_ALIGN (VarStoreHeader + 1);
}


/**
  This code gets the pointer to the last variable memory pointer byte.

  @param[in]  VarStoreHeader   Pointer to the Variable Store Header.

  @return   VARIABLE_HEADER* pointer to last unavailable Variable Header.

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
  This code checks if variable header is valid or not.

  @param[in]  Variable  Pointer to the Variable Header.

  @retval     TRUE      Variable header is valid.
  @retval     FALSE     Variable header is not valid.

**/
BOOLEAN
IsValidVariableHeader (
  IN  VARIABLE_HEADER   *Variable
  )
{
  if (Variable == NULL || Variable->StartId != VARIABLE_DATA ) {
    return FALSE;
  }

  return TRUE;
}


/**
  This code gets the size of variable header.

  @param[in] AuthFlag    Authenticated variable flag.

  @return    Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  IN  BOOLEAN       AuthFlag
  )
{
  UINTN Value;

  if (AuthFlag) {
    Value = sizeof (AUTHENTICATED_VARIABLE_HEADER);
  } else {
    Value = sizeof (VARIABLE_HEADER);
  }

  return Value;
}


/**
  This code gets the size of name of variable.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     Size of variable in bytes in type UINTN.

**/
UINTN
NameSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable,
  IN  BOOLEAN           AuthFlag
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
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
  This code gets the size of data of variable.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     Size of variable in bytes in type UINTN.

**/
UINTN
DataSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable,
  IN  BOOLEAN           AuthFlag
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
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
  This code gets the pointer to the variable name.

  @param[in]   Variable    Pointer to the Variable Header.
  @param[in]   AuthFlag    Authenticated variable flag.

  @return      A CHAR16* pointer to Variable Name.

**/
CHAR16 *
GetVariableNamePtr (
  IN VARIABLE_HEADER    *Variable,
  IN BOOLEAN            AuthFlag
  )
{
  return (CHAR16 *) ((UINTN) Variable + GetVariableHeaderSize (AuthFlag));
}


/**
  This code gets the pointer to the variable guid.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     A EFI_GUID* pointer to Vendor Guid.

**/
EFI_GUID *
GetVendorGuidPtr (
  IN VARIABLE_HEADER    *Variable,
  IN BOOLEAN            AuthFlag
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (AuthFlag) {
    return &AuthVariable->VendorGuid;
  } else {
    return &Variable->VendorGuid;
  }
}


/**
  This code gets the pointer to the variable data.

  @param[in]   Variable         Pointer to the Variable Header.
  @param[in]   VariableHeader   Pointer to the Variable Header that has consecutive content.
  @param[in]   AuthFlag         Authenticated variable flag.

  @return      A UINT8* pointer to Variable Data.

**/
UINT8 *
GetVariableDataPtr (
  IN  VARIABLE_HEADER   *Variable,
  IN  VARIABLE_HEADER   *VariableHeader,
  IN  BOOLEAN           AuthFlag
  )
{
  UINTN Value;

  //
  // Be careful about pad size for alignment
  //
  Value =  (UINTN) GetVariableNamePtr (Variable, AuthFlag);
  Value += NameSizeOfVariable (VariableHeader, AuthFlag);
  Value += GET_PAD_SIZE (NameSizeOfVariable (VariableHeader, AuthFlag));

  return (UINT8 *) Value;
}


/**
  This code gets the pointer to the next variable header.

  @param[in]  StoreInfo         Pointer to variable store info structure.
  @param[in]  Variable          Pointer to the Variable Header.
  @param[in]  VariableHeader    Pointer to the Variable Header that has consecutive content.

  @return     A VARIABLE_HEADER* pointer to next variable header.

**/
VARIABLE_HEADER *
GetNextVariablePtr (
  IN  VARIABLE_STORE_INFO   *StoreInfo,
  IN  VARIABLE_HEADER       *Variable,
  IN  VARIABLE_HEADER       *VariableHeader
  )
{
  UINTN                 Value;

  Value =  (UINTN) GetVariableDataPtr (Variable, VariableHeader, StoreInfo->AuthFlag);
  Value += DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag);
  Value += GET_PAD_SIZE (DataSizeOfVariable (VariableHeader, StoreInfo->AuthFlag));
  //
  // Be careful about pad size for alignment
  //
  Value = HEADER_ALIGN (Value);

  return (VARIABLE_HEADER *) Value;
}


/**
  Get variable store status.

  @param[in]  VarStoreHeader  Pointer to the Variable Store Header.

  @retval     EfiRaw          Variable store is raw
  @retval     EfiValid        Variable store is valid
  @retval     EfiInvalid      Variable store is invalid

**/
VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN VARIABLE_STORE_HEADER *VarStoreHeader
  )
{
  if ((CompareGuid (&VarStoreHeader->Signature, &gPeiVariableCacheHobGuid) ||
       CompareGuid (&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
       CompareGuid (&VarStoreHeader->Signature, &gEfiVariableGuid)) &&
      VarStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
      VarStoreHeader->State == VARIABLE_STORE_HEALTHY
      ) {

    return EfiValid;
  }

  if (((UINT32 *) (&VarStoreHeader->Signature))[0] == 0xffffffff &&
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
  Compare two variable names, one of them may be inconsecutive.

  @param[in] StoreInfo      Pointer to variable store info structure.
  @param[in] Name1          Pointer to one variable name.
  @param[in] Name2          Pointer to another variable name.
  @param[in] NameSize       Variable name size.

  @retval    TRUE           Name1 and Name2 are identical.
  @retval    FALSE          Name1 and Name2 are not identical.

**/
BOOLEAN
CompareVariableName (
  IN VARIABLE_STORE_INFO    *StoreInfo,
  IN CONST CHAR16           *Name1,
  IN CONST CHAR16           *Name2,
  IN UINTN                  NameSize
  )
{

  //
  // Both Name1 and Name2 are consecutive.
  //
  if (CompareMem ((UINT8 *) Name1, (UINT8 *) Name2, NameSize) == 0) {
    return TRUE;
  }
  return FALSE;
}


/**
  This function compares a variable with variable entries in database.

  @param[in]  StoreInfo       Pointer to variable store info structure.
  @param[in]  Variable        Pointer to the variable in our database
  @param[in]  VariableHeader  Pointer to the Variable Header that has consecutive content.
  @param[in]  VariableName    Name of the variable to compare to 'Variable'
  @param[in]  VendorGuid      GUID of the variable to compare to 'Variable'
  @param[out] PtrTrack        Variable Track Pointer structure that contains Variable Information.

  @retval     EFI_SUCCESS     Found match variable
  @retval     EFI_NOT_FOUND   Variable not found

**/
EFI_STATUS
CompareWithValidVariable (
  IN  VARIABLE_STORE_INFO           *StoreInfo,
  IN  VARIABLE_HEADER               *Variable,
  IN  VARIABLE_HEADER               *VariableHeader,
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT VARIABLE_POINTER_TRACK        *PtrTrack
  )
{
  VOID      *Point;
  EFI_GUID  *TempVendorGuid;

  TempVendorGuid = GetVendorGuidPtr (VariableHeader, StoreInfo->AuthFlag);

  if (VariableName[0] == 0) {
    PtrTrack->CurrPtr = Variable;
    return EFI_SUCCESS;
  } else {
    //
    // Don't use CompareGuid function here for performance reasons.
    // Instead we compare the GUID a UINT32 at a time and branch
    // on the first failed comparison.
    //
    if ((((INT32 *) VendorGuid)[0] == ((INT32 *) TempVendorGuid)[0]) &&
        (((INT32 *) VendorGuid)[1] == ((INT32 *) TempVendorGuid)[1]) &&
        (((INT32 *) VendorGuid)[2] == ((INT32 *) TempVendorGuid)[2]) &&
        (((INT32 *) VendorGuid)[3] == ((INT32 *) TempVendorGuid)[3])
        ) {
      ASSERT (NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag) != 0);
      Point = (VOID *) GetVariableNamePtr (Variable, StoreInfo->AuthFlag);
      if (CompareVariableName (StoreInfo, VariableName, Point, NameSizeOfVariable (VariableHeader, StoreInfo->AuthFlag))) {
        PtrTrack->CurrPtr = Variable;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Return the variable HOB header and the store info for the given HOB type

  @param[in]  Type         The type of the variable HOB.
  @param[out] StoreInfo    Return the store info.

  @return     Pointer to the variable HOB header.

**/
VARIABLE_STORE_HEADER *
GetHobVariableStore (
  IN VARIABLE_HOB_TYPE           Type,
  OUT VARIABLE_STORE_INFO        *StoreInfo
  )
{
  EFI_HOB_GUID_TYPE              *GuidHob;

  StoreInfo->IndexTable = NULL;
  StoreInfo->AuthFlag = FALSE;

  switch (Type) {
    case VariableHobTypeCache:
      GuidHob = GetFirstGuidHob (&gPeiVariableCacheHobGuid);
      if (GuidHob != NULL) {
        StoreInfo->AuthFlag = FALSE;
        StoreInfo->VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
        return StoreInfo->VariableStoreHeader;
      }
      break;
    case VariableHobTypeDefault:
      GuidHob = GetFirstGuidHob (&gEfiAuthenticatedVariableGuid);
      if (GuidHob != NULL) {
        StoreInfo->AuthFlag = TRUE;
        StoreInfo->VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
        return StoreInfo->VariableStoreHeader;
      }
      GuidHob = GetFirstGuidHob (&gEfiVariableGuid);
      if (GuidHob != NULL) {
        StoreInfo->AuthFlag = FALSE;
        StoreInfo->VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
        return StoreInfo->VariableStoreHeader;
      }
      break;
  }
  return NULL;
}


/**
  Get variable header that has consecutive content.

  @param[in]  StoreInfo      Pointer to variable store info structure.
  @param[in]  Variable       Pointer to the Variable Header.
  @param[out] VariableHeader Pointer to Pointer to the Variable Header that has consecutive content.

  @retval     TRUE           Variable header is valid.
  @retval     FALSE          Variable header is not valid.

**/
BOOLEAN
GetVariableHeader (
  IN VARIABLE_STORE_INFO    *StoreInfo,
  IN VARIABLE_HEADER        *Variable,
  OUT VARIABLE_HEADER       **VariableHeader
  )
{

  if (Variable == NULL) {
    return FALSE;
  }

  //
  // First assume variable header pointed by Variable is consecutive.
  //
  *VariableHeader = Variable;

  if (Variable >= GetEndPointer (StoreInfo->VariableStoreHeader)) {
    //
    // Reach the end of variable store.
    //
    return FALSE;
  }

  return IsValidVariableHeader (*VariableHeader);
}


/**
  Get variable name or data to output buffer.

  @param[in]  StoreInfo     Pointer to variable store info structure.
  @param[in]  NameOrData    Pointer to the variable name/data that may be inconsecutive.
  @param[in]  Size          Variable name/data size.
  @param[out] Buffer        Pointer to output buffer to hold the variable name/data.

**/
VOID
GetVariableNameOrData (
  IN VARIABLE_STORE_INFO    *StoreInfo,
  IN UINT8                  *NameOrData,
  IN UINTN                  Size,
  OUT UINT8                 *Buffer
  )
{

  //
  // Variable name/data is consecutive.
  //
  CopyMem (Buffer, NameOrData, Size);
}


/**
  Find the variable in the specified variable store.

  @param[in]   StoreInfo              Pointer to the store info structure.
  @param[in]   VariableName           Name of the variable to be found
  @param[in]   VendorGuid             Vendor GUID to be found.
  @param[out]  PtrTrack               Variable Track Pointer structure that contains Variable Information.

  @retval      EFI_SUCCESS            Variable found successfully
  @retval      EFI_NOT_FOUND          Variable not found
  @retval      EFI_INVALID_PARAMETER  Invalid variable name

**/
EFI_STATUS
FindVariableInHobsInternal (
  IN VARIABLE_STORE_INFO         *StoreInfo,
  IN CONST CHAR16                *VariableName,
  IN CONST EFI_GUID              *VendorGuid,
  OUT VARIABLE_POINTER_TRACK     *PtrTrack
  )
{
  VARIABLE_HEADER         *Variable;
  VARIABLE_HEADER         *InDeletedVariable;
  VARIABLE_STORE_HEADER   *VariableStoreHeader;
  VARIABLE_HEADER         *VariableHeader;

  VariableStoreHeader = StoreInfo->VariableStoreHeader;

  if (VariableStoreHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (GetVariableStoreStatus (VariableStoreHeader) != EfiValid) {
    return EFI_UNSUPPORTED;
  }

  if (~VariableStoreHeader->Size == 0) {
    return EFI_NOT_FOUND;
  }

  PtrTrack->StartPtr = GetStartPointer (VariableStoreHeader);
  PtrTrack->EndPtr   = GetEndPointer   (VariableStoreHeader);

  InDeletedVariable = NULL;

  //
  // No Variable Address equals zero, so 0 as initial value is safe.
  //
  VariableHeader = NULL;

  //
  // Start at the beginning of the HOB
  //
  Variable = PtrTrack->StartPtr;

  //
  // Find the variable by walking through the variable store
  //
  while (GetVariableHeader (StoreInfo, Variable, &VariableHeader)) {
    if (VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      if (CompareWithValidVariable (StoreInfo, Variable, VariableHeader, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (VariableHeader->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }
    Variable = GetNextVariablePtr (StoreInfo, Variable, VariableHeader);
  }
  PtrTrack->CurrPtr = InDeletedVariable;

  return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}


/**
  Find the variable in HOB variable storages.

  @param[in]   VariableName           Name of the variable to be found
  @param[in]   VendorGuid             Vendor GUID to be found.
  @param[out]  PtrTrack               Variable Track Pointer structure that contains Variable Information.
  @param[out]  StoreInfo              Return the store info.

  @retval      EFI_SUCCESS            Variable found successfully
  @retval      EFI_NOT_FOUND          Variable not found
  @retval      EFI_INVALID_PARAMETER  Invalid variable name

**/
EFI_STATUS
FindVariableInHobs (
  IN CONST  CHAR16            *VariableName,
  IN CONST  EFI_GUID          *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT VARIABLE_STORE_INFO     *StoreInfo
  )
{
  EFI_STATUS                  Status;
  VARIABLE_HOB_TYPE           Type;

  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Type = (VARIABLE_HOB_TYPE) 0; Type < VariableHobTypeMax; Type++) {
    if (GetHobVariableStore (Type, StoreInfo) != NULL) {
      Status = FindVariableInHobsInternal (
                 StoreInfo,
                 VariableName,
                 VendorGuid,
                 PtrTrack
                 );
      if (!EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Retrieves a variable value from HOB resources.

  Read the specified variable from the UEFI variable store in the HOB. If the Data
  buffer is too small to hold the contents of the variable, the error
  EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the required buffer
  size to obtain the data.

  @param[in]      VariableName          A pointer to a null-terminated string that is the variable's name.
  @param[in]      VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                        VariableGuid and VariableName must be unique.
  @param[out]     Attributes            If non-NULL, on return, points to the variable's attributes.
  @param[in, out] DataSize              On entry, points to the size in bytes of the Data buffer.
                                        On return, points to the size of the data returned in Data.
  @param[out]     Data                  Points to the buffer which will hold the returned variable value.

  @retval         EFI_SUCCESS           The variable was read successfully.
  @retval         EFI_NOT_FOUND         The variable could not be found.
  @retval         EFI_BUFFER_TOO_SMALL  The DataSize is too small for the resulting data.
                                        DataSize is updated with the size required for
                                        the specified variable.
  @retval         EFI_INVALID_PARAMETER VariableName, VariableGuid, DataSize or Data is NULL.

**/
EFI_STATUS
EFIAPI
GetHobVariable (
  IN CONST  CHAR16                          *VariableName,
  IN CONST  EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *Attributes,
  IN OUT    UINTN                           *DataSize,
  OUT       VOID                            *Data
  )
{
  EFI_STATUS              Status;
  UINTN                   HobVariableDataSize;
  VARIABLE_HEADER         *HobVariableHeader;
  VARIABLE_HOB_TYPE       Type;
  VARIABLE_POINTER_TRACK  HobVariable;
  VARIABLE_STORE_INFO     HobStoreInfo;

  //
  // Check the HOB variable stores if they exist
  //
  for (Type = (VARIABLE_HOB_TYPE) 0; Type < VariableHobTypeMax; Type++) {
    if (GetHobVariableStore (Type, &HobStoreInfo) != NULL) {
      DEBUG ((EFI_D_INFO, "Temp Debug: Found a HOB variable store in PeiGetVariable()\n"));
      Status = FindVariableInHobsInternal (
                 &HobStoreInfo,
                 VariableName,
                 VariableGuid,
                 &HobVariable
                 );
      if (HobVariable.CurrPtr == NULL || EFI_ERROR (Status)) {
        if (Status == EFI_NOT_FOUND) {
          //
          // Check the next HOB
          //
          continue;
        }
        DEBUG ((EFI_D_ERROR, "Temp Debug: HobVariable found was null or had an error\n"));
        return Status;
      }

      GetVariableHeader (&HobStoreInfo, HobVariable.CurrPtr, &HobVariableHeader);

      //
      // Get data size
      //
      HobVariableDataSize = DataSizeOfVariable (HobVariableHeader, HobStoreInfo.AuthFlag);
      if (*DataSize >= HobVariableDataSize) {
        DEBUG ((EFI_D_INFO, "Temp Debug: Data buffer passed for variable data is large enough\n"));
        if (Data == NULL) {
          DEBUG ((EFI_D_ERROR, "Temp Debug: Data in the variable is NULL (invalid)\n"));
          return EFI_INVALID_PARAMETER;
        }

        GetVariableNameOrData (&HobStoreInfo, GetVariableDataPtr (HobVariable.CurrPtr, HobVariableHeader, HobStoreInfo.AuthFlag), HobVariableDataSize, Data);

        if (Attributes != NULL) {
          *Attributes = HobVariableHeader->Attributes;
        }

        *DataSize = HobVariableDataSize;

        DEBUG ((EFI_D_INFO, "Temp Debug: Variable was found in the HOB. No need to check storage PPIs. Returning success.\n\n"));

        //
        // Variable was found in the HOB
        //
        return EFI_SUCCESS;
      } else {
        DEBUG ((EFI_D_ERROR, "Temp Debug: Data buffer provided for variable data is too small!\n"));
        *DataSize = HobVariableDataSize;
        return EFI_BUFFER_TOO_SMALL;
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  This service retrieves a variable's value using its name and GUID.

  Read the specified variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable, the error
  EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the required buffer
  size to obtain the data.

  @param[in]      This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param[in]      VariableName          A pointer to a null-terminated string that is the variable's name.
  @param[in]      VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                        VariableGuid and VariableName must be unique.
  @param[out]     Attributes            If non-NULL, on return, points to the variable's attributes.
  @param[in, out] DataSize              On entry, points to the size in bytes of the Data buffer.
                                        On return, points to the size of the data returned in Data.
  @param[out]     Data                  Points to the buffer which will hold the returned variable value.

  @retval         EFI_SUCCESS           The variable was read successfully.
  @retval         EFI_NOT_FOUND         The variable could not be found.
  @retval         EFI_BUFFER_TOO_SMALL  The DataSize is too small for the resulting data.
                                        DataSize is updated with the size required for
                                        the specified variable.
  @retval         EFI_INVALID_PARAMETER VariableName, VariableGuid, DataSize or Data is NULL.
  @retval         EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
PeiGetVariable (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST  CHAR16                          *VariableName,
  IN CONST  EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *Attributes,
  IN OUT    UINTN                           *DataSize,
  OUT       VOID                            *Data
  )
{
  EFI_GUID                DiscoveredInstanceGuid;
  EFI_GUID                VariableStorageInstanceId;
  EFI_PEI_PPI_DESCRIPTOR  *VariableStoragePpiDescriptor;
  EFI_STATUS              Status;
  UINTN                   Instance;
  VARIABLE_STORAGE_PPI    *VariableStoragePpi;

  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL) {
    DEBUG ((EFI_D_ERROR, "Temp Debug: Invalid parameter passed to PeiGetVariable()\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if the variable can be found in a HOB store
  //
  Status = GetHobVariable (VariableName, VariableGuid, Attributes, DataSize, Data);

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_INFO, "Temp Debug: Could not find the variable in the HOBs. Checking storage PPIs...\n"));
  } else if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Temp Debug: Error occurred checking the HOBs for the variable.\n"));
  } else {
    return Status;
  }

  //
  // Determine which PPI instance should be used for this variable
  //
  Status = GetVariableStorageId (VariableName, VariableGuid, FALSE, &VariableStorageInstanceId);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Temp Debug: Could not find the Variable Storage PPI ID for this variable!\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  for (Instance = 0; Status != EFI_NOT_FOUND; Instance++) {
    Status = PeiServicesLocatePpi (
               &gVariableStoragePpiGuid,
               Instance,
               &VariableStoragePpiDescriptor,
               (VOID **) &VariableStoragePpi
               );

    if (!EFI_ERROR (Status) && !EFI_ERROR (VariableStoragePpi->GetId (&DiscoveredInstanceGuid))) {
      DEBUG ((EFI_D_INFO, "Correct VariableStorageId GUID = %g\nDiscoveredGuid = %g\n", &VariableStorageInstanceId, &DiscoveredInstanceGuid));

      if (CompareGuid (&VariableStorageInstanceId, &DiscoveredInstanceGuid)) {
        //
        // Found the appropriate Variable Storage PPI. Now use it to get the variable.
        //
        DEBUG ((EFI_D_INFO, "Temp Debug: Found the appropriate Variable Storage PPI. Calling VariableStoragePpi->GetVariable()...\n"));

        return VariableStoragePpi->GetVariable (VariableStoragePpi, VariableName, VariableGuid, Attributes, DataSize, Data);
      }
    } else if (Status != EFI_NOT_FOUND) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Return the next variable name and GUID in the HOB data store.

  @param[in]  VariableNameSize      On entry, points to the size of the buffer pointed to by VariableName.
                                    On return, the size of the variable name buffer.
  @param[in]  VariableName          On entry, a pointer to a null-terminated string that is the variable's name.
                                    On return, points to the next variable's null-terminated name string.
  @param[in]  VariableGuid          On entry, a pointer to an EFI_GUID that is the variable's GUID.
                                    On return, a pointer to the next variable's GUID.

  @retval     EFI_SUCCESS           The variable was read successfully.
  @retval     EFI_NOT_FOUND         The variable could not be found.
  @retval     EFI_BUFFER_TOO_SMALL  The VariableNameSize is too small for the resulting
                                    data. VariableNameSize is updated with the size
                                    required for the specified variable.
  @retval     EFI_INVALID_PARAMETER VariableName, VariableGuid or
                                    VariableNameSize is NULL.
  @retval     EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
PeiGetHobNextVariableName (
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VariableGuid
  )
{
  VARIABLE_HOB_TYPE       Type;
  VARIABLE_POINTER_TRACK  Variable;
  VARIABLE_POINTER_TRACK  VariableInDefaultHob;
  VARIABLE_POINTER_TRACK  VariablePtrTrack;
  UINTN                   VarNameSize;
  EFI_STATUS              Status;
  VARIABLE_STORE_HEADER   *VariableStoreHeader[VariableHobTypeMax];
  VARIABLE_HEADER         *VariableHeader;
  VARIABLE_STORE_INFO     StoreInfo;
  VARIABLE_STORE_INFO     StoreInfoForDefault;
  VARIABLE_STORE_INFO     StoreInfoForCache;

  if (VariableName == NULL || VariableGuid == NULL || VariableNameSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VariableHeader = NULL;

  Status = FindVariableInHobs (VariableName, VariableGuid, &Variable, &StoreInfo);
  if (Variable.CurrPtr == NULL || Status != EFI_SUCCESS) {
    return Status;
  }

  if (VariableName[0] != 0) {
    //
    // If variable name is not NULL, get next variable
    //
    GetVariableHeader (&StoreInfo, Variable.CurrPtr, &VariableHeader);
    Variable.CurrPtr = GetNextVariablePtr (&StoreInfo, Variable.CurrPtr, VariableHeader);
  }

  VariableStoreHeader[VariableHobTypeDefault] = GetHobVariableStore (VariableHobTypeDefault, &StoreInfoForDefault);
  VariableStoreHeader[VariableHobTypeCache]  = GetHobVariableStore (VariableHobTypeCache, &StoreInfoForCache);

  while (TRUE) {
    //
    // Switch from HOB to Non-Volatile.
    //
    while (!GetVariableHeader (&StoreInfo, Variable.CurrPtr, &VariableHeader)) {
      //
      // Find current storage index
      //
      for (Type = (VARIABLE_HOB_TYPE) 0; Type < VariableHobTypeMax; Type++) {
        if ((VariableStoreHeader[Type] != NULL) && (Variable.StartPtr == GetStartPointer (VariableStoreHeader[Type]))) {
          break;
        }
      }
      ASSERT (Type < VariableHobTypeMax);
      //
      // Switch to next storage
      //
      for (Type++; Type < VariableHobTypeMax; Type++) {
        if (VariableStoreHeader[Type] != NULL) {
          break;
        }
      }
      //
      // Capture the case that
      // 1. current storage is the last one, or
      // 2. no further storage
      //
      if (Type == VariableHobTypeMax) {
        return EFI_NOT_FOUND;
      }
      Variable.StartPtr = GetStartPointer (VariableStoreHeader[Type]);
      Variable.EndPtr   = GetEndPointer   (VariableStoreHeader[Type]);
      Variable.CurrPtr  = Variable.StartPtr;
      GetHobVariableStore (Type, &StoreInfo);
    }

    if (VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      if (VariableHeader->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
        //
        // If it is a IN_DELETED_TRANSITION variable,
        // and there is also a same ADDED one at the same time,
        // don't return it.
        //
        Status = FindVariableInHobsInternal (
                   &StoreInfo,
                   GetVariableNamePtr (Variable.CurrPtr, StoreInfo.AuthFlag),
                   GetVendorGuidPtr (VariableHeader, StoreInfo.AuthFlag),
                   &VariablePtrTrack
                   );
        if (!EFI_ERROR (Status) && VariablePtrTrack.CurrPtr != Variable.CurrPtr) {
          Variable.CurrPtr = GetNextVariablePtr (&StoreInfo, Variable.CurrPtr, VariableHeader);
          continue;
        }
      }

      //
      // Don't return cache HOB variable when default HOB overrides it
      //
      if ((VariableStoreHeader[VariableHobTypeDefault] != NULL) && (VariableStoreHeader[VariableHobTypeCache] != NULL) &&
          (Variable.StartPtr == GetStartPointer (VariableStoreHeader[VariableHobTypeCache]))
         ) {
        Status = FindVariableInHobsInternal (
                   &StoreInfoForDefault,
                   GetVariableNamePtr (Variable.CurrPtr, StoreInfo.AuthFlag),
                   GetVendorGuidPtr (VariableHeader, StoreInfo.AuthFlag),
                   &VariableInDefaultHob
                   );
        if (!EFI_ERROR (Status)) {
          Variable.CurrPtr = GetNextVariablePtr (&StoreInfo, Variable.CurrPtr, VariableHeader);
          continue;
        }
      }

      VarNameSize = NameSizeOfVariable (VariableHeader, StoreInfo.AuthFlag);
      ASSERT (VarNameSize != 0);

      if (VarNameSize <= *VariableNameSize) {
        GetVariableNameOrData (&StoreInfo, (UINT8 *) GetVariableNamePtr (Variable.CurrPtr, StoreInfo.AuthFlag), VarNameSize, (UINT8 *) VariableName);

        CopyMem (VariableGuid, GetVendorGuidPtr (VariableHeader, StoreInfo.AuthFlag), sizeof (EFI_GUID));

        Status = EFI_SUCCESS;
      } else {
        Status = EFI_BUFFER_TOO_SMALL;
      }

      *VariableNameSize = VarNameSize;
      //
      // Variable is found
      //
      return Status;
    } else {
      Variable.CurrPtr = GetNextVariablePtr (&StoreInfo, Variable.CurrPtr, VariableHeader);
    }
  }
}


/**
  Determines if a variable exists in the variable store HOBs

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
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  HobVariable;
  VARIABLE_STORE_INFO     HobStoreInfo;

  Status = FindVariableInHobs (VariableName, VariableGuid, &HobVariable, &HobStoreInfo);
  if ((HobVariable.CurrPtr != NULL) && (!EFI_ERROR (Status))) {
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
  Return the next variable name and GUID.

  This function is called multiple times to retrieve the VariableName
  and VariableGuid of all variables currently available in the system.
  On each call, the previous results are passed into the interface,
  and, on return, the interface returns the data for the next
  interface. When the entire variable list has been returned,
  EFI_NOT_FOUND is returned.

  @param[in]  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @param[in]  VariableNameSize      On entry, points to the size of the buffer pointed to by VariableName.
                                    On return, the size of the variable name buffer.
  @param[in]  VariableName          On entry, a pointer to a null-terminated string that is the variable's name.
                                    On return, points to the next variable's null-terminated name string.
  @param[in]  VariableGuid          On entry, a pointer to an EFI_GUID that is the variable's GUID.
                                    On return, a pointer to the next variable's GUID.

  @retval     EFI_SUCCESS           The variable was read successfully.
  @retval     EFI_NOT_FOUND         The variable could not be found.
  @retval     EFI_BUFFER_TOO_SMALL  The VariableNameSize is too small for the resulting
                                    data. VariableNameSize is updated with the size
                                    required for the specified variable.
  @retval     EFI_INVALID_PARAMETER VariableName, VariableGuid or
                                    VariableNameSize is NULL.
  @retval     EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
PeiGetNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VariableGuid
  )
{
  EFI_GUID                DiscoveredInstanceGuid;
  EFI_GUID                VariableStorageInstanceId;
  EFI_PEI_PPI_DESCRIPTOR  *VariableStoragePpiDescriptor;
  EFI_STATUS              Status;
  VARIABLE_STORAGE_PPI    *VariableStoragePpi;
  BOOLEAN                 SearchComplete;
  UINTN                   Instance;
  UINT32                  VarAttributes;

  if (VariableName == NULL || VariableGuid == NULL || VariableNameSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the HOBs first
  //
  Status = PeiGetHobNextVariableName (VariableNameSize, VariableName, VariableGuid);
  if (!EFI_ERROR (Status)) {
    return Status;
  } else if (Status != EFI_NOT_FOUND) {
    return Status;
  }
  //
  // If VariableName is an empty string or we reached the end of the HOBs,
  // get the first variable from the first Variable Storage PPI
  //
  if (VariableName[0] == 0 || (Status == EFI_NOT_FOUND && VariableExistsInHob (VariableName, VariableGuid))) {
    ZeroMem ((VOID *) VariableName, *VariableNameSize);
    ZeroMem ((VOID *) VariableGuid, sizeof (VariableGuid));
    Status = PeiServicesLocatePpi (
               &gVariableStoragePpiGuid,
               0,
               &VariableStoragePpiDescriptor,
               (VOID **) &VariableStoragePpi
               );
    if (!EFI_ERROR (Status)) {
        Status = VariableStoragePpi->GetNextVariableName (
                  VariableStoragePpi,
                  VariableNameSize,
                  VariableName,
                  VariableGuid,
                  &VarAttributes
                  );
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_INFO, "VariableStoragePpi->GetNextVariableName status %r\n", Status));
        }
    }
    return Status;
  }

  //
  // All of the HOB variables have been already enumerated, enumerate the Variable Storage PPIs
  //
  SearchComplete = FALSE;
  while (!SearchComplete) {
    Status = GetVariableStorageId (VariableName, VariableGuid, FALSE, &VariableStorageInstanceId);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    for (Instance = 0; Status != EFI_NOT_FOUND; Instance++) {
      Status = PeiServicesLocatePpi (
                 &gVariableStoragePpiGuid,
                 Instance,
                 &VariableStoragePpiDescriptor,
                 (VOID **) &VariableStoragePpi
                 );
      if (!EFI_ERROR (Status)) {
        Status = VariableStoragePpi->GetId (&DiscoveredInstanceGuid);
        if (!EFI_ERROR (Status)) {
          if (CompareGuid (&VariableStorageInstanceId, &DiscoveredInstanceGuid)) {
            Status = VariableStoragePpi->GetNextVariableName (
                      VariableStoragePpi,
                      VariableNameSize,
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
              return Status;
            } else if (Status == EFI_NOT_FOUND) {
              //
              // If we reached the end of the variables in the current variable
              // storage PPI, get the first variable in the next variable storage PPI
              //
              ZeroMem ((VOID *) VariableName, *VariableNameSize);
              ZeroMem ((VOID *) VariableGuid, sizeof (VariableGuid));
              Status = PeiServicesLocatePpi (
                         &gVariableStoragePpiGuid,
                         Instance + 1,
                         &VariableStoragePpiDescriptor,
                         (VOID **) &VariableStoragePpi
                         );
              if (!EFI_ERROR (Status)) {
                Status = VariableStoragePpi->GetNextVariableName (
                          VariableStoragePpi,
                          VariableNameSize,
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
                }
                return Status;
              } else if (Status == EFI_NOT_FOUND) {
                // This is the last variable
                SearchComplete = TRUE;
                break;
              } else {
                ASSERT_EFI_ERROR (Status);
                return Status;
              }
            } else {
              return Status;
            }
          }
        } else {
          ASSERT_EFI_ERROR (Status);
          return Status;
        }
      } else if (Status == EFI_NOT_FOUND) {
        SearchComplete = TRUE;
      } else {
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
    }
  }
  return EFI_NOT_FOUND;
}

