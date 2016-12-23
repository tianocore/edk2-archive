/** @file
  GPIO setting for CherryView.
  This file includes package header files, library classes.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOARDINIT_H_
#define _BOARDINIT_H_

#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/Timerlib.h>
#include <Guid/PlatformInfo_Aplk.h>

VOID GpioTest (VOID);

#endif

