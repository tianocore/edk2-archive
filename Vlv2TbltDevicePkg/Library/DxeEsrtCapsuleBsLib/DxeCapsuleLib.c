/** @file
  Capsule Boottime Library instance to update capsule image to flash.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/         


#include <PiDxe.h>

#include <IndustryStandard/Bmp.h>
#include <IndustryStandard/WindowsUxCapsule.h>

#include <Guid/FmpCapsule.h>
#include <Guid/Capsule.h>
#include <Guid/SystemResourceTable.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CapsuleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootManagerLib.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/EsrtManagement.h>
#include <Protocol/FirmwareManagement.h>
#include <Protocol/DevicePath.h>


/**
  Function indicate the current completion progress of the firmware
  update. Platform may override with own specific progress function.

  @param  Completion    A value between 1 and 100 indicating the current completion progress of the firmware update

  @retval EFI_SUCESS    Input capsule is a correct FMP capsule.
**/
EFI_STATUS
EFIAPI
Update_Image_Progress (
   IN UINTN Completion
)
{
  return EFI_SUCCESS;
}

/**
  Validate Fmp capsules layout.

  @param  CapsuleHeader    Points to a capsule header.

  @retval EFI_SUCESS                     Input capsule is a correct FMP capsule.
  @retval EFI_INVALID_PARAMETER  Input capsule is not a correct FMP capsule.
**/
EFI_STATUS
ValidateFmpCapsule (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER       *FmpCapsuleHeader;
  UINT8                                        *EndOfCapsule;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *ImageHeader;
  UINT8                                        *EndOfPayload;
  UINT64                                       *ItemOffsetList;
  UINT32                                       ItemNum;
  UINTN                                        Index;

  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *) ((UINT8 *) CapsuleHeader + CapsuleHeader->HeaderSize);
  EndOfCapsule     = (UINT8 *) CapsuleHeader + CapsuleHeader->CapsuleImageSize;

  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    return EFI_INVALID_PARAMETER;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);

  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;

  if (ItemNum == FmpCapsuleHeader->EmbeddedDriverCount) {
    //
    // No payload element 
    //
    if (((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]) < EndOfCapsule) {
      return EFI_SUCCESS;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }

  if (FmpCapsuleHeader->PayloadItemCount != 0) {
    //
    // Check if the last payload is within capsule image range
    //
    ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]);
    EndOfPayload = (UINT8 *)(ImageHeader + 1) + ImageHeader->UpdateImageSize + ImageHeader->UpdateVendorCodeSize;
  } else {
    //
    // No driver & payload element in FMP
    //
    EndOfPayload = (UINT8 *)(FmpCapsuleHeader + 1);
  }

  if (EndOfPayload != EndOfCapsule) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // All the address in ItemOffsetList must be stored in ascending order
  //
  if (ItemNum >= 2) {
    for (Index = 0; Index < ItemNum - 1; Index++) {
      if (ItemOffsetList[Index] >= ItemOffsetList[Index + 1]) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  BmpImageSize  Number of bytes in BmpImage
  @param  GopBlt        Buffer containing GOP version of BmpImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
  @param  PixelWidth    Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
static
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  BOOLEAN                       IsAllocated;

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  //
  // Doesn't support compress.
  //
  if (BmpHeader->CompressionType != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
      return EFI_UNSUPPORTED;
  }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *PixelWidth   = BmpHeader->PixelWidth;
  *PixelHeight  = BmpHeader->PixelHeight;

  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;
  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      case 32:
        //
        // it is 32-bit BMP. Skip pixel's highest byte 
        //
        Blt->Blue  = *Image++;
        Blt->Green = *Image++;
        Blt->Red   = *Image++;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        if (IsAllocated) {
          FreePool (*GopBlt);
          *GopBlt = NULL;
        }
        return EFI_UNSUPPORTED;
        break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}


/**
  Those capsules supported by the firmwares.

  @param  CapsuleHeader    Points to a capsule header.

  @retval EFI_SUCESS       Input capsule is supported by firmware.
  @retval EFI_UNSUPPORTED  Input capsule is not supported by the firmware.
**/
EFI_STATUS
DisplayCapsuleImage (
  IN DISPLAY_DISPLAY_PAYLOAD  *ImagePayload,
  IN UINTN                    PayloadSize
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINTN                         BltSize;
  UINTN                         Height;
  UINTN                         Width;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;

  //
  // Only Support Bitmap by now
  //
  if (ImagePayload->ImageType != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Try to open GOP 
  //
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (GraphicsOutput->Mode->Mode != ImagePayload->Mode) {
    return EFI_UNSUPPORTED;
  }

  Blt = NULL;
  Width = 0;
  Height = 0;
  Status = ConvertBmpToGopBlt (
             ImagePayload + 1,
             PayloadSize - sizeof(DISPLAY_DISPLAY_PAYLOAD),
             (VOID **)&Blt,
             &BltSize,
             &Height,
             &Width
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             Blt,
                             EfiBltBufferToVideo,
                             0,
                             0,
                             (UINTN) ImagePayload->OffsetX,
                             (UINTN) ImagePayload->OffsetY,
                             Width,
                             Height,
                             Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                             );

  FreePool(Blt);

  return Status;
}

/**
  Process Firmware management protocol data capsule.  

  @param  CapsuleHeader         Points to a capsule header.

  @retval EFI_SUCESS            Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
ProcessFmpCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                                    Status;
  EFI_STATUS                                    StatusEsrt;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  UINT8                                         *EndOfCapsule;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  UINT8                                         *Image;
  EFI_HANDLE                                    ImageHandle;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINTN                                         ExitDataSize;
  EFI_HANDLE                                    *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL              *Fmp;
  UINTN                                         NumberOfHandles;
  UINTN                                         DescriptorSize;
  UINT8                                         FmpImageInfoCount;
  UINT32                                        FmpImageInfoDescriptorVer;
  UINTN                                         ImageInfoSize;
  UINT32                                        PackageVersion;
  CHAR16                                        *PackageVersionName;
  CHAR16                                        *AbortReason;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *FmpImageInfoBuf;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *TempFmpImageInfo;
  UINTN                                         DriverLen;
  UINTN                                         Index1;
  UINTN                                         Index2;
  MEMMAP_DEVICE_PATH                            MemMapNode;
  EFI_DEVICE_PATH_PROTOCOL                      *DriverDevicePath;
  ESRT_MANAGEMENT_PROTOCOL                      *EsrtProtocol;
  EFI_SYSTEM_RESOURCE_ENTRY                     EsrtEntry;

  Status           = EFI_SUCCESS;
  HandleBuffer     = NULL;
  ExitDataSize     = 0;
  DriverDevicePath = NULL;
  EsrtProtocol     = NULL;

  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER *) ((UINT8 *) CapsuleHeader + CapsuleHeader->HeaderSize);
  EndOfCapsule     = (UINT8 *) CapsuleHeader + CapsuleHeader->CapsuleImageSize;

  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    return EFI_INVALID_PARAMETER;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);

  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;

  //
  // capsule in which driver count and payload count are both zero is not processed.
  //
  if (ItemNum == 0) {
    return EFI_SUCCESS;
  }
  
  //
  // Update corresponding ESRT entry LastAttemp Status 
  //
  Status = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtProtocol);

  //
  // 1. ConnectAll to ensure 
  //    All the communication protocol required by driver in capsule installed 
  //    All FMP protocols are installed
  //
  EfiBootManagerConnectAll();


  //
  // 2. Try to load & start all the drivers within capsule 
  //
  SetDevicePathNodeLength (&MemMapNode.Header, sizeof (MemMapNode));
  MemMapNode.Header.Type     = HARDWARE_DEVICE_PATH;
  MemMapNode.Header.SubType  = HW_MEMMAP_DP;
  MemMapNode.MemoryType      = EfiBootServicesCode;
  MemMapNode.StartingAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)CapsuleHeader;
  MemMapNode.EndingAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)((UINT8 *)CapsuleHeader + CapsuleHeader->CapsuleImageSize - 1);

  DriverDevicePath = AppendDevicePathNode (NULL, &MemMapNode.Header);
  if (DriverDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < FmpCapsuleHeader->EmbeddedDriverCount; Index++) {
    if (FmpCapsuleHeader->PayloadItemCount == 0 && Index == (UINTN)FmpCapsuleHeader->EmbeddedDriverCount - 1) {
      //
      // When driver is last element in the ItemOffsetList array, the driver size is calculated by reference CapsuleImageSize in EFI_CAPSULE_HEADER
      //
      DriverLen = CapsuleHeader->CapsuleImageSize - CapsuleHeader->HeaderSize - (UINTN)ItemOffsetList[Index];
    } else {
      DriverLen = (UINTN)ItemOffsetList[Index + 1] - (UINTN)ItemOffsetList[Index];
    }

    Status = gBS->LoadImage(
                    FALSE,
                    gImageHandle,
                    DriverDevicePath,
                    (UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index],
                    DriverLen,
                    &ImageHandle
                    );
    if (EFI_ERROR(Status)) {
      goto EXIT;
    }

    Status = gBS->StartImage(
                    ImageHandle, 
                    &ExitDataSize, 
                    NULL
                    );
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Driver Return Status = %r\n", Status));
      goto EXIT;
    }
  }

  //
  // Connnect all again to connect drivers within capsule 
  //
  if (FmpCapsuleHeader->EmbeddedDriverCount > 0) {
    EfiBootManagerConnectAll();
  }

  //
  // 3. Route payload to right FMP instance
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  if (!EFI_ERROR(Status)) {
    for(Index1 = 0; Index1 < NumberOfHandles; Index1++) {
      Status = gBS->HandleProtocol(
                      HandleBuffer[Index1],
                      &gEfiFirmwareManagementProtocolGuid,
                      (VOID **)&Fmp
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }

      ImageInfoSize = 0;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
                      );
      if (Status != EFI_BUFFER_TOO_SMALL) {
        continue;
      }

      FmpImageInfoBuf = NULL;
      FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
      if (FmpImageInfoBuf == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
      }

      PackageVersionName = NULL;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,               // ImageInfoSize
                      FmpImageInfoBuf,              // ImageInfo
                      &FmpImageInfoDescriptorVer,   // DescriptorVersion
                      &FmpImageInfoCount,           // DescriptorCount
                      &DescriptorSize,              // DescriptorSize
                      &PackageVersion,              // PackageVersion
                      &PackageVersionName           // PackageVersionName
                      );

      //
      // If FMP GetInformation interface failed, skip this resource
      //
      if (EFI_ERROR(Status)) {
        FreePool(FmpImageInfoBuf);
        continue;
      }

      if (PackageVersionName != NULL) {
        FreePool(PackageVersionName);
      }

      TempFmpImageInfo = FmpImageInfoBuf;
      for (Index2 = 0; Index2 < FmpImageInfoCount; Index2++) {
        //
        // Check all the payload entry in capsule payload list 
        //
        for (Index = FmpCapsuleHeader->EmbeddedDriverCount; Index < ItemNum; Index++) {
          ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index]);
          if (CompareGuid(&ImageHeader->UpdateImageTypeId, &TempFmpImageInfo->ImageTypeId) &&
              ImageHeader->UpdateImageIndex == TempFmpImageInfo->ImageIndex) {
            AbortReason = NULL;
            if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
              if(ImageHeader->UpdateHardwareInstance != 0){ 
                //
                // FMP Version is >=2 & UpdateHardwareInstance Skip 2 case 
                //  1. FMP Image info Version < 3
                //  2. HardwareInstance doesn't match
                //
                if (FmpImageInfoDescriptorVer < EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION || 
                   ImageHeader->UpdateHardwareInstance != TempFmpImageInfo->HardwareInstance) {
                  continue;
                }
              }
              Image = (UINT8 *)(ImageHeader + 1);
            } else {
              //
              // If the EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER is version 1, only match ImageTypeId.
              // Header should exclude UpdateHardwareInstance field
              //
              Image = (UINT8 *)ImageHeader + ((EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *) 0)->UpdateHardwareInstance;
            }

            if (ImageHeader->UpdateVendorCodeSize == 0) {
              Status = Fmp->SetImage(
                              Fmp,
                              TempFmpImageInfo->ImageIndex,           // ImageIndex
                              Image,                                  // Image
                              ImageHeader->UpdateImageSize,           // ImageSize
                              NULL,                                   // VendorCode
                              Update_Image_Progress,                  // Progress
                              &AbortReason                            // AbortReason
                              );
            } else {
              Status = Fmp->SetImage(
                              Fmp,
                              TempFmpImageInfo->ImageIndex,                                          // ImageIndex
                              Image,                                                                 // Image
                              ImageHeader->UpdateImageSize,                                          // ImageSize
                              Image + ImageHeader->UpdateImageSize,                                  // VendorCode
                              Update_Image_Progress,                                                 // Progress
                              &AbortReason                                                           // AbortReason
                              );
            }
            if (AbortReason != NULL) {
              DEBUG ((EFI_D_ERROR, "%s\n", AbortReason));
              FreePool(AbortReason);
            }
            //
            // Update EsrtEntry For V1, V2 FMP instance. V3 FMP ESRT cache will be synced up through EsrtSyncFmp interface
            //
            if (FmpImageInfoDescriptorVer < EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION && EsrtProtocol != NULL) {
               StatusEsrt = EsrtProtocol->GetEsrtEntry(&CapsuleHeader->CapsuleGuid, &EsrtEntry);
               if (!EFI_ERROR(StatusEsrt)){
                 if (!EFI_ERROR(Status)) {
                   EsrtEntry.LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;
                 } else {
                   EsrtEntry.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL;
                 }
                 EsrtEntry.LastAttemptVersion = 0;
                 EsrtProtocol->UpdateEsrtEntry(&EsrtEntry);
               }
             }
          }
        }
        //
        // Use DescriptorSize to move ImageInfo Pointer to stay compatible with different ImageInfo version
        //
        TempFmpImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)((UINT8 *)TempFmpImageInfo + DescriptorSize);
      }
      FreePool(FmpImageInfoBuf);
    }
  }

EXIT:

  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }

  if (DriverDevicePath != NULL) {
    FreePool(DriverDevicePath);
  }

  return Status;
}


/**
  Those capsules supported by the firmwares.

  @param  CapsuleHeader    Points to a capsule header.

  @retval EFI_SUCESS       Input capsule is supported by firmware.
  @retval EFI_UNSUPPORTED  Input capsule is not supported by the firmware.
  @retval EFI_INVALID_PARAMETER Input capsule layout is not correct

**/
EFI_STATUS
EFIAPI
SupportCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                   Status;
  EFI_SYSTEM_RESOURCE_ENTRY    EsrtEntry;
  ESRT_MANAGEMENT_PROTOCOL     *EsrtProtocol;

  //
  // We may need to add Hook point for OEM known Guids like gFwUpdateDisplayCapsuleGuid
  //

  //
  // check Display Capsule Guid
  //
  if (CompareGuid(&gWindowsUxCapsuleGuid, &CapsuleHeader->CapsuleGuid)) {
    return EFI_SUCCESS;
  }

  if (CompareGuid (&gEfiFmpCapsuleGuid, &CapsuleHeader->CapsuleGuid)) {
    //
    // Check layout of FMP capsule
    //
    return ValidateFmpCapsule(CapsuleHeader);
  }

  Status = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtProtocol);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = EsrtProtocol->GetEsrtEntry(&CapsuleHeader->CapsuleGuid, &EsrtEntry);
  if (!EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


/**
  The firmware implements to process the capsule image.

  @param  CapsuleHeader         Points to a capsule header.

  @retval EFI_SUCESS            Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  UINT32                       Length;
  EFI_FIRMWARE_VOLUME_HEADER   *FvImage;
  EFI_FIRMWARE_VOLUME_HEADER   *ProcessedFvImage;
  EFI_STATUS                   Status;
  EFI_STATUS                   StatusEsrt;
  EFI_HANDLE                   FvProtocolHandle;
  UINT32                       FvAlignment;
  EFI_SYSTEM_RESOURCE_ENTRY    EsrtEntry;
  UINT32                       AttemptStatus;
  ESRT_MANAGEMENT_PROTOCOL     *EsrtManagement;

  FvImage          = NULL;
  ProcessedFvImage = NULL;
  Status           = EFI_SUCCESS;
  AttemptStatus    = LAST_ATTEMPT_STATUS_SUCCESS;

  if (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  //
  // Display image in firmware update display capsule
  //
  if (CompareGuid(&gWindowsUxCapsuleGuid, &CapsuleHeader->CapsuleGuid)) {
    return DisplayCapsuleImage(
             (DISPLAY_DISPLAY_PAYLOAD *)(CapsuleHeader + 1), 
             (UINTN)(CapsuleHeader->CapsuleImageSize - sizeof(EFI_CAPSULE_HEADER)));
  }

  //
  // Check FMP capsule layout
  //
  if (CompareGuid (&gEfiFmpCapsuleGuid, &CapsuleHeader->CapsuleGuid)){
    Status = ValidateFmpCapsule(CapsuleHeader);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Press EFI FMP Capsule
    //
    Status = ProcessFmpCapsuleImage(CapsuleHeader);

    //
    // Indicate to sync Esrt on next boot
    //
    PcdSetBool(PcdEsrtSyncFmp, TRUE);
    return Status;
  }

  //
  // Other non-FMP capsule handler
  //
  // Skip the capsule header, move to the Firware Volume
  //
  FvImage = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINT8 *) CapsuleHeader + CapsuleHeader->HeaderSize);
  Length  = CapsuleHeader->CapsuleImageSize - CapsuleHeader->HeaderSize;

  while (Length != 0) {
    //
    // Point to the next firmware volume header, and then
    // call the DXE service to process it.
    //
    if (FvImage->FvLength > (UINTN) Length) {
      //
      // Notes: need to stuff this status somewhere so that the
      // error can be detected at OS runtime
      //
      Status = EFI_VOLUME_CORRUPTED;
      break;
    }

    FvAlignment = 1 << ((FvImage->Attributes & EFI_FVB2_ALIGNMENT) >> 16);
    //
    // FvAlignment must be more than 8 bytes required by FvHeader structure.
    //
    if (FvAlignment < 8) {
      FvAlignment = 8;
    }
    //
    // Check FvImage Align is required.
    //
    if (((UINTN) FvImage % FvAlignment) == 0) {
      ProcessedFvImage = FvImage;
    } else {
      //
      // Allocate new aligned buffer to store FvImage.
      //
      ProcessedFvImage = (EFI_FIRMWARE_VOLUME_HEADER *) AllocateAlignedPages ((UINTN) EFI_SIZE_TO_PAGES ((UINTN) FvImage->FvLength), (UINTN) FvAlignment);
      if (ProcessedFvImage == NULL) {
        AttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES;
        Status        = EFI_OUT_OF_RESOURCES;
        break;
      }
      CopyMem (ProcessedFvImage, FvImage, (UINTN) FvImage->FvLength);
    }

    Status = gDS->ProcessFirmwareVolume (
                    (VOID *) ProcessedFvImage,
                    (UINTN) ProcessedFvImage->FvLength,
                    &FvProtocolHandle
                    );
    if (EFI_ERROR (Status)) {
      AttemptStatus = LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT;
      break;
    }

    //
    // Call the dispatcher to dispatch any drivers from the produced firmware volume
    //
    gDS->Dispatch ();
    //
    // On to the next FV in the capsule
    //
    Length -= (UINT32) FvImage->FvLength;
    FvImage = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINT8 *) FvImage + FvImage->FvLength);
  }

  //
  // Update corresponding ESRT entry LastAttemp Status 
  //
  StatusEsrt = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtManagement);
  if (!EFI_ERROR(StatusEsrt)) {
    StatusEsrt = EsrtManagement->GetEsrtEntry(&CapsuleHeader->CapsuleGuid, &EsrtEntry);
    if (!EFI_ERROR(StatusEsrt)){
      //
      // Update version can't be get from FV, set LastAttemptVersion to zero after a failed update
      //
      if (AttemptStatus != LAST_ATTEMPT_STATUS_SUCCESS) {
        EsrtEntry.LastAttemptVersion = 0;
      }
      EsrtEntry.LastAttemptStatus = AttemptStatus;
      EsrtManagement->UpdateEsrtEntry(&EsrtEntry);
    }
  }

  //
  // Indicate to sync Esrt on next boot
  //
  PcdSetBool(PcdEsrtSyncFmp, TRUE);
  return Status;
}


/**
    The constructor function of DxeCapsuleLib.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor successfully .
  @retval Other value   The constructor can't add string package.

**/
EFI_STATUS
EFIAPI
DxeCapsuleLibConstructor (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  return EFI_SUCCESS;
}
