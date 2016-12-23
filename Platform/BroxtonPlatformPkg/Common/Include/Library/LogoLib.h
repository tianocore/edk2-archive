/** @file
  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _LOGO_LIB_H_
#define _LOGO_LIB_H_

/**
  Use SystemTable ConOut to stop video based Simple Text Out consoles from going
  to the video device. Put up LogoFile on every video device that is a console.

  @param[in]  LogoFile         The file name of logo to display on the center of the screen.

  @retval     EFI_SUCCESS      ConsoleControl has been flipped to graphics and logo displayed.
  @retval     EFI_UNSUPPORTED  Logo not found.

**/
EFI_STATUS
EFIAPI
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  );

/**
  Use SystemTable ConOut to turn on video based Simple Text Out consoles. The
  Simple Text Out screens will now be synced up with all non-video output devices.

  @retval  EFI_SUCCESS     UGA devices are back in text mode and synced up.

**/
EFI_STATUS
EFIAPI
DisableQuietBoot (
  VOID
  );

/**
  Show progress bar with title above it. It only works in Graphics mode.

  @param[in] TitleForeground   Foreground color for Title.
  @param[in] TitleBackground   Background color for Title.
  @param[in] Title             Title above progress bar.
  @param[in] ProgressColor     Progress bar color.
  @param[in] Progress          Progress (0-100)
  @param[in] PreviousValue     The previous value of the progress.

  @retval    EFI_STATUS       Success update the progress bar

**/
EFI_STATUS
EFIAPI
ShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  );
#endif

