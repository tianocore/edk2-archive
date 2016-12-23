/** @file
  FrameBufferBltLib - Library to perform blt operations on a frame buffer.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/GraphicsOutput.h>

#if 0
#define VDEBUG DEBUG
#else
#define VDEBUG(x)
#endif

#define MAX_LINE_BUFFER_SIZE (SIZE_4KB * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))

UINT8             mBltLibLineBuffer[MAX_LINE_BUFFER_SIZE];
EFI_PIXEL_BITMASK mBltLibRgbPixelMasks = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
EFI_PIXEL_BITMASK mBltLibBgrPixelMasks = {0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};

EFI_STATUS
BltLibParsePixelFormat (
  IN  EFI_GRAPHICS_PIXEL_FORMAT  PixelFormat,
  IN  EFI_PIXEL_BITMASK          *PixelInformation,
  OUT UINT8                      *PixelShl,
  OUT UINT8                      *PixelShr,
  OUT UINT32                     *PixelMask,
  OUT UINT8                      *BytesPerPixel
  )
{
  UINTN                          Index;
  UINT32                         MergedMasks;
  UINT32                         *Mask;

  switch (PixelFormat) {
    case PixelRedGreenBlueReserved8BitPerColor:
      return BltLibParsePixelFormat (PixelBitMask, &mBltLibRgbPixelMasks, PixelShl, PixelShr, PixelMask, BytesPerPixel);

    case PixelBlueGreenRedReserved8BitPerColor:
      return BltLibParsePixelFormat (PixelBitMask, &mBltLibBgrPixelMasks, PixelShl, PixelShr, PixelMask, BytesPerPixel);

    case PixelBitMask:
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  MergedMasks = 0;
  Mask = (UINT32 *) PixelInformation;
  for (Index = 0; Index < 4; Index++) {
    //
    // Only ReservedMask can be 0
    //
    if (Index != 3 && Mask[Index] == 0) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // The Mask of each color shouldn't overlap
    //
    if ((MergedMasks & Mask[Index]) != 0) {
      return EFI_INVALID_PARAMETER;
    }
    MergedMasks |= Mask[Index];

    if (PixelShl != NULL && PixelShr != NULL) {
      PixelShl[Index]  = (UINT8) (HighBitSet32 (Mask[Index]) - 23 + (Index * 8));
      PixelShl[Index] %= 32;
      if ((INT8) PixelShl[Index] < 0) {
        PixelShr[Index] = -PixelShl[Index];
        PixelShl[Index] = 0;
      } else {
        PixelShr[Index] = 0;
      }
      VDEBUG ((EFI_D_INFO, "%d: shl:%d shr:%d mask:%x\n", Index, PixelShl[Index], PixelShr[Index], Mask[Index]));
    }
  }
  if (PixelMask != NULL) {
    CopyMem (PixelMask, PixelInformation, sizeof (EFI_PIXEL_BITMASK));
  }

  if (BytesPerPixel != NULL) {
    *BytesPerPixel = (UINT8) ((HighBitSet32 (MergedMasks) + 7) / 8);
    VDEBUG ((EFI_D_INFO, "Bytes per pixel: %d\n", *BytesPerPixel));
  }

  return EFI_SUCCESS;
}


EFI_STATUS
BltLibVerifyLocation (
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *FrameBufferInfo,
  IN  UINTN                                 X,
  IN  UINTN                                 Y,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height
  )
{
  if ((X >= FrameBufferInfo->HorizontalResolution) ||
      (Width > FrameBufferInfo->HorizontalResolution - X)) {
    VDEBUG ((EFI_D_INFO, "VideoFill: Past screen (X)\n"));
    return EFI_INVALID_PARAMETER;
  }

  if ((Y >= FrameBufferInfo->VerticalResolution) ||
      (Height > FrameBufferInfo->VerticalResolution - Y)) {
    VDEBUG ((EFI_D_INFO, "VideoFill: Past screen (Y)\n"));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}


/**
  Performs a UEFI Graphics Output Protocol Blt Video Fill.

  @param[in]  FrameBuffer            Pointer to the start of the frame buffer
  @param[in]  FrameBufferInfo        Describes the frame buffer characteristics
  @param[in]  Color                  Color to fill the region with
  @param[in]  DestinationX           X location to start fill operation
  @param[in]  DestinationY           Y location to start fill operation
  @param[in]  Width                  Width (in pixels) to fill
  @param[in]  Height                 Height to fill

  @retval     EFI_DEVICE_ERROR       A hardware error occured
  @retval     EFI_INVALID_PARAMETER  Invalid parameter passed in
  @retval     EFI_SUCCESS            Blt operation success

**/
EFI_STATUS
EFIAPI
BltVideoFill (
  IN  VOID                                  *FrameBuffer,
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *FrameBufferInfo,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *Color,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height
  )
{
  EFI_STATUS                      Status;
  UINTN                           Y;
  UINT8                           *Destination;
  UINTN                           X;
  UINT8                           Uint8;
  UINT32                          Uint32;
  UINT64                          WideFill;
  BOOLEAN                         UseWideFill;
  BOOLEAN                         LineBufferReady;
  UINTN                           Offset;
  UINTN                           WidthInBytes;
  UINTN                           SizeInBytes;
  UINT8                           PixelShr[4];
  UINT8                           PixelShl[4];
  UINT32                          PixelMask[4];
  UINT8                           BytesPerPixel;

  Status = BltLibVerifyLocation (FrameBufferInfo, DestinationX, DestinationY, Width, Height);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = BltLibParsePixelFormat (
             FrameBufferInfo->PixelFormat,
             &FrameBufferInfo->PixelInformation,
             PixelShr, PixelShl, PixelMask, &BytesPerPixel
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  WidthInBytes = Width * BytesPerPixel;

  Uint32   = *(UINT32 *) Color;
  WideFill = (((Uint32 << PixelShl[0]) >> PixelShr[0]) & PixelMask[0]) |
             (((Uint32 << PixelShl[1]) >> PixelShr[1]) & PixelMask[1]) |
             (((Uint32 << PixelShl[2]) >> PixelShr[2]) & PixelMask[2]) |
             (((Uint32 << PixelShl[3]) >> PixelShr[3]) & PixelMask[3]);
  VDEBUG ((EFI_D_INFO, "VideoFill: color=0x%x, wide-fill=0x%x\n", Uint32, WideFill));

  //
  // If the size of the pixel data evenly divides the sizeof
  // WideFill, then a wide fill operation can be used
  //
  UseWideFill = TRUE;
  if (sizeof (WideFill) % BytesPerPixel == 0) {
    for (X = BytesPerPixel; X < sizeof (WideFill); X++) {
      ((UINT8 *) &WideFill)[X] = ((UINT8 *) &WideFill)[X % BytesPerPixel];
    }
  } else {
    //
    // If all the bytes in the pixel are the same value, then use
    // a wide fill operation.
    //
    for (X = 1, Uint8 = ((UINT8 *) &WideFill)[0]; X < BytesPerPixel; X++) {
      if (Uint8 != ((UINT8 *) &WideFill)[X]) {
        UseWideFill = FALSE;
        break;
      }
    }
    if (UseWideFill) {
      SetMem (&WideFill, sizeof (WideFill), Uint8);
    }
  }

  if (UseWideFill && (DestinationX == 0) && (Width == FrameBufferInfo->HorizontalResolution)) {
    VDEBUG ((EFI_D_INFO, "VideoFill (wide, one-shot)\n"));
    Offset = DestinationY * FrameBufferInfo->PixelsPerScanLine * BytesPerPixel;
    Destination = (UINT8 *) FrameBuffer + Offset;
    SizeInBytes = WidthInBytes * Height;
    if (SizeInBytes >= 8) {
      SetMem64 (Destination, SizeInBytes & ~7, WideFill);
      Destination += (SizeInBytes & ~7);
      SizeInBytes &= 7;
    }
    if (SizeInBytes > 0) {
      CopyMem (Destination, &WideFill, SizeInBytes);
    }
  } else {
    LineBufferReady = FALSE;
    for (Y = DestinationY; Y < (Height + DestinationY); Y++) {
      Offset = ((Y * FrameBufferInfo->PixelsPerScanLine) + DestinationX) * BytesPerPixel;
      Destination = (UINT8 *) FrameBuffer + Offset;

      if (UseWideFill && (((UINTN) Destination & 7) == 0)) {
        VDEBUG ((EFI_D_INFO, "VideoFill (wide)\n"));
        SizeInBytes = WidthInBytes;
        if (SizeInBytes >= 8) {
          SetMem64 (Destination, SizeInBytes & ~7, WideFill);
          Destination += (SizeInBytes & ~7);
          SizeInBytes &= 7;
        }
        if (SizeInBytes > 0) {
          CopyMem (Destination, &WideFill, SizeInBytes);
        }
      } else {
        VDEBUG ((EFI_D_INFO, "VideoFill (not wide)\n"));
        if (!LineBufferReady) {
          CopyMem (mBltLibLineBuffer, &WideFill, BytesPerPixel);
          for (X = 1; X < Width; ) {
            CopyMem (
              (mBltLibLineBuffer + (X * BytesPerPixel)),
              mBltLibLineBuffer,
              MIN (X, Width - X) * BytesPerPixel
              );
            X += MIN (X, Width - X);
          }
          LineBufferReady = TRUE;
        }
        CopyMem (Destination, mBltLibLineBuffer, WidthInBytes);
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Performs a UEFI Graphics Output Protocol Blt Video to Buffer operation.

  @param[in]  FrameBuffer            Pointer to the start of the frame buffer
  @param[in]  FrameBufferInfo        Describes the frame buffer characteristics
  @param[out] BltBuffer              Output buffer for pixel color data
  @param[in]  SourceX                X location within video
  @param[in]  SourceY                Y location within video
  @param[in]  DestinationX           X location within BltBuffer
  @param[in]  DestinationY           Y location within BltBuffer
  @param[in]  Width                  Width (in pixels)
  @param[in]  Height                 Height
  @param[in]  Delta                  Number of bytes in a row of BltBuffer

  @retval     EFI_DEVICE_ERROR       A hardware error occured
  @retval     EFI_INVALID_PARAMETER  Invalid parameter passed in
  @retval     EFI_SUCCESS            Blt operation success

**/
EFI_STATUS
EFIAPI
BltVideoToBuffer (
  IN  VOID                                 *FrameBuffer,
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *FrameBufferInfo,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer,
  IN  UINTN                                SourceX,
  IN  UINTN                                SourceY,
  IN  UINTN                                DestinationX,
  IN  UINTN                                DestinationY,
  IN  UINTN                                Width,
  IN  UINTN                                Height,
  IN  UINTN                                Delta
  )
{
  EFI_STATUS                               Status;
  UINTN                                    DstY;
  UINTN                                    SrcY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL            *Blt;
  UINT8                                    *Source;
  UINT8                                    *Destination;
  UINTN                                    X;
  UINT32                                   Uint32;
  UINTN                                    Offset;
  UINTN                                    WidthInBytes;
  UINT8                                    PixelShr[4];
  UINT8                                    PixelShl[4];
  UINT32                                   PixelMask[4];
  UINT8                                    BytesPerPixel;

  //
  // Video to BltBuffer: Source is Video, destination is BltBuffer
  //
  Status = BltLibVerifyLocation (FrameBufferInfo, SourceX, SourceY, Width, Height);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = BltLibParsePixelFormat (
             FrameBufferInfo->PixelFormat,
             &FrameBufferInfo->PixelInformation,
             PixelShl, PixelShr, PixelMask, &BytesPerPixel
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }

  WidthInBytes = Width * BytesPerPixel;

  //
  // Video to BltBuffer: Source is Video, destination is BltBuffer
  //
  for (SrcY = SourceY, DstY = DestinationY; DstY < (DestinationY + Height); SrcY++, DstY++) {

    Offset = ((SrcY * FrameBufferInfo->PixelsPerScanLine) + SourceX) * BytesPerPixel;
    Source = (UINT8 *) FrameBuffer + Offset;

    if (FrameBufferInfo->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
      Destination = (UINT8 *) BltBuffer + (DstY * Delta) + (DestinationX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    } else {
      Destination = mBltLibLineBuffer;
    }

    CopyMem (Destination, Source, WidthInBytes);

    if (FrameBufferInfo->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
      for (X = 0; X < Width; X++) {
        Blt    = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) ((UINT8 *) BltBuffer + (DstY * Delta) + (DestinationX + X) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
        Uint32 = *(UINT32 *) (mBltLibLineBuffer + (X * BytesPerPixel));
        *(UINT32 *) Blt = (((Uint32 & PixelMask[0]) >> PixelShl[0]) << PixelShr[0]) |
                          (((Uint32 & PixelMask[1]) >> PixelShl[1]) << PixelShr[1]) |
                          (((Uint32 & PixelMask[2]) >> PixelShl[2]) << PixelShr[2]) |
                          (((Uint32 & PixelMask[3]) >> PixelShl[3]) << PixelShr[3]);
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Performs a UEFI Graphics Output Protocol Blt Buffer to Video operation.

  @param[in]  FrameBuffer            Pointer to the start of the frame buffer
  @param[in]  FrameBufferInfo        Describes the frame buffer characteristics
  @param[in]  BltBuffer              Output buffer for pixel color data
  @param[in]  SourceX                X location within BltBuffer
  @param[in]  SourceY                Y location within BltBuffer
  @param[in]  DestinationX           X location within video
  @param[in]  DestinationY           Y location within video
  @param[in]  Width                  Width (in pixels)
  @param[in]  Height                 Height
  @param[in]  Delta                  Number of bytes in a row of BltBuffer

  @retval     EFI_DEVICE_ERROR       A hardware error occured
  @retval     EFI_INVALID_PARAMETER  Invalid parameter passed in
  @retval     EFI_SUCCESS            Blt operation success

**/
EFI_STATUS
EFIAPI
BltBufferToVideo (
  IN  VOID                                 *FrameBuffer,
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *FrameBufferInfo,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer,
  IN  UINTN                                SourceX,
  IN  UINTN                                SourceY,
  IN  UINTN                                DestinationX,
  IN  UINTN                                DestinationY,
  IN  UINTN                                Width,
  IN  UINTN                                Height,
  IN  UINTN                                Delta
  )
{
  EFI_STATUS                               Status;
  UINTN                                    DstY;
  UINTN                                    SrcY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL            *Blt;
  UINT8                                    *Source;
  UINT8                                    *Destination;
  UINTN                                    X;
  UINT32                                   Uint32;
  UINTN                                    Offset;
  UINTN                                    WidthInBytes;
  UINT8                                    PixelShr[4];
  UINT8                                    PixelShl[4];
  UINT32                                   PixelMask[4];
  UINT8                                    BytesPerPixel;

  //
  // BltBuffer to Video: Source is BltBuffer, destination is Video
  //
  Status = BltLibVerifyLocation (FrameBufferInfo, DestinationX, DestinationY, Width, Height);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = BltLibParsePixelFormat (
             FrameBufferInfo->PixelFormat,
             &FrameBufferInfo->PixelInformation,
             PixelShl, PixelShr, PixelMask, &BytesPerPixel
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }

  WidthInBytes = Width * BytesPerPixel;

  for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {

    Offset = ((DstY * FrameBufferInfo->PixelsPerScanLine) + DestinationX) * BytesPerPixel;
    Destination = (UINT8 *) FrameBuffer + Offset;

    if (FrameBufferInfo->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
      Source = (UINT8 *) BltBuffer + (SrcY * Delta);
    } else {
      for (X = 0; X < Width; X++) {
        Blt =
          (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (
              (UINT8 *) BltBuffer +
              (SrcY * Delta) +
              ((SourceX + X) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))
            );
        Uint32 = *(UINT32 *) Blt;
        *(UINT32 *) (mBltLibLineBuffer + (X * BytesPerPixel)) =
          (((Uint32 << PixelShl[0]) >> PixelShr[0]) & PixelMask[0]) |
          (((Uint32 << PixelShl[1]) >> PixelShr[1]) & PixelMask[1]) |
          (((Uint32 << PixelShl[2]) >> PixelShr[2]) & PixelMask[2]) |
          (((Uint32 << PixelShl[3]) >> PixelShr[3]) & PixelMask[3]);
      }
      Source = mBltLibLineBuffer;
    }

    CopyMem (Destination, Source, WidthInBytes);
  }

  return EFI_SUCCESS;
}


/**
  Performs a UEFI Graphics Output Protocol Blt Video to Video operation.

  @param[in]  FrameBuffer            Pointer to the start of the frame buffer
  @param[in]  FrameBufferInfo        Describes the frame buffer characteristics
  @param[in]  SourceX                X location within video
  @param[in]  SourceY                Y location within video
  @param[in]  DestinationX           X location within video
  @param[in]  DestinationY           Y location within video
  @param[in]  Width                  Width (in pixels)
  @param[in]  Height                 Height

  @retval     EFI_DEVICE_ERROR       A hardware error occured
  @retval     EFI_INVALID_PARAMETER  Invalid parameter passed in
  @retval     EFI_SUCCESS            Blt operation success

**/
EFI_STATUS
EFIAPI
BltVideoToVideo (
  IN  VOID                                 *FrameBuffer,
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *FrameBufferInfo,
  IN  UINTN                                 SourceX,
  IN  UINTN                                 SourceY,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *Source;
  UINT8                                     *Destination;
  UINTN                                     Offset;
  UINTN                                     WidthInBytes;
  INTN                                      LineStride;
  UINT8                                     BytesPerPixel;

  //
  // Video to Video: Source is Video, destination is Video
  //
  Status = BltLibVerifyLocation (FrameBufferInfo, SourceX, SourceY, Width, Height);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = BltLibVerifyLocation (FrameBufferInfo, DestinationX, DestinationY, Width, Height);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = BltLibParsePixelFormat (
             FrameBufferInfo->PixelFormat,
             &FrameBufferInfo->PixelInformation,
             NULL, NULL, NULL, &BytesPerPixel
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  WidthInBytes = Width * BytesPerPixel;

  Offset      = ((SourceY * FrameBufferInfo->PixelsPerScanLine) + SourceX) * BytesPerPixel;
  Source      = (UINT8 *) FrameBuffer + Offset;

  Offset      = ((DestinationY * FrameBufferInfo->PixelsPerScanLine) + DestinationX) * BytesPerPixel;
  Destination = (UINT8 *) FrameBuffer + Offset;

  LineStride  = FrameBufferInfo->PixelsPerScanLine * BytesPerPixel;
  if (Destination > Source) {
    //
    // Copy from last line to avoid source is corrupted by copying
    //
    Source += Height * LineStride;
    Destination += Height * LineStride;
    LineStride = -LineStride;
  }

  while (Height-- > 0) {
    CopyMem (Destination, Source, WidthInBytes);

    Source += LineStride;
    Destination += LineStride;
  }

  return EFI_SUCCESS;
}

