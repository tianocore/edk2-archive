/** @file
  SPI Bus Protocol.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SPI_BUS_H__
#define __SPI_BUS_H__


typedef struct _EFI_SPI_BUS_PROTOCOL EFI_SPI_BUS_PROTOCOL;

typedef
EFI_STATUS (EFIAPI* SPI_BUS_STARTREQUEST)(
  IN EFI_SPI_BUS_PROTOCOL *This,
  IN EFI_EVENT Event OPTIONAL,
  IN EFI_SPI_REQUEST_PACKET *RequestPacket,
  OUT EFI_STATUS *SpiStatus OPTIONAL
  );

typedef
UINTN
(EFIAPI* SPI_BUS_READ)(
  IN OUT UINT8   *Buffer,
  IN  UINTN   NumberOfBytes
  );

typedef
UINTN
(EFIAPI* SPI_BUS_WRITE)(
  IN OUT UINT8  *Buffer,
  IN UINTN  NumberOfBytes
  );

struct _EFI_SPI_BUS_PROTOCOL {
  SPI_BUS_STARTREQUEST  StartRequest;
  SPI_BUS_READ          SPIRead;
  SPI_BUS_WRITE         SPIWrite;
  UINT32                MaximumReceiveBytes;
  UINT32                MaximumTransmitBytes;
  UINT32                MaximumTotalBytes;
};

extern EFI_GUID gEfiSpiBusProtocolGuid;

#endif  //  __SPI_BUS_H__

