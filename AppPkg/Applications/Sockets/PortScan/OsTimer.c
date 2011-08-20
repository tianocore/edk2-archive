/** @file
  OS Timer Support

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "OsTimer.h"
#include <Library/UefiBootServicesTableLib.h>


/**
  Cancel a timer

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerCancel (
  IN EFI_EVENT TimerEvent
  )
{
  EFI_STATUS Status;

  //
  //  Cancel the timer
  //
  Status = gBS->SetTimer ( TimerEvent,
                           TimerCancel,
                           0 );

  //
  //  Return the cancel status
  //
  return Status;
}


/**
  Close a timer

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerClose (
  IN EFI_EVENT TimerEvent
  )
{
  EFI_STATUS Status;

  //
  //  Close the timer event
  //
  Status = gBS->CloseEvent ( TimerEvent );

  //
  //  Return the close status
  //
  return Status;
}


/**
  Create a timer

  @param [out] TimerEvent Address to return the timer event handle

  @retval 0               Successfully operation
 **/
EFI_STATUS
OsTimerCreate (
  OUT EFI_EVENT * pTimerEvent
  )
{
  EFI_STATUS Status;

  //
  //  Create the timer event
  //
  Status = gBS->CreateEvent ( EVT_TIMER,
                              0,
                              NULL,
                              NULL,
                              pTimerEvent );

  //
  //  Return the creation status
  //
  return Status;
}


/**
  Determine if a timer has expired

  @param [in] TimerEvent  Event handle for the timer

  @retval 0               Timer expired
 **/
EFI_STATUS
OsTimerExpired (
  IN EFI_EVENT TimerEvent
  )
{
  EFI_STATUS Status;

  //
  //  Determine if the timer is expired
  //
  Status = gBS->CheckEvent ( TimerEvent );

  //
  //  Return the expiration status
  //
  return Status;
}


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
  )
{
  EFI_STATUS Status;

  //
  //  Start the timer
  //
  Status = gBS->SetTimer ( TimerEvent,
                           TimerRelative,
                           Timeout );

  //
  //  Return the start status
  //
  return Status;
}
