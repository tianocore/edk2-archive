/** @file
  Implementation for a generic GOP driver.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "GraphicsOutput.h"

ACPI_ADR_DEVICE_PATH mGraphicsOutputAdrNode = {
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    sizeof (ACPI_ADR_DEVICE_PATH),
    0
  },
  ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
};

/**
  Returns information for an available graphics mode that the graphics device
  and the set of active video output devices supports.

  @param[in]   This                       The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param[in]   ModeNumber                 The mode number to return information on.
  @param[out]  SizeOfInfo                 A pointer to the size, in bytes, of the Info buffer.
  @param[out]  Info                       A pointer to callee allocated buffer that returns information about ModeNumber.

  @retval      EFI_SUCCESS                Valid mode information was returned.
  @retval      EFI_DEVICE_ERROR           A hardware error occurred trying to retrieve the video mode.
  @retval      EFI_INVALID_PARAMETER      ModeNumber is not valid.

**/
EFI_STATUS
EFIAPI
GraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
  if (This == NULL || Info == NULL || SizeOfInfo == NULL || ModeNumber >= This->Mode->MaxMode) {
    return EFI_INVALID_PARAMETER;
  }

  *SizeOfInfo = This->Mode->SizeOfInfo;
  *Info       = AllocateCopyPool (*SizeOfInfo, This->Mode->Info);

  return EFI_SUCCESS;
}


/**
  Set the video device into the specified mode and clears the visible portions of
  the output display to black.

  @param[in]  This                  The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param[in]  ModeNumber            Abstraction that defines the current video mode.

  @retval     EFI_SUCCESS           The graphics mode specified by ModeNumber was selected.
  @retval     EFI_DEVICE_ERROR      The device had an error and could not complete the request.
  @retval     EFI_UNSUPPORTED       ModeNumber is not supported by this device.

**/
EFI_STATUS
EFIAPI
GraphicsOutputSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN  UINT32                       ModeNumber
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    Black;

  if (ModeNumber >= This->Mode->MaxMode) {
    return EFI_UNSUPPORTED;
  }

  Black.Blue = 0;
  Black.Green = 0;
  Black.Red = 0;
  Black.Reserved = 0;

  return BltVideoFill ((VOID *) (UINTN) This->Mode->FrameBufferBase, This->Mode->Info, &Black, 0, 0, This->Mode->Info->HorizontalResolution, This->Mode->Info->VerticalResolution);
}


/**
  Blt a rectangle of pixels on the graphics screen. Blt stands for BLock Transfer.

  @param[in]  This                   Protocol instance pointer.
  @param[in]  BltBuffer              The data to transfer to the graphics screen.
                                     Size is at least Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL).
  @param[in]  BltOperation           The operation to perform when copying BltBuffer on to the graphics screen.
  @param[in]  SourceX                The X coordinate of source for the BltOperation.
  @param[in]  SourceY                The Y coordinate of source for the BltOperation.
  @param[in]  DestinationX           The X coordinate of destination for the BltOperation.
  @param[in]  DestinationY           The Y coordinate of destination for the BltOperation.
  @param[in]  Width                  The width of a rectangle in the blt rectangle in pixels.
  @param[in]  Height                 The height of a rectangle in the blt rectangle in pixels.
  @param[in]  Delta                  Not used for EfiBltVideoFill or the EfiBltVideoToVideo operation.
                                     If a Delta of zero is used, the entire BltBuffer is being operated on.
                                     If a subrectangle of the BltBuffer is being used then Delta
                                     represents the number of bytes in a row of the BltBuffer.

  @retval     EFI_SUCCESS            BltBuffer was drawn to the graphics screen.
  @retval     EFI_INVALID_PARAMETER  BltOperation is not valid.
  @retval     EFI_DEVICE_ERROR       The device had an error and could not complete the request.

**/
EFI_STATUS
EFIAPI
GraphicsOutputBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL      *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
  IN  UINTN                             SourceX,
  IN  UINTN                             SourceY,
  IN  UINTN                             DestinationX,
  IN  UINTN                             DestinationY,
  IN  UINTN                             Width,
  IN  UINTN                             Height,
  IN  UINTN                             Delta         OPTIONAL
  )
{
  EFI_STATUS                            Status;
  EFI_TPL                               Tpl;
  //
  // We have to raise to TPL_NOTIFY, so we make an atomic write to the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  switch (BltOperation) {
    case EfiBltVideoToBltBuffer:
      Status = BltVideoToBuffer (
                 (VOID *) (UINTN) This->Mode->FrameBufferBase,
                 This->Mode->Info,
                 BltBuffer,
                 SourceX,
                 SourceY,
                 DestinationX,
                 DestinationY,
                 Width,
                 Height,
                 Delta
                 );
      break;

    case EfiBltVideoToVideo:
      Status = BltVideoToVideo (
                 (VOID *) (UINTN) This->Mode->FrameBufferBase,
                 This->Mode->Info,
                 SourceX,
                 SourceY,
                 DestinationX,
                 DestinationY,
                 Width,
                 Height
                 );
      break;

    case EfiBltVideoFill:
      Status = BltVideoFill (
                 (VOID *) (UINTN) This->Mode->FrameBufferBase,
                 This->Mode->Info,
                 BltBuffer,
                 DestinationX,
                 DestinationY,
                 Width,
                 Height
                 );
      break;

    case EfiBltBufferToVideo:
      Status = BltBufferToVideo (
                 (VOID *) (UINTN) This->Mode->FrameBufferBase,
                 This->Mode->Info,
                 BltBuffer,
                 SourceX,
                 SourceY,
                 DestinationX,
                 DestinationY,
                 Width,
                 Height,
                 Delta
                 );
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  gBS->RestoreTPL (Tpl);

  return Status;
}


GRAPHICS_OUTPUT_PRIVATE_DATA        mGraphicsOutputInstance = {
  NULL,                                            // GraphicsOutputHandle
  {
    GraphicsOutputQueryMode,
    GraphicsOutputSetMode,
    GraphicsOutputBlt,
    &mGraphicsOutputInstance.GraphicsOutputMode
  },
  {
    1,                                             // MaxMode
    0,                                             // Mode
    NULL,                                          // Info
    sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION), // SizeOfInfo
    0,                                             // FrameBufferBase
    0                                              // FrameBufferSize
  },
  NULL,                                            // DevicePath
  NULL,                                            // PciIo
  0                                                // PciAttributes
};


/**
  Test whether the Controller can be managed by the driver.

  @param[in]  This                     Driver Binding protocol instance pointer.
  @param[in]  Controller               The PCI controller.
  @param[in]  RemainingDevicePath      Optional parameter use to pick a specific child
                                       device to start.

  @retval     EFI_SUCCESS              The driver can manage the video device.
  @retval     other                    The driver cannot manage the video device.

**/
EFI_STATUS
EFIAPI
GraphicsOutputDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;

  //
  // Since there is only one GraphicsInfo HOB, the driver only manages one video device.
  //
  if (mGraphicsOutputInstance.GraphicsOutputHandle != NULL) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Test the PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    Status = EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Test the DevicePath protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    Status = EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if ((RemainingDevicePath == NULL) ||
      IsDevicePathEnd (RemainingDevicePath) ||
      CompareMem (RemainingDevicePath, &mGraphicsOutputAdrNode, sizeof (mGraphicsOutputAdrNode)) == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


/**
  Start the video controller.

  @param[in]  This                     Driver Binding protocol instance pointer.
  @param[in]  ControllerHandle         The PCI controller.
  @param[in]  RemainingDevicePath      Optional parameter use to pick a specific child
                                       device to start.

  @retval     EFI_SUCCESS              The driver starts to manage the video device.
  @retval     other                    The driver cannot manage the video device.

**/
EFI_STATUS
EFIAPI
GraphicsOutputDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  EFI_HANDLE                        Handle;
  EFI_DEVICE_PATH                   *PciDevicePath;
  PCI_TYPE00                        Pci;
  UINT8                             Index;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Resources;

  //
  // Open the PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    Status = EFI_SUCCESS;
  }
  ASSERT_EFI_ERROR (Status);

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &PciDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    Status = EFI_SUCCESS;
  }
  ASSERT_EFI_ERROR (Status);

  //
  // Read the PCI Class Code from the PCI Device
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        OFFSET_OF (PCI_TYPE00, Hdr.ClassCode),
                        sizeof (Pci.Hdr.ClassCode),
                        &Pci.Hdr.ClassCode
                        );
  if (!EFI_ERROR (Status)) {
    if (!IS_PCI_DISPLAY (&Pci)) {
      Status = EFI_UNSUPPORTED;
    } else {
      //
      // If it's a video device, check whether the device's frame buffer size matches the one in HOB.
      // And update the frame buffer base.
      //
      for (Index = 1; Index < MAX_PCI_BAR; Index++) {
        Status = PciIo->GetBarAttributes (PciIo, Index, NULL, (VOID**) &Resources);
        if (!EFI_ERROR (Status)) {
          DEBUG ((EFI_D_INFO, "[GraphicsOutputDxe]: BAR[%d]: Base = %lx, Length = %lx\n", Index, Resources->AddrRangeMin, Resources->AddrLen));
          if (Resources->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR &&
              Resources->Len == (UINT16) (sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) - 3) &&
              Resources->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM &&
              Resources->AddrLen >= mGraphicsOutputInstance.GraphicsOutputMode.FrameBufferSize) {
            mGraphicsOutputInstance.GraphicsOutputMode.FrameBufferBase = Resources->AddrRangeMin;
            if (!EFI_ERROR (Status)) {
              DEBUG ((EFI_D_INFO, "[GraphicsOutputDxe]: ... matched!\n"));
              break;
            }
          }
        }
      }
      if (Index == MAX_PCI_BAR) {
        Status = EFI_UNSUPPORTED;
      }
    }
  }

  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Set attributes
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationGet,
                    0,
                    &mGraphicsOutputInstance.PciAttributes
                    );
  if (!EFI_ERROR (Status)) {
    Status = PciIo->Attributes (
                      PciIo,
                      EfiPciIoAttributeOperationEnable,
                      EFI_PCI_DEVICE_ENABLE,
                      NULL
                      );
  }

  if (EFI_ERROR (Status)) {
    goto Error;
  }

  if ((RemainingDevicePath != NULL) && IsDevicePathEnd (RemainingDevicePath)) {
    return EFI_SUCCESS;
  }


  mGraphicsOutputInstance.DevicePath = AppendDevicePathNode (PciDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &mGraphicsOutputAdrNode);
  ASSERT (mGraphicsOutputInstance.DevicePath != NULL);

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mGraphicsOutputInstance.GraphicsOutputHandle,
                  &gEfiGraphicsOutputProtocolGuid, &mGraphicsOutputInstance.GraphicsOutput,
                  &gEfiDevicePathProtocolGuid, mGraphicsOutputInstance.DevicePath,
                  NULL
                  );

  if (!EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &mGraphicsOutputInstance.PciIo,
                    This->DriverBindingHandle,
                    mGraphicsOutputInstance.GraphicsOutputHandle,
                    EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                    );
    if (EFI_ERROR (Status)) {
      gBS->UninstallMultipleProtocolInterfaces (
             mGraphicsOutputInstance.GraphicsOutputHandle,
             &gEfiGraphicsOutputProtocolGuid, &mGraphicsOutputInstance.GraphicsOutput,
             &gEfiDevicePathProtocolGuid, mGraphicsOutputInstance.DevicePath,
             NULL
             );
    }
  }

  if (EFI_ERROR (Status)) {
    mGraphicsOutputInstance.GraphicsOutputHandle = NULL;
    //
    // Restore original PCI attributes
    //
    PciIo->Attributes (
             PciIo,
             EfiPciIoAttributeOperationSet,
             mGraphicsOutputInstance.PciAttributes,
             NULL
             );
    FreePool (mGraphicsOutputInstance.DevicePath);
  }

Error:
  if (EFI_ERROR (Status)) {
    //
    // Close the PCI I/O Protocol
    //
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );

    //
    // Close the PCI I/O Protocol
    //
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
  }

  return Status;
}


/**
  Stop the video controller.

  @param[in]  This                     Driver Binding protocol instance pointer.
  @param[in]  Controller               The PCI controller.
  @param[in]  NumberOfChildren         The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer        An array of child handles to be freed. May be NULL
                                       if NumberOfChildren is 0.

  @retval     EFI_SUCCESS              The device was stopped.
  @retval     EFI_DEVICE_ERROR         The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
GraphicsOutputDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN UINTN                          NumberOfChildren,
  IN EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                      Status;

  if (NumberOfChildren == 0) {
    //
    // Restore original PCI attributes
    //
    Status = mGraphicsOutputInstance.PciIo->Attributes (
                                              mGraphicsOutputInstance.PciIo,
                                              EfiPciIoAttributeOperationSet,
                                              mGraphicsOutputInstance.PciAttributes,
                                              NULL
                                              );
    ASSERT_EFI_ERROR (Status);

    //
    // Close the PCI I/O Protocol
    //
    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  }

  ASSERT (NumberOfChildren == 1);
  ASSERT (ChildHandleBuffer[0] == mGraphicsOutputInstance.GraphicsOutputHandle);
  Status = gBS->CloseProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  This->DriverBindingHandle,
                  mGraphicsOutputInstance.GraphicsOutputHandle
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Remove the GOP protocol interface from the system
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  mGraphicsOutputInstance.GraphicsOutputHandle,
                  &gEfiGraphicsOutputProtocolGuid, &mGraphicsOutputInstance.GraphicsOutput,
                  &gEfiDevicePathProtocolGuid,     mGraphicsOutputInstance.DevicePath,
                  NULL
                  );
  if (!EFI_ERROR (Status)) {
    mGraphicsOutputInstance.GraphicsOutputHandle = NULL;
    FreePool (mGraphicsOutputInstance.DevicePath);
  } else {
    Status = gBS->OpenProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &mGraphicsOutputInstance.PciIo,
                    This->DriverBindingHandle,
                    mGraphicsOutputInstance.GraphicsOutputHandle,
                    EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

EFI_DRIVER_BINDING_PROTOCOL mGraphicsOutputDriverBinding = {
  GraphicsOutputDriverBindingSupported,
  GraphicsOutputDriverBindingStart,
  GraphicsOutputDriverBindingStop,
  0xffffffff,
  NULL,
  NULL
};


/**
  The Entry Point for GraphicsOutput driver.

  It installs DriverBinding, ComponentName and ComponentName2 protocol if there is
  GraphicsInfo HOB passed from Graphics PEIM.

  @param[in] ImageHandle       The firmware allocated handle for the EFI image.
  @param[in] SystemTable       A pointer to the EFI System Table.

  @retval    EFI_SUCCESS       The entry point is executed successfully.
  @retval    other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeGraphicsOutput (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  VOID                                 *HobStart;
  EFI_PEI_GRAPHICS_INFO_HOB            *GraphicsInfo;

  HobStart = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);

  if ((HobStart == NULL) || (GET_GUID_HOB_DATA_SIZE (HobStart) != sizeof (*GraphicsInfo))) {
    return EFI_NOT_FOUND;
  }

  GraphicsInfo = (EFI_PEI_GRAPHICS_INFO_HOB *) (GET_GUID_HOB_DATA (HobStart));

  //
  // Cache in global data
  //
  DEBUG ((EFI_D_INFO, "FrameBuffer = %lx/%lx\n", GraphicsInfo->FrameBufferBase, GraphicsInfo->FrameBufferSize));
  mGraphicsOutputInstance.GraphicsOutputMode.Info = &GraphicsInfo->GraphicsMode;
  mGraphicsOutputInstance.GraphicsOutputMode.FrameBufferBase = GraphicsInfo->FrameBufferBase;
  mGraphicsOutputInstance.GraphicsOutputMode.FrameBufferSize = GraphicsInfo->FrameBufferSize;

  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mGraphicsOutputDriverBinding,
             ImageHandle,
             &mGraphicsOutputComponentName,
             &mGraphicsOutputComponentName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

