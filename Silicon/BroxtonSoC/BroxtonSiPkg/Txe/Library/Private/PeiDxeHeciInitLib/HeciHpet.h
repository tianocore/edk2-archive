/** @file
  Definitions for HECI driver.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_HPET_H
#define _HECI_HPET_H

#include "HeciCore.h"
#include <IndustryStandard/Pci22.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <HeciRegs.h>
#include <SeCAccess.h>

volatile
UINT32 *
EnableHpet (
  VOID
  );

VOID
SaveHpet (
  VOID
  );

/**
  Restore the value of High Performance Timer

  @param[in]  None

  @retval     None

**/
VOID
RestoreHpet (
  VOID
  );

/**
  Used for calculating timeouts

  @param[in]  Start   Snapshot of the HPET timer
  @param[in]  End     Calculated time when timeout period will be done
  @param[in]  Time    Timeout period in microseconds

  @retval     None

**/
volatile
UINT32 *
StartTimer (
  OUT UINT32 *Start,
  OUT UINT32 *End,
  IN  UINT32 Time
  );

/**
  Used to determine if a timeout has occured.

  @param[in]  Start   Snapshot of the HPET timer when the timeout period started.
  @param[in]  End     Calculated time when timeout period will be done.

  @retval     EFI_STATUS

**/
EFI_STATUS
Timeout (
  IN  UINT32 Start,
  IN  UINT32                      End,
  IN  volatile UINT32             *HpetTimer
  );

/**
  Delay for at least the request number of microseconds

  @param[in]  delayTime       Number of microseconds to delay.

  @retval     None

**/
VOID
IoDelay (
  UINT32 delayTime
  );

#endif ///< _HECI_HPET_H

