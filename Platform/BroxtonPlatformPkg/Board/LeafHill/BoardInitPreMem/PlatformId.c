/** @file
  Implement Platform ID code.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

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

//
// Board ID pin definiton
//
// Bit     SoC Pin            Value (Leaf Hill)  Value (Minnow Board 3)    Value (Benson Glacier)   Value (Aurora Glacier)
// 0    GPIO_224 (J45)         PU (1)              PU (1)                    PD  (0)                  PD  (0)
// 1    GPIO_213 (M47)         PU (1)              PU (1)                    PD  (0)                  PU  (1)
// 2    GPIO_223 (H48)         PU (1)              PU (1)                    PU  (1)                  PU  (1)
// 3    GP_CAMERASB10 (R34)    PD (0)              PU (1)                    PU  (1)                  PU  (1)
//

EFI_STATUS
EFIAPI
LeafHillGetEmbeddedBoardIdFabId(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  )
{
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD1   padConfg1;
  IN UINT32       CommAndOffset;

  DEBUG ((DEBUG_INFO, "GetEmbeddedBoardIdFabId++\n"));

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

  DEBUG ((DEBUG_INFO,  "BoardId from PMIC strap: %02X\n", *BoardId));

  //
  // Fab_ID0: PMIC_I2C_SDA
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x0108);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //
  // Set to Pull Up 20K
  //
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Fab_ID1: PMIC_I2C_SCL
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x0100);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //Set to Pull Up 20K
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Fab_ID2: PMIC_BCUDISW2
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00D8);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //
  // Set to Pull Up 20K
  //
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Fab_ID3: PMIC_BCUDISCRIT
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00E0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //
  // Set to Pull Up 20K
  //
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);

  *FabId = (UINT8) (((GpioPadRead (GetCommOffset (NORTHWEST, 0x0108) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                   (((GpioPadRead (GetCommOffset (NORTHWEST, 0x0100) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1) | \
                   (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00D8) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 2) | \
                   (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00E0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 3));

  DEBUG ((EFI_D_INFO,  "FabId from PMIC strap: %02X\n", *FabId));


  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LeafHillGetIVIBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  )
{
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD1   padConfg1;
  IN UINT32       CommAndOffset;

  DEBUG ((DEBUG_INFO, "GetIVIBoardIdFabId++\n"));

  //
  // Board_ID0: GPIO_62
  //
  CommAndOffset = GetCommOffset (NORTH, 0x0190);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
  padConfg1.r.IOSTerm = 0x3; //Enable Pullup
  padConfg1.r.Term = 0xC;    //20k wpu
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Board_ID1: GPIO_63
  //
  CommAndOffset = GetCommOffset (NORTH, 0x0198);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
  padConfg1.r.IOSTerm = 0x3; //Enable Pullup
  padConfg1.r.Term = 0xC;    //20k wpu
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Board_ID2: GPIO_64
  //
  CommAndOffset = GetCommOffset (NORTH, 0x01A0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
  padConfg1.r.IOSTerm = 0x3; //Enable Pullup
  padConfg1.r.Term = 0xC;    //20k wpu
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  //
  // Board_ID3: GPIO_65
  //
  CommAndOffset = GetCommOffset (NORTH, 0x01A8);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  padConfg1.padCnf1 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
  padConfg1.r.IOSTerm = 0x3; //Enable Pullup
  padConfg1.r.Term = 0xC;    //20k wpu
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);

  *BoardId = (UINT8) (((GpioPadRead (GetCommOffset (NORTH, 0x0190) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                     (((GpioPadRead (GetCommOffset (NORTH, 0x0198) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1) | \
                     (((GpioPadRead (GetCommOffset (NORTH, 0x01A0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 2) | \
                     (((GpioPadRead (GetCommOffset (NORTH, 0x01A8) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 3));

  return EFI_SUCCESS;
}

