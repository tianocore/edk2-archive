/** @file
  The definition for SD media device driver model and blkio protocol routines.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SdMediaDevice.h"

EFI_DRIVER_BINDING_PROTOCOL gSdMediaDeviceDriverBinding = {
  SdMediaDeviceSupported,
  SdMediaDeviceStart,
  SdMediaDeviceStop,
  0x20,
  NULL,
  NULL
};


/**
  Entry point for EFI drivers.

  @param[in]  ImageHandle      EFI_HANDLE.
  @param[in]  SystemTable      EFI_SYSTEM_TABLE.

  @retval     EFI_SUCCESS      Driver is successfully loaded.
  @retval     Others           Failed.

**/
EFI_STATUS
EFIAPI
InitializeSdMediaDevice (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gSdMediaDeviceDriverBinding,
           ImageHandle,
           &gSdMediaDeviceName,
           &gSdMediaDeviceName2
           );
}


/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has BlockIoProtocol installed will be supported.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  Controller           Handle of device to test.
  @param[in]  RemainingDevicePath  Not used.

  @retval     EFI_SUCCESS          This driver supports this device.
  @retval     EFI_UNSUPPORTED      This driver does not support this device.

**/
EFI_STATUS
EFIAPI
SdMediaDeviceSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_SD_HOST_IO_PROTOCOL   *SdHostIo;

  //
  // Test whether there is PCI IO Protocol attached on the controller handle.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSdHostIoProtocolGuid,
                  (VOID **) &SdHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiSdHostIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

Exit:
  return Status;
}


/**
  Starting the SD Media Device Driver.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  Controller           Handle of device to test.
  @param[in]  RemainingDevicePath  Not used.

  @retval     EFI_SUCCESS          This driver supports this device.
  @retval     EFI_UNSUPPORTED      This driver does not support this device.
  @retval     EFI_DEVICE_ERROR     This driver cannot be started due to device Error.
                                   EFI_OUT_OF_RESOURCES- Failed due to resource shortage.

**/
EFI_STATUS
EFIAPI
SdMediaDeviceStart (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_SD_HOST_IO_PROTOCOL   *SdHostIo;
  CARD_DATA                 *CardData;

  CardData = NULL;

  //
  // Open PCI I/O Protocol and save pointer to open protocol
  // in private data area.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSdHostIoProtocolGuid,
                  (VOID **) &SdHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SDMediaDeviceStart: Fail to open gEfiSdHostIoProtocolGuid \r\n"));
    goto Exit;
  }

  Status = SdHostIo->DetectCardAndInitHost (SdHostIo);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "SdMediaDeviceStart: Fail to DetectCardAndInitHost \r\n"));
    goto Exit;
  }

  CardData = (CARD_DATA *) AllocateZeroPool (sizeof (CARD_DATA));
  if (CardData == NULL) {
    Status =  EFI_OUT_OF_RESOURCES;
    DEBUG ((EFI_D_ERROR, "SdMediaDeviceStart: Fail to AllocateZeroPool(CARD_DATA) \r\n"));
    goto Exit;
  }

  ASSERT (SdHostIo->HostCapability.BoundarySize >= 4 * 1024);
  CardData->RawBufferPointer = (UINT8 *) ((UINTN) DMA_MEMORY_TOP);
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  EFI_SIZE_TO_PAGES (2 * SdHostIo->HostCapability.BoundarySize),
                  (EFI_PHYSICAL_ADDRESS *) (&CardData->RawBufferPointer)
                  );

  if (CardData->RawBufferPointer == NULL) {
    DEBUG ((EFI_D_ERROR, "SdMediaDeviceStart: Fail to AllocateZeroPool(2*x) \r\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  CardData->AlignedBuffer = CardData->RawBufferPointer - ((UINTN) (CardData->RawBufferPointer) & (SdHostIo->HostCapability.BoundarySize - 1)) + SdHostIo->HostCapability.BoundarySize;

  CardData->Signature = CARD_DATA_SIGNATURE;
  CardData->SdHostIo  = SdHostIo;

  Status = MMCSDCardInit (CardData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SdMediaDeviceStart: Fail to MMCSDCardInit \r\n"));
    goto Exit;
  }
  DEBUG ((EFI_D_INFO, "SdMediaDeviceStart: MMCSDCardInit SuccessFul\n"));

  if (CardData->CardType == CEATACard) {
    Status = CEATABlockIoInit (CardData);
  } else {
    Status = MMCSDBlockIoInit (CardData);
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SdMediaDeviceStart: Fail to BlockIoInit \r\n"));
    goto Exit;
  }
  DEBUG ((EFI_D_INFO, "SdMediaDeviceStart: BlockIo is successfully installed\n"));

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiBlockIoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &CardData->BlockIo
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SdMediaDeviceStart: Fail to install gEfiBlockIoProtocolGuid \r\n"));
    goto Exit;
  }

  //
  // Install the component name protocol
  //
  CardData->ControllerNameTable = NULL;

  AddUnicodeString2 (
    "eng",
    gSdMediaDeviceName.SupportedLanguages,
    &CardData->ControllerNameTable,
    L"MMC/SD Media Device",
    TRUE
    );

  AddUnicodeString2 (
    "en",
    gSdMediaDeviceName2.SupportedLanguages,
    &CardData->ControllerNameTable,
    L"MMC/Sd Media Device",
    FALSE
    );

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "SdMediaDeviceStart: End with failure\r\n"));
    if (CardData != NULL) {
      if (CardData->RawBufferPointer != NULL) {
        gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) CardData->RawBufferPointer, EFI_SIZE_TO_PAGES (2 * SdHostIo->HostCapability.BoundarySize));
      }
      FreePool (CardData);
    }
  }

  return Status;
}


/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param[in]  This                 Protocol instance pointer.
  @param[in]  Controller           Handle of device to stop driver on.
  @param[in]  NumberOfChildren     Number of Children in the ChildHandleBuffer.
  @param[in]  ChildHandleBuffer    List of handles for the children we need to stop.

  @retval     EFI_SUCCESS
  @retval     others

**/
EFI_STATUS
EFIAPI
SdMediaDeviceStop (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN UINTN                           NumberOfChildren,
  IN EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                Status;
  CARD_DATA                 *CardData;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;

  //
  // First find BlockIo Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiBlockIoProtocolGuid,
                  (VOID **) &BlockIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  CardData  = CARD_DATA_FROM_THIS (BlockIo);

  //
  // Uninstall Block I/O protocol from the device handle
  //
  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiBlockIoProtocolGuid,
                  BlockIo
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CardData != NULL) {
    if (CardData->RawBufferPointer != NULL) {
      gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) CardData->RawBufferPointer, EFI_SIZE_TO_PAGES (2 * CardData->SdHostIo->HostCapability.BoundarySize));
    }
    FreeUnicodeStringTable (CardData->ControllerNameTable);
    FreePool (CardData);
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiSdHostIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

