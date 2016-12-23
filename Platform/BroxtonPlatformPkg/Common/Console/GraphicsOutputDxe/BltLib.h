/** @file
  Library for performing video blt operations.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BLT_LIB__
#define __BLT_LIB__

#include <Protocol/GraphicsOutput.h>

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
  );

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
  );

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
  );

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
  IN  VOID                                  *FrameBuffer,
  IN  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *FrameBufferInfo,
  IN  UINTN                                 SourceX,
  IN  UINTN                                 SourceY,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height
  );

#endif

