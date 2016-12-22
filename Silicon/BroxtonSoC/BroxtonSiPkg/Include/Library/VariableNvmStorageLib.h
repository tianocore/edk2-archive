/** @file
  Variable NVM Storage Helper library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VARIABLE_NVM_STORAGE_LIB_H_
#define _VARIABLE_NVM_STORAGE_LIB_H_

#include <Uefi.h>
#include <Guid/VariableFormat.h>
#include <VariableNvmStorageFormat.h>

/**
  Returns a pointer of type VARIABLE_NVM_STORE_HEADER to a buffer containing
  the header of an empty variable store.

  @param[out]  VarStoreHeader    Pointer to the Variable Store Header.

  @retval      EFI_SUCCESS       The variable store was created successfully.
  @retval      Others            An error occurred creating the variable store.

**/
EFI_STATUS
CreateEmptyVariableStore (
  OUT  VARIABLE_NVM_STORE_HEADER  *VarStoreHeader
  );

/**
  Gets the pointer to the first variable header in the given variable header region.

  @param[in]  VarStoreHeader    Pointer to the Variable Store Header.

  @return     Pointer to the first variable header

**/
VARIABLE_NVM_HEADER *
GetStartPointer (
  IN  VARIABLE_NVM_STORE_HEADER  *VarStoreHeader
  );

/**
  This code gets the pointer to the last variable header in the variable header region.

  @param[in]  VarStoreHeader     Pointer to the Variable Store Header.

  @return   VARIABLE_NVM_HEADER* pointer to last unavailable Variable Header.

**/
VARIABLE_NVM_HEADER *
GetEndPointer (
  IN  VARIABLE_NVM_STORE_HEADER  *VarStoreHeader
  );

/**
  This code checks if the variable header is valid or not.

  @param[in]  Variable          Pointer to the Variable Header.
  @param[in]  VariableStoreEnd  Pointer to the end of the Variable Store

  @retval     TRUE              Variable header is valid.
  @retval     FALSE             Variable header is not valid.

**/
BOOLEAN
IsValidVariableHeader (
  IN  VARIABLE_NVM_HEADER  *Variable,
  IN  VARIABLE_NVM_HEADER  *VariableStoreEnd
  );

/**
  This code checks if the variable header is valid or not.

  @param[in]   VarStoreHeader         Pointer to the Variable Store Header.
  @param[out]  IsAuthenticatedStore   Indicates whether this store is authenticated.

  @retval      EFI_SUCCESS            The authenticated status of the store was successfully determined.
  @retval      EFI_INVALID_PARAMETER  An invalid parameter was passed to the function.

**/
EFI_STATUS
IsAuthenticatedVariableStore (
  IN   VARIABLE_NVM_STORE_HEADER  *VarStoreHeader,
  OUT  BOOLEAN                    *IsAuthenticatedStore
  );

/**
  This code gets the size of the variable header.

  @param[in]  AuthFlag   Authenticated variable flag.

  @return     Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  IN  BOOLEAN       AuthFlag
  );

/**
  This code gets the size of the name of the variable.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     Size of variable in bytes in type UINTN.

**/
UINTN
NameSizeOfVariable (
  IN  VARIABLE_NVM_HEADER  *Variable,
  IN  BOOLEAN              AuthFlag
  );

/**
  This code gets the size of the variable data.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     Size of variable in bytes in type UINTN.

**/
UINTN
DataSizeOfVariable (
  IN  VARIABLE_NVM_HEADER  *Variable,
  IN  BOOLEAN              AuthFlag
  );

/**
  This code gets the pointer to the variable name.

  @param[in]   Variable    Pointer to the Variable Header.
  @param[in]   AuthFlag    Authenticated variable flag.

  @return      A CHAR16* pointer to Variable Name.

**/
CHAR16 *
GetVariableNamePtr (
  IN  VARIABLE_NVM_HEADER  *Variable,
  IN  BOOLEAN              AuthFlag
  );

/**
  This code gets the pointer to the variable guid.

  @param[in]  Variable    Pointer to the Variable Header.
  @param[in]  AuthFlag    Authenticated variable flag.

  @return     A EFI_GUID* pointer to Vendor Guid.

**/
EFI_GUID *
GetVendorGuidPtr (
  IN  VARIABLE_NVM_HEADER  *Variable,
  IN  BOOLEAN              AuthFlag
  );

/**
  This code gets the pointer to the variable data.

  This is an "offset" that is interpreted as needed based on the caller's requirements.

  @param[in]   Variable         Pointer to the Variable Header.
  @param[in]   StoreInfo        Pointer to a Variable Store Info structure for this variable.

  @return      A UINT32 value representing the offset to the variable data.

**/
UINT32
GetVariableDataPtr (
  IN  VARIABLE_NVM_HEADER     *Variable,
  IN  VARIABLE_NVM_STORE_INFO *StoreInfo
  );

/**
  This code gets the pointer to the next variable header.

  @param[in]  StoreInfo         Pointer to variable store info structure.
  @param[in]  VariableHeader    Pointer to the Variable Header.

  @return     A VARIABLE_NVM_HEADER* pointer to next variable header.

**/
VARIABLE_NVM_HEADER *
GetNextVariablePtr (
  IN  VARIABLE_NVM_STORE_INFO  *StoreInfo,
  IN  VARIABLE_NVM_HEADER      *VariableHeader
  );

/**
  Get variable store status.

  @param[in]  VarStoreHeader  Pointer to the Variable Store Header.

  @retval     EfiRaw          Variable store is raw
  @retval     EfiValid        Variable store is valid
  @retval     EfiInvalid      Variable store is invalid

**/
VARIABLE_STORE_STATUS
GetVariableStoreStatus (
  IN  VARIABLE_NVM_STORE_HEADER  *VarStoreHeader
  );

/**
  Compare two variable names.

  @param[in]  StoreInfo      Pointer to variable store info structure.
  @param[in]  Name1          Pointer to one variable name.
  @param[in]  Name2          Pointer to another variable name.
  @param[in]  NameSize       Variable name size.

  @retval     TRUE           Name1 and Name2 are identical.
  @retval     FALSE          Name1 and Name2 are not identical.

**/
BOOLEAN
CompareVariableName (
  IN  VARIABLE_NVM_STORE_INFO  *StoreInfo,
  IN  CONST CHAR16             *Name1,
  IN  CONST CHAR16             *Name2,
  IN  UINTN                    NameSize
  );

/**
  This function compares a variable with the variable entry in the database.

  @param[in]  StoreInfo      Pointer to variable store info structure.
  @param[in]  Variable       Pointer to the variable in our database
  @param[in]  VariableHeader Pointer to the Variable Header that has consecutive content.
  @param[in]  VariableName   Name of the variable to compare to 'Variable'
  @param[in]  VendorGuid     GUID of the variable to compare to 'Variable'
  @param[out] PtrTrack       Variable Track Pointer structure that contains Variable Information.

  @retval     EFI_SUCCESS    Found match variable
  @retval     EFI_NOT_FOUND  Variable not found

**/
EFI_STATUS
CompareWithValidVariable (
  IN  VARIABLE_NVM_STORE_INFO      *StoreInfo,
  IN  VARIABLE_NVM_HEADER          *Variable,
  IN  VARIABLE_NVM_HEADER          *VariableHeader,
  IN  CONST CHAR16                 *VariableName,
  IN  CONST EFI_GUID               *VendorGuid,
  OUT VARIABLE_NVM_POINTER_TRACK   *PtrTrack
  );

/**
  Validate the provided variable header.

  @param[in]  StoreInfo      Pointer to variable store info structure.
  @param[in]  Variable       Pointer to the Variable Header.
  @param[out] VariableHeader Pointer to Pointer to the Variable Header that has consecutive content.

  @retval     TRUE           Variable header is valid.
  @retval     FALSE          Variable header is not valid.

**/
BOOLEAN
GetVariableHeader (
  IN  VARIABLE_NVM_STORE_INFO   *StoreInfo,
  IN  VARIABLE_NVM_HEADER       *Variable,
  OUT VARIABLE_NVM_HEADER       **VariableHeader
  );

/**
  Copy the variable name to the output buffer.

  @param[in]   StoreInfo     Pointer to variable store info structure.
  @param[in]   NameOrData    Pointer to the variable name/data that may not be consecutive.
  @param[in]   Size          Variable name/data size.
  @param[out]  Buffer        Pointer to output buffer to hold the variable name/data.

**/
VOID
GetVariableName (
  IN  VARIABLE_NVM_STORE_INFO  *StoreInfo,
  IN  UINT8                    *Name,
  IN  UINTN                    Size,
  OUT UINT8                    *Buffer
  );

/**
  Compares two EFI_TIME data structures.

  @param[in]   FirstTime   A pointer to the first EFI_TIME data.
  @param[in]   SecondTime  A pointer to the second EFI_TIME data.

  @retval      TRUE        The FirstTime is not later than the SecondTime.
  @retval      FALSE       The FirstTime is later than the SecondTime.

**/
BOOLEAN
VariableNvmCompareTimeStamp (
  IN EFI_TIME      *FirstTime,
  IN EFI_TIME      *SecondTime
  );

#endif

