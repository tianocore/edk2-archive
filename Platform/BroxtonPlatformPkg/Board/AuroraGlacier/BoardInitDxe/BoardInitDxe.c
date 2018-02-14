/** @file
  Board specific functions in DXE phase to be set as dynamic PCD and consumed by
  commmon platform code.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitDxe.h"
#include <Protocol/SmbusHc.h>

GET_BOARD_NAME mAuroraGetBoardNamePtr = AuroraGetBoardName;

CHAR16*
EFIAPI
AuroraGetBoardName (
  IN  UINT8                   BoardId
  )
{
  STATIC CHAR16  BoardName[40];

  DEBUG ((EFI_D_INFO,  "BoardInitDxe: GetBoardName - Aurora Glacier\n"));

  UnicodeSPrint (BoardName, sizeof (BoardName), L"Aurora Glacier ");

  if (BoardId != (UINT8) BOARD_ID_AURORA) {
    return NULL;
  } else {
    return BoardName;
  }
}


VOID
EFIAPI
AuroraProgramPmicPowerSequence (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  EFI_STATUS                Status;
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_SMBUS_DEVICE_COMMAND  Command;
  UINTN                     Length;
  UINT8                     BufferData[1];
  EFI_SMBUS_HC_PROTOCOL     *SmbusControllerProtocol;
  
  //
  // Programe IDTP9810 PMIC.
  //
  
  DEBUG ((EFI_D_INFO, "Programe PMIC. \n"));
  
  //
  // Locate SMBus protocol
  //
  Status  = gBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID **)&SmbusControllerProtocol);
  ASSERT_EFI_ERROR(Status);
  
  SlaveAddress.SmbusDeviceAddress = (0xBC >> 1); // 0x5E
  Command = 0x00; // Offset
  Length  = 1;
  
  //
  // Read one byte
  //
  Status = SmbusControllerProtocol->Execute ( 
                                      SmbusControllerProtocol,
                                      SlaveAddress,
                                      Command,
                                      EfiSmbusReadByte,
                                      FALSE,
                                      &Length,
                                      BufferData
                                      );
  
  
  DEBUG ((EFI_D_INFO, "PMIC Vendor ID = %0x. \n", (UINT32) BufferData[0]));
  

  SlaveAddress.SmbusDeviceAddress = (0xBC >> 1); // 0x5E
  Command = 0x2A; // Offset
  Length  = 1;
  
  //
  // Read one byte
  //
  Status = SmbusControllerProtocol->Execute ( 
                                      SmbusControllerProtocol,
                                      SlaveAddress,
                                      Command,
                                      EfiSmbusReadByte,
                                      FALSE,
                                      &Length,
                                      BufferData
                                      );
  
  
  DEBUG ((EFI_D_INFO, "PMIC Power Sequence Configuration  Offset 0x2A PWRSEQCFG = %0x. \n", (UINT32) BufferData[0])); 

  //
  // Set Bit 2 (SUSPWRDNACKCFG) of PWRSEQCFG.
  // 0 = SUSPWRDNACK signal is ignored. PMIC will not go to G3 when SUSPWRDNACK goes high in S4 state.
  // 1 = PMIC responses to SUSPWRDNACK signal.
  //
  //
  BufferData[0] = BufferData[0] | 0x04;
  Status = SmbusControllerProtocol->Execute ( 
                                      SmbusControllerProtocol,
                                      SlaveAddress,
                                      Command,
                                      EfiSmbusWriteByte,
                                      FALSE,
                                      &Length,
                                      BufferData
                                      );
 DEBUG ((EFI_D_INFO, "PMIC Power Sequence Configuration  Set Bit 2 (SUSPWRDNACKCFG) of PWRSEQCFG. \n")); 


  //
  // Read one byte
  //
  Status = SmbusControllerProtocol->Execute ( 
                                      SmbusControllerProtocol,
                                      SlaveAddress,
                                      Command,
                                      EfiSmbusReadByte,
                                      FALSE,
                                      &Length,
                                      BufferData
                                      );
  
  
  DEBUG ((EFI_D_INFO, "PMIC Power Sequence Configuration  Offset 0x2A PWRSEQCFG = %0x. \n", (UINT32) BufferData[0])); 
}



/**
  Set PCDs for board specific functions.

  @param[in]  ImageHandle   ImageHandle of the loaded driver.
  @param[in]  SystemTable   Pointer to the EFI System Table.

  @retval     EFI_SUCCESS   The handlers were registered successfully.

**/
EFI_STATUS
EFIAPI
AuroraBoardInitDxeConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8       BoardId;
  EFI_EVENT   ReadyToBootEvent;

  BoardId = PcdGet8 (PcdBoardId);
  if (BoardId != (UINT8) BOARD_ID_AURORA) {
    return EFI_SUCCESS;
  }

  PcdSet64 (PcdGetBoardNameFunc, (UINT64) mAuroraGetBoardNamePtr);

  EfiCreateEventReadyToBootEx (
    TPL_CALLBACK,
    AuroraProgramPmicPowerSequence,
    NULL,
    &ReadyToBootEvent
    );
  
  return EFI_SUCCESS;
}

