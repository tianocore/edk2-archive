/** @file
  Board specific functions in DXE phase to be set as dynamic PCD and consumed by
  commmon platform code.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitDxe.h"

GET_BOARD_NAME mLhGetBoardNamePtr = LhGetBoardName;

CHAR16*
EFIAPI
LhGetBoardName (
  IN  UINT8                   BoardId
  )
{
  STATIC CHAR16  BoardName[40];

  DEBUG ((EFI_D_INFO,  "BoardInitDxe: GetBoardName - Leaf Hill\n"));
  UnicodeSPrint (BoardName, sizeof (BoardName), L"Leaf Hill CRB ");

  if (BoardId != (UINT8) BOARD_ID_LFH_CRB) {
    return NULL;
  } else {
    return BoardName;
  }
}


/**
  Set PCDs for board specific functions

  @param[in]  ImageHandle   ImageHandle of the loaded driver.
  @param[in]  SystemTable   Pointer to the EFI System Table.

  @retval     EFI_SUCCESS   The handlers were registered successfully.

**/
EFI_STATUS
EFIAPI
LhBoardInitDxeConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8       BoardId;

  BoardId = PcdGet8 (PcdBoardId);
  if (BoardId != (UINT8) BOARD_ID_LFH_CRB) {
    return EFI_SUCCESS;
  }

  PcdSet64 (PcdGetBoardNameFunc, (UINT64) mLhGetBoardNamePtr);

  return EFI_SUCCESS;
}

