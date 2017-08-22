/** @file
  This file does TypeC initialization.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "TypeC.h"

static MUX_PROGRAMMING_TABLE mBgMuxTable[] = {
  // Address   Register                Data             String
  //====================================================================================
   {A_GENERAL, R_FIRMWARE_VERSION,     MUX_TABLE_NULL, "Firmware Version Number"},
   {A_STATUS,  R_CC_STATUS_1,          MUX_TABLE_NULL, "CC_Status_1"},
   {A_STATUS,  R_CC_STATUS_2,          MUX_TABLE_NULL, "CC_Status_2"},
   {A_STATUS,  R_CC_STATUS_3,          MUX_TABLE_NULL, "CC_Status_3"},
   {A_STATUS,  R_MUX_HPD_ASSERT,       MUX_TABLE_NULL, "MUX_In_HPD_Assertion"},
   {A_STATUS,  R_MUX_STATUS,           MUX_TABLE_NULL, "MUX Status"},
   {A_STATUS,  R_MUX_DP_TRAINING,      MUX_TABLE_NULL, "MUX_DP_Training_Disable"},
   {A_STATUS,  R_MUX_DP_AUX_INTERCEPT, MUX_TABLE_NULL, "MUX_DP_AUX_Interception_Disable"},
   {A_STATUS,  R_MUX_DP_EQ_CONFIG,     MUX_TABLE_NULL, "MUX_DP_EQ_Configuration"},
   {A_STATUS,  R_MUX_DP_OUTPUT_CONFIG, MUX_TABLE_NULL, "MUX_DP_Output_Configuration"}
};

VOID
PrintChar (
  IN UINTN     DebugMask,
  IN UINTN     Count,
  IN CHAR16   *Char
)
{
  UINTN Index;

  for (Index = 0; Index < Count; Index++) {
    DEBUG ((DebugMask, "%s", Char));
  }
}

#define DIVIDING_LINE "+----------------------------------------------------+------------------+\n"

VOID
DumpParagraph (
  IN   UINTN   DebugMask,
  IN   VOID   *Ptr,
  IN   UINTN   Count
  )
{
  CHAR8     AsciiBuffer[17];
  UINT8    *Data;
  UINTN     Index;
  UINTN     Paragraphs;
  UINTN     PlaceHolder;
  UINTN     PlaceHolders;

  //
  // Use a different pointer so that the one passed in doesn't change
  //
  Data = (UINT8 *) Ptr;
  //
  // Calcualte the number of paragraphs
  //
  Paragraphs = Count / 16;
  if ((Paragraphs * 16) < Count) {
    Paragraphs++;
  }
  //
  // Calculate the number of columns
  //
  PlaceHolder  = Paragraphs;
  PlaceHolders = 0;
  while (PlaceHolder > 0) {
    PlaceHolders++;
    PlaceHolder >>= 4;
  }

  //
  // Dump the buffer
  //
  if (Count > 0 ) {
    //
    // Print header
    //
    PrintChar (DebugMask, PlaceHolders + 5, L" ");
    DEBUG ((DebugMask, DIVIDING_LINE));
    PrintChar (DebugMask, PlaceHolders + 5, L" ");
    DEBUG ((DebugMask, "| x0 x1 x2 x3  x4 x5 x6 x7  x8 x9 xA xB  xC xD xE xF |      String      |\n"));
    DEBUG ((DebugMask, " +"));
    PrintChar (DebugMask, PlaceHolders + 3, L"-");
    DEBUG ((DebugMask, DIVIDING_LINE));
    //
    // Print data
    //
    for (Index = 0; Index < (Paragraphs * 16); Index++) {
      //
      // Print divider
      //
      if (Index % 0x10 == 0x00) {
        if ((Index > 0) && ((Index / 0x10) % 0x04 == 0x00) && (Paragraphs > 6)) {
          DEBUG ((DebugMask, " +"));
          PrintChar (DebugMask, PlaceHolders + 3, L"-");
          DEBUG ((DebugMask, DIVIDING_LINE));
        }
        DEBUG ((DebugMask, " | %0*xx | ", PlaceHolders, (Index / 0x10)));
      }
      //
      // Print the data or a filler
      //
      if (Index < Count) {
        DEBUG ((DebugMask, "%02x ", Data[Index]));
        if ((Data[Index] < 32) || (Data[Index] > 126)) {
          //
          // Not printable
          //
          AsciiBuffer[(Index % 0x10)] = '.';
        } else {
          //
          // Printable
          //
          AsciiBuffer[(Index % 0x10)] = Data[Index];
        }
      } else {
        DEBUG ((DebugMask, "   "));
        AsciiBuffer[(Index % 0x10)] = ' ';
      }
      //
      // Print break or line end if needed
      //
      if (Index % 0x10 == 0x0F) {
        AsciiBuffer[16] = 0x00;
        DEBUG ((DebugMask, "| %a |\n", AsciiBuffer));
      } else if (Index % 0x04 == 0x03) {
        DEBUG ((DebugMask, " "));
      }
    }
    //
    // Print footer
    //
    DEBUG ((DebugMask, " +"));
    PrintChar (DebugMask, PlaceHolders + 3, L"-");
    DEBUG ((DebugMask, DIVIDING_LINE));
  }
}

EFI_STATUS
EFIAPI
ReadMux (
  IN   UINT8    SlaveAddress,
  IN   UINT8    Offset,
  OUT  UINT8   *Data
  )
{
  UINT8          RetryCount;
  EFI_STATUS     Status;

  RetryCount = MUX_RETRY_COUNT;
  do {
    *Data = 0x00;
    Status = ByteReadI2C (PARADE_MUX_I2C_BUS, SlaveAddress, Offset, 1, Data);
  } while ((RetryCount-- > 0) && (EFI_ERROR (Status)));

  return Status;
}

EFI_STATUS
EFIAPI
WriteMux (
  IN   UINT8    SlaveAddress,
  IN   UINT8    Offset,
  OUT  UINT8   *Data
  )
{
  UINT8          RetryCount;
  EFI_STATUS     Status;

  RetryCount = MUX_RETRY_COUNT;
  do {
    Status = ByteWriteI2C (PARADE_MUX_I2C_BUS, SlaveAddress, Offset, 1, Data);
  } while ((RetryCount-- > 0) && (EFI_ERROR (Status)));

  return Status;
}

VOID
DumpMux (
  VOID
  )
{
  UINT8          Data[256];
  UINT16         Offset;
  BXT_CONF_PAD0  padConfg0;
  BXT_CONF_PAD1  padConfg1;
  UINT8          SlaveAddress;
  EFI_STATUS     Status;

  //
  // Loop thru device and dump it all
  //
  DEBUG ((DEBUG_INFO, "\n%a(#%d) - Dump the PS8750 I2C data\n", __FUNCTION__, __LINE__));
  for (SlaveAddress = 0x08; SlaveAddress <= 0x0E; SlaveAddress++) {
    for (Offset = 0x00; Offset <= 0xFF; Offset++) {
      Status = ReadMux (SlaveAddress, (UINT8) Offset, &Data[Offset]);
      if (EFI_ERROR (Status)) Data[Offset] = 0xFF;
    }
    DEBUG ((DEBUG_INFO, "\nSlaveAddress = 0x%02x\n", (SlaveAddress << 1)));
    DumpParagraph (DEBUG_INFO, Data, 256);
  }
  DEBUG ((DEBUG_INFO, "\n"));
  padConfg0.padCnf0 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg1.padCnf1 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF1_OFFSET);
  DEBUG ((DEBUG_INFO, "%a(#%d) - GPIO_199 (DDI1 HPD) Rx = %d  RxInv = %d\n\n", __FUNCTION__, __LINE__, padConfg0.r.GPIORxState, padConfg0.r.RXINV));
}

EFI_STATUS
EFIAPI
SetupTypecMuxAux (
  VOID
  )
{
  UINT8            Data8;
  UINTN            index;
  MUX_DATA_TABLE   MuxData;
  BXT_CONF_PAD0    padConfg0;
  BXT_CONF_PAD1    padConfg1;
  UINT8            *Ptr;
  EFI_STATUS       Status;

  //
  // Read/write MUX info
  //
  Ptr = (UINT8 *) &MuxData;
  for (index = 0; index < (sizeof (mBgMuxTable) / sizeof (mBgMuxTable[0])); index++) {
    Status = ReadMux (mBgMuxTable[index].Address, mBgMuxTable[index].Register, &Data8);
    DEBUG ((DEBUG_INFO, "%a(#%d) - %.*a [0x%02x:0x%02x] = 0x%02x (%r)\n", __FUNCTION__, __LINE__, MUX_TABLE_STRING_LENGTH, mBgMuxTable[index].String, (mBgMuxTable[index].Address << 1), mBgMuxTable[index].Register, Data8, Status));
    Ptr[index] = Data8;
    if ((mBgMuxTable[index].Data != MUX_TABLE_NULL) && (!EFI_ERROR (Status))) {
      Data8 = (UINT8) (mBgMuxTable[index].Data & 0x00FF);
      Status = WriteMux (mBgMuxTable[index].Address, mBgMuxTable[index].Register, &Data8);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a(#%d) - ERROR: ByteWriteI2C returned %r for %a = 0x%02x\n", __FUNCTION__, __LINE__, Status, mBgMuxTable[index].String, Data8));
      } else {
        Status = ReadMux (mBgMuxTable[index].Address, mBgMuxTable[index].Register, &Data8);
        DEBUG ((DEBUG_INFO, "%a(#%d) - %.*a [0x%02x:0x%02x] = 0x%02x (%r)\n", __FUNCTION__, __LINE__, MUX_TABLE_STRING_LENGTH, mBgMuxTable[index].String, (mBgMuxTable[index].Address << 1), mBgMuxTable[index].Register, Data8, Status));
        Ptr[index] = Data8;
      }
    }
  }

  //
  // Display HPD
  //
  padConfg0.padCnf0 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg1.padCnf1 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF1_OFFSET);
  DEBUG ((DEBUG_INFO, "%a(#%d) - GPIO_199 (DDI1 HPD) Rx = %d  RxInv = %d\n", __FUNCTION__, __LINE__, padConfg0.r.GPIORxState, padConfg0.r.RXINV));

  //
  // See if we need to assert the HPD on the MUX
  //
  if ((MuxData.MuxStatus & BIT7) == BIT7) {
    //
    // We are in DP mode
    //
    if ((MuxData.HpdAssert & BIT7) != BIT7) {
      //
      // We need to assert the MUX HPD
      //
      Data8  = MuxData.HpdAssert | BIT7;
      Status = WriteMux (A_STATUS,  R_MUX_HPD_ASSERT, &Data8);

      //
      // Display HPD
      //
      padConfg0.padCnf0 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF0_OFFSET);
      padConfg1.padCnf1 = GpioPadRead (NW_GPIO_199 + BXT_GPIO_PAD_CONF1_OFFSET);
      DEBUG ((DEBUG_INFO, "%a(#%d) - GPIO_199 (DDI1 HPD) Rx = %d  RxInv = %d\n", __FUNCTION__, __LINE__, padConfg0.r.GPIORxState, padConfg0.r.RXINV));
    }
  }

  return EFI_SUCCESS;
}

