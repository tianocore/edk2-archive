/** @file
  RTC Power failure handler.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformInit.h"


BOOLEAN
IsRtcUipAlwaysSet (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_PEI_STALL_PPI  *StallPpi;
  UINTN              Count;

  (**PeiServices).LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi);

  for (Count = 0; Count < 500; Count++) { // Maximum waiting approximates to 1.5 seconds (= 3 msec * 500)
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERA);
    if ((IoRead8 (R_RTC_TARGET2) & B_RTC_REGISTERA_UIP) == 0) {
      return FALSE;
    }

    StallPpi->Stall (PeiServices, StallPpi, 3000);
  }

  return TRUE;
}


EFI_STATUS
RtcPowerFailureHandler (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  UINT16          DataUint16;
  UINT8           DataUint8;
  BOOLEAN         RtcUipIsAlwaysSet;

  //
  // When the RTC_PWR_STS bit is set, it indicates that the RTCRST# signal went low.
  // Software should clear this bit. Changing the RTC battery sets this bit.
  // System BIOS should reset CMOS to default values if the RTC_PWR_STS bit is set.
  //
  // The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set before memory initialization.
  //  1.  If the RTC_PWR_STS bit is set which indicates a new coin-cell battery insertion or a battery failure,
  //        steps 2 through 5 should be executed.
  //  2.  Set RTC Register 0x0A[6:4] to '110' or '111'.
  //  3.  Set RTC Register 0x0B[7].
  //  4.  Set RTC Register 0x0A[6:4] to '010'.
  //  5.  Clear RTC Register 0x0B[7].
  //
  DataUint16        = MmioRead16 (PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1);
  RtcUipIsAlwaysSet = IsRtcUipAlwaysSet (PeiServices);
  if ((DataUint16 & B_PMC_GEN_PMCON_RTC_PWR_STS) || (RtcUipIsAlwaysSet)) {
    //
    // Execute the sequence below. This will ensure that the RTC state machine has been initialized.
    //
    // Step 1.
    // BIOS clears this bit by writing a '0' to it.
    //
    if (DataUint16 & B_PMC_GEN_PMCON_RTC_PWR_STS) {
      DataUint16 &= ~B_PMC_GEN_PMCON_RTC_PWR_STS;
      MmioWrite16 ((PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1), DataUint16);

      //
      // Set to invalid date to be set later to a valid time
      //
      IoWrite8 (R_RTC_INDEX2, R_RTC_YEAR);
      IoWrite8 (R_RTC_TARGET2, 0x0FF);
      IoWrite8 (R_RTC_INDEX2, R_RTC_MONTH);
      IoWrite8 (R_RTC_TARGET2, 0x0FF);
      IoWrite8 (R_RTC_INDEX2, R_RTC_DAYOFMONTH);
      IoWrite8 (R_RTC_TARGET2, 0x0FF);
      IoWrite8 (R_RTC_INDEX2, R_RTC_DAYOFWEEK);
      IoWrite8 (R_RTC_TARGET2, 0x0FF);
    }

    //
    // Step 2.
    // Set RTC Register 0Ah[6:4] to '110' or '111'.
    //
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERA);
    IoWrite8 (R_RTC_TARGET2, (V_RTC_REGISTERA_DV_DIV_RST1 | V_RTC_REGISTERA_RS_976P5US));

    //
    // Step 3.
    // Set RTC Register 0Bh[7].
    //
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERB);
    DataUint8 = (IoRead8 (R_RTC_TARGET2) | B_RTC_REGISTERB_SET);
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERB);
    IoWrite8 (R_RTC_TARGET2, DataUint8);

    //
    // Step 4.
    // Set RTC Register 0Ah[6:4] to '010'.
    //
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERA);
    IoWrite8 (R_RTC_TARGET2, (V_RTC_REGISTERA_DV_NORM_OP | V_RTC_REGISTERA_RS_976P5US));

    //
    // Step 5.
    // Clear RTC Register 0Bh[7].
    //
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERB);
    DataUint8 = (IoRead8 (R_RTC_TARGET2) & (UINT8)~B_RTC_REGISTERB_SET);
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERB);
    IoWrite8 (R_RTC_TARGET2, DataUint8);
  }

  return EFI_SUCCESS;
}


