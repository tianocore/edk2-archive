/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformBootManager.h"

#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define SA_IGD_DEV           0x02
#define SA_IGD_FUN           0x00

//
// Predefined platform root bridge.
//
GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_ROOT_BRIDGE_DEVICE_PATH  gPlatformRootBridge0 = {
  mPciRootBridge,
  mEndEntire
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_DEVICE_PATH_PROTOCOL          *mPlatformRootBridges[] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gPlatformRootBridge0,
  NULL
};

//
// Platform specific keyboard device path.
//
GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_KEYBOARD_DEVICE_PATH     mKeyboardDevicePath = {
  mPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    PCI_FUNCTION_NUMBER_PCH_LPC,
    PCI_DEVICE_NUMBER_PCH_LPC
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0303),
    0
  },
  mEndEntire
};

//
// Platform specific serial device path.
//
GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_ISA_SERIAL_DEVICE_PATH   mSerialDevicePath = {
  mPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    PCI_FUNCTION_NUMBER_PCH_LPC,
    PCI_DEVICE_NUMBER_PCH_LPC
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),
    0
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8) (sizeof (UART_DEVICE_PATH)),
    (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
    (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  mEndEntire
};

GLOBAL_REMOVE_IF_UNREFERENCED USB_CLASS_FORMAT_DEVICE_PATH mUsbClassKeyboardDevicePath = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_USB_CLASS_DP,
      (UINT8) (sizeof (USB_CLASS_DEVICE_PATH)),
      (UINT8) ((sizeof (USB_CLASS_DEVICE_PATH)) >> 8)
    },
    0xffff,           // VendorId 
    0xffff,           // ProductId 
    CLASS_HID,        // DeviceClass 
    SUBCLASS_BOOT,    // DeviceSubClass
    PROTOCOL_KEYBOARD // DeviceProtocol
  },

  { 
    END_DEVICE_PATH_TYPE, 
    END_ENTIRE_DEVICE_PATH_SUBTYPE, 
    END_DEVICE_PATH_LENGTH, 
    0
  }
};

//
// Onboard VGA controller device path.
//
GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH         mPlatformIGDDevice = {
  mPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    SA_IGD_FUN,
    SA_IGD_DEV
  },
  mEndEntire
};

//
// Predefined platform default console device path.
//
GLOBAL_REMOVE_IF_UNREFERENCED BDS_CONSOLE_CONNECT_ENTRY         mPlatformConsole[] = {
  {
     (EFI_DEVICE_PATH_PROTOCOL *) &mSerialDevicePath,
     (CONSOLE_OUT | CONSOLE_IN)
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mKeyboardDevicePath,
    CONSOLE_IN
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mUsbClassKeyboardDevicePath, 
    CONSOLE_IN
  },
  {
    NULL,
    0
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED USB_CLASS_FORMAT_DEVICE_PATH gUsbClassMassStorageDevice = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_USB_CLASS_DP,
      (UINT8) (sizeof (USB_CLASS_DEVICE_PATH)),
      (UINT8) ((sizeof (USB_CLASS_DEVICE_PATH)) >> 8)
    },
    0xffff,  // VendorId 
    0xffff,  // ProductId 
    0x08,    // DeviceClass    - USB Mass Storage Class
    0x06,    // DeviceSubClass - SCSI Transparent Command Set
    0xff     // DeviceProtocol - Match any Device Protocol
  },
  mEndEntire
};
