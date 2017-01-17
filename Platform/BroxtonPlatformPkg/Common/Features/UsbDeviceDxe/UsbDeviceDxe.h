/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __USB_DEVICE_DXE_H__
#define __USB_DEVICE_DXE_H__

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DriverLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Protocol/EfiUsbFnIo.h>
#include <Protocol/UsbDeviceModeProtocol.h>
#include <PlatformBaseAddresses.h>
#include <ScAccess.h>
#include "UsbFuncIo.h"
#include "UsbDeviceMode.h"


#define PCI_IF_USBDEV                      0xFE

#define EFI_USB_DEV_SIGNATURE              0x55534244 //"USBD"
#define USBFUIO_CONTEXT_FROM_PROTOCOL(a)   CR (a, USB_XDCI_DEV_CONTEXT, UsbFunIoProtocol, EFI_USB_DEV_SIGNATURE)
#define USBUSBD_CONTEXT_FROM_PROTOCOL(a)   CR (a, USB_XDCI_DEV_CONTEXT, UsbDevModeProtocol, EFI_USB_DEV_SIGNATURE)


typedef struct _USB_FUIO_EVENT_NODE   USB_FUIO_EVENT_NODE;

#pragma pack(1)
struct _USB_FUIO_EVENT_NODE{
  EFI_USBFN_MESSAGE            Message;
  UINTN                        PayloadSize;
  EFI_USBFN_MESSAGE_PAYLOAD    Payload;
  USB_FUIO_EVENT_NODE          *Nextptr;
};

typedef struct {
  UINTN                         Signature;
  UINTN                         XdciMmioBarAddr;
  EFI_HANDLE                    XdciHandle;
  //
  // Timer to handle EndPoint event periodically.
  //
  EFI_EVENT                     XdciPollTimer;
  EFI_USB_DEVICE_MODE_PROTOCOL  UsbDevModeProtocol;
  EFI_USBFN_IO_PROTOCOL         UsbFunIoProtocol;

  //
  // Structure members used by UsbFunIoProtocol.
  //
  USB_MEM_NODE                  *FirstNodePtr;
  EFI_USB_DEVICE_INFO           *DevInfoPtr;
  EFI_USB_CONFIG_INFO           IndexPtrConfig;
  EFI_USB_INTERFACE_INFO        IndexPtrInteface;
  USB_DEVICE_ENDPOINT_INFO      IndexPtrInEp;
  USB_DEVICE_ENDPOINT_INFO      IndexPtrOutEp;
  XDCI_CORE_HANDLE              *XdciDrvIfHandle;
  USB_DEV_CORE                  *DrvCore;
  UINT16                        VendorId;
  UINT16                        DeviceId;
  USBD_EP_XFER_REC              EndPointXferRec[DWC_XDCI_MAX_ENDPOINTS];
  BOOLEAN                       StartUpController;
  BOOLEAN                       DevReConnect;
  BOOLEAN                       DevResetFlag;
  EFI_EVENT                     TimerEvent;
  USB_FUIO_EVENT_NODE           *EventNodePtr;
  //
  // Following structure members are used by UsbDevModeProtocol.
  //

} USB_XDCI_DEV_CONTEXT;
#pragma pack()



/**
  Check whether USB bus driver support this device.

  @param  This                   The USB bus driver binding protocol.
  @param  Controller             The controller handle to check.
  @param  RemainingDevicePath    The remaining device path.

  @retval EFI_SUCCESS            The bus supports this controller.
  @retval EFI_UNSUPPORTED        This device isn't supported.

**/
EFI_STATUS
EFIAPI
UsbDeviceDxeDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Start to process the controller.

  @param  This                   The USB bus driver binding instance.
  @param  Controller             The controller to check.
  @param  RemainingDevicePath    The remaining device patch.

  @retval EFI_SUCCESS            The controller is controlled by the usb bus.
  @retval EFI_ALREADY_STARTED    The controller is already controlled by the usb
                                 bus.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources.

**/
EFI_STATUS
EFIAPI
UsbDeviceDxeDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Stop handle the controller by this USB bus driver.

  @param  This                   The USB bus driver binding protocol.
  @param  Controller             The controller to release.
  @param  NumberOfChildren       The child of USB bus that opened controller
                                 BY_CHILD.
  @param  ChildHandleBuffer      The array of child handle.

  @retval EFI_SUCCESS            The controller or children are stopped.
  @retval EFI_DEVICE_ERROR       Failed to stop the driver.

**/
EFI_STATUS
EFIAPI
UsbDeviceDxeDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
  );

VOID
EFIAPI
PlatformSpecificInit (
  VOID
  );

extern EFI_COMPONENT_NAME_PROTOCOL    mUsbDeviceDxeComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL   mUsbDeviceDxeComponentName2;

#endif

