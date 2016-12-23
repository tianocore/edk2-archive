/** @file
  SMM Stall implementation.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// Include files
//
#include "Library/StallSmmLib.h"
#include "Pi/PiSmmCis.h"
#include "PiDxe.h"
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include "ScAccess.h"


/**
  Delay for at least the request number of microseconds.
  Timer used is ACPI time counter, which has 1us granularity.

  @param[in] Microseconds      Number of microseconds to delay.

  @retval    None

**/
VOID
SmmStall (
  IN  UINTN   Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;
  UINT16  AcpiBaseAddr;

  if (Microseconds == 0) {
    return;
  }

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  OriginalTick = IoRead32 (AcpiBaseAddr + R_ACPI_PM1_TMR);
  CurrentTick = OriginalTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  //
  // The loops needed by timer overflow
  //
  Counts = Ticks / V_ACPI_PM1_TMR_MAX_VAL;

  //
  // Remaining clocks within one loop
  //
  RemainingTick = Ticks % V_ACPI_PM1_TMR_MAX_VAL;

  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 (AcpiBaseAddr + R_ACPI_PM1_TMR);
    //
    // Check if timer overflow
    //
    if (CurrentTick < OriginalTick) {
      Counts--;
    }
    OriginalTick = CurrentTick;
  }
}

