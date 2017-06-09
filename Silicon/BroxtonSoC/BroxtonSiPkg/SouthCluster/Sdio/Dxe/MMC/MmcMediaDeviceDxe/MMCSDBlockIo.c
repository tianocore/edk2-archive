/** @file
  Block I/O protocol for MMC/SD device.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <MediaDeviceDriver.h>


/**
  Implements EFI_BLOCK_IO_PROTOCOL.Reset() function.

  @param[in]  This                    The EFI_BLOCK_IO_PROTOCOL instance.
  @param[in]  ExtendedVerification    Indicates that the driver may perform a more
                                      exhaustive verification operation of the device during reset.
                                      (This parameter is ingored in this driver.)

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_UNSUPPORTED
  @retval     EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
MMCSDBlockReset (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  BOOLEAN                 ExtendedVerification
  )
{
  CARD_DATA                   *CardData;
  EFI_MMC_HOST_IO_PROTOCOL    *MmcHostIo;

  CardData  = CARD_DATA_FROM_THIS (This);
  MmcHostIo = CardData->MmcHostIo;

  DEBUG ((EFI_D_INFO, "MMC SD Block: Resetting host\n"));

  return MmcHostIo->ResetMmcHost (MmcHostIo, Reset_DAT_CMD);
}


/**
  Implements EFI_BLOCK_IO_PROTOCOL.ReadBlocks() function.

  @param[in]   This                    The EFI_BLOCK_IO_PROTOCOL instance.
  @param[in]   MediaId                 The media id that the read request is for.
  @param[in]   LBA                     The starting logical block address to read from on the device.
  @param[in]   BufferSiz               The size of the Buffer in bytes. This must be a multiple of
                                       the intrinsic block size of the device.
  @param[out]  Buffer                  A pointer to the destination buffer for the data. The caller
                                       is responsible for either having implicit or explicit ownership
                                       of the buffer.

  @retval      EFI_INVALID_PARAMETER   Parameter is error
  @retval      EFI_SUCCESS             Success
  @retval      EFI_DEVICE_ERROR        Hardware Error
  @retval      EFI_NO_MEDIA            No media
  @retval      EFI_MEDIA_CHANGED       Media Change
  @retval      EFI_BAD_BUFFER_SIZE     Buffer size is bad

**/
EFI_STATUS
EFIAPI
MMCSDBlockReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 LBA,
  IN  UINTN                   BufferSize,
  OUT VOID                    *Buffer
  )
{
  EFI_STATUS                  Status;
  UINT32                      Address;
  CARD_DATA                   *CardData;
  EFI_MMC_HOST_IO_PROTOCOL     *MmcHostIo;
  UINT32                      RemainingLength;
  UINT32                      TransferLength;
  UINT8                       *BufferPointer;
  BOOLEAN                     SectorAddressing;
  UINT64                      CardSize;
  MMC_PARTITION_DATA          *Partition;
  UINTN                       TotalBlock;

  Status   = EFI_SUCCESS;
  Partition = CARD_PARTITION_DATA_FROM_THIS (This);
  CardData  = Partition->CardData;
  MmcHostIo = CardData->MmcHostIo;

  //
  // Media ID has high priority that need to be verify first
  //
  if (MediaId != Partition->BlockIoMedia.MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  Status = MmcSelectPartition (Partition);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // BufferSize must be a multiple of the intrinsic block size of the device.
  //
  if (ModU64x32 (BufferSize,Partition->BlockIoMedia.BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  CardSize = MultU64x32 (CardData->BlockNumber, CardData->BlockLen);

  if ((CardData->CardType == SdMemoryCard2High) || (CardSize >= SIZE_2GB)) {
    SectorAddressing = TRUE;
  } else {
    SectorAddressing = FALSE;
  }

  if (SectorAddressing) {
    //
    // Sector Address
    //
    Address = (UINT32) DivU64x32 (MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize), 512);
  } else {
    //
    // Byte Address
    //
    Address  = (UINT32) MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize);
  }

  TotalBlock = (UINTN) DivU64x32 (BufferSize, Partition->BlockIoMedia.BlockSize);
  //
  // Make sure the range to read is valid.
  //
  if (LBA + TotalBlock > Partition->BlockIoMedia.LastBlock + 1) {
    return EFI_INVALID_PARAMETER;
  }

  if (!(Partition->BlockIoMedia.MediaPresent)) {
    return EFI_NO_MEDIA;
  }

  if (!Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: Invalid parameter \n"));
    goto Done;
  }

  if ((BufferSize % Partition->BlockIoMedia.BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: Bad buffer size \n"));
    goto Done;
  }

  if (BufferSize == 0) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  BufferPointer   = Buffer;
  RemainingLength = (UINT32) BufferSize;

  while (RemainingLength > 0) {
    if ((BufferSize >= Partition->BlockIoMedia.BlockSize)) {
      if (RemainingLength > MmcHostIo->HostCapability.BoundarySize) {
        TransferLength = MmcHostIo->HostCapability.BoundarySize;
      } else {
        TransferLength = RemainingLength;
      }

      if (CardData->CardType == MMCCard) {
        if (!(CardData->ExtCSDRegister.CARD_TYPE & (BIT2 | BIT3))) {
          Status = SendCommand (
                     MmcHostIo,
                     SET_BLOCKLEN,
                     Partition->BlockIoMedia.BlockSize,
                     NoData,
                     NULL,
                     0,
                     ResponseR1,
                     TIMEOUT_COMMAND,
                     (UINT32 *) &(CardData->CardStatus)
                     );
          if (EFI_ERROR (Status)) {
            break;
          }
        }

        Status = SendCommand (
                   MmcHostIo,
                   SET_BLOCK_COUNT,
                   TransferLength / Partition->BlockIoMedia.BlockSize,
                   NoData,
                   NULL,
                   0,
                   ResponseR1,
                   TIMEOUT_COMMAND,
                   (UINT32 *) &(CardData->CardStatus)
                   );
        if (EFI_ERROR (Status)) {
          break;
        }
      }

      Status = SendCommand (
                 MmcHostIo,
                 READ_MULTIPLE_BLOCK,
                 Address,
                 InData,
                 CardData->AlignedBuffer,
                 TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32 *) &(CardData->CardStatus)
                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: READ_MULTIPLE_BLOCK -> Fail\n"));
        break;
      }
    } else {
      if (RemainingLength > Partition->BlockIoMedia.BlockSize) {
        TransferLength = Partition->BlockIoMedia.BlockSize;
      } else {
        TransferLength = RemainingLength;
      }

      Status = SendCommand (
                 MmcHostIo,
                 READ_SINGLE_BLOCK,
                 Address,
                 InData,
                 CardData->AlignedBuffer,
                 (UINT32) TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32*) &(CardData->CardStatus)
                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: READ_SINGLE_BLOCK -> Fail\n"));
        break;
      }
    }

    CopyMem (BufferPointer, CardData->AlignedBuffer, TransferLength);

    if (SectorAddressing) {
      //
      // Sector Address
      //
      Address += TransferLength / 512;
    } else {
      //
      // Byte Address
      //
      Address += TransferLength;
    }
    BufferPointer   += TransferLength;
    RemainingLength -= TransferLength;
  }

  if (EFI_ERROR (Status)) {
    if ((CardData->CardType == SdMemoryCard) ||
        (CardData->CardType == SdMemoryCard2)||
        (CardData->CardType == SdMemoryCard2High)) {
      SendCommand (
        MmcHostIo,
        STOP_TRANSMISSION,
        0,
        NoData,
        NULL,
        0,
        ResponseR1b,
        TIMEOUT_COMMAND,
        (UINT32 *) &(CardData->CardStatus)
        );
    } else {
      SendCommand (
        MmcHostIo,
        STOP_TRANSMISSION,
        0,
        NoData,
        NULL,
        0,
        ResponseR1,
        TIMEOUT_COMMAND,
        (UINT32 *) &(CardData->CardStatus)
        );
    }
  }

Done:
  return Status;
}


/**
  Implements EFI_BLOCK_IO_PROTOCOL.WriteBlocks() function.

  @param[in]  This                    The EFI_BLOCK_IO_PROTOCOL instance.
  @param[in]  MediaId                 The media id that the write request is for.
  @param[in]  LBA                     The starting logical block address to be written.
                                      The caller is responsible for writing to only
                                      legitimate locations.
  @param[in]  BufferSize
                                      The size of the Buffer in bytes. This must be a multiple of
                                      the intrinsic block size of the device.
  @param[in]  Buffer                  A pointer to the source buffer for the data. The caller
                                      is responsible for either having implicit or explicit ownership
                                      of the buffer.

  @retval     EFI_INVALID_PARAMETER   Parameter is error
  @retval     EFI_SUCCESS             Success
  @retval     EFI_DEVICE_ERROR        Hardware Error
  @retval     EFI_NO_MEDIA            No media
  @retval     EFI_MEDIA_CHANGED       Media Change
  @retval     EFI_BAD_BUFFER_SIZE     Buffer size is bad

**/
EFI_STATUS
EFIAPI
MMCSDBlockWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 LBA,
  IN  UINTN                   BufferSize,
  IN  VOID                    *Buffer
  )
{
  EFI_STATUS                  Status;
  UINT32                      Address;
  CARD_DATA                   *CardData;
  EFI_MMC_HOST_IO_PROTOCOL    *MmcHostIo;
  UINT32                      RemainingLength;
  UINT32                      TransferLength;
  UINT8                       *BufferPointer;
  BOOLEAN                     SectorAddressing;
  UINT64                      CardSize;
  MMC_PARTITION_DATA          *Partition;

  Status   = EFI_SUCCESS;
  Partition = CARD_PARTITION_DATA_FROM_THIS (This);
  CardData  = Partition->CardData;
  MmcHostIo = CardData->MmcHostIo;

  Status = MmcSelectPartition (Partition);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CardSize = MultU64x32 (CardData->BlockNumber, CardData->BlockLen);

  if ((CardData->CardType == SdMemoryCard2High) || (CardSize >= SIZE_2GB)) {
    SectorAddressing = TRUE;
  } else {
    SectorAddressing = FALSE;
  }

  if (SectorAddressing) {
    //
    // Sector Address
    //
    Address = (UINT32) DivU64x32 (MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize), 512);
  } else {
    //
    // Byte Address
    //
    Address = (UINT32) MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize);
  }

  if (!Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: Invalid parameter \n"));
    goto Done;
  }

  if ((BufferSize % Partition->BlockIoMedia.BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: Bad buffer size \n"));
    goto Done;
  }

  if (BufferSize == 0) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  if (This->Media->ReadOnly == TRUE) {
    Status = EFI_WRITE_PROTECTED;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: Write protected \n"));
    goto Done;
  }

  BufferPointer   = Buffer;
  RemainingLength = (UINT32) BufferSize;

  while (RemainingLength > 0) {
    if ((BufferSize >= Partition->BlockIoMedia.BlockSize) ) {
      if (RemainingLength > MmcHostIo->HostCapability.BoundarySize) {
        TransferLength = MmcHostIo->HostCapability.BoundarySize;
      } else {
        TransferLength = RemainingLength;
      }

      if ((CardData->CardType == SdMemoryCard) ||
          (CardData->CardType == SdMemoryCard2)||
          (CardData->CardType == SdMemoryCard2High)) {
        //
        // Write performance improvement
        //
        if ((TransferLength / Partition->BlockIoMedia.BlockSize) > 64) {
          Status  = SendAppCommand (
                      CardData,
                      SET_WR_BLK_ERASE_COUNT,
                      (UINT32) (TransferLength / Partition->BlockIoMedia.BlockSize),
                      NoData,
                      NULL,
                      0,
                      ResponseR1,
                      TIMEOUT_COMMAND,
                      (UINT32 *) &(CardData->CardStatus)
                      );
        }
      } else if (CardData->CardType == MMCCard) {

        if (!(CardData->ExtCSDRegister.CARD_TYPE & (BIT2 | BIT3)))  {
          Status = SendCommand (
                     MmcHostIo,
                     SET_BLOCKLEN,
                     Partition->BlockIoMedia.BlockSize,
                     NoData,
                     NULL,
                     0,
                     ResponseR1,
                     TIMEOUT_COMMAND,
                     (UINT32 *) &(CardData->CardStatus)
                     );

          if (EFI_ERROR (Status)) {
            break;
          }
        }
        Status = SendCommand (
                   MmcHostIo,
                   SET_BLOCK_COUNT,
                   TransferLength / Partition->BlockIoMedia.BlockSize,
                   NoData,
                   NULL,
                   0,
                   ResponseR1,
                   TIMEOUT_COMMAND,
                   (UINT32 *) &(CardData->CardStatus)
                   );
        if (EFI_ERROR (Status)) {
          break;
        }
      }

      CopyMem (CardData->AlignedBuffer, BufferPointer, TransferLength);

      Status = SendCommand (
                 MmcHostIo,
                 WRITE_MULTIPLE_BLOCK,
                 Address,
                 OutData,
                 CardData->AlignedBuffer,
                 (UINT32) TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32 *) &(CardData->CardStatus)
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: WRITE_MULTIPLE_BLOCK -> Fail\n"));
        break;
      }
    } else {
      if (RemainingLength > Partition->BlockIoMedia.BlockSize) {
        TransferLength = Partition->BlockIoMedia.BlockSize;
      } else {
        TransferLength = RemainingLength;
      }

      CopyMem (CardData->AlignedBuffer, BufferPointer, TransferLength);

      Status = SendCommand (
                 MmcHostIo,
                 WRITE_BLOCK,
                 Address,
                 OutData,
                 CardData->AlignedBuffer,
                 (UINT32) TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32 *) &(CardData->CardStatus)
                 );
    }

    if (SectorAddressing) {
      //
      // Sector Address
      //
      Address += TransferLength / 512;
    } else {
      //
      // Byte Address
      //
      Address += TransferLength;
    }
    BufferPointer   += TransferLength;
    RemainingLength -= TransferLength;
  }

  if (EFI_ERROR (Status)) {
    SendCommand (
      MmcHostIo,
      STOP_TRANSMISSION,
      0,
      NoData,
      NULL,
      0,
      ResponseR1b,
      TIMEOUT_COMMAND,
      (UINT32 *) &(CardData->CardStatus)
      );
  }

Done:
  return Status;
}


/**
  Implements EFI_BLOCK_IO_PROTOCOL.FlushBlocks() function.
  (In this driver, this function just returns EFI_SUCCESS.)

  @param[in]  This          The EFI_BLOCK_IO_PROTOCOL instance.

  @retval     EFI_SUCCESS   Success

**/
EFI_STATUS
EFIAPI
MMCSDBlockFlushBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This
  )
{
  return EFI_SUCCESS;
}


/**
  MMC/SD card BlockIo init function

  @param[in]  CardData       Pointer to CARD_DATA

  @retval     EFI_SUCCESS    Success

**/
EFI_STATUS
MMCSDBlockIoInit (
  IN  CARD_DATA       *CardData
  )
{
  UINTN               Loop;
  MMC_PARTITION_DATA  *Partition;
  EXT_CSD             *ExtCsd;
  UINT64              GP_CHUNK_SIZE;
  UINT32              GP_SIZE_MULT;
  UINT64              GppSize;
  UINTN               GppIndex=0;

  Partition = CardData->Partitions;
  ExtCsd = &CardData->ExtCSDRegister;

  //
  // Determine GP partitioning chunk size
  //
  GP_CHUNK_SIZE = 0;
  if (((ExtCsd->PARTITIONING_SUPPORT & BIT0) == BIT0) &&
      ((ExtCsd->PARTITION_SETTING_COMPLETED & BIT0) == BIT0)) {
    GP_CHUNK_SIZE = MultU64x32 (ExtCsd->HC_WP_GRP_SIZE, ExtCsd->HC_ERASE_GRP_SIZE);
    GP_CHUNK_SIZE = MultU64x32 (GP_CHUNK_SIZE, SIZE_512KB);
  }

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Partition++, Loop++) {
    //
    // BlockIO protocol
    //
    Partition->BlockIo.Revision    = EFI_BLOCK_IO_PROTOCOL_REVISION;
    Partition->BlockIo.Media       = &(Partition->BlockIoMedia);
    Partition->BlockIo.Reset       = MMCSDBlockReset;
    Partition->BlockIo.ReadBlocks  = MMCSDBlockReadBlocks ;
    Partition->BlockIo.WriteBlocks = MMCSDBlockWriteBlocks;
    Partition->BlockIo.FlushBlocks = MMCSDBlockFlushBlocks;

    Partition->BlockIoMedia.MediaId          = 0;
    Partition->BlockIoMedia.RemovableMedia   = FALSE;
    Partition->BlockIoMedia.MediaPresent     = TRUE;
    Partition->BlockIoMedia.LogicalPartition = FALSE;

    //
    // Force the User partition to be enabled
    //
    if (Loop == 0) {
      Partition->Present = TRUE;
    }

    if (CardData->CSDRegister.PERM_WRITE_PROTECT || CardData->CSDRegister.TMP_WRITE_PROTECT) {
      Partition->BlockIoMedia.ReadOnly       = TRUE;
    } else {
      Partition->BlockIoMedia.ReadOnly       = FALSE;
    }

    Partition->BlockIoMedia.WriteCaching     = FALSE;
    Partition->BlockIoMedia.BlockSize        = CardData->BlockLen;
    Partition->BlockIoMedia.IoAlign          = 1;
    Partition->BlockIoMedia.LastBlock        = (EFI_LBA) (CardData->BlockNumber - 1);

    //
    // Handle GPP partitions
    //
    GppSize = 0;
    if ((GP_CHUNK_SIZE != 0) && (Loop >= 4)) {
      Partition->BlockIoMedia.LastBlock = (EFI_LBA) 0;
      GppIndex = Loop - 4;
      GP_SIZE_MULT = MmcGetExtCsd24 (
                       CardData,
                       OFFSET_OF (EXT_CSD, GP_SIZE_MULT_1) + (3 * GppIndex)
                       );
      GppSize = MultU64x32 (GP_SIZE_MULT, (UINT32) GP_CHUNK_SIZE);
    }

    if (GppSize != 0) {
      Partition->BlockIoMedia.LastBlock =
        DivU64x32 (GppSize, Partition->BlockIoMedia.BlockSize) - 1;
      DEBUG ((EFI_D_INFO,
        "GPP%d last-block: 0x%lx\n",
        GppIndex + 1,
        Partition->BlockIoMedia.LastBlock
        ));
      Partition->Present = TRUE;
    }

    if (CardData->CardType == MMCCard) {
      //
      // Handle Boot partitions
      //
      if ((Loop == 1) || (Loop == 2)) {
        Partition->BlockIoMedia.LastBlock = 128 * 1024 * ((UINTN) MmcGetExtCsd8 (CardData, OFFSET_OF (EXT_CSD, BOOT_SIZE_MULTI))) / 512;
        Partition->Present = TRUE;
      }
    }
  }

  DEBUG ((EFI_D_INFO, "MMC SD Block I/O: Initialized\n"));

  return EFI_SUCCESS;
}

