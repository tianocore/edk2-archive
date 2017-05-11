/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UsbDeviceDxe.h"
#include <Guid/EventGroup.h>

EFI_DRIVER_BINDING_PROTOCOL mUsbDeviceDxeDriverBinding = {
  UsbDeviceDxeDriverSupported,
  UsbDeviceDxeDriverStart,
  UsbDeviceDxeDriverStop,
  0x1,
  NULL,
  NULL
};



VOID
EFIAPI
PlatformSpecificInit (
  VOID
  )
{
  UINTN                 XhciPciMmBase;
  EFI_PHYSICAL_ADDRESS  XhciMemBaseAddress;

  XhciPciMmBase   = MmPciAddress (
                      0,
                      0,
                      PCI_DEVICE_NUMBER_XHCI,
                      PCI_FUNCTION_NUMBER_XHCI,
                      0
                      );


  XhciMemBaseAddress = MmioRead32 ((UINTN) (XhciPciMmBase + R_XHCI_MEM_BASE)) & B_XHCI_MEM_BASE_BA;
  DEBUG ((DEBUG_INFO, "XhciPciMmBase=%x, XhciMemBaseAddress=%x\n", XhciPciMmBase, XhciMemBaseAddress));

  MmioWrite32 ((UINTN)(XhciMemBaseAddress + R_XHCI_MEM_DUAL_ROLE_CFG0), 0x1310800);

  PmicUSBSwitchControl (TRUE);//conduction USB switch.
  return;
}


VOID
EFIAPI
UsbDeviceDxeExitBootService (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  USB_XDCI_DEV_CONTEXT  *UsbXdciDevContext;

  UsbXdciDevContext = (USB_XDCI_DEV_CONTEXT *) Context;
  DEBUG ((EFI_D_INFO, "UsbDeviceDxeExitBootService enter\n"));

  if (UsbXdciDevContext->XdciPollTimer != NULL) {
    gBS->SetTimer (UsbXdciDevContext->XdciPollTimer, TimerCancel, 0);
    gBS->CloseEvent (UsbXdciDevContext->XdciPollTimer);
    UsbXdciDevContext->XdciPollTimer = NULL;
    }

  return;
}

/**
  The USB bus driver entry pointer.

  @param ImageHandle       The driver image handle.
  @param SystemTable       The system table.

  @return EFI_SUCCESS      The component name protocol is installed.
  @return Others           Failed to init the usb driver.

**/
EFI_STATUS
EFIAPI
UsbDeviceDxeEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &mUsbDeviceDxeDriverBinding,
           ImageHandle,
           &mUsbDeviceDxeComponentName,
           &mUsbDeviceDxeComponentName2
           );
}

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
  )
{
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  USB_CLASSC                UsbClassCReg;


  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        PCI_CLASSCODE_OFFSET,
                        sizeof (USB_CLASSC) / sizeof (UINT8),
                        &UsbClassCReg
                        );

  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  //
  // Test whether the controller belongs to USB device type
  //
  // 0x0C03FE / 0x0C0380
  //
  if ((UsbClassCReg.BaseCode != PCI_CLASS_SERIAL) ||
      (UsbClassCReg.SubClassCode != PCI_CLASS_SERIAL_USB) ||
      ((UsbClassCReg.ProgInterface != PCI_IF_USBDEV) && (UsbClassCReg.ProgInterface != 0x80))) {
    Status = EFI_UNSUPPORTED;
  }

ON_EXIT:
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}


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
  )
{
  EFI_STATUS            Status;
  USB_XDCI_DEV_CONTEXT  *UsbXdciDevContext;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  EFI_EVENT             ExitBootServicesEvent;

  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbFunIoEntryPoint - Entry\n"));

  UsbXdciDevContext = NULL;

  //
  // Provide protocol interface
  //
  //
  // Get the PCI I/O Protocol on PciHandle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  UsbXdciDevContext = AllocateZeroPool (sizeof (USB_XDCI_DEV_CONTEXT));
  if (UsbXdciDevContext == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  //
  // Initialize the driver context
  //
  UsbXdciDevContext->StartUpController = FALSE;
  UsbXdciDevContext->XdciHandle = Controller;
  UsbXdciDevContext->FirstNodePtr = NULL;
  UsbXdciDevContext->Signature = EFI_USB_DEV_SIGNATURE;

  PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               R_OTG_BAR0,
               1,
               &UsbXdciDevContext->XdciMmioBarAddr
               );

  UsbXdciDevContext->XdciMmioBarAddr &= B_OTG_BAR0_BA;
  DEBUG ((USB_FUIO_DEBUG_INFO, "USB DEV mode IO addr 0x%08x\n", UsbXdciDevContext->XdciMmioBarAddr));

  CopyMem (
    &(UsbXdciDevContext->UsbFunIoProtocol),
    &mUsbFunIoProtocol,
    sizeof (EFI_USBFN_IO_PROTOCOL)
    );

  CopyMem (
    &(UsbXdciDevContext->UsbDevModeProtocol),
    &mUsbDeviceModeProtocol,
    sizeof (EFI_USB_DEVICE_MODE_PROTOCOL)
    );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UsbDeviceDxeExitBootService,
                  UsbXdciDevContext,
                  &gEfiEventExitBootServicesGuid,
                  &ExitBootServicesEvent
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &UsbXdciDevContext->XdciHandle,
                  &gEfiUsbFnIoProtocolGuid,
                  &UsbXdciDevContext->UsbFunIoProtocol,
                  &gEfiUsbDeviceModeProtocolGuid,
                  &UsbXdciDevContext->UsbDevModeProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((USB_FUIO_DEBUG_ERROR, "ERROR - Failed to install upper protocol, Status: %r\n", Status));
    goto ErrorExit;
  }

  DEBUG ((USB_FUIO_DEBUG_LOAD, "Done - install upper protocol complete\n"));
  DEBUG ((USB_FUIO_DEBUG_LOAD, "UsbFunIoEntryPoint - Exit\n"));
  return Status;

ErrorExit:
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (UsbXdciDevContext != NULL) {
    if (UsbXdciDevContext->XdciPollTimer != NULL) {
      gBS->CloseEvent (UsbXdciDevContext->XdciPollTimer);
      UsbXdciDevContext->XdciPollTimer = NULL;
    }
    FreePool (UsbXdciDevContext);
  }

  DEBUG ((USB_FUIO_DEBUG_ERROR, "ERROR - UsbFunIoEntryPoint - Exit\n"));
  return Status;
}

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
  )
{
  EFI_USBFN_IO_PROTOCOL           *UsbFunIoProtocol;
  EFI_STATUS                      Status;
  USB_XDCI_DEV_CONTEXT            *UsbXdciDevContext;


  //
  // Locate USB_BUS for the current host controller
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbFnIoProtocolGuid,
                  (VOID **)&UsbFunIoProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );


  if (EFI_ERROR (Status)) {
    return Status;
  }

  UsbXdciDevContext = USBFUIO_CONTEXT_FROM_PROTOCOL (UsbFunIoProtocol);

  //
  // free pool
  //
  while (UsbXdciDevContext->FirstNodePtr != NULL) {
    RemoveNode (UsbFunIoProtocol, UsbXdciDevContext->FirstNodePtr);
  }

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  UsbXdciDevContext->XdciHandle,
                  &gEfiUsbFnIoProtocolGuid,
                  &UsbXdciDevContext->UsbFunIoProtocol,
                  &gEfiUsbDeviceModeProtocolGuid,
                  &UsbXdciDevContext->UsbDevModeProtocol,
                  NULL
                  );

  if (UsbXdciDevContext->StartUpController == TRUE) {
    Status = StopController (UsbFunIoProtocol);
    DEBUG ((USB_FUIO_DEBUG_INFO, "USB DEV mode STOP UsbFnDeInitDevice %r\n", Status));
  }

  if (UsbXdciDevContext->XdciPollTimer != NULL) {
    gBS->SetTimer (UsbXdciDevContext->XdciPollTimer, TimerCancel, 0);
    gBS->CloseEvent (UsbXdciDevContext->XdciPollTimer);
    UsbXdciDevContext->XdciPollTimer = NULL;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  FreePool (UsbXdciDevContext);
  return EFI_SUCCESS;
}

