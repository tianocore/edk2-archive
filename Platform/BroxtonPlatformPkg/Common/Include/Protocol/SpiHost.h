/** @file
  SPI Host Protocol.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_HOST_H__
#define __SPI_HOST_H__

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/MmioDevice.h>
#include <Protocol/SpiAcpi.h>


/// SPI device operation
///
/// This structure provides the information necessary for an operation
/// on an SPI device
///
typedef struct {
  ///
  /// Number of bytes to send to the SPI device
  ///
  UINT32 WriteBytes;

  ///
  /// Number of bytes to read, set to zero for write only operations
  ///
  UINT32 ReadBytes;

  ///
  /// Address of the buffer containing the data to send to the SPI device.
  /// The WriteBuffer must be at least WriteBytes in length.
  ///
  UINT8 *WriteBuffer;

  ///
  /// Address of the buffer to receive data from the SPI device. Use NULL
  /// for write only operations.  The ReadBuffer must be at least ReadBytes
  /// in length.
  ///
  UINT8 *ReadBuffer;

  UINT32 BytesWritten;

  UINT32 BytesRead;

  ///
  /// Timeout for the SPI operation in 100 ns units
  ///
  UINT32 Timeout;
} EFI_SPI_REQUEST_PACKET;


typedef struct _EFI_SPI_ACPI_PROTOCOL EFI_SPI_ACPI_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_SPI_ENUMERATE) (
  IN EFI_SPI_ACPI_PROTOCOL *This,
  IN OUT EFI_SPI_DEVICE **Device
  );

struct _EFI_SPI_ACPI_PROTOCOL{
  ///
  /// Start an I2C operation on the bus
  ///
  EFI_SPI_ENUMERATE Enumerate;
};

typedef struct _EFI_SPI_HOST_PROTOCOL EFI_SPI_HOST_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI* SPI_HOST_STARTREQUEST) (
  IN EFI_SPI_HOST_PROTOCOL *This,
  IN EFI_SPI_DEVICE * SpiDevice,
  IN EFI_EVENT Event OPTIONAL,
  IN EFI_SPI_REQUEST_PACKET *RequestPacket,
  OUT EFI_STATUS *SpiStatus
  );

struct _EFI_SPI_HOST_PROTOCOL{
  SPI_HOST_STARTREQUEST StartRequest;
  UINT32 MaximumReceiveBytes;
  UINT32 MaximumTransmitBytes;
  UINT32 MaximumTotalBytes;
};

extern EFI_GUID gEfiSpiHostProtocolGuid;

#endif  //  __SPI_HOST_H__

