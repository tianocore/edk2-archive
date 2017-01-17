/** @file
  Dxe library for accessing dummy PMIC registers.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PmicLib.h>
#include "PmicPrivate.h"

/**
  Reads an 8-bit PMIC register.

  Reads the 8-bit PMIC register specified by Register.
  The 8-bit read value is returned.

  @param[in]  BaseAddress     - IPC operation address for target PMIC device.
  @param[in]  Register        - The PMIC register to read.

  @retval 0                   - Function not supported yet.
**/
UINT8
EFIAPI
DmPmicRead8 (
  IN UINT8                     BaseAddress,
  IN UINT8                     Register
  )
{
  return 0x0;
}

/**
  Writes an 8-bit PMIC register with a 8-bit value.

  Writes the 8-bit PMIC register specified by Register with the value specified
  by Value and return the operation status.

  @param[in]  BaseAddress     - IPC operation address for target PMIC device.
  @param[in]  Register        - The PMIC register to write.
  @param[in]  Value           - The value to write to the PMIC register.

  @retval EFI_SUCCESS         - Function not supported yet.
**/
EFI_STATUS
EFIAPI
DmPmicWrite8 (
  IN UINT8                     BaseAddress,
  IN UINT8                     Register,
  IN UINT8                     Value
  )
{
  return EFI_SUCCESS;
}

/**
  AC/DC Adapter Detection Status

  @retval TRUE                - Connected
**/
BOOLEAN
EFIAPI
DmPmicIsACOn (
  VOID
  )
{
  return TRUE;
}

/**
  Probe to find the correct PMIC object.

  After probling, g_pmic_obj points to correct PMIC object
  This routine is invoked when library is loaded .

  @retval TRUE                - Always return true to indicate dummy PMIC existed.
**/
BOOLEAN
EFIAPI
DmPmicProbe (
  VOID
  )
{
  DEBUG ((DEBUG_ERROR, "Dummy PMIC detected\n"));
  return TRUE;
}

struct PmicObject DmObj = {
  PMIC_I2C_BUSNO,
  PMIC_PAGE_0_I2C_ADDR,
  PMIC_PAGE_0_I2C_ADDR,
  PMIC_TYPE_DUMMY,
  DmPmicRead8,
  DmPmicWrite8,

  DmPmicProbe,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  
  //
  //PUPDR interfaces
  //
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  DmPmicIsACOn,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

