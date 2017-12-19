/** @file
  Get Soc ID library implementation.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _STEPPING_LIB_H_
#define _STEPPING_LIB_H_

///
/// BXT Series
///
typedef enum {
  Bxt          = 0x00,
  Bxt1,
  BxtX,
  BxtP,
  BxtSeriesMax = 0xFF
} BXT_SERIES;

///
/// Silicon Steppings
///
typedef enum {
  BxtA0          = 0x00,
  BxtA1,
  BxtB0          = 0x10,
  BxtB1,
  BxtB2,
  BxtC0,
  BxtXA0         = 0x20,
  BxtPA0         = 0x40,
  BxtPB0         = 0x50,
  BxtPB1,
  BxtPB2,
  BxtPE0         = 0x60,
  BxtSteppingMax = 0xFF
} BXT_STEPPING;

///
/// BXT E0 FIB Types
///
typedef enum {
  BxtHardFib  = 0x00,
  BxtSoftFib  = 0x02,
  BxtNoFib    = 0xFF
} BXT_FIB_TYPE;

/**
  Return SOC series type

  @retval  BXT_SERIES          SOC series type

**/
BXT_SERIES
EFIAPI
GetBxtSeries (
  VOID
  );

/**
  This procedure will get Soc Stepping

  @retval  Soc Stepping

**/

BXT_STEPPING
EFIAPI
BxtStepping (
  VOID
  );

BXT_FIB_TYPE
EFIAPI
GetFibType (
  VOID
  );

#endif // _STEPPING_LIB_H_

