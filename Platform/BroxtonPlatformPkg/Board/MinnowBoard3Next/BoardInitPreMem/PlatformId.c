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

PAD_ID_INFO gRawBoardIdPadInfo[] = {
  {NW_PMIC_STDBY,   EnPd, P_20K_L},
  {NW_GPIO_213,     EnPd, P_20K_L},
  {NW_PMIC_RESET_B, EnPd, P_20K_L},
  {NW_PMIC_PWRGOOD, EnPd, P_20K_L},
  {N_GPIO_27,       EnPd, P_20K_L},
  {N_GPIO_72,       EnPd, P_20K_L},
  {N_GPIO_64,       EnPd, P_20K_L}
};

//
// MinnowBoardv3 = 0x00000017
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PU -> 1
// NW_GPIO_213     - BOARD_ID1 - 10k PU -> 1
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1
// NW_PMIC_PWRGOOD -           - 10k PD -> 0
// N_GPIO_27       - BOARD_ID3 - 10k PU -> 1
// N_GPIO_72       -           - Float  -> 0
// N_GPIO_64       -           - Float  -> 0
//===========================================

// Benson Glacier = 0x00000024
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PD -> 0
// NW_GPIO_213     - BOARD_ID1 - 10k PD -> 0
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1
// NW_PMIC_PWRGOOD -           - Float  -> 0
// N_GPIO_27       -           - Float  -> 0
// N_GPIO_72       - BOARD_ID3 - 10k PU -> 1
// N_GPIO_64       -           - Float  -> 0
//===========================================

// MinnowBoardv3Next = 0x00000040
//===========================================
// NW_PMIC_STDBY   -           - Float  -> 0
// NW_GPIO_213     -           - Float  -> 0
// NW_PMIC_RESET_B -           - Float  -> 0
// NW_PMIC_PWRGOOD -           - Float  -> 0
// N_GPIO_27       -           - Float  -> 0
// N_GPIO_72       -           - Float  -> 0
// N_GPIO_64       -           - 10k PU -> 1
//===========================================

// LeafHill = 0x00000047
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PU -> 1
// NW_GPIO_213     - BOARD_ID1 - 10k PU -> 1
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1
// NW_PMIC_PWRGOOD - BOARD_ID3 - 10k PD -> 0
// N_GPIO_27       -           - Float  -> 0
// N_GPIO_72       -           - Float  -> 0
// N_GPIO_64       -           - 0k PU  -> 1
//===========================================

BOARD_ID_INFO gBoardIdInfo[] = {
  {0x00000017, BOARD_ID_MINNOW},      // MinnowBoardv3
  {0x00000024, BOARD_ID_BENSON},      // Benson Glacier
  {0x00000040, BOARD_ID_MINNOW_NEXT}, // MinnowBoardv3Next
  {0x00000047, BOARD_ID_LFH_CRB}      // LeafHill
};

PAD_ID_INFO gMb3nHwconfPadInfo[] = {
  {W_GPIO_128, DisPuPd, P_NONE}, // HWCONF0
  {W_GPIO_131, DisPuPd, P_NONE}, // HWCONF1
  {W_GPIO_130, DisPuPd, P_NONE}, // HWCONF2
  {W_GPIO_129, DisPuPd, P_NONE}, // HWCONF3
  {W_GPIO_139, DisPuPd, P_NONE}, // HWCONF4
  {W_GPIO_138, DisPuPd, P_NONE}, // HWCONF5
  {NW_GPIO_80, DisPuPd, P_NONE}, // HWCONF6
  {NW_GPIO_81, DisPuPd, P_NONE}, // HWCONF7
  {NW_GPIO_83, DisPuPd, P_NONE}  // HWCONF8
};

UINT32
EFIAPI
GetId (
  IN   PAD_ID_INFO   *PadInfoPtr,
  IN   UINT8          NumberOfEntries
  )
{
  UINT8           bit;
  UINT32          CommAndOffset;
  UINT8           index;
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD0   padConfg0Org;
  BXT_CONF_PAD1   padConfg1;
  BXT_CONF_PAD1   padConfg1Org;
  UINT32          ReturnId;

  //
  // Get ID from PAD
  //
  if ((PadInfoPtr == NULL) || (NumberOfEntries == 0)) {
    //
    // Nothing in structure. Skip.
    //
    ReturnId = 0xFF;
  } else {
    ReturnId = 0;
    for (index = 0; index < NumberOfEntries; index++) {
      CommAndOffset           = PadInfoPtr[index].CommAndOffset;
      padConfg0Org.padCnf0    = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
      padConfg1Org.padCnf1    = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
      //
      // Set pad to be able to read the bit
      //
      padConfg0.padCnf0       = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
      padConfg1.padCnf1       = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
      padConfg0.r.PMode       = M0;                        // Set to GPIO mode
      padConfg0.r.GPIORxTxDis = GPI;                       // Set to GPI
      padConfg1.r.IOSTerm     = PadInfoPtr[index].IOSTerm; // Enable pull-up/down
      padConfg1.r.Term        = PadInfoPtr[index].Term;    // Set pull-up/down value
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
      //
      // Read the bit
      //
      bit       = (UINT8) (((GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << index);
      ReturnId |= bit;
      //
      // Restore orginal pad programming.
      //
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0Org.padCnf0);
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1Org.padCnf1);
    }
  }
  return ReturnId;
}

UINT8
EFIAPI
GetCommonBoardId (
  VOID
  )
{
  UINT8    BoardId;
  UINT8    index;
  UINT32   RawBoardId;
  
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Get BoardId
  //
  RawBoardId = GetId (gRawBoardIdPadInfo, sizeof (gRawBoardIdPadInfo) / sizeof (gRawBoardIdPadInfo[0]));

  //
  // Convert from a 32-bit raw BoardId to an 8-bit one.
  //
  BoardId = BOARD_ID_APL_UNKNOWN;
  for (index = 0; index < sizeof (gBoardIdInfo) / sizeof (gBoardIdInfo[0]); index++) {
    if (gBoardIdInfo[index].RawId == RawBoardId) {
      BoardId = gBoardIdInfo[index].BoardId;
      break;
    }
  }
  
  DEBUG ((DEBUG_INFO, "%a(#%3d) - BoardId: %02X\n", __FUNCTION__, __LINE__, BoardId));
  return BoardId;
}



EFI_STATUS
EFIAPI
Minnow3NextGetEmbeddedBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  )
{
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Get BoardId
  //
  *BoardId = GetCommonBoardId ();

  if (*BoardId != BOARD_ID_MINNOW_NEXT) {
    *BoardId = BOARD_ID_APL_UNKNOWN;
    *FabId   = UNKNOWN_FAB;
  } else {
    //
    // Get FabId
    //
    *FabId = FAB_ID_A; // MBv3N FabID is behind the EC. Just say Fab A for now.
    DEBUG ((DEBUG_INFO, "%a(#%3d) - FabId  : %02X\n", __FUNCTION__, __LINE__, *FabId));
  }

  return EFI_SUCCESS;
}

UINT32
EFIAPI
Minnow3NextGetHwconfStraps (
  VOID
  )
{
  UINT32   HwconfStraps;
  
  //
  // Get HWCONF straps
  //
  HwconfStraps = GetId (gMb3nHwconfPadInfo, sizeof (gMb3nHwconfPadInfo) / sizeof (gMb3nHwconfPadInfo[0]));

  return HwconfStraps;
}

