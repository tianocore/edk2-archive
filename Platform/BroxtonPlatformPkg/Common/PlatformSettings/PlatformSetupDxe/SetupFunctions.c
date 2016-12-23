/** @file
  Setup Function Driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformSetupDxe.h"

VOID
SwapEntries (
  IN  CHAR8     *Data
  )
{
  UINT16    Index;
  CHAR8     Temp8;

  Index = 0;
  while (Data[Index] != 0 && Data[Index+1] != 0) {
    Temp8 = Data[Index];
    Data[Index] = Data[Index+1];
    Data[Index+1] = Temp8;
    Index +=2;
  }

  return;
}


UINT32
ConvertBase10ToRaw (
  IN  EFI_EXP_BASE10_DATA            *Data
  )
{
  UINTN         Index;
  UINT32        RawData;

  RawData = Data->Value;
  for (Index = 0; Index < (UINTN) Data->Exponent; Index++) {
     RawData *= 10;
  }

  return  RawData;
}

UINT32
ConvertBase2ToRaw (
  IN  EFI_EXP_BASE2_DATA             *Data
  )
{
  UINTN         Index;
  UINT32        RawData;

  RawData = Data->Value;
  for (Index = 0; Index < (UINTN) Data->Exponent; Index++) {
     RawData <<= 1;
  }

  return  RawData;
}

