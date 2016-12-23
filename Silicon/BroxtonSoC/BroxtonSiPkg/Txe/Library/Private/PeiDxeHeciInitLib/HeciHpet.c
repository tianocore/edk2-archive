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

#include "HeciHpet.h"

//
// Extern for shared HECI data and protocols
//
volatile UINT32       mSaveHpetConfigReg;

/**
  Enable Hpet function.

  @param[in]  None

  @retval     UINT32            Return the High Precision Event Timer base address

**/
volatile
UINT32 *
EnableHpet (
  VOID
  )
{
  volatile UINT32         *HpetTimer;

  HpetTimer = (VOID *) (UINTN) (HPET_BASE_ADDRESS);

  //
  // Start the timer so it is up and running
  //
  HpetTimer[HPET_GEN_CONFIG_LOW] = HPET_START;

  DEBUG ((EFI_D_INFO, "EnableHpet %x %x\n\n ", HPET_GEN_CONFIG_LOW, HpetTimer));
  return HpetTimer;

}


/**
  Store the value of High Performance Timer

  @param[in]  None

  @retval     None

**/
VOID
SaveHpet (
  VOID
  )
{
}

/**
  Restore the value of High Performance Timer

  @param[in]  None

  @retval    None

**/
VOID
RestoreHpet (
  VOID
  )
{
}

/**
  Used for calculating timeouts

  @param[in]  Start   Snapshot of the HPET timer
  @param[in]  End     Calculated time when timeout period will be done
  @param[in]  Time    Timeout period in microseconds

  @retval  None

**/
volatile
UINT32 *
StartTimer (
  OUT UINT32 *Start,
  OUT UINT32 *End,
  IN  UINT32 Time
  )
{
  UINT32          Ticks;
  volatile UINT32 *HpetTimer;

  //
  // Make sure that HPET is enabled and running
  //
  HpetTimer = EnableHpet ();

  //
  // Read current timer value into start time from HPET
  //
  *Start = HpetTimer[HPET_MAIN_COUNTER_LOW];

  //
  // Convert microseconds into 70ns timer ticks
  //
  Ticks = Time * HPET_TICKS_PER_MICRO;

  //
  // Compute end time
  //
  *End = *Start + Ticks;

  return HpetTimer;
}


/**
  Used to determine if a timeout has occured.

  @param[in]  Start   Snapshot of the HPET timer when the timeout period started.
  @param[in]  End     Calculated time when timeout period will be done.

  @retval     EFI_STATUS

**/
EFI_STATUS
Timeout (
  IN  UINT32                      Start,
  IN  UINT32                      End,
  IN  volatile UINT32             *HpetTimer
  )
{
  UINT32  Current;

  //
  // Read HPET and assign the value as the current time.
  //
  Current = HpetTimer[HPET_MAIN_COUNTER_LOW];

  //
  // Test basic case (no overflow)
  //
  if ((Start < End) && (End <= Current)) {
    return EFI_TIMEOUT;
  }

  //
  // Test basic start/end conditions with overflowed timer
  //
  if ((Start < End) && (Current < Start)) {
    return EFI_TIMEOUT;
  }

  //
  // Test for overflowed start/end condition
  //
  if ((Start > End) && ((Current < Start) && (Current > End))) {
    return EFI_TIMEOUT;
  }

  //
  // Catch corner case of broken arguments
  //
  if (Start == End) {
    return EFI_TIMEOUT;
  }

  DEBUG ((EFI_D_INFO, "crnt %X start %X end %X\n", Current, Start, End));

  //
  // Else, we have not yet timed out
  //
  return EFI_SUCCESS;
}


/**
  Delay for at least the request number of microseconds

  @param[in]  delayTime    Number of microseconds to delay.

  @retval     None

**/
VOID
IoDelay (
  UINT32 delayTime
  )
{
  MicroSecondDelay(delayTime);
}

