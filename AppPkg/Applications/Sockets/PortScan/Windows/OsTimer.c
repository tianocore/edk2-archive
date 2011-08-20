/** @file
  Windows version of the port map test application
  

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <OsTimer.h>


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
  //  Assume success
  //
  Status = NO_ERROR;

  //
  //  Start the timer
  //
  if ( !CancelWaitableTimer ( TimerEvent )) {
    Status = GetLastError ( );
  }

  //
  //  Return the start status
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
  //  Assume success
  //
  Status = NO_ERROR;

  //
  //  Close the timer event
  //
  if ( !CloseHandle ( TimerEvent )) {
    Status = GetLastError ( );
  }

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
  EFI_EVENT TimerEvent;
  EFI_STATUS Status;

  //
  //  Assume success
  //
  Status = NO_ERROR;

  //
  //  Attempt to create the timer event
  //
  TimerEvent = CreateWaitableTimerA ( NULL, TRUE, NULL );
  if ( NULL == TimerEvent ) {
    Status = GetLastError ( );
  }

  //
  //  Return the timer handle
  //
  *pTimerEvent = TimerEvent;

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
  //  Determine if the timer is set
  //
  Status = WaitForSingleObject ( TimerEvent, 0 );

  //
  //  Return the timeout status
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
  INT64 DueTime;
  EFI_STATUS Status;

  //
  //  Assume success
  //
  Status = NO_ERROR;

  //
  //  Start the timer
  //
  DueTime = -(INT64)Timeout;
  if ( !SetWaitableTimer ( TimerEvent,
                           (LARGE_INTEGER *)&DueTime,
                           0,
                           NULL,
                           NULL,
                           0 )) {
    Status = GetLastError ( );
  }

  //
  //  Return the start status
  //
  return Status;
}