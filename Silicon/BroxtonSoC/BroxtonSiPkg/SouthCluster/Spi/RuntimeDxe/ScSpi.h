/** @file
  Header file for the SC SPI Runtime Driver.

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SPI_RT_H_
#define _SC_SPI_RT_H_

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <ScAccess.h>
#include <Protocol/Spi.h>
#include <Library/ScSpiCommonLib.h>
#include <IndustryStandard/Pci30.h>

//
// Function prototypes used by the SPI protocol.
//
/**
  Fixup internal data pointers so that the services can be called in virtual mode.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.

**/
VOID
EFIAPI
PchSpiVirtualAddressChangeEvent (
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  );

#endif

