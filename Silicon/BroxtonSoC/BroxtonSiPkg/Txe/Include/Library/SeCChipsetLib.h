/** @file
  Header file for SEC Chipset Lib.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_CHIPSET_LIB_H_
#define _SEC_CHIPSET_LIB_H_

typedef enum {
  HECI1 = 0,
  HECI2,
  HECI3,
  FTPM = 7
} SEC_DEVICE;

typedef enum {
  Disabled = 0,
  Enabled
} SEC_DEVICE_FUNC_CTRL;


/**
  Enable/Disable SEC devices

  @param[in]  WhichDevice       Select of SEC device
  @param[in]  DeviceFuncCtrl    Function control

  @retval     None
**/
VOID
SeCDeviceControl (
  IN  SEC_DEVICE                   WhichDevice,
  IN  SEC_DEVICE_FUNC_CTRL         DeviceFuncCtrl
  );

#endif

