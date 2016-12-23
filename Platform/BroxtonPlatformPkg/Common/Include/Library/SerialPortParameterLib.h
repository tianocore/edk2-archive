/** @file
  This library class provides capability to get/set serial port baud rate.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SERIAL_PORT_PARAMETER_LIB__
#define __SERIAL_PORT_PARAMETER_LIB__

#define STATUS_CODE_USE_RAM        BIT0
#define STATUS_CODE_USE_ISA_SERIAL BIT1
#define STATUS_CODE_USE_USB        BIT2
#define STATUS_CODE_USE_USB3       BIT3
#define STATUS_CODE_USE_SERIALIO   BIT4
#define STATUS_CODE_USE_TRACEHUB   BIT5
#define STATUS_CODE_CMOS_INVALID   BIT6
#define STATUS_CODE_CMOS_VALID     BIT7


/**
  Returns the serial port baud rate.

  @return  Baud rate of serial port.

**/
UINT32
EFIAPI
GetSerialPortBaudRate (
  VOID
  );

/**
  Sets the serial port baud rate value.

  @param[in] BaudRate     Baud rate value to be set.

  @retval  TRUE           The baud rate of serial port was sucessfully set.
  @retval  FALSE          The baud rate of serial port could not be set.

**/
BOOLEAN
EFIAPI
SetSerialPortBaudRate (
  UINT32        BaudRate
  );

/**
  Returns enabled debug interfaces.

  @retval  Enabled debug interfaces bitmask.

**/
UINT8
GetDebugInterface (
  );

/**
  Sets debug interfaces.

  @param[in] DebugInterface  Debug interfaces to be set.

**/
VOID
SetDebugInterface (
  UINT8 DebugInterface
  );

#endif

