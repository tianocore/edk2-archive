/** @file
  Header file for Gpio setting for multiplatform before Memory init.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BOARDGPIOS_PREMEM_H_
#define _BOARDGPIOS_PREMEM_H_

//
// Function Prototypes Only - Do not add #includes here
//
EFI_STATUS
MultiPlatformGpioProgramPreMem (
  IN OUT UINT64   *StartTimerTick
  );

EFI_STATUS
MultiPlatformGpioUpdatePreMem (
  VOID
  );

#endif

