/** @file
  GUID for the the Pre-Memory Location HOB.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PRE_MEMORY_VARIABLE_LOCATION_HOB_GUID_H_
#define _PRE_MEMORY_VARIABLE_LOCATION_HOB_GUID_H_

extern EFI_GUID gPreMemoryVariableLocationHobGuid;

typedef struct {
  VOID      *VariableDataPtr;
  UINT32    VariableDataSize;
} PRE_MEMORY_VARIABLE_LOCATION_HOB;

#endif

