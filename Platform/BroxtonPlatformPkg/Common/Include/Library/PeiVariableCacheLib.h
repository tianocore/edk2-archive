/** @file
  Header file for the Variable Cache Library.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_VARIABLE_CACHE_LIB_H_
#define _PEI_VARIABLE_CACHE_LIB_H_

#include <PiPei.h>

#include <Ppi/ReadOnlyVariable2.h>

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/PeiVariableCacheHobGuid.h>
#include <Guid/PreMemoryVariableLocationHobGuid.h>
#include <Guid/SetupVariable.h>
#include <Guid/VariableFormat.h>

#pragma pack(push, 1)

typedef struct {
  EFI_HOB_GUID_TYPE        HobHeader;
  VARIABLE_STORE_HEADER    VariableCacheStoreHeader;
  VARIABLE_HEADER          SetupVariableHeader;
  CHAR16                   SetupVariableName[sizeof (PLATFORM_SETUP_VARIABLE_NAME)];
  SETUP_DATA               SetupVariableData;
} VARIABLE_CACHE_STORE;

#pragma pack(pop)

/**
  Creates the PEI variable cache HOB.

  @param  VOID

  @retval EFI_SUCCESS           The PEI variable cache was successfully created

**/
EFI_STATUS
EFIAPI
CreateVariableCacheHob (
  VOID
  );

#endif

