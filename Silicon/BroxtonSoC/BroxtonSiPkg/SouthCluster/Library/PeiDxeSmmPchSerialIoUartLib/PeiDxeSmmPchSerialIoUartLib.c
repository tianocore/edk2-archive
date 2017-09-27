/** @file
  PCH Serial IO UART Lib implementation.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <ScAccess.h>
#include <Library/ScPcrLib.h>
#include <Library/ScSerialIoLib.h>


#define R_PCH_SERIAL_IO_8BIT_UART_RXBUF      0x00
#define R_PCH_SERIAL_IO_8BIT_UART_TXBUF      0x00
#define R_PCH_SERIAL_IO_8BIT_UART_BAUD_LOW   0x00
#define R_PCH_SERIAL_IO_8BIT_UART_BAUD_HIGH  0x01
#define R_PCH_SERIAL_IO_8BIT_UART_FCR        0x02
#define R_PCH_SERIAL_IO_8BIT_UART_IIR        0x02
#define R_PCH_SERIAL_IO_8BIT_UART_LCR        0x03
#define R_PCH_SERIAL_IO_8BIT_UART_MCR        0x04
#define R_PCH_SERIAL_IO_8BIT_UART_LSR        0x05
#define R_PCH_SERIAL_IO_8BIT_UART_USR        0x1F

#define R_PCH_SERIAL_IO_NATIVE_UART_RXBUF      0x00
#define R_PCH_SERIAL_IO_NATIVE_UART_TXBUF      0x00
#define R_PCH_SERIAL_IO_NATIVE_UART_BAUD_LOW   0x00
#define R_PCH_SERIAL_IO_NATIVE_UART_BAUD_HIGH  0x04
#define R_PCH_SERIAL_IO_NATIVE_UART_FCR        0x08
#define R_PCH_SERIAL_IO_NATIVE_UART_IIR        0x08
#define R_PCH_SERIAL_IO_NATIVE_UART_LCR        0x0C
#define R_PCH_SERIAL_IO_NATIVE_UART_MCR        0x10
#define R_PCH_SERIAL_IO_NATIVE_UART_LSR        0x14
#define R_PCH_SERIAL_IO_NATIVE_UART_USR        0x7C

#define B_PCH_SERIAL_IO_UART_IIR_FIFOSE   BIT7|BIT6
#define B_PCH_SERIAL_IO_UART_LSR_TXRDY    BIT5
#define B_PCH_SERIAL_IO_UART_LSR_RXDA     BIT0
#define B_PCH_SERIAL_IO_UART_LCR_DLAB     BIT7
#define B_PCH_SERIAL_IO_UART_FCR_FCR      BIT0
#define B_PCH_SERIAL_IO_UART_MCR_RTS      BIT1
#define B_PCH_SERIAL_IO_UART_MCR_AFCE     BIT5
#define B_PCH_SERIAL_IO_UART_USR_TFNF     BIT1

/**
  Initialize selected SerialIo UART.

  @param[in]  UartNumber           Selects Serial IO UART device (0-2)
  @param[in]  FifoEnable           When TRUE, enables 64-byte FIFOs.
  @param[in]  BaudRate             Baud rate.
  @param[in]  LineControl          Data length, parity, stop bits.
  @param[in]  HardwareFlowControl  Automated hardware flow control. If TRUE, hardware automatically checks CTS when sending data, and sets RTS when receiving data.

**/
VOID
SerialIo16550Init (
  IN UINTN      Base,
  IN BOOLEAN    FifoEnable,
  IN UINT32     BaudRate,
  IN UINT8      LineControl,
  IN BOOLEAN    HardwareFlowControl,
  IN UINT8      ShiftOffset
  )
{
  UINTN          Divisor;

  Divisor = MAX_BAUD_RATE / BaudRate;
  //
  // Configure baud rate
  //
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_LCR >> ShiftOffset), B_PCH_SERIAL_IO_UART_LCR_DLAB);
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_BAUD_HIGH >> ShiftOffset), (UINT8) (Divisor >> 8));
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_BAUD_LOW >> ShiftOffset), (UINT8) (Divisor & 0xff));
  //
  //  Configure Line control and switch back to bank 0
  //
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_LCR >> ShiftOffset), LineControl & 0x1F);
  //
  // Enable and reset FIFOs
  //
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_FCR >> ShiftOffset), FifoEnable ? B_PCH_SERIAL_IO_UART_FCR_FCR : 0);
  //
  // Put Modem Control Register(MCR) into its reset state of 0x00.
  //
  MmioWrite8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_MCR >> ShiftOffset), B_PCH_SERIAL_IO_UART_MCR_RTS | (HardwareFlowControl ? B_PCH_SERIAL_IO_UART_MCR_AFCE : 0) );

  return;
}


/**
  Initialize selected SerialIo UART.
  This init function MUST be used prior any SerialIo UART functions to init serial io controller
  if platform is going use serialio UART as debug output.

  @param[in]  UartNumber           Selects Serial IO UART device (0-2)
  @param[in]  FifoEnable           When TRUE, enables 64-byte FIFOs.
  @param[in]  BaudRate             Baud rate.
  @param[in]  LineControl          Data length, parity, stop bits.
  @param[in]  HardwareFlowControl  Automated hardware flow control. If TRUE, hardware automatically checks CTS when sending data,
                                   and sets RTS when receiving data.
  @retval     BOOLEAN              Initilization succeeded.

**/
BOOLEAN
EFIAPI
PchSerialIoUartInit (
  IN UINT8      UartNumber,
  IN BOOLEAN    FifoEnable,
  IN UINT32     BaudRate,
  IN UINT8      LineControl,
  IN BOOLEAN    HardwareFlowControl
  )
{
  UINT32 UartMode    = 0;
  UINTN  Bar         = 0;
  UINT8  ShiftOffset = 0;

  if (UartNumber > 3) {
    return FALSE;  // In case of invalid UART device
  }

#ifdef PCH_PO_FLAG
  ConfigureSerialIoController (UartNumber + PchSerialIoIndexUart0, PchSerialIoPci, 0);
#else
  ConfigureSerialIoController (UartNumber + PchSerialIoIndexUart0, PchSerialIoPci);
#endif

  //
  // Find UART Mode (Checking for 16550 Mode)
  //
  PchPcrRead32 (0x90, 0x618, &UartMode);

  if (UartMode == BIT2) {
    ShiftOffset = 2;
  }

  Bar = FindSerialIoBar (UartNumber + PchSerialIoIndexUart0, 0);

  if (Bar == 0xFFFFFFFF) {
    return FALSE;
  }

  SerialIo16550Init (Bar, FifoEnable, BaudRate, LineControl, HardwareFlowControl, ShiftOffset);
  return TRUE;
}


/**
  Write data to serial device.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param[in]  UartNumber       Selects Serial IO UART device (0-2)
  @param[in]  Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval     UINTN            Actual number of bytes writed to serial device.
**/
UINTN
EFIAPI
PchSerialIoUartOut (
  IN UINT8  UartNumber,
  IN UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  UINTN           BytesLeft;
  volatile UINTN  Base;
  UINT32          UartMode = 0;
  UINT8           ShiftOffset = 0;
  UINT32          TxAttemptCount = 0;

  if (UartNumber > 3) {
    return 0;  // In case of invalid UART device
  }

  Base = FindSerialIoBar (UartNumber + PchSerialIoIndexUart0, 0);

  //
  // Sanity checks to avoid infinite loop when trying to print through uninitialized UART
  //
  // If BAR is unavailable, write 0 bytes to the device
  //
  if (Base == 0xFFFFFFFF ||
      (Base & 0xFFFFFF00) == 0x0 ||
      MmioRead8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_USR >> ShiftOffset)) == 0xFF ||
      Buffer == NULL) {
    return 0;
  }

  PchPcrRead32(0x90, 0x618, &UartMode);

  if (UartMode == BIT2)
  {
    ShiftOffset = 2;
  }

  BytesLeft = NumberOfBytes;

  while (BytesLeft != 0 && TxAttemptCount < 200) {
    //
    // Write data while there's room in TXFIFO. If HW Flow Control was enabled, it happens automatically on hardware level.
    //
    while ((MmioRead8(Base + (R_PCH_SERIAL_IO_NATIVE_UART_IIR >> ShiftOffset)) & 0xF) == 0x10);

    if (MmioRead8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_USR >> ShiftOffset)) & B_PCH_SERIAL_IO_UART_USR_TFNF) {
      MmioWrite8 (Base + R_PCH_SERIAL_IO_NATIVE_UART_TXBUF, *Buffer);

      TxAttemptCount = 0;
      Buffer++;
      BytesLeft--;
    }
    TxAttemptCount++;
  }

  return NumberOfBytes;
}


/**
  Read data from serial device and save the datas in buffer.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param[in]  UartNumber           Selects Serial IO UART device (0-2)
  @param[out] Buffer               Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes        Number of output bytes which are cached in Buffer.
  @param[in]  WaitUntilBufferFull  When TRUE, function waits until whole buffer is filled. When FALSE,
                                   function returns as soon as no new characters are available.

  @retval     UINTN                Actual number of bytes read to the serial device.

**/
UINTN
EFIAPI
PchSerialIoUartIn (
  IN  UINT8     UartNumber,
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes,
  IN  BOOLEAN   WaitUntilBufferFull
  )
{
  UINTN  BytesReceived;
  UINTN  Base;
  UINT32 UartMode = 0;
  UINT8  ShiftOffset = 0;

  if (UartNumber > 3) {
    return 0;  // In case of invalid UART device
  }

  Base = FindSerialIoBar (UartNumber + PchSerialIoIndexUart0, 0);

  //
  // If BAR is unavailable, receive 0 bytes
  //
  if (Base == 0xFFFFFFFF) {
    return 0;
  }

  PchPcrRead32 (0x90, 0x618, &UartMode);
  if (UartMode == 4) {
    ShiftOffset = 2;
  }

  if (NULL == Buffer) {
    return 0;
  }

  BytesReceived = 0;

  while (BytesReceived != NumberOfBytes) {
    //
    // check if there's data in RX buffer
    //
    if (MmioRead8 (Base + (R_PCH_SERIAL_IO_NATIVE_UART_LSR >> ShiftOffset)) & B_PCH_SERIAL_IO_UART_LSR_RXDA) {
      //
      // Receive data
      //
      *Buffer = MmioRead8 (Base + R_PCH_SERIAL_IO_NATIVE_UART_RXBUF);
      Buffer++;
      BytesReceived++;
    } else {
      if (!WaitUntilBufferFull && ((MmioRead8(Base + (R_PCH_SERIAL_IO_NATIVE_UART_LSR >> ShiftOffset)) & BIT0) == 0)) {
        //
        // If there's no data and function shouldn't wait, exit early
        //
        return BytesReceived;
      }
    }
  }

  return BytesReceived;
}


/**
  Polls a serial device to see if there is any data waiting to be read.

  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @param[in]  UartNumber       Selects Serial IO UART device (0-2)

  @retval     TRUE             Data is waiting to be read from the serial device.
  @retval     FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
PchSerialIoUartPoll (
  IN  UINT8     UartNumber
  )
{
  UINTN Base;

  if (UartNumber > 3) {
    return FALSE;  // In case of invalid UART device
  }

  Base = FindSerialIoBar (UartNumber + PchSerialIoIndexUart0, 0);
  //
  // Read the serial port status
  //
  if ((MmioRead8 (Base + R_PCH_SERIAL_IO_NATIVE_UART_LSR) & B_PCH_SERIAL_IO_UART_LSR_RXDA) != 0) {
    return TRUE;
  }
  return FALSE;
}

