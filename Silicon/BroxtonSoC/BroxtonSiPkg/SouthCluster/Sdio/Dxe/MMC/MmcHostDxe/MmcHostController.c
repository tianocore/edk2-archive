/** @file
  MMC Host I/O protocol implementation

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MmcHostDriver.h"

#define MMC_HOST_DEBUG(a)    do { \
                                if (MmcHostData->EnableVerboseDebug) { \
                                    DEBUG (a); \
                                  } \
                                } while (0);

UINT32  gMmcHostDebugLevel = DEBUG_INFO;

/**
  Get Error Reason

  @param[in]  CommandIndex      The command index to set the command index field of command register
  @param[in]  ErrorCode         Mmchost specific error code

  @retval     EFI_DEVICE_ERROR
  @retval     EFI_TIMEOUT
  @retval     EFI_CRC_ERROR

**/
STATIC
EFI_STATUS
GetErrorReason (
  IN  UINT16    CommandIndex,
  IN  UINT16    ErrorCode
  )
{
  EFI_STATUS    Status;

  Status = EFI_DEVICE_ERROR;
  DEBUG ((gMmcHostDebugLevel, "[%2d] -- ", CommandIndex));

  if (ErrorCode & BIT0) {
    Status = EFI_TIMEOUT;
    DEBUG ((gMmcHostDebugLevel, "Command Timeout Error"));
  }

  if (ErrorCode & BIT1) {
    Status = EFI_CRC_ERROR;
    DEBUG ((gMmcHostDebugLevel, "Command CRC Error"));
  }

  if (ErrorCode & BIT2) {
    DEBUG ((gMmcHostDebugLevel, "Command End Bit Error"));
  }

  if (ErrorCode & BIT3) {
    DEBUG ((gMmcHostDebugLevel, "Command Index Error"));
  }
  if (ErrorCode & BIT4) {
    Status = EFI_TIMEOUT;
    DEBUG ((gMmcHostDebugLevel, "Data Timeout Error"));
  }

  if (ErrorCode & BIT5) {
    Status = EFI_CRC_ERROR;
    DEBUG ((gMmcHostDebugLevel, "Data CRC Error"));
  }

  if (ErrorCode & BIT6) {
    DEBUG ((gMmcHostDebugLevel, "Data End Bit Error"));
  }

  if (ErrorCode & BIT7) {
    DEBUG ((gMmcHostDebugLevel, "Current Limit Error"));
  }

  if (ErrorCode & BIT8) {
    DEBUG ((gMmcHostDebugLevel, "Auto CMD12 Error"));
  }

  if (ErrorCode & BIT9) {
    DEBUG ((gMmcHostDebugLevel, "ADMA Error"));
  }

  DEBUG ((gMmcHostDebugLevel, "\n"));

  return Status;
}


/**
  Read MMC host register

  @param[in]      MmcHost                    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]      Width                      Signifies the width of the memory or I/O operations.
  @param[in]      Offset                     Offset of the MMC Card
  @param[in, out] Buffer                     Buffer read from MMC Card

  @retval         EFI_INVALID_PARAMETER
  @retval         EFI_UNSUPPORTED
  @retval         EFI_SUCCESS

**/
STATIC
EFI_STATUS
MmcHostRead (
  IN     MMCHOST_DATA                 *MmcHost,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINTN                        Offset,
  IN OUT VOID                         *Buffer
  )
{
  return MmcHost->PciIo->Mem.Read (
                               MmcHost->PciIo,
                               Width,
                               0,
                               (UINT64) Offset,
                               1,
                               Buffer
                               );
}


/**
  Read MMC host register

  @param[in]  MmcHost      Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset       Offset of the MMC Card

  @retval     Data         Data read from MMC Card

**/
UINT8
MmcHostRead8 (
  IN     MMCHOST_DATA      *MmcHost,
  IN     UINTN             Offset
  )
{
  UINT8 Data;

  MmcHostRead (MmcHost, EfiPciIoWidthUint8, Offset, &Data);

  return Data;
}


/**
  Read MMC host register

  @param[in]  MmcHost        Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset         Offset of the MMC Card

  @retval     Data           Data read from MMC Card

**/
UINT16
MmcHostRead16 (
  IN     MMCHOST_DATA        *MmcHost,
  IN     UINTN               Offset
  )
{
  UINT16 Data;

  MmcHostRead (MmcHost, EfiPciIoWidthUint16, Offset, &Data);

  return Data;
}


/**
  Read MMC host register

  @param[in]  MmcHost    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset     Offset of the MMC Card

  @retval     Data       Data read from MMC Card

**/
UINT32
MmcHostRead32 (
  IN     MMCHOST_DATA    *MmcHost,
  IN     UINTN           Offset
  )
{
  UINT32 Data;

  MmcHostRead (MmcHost, EfiPciIoWidthUint32, Offset, &Data);

  return Data;
}


/**
  Dump MMC host registers

  @param[in]  MmcHost     Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     None

**/
VOID
DebugPrintMmcHostRegisters (
  IN     MMCHOST_DATA     *MmcHost
  )
{
#ifdef VERBOSE_REGISTER_ACCESS_DEBUG
  UINTN    Loop;

  if (!MmcHost->EnableVerboseDebug) {
    return;
  }

  for (Loop = 0; Loop < 0x40; Loop++) {
    DEBUG ((EFI_D_INFO, " %02x", MmcHostRead8 (MmcHost, Loop)));
    if ((Loop % 0x10) == 0xf) {
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }

#endif
}


/**
  Dump MMC host registers before write

  @param[in]  MmcHost    Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     None

**/
VOID
DebugPreMmcHostWrite (
  IN     MMCHOST_DATA                  *MmcHost
  )
{
#ifdef VERBOSE_REGISTER_ACCESS_DEBUG
  STATIC UINTN DebugCount = 0;

  if (!MmcHost->EnableVerboseDebug) {
    return;
  }

  DebugCount++;

  if (DebugCount < 0x100) {
    DEBUG ((EFI_D_INFO, "MMC HOST Registers before write:\n"));
    DebugPrintMmcHostRegisters (MmcHost);
  }
#endif
}


/**
  Dump MMC host registers after write

  @param[in]  MmcHost      Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     None

**/
VOID
DebugPostMmcHostWrite (
  IN     MMCHOST_DATA                  *MmcHost
  )
{
#ifdef VERBOSE_REGISTER_ACCESS_DEBUG
  STATIC UINTN DebugCount = 0;

  if (!MmcHost->EnableVerboseDebug) {
    return;
  }

  DebugCount++;

  if (DebugCount < 0x10) {
    DEBUG ((EFI_D_INFO, "MMC HOST Registers after write:\n"));
    DebugPrintMmcHostRegisters (MmcHost);
  }
#endif
}


/**
  Write MMC host register

  @param[in]      MmcHost                Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]      Width                  Signifies the width of the memory or I/O operations.
  @param[in]      Offset                 Offset of the MMC Card
  @param[in, out] Buffer                 Buffer read from MMC Card

  @retval         EFI_INVALID_PARAMETER
  @retval         EFI_UNSUPPORTED
  @retval         EFI_SUCCESS

**/
STATIC
EFI_STATUS
MmcHostWrite (
  IN     MMCHOST_DATA                 *MmcHost,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINTN                        Offset,
  IN OUT VOID                         *Buffer
  )
{
  EFI_STATUS    Status;

  DebugPreMmcHostWrite (MmcHost);
  Status = MmcHost->PciIo->Mem.Write (
                                 MmcHost->PciIo,
                                 Width,
                                 0,
                                 (UINT64) Offset,
                                 1,
                                 Buffer
                                 );

  DebugPostMmcHostWrite (MmcHost);

  return Status;
}


/**
  Write MMC host register

  @param[in]  MmcHost    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset     Offset of the MMC Card
  @param[in]  Data       Data write to MMC Card

  @retval     Data       Written to MmcHost

**/
UINT8
MmcHostWrite8 (
  IN     MMCHOST_DATA                 *MmcHost,
  IN     UINTN                        Offset,
  IN     UINT8                        Data
  )
{
  MmcHostWrite (MmcHost, EfiPciIoWidthUint8, Offset, &Data);

  return Data;
}


/**
  Write MMC host register

  @param[in]  MmcHost    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset     Offset of the MMC Card
  @param[in]  Data       Data write to MMC Card

  @retval     Data       Data written to MMC Card

**/
UINT16
MmcHostWrite16 (
  IN     MMCHOST_DATA                 *MmcHost,
  IN     UINTN                        Offset,
  IN     UINT16                       Data
  )
{
  MmcHostWrite (MmcHost, EfiPciIoWidthUint16, Offset, &Data);

  return Data;
}


/**
  Write MMC host register

  @param[in]  MmcHost    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Offset     Offset of the MMC Card
  @param[in]  Data       Data write to MMC Card

  @retval     Data       Data written to MMC Card

**/
UINT32
MmcHostWrite32 (
  IN     MMCHOST_DATA                 *MmcHost,
  IN     UINTN                        Offset,
  IN     UINT32                       Data
  )
{
  MmcHostWrite (MmcHost, EfiPciIoWidthUint32, Offset, &Data);

  return Data;
}


/**
  Check Controller Version

  @param[in]  MmcHost      Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     Data         Written to MmcHost

**/
UINT32
CheckControllerVersion (
  IN MMCHOST_DATA          *MmcHost
  )
{
  UINT16     Data16;

  Data16 = MmcHostRead16 (MmcHost, MMIO_CTRLRVER);
  DEBUG ((EFI_D_INFO, "CheckControllerVersion: %x \n", Data16 & 0xFF));

  return (Data16 & 0xFF);
}


/**
  Power on/off the LED associated with the slot

  @param[in]  This         Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Enable       TRUE to set LED on, FALSE to set LED off

  @retval     EFI_SUCCESS

**/
STATIC
EFI_STATUS
HostLEDEnable (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  BOOLEAN                    Enable
  )
{
  MMCHOST_DATA                   *MmcHostData;
  UINT8                          Data;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);
  Data = MmcHostRead8 (MmcHostData, MMIO_HOSTCTL);

  if (Enable) {
    //
    //LED On
    //
    Data |= BIT0;
  } else {
    //
    //LED Off
    //
    Data &= ~BIT0;
  }

  MmcHostWrite8 (MmcHostData, MMIO_HOSTCTL, Data);

  return EFI_SUCCESS;
}


/**
  The main function used to send the command to the card inserted into the MMC host
  slot.
  It will assemble the arguments to set the command register and wait for the command
  and transfer completed until timeout. Then it will read the response register to fill
  the ResponseData

  @param[in]  This                    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  CommandIndex            The command index to set the command index field of command register
  @param[in]  Argument                Command argument to set the argument field of command register
  @param[in]  DataType                TRANSFER_TYPE, indicates no data, data in or data out
  @param[in]  Buffer                  Contains the data read from / write to the device
  @param[in]  BufferSize              The size of the buffer
  @param[in]  ResponseType            RESPONSE_TYPE
  @param[in]  TimeOut                 Time out value in 1 ms unit
  @param[out] ResponseData            Depending on the ResponseType, such as CSD or card status

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_UNSUPPORTED
  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SendCommand (
  IN   EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData OPTIONAL
  )
{
  EFI_STATUS                      Status;
  MMCHOST_DATA                    *MmcHostData;
  UINT32                          ResponseDataCount;
  UINT16                          Data16;
  UINT32                          Data32;
  UINT64                          Data64;
  UINT8                           Index;
  BOOLEAN                         CommandCompleted;
  INT32                           Timeout = 1000;

  Status             = EFI_SUCCESS;
  ResponseDataCount  = 1;
  MmcHostData        = MMCHOST_DATA_FROM_THIS (This);

  if (Buffer != NULL && DataType == NoData) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((gMmcHostDebugLevel, "SendCommand: Invalid parameter -> \n"));
    goto Exit;
  }

  if (((UINTN) Buffer & (This->HostCapability.BoundarySize - 1)) != (UINTN) NULL) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((gMmcHostDebugLevel, "SendCommand: Invalid parameter -> \n"));
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "SendCommand: CMD%d \n", CommandIndex));
  if (MmcHostData->EnableVerboseDebug) {
    DEBUG ((EFI_D_INFO, "00 -10:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x00), MmcHostRead32 (MmcHostData, 0x04), MmcHostRead32 (MmcHostData, 0x08), MmcHostRead32 (MmcHostData, 0x0C)));
    DEBUG ((EFI_D_INFO, "10 -20:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x10), MmcHostRead32 (MmcHostData, 0x14), MmcHostRead32 (MmcHostData, 0x18), MmcHostRead32 (MmcHostData, 0x1C)));
    DEBUG ((EFI_D_INFO, "20 -30:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x20), MmcHostRead32 (MmcHostData, 0x24), MmcHostRead32 (MmcHostData, 0x28), MmcHostRead32 (MmcHostData, 0x2C)));
    DEBUG ((EFI_D_INFO, "30 -40:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x30), MmcHostRead32 (MmcHostData, 0x34), MmcHostRead32 (MmcHostData, 0x38), MmcHostRead32 (MmcHostData, 0x3C)));
    DEBUG ((EFI_D_INFO, "40 -50:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x40), MmcHostRead32 (MmcHostData, 0x44), MmcHostRead32 (MmcHostData, 0x48), MmcHostRead32 (MmcHostData, 0x4C)));
    DEBUG ((EFI_D_INFO, "50 -60:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x50), MmcHostRead32 (MmcHostData, 0x54), MmcHostRead32 (MmcHostData, 0x58), MmcHostRead32 (MmcHostData, 0x5C)));
  }

  //
  // Check CMD INHIBIT and DATA INHIBIT before send command
  //
  do {
    Data32 = MmcHostRead32 (MmcHostData, MMIO_PSTATE);
    if (MmcHostData->EnableVerboseDebug) {
      DEBUG ((EFI_D_INFO, "Wait CMD INHIBIT %x\n",Data32 ));
    }
  } while (Timeout-- > 0 && Data32 & BIT0);

  Timeout = 1000;
  do {
    Data32 = MmcHostRead32 (MmcHostData, MMIO_PSTATE);
    if (MmcHostData->EnableVerboseDebug) {
      DEBUG ((EFI_D_INFO, "Wait DATA INHIBIT %x\n",Data32 ));
    }
  } while (Timeout-- >0 && Data32 & BIT1);

  //
  //Clear status bits
  //
  MMC_HOST_DEBUG ((EFI_D_INFO, "NINTSTS(0x30) <- 0x%x\n", (UINTN) 0xffff));
  MmcHostWrite16 (MmcHostData, MMIO_NINTSTS, 0xffff);
  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: NINTSTS (0x30): 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_NINTSTS)));

  MMC_HOST_DEBUG ((EFI_D_INFO, "ERINTSTS <- 0x%x\n", (UINTN) 0xffff));
  MmcHostWrite16 (MmcHostData, MMIO_ERINTSTS, 0xffff);
  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: ERINTSTS (0x32): 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_ERINTSTS)));

  Data16 = MmcHostRead16 (MmcHostData, MMIO_NINTEN);
  MmcHostWrite16 (MmcHostData, MMIO_NINTEN,  (Data16 | BIT3));
  MMC_HOST_DEBUG ((DEBUG_INFO, "SendCommand: NINTEN (0x34) <- 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_NINTEN)));
  MMC_HOST_DEBUG ((DEBUG_INFO, "SendCommand: DMAADR (0x00) <- 0x%x\n", (UINTN) Buffer));
  MmcHostWrite32 (MmcHostData, MMIO_DMAADR, (UINT32) (UINTN) Buffer);

  if (Buffer != NULL) {
    Data16 = MmcHostRead16 (MmcHostData, MMIO_BLKSZ);
    Data16 &= ~(0xFFF);
    if (BufferSize <= MmcHostData->BlockLength) {
      Data16 |= BufferSize;
    } else {
      Data16 |= MmcHostData->BlockLength;
    }
    Data16 |= 0x7000; // Set to 512KB for SDMA block size
  } else {
    Data16 = 0;
  }

  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: BLKSZ(0x04) <- 0x%x\n", Data16));
  MmcHostWrite16 (MmcHostData, MMIO_BLKSZ, Data16);

  if (Buffer != NULL) {
    if (BufferSize <= MmcHostData->BlockLength) {
      Data16 = 1;
    } else {
      Data16 = (UINT16) (BufferSize / MmcHostData->BlockLength);
    }
  } else {
    Data16 = 0;
  }

  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: BLKCNT(0x06) <- 0x%x\n", Data16));
  MmcHostWrite16 (MmcHostData, MMIO_BLKCNT, Data16);

  //
  // Argument
  //
  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: CMDARG (0x08) <- 0x%x\n", Argument));
  MmcHostWrite32 (MmcHostData, MMIO_CMDARG, Argument);

  //
  // Transfer Mode
  //
  Data16 = MmcHostRead16 (MmcHostData, MMIO_XFRMODE);
  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: XFRMODE (0x0C) mode read 0x%x\n", Data16));

  //
  // BIT0   DMA Enable
  // BIT2   Auto Cmd12
  //
  if (DataType == InData) {
    Data16 |= BIT4 | BIT0;
  } else if (DataType == OutData) {
    Data16 &= ~BIT4;
    Data16 |= BIT0;
  } else {
    Data16 &= ~(BIT4 | BIT0);
  }

  if (BufferSize <= MmcHostData->BlockLength) {
    Data16 &= ~ (BIT5 | BIT1 | BIT2);
  } else {
    if (MmcHostData->IsAutoStopCmd && !MmcHostData->IsEmmc) {
      Data16 |= (BIT5 | BIT1 | BIT2);
    } else {
      Data16 |= (BIT5 | BIT1);
    }
  }
  if (CommandIndex == SEND_EXT_CSD) {
    Data16 |= BIT1;
    MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand:Enable bit 1, XFRMODE <- 0x%x\n", Data16));
  }

  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: XFRMODE (0x0C) <- 0x%x\n", Data16));
  MmcHostWrite16 (MmcHostData, MMIO_XFRMODE, Data16);

  switch (ResponseType) {
    case ResponseNo:
      Data16 = (CommandIndex << 8);
      ResponseDataCount = 0;
      break;

    case ResponseR1:
    case ResponseR5:
    case ResponseR6:
    case ResponseR7:
      Data16 = (CommandIndex << 8) | BIT1 | BIT4| BIT3;
      ResponseDataCount = 1;
      break;

    case ResponseR1b:
    case ResponseR5b:
      Data16 = (CommandIndex << 8) | BIT0 | BIT1 | BIT4| BIT3;
      ResponseDataCount = 1;
      break;

    case ResponseR2:
      Data16 = (CommandIndex << 8) | BIT0 | BIT3;
      ResponseDataCount = 4;
      break;

    case ResponseR3:
    case ResponseR4:
      Data16 = (CommandIndex << 8) | BIT1;
      ResponseDataCount = 1;
      break;

    default:
      ASSERT (0);
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
  }

  if (DataType != NoData) {
    Data16 |= BIT5;
  }

  HostLEDEnable (This, TRUE);

  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: SDCMD(0x0E) <- 0x%x\n", Data16));
  CommandCompleted = FALSE;
  MmcHostWrite16 (MmcHostData, MMIO_SDCMD, Data16);

  if (MmcHostData->EnableVerboseDebug) {
    DEBUG ((EFI_D_INFO, "After write to  Command\n"));
    DEBUG ((EFI_D_INFO, "00 -10:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x00), MmcHostRead32 (MmcHostData, 0x04), MmcHostRead32 (MmcHostData, 0x08), MmcHostRead32 (MmcHostData, 0x0C)));
    DEBUG ((EFI_D_INFO, "10 -20:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x10), MmcHostRead32 (MmcHostData, 0x14), MmcHostRead32 (MmcHostData, 0x18), MmcHostRead32 (MmcHostData, 0x1C)));
    DEBUG ((EFI_D_INFO, "20 -30:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x20), MmcHostRead32 (MmcHostData, 0x24), MmcHostRead32 (MmcHostData, 0x28), MmcHostRead32 (MmcHostData, 0x2C)));
    DEBUG ((EFI_D_INFO, "30 -40:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x30), MmcHostRead32 (MmcHostData, 0x34), MmcHostRead32 (MmcHostData, 0x38), MmcHostRead32 (MmcHostData, 0x3C)));
    DEBUG ((EFI_D_INFO, "40 -50:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x40), MmcHostRead32 (MmcHostData, 0x44), MmcHostRead32 (MmcHostData, 0x48), MmcHostRead32 (MmcHostData, 0x4C)));
    DEBUG ((EFI_D_INFO, "50 -60:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x50), MmcHostRead32 (MmcHostData, 0x54), MmcHostRead32 (MmcHostData, 0x58), MmcHostRead32 (MmcHostData, 0x5C)));
  }

  TimeOut *= 1000;  //ms to us conversion
  do {
    Data16 = MmcHostRead16 (MmcHostData, MMIO_ERINTSTS);
    if (MmcHostData->EnableVerboseDebug && (Data16 != 0)) {
      DEBUG ((EFI_D_INFO, "SendCommand: ERINTSTS (0x32): 0x%x\n", Data16));
      DEBUG ((EFI_D_INFO, "SendCommand: ERINTEN (0x36): 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_ERINTEN)));
      DEBUG ((EFI_D_INFO, "SendCommand: NINTSTS (0x30): 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_NINTSTS)));
      DEBUG ((EFI_D_INFO, "SendCommand: NINTEN (0x34): 0x%x\n", MmcHostRead16 (MmcHostData, MMIO_NINTEN)));
    }

    if ((Data16 & 0x17FF) != 0) {
      Status = GetErrorReason (CommandIndex, Data16);
      DEBUG ((EFI_D_INFO, "00 -10:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x00), MmcHostRead32 (MmcHostData, 0x04), MmcHostRead32 (MmcHostData, 0x08), MmcHostRead32 (MmcHostData, 0x0C)));
      DEBUG ((EFI_D_INFO, "10 -20:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x10), MmcHostRead32 (MmcHostData, 0x14), MmcHostRead32 (MmcHostData, 0x18), MmcHostRead32 (MmcHostData, 0x1C)));
      DEBUG ((EFI_D_INFO, "20 -30:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x20), MmcHostRead32 (MmcHostData, 0x24), MmcHostRead32 (MmcHostData, 0x28), MmcHostRead32 (MmcHostData, 0x2C)));
      DEBUG ((EFI_D_INFO, "30 -40:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x30), MmcHostRead32 (MmcHostData, 0x34), MmcHostRead32 (MmcHostData, 0x38), MmcHostRead32 (MmcHostData, 0x3C)));
      DEBUG ((EFI_D_INFO, "40 -50:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x40), MmcHostRead32 (MmcHostData, 0x44), MmcHostRead32 (MmcHostData, 0x48), MmcHostRead32 (MmcHostData, 0x4C)));
      DEBUG ((EFI_D_INFO, "50 -60:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x50), MmcHostRead32 (MmcHostData, 0x54), MmcHostRead32 (MmcHostData, 0x58), MmcHostRead32 (MmcHostData, 0x5C)));
      goto Exit;
    }

    Data16 = MmcHostRead16 (MmcHostData, MMIO_NINTSTS) & 0x1ff;
    if (MmcHostData->EnableVerboseDebug && (Data16 > 1)) {
      DEBUG ((EFI_D_INFO, "SendCommand: NINTSTS (0x30): 0x%x\n", Data16));
    }

    if ((Data16 & BIT0) == BIT0) {
      //
      // Command completed
      //
      CommandCompleted = TRUE;

      MmcHostWrite16 (MmcHostData, MMIO_NINTSTS, BIT0);

      if ((DataType == NoData) && (ResponseType != ResponseR1b)) {
        break;
      }
    }

    if (CommandCompleted) {
      //
      // DMA interrupted
      //
      if ((Data16 & BIT3) == BIT3) {
        MmcHostWrite16 (MmcHostData, MMIO_NINTSTS, BIT3);
        Data32 = MmcHostRead32 (MmcHostData, MMIO_DMAADR);
        MMC_HOST_DEBUG ((DEBUG_INFO, "SendCommand: DMAADR (0x00) <- 0x%x\n", Data32));
        MmcHostWrite32 (MmcHostData, MMIO_DMAADR, Data32);
      }

      //
      // Transfer completed
      //
      if ((Data16 & BIT1) == BIT1) {
        MmcHostWrite16 (MmcHostData, MMIO_NINTSTS, BIT1);
        break;
      }
    }

    gBS->Stall (1);
    TimeOut --;

  } while (TimeOut > 0);

  if (TimeOut == 0) {
    Status = EFI_TIMEOUT;
    DEBUG ((gMmcHostDebugLevel, "SendCommand: Time out \n"));
    goto Exit;
  }

  if (ResponseData != NULL) {
    UINT32 *ResDataPtr = NULL;

    ResDataPtr = ResponseData;
    for (Index = 0; Index < ResponseDataCount; Index++) {
      *ResDataPtr = MmcHostRead32 (MmcHostData, MMIO_RESP + Index * 4);
      ResDataPtr++;
    }
    MMC_HOST_DEBUG ((EFI_D_INFO, "Reponse Data 0: RESPONSE (0x10) <- 0x%x\n", *ResponseData));

    if (ResponseType == ResponseR2) {
      //
      // Adjustment for R2 response
      //
      Data32 = 1;
      for (Index = 0; Index < ResponseDataCount; Index++) {
        Data64 = LShiftU64 (*ResponseData, 8);
        *ResponseData = (UINT32) ((Data64 & 0xFFFFFFFF) | Data32);
        Data32 =  (UINT32) RShiftU64 (Data64, 32);
        ResponseData++;
      }
    }
  }

  if (MmcHostData->EnableVerboseDebug) {
    DEBUG ((EFI_D_INFO, "Before Exit Send Command\n"));
    DEBUG ((EFI_D_INFO, "00 -10:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x00), MmcHostRead32 (MmcHostData, 0x04), MmcHostRead32 (MmcHostData, 0x08), MmcHostRead32 (MmcHostData, 0x0C)));
    DEBUG ((EFI_D_INFO, "10 -20:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x10), MmcHostRead32 (MmcHostData, 0x14), MmcHostRead32 (MmcHostData, 0x18), MmcHostRead32 (MmcHostData, 0x1C)));
    DEBUG ((EFI_D_INFO, "20 -30:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x20), MmcHostRead32 (MmcHostData, 0x24), MmcHostRead32 (MmcHostData, 0x28), MmcHostRead32 (MmcHostData, 0x2C)));
    DEBUG ((EFI_D_INFO, "30 -40:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x30), MmcHostRead32 (MmcHostData, 0x34), MmcHostRead32 (MmcHostData, 0x38), MmcHostRead32 (MmcHostData, 0x3C)));
    DEBUG ((EFI_D_INFO, "40 -50:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x40), MmcHostRead32 (MmcHostData, 0x44), MmcHostRead32 (MmcHostData, 0x48), MmcHostRead32 (MmcHostData, 0x4C)));
    DEBUG ((EFI_D_INFO, "50 -60:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x50), MmcHostRead32 (MmcHostData, 0x54), MmcHostRead32 (MmcHostData, 0x58), MmcHostRead32 (MmcHostData, 0x5C)));
  }

Exit:
  HostLEDEnable (This, FALSE);
  MMC_HOST_DEBUG ((EFI_D_INFO, "SendCommand: Status -> %r\n", Status));

  return Status;
}


/**
  Set clock frequency of the host, the actual frequency
  may not be the same as MaxFrequencyInKHz. It depends on
  the max frequency the host can support, divider, and host
  speed mode.

  @param[in]  This             Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  MaxFrequency     Max frequency in HZ

  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetClockFrequency (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                      MaxFrequency
  )
{
  UINT16                          Data16;
  UINT32                          Frequency;
  UINT32                          Divider = 0;
  MMCHOST_DATA                    *MmcHostData;
  UINT32                          TimeOutCount;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: BaseClockInMHz = %d \n", MmcHostData->BaseClockInMHz));

  Frequency = (MmcHostData->BaseClockInMHz * 1000 * 1000) / MaxFrequency;
  DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: FrequencyInHz = %d \n", Frequency));

  if ((MmcHostData->BaseClockInMHz * 1000 * 1000 % MaxFrequency) != 0) {
    Frequency += 1;
  }

  Divider = 1;
  while (Frequency > Divider) {
    Divider = Divider * 2;
  }

  if (Divider >= 0x400) {
    Divider = 0x200;
  }
  DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: before shift: Base Clock Divider = 0x%x \n", Divider));
  Divider = Divider >> 1;
  DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: before shift: Base Clock Divider = 0x%x \n", Divider));

  MmcHostData->CurrentClockInKHz = (MmcHostData->BaseClockInMHz * 1000);
  if (Divider != 0) {
    MmcHostData->CurrentClockInKHz = MmcHostData->CurrentClockInKHz / (Divider * 2);
  }

  if (2 == CheckControllerVersion (MmcHostData)) {
    Data16 = (UINT16) ((Divider & 0xFF) << 8 | (((Divider & 0xFF00) >>8)<<6));
  } else {
    Data16 = (UINT16) ( Divider << 8);
  }

  DEBUG ((gMmcHostDebugLevel,
    "SetClockFrequency: base=%dMHz, clkctl=0x%04x, f=%dKHz\n",
    MmcHostData->BaseClockInMHz,
    Data16,
    MmcHostData->CurrentClockInKHz
    ));
  DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: set MMIO_CLKCTL value = 0x%x \n", Data16));
  MmcHostWrite16 (MmcHostData, MMIO_CLKCTL, Data16);

  gBS->Stall (10);
  Data16 |= BIT0;
  MmcHostWrite16 (MmcHostData, MMIO_CLKCTL, Data16);

  TimeOutCount = TIME_OUT_1S;
  do {
    Data16 = MmcHostRead16 (MmcHostData, MMIO_CLKCTL);
    gBS->Stall (10);
    TimeOutCount --;
    if (TimeOutCount == 0) {
      DEBUG ((gMmcHostDebugLevel, "SetClockFrequency: Timeout\n"));
      return EFI_TIMEOUT;
    }
  } while ((Data16 & BIT1) != BIT1);

  gBS->Stall (10);
  Data16 |= BIT2;
  MmcHostWrite16 (MmcHostData, MMIO_CLKCTL, Data16);

  return EFI_SUCCESS;
}


/**
  Set bus width of the host

  @param[in]  This         Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  BusWidth     Bus width in 1, 4, 8 bits

  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetBusWidth (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                      BusWidth
  )
{
  MMCHOST_DATA                    *MmcHostData;
  UINT8                           Data;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  if ((BusWidth != 1) && (BusWidth != 4) && (BusWidth != 8)) {
    DEBUG ((gMmcHostDebugLevel, "SetBusWidth: Invalid parameter \n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((MmcHostData->MmcHostIo.HostCapability.BusWidth8 == FALSE) && (BusWidth == 8)) {
    DEBUG ((gMmcHostDebugLevel, "SetBusWidth: Invalid parameter \r\n"));
    return EFI_INVALID_PARAMETER;
  }
  Data = MmcHostRead8 (MmcHostData, MMIO_HOSTCTL);

  if (BusWidth == 8) {
    DEBUG ((gMmcHostDebugLevel, "SetBusWidth: Bus Width is 8-bit ... \r\n"));
    Data |= BIT5;
  } else if (BusWidth == 4) {
    DEBUG ((gMmcHostDebugLevel, "SetBusWidth: Bus Width is 4-bit ... \r\n"));
    Data &= ~BIT5;
    Data |= BIT1;
  } else {
    DEBUG ((gMmcHostDebugLevel, "SetBusWidth: Bus Width is 1-bit ... \r\n"));
    Data &= ~BIT5;
    Data &= ~BIT1;
  }
  MmcHostWrite8 (MmcHostData, MMIO_HOSTCTL, Data);
  DEBUG ((gMmcHostDebugLevel, "SetBusWidth: MMIO_HOSTCTL value: 0x%x  \n", MmcHostRead8 (MmcHostData, MMIO_HOSTCTL)));

  return EFI_SUCCESS;
}


/**
  Set voltage which could supported by the host.
  Support 0(Power off the host), 1.8V, 3.0V, 3.3V

  @param[in]  This                       Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Voltage                    Units in 0.1 V

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_UNSUPPORTED
  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetHostVoltage (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                      Voltage
  )
{
  MMCHOST_DATA                    *MmcHostData;
  UINT8                           Data;
  EFI_STATUS                      Status;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);
  Status = EFI_SUCCESS;

  Data = MmcHostRead8 (MmcHostData, MMIO_PWRCTL);

  if (Voltage == 0) {
    //
    //Power Off the host
    //
    Data &= ~BIT0;
  } else if (Voltage <= 18 && This->HostCapability.V18Support) {
    //
    //1.8V
    //
    Data |= (BIT1 | BIT3 | BIT0);
  } else if (Voltage > 18 &&  Voltage <= 30 && This->HostCapability.V30Support) {
    //
    //3.0V
    //
    Data |= (BIT2 | BIT3 | BIT0);
  } else if (Voltage > 30 && Voltage <= 33 && This->HostCapability.V33Support) {
    //
    //3.3V
    //
    Data |= (BIT1 | BIT2 | BIT3 | BIT0);
  } else {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  MmcHostWrite8 (MmcHostData, MMIO_PWRCTL, Data);
  gBS->Stall (10);

Exit:
  return Status;
}


/**
  Set Host High Speed

  @param[in]  This                      Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  HighSpeed                 True for High Speed Mode set, false for normal mode

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetHostSpeedMode(
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                      HighSpeed
  )
{
  EFI_STATUS                      Status;
  MMCHOST_DATA                    *MmcHostData;
  UINT8                           Data8;

  Status = EFI_SUCCESS;
  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  if (MmcHostData->IsEmmc) {
    Data8 = MmcHostRead8 (MmcHostData, MMIO_HOSTCTL);
    Data8 &= ~(BIT2);
    if (HighSpeed) {
      Data8 |= BIT2;
      DEBUG ((gMmcHostDebugLevel, "High Speed mode: Data8=0x%x \n", Data8));
    } else {
      DEBUG ((gMmcHostDebugLevel, "Normal Speed mode: Data8=0x%x \n", Data8));
    }
    MmcHostWrite8 (MmcHostData, MMIO_HOSTCTL, Data8);
    gBS->Stall (10);
    DEBUG ((gMmcHostDebugLevel, "MMIO_HOSTCTL value: 0x%x  \n", MmcHostRead8 (MmcHostData, MMIO_HOSTCTL)));
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


/**
  Set Host mode in DDR

  @param[in]  This                        Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  SetHostDdrMode              True for DDR Mode set, false returns EFI_SUCCESS

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetHostDdrMode(
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     DdrMode
  )
{
  EFI_STATUS                     Status;
  MMCHOST_DATA                   *MmcHostData;
  UINT16                         ModeSet;

  Status = EFI_SUCCESS;
  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  if (MmcHostData->IsEmmc) {
    ModeSet = MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2);
    ModeSet &= ~(BIT0 | BIT1 | BIT2);
    if (DdrMode) {
      ModeSet |= 0x0004;
      ModeSet |= BIT3;  //1.8v
      DEBUG ((gMmcHostDebugLevel, "DDR mode: Data16=0x%x \n", ModeSet));
    } else {
      if (CheckControllerVersion (MmcHostData) != 2) {
        ModeSet =  0x0;
      }
    }

    MmcHostWrite16 (MmcHostData, MMIO_HOST_CTL2, ModeSet);
    gBS->Stall (10);
    DEBUG ((gMmcHostDebugLevel, "MMIO_HOST_CTL2 value: 0x%x  \n", MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2)));
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


/**
  Set Host SDR Mode

  @param[in] This                    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in] DdrMode                 True for SDR Mode set, false for normal mode

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   A parameter was incorrect.

**/
EFI_STATUS
EFIAPI
SetHostSdrMode(
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                      SdrMode
  )
{
  MMCHOST_DATA                    *MmcHostData;
  UINT16                          ModeSet;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  if (MmcHostData->IsEmmc) {
    ModeSet = MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2);
    ModeSet &= ~(BIT0 | BIT1 | BIT2);
    if (SdrMode) {
      if (MmcHostData->MmcHostIo.HostCapability.SDR104Support) {
        ModeSet |= 3;
      } else if (MmcHostData->MmcHostIo.HostCapability.SDR50Support) {
        ModeSet |= 2;
      }
      ModeSet |= 1;
      DEBUG ((gMmcHostDebugLevel, "SDR mode: Data16=0x%x \n", ModeSet));
    } else {
      if (CheckControllerVersion (MmcHostData) != 2) {
        ModeSet =  0x0;
      }
    }

    MmcHostWrite16 (MmcHostData, MMIO_HOST_CTL2, ModeSet);
     gBS->Stall (10);
    DEBUG ((gMmcHostDebugLevel, "MMIO_HOST_CTL2 value: 0x%x  \n", MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2)));
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


/**
  Reset the host

  @param[in] This                      Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in] ResetType                 Reset data/cmd all

  @retval    EFI_SUCCESS               The function completed successfully
  @retval    EFI_TIMEOUT               The timeout time expired.

**/
EFI_STATUS
EFIAPI
ResetMmcHost (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  RESET_TYPE                 ResetType
  )
{
  MMCHOST_DATA                   *MmcHostData;
  UINT8                          Data8;
  UINT32                         Data;
  UINT16                         ErrStatus;
  UINT8                          Mask;
  UINT32                         TimeOutCount;
  UINT16                         SaveClkCtl;
  UINT16                         SavePwrCtl = 0;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);
  MmcHostData->IsEmmc = TRUE;
  Mask = 0;
  ErrStatus = 0;

  if (ResetType == Reset_Auto) {
    ErrStatus = MmcHostRead16 (MmcHostData, MMIO_ERINTSTS);
    if ((ErrStatus & 0xF) != 0) {
      //
      //Command Line
      //
      Mask |= BIT1;
    }
    if ((ErrStatus & 0x70) != 0) {
      //
      //Data Line
      //
      Mask |= BIT2;
    }
  }

  if (ResetType == Reset_DAT || ResetType == Reset_DAT_CMD) {
    Mask |= BIT2;
  }
  if (ResetType == Reset_CMD || ResetType == Reset_DAT_CMD) {
    Mask |= BIT1;
  }
  if (ResetType == Reset_All) {
    Mask = BIT0;
  }
  if (ResetType == Reset_HW) {
    SavePwrCtl = MmcHostRead16 (MmcHostData, MMIO_PWRCTL);
    DEBUG ((gMmcHostDebugLevel, "Write SavePwrCtl: %x \n", (SavePwrCtl | BIT4)));

    MmcHostWrite16 (MmcHostData, MMIO_PWRCTL, SavePwrCtl | BIT4);
    gBS->Stall (10);
    DEBUG ((gMmcHostDebugLevel, "Write SavePwrCtl: %x \n", (SavePwrCtl &(~BIT4))));
    MmcHostWrite16 (MmcHostData, MMIO_PWRCTL, SavePwrCtl &(~BIT4));
    gBS->Stall (10);
  }

  if (Mask == 0) {
    return EFI_SUCCESS;
  }

  SaveClkCtl = MmcHostRead16 (MmcHostData, MMIO_CLKCTL);

  MmcHostWrite16 (MmcHostData, MMIO_CLKCTL, 0);

  gBS->Stall (10);

  //
  // Reset the MMC host controller
  //
  MmcHostWrite8 (MmcHostData, MMIO_SWRST, Mask);

  Data = 0;
  TimeOutCount  = TIME_OUT_1S;
  do {
    gBS->Stall (10);
    TimeOutCount --;

    Data8 = MmcHostRead8 (MmcHostData, MMIO_SWRST);
    if ((Data8 & Mask) == 0) {
      break;
    }
  } while (TimeOutCount > 0);

  //
  // We now restore the MMIO_CLKCTL register which we set to 0 above.
  //
  MmcHostWrite16 (MmcHostData, MMIO_CLKCTL, SaveClkCtl);

  if (TimeOutCount == 0) {
    DEBUG ((gMmcHostDebugLevel, "ResetMMCHost: Time out \n"));
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  Enable AutoStop Cmd

  @param[in] This                Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in] Enable              TRUE to enable

  @retval    EFI_SUCCESS         The function completed successfully

**/
EFI_STATUS
EFIAPI
EnableAutoStopCmd (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  BOOLEAN                     Enable
  )
{
  MMCHOST_DATA                    *MmcHostData;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  MmcHostData->IsAutoStopCmd = Enable;

  return EFI_SUCCESS;
}


/**
  Set the Block length

  @param[in] This           Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in] BlockLength    card supportes block length

  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetBlockLength (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     BlockLength
  )
{
  MMCHOST_DATA                   *MmcHostData;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  DEBUG ((gMmcHostDebugLevel, "SetBlockLength: Block length on the host controller: %d \n", BlockLength));
  MmcHostData->BlockLength = BlockLength;

  return EFI_SUCCESS;
}


/**
  Detect card and init Sd host
  Find whether these is a card inserted into the slot. If so
  init the host. If not, return EFI_NOT_FOUND.

  @param[in] This                    Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval    EFI_INVALID_PARAMETER
  @retval    EFI_UNSUPPORTED
  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
DetectCardAndInitHost (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This
  )
{
  MMCHOST_DATA                   *MmcHostData;
  UINT16                         Data16;
  UINT32                         Data;
  EFI_STATUS                     Status;
  UINT8                          Voltages[] = { 33, 30, 18 };
  UINTN                          Loop;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  Data = 0;
  Data = MmcHostRead32 (MmcHostData, MMIO_PSTATE);

  if ((Data & (BIT16 | BIT17 | BIT18)) != (BIT16 | BIT17 | BIT18)) {
    //
    // Has no card inserted
    //
    DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: No card\n"));
    Status =  EFI_NOT_FOUND;
    goto Exit;
  }
  DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: Card present\n"));

  //
  //Enable normal status change
  //
  MmcHostWrite16 (MmcHostData, MMIO_NINTEN, BIT1 | BIT0);

  //
  // Enable error status change
  //
  Data16 = MmcHostRead16 (MmcHostData, MMIO_ERINTEN);
  Data16 |= 0xFFFF; //(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8);
  MmcHostWrite16 (MmcHostData, MMIO_ERINTEN, Data16);

  //
  // Data transfer Timeout control
  //
  MmcHostWrite8 (MmcHostData, MMIO_TOCTL, 0x0E);

  //
  // Stall 1 milliseconds to increase MMC stability.
  //
  gBS->Stall (10);

  Status = SetClockFrequency (This, FREQUENCY_OD);
  if (EFI_ERROR (Status)) {
    DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: SetClockFrequency failed\n"));
    goto Exit;
  }
  DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: SetClockFrequency done\n"));

  Status =  EFI_NOT_FOUND;
  for (Loop = 0; Loop < sizeof (Voltages); Loop++) {
    DEBUG ((
      EFI_D_INFO,
      "DetectCardAndInitHost: SetHostVoltage %d.%dV\n",
      Voltages[Loop] / 10,
      Voltages[Loop] % 10
      ));
    Status = SetHostVoltage (This, Voltages[Loop]);
    if (EFI_ERROR (Status)) {
      DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: SetHostVoltage failed\n"));
    } else {
      DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: SetHostVoltage done\n"));
      break;
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((gMmcHostDebugLevel, "DetectCardAndInitHost: Failed to SetHostVoltage\n"));
    goto Exit;
  }

Exit:
  return Status;
}


/**
  Setup the MMC Host Device

  @param[in] This            Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetupDevice (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This
  )
{
  UINT32                          tempD = 0;
  INT32                           timeOut = 1000;
  UINT8                           temp8 = 0;
  UINT16                          temp16 = 0;
  MMCHOST_DATA                    *MmcHostData;

  MmcHostData = MMCHOST_DATA_FROM_THIS (This);

  //
  // Reset HC and wait for self-clear
  //
  MmcHostWrite8 (MmcHostData, MMIO_SWRST, 0x7);
  gBS->Stall (1000);
  timeOut = 1000;
  do {
    temp8 = MmcHostRead8 (MmcHostData, MMIO_SWRST);
    timeOut--;
  } while ((temp8 & (1 << 0)) && (timeOut > 0));

  DEBUG ((EFI_D_INFO, "Reset HC and wait for self-clear Done\n"));

  //
  // Enable all interrupt status bits (NO CARD_INTERRUPT!)
  //
  MmcHostWrite16 (MmcHostData, MMIO_NINTEN, 0x3);

  //
  // Clear all interrupt status bits
  //
  MmcHostWrite16 (MmcHostData, MMIO_NINTSTS, 0xFFFF);
  MmcHostWrite16 (MmcHostData, MMIO_ERINTEN, 0xFFFF);

  if (2 == CheckControllerVersion (MmcHostData))
    temp16 = (UINT16) (0x1 << 6);
  else
    temp16 = (UINT16) (0x80 << 8);
  //
  // Set to 400KB, enable internal clock and wait for stability
  //
  MmcHostWrite32 (MmcHostData, MMIO_CLKCTL, (1<<0) | temp16);

  gBS->Stall (1000);
  do {
    tempD = MmcHostRead32 (MmcHostData, MMIO_CLKCTL);
    timeOut--;
  } while ((!(tempD & (1 << 1))) && (timeOut > 0));
  gBS->Stall (1000);

  //
  // Enable MMC clock
  //
  tempD |= (1 << 2);
  MmcHostWrite32 (MmcHostData, MMIO_CLKCTL, tempD);
  gBS->Stall (1000);

  temp8 = MmcHostRead8 (MmcHostData, MMIO_PWRCTL);
  DEBUG ((DEBUG_INFO, "==========%a, %d, offset=0x%x, PWRCTL = 0x%x================\n", __FUNCTION__, __LINE__, MMIO_PWRCTL, temp8));

  //
  // Apply 1.8V to the bus
  //
  temp8 = (0x5 << (1) );
  MmcHostWrite8 (MmcHostData, MMIO_PWRCTL, temp8);
  gBS->Stall (1000);
  temp8 = MmcHostRead8 (MmcHostData, MMIO_PWRCTL);
  DEBUG ((DEBUG_INFO, "==========%a, %d, set (0x5 << 1):offset=0x%x, PWRCTL = 0x%x================\n", __FUNCTION__, __LINE__, MMIO_PWRCTL, temp8));

  //
  // Set 1.8V sigaling Enabled
  //
  temp16 = MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2);
  temp16 &= ~BIT3;
  MmcHostWrite16 (MmcHostData, MMIO_HOST_CTL2, temp16);
  DEBUG ((gMmcHostDebugLevel, "Set 1.8 V signaling Enable:0x%x \r\n", MmcHostRead16 (MmcHostData, MMIO_HOST_CTL2)));

  //
  // Apply power to MMC
  //
  temp8 = MmcHostRead8 (MmcHostData, MMIO_PWRCTL);
  DEBUG ((DEBUG_INFO, "==========%a, %d, read offset=0x%x, PWRCTL = 0x%x================\n", __FUNCTION__, __LINE__, MMIO_PWRCTL, temp8));

  temp8 |= (1 << (0));
  DEBUG ((DEBUG_INFO, "==========%a, %d, set 1<<0:offset=0x%x, PWRCTL = 0x%x================\n", __FUNCTION__, __LINE__, MMIO_PWRCTL, temp8));

  MmcHostWrite8 (MmcHostData, MMIO_PWRCTL, temp8);
  gBS->Stall (1000);              ///<synced to byt-cr pei. bxt power on used (50 * 1000)
  temp8 = MmcHostRead8 (MmcHostData, MMIO_PWRCTL);

  DEBUG ((DEBUG_INFO, "==========%a, %d, read offset=0x%x, PWRCTL = 0x%x================\n", __FUNCTION__, __LINE__, MMIO_PWRCTL, temp8));

  //
  // MAX out the DATA_TIMEOUT
  //
  MmcHostWrite8 (MmcHostData, MMIO_TOCTL, 0xE);
  gBS->Stall (1000);

  if (MmcHostData->EnableVerboseDebug) {
    DEBUG ((EFI_D_INFO, "==========%a, Start. RegMap================\n", __FUNCTION__));
    DEBUG ((DEBUG_INFO, "00 -10:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x00), MmcHostRead32 (MmcHostData, 0x04), MmcHostRead32 (MmcHostData, 0x08), MmcHostRead32 (MmcHostData, 0x0C)));
    DEBUG ((DEBUG_INFO, "10 -20:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x10), MmcHostRead32 (MmcHostData, 0x14), MmcHostRead32 (MmcHostData, 0x18), MmcHostRead32 (MmcHostData, 0x1C)));
    DEBUG ((DEBUG_INFO, "20 -30:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x20), MmcHostRead32 (MmcHostData, 0x24), MmcHostRead32 (MmcHostData, 0x28), MmcHostRead32 (MmcHostData, 0x2C)));
    DEBUG ((DEBUG_INFO, "30 -40:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x30), MmcHostRead32 (MmcHostData, 0x34), MmcHostRead32 (MmcHostData, 0x38), MmcHostRead32 (MmcHostData, 0x3C)));
    DEBUG ((DEBUG_INFO, "40 -50:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x40), MmcHostRead32 (MmcHostData, 0x44), MmcHostRead32 (MmcHostData, 0x48), MmcHostRead32 (MmcHostData, 0x4C)));
    DEBUG ((DEBUG_INFO, "50 -60:  %08x  %08x  %08x  %08x \n", MmcHostRead32 (MmcHostData, 0x50), MmcHostRead32 (MmcHostData, 0x54), MmcHostRead32 (MmcHostData, 0x58), MmcHostRead32 (MmcHostData, 0x5C)));
    DEBUG ((DEBUG_INFO, "==========%a, END. RegMap================\n", __FUNCTION__));
  }
  return EFI_SUCCESS;
}

