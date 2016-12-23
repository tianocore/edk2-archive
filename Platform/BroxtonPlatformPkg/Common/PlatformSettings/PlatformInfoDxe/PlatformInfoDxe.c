/** @file
  Platform Info driver to public platform related HOB data.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformInfoDxe.h"
#include <Guid/BxtVariable.h>


/**
  Entry point for the driver.

  This routine get the platform HOB data from PEI and publish
  as Platform Info variable that can be accessed during boot service and
  runtime.

  @param[in]  ImageHandle    Image Handle.
  @param[in]  SystemTable    EFI System Table.

  @retval     Status         Function execution status.

**/
EFI_STATUS
EFIAPI
PlatformInfoInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_PLATFORM_INFO_HOB       *PlatformInfoHobPtr;
  EFI_PEI_HOB_POINTERS        GuidHob;

  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
    PlatformInfoHobPtr = GET_GUID_HOB_DATA (GuidHob.Guid);
    //
    // Write the Platform Info to volatile memory
    //
    Status = gRT->SetVariable(
                    L"PlatformInfo",
                    &gEfiBxtVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (EFI_PLATFORM_INFO_HOB),
                    PlatformInfoHobPtr
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

