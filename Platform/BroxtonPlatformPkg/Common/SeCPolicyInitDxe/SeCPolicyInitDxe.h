/** @file
  Header file for SEC policy initialization.

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_SEC_PLATFORM_POLICY_H_
#define _DXE_SEC_PLATFORM_POLICY_H_

#include <PiDxe.h>
#include <Protocol/SeCPlatformPolicy.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SeCOperation.h>
#include <Protocol/Heci.h>
#include <Guid/SetupVariable.h>

#include "ScAccess.h"
#include "SetupMode.h"
#include "MkhiMsgs.h"
#include "HeciRegs.h"
#include "SeCChipset.h"
#include "CoreBiosMsg.h"
#include "SeCAccess.h"
#include <Library/SeCLib.h>

#define FW_MSG_DELAY          1000    // show warning msg and stay for 1 milisecond.
#define FW_MSG_DELAY_TIMEOUT  10


EFI_STATUS
EFIAPI
GetPlatformSeCInfo (
  OUT SEC_INFOMATION  *SeCInfo
  );

EFI_STATUS
EFIAPI
SetPlatformSeCInfo (
  IN SEC_INFOMATION  *SeCInfo
  );

EFI_STATUS
EFIAPI
PerformSeCOperation (
  IN  UINTN  SeCOpId
  );

VOID
EFIAPI
ShowSeCReportError (
  IN SEC_ERROR_MSG_ID            MsgId
  );

VOID
EFIAPI
SeCPolicyReadyToBootEvent (
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
  );

#endif

