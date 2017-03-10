/** @file
  Prototype of the PeiSpiInitLib library.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SPI_INIT_LIBRARY_H_
#define _PEI_SPI_INIT_LIBRARY_H_

#include <Ppi/ScPolicy.h>

/**
  Installs SC SPI PPI.

  @param[in]  None

  @retval EFI_SUCCESS             SC SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
EFIAPI
InstallScSpi (
  VOID
  );

#endif
