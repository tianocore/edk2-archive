/** @file
  Header file for the Platform ID code.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MINNOWBOARD_NEXT_PLATFORM_ID_H__
#define __MINNOWBOARD_NEXT_PLATFORM_ID_H__

typedef struct {
  UINT32   CommAndOffset;
  UINT8    IOSTerm;
  UINT8    Term;
} PAD_ID_INFO;

typedef struct {
  UINT32   RawId;
  UINT8    BoardId;
} BOARD_ID_INFO;

EFI_STATUS
EFIAPI
Minnow3NextGetEmbeddedBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  );

UINT32
EFIAPI
Minnow3NextGetHwconfStraps (
  VOID
  );

#endif

