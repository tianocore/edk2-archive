/** @file
  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CMOS_ACCESS_LIB_
#define _CMOS_ACCESS_LIB_

#include <Base.h>
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/CmosAccessLib.h>
#include <Library/BasePlatformCmosLib.h>

//
// CMOS access Port address
//
#define PORT_70  0x70
#define PORT_71  0x71
#define PORT_72  0x72
#define PORT_73  0x73

#define CMOS_LOW_MEM_ST   0
#define CMOS_LOW_MEM_END  127

#define CMOS_HIGH_MEM_ST  128
#define CMOS_HIGH_MEM_END 255

#define CMOS_START_ADDR   40
#define CMOS_END_ADDR     255

//
// Don't set this value more than 254 and equal to 127
//
#define CMOS_CHECKSUM_ADDR_LOW  254
#define CMOS_CHECKSUM_ADDR_HIGH (CMOS_CHECKSUM_ADDR_LOW + 1)

#endif // _CMOS_ACCESS_LIB_

