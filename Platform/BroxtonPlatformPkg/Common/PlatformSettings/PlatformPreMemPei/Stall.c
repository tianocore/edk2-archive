/** @file
  Produce Stall Ppi.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <ScAccess.h>
#include <PlatformBaseAddresses.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/Stall.h>
#include "Stall.h"  //Function Prototypes


/**
  Waits for at least the given number of microseconds.

  @param[in]  PeiServices     General purpose services available to every PEIM.
  @param[in]  This            PPI instance structure.
  @param[in]  Microseconds    Desired length of time to wait.

  @retval     EFI_SUCCESS     If the desired amount of time was passed.

**/
EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN CONST EFI_PEI_STALL_PPI  *This,
  IN UINTN                    Microseconds
  )
{
  //
  // !! This should be re-written to use TimerLib !!
  //
  UINTN   Ticks;
  UINTN   Counts;
  UINT16  AcpiBaseAddr;
  UINT32  CurrentTick;
  UINT32  OriginalTick;
  UINT32  RemainingTick;

  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  OriginalTick = IoRead32 (AcpiBaseAddr + R_ACPI_PM1_TMR);
  OriginalTick &= (V_ACPI_PM1_TMR_MAX_VAL - 1);
  CurrentTick = OriginalTick;

  //
  // The timer frequency is 3.579545MHz, so 1 ms corresponds to 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  //
  // The loops needed for timer overflow
  //
  Counts = (UINTN) RShiftU64 ((UINT64) Ticks, 24);

  //
  // Remaining clocks within one loop
  //
  RemainingTick = Ticks & 0xFFFFFF;

  //
  // Do not intend to use TMROF_STS bit of register PM1_STS, because this add extra
  // one I/O operation, and may generate SMI
  //
  while (Counts != 0) {
    CurrentTick = IoRead32 (AcpiBaseAddr + R_ACPI_PM1_TMR) & B_ACPI_PM1_TMR_VAL;
    if (CurrentTick <= OriginalTick) {
      Counts--;
    }
    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = IoRead32 (AcpiBaseAddr + R_ACPI_PM1_TMR) & B_ACPI_PM1_TMR_VAL;
  }

  return EFI_SUCCESS;
}

