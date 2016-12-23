/** @file
  Mmc Main PEI Library header.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_MMC_MAIN_LIB_H_
#define _PEI_MMC_MAIN_LIB_H_

/**
  Entry point for EFI drivers.

  @param[in]  PeiServices              EFI_PEI_SERVICES

  @retval     EFI_SUCCESS              Success
  @retval     EFI_DEVICE_ERROR         Fail

**/
EFI_STATUS
EFIAPI
MmcMainEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

#endif

