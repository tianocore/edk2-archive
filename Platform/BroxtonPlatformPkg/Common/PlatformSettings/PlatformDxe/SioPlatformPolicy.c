/** @file
  Sio Platform Policy Setting.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include <Protocol/LpcWpc83627Policy.h>


EFI_WPC83627_POLICY_PROTOCOL  mSio83627PolicyData = {
  { EFI_WPC83627_COM1_ENABLE,       // Com1
    EFI_WPC83627_LPT1_ENABLE,       // Lpt1
    EFI_WPC83627_FDD_DISABLE,       // Floppy
    EFI_WPC83627_FDD_WRITE_ENABLE,  // FloppyWriteProtect
    EFI_WPC83627_RESERVED_DEFAULT,  // Port80
    EFI_WPC83627_ECIR_DISABLE,      // CIR
    EFI_WPC83627_PS2_KBC_ENABLE,    // Ps2Keyboard
    EFI_WPC83627_RESERVED_DEFAULT,  // Ps2Mouse
    EFI_WPC83627_COM2_ENABLE,       // Com2
    EFI_WPC83627_COM3_ENABLE,       // Com3
    EFI_WPC83627_COM4_ENABLE,       // Com4
    EFI_WPC83627_RESERVED_DEFAULT,  // Dac
    0x00                            // Rsvd
    },
  LptModeEcp,                       // LptMode
};


/**
  Publish the platform SIO policy setting.

  @retval    EFI_SUCCESS

**/
VOID
InitSioPlatformPolicy(
  )
{
  EFI_HANDLE              Handle;
  EFI_STATUS              Status;

  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiLpcWpc83627PolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSio83627PolicyData
                  );

  ASSERT_EFI_ERROR (Status);
}

