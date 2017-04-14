/** @file
  SMM S3 handler Driver implementation file.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmmPlatform.h"

extern  UINT16                          mAcpiBaseAddr;

EFI_STATUS
SaveRuntimeScriptTable (
  IN EFI_SMM_SYSTEM_TABLE2       *Smst
  )
{
  UINT16  Data16;
  UINT8   Data8;
  UINT32  DwordData;
  UINT32  PciOffset;

  IoWrite8 (0x80, 0x53);

  for (PciOffset = 0x10; PciOffset < 0x0cf; PciOffset += 4) {
    DwordData = MmioRead32 (MmPciAddress (0,  SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PciOffset));

    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      MmPciAddress (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PciOffset),
      1,
      &DwordData
      );
  }

  IoWrite8 (0x80, 0x53);

  //
  // Save I/O ports to S3 script table
  //

  //
  // Selftest KBC
  //
  Data8 = 0xAA;
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint8,
    0x64,
    (UINTN) 1,
    &Data8
    );

  //
  // Save B_ICH_TCO_CNT_LOCK so it will be done on S3 resume path.
  //
  Data16 = IoRead16 (mAcpiBaseAddr + R_TCO_CNT);
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint16,
    mAcpiBaseAddr + R_TCO_CNT,
    1,
    &Data16
    );

  return EFI_SUCCESS;
}

