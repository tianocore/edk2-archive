/** @file
  Serial I/O Port library functions with no library constructor/destructor.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/SerialPortParameterLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/CmosAccessLib.h>
#include <Library/ScSerialIoUartLib.h>

#ifdef TRACE_HUB_DEBUGLIB_USAGE
#include <Library/TraceHubDebugLib.h>
#endif

//
// UART register offsets and bitfields
//
#define RXBUF_OFFSET      0x00
#define TXBUF_OFFSET      0x00
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

//
// UART Register Bit Defines
//
#define FCR_FIFOE    BIT0
#define FCR_FIFO64   BIT5
#define LSR_TXRDY    0x20
#define LSR_RXDA     0x01
#define DLAB         BIT7
#define MCR_RTS      BIT1
#define MSR_CTS      BIT4
#define MSR_DSR      BIT5

#define MAX_BAUD_RATE     115200

UINT8
SerialPortReadRegister (
  UINTN  Offset
  )
{
  return IoRead8 ((UINTN) PcdGet64 (PcdSerialRegisterBase) + Offset);
}

UINT8
SerialPortWriteRegister (
  UINTN  Offset,
  UINT8  Value
  )
{
  return IoWrite8 ((UINTN) PcdGet64 (PcdSerialRegisterBase) + Offset, Value);
}

RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  return RETURN_SUCCESS;
}


/**
  Write data to serial device.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param[in]  Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval      0               Write data failed.
  @retval     !0               Actual number of bytes writed to serial device.

**/
UINTN
EFIAPI
UARTDbgOut (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
)
{
  UINTN    Result;
  UINTN    FifoSize;
  UINTN    Index;

  if (NULL == Buffer) {
    return 0;
  }

  //
  // Compute the maximum size of the Tx FIFO
  //
  FifoSize = 1;
  if ((PcdGet8 (PcdSerialFifoControl) & FCR_FIFOE) != 0) {
    if ((PcdGet8 (PcdSerialFifoControl) & FCR_FIFO64) == 0) {
      FifoSize = 16;
    } else {
      FifoSize = 64;
    }
  }

  Result = NumberOfBytes;

  while (NumberOfBytes != 0) {
    //
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    //
    while ((SerialPortReadRegister (LSR_OFFSET) & LSR_TXRDY) == 0);

    //
    // Fill then entire Tx FIFO
    //
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {
      if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
        if (PcdGetBool (PcdSerialDetectCable)) {
          //
          // Wait for both DSR and CTS to be set
          //   DSR is set if a cable is connected.
          //   CTS is set if it is ok to transmit data
          //
          //   DSR  CTS  Description                               Action
          //   ===  ===  ========================================  ========
          //    0    0   No cable connected.                       Wait
          //    0    1   No cable connected.                       Wait
          //    1    0   Cable connected, but not clear to send.   Wait
          //    1    1   Cable connected, and clear to send.       Transmit
          //
          while ((SerialPortReadRegister (MSR_OFFSET) & (MSR_DSR | MSR_CTS)) != (MSR_DSR | MSR_CTS));
        } else {
          //
          // Wait for both DSR and CTS to be set OR for DSR to be clear.
          //   DSR is set if a cable is connected.
          //   CTS is set if it is ok to transmit data
          //
          //   DSR  CTS  Description                               Action
          //   ===  ===  ========================================  ========
          //    0    0   No cable connected.                       Transmit
          //    0    1   No cable connected.                       Transmit
          //    1    0   Cable connected, but not clear to send.   Wait
          //    1    1   Cable connected, and clar to send.        Transmit
          //
          while ((SerialPortReadRegister (MSR_OFFSET) & (MSR_DSR | MSR_CTS)) == (MSR_DSR));
        }
      }

      //
      // Write byte to the transmit buffer.
      //
      SerialPortWriteRegister (TXBUF_OFFSET, *Buffer);
    }
  }

  return Result;
}


/**
  Common function to write trace data to a chosen debug interface like
  UART Serial device, USB Serial device or Trace Hub device

  @param[in]  Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{

  PchSerialIoUartOut (PcdGet8 (PcdSerialIoUartNumber), Buffer, NumberOfBytes);

  return RETURN_SUCCESS;
}


/**
  Read data from serial device and save the datas in buffer.

  If the buffer is NULL, then return 0;
  if NumberOfBytes is zero, then return 0.

  @param[out]  Buffer           Point of data buffer which need to be writed.
  @param[in]   NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval       0               Read data failed.
  @retval      !0               Actual number of bytes raed to serial device.

**/
UINTN
EFIAPI
UARTDbgIn (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
  )
{
  UINTN    Result;
  UINT8    Mcr;

  if (NULL == Buffer) {
    return 0;
  }

  Result = NumberOfBytes;

  Mcr = (UINT8) (SerialPortReadRegister (MCR_OFFSET) & ~ MCR_RTS);

  for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {
    //
    // Wait for the serial port to have some data.
    //
    while ((SerialPortReadRegister (LSR_OFFSET) & LSR_RXDA) == 0) {
      if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
        //
        // Set RTS to let the peer send some data
        //
        SerialPortWriteRegister (MCR_OFFSET, (UINT8) (Mcr | MCR_RTS));
      }
    }

    if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
      //
      // Clear RTS to prevent peer from sending data
      //
      SerialPortWriteRegister (MCR_OFFSET, Mcr);
    }

    //
    // Read byte from the receive buffer.
    //
    *Buffer = SerialPortReadRegister (RXBUF_OFFSET);
  }

  return Result;
}


/**
  Common function to Read data from UART serial device, USB serial device and save the datas in buffer.

  @param[out] Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
  )
{

  PchSerialIoUartIn (PcdGet8 (PcdSerialIoUartNumber), Buffer, NumberOfBytes, FALSE);

  return RETURN_SUCCESS;
}


/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls a serial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
UARTDbgPoll (
  VOID
  )
{
  //
  // Read the serial port status
  //
  if ((SerialPortReadRegister (LSR_OFFSET) & LSR_RXDA) != 0) {
    if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
      //
      // Clear RTS to prevent peer from sending data
      //
      SerialPortWriteRegister (MCR_OFFSET, (UINT8) (SerialPortReadRegister (MCR_OFFSET) & ~MCR_RTS));
    }
    return TRUE;
  }

  if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
    //
    // Set RTS to let the peer send some data
    //
    SerialPortWriteRegister (MCR_OFFSET, (UINT8) (SerialPortReadRegister (MCR_OFFSET) | MCR_RTS));
  }
  return FALSE;
}


/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls a serial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{

  BOOLEAN   Status;

  Status = FALSE;

  Status |= PchSerialIoUartPoll (PcdGet8 (PcdSerialIoUartNumber));

  return Status;
}

