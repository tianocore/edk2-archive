/** @file
  Register Definitions for I2C Library.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_LIB_H_
#define _I2C_LIB_H_

#include <Uefi.h>
#include <Library/IoLib.h>
//
// FIFO write workaround value.
//
#define FIFO_WRITE_DELAY    2

/**
  Program LPSS I2C PCI controller's BAR0 and enable memory decode.

  @retval EFI_SUCCESS           - I2C controller's BAR0 is programmed and memory decode enabled.
**/
EFI_STATUS
ProgramPciLpssI2C (
  VOID
  );

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  ReadBytes             Number of bytes to be read
  @param[out] ReadBuffer            Address to which the value read has to be stored
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value read successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to read a byte via I2C

**/
EFI_STATUS
ByteReadI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  WriteBytes            Number of bytes to be written
  @param[in]  WriteBuffer           Address to which the byte value has to be written
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value written successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to write a byte via I2C

**/
EFI_STATUS
ByteWriteI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Read bytes from I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  ReadBytes           Number of bytes to be read
  @param[out] ReadBuffer          Address to which the value read has to be stored

  @retval     EFI_SUCCESS         Read bytes from I2C device successfully
  @retval     Others              Return status depends on ByteReadI2C_Basic

**/
EFI_STATUS
ByteReadI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer
  );

/**
  Write bytes to I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  WriteBytes          Number of bytes to be written
  @param[in]  WriteBuffer         Address to which the byte value has to be written

  @retval     EFI_SUCCESS         Write bytes to I2C device successfully
  @retval     Others              Return status depends on ByteWriteI2C_Basic

**/
EFI_STATUS
ByteWriteI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer
  );

#endif  // _I2C_LIB_H_

