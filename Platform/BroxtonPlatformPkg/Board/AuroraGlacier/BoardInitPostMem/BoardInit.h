/** @file
  GPIO setting for Aurora.
  This file includes package header files, library classes.

  Copyright (c) 2013 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _AURORA_BOARDINIT_H_
#define _AURORA_BOARDINIT_H_

#include <PiPei.h>

#include <Guid/PlatformInfo_Aplk.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/I2CLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>

#include <Ppi/BoardInitSignalling.h>

#include "BoardInitMiscs.h"
#include <ScRegs/RegsPcu.h>

VOID AuroraGpioTest (VOID);

#endif

