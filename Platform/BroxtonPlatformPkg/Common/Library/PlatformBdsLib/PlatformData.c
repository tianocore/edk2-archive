/** @file
  Defined the platform specific device path which will be used by
  platform Bbd to perform the platform policy connect.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BdsPlatform.h"

//
// Predefined platform default time out value
//
UINT16  gPlatformBootTimeOutDefault = 10;

//
// Predefined platform root bridge
//
PLATFORM_ROOT_BRIDGE_DEVICE_PATH gPlatformRootBridge0 = {
  gPciRootBridge,
  gEndEntire
};

EFI_DEVICE_PATH_PROTOCOL* gPlatformRootBridges [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gPlatformRootBridge0,
  NULL
};

//
// Platform specific ISA keyboard device path
//
PLATFORM_ISA_KEYBOARD_DEVICE_PATH gIsaKeyboardDevicePath = {
  gPciRootBridge,
  gPciIsaBridge,
  gPnpPs2Keyboard,
  gEndEntire
};

//
// Platform specific on chip PCI VGA device path
//
PLATFORM_ONBOARD_VGA_DEVICE_PATH gOnChipPciVgaDevicePath = {
  gPciRootBridge,
  PCI_DEVICE_PATH_NODE (0, 0x2),
  gEndEntire
};

//
// Platform specific plug in PCI VGA device path
//
PLATFORM_OFFBOARD_VGA_DEVICE_PATH gPlugInPciVgaDevicePath = {
  gPciRootBridge,
  PCI_DEVICE_PATH_NODE (0, 0x1),
  PCI_DEVICE_PATH_NODE (0, 0x0),
  gEndEntire
};

//
// Platform specific ISA serial device path
//
PLATFORM_ISA_SERIAL_DEVICE_PATH gIsaSerialDevicePath = {
  gPciRootBridge,
  gPciIsaBridge,
  gPnp16550ComPort,
  gUart (115200, 8, 1, 1),
  gPcAnsiTerminal,
  gEndEntire
};

//
// Platform specific SPI Uart device path
//
PLATFORM_ISA_SERIAL_DEVICE_PATH gSpiDevicePath = {
  gPciRootBridge,
  gPciSpiBridge,
  gPnp16550ComPort,
  gUart (115200, 8, 1, 1),
  gPcAnsiTerminal,
  gEndEntire
};

USB_CLASS_FORMAT_DEVICE_PATH gUsbClassKeyboardDevicePath = {
  gUsbKeyboardMouse,
  gEndEntire
};

//
// Predefined platform default console device path
//
BDS_CONSOLE_CONNECT_ENTRY gPlatformConsole [] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gSpiDevicePath, (CONSOLE_OUT | CONSOLE_IN)},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gIsaSerialDevicePath, (CONSOLE_OUT | CONSOLE_IN)},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gIsaKeyboardDevicePath, CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gUsbClassKeyboardDevicePath, CONSOLE_IN},
  {NULL, 0}
};

//
// All the possible platform PCI VGA device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePciVgaConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gOnChipPciVgaDevicePath,
  (EFI_DEVICE_PATH_PROTOCOL *) &gPlugInPciVgaDevicePath,
  NULL
};

//
// Legacy hard disk boot option
//
LEGACY_HD_DEVICE_PATH gLegacyHd = {
  {
    {
      BBS_DEVICE_PATH,
      BBS_BBS_DP,
      {
        (UINT8) (sizeof (BBS_BBS_DEVICE_PATH)),
        (UINT8) ((sizeof (BBS_BBS_DEVICE_PATH)) >> 8)
      }
    },
    BBS_TYPE_HARDDRIVE,
    0,
    {0}
  },
  gEndEntire
};

//
// Legacy cdrom boot option
//
LEGACY_HD_DEVICE_PATH gLegacyCdrom = {
  {
    {
      BBS_DEVICE_PATH,
      BBS_BBS_DP,
      {
        (UINT8) (sizeof (BBS_BBS_DEVICE_PATH)),
        (UINT8) ((sizeof (BBS_BBS_DEVICE_PATH)) >> 8),
       } 
    },
    BBS_TYPE_CDROM,
    0,
    {0}
  },
  gEndEntire
};

//
// Predefined platform specific perdict boot option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformBootOption [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gLegacyHd,
  (EFI_DEVICE_PATH_PROTOCOL *) &gLegacyCdrom,
  NULL
};

EFI_DEVICE_PATH_PROTOCOL* gSerialIoConnect[] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gIsaSerialDevicePath,
  NULL
};

//
// Predefined platform specific driver option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformDriverOption [] = {
  NULL
};

//
// Platform specific SATA controller device path
//
PLATFORM_PCI_DEVICE_PATH gSataBootDevPath0 = {
  gPciRootBridge,
  PCI_DEVICE_PATH_NODE (0x00, 0x12),
  gEndEntire
};

//
// eMMC device at BDF(0x0, 0x1C, 0x0)
//
PLATFORM_PCI_DEVICE_PATH gEmmcBootDevPath0 = {
  gPciRootBridge,
  PCI_DEVICE_PATH_NODE (0x00, 0x1C),
  gEndEntire
};

//
// Predefined platform connect sequence
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectSequence [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gPlatformRootBridge0,  // Force PCI enumer before Legacy OpROM shadow
  (EFI_DEVICE_PATH_PROTOCOL *) &gSataBootDevPath0,
  (EFI_DEVICE_PATH_PROTOCOL *) &gEmmcBootDevPath0,
  NULL
};

//
// Platform specific USB controller device path
//
PLATFORM_USB_DEVICE_PATH gUsbDevicePath0 = {
  gPciRootBridge,
  PCI_DEVICE_PATH_NODE (0, 0x15),
  gEndEntire
};

//
// Predefined platform device path for user authtication
//
EFI_DEVICE_PATH_PROTOCOL* gUserAuthenticationDevice[] = {
  //
  // Predefined device path for secure card (USB disk).
  //
  (EFI_DEVICE_PATH_PROTOCOL *) &gUsbDevicePath0,
  NULL
};

//
// Predefined platform console device path
//
BDS_CONSOLE_CONNECT_ENTRY gPlatformSimpleOnChipPciVgaConOutConsole [] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gOnChipPciVgaDevicePath, CONSOLE_OUT},
  {NULL, 0}
};

BDS_CONSOLE_CONNECT_ENTRY gPlatformSimpleUsbConInConsole [] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gUsbClassKeyboardDevicePath, CONSOLE_IN},
  {NULL, 0}
};

BDS_CONSOLE_CONNECT_ENTRY gPlatformSimplePs2ConInConsole [] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gIsaKeyboardDevicePath, CONSOLE_IN},
  {NULL, 0}
};

//
// Predefined platform specific perdict boot option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformSimpleBootOption [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gEmmcBootDevPath0,
  (EFI_DEVICE_PATH_PROTOCOL *) &gSataBootDevPath0,
  NULL
};

