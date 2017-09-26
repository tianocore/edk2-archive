/** @file
  Board specific functions in DXE phase to be set as dynamic PCD and consumed by
  commmon platform code.

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitDxe.h"

GET_BOARD_NAME mBgGetBoardNamePtr = BgGetBoardName;

CHAR16*
EFIAPI
BgGetBoardName (
  IN  UINT8                   BoardId
  )
{
  STATIC CHAR16  BoardName[40];

  DEBUG ((EFI_D_INFO,  "BoardInitDxe: GetBoardName - Benson Glacier\n"));

  UnicodeSPrint (BoardName, sizeof (BoardName), L"Benson Glacier ");

  if (BoardId != (UINT8) BOARD_ID_BENSON) {
    return NULL;
  } else {
    return BoardName;
  }
}


/**
  Set PCDs for board specific functions.

  @param[in]  ImageHandle   ImageHandle of the loaded driver.
  @param[in]  SystemTable   Pointer to the EFI System Table.

  @retval     EFI_SUCCESS   The handlers were registered successfully.

**/
EFI_STATUS
EFIAPI
BgBoardInitDxeConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8       BoardId;

  BoardId = PcdGet8 (PcdBoardId);
  if (BoardId != (UINT8) BOARD_ID_BENSON) {
    return EFI_SUCCESS;
  }

  PcdSet64 (PcdGetBoardNameFunc, (UINT64) mBgGetBoardNamePtr);

  return EFI_SUCCESS;
}

