/** @file
  HII Library implementation that uses DXE protocols and services.

  Copyright (c) 2006, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <PiDxe.h>

#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS
EFIAPI
HiiLibCreateNewPackages (
  IN UINTN      NumberOfPackages,
  IN EFI_GUID   *GuidId,
  IN EFI_HANDLE DriverHandle,
  OUT VOID      **HiiHandle,         //Framework is FRAMEWORK_HII_HANDLE; UEFI is EFI_HII_HANDLE; 
                                     // C:\D\Work\Tiano\Tiano_Main_Trunk\TIANO\Platform\IntelEpg\SR870BN4\MemorySubClassDriver\DualChannelDdr\MemorySubClass.c make use of this output value
  ...
  );
{

  return EFI_SUCCESS;
}
