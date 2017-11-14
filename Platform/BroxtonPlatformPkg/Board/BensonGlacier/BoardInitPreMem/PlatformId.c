/** @file
  Implement Platform ID code.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/I2CLib.h>
#include <Library/GpioLib.h>
#include <Guid/PlatformInfo.h>
#include "PlatformId.h"

EFI_STATUS
EFIAPI
BensonGetBoardId(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId
  )
{
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD1   padConfg1;
  UINT32          CommAndOffset;

  DEBUG ((DEBUG_INFO, "GetBoardId.\n"));

  //
  // Board_ID0: PMIC_STDBY
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00F0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;         // Set to GPIO mode
  padConfg0.r.GPIORxTxDis = 0x1; // Set to GPI
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
  //
  // Set to Pull Up 20K
  //
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Board_ID1: PMIC_SDWN_B
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00D0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //
  // Board_ID2: PMIC_RESET_B
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00C8);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);

  //
  // Board_ID3: GP_CAMERASB10
  //

  CommAndOffset = GetCommOffset (NORTH, 0x01E0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);

  padConfg0.r.PMode = M0; // Set to GPIO mode
  padConfg0.r.GPIORxTxDis = GPI;  // Set to GPI
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);

  padConfg1.r.IOSTerm  = EnPu;    // Enable pull-up
  padConfg1.r.Term     = P_20K_H; // Set to 20K pull-up    
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);

  //
  // Read out Board_ID 
  //
  *BoardId = (UINT8) (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00F0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                     (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00D0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1) | \
                     (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00C8) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 2) | \
                     (((GpioPadRead (GetCommOffset (NORTH, 0x01E0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 3));

  DEBUG ((DEBUG_INFO,  "BoardId: %02X\n", *BoardId));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BensonGetFabId(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *FabId
  )
{
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD1   padConfg1;
  UINT32           CommAndOffset;

  DEBUG ((DEBUG_INFO, "Benson GetFabId++\n"));


  //
  // FAB_ID: GPIO_30
  //

  CommAndOffset = GetCommOffset (NORTH, 0x0F0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);

  padConfg0.r.PMode = M0; // Set to GPIO mode
  padConfg0.r.GPIORxTxDis = GPI;  // Set to GPI
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);

  padConfg1.r.IOSTerm  = EnPd;    // Enable pull-down
  padConfg1.r.Term     = P_20K_L; // Set to 20K pull-down    
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);


  *FabId = (UINT8) (((GpioPadRead (GetCommOffset (NORTH, 0x0F0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1));

  DEBUG ((EFI_D_INFO,  "FabId: %02X\n", *FabId));

  return EFI_SUCCESS;
}

