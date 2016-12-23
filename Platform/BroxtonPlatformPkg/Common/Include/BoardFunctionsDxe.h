/** @file
  Typedefs for board specific functions that are set as dynamic PCDs to be consumed
  by platform common codes.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOARD_FUNCTIONS_DXE_H
#define _BOARD_FUNCTIONS_DXE_H

#include <PiDxe.h>

typedef
CHAR16*
(EFIAPI *GET_BOARD_NAME) (
  IN  UINT8                   BoardId
  );

#endif

