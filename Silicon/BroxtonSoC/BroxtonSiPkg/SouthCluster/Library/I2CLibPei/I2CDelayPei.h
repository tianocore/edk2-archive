/** @file
  imer prototype for I2C Pei Library.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_DELAY_PEI_
#define _I2C_DELAY_PEI_

#include <PiPei.h>

/**
  Stalls the CPU for at least the given number of microseconds.

  @param[in]  MicroSeconds     The minimum number of microseconds to delay.

  @retval  EFI_SUCCESS         Time delay successfully
**/
EFI_STATUS
EFIAPI
MicroSecondDelay (
  IN UINTN                     MicroSeconds
  );

#endif
