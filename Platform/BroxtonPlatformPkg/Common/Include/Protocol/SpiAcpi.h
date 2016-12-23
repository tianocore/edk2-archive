/** @file
  SPI ACPI Protocol.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_ACPI_H__
#define __SPI_ACPI_H__

#include <Library/SpiTargetSettings.h>

///
///  EFI SPI Device
///
typedef struct {
  UINT8 Index;
  UART_DEVICE_PATH     *DevicePath;
  SPI_TARGET_SETTINGS  *targetSettings;
} EFI_SPI_DEVICE;

///
///  SPI PLATFORM CONTEXT
///
typedef struct {
  UINTN BaseAddress;
  UINT64 InputFrequencyHertz;
} SPI_PLATFORM_CONTEXT;

extern EFI_GUID gEfiSpiAcpiProtocolGuid;

#endif  //  __SPI_ACPI_H__

