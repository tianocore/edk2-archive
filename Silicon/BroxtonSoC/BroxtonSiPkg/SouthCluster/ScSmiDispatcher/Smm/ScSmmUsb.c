/** @file
  File to contain all the hardware specific stuff for the Smm USB dispatch protocol.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"
#include <Protocol/PciRootBridgeIo.h>

GLOBAL_REMOVE_IF_UNREFERENCED SC_SMM_SOURCE_DESC mUSB1_LEGACY = {
  SC_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_SMI_EN}
      },
      S_SMI_EN,
      N_SMI_EN_LEGACY_USB
    },
    NULL_BIT_DESC_INITIALIZER
  },

  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_SMI_STS}
      },
      S_SMI_STS,
      N_SMI_STS_LEGACY_USB
    }
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED SC_SMM_SOURCE_DESC mUSB3_LEGACY = {
  SC_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_SMI_EN}
      },
      S_SMI_EN,
      N_SMI_EN_LEGACY_USB3
    },
    NULL_BIT_DESC_INITIALIZER
  },

  {
    {
      {
        ACPI_ADDR_TYPE,
        {R_SMI_STS}
      },
      S_SMI_STS,
      N_SMI_STS_LEGACY_USB3
    }
  }
};

typedef enum {
  ScUsbControllerLpc0 = 0,
  ScUsbControllerXhci,
  ScUsbControllerTypeMax
} SC_USB_CONTROLLER_TYPE;

typedef struct {
  UINT8                   Function;
  UINT8                   Device;
  SC_USB_CONTROLLER_TYPE  UsbConType;
} USB_CONTROLLER;

GLOBAL_REMOVE_IF_UNREFERENCED USB_CONTROLLER  mUsbControllersMap[] = {
  {
    PCI_FUNCTION_NUMBER_PCH_LPC,
    PCI_DEVICE_NUMBER_PCH_LPC,
    ScUsbControllerLpc0
  },

  {
    PCI_FUNCTION_NUMBER_XHCI,
    PCI_DEVICE_NUMBER_XHCI,
    ScUsbControllerXhci
  }
};


/**
  Find the handle that best matches the input Device Path and return the USB controller type

  @param[in]  DevicePath           Pointer to the device Path table
  @param[out] Controller           Returned with the USB controller type of the input device path

  @retval     EFI_SUCCESS          Find the handle that best matches the input Device Path
  @retval     EFI_UNSUPPORTED      Invalid device Path table or can't find any match USB device path
                                   SC_USB_CONTROLLER_TYPE The USB controller type of the input
                                   device path

**/
EFI_STATUS
DevicePathToSupportedController (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT SC_USB_CONTROLLER_TYPE     *Controller
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     DeviceHandle;
  ACPI_HID_DEVICE_PATH           *AcpiNode;
  PCI_DEVICE_PATH                *PciNode;
  EFI_DEVICE_PATH_PROTOCOL       *RemaingDevicePath;
  UINT8                          UsbIndex;

  //
  // Find the handle that best matches the Device Path. If it is only a
  // partial match the remaining part of the device path is returned in
  // RemainingDevicePath.
  //
  RemaingDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  &DevicePath,
                  &DeviceHandle
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  DevicePath = RemaingDevicePath;

  //
  // Get first node: Acpi Node
  //
  AcpiNode = (ACPI_HID_DEVICE_PATH *) RemaingDevicePath;

  if (AcpiNode->Header.Type != ACPI_DEVICE_PATH ||
      AcpiNode->Header.SubType != ACPI_DP ||
      DevicePathNodeLength (&AcpiNode->Header) != sizeof (ACPI_HID_DEVICE_PATH) ||
      AcpiNode->HID != EISA_PNP_ID (0x0A03) ||
      AcpiNode->UID != 0
      ) {
    return EFI_UNSUPPORTED;
  } else {
    //
    // Get the next node: Pci Node
    //
    RemaingDevicePath = NextDevicePathNode (RemaingDevicePath);
    PciNode           = (PCI_DEVICE_PATH *) RemaingDevicePath;
    if (PciNode->Header.Type != HARDWARE_DEVICE_PATH ||
        PciNode->Header.SubType != HW_PCI_DP ||
        DevicePathNodeLength (&PciNode->Header) != sizeof (PCI_DEVICE_PATH)
        ) {
      return EFI_UNSUPPORTED;
    }

    for (UsbIndex = 0; UsbIndex < sizeof (mUsbControllersMap) / sizeof (USB_CONTROLLER); UsbIndex++) {
      if ((PciNode->Device == mUsbControllersMap[UsbIndex].Device) &&
          (PciNode->Function == mUsbControllersMap[UsbIndex].Function)) {
        *Controller = mUsbControllersMap[UsbIndex].UsbConType;
        return EFI_SUCCESS;
      }
    }

    return EFI_UNSUPPORTED;
  }
}


/**
  Maps a USB context to a source description.

  @param[in]  Context              The context we need to map.  Type must be USB.
  @param[out] SrcDesc              The source description that corresponds to the given context.

  @retval     None

**/
VOID
MapUsbToSrcDesc (
  IN  SC_SMM_CONTEXT         *Context,
  OUT SC_SMM_SOURCE_DESC     *SrcDesc
  )
{
  SC_USB_CONTROLLER_TYPE     Controller;
  EFI_STATUS                 Status;

  Status = DevicePathToSupportedController (Context->Usb.Device, &Controller);

  //
  // Either the device path passed in by the child is incorrect or
  // the ones stored here internally are incorrect.
  //
  ASSERT_EFI_ERROR (Status);

  switch (Context->Usb.Type) {
    case UsbLegacy:
      switch (Controller) {
        case ScUsbControllerLpc0:
          CopyMem ((VOID *) SrcDesc, (VOID *) (&mUSB1_LEGACY), sizeof (SC_SMM_SOURCE_DESC));
          break;

        case ScUsbControllerXhci:
          CopyMem ((VOID *) SrcDesc, (VOID *) (&mUSB3_LEGACY), sizeof (SC_SMM_SOURCE_DESC));
          break;

        default:
          ASSERT (FALSE);
          break;
      }
      break;

    case UsbWake:
      ASSERT (FALSE);
      break;

    default:
      ASSERT (FALSE);
      break;
  }
}

