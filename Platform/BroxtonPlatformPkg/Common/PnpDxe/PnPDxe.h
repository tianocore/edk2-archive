/** @file
  Power and Performance header file.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PNP_DXE_H_
#define _PNP_DXE_H_

#include <IndustryStandard/Pci22.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include "SaAccess.h"
#include <Guid/SetupVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SideBandLib.h>
#include <Protocol/SetupMode.h>

#include "AunitBiosSettings.h"
#include "BunitBiosSettings.h"
#include "TunitBiosSettings.h"

#define PNP_POWER       1
#define PNP_PERF        2
#define PNP_POWER_PERF  3

#define VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT \
  AUNIT_VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT, \
  BUNIT_VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT, \
  TUNIT_VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT

#define VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT \
  AUNIT_VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT, \
  BUNIT_VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT, \
  TUNIT_VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT

#define VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT \
  AUNIT_VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT, \
  BUNIT_VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT, \
  TUNIT_VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT

typedef struct {
  UINT8   MsgPort;
  UINT16  MsgRegAddr;
  UINT8   MSB;
  UINT8   LSB;
  UINT32  Value;
} PNP_SETTING;

#endif

