/** @file
  Header file for PchInfoLib.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_INFO_LIB_H_
#define _PCH_INFO_LIB_H_

#include <ScAccess.h>

typedef enum {
  PchH          = 1,
  PchLp,
  PchUnknownSeries
} PCH_SERIES;

/**
  Return Pch Series.

  @retval  PCH_SERIES                Pch Series

**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  );

/**
  Get Pch Maximum Pcie Root Port Number.

  @retval   PcieMaxRootPort         Pch Maximum Pcie Root Port Number

**/
UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  );

#endif // _PCH_INFO_LIB_H_

