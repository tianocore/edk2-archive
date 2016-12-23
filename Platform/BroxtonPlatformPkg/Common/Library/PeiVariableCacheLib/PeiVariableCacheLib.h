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
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/PeiVariableCacheHobGuid.h>
#include <Guid/PreMemoryVariableLocationHobGuid.h>
#include <Guid/VariableFormat.h>


/**
  Creates the variable cache HOB which holds variables in memory decided
  by the platform.

  @param  VOID

  @retval EFI_SUCCESS           The PEI variable cache was successfully created

**/
EFI_STATUS
EFIAPI
CreateVariableCacheHob (
  VOID
  );

#endif

