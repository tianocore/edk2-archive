/** @file
  This file contains common declarations for Variable Storage data structures.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VARIABLE_STORAGE_FORMAT_H_
#define _VARIABLE_STORAGE_FORMAT_H_

#define VARIABLE_NVM_STORE_REVISION_1_0   0x1

#define NVM_HEADER_ALIGNMENT  4
#define NVM_HEADER_ALIGN(a)   (((UINTN) (a) + NVM_HEADER_ALIGNMENT - 1) & (~(UINTN)(NVM_HEADER_ALIGNMENT - 1)))

//
// Calculates the minimal pad bytes needed to make the current pad size satisfy the alignment requirement.
//
#define NVM_GET_PAD_SIZE(a)    (((~a) + 1) & (NVM_HEADER_ALIGNMENT - 1)) ///< Bytes for alignment padding higher in memory
#define NVM_GET_PAD_SIZE_N(a)  (((~a) - 1) & (NVM_HEADER_ALIGNMENT - 1)) ///< Bytes for alignment padding lower in memory

#pragma pack (push, 1)

//
// Variable Store Header
//
// This table is at the beginning of every variable store.
//
typedef struct {
  EFI_GUID  Signature;                   ///< Signature to identify this table
  UINT32    Size;                        ///< Length in bytes of this entire header
  UINT8     Revision;                    ///< Revision of this header
  UINT8     Checksum;                    ///< Checksum of this header
  UINT8     Format;                      ///< Format of the variable store
  UINT8     State;                       ///< Indicates the current state of the variable store
  UINT32    VariableDataTotalLength;     ///< Length in bytes of all variable data in this store
  UINT32    VariableHeaderTotalEntries;  ///< Total number of variable headers in this store
  UINT32    VariableHeaderTotalLength;   ///< Length in bytes of all variable headers in this store
  UINT32    VariableHeaderOffset;        ///< Offset, in bytes, from the beginning of this header
                                         ///< to the first variable header
} VARIABLE_NVM_STORE_HEADER;

//
// Variable Header
//
// This header maintains all meta data associated with a variable.
//
typedef struct {
  UINT16      StartId;     ///< Variable data section start identifier
  UINT8       Reserved;    ///< Reserved field
  UINT8       State;       ///< State of the variable
  UINT32      Attributes;  ///< Variable attributes
  UINT32      NameSize;    ///< Length in bytes of the variable name string
  UINT32      DataSize;    ///< Length in bytes of the variable data region
  UINT32      DataOffset;  ///< Offset in bytes from the beginning of this
                           ///< structure to the corresponding data section
  EFI_GUID    VendorGuid;  ///< Unique identifier for the variable vendor
} VARIABLE_NVM_HEADER;

//
// Authenticated Variable Header
//
typedef struct {
  UINT16      StartId;         ///< Variable data section start identifier
  UINT8       Reserved;        ///< Reserved field
  UINT8       State;           ///< State of the variable
  UINT32      Attributes;      ///< Variable attributes
  UINT32      NameSize;        ///< Length in bytes of the variable name string
  UINT32      DataSize;        ///< Length in bytes of the variable data region
  UINT32      DataOffset;      ///< Offset in bytes from the beginning of this
                               ///< structure to the corresponding data section
  EFI_GUID    VendorGuid;      ///< Unique identifier for the variable vendor
  UINT64      MonotonicCount;  ///< Associated monotonic count value to protect against replay attack
  EFI_TIME    TimeStamp;       ///< Associated TimeStamp value to protect against replay attack
  UINT32      PubKeyIndex;     ///< Index of associated public key in database
} AUTHENTICATED_VARIABLE_NVM_HEADER;

//
// Variable Index Table
//
// This table tracks distance between variable headers.
//
typedef struct {
  UINT16               Length;       ///< Length of the index table
  UINT16               GoneThrough;  ///< Entries gone through when searching
  VARIABLE_NVM_HEADER  *EndPtr;      ///< Last variable in the variable header region
  VARIABLE_NVM_HEADER  *StartPtr;    ///< First variable in the variable header region
  //
  // This field is used to store the distance
  // of two neighboring VAR_ADDED type variables.
  // The meaning of this field is implementation-
  // dependent.
  //
} VARIABLE_NVM_INDEX_TABLE;

//
// Variable Store Info
//
// Associates an index table with a corresponding variable store.
//
typedef struct {
  VARIABLE_NVM_STORE_HEADER  *VariableStoreHeader;  ///< Header for the NVM variable store
  VARIABLE_NVM_INDEX_TABLE   *IndexTable;           ///< Pointer to the variable header region index table
  BOOLEAN                    AuthFlag;              ///< Indicates if the store contains authenticated variables
} VARIABLE_NVM_STORE_INFO;

//
// Variable Pointer Tracking Structure
//
// This structure is used to track variable header pointers and information.
//
typedef struct {
  VARIABLE_NVM_HEADER        *CurrPtr;
  VARIABLE_NVM_HEADER        *StartPtr;
  VARIABLE_NVM_HEADER        *EndPtr;
  VARIABLE_NVM_HEADER        *InDeletedTransitionPtr;
} VARIABLE_NVM_POINTER_TRACK;

#pragma pack (pop)

#endif

