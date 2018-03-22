/** @file
  Header file of Platform Boot Manager Lib.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BDS_PLATFORM_H
#define _BDS_PLATFORM_H

#include <PiDxe.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LoadFile.h>
#include <Protocol/PciIo.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/GenericMemoryTest.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/GlobalVariable.h>
#include <Guid/MemoryOverwriteControl.h>
#include <Guid/FileInfo.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PlatformBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/CapsuleLib.h>
#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/PciCodeId.h>

#define CONSOLE_OUT 0x00000001
#define STD_ERROR   0x00000002
#define CONSOLE_IN  0x00000004
#define CONSOLE_ALL (CONSOLE_OUT | CONSOLE_IN | STD_ERROR)
#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1

#define mPciRootBridge \
  { \
    ACPI_DEVICE_PATH, ACPI_DP, (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID (0x0A03), 0 \
  }

#define mEndEntire \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 \
  }

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     ConnectType;
} BDS_CONSOLE_CONNECT_ENTRY;

//
// Platform Root Bridge.
//
typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ROOT_BRIDGE_DEVICE_PATH;

//
// Below is the platform console device path.
//
typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      Keyboard;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_KEYBOARD_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      IsaSerial;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ISA_SERIAL_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_PEG_ROOT_CONTROLLER_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           PciBridge;
  PCI_DEVICE_PATH           PciDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_PCI_CONTROLLER_DEVICE_PATH;

typedef struct {
  USB_CLASS_DEVICE_PATH           UsbClass;
  EFI_DEVICE_PATH_PROTOCOL        End;
} USB_CLASS_FORMAT_DEVICE_PATH;

typedef
VOID
(*PROCESS_VARIABLE) (
  VOID  **Variable,
  UINTN *VariableSize
  );

/**
  Generic function to update the console variable.
  Please refer to FastBootSupport.c for how to use it.

  @param VariableName    - The name of the variable to be updated
  @param AgentGuid       - The Agent GUID
  @param ProcessVariable - The function pointer to update the variable
                           NULL means to restore to the original value
**/
VOID
UpdateEfiGlobalVariable (
  CHAR16           *VariableName,
  EFI_GUID         *AgentGuid,
  PROCESS_VARIABLE ProcessVariable
  );

VOID
ConnectSequence (
  IN EFI_BOOT_MODE                      BootMode
  );

INTN
EFIAPI
CompareBootOption (
  CONST EFI_BOOT_MANAGER_LOAD_OPTION  *Left,
  CONST EFI_BOOT_MANAGER_LOAD_OPTION  *Right
  );

VOID 
PrintBootPrompt (
  VOID
  );

VOID
RegisterStaticHotkey (
  VOID
  );

VOID
RegisterDynamicHotkey (
  VOID
  );

VOID
RegisterDefaultBootOption (
  VOID
  );

VOID
BootUi (
  VOID
  );

EFI_STATUS
AutomaticFirmwareUpdateHandler (
  CHAR16 *FileName
  );

EFI_STATUS
EFIAPI
EfiPlatformBootManagerProcessCapsules (
  VOID
  );

VOID
PlatformBootManagerAttemptUsbBoot (
  VOID
  );

EFI_STATUS
SetMorControl (
  VOID
  );
  
extern PLATFORM_ISA_SERIAL_DEVICE_PATH           mSerialDevicePath;
extern EFI_DEVICE_PATH_PROTOCOL                  *mPlatformRootBridges[];
extern BDS_CONSOLE_CONNECT_ENTRY                 mPlatformConsole[];
extern PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH   mPlatformIGDDevice;
extern PLATFORM_KEYBOARD_DEVICE_PATH             mKeyboardDevicePath;
extern USB_CLASS_FORMAT_DEVICE_PATH              mUsbClassKeyboardDevicePath;
extern USB_CLASS_FORMAT_DEVICE_PATH              gUsbClassMassStorageDevice;
extern EFI_GUID                                  gUefiShellFileGuid;
extern EFI_HII_HANDLE                            mPlatformBdsLibStringPackHandle;
extern BDS_CONSOLE_CONNECT_ENTRY                 mPlatformConsole [];
extern EFI_BOOT_MODE                             mBootMode;

#endif
