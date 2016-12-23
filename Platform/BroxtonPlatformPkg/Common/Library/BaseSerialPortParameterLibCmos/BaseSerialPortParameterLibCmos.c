/** @file
  DebugPrintErrorLevel access library instance.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/PcdLib.h>
#include <Library/CmosAccessLib.h>


/**
  Returns the serial port baud rate.

  @return  Baud rate of serial port.

**/
UINT32
EFIAPI
GetSerialPortBaudRate (
  VOID
  )
{
  return ReadCmos32 (PcdGet8 (PcdSerialBaudRateCmosIndex));
}


/**
  Sets the serial port baud rate value.

  @param[in]                Baud rate value to be set.

  @retval    TRUE           The baud rate of serial port was sucessfully set.
  @retval    FALSE          The baud rate of serial port could not be set.

**/
BOOLEAN
EFIAPI
SetSerialPortBaudRate (
  UINT32        BaudRate
  )
{
  RETURN_STATUS    Status;

  Status = WriteCmos32 (PcdGet8 (PcdSerialBaudRateCmosIndex), BaudRate);
  return (BOOLEAN) (Status == RETURN_SUCCESS);
}


UINT8
GetDebugInterface (
  )
{
  return ReadCmos8 (PcdGet8 (PcdStatusCodeFlagsCmosIndex));
}


VOID
SetDebugInterface (
  UINT8 DebugInterface
  )
{
  WriteCmos8 (PcdGet8 (PcdStatusCodeFlagsCmosIndex), DebugInterface);
}

