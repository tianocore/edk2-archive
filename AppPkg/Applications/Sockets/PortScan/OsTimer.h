/** @file
  Definitions for the OS timer support routines

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _OS_TIMER_H_
#define _OS_TIMER_H_

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "OsTypes.h"

//------------------------------------------------------------------------------
//  Timer API
//------------------------------------------------------------------------------

/**
  Cancel a timer

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerCancel (
  IN EFI_EVENT TimerEvent
  );

/**
  Close a timer

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerClose (
  IN EFI_EVENT TimerEvent
  );

/**
  Create a timer

  @param [out] TimerEvent Address to return the timer event handle

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerCreate (
  OUT EFI_EVENT * pTimerEvent
  );

/**
  Determine if a timer has expired

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Timer expired
 **/
EFI_STATUS
OsTimerExpired (
  IN EFI_EVENT TimerEvent
  );

/**
  Start a timer

  @param [in] TimerEvent  Event handle for the timer
  @param [in] Timeout     Relative time when the timer expires

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerStart (
  IN EFI_EVENT TimerEvent,
  IN UINT64 Timeout
  );

//------------------------------------------------------------------------------

#endif  //  _OS_TIMER_H_