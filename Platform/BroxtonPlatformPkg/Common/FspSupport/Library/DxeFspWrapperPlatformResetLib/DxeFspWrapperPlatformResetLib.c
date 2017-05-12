/** @file
  Dxe library function to reset the system from FSP wrapper.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HeciMsgLib.h>
#include <FspEas.h>

/**
  Perform platform related reset in FSP wrapper.

  @param[in] ResetType  The type of reset the platform has to perform.

  @return    Will reset the system based on Reset status provided.

**/
VOID
EFIAPI
CallFspWrapperResetSystem (
  IN UINT32    ResetType
  )
{
  EFI_RESET_TYPE             EfiResetType = EfiResetCold;

  switch (ResetType) {
    case FSP_STATUS_RESET_REQUIRED_COLD:
      EfiResetType = EfiResetCold;
      break;
    case FSP_STATUS_RESET_REQUIRED_WARM:
      EfiResetType = EfiResetWarm;
      break;
    case FSP_STATUS_RESET_REQUIRED_3:
      EfiResetType = EfiResetShutdown;
      break;
    case FSP_STATUS_RESET_REQUIRED_5:
      HeciSendResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
      CpuDeadLoop();
      break;
    default:
      return;
  }
  gRT->ResetSystem (EfiResetType, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop();
}

