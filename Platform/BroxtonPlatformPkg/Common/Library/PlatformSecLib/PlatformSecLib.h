/** @file
  Internal header file for SEC Platform hook library.

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef  _PLATFORM_SEC_LIB_H_
#define  _PLATFORM_SEC_LIB_H_

#include <PiPei.h>
#include <Ppi/SecPlatformInformation.h>
#include <Ppi/TemporaryRamSupport.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PerformanceLib.h>
#include <Library/LocalApicLib.h>
#include <Library/CmosAccessLib.h>

#define Flat32Start                  _ModuleEntryPoint


#define CAR_BASE_ADDR 0xFEF80000
#define CAR_SIZE  0x80000

#endif

