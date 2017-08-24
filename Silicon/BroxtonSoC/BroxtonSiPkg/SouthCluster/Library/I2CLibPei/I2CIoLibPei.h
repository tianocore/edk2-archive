/** @file
  IO prototype for I2C Pei Library.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_IOLIB_PEI_
#define _I2C_IOLIB_PEI_

#include <PiPei.h>

/**
  Reads an 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address. The 8-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param[in] Address   - The MMIO register to read.

  @retval UINT8        - The UINT8 value read.
**/
UINT8
EFIAPI
I2CLibPeiMmioRead8 (
  IN      UINTN                     Address
  );


/**
  Reads a 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address   - The MMIO register to read.

  @retval UINT16       - The UINT16 value read.
**/
UINT16
EFIAPI
I2CLibPeiMmioRead16 (
  IN      UINTN                     Address
  );


/**
  Writes a 16-bit MMIO register.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in]  Address  - The MMIO register to write.
  @param[in]  Value    - The value to write to the MMIO register.

  @retval UINT16       - The UINT16 value written.
**/
UINT16
EFIAPI
I2CLibPeiMmioWrite16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  );


/**
  Reads a 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address   - The MMIO register to read.

  @retval UINT32       - The UINT32 value read.

**/
UINT32
EFIAPI
I2CLibPeiMmioRead32 (
  IN      UINTN                     Address
  );


/**
  Writes a 32-bit MMIO register.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in]  Address  - The MMIO register to write.
  @param[in]  Value    - The value to write to the MMIO register.

  @retval UINT32       - The UINT32 value written.
**/
UINT32
EFIAPI
I2CLibPeiMmioWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  );


/**
  Do logical OR operation with the value read from the 32-bit  MMIO register
  and write it back to 32-bit MMIO register.

  @param[in]  Address  - The MMIO register to write.
  @param[in]  OrData   - The value to do logical OR operation with the value read from the MMIO register.

  @retval UINT32       - The final value written to the MMIO register.
**/
UINT32
EFIAPI
I2CLibPeiMmioOr32 (
  IN      UINTN                     Address,
  IN      UINT32                    OrData
  );

#endif
