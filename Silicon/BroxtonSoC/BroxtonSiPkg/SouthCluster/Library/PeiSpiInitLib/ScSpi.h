/** @file
  Header file for the SC SPI PEI Library.

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SPI_H_
#define _SC_SPI_H_

#include <Ppi/Spi.h>
#include <ScAccess.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/ScSpiCommonLib.h>

typedef struct {
  EFI_PEI_PPI_DESCRIPTOR  PpiDescriptor;
  SPI_INSTANCE            SpiInstance;
} PEI_SPI_INSTANCE;

/**
  Installs SC SPI PPI

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
EFIAPI
InstallScSpi (
  VOID
  );
#endif
