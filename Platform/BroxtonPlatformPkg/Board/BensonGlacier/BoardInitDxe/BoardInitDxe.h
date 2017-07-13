/** @file
  The internal header file includes the common header files, defines
  internal structure and functions used by ImageVerificationLib.

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BENSON_BOARD_INIT_DXE_H__
#define __BENSON_BOARD_INIT_DXE_H__

#include <BoardFunctionsDxe.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Guid/PlatformInfo_Aplk.h>

CHAR16*
EFIAPI
BgGetBoardName (
  IN  UINT8                   BoardId
  );

#endif
