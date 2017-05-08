/** @file
  Header file for PCI Express helps library implementation.

  Copyright (c) 2008 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_PCI_EXPRESS_HELPERS_LIBRARY_H_
#define _PCH_PCI_EXPRESS_HELPERS_LIBRARY_H_

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci30.h>
#include <Ppi/ScPolicy.h>
#include <ScAccess.h>
#include <Library/ScPlatformLib.h>
#include <Library/MmPciLib.h>
#include <Library/SideBandLib.h>
#include <Library/SteppingLib.h>
#include <Private/Library/PeiDxeSmmScPciExpressHelpersLib.h>


#define DEVICE_ID_NOCARE    0xFFFF

#define LTR_VALUE_MASK (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7 + BIT8 + BIT9)
#define LTR_SCALE_MASK (BIT10 + BIT11 + BIT12)

#ifndef SLE_FLAG
  #define CONFIG_WRITE_LOOP_COUNT   100000
#else // SLE_FLAG
  #define CONFIG_WRITE_LOOP_COUNT   10
#endif // SLE_FLAG

//
// LTR related macros
//
#define LTR_LATENCY_VALUE(x)           ((x) & LTR_VALUE_MASK)
#define LTR_SCALE_VALUE(x)             (((x) & LTR_SCALE_MASK) >> 10)
#define LTR_LATENCY_NS(x)              (LTR_LATENCY_VALUE(x) * (1 << (5 * LTR_SCALE_VALUE(x))))

#endif

