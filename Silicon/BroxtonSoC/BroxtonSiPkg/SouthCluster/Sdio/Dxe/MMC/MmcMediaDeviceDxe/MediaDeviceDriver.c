/** @file
  UEFI Driver Entry and Binding support.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <MediaDeviceDriver.h>
#include <Protocol/EmmcCardInfoProtocol.h>

//
// MMCSDIOController Driver Global Variables
//
EFI_DRIVER_BINDING_PROTOCOL gMediaDeviceDriverBinding = {
  MediaDeviceDriverBindingSupported,
  MediaDeviceDriverBindingStart,
  MediaDeviceDriverBindingStop,
  0x10,
  NULL,
  NULL
};

EFI_EMMC_CARD_INFO_PROTOCOL *gEfiEmmcCardInfo = NULL;

/**
  Entry point for EFI drivers.

  @param[in]  ImageHandle          EFI_HANDLE
  @param[in]  SystemTable          EFI_SYSTEM_TABLE

  @retval     EFI_SUCCESS          Success
  @retval     EFI_DEVICE_ERROR     Fail

**/
EFI_STATUS
EFIAPI
MediaDeviceDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallAllDriverProtocols (
           ImageHandle,
           SystemTable,
           &gMediaDeviceDriverBinding,
           ImageHandle,
           &gMediaDeviceComponentName,
           NULL,
           NULL
           );
}


/**
  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  that has installed will be supported.

  @param[in]  This                  Protocol instance pointer.
  @param[in]  Controlle             Handle of device to test
  @param[in]  RemainingDevicePath   Not used

  @retval     EFI_SUCCESS           This driver supports this device.
  @retval     EFI_UNSUPPORTED       This driver does not support this device.

**/
EFI_STATUS
EFIAPI
MediaDeviceDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS                 Status;
  EFI_MMC_HOST_IO_PROTOCOL   *MmcHostIo;

  //
  // Test whether there is MMCHostIO Protocol attached on the controller handle.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  (VOID **) &MmcHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiMmcHostIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

Exit:
  return Status;
}


/**
  Starting the Media Device Driver

  @param[in]  This                    Protocol instance pointer.
  @param[in]  Controller              Handle of device to test
  @param[in]  RemainingDevicePath     Not used

  @retval     EFI_SUCCESS             supports this device.
  @retval     EFI_UNSUPPORTED         do not support this device.
  @retval     EFI_DEVICE_ERROR        cannot be started due to device Error
  @retval     EFI_OUT_OF_RESOURCES    cannot allocate resources

**/
EFI_STATUS
EFIAPI
MediaDeviceDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_STATUS                   Status;
  EFI_MMC_HOST_IO_PROTOCOL     *MmcHostIo;
  CARD_DATA                    *CardData;
  UINTN                        Loop;
  EFI_EMMC_CARD_INFO_PROTOCOL  *EfiEmmcCardInfoRegister;

  DEBUG ((EFI_D_INFO, "%a(%d): %a()\n", __FILE__, __LINE__, __FUNCTION__));

  EfiEmmcCardInfoRegister = NULL;
  CardData = NULL;
  MmcHostIo = NULL;

  //
  // Alloc memory for EfiEmmcCardInfoRegister variable
  //
  EfiEmmcCardInfoRegister = (EFI_EMMC_CARD_INFO_PROTOCOL *) AllocateZeroPool (sizeof (EFI_EMMC_CARD_INFO_PROTOCOL));
  if (EfiEmmcCardInfoRegister == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Open PCI I/O Protocol and save pointer to open protocol
  // in private data area.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  (VOID **) &MmcHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStart: Fail to open gEfiMmcHostIoProtocolGuid \n"));
    goto Exit;
  }

  Status = MmcHostIo->DetectCardAndInitHost (MmcHostIo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MediaDeviceDriverBindingStart: Fail to DetectCardAndInitHost %r\n", Status));
    goto Exit;
  }

  CardData = (CARD_DATA *) AllocateZeroPool (sizeof (CARD_DATA));
  if (CardData == NULL) {
    Status =  EFI_OUT_OF_RESOURCES;
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStart: Fail to AllocateZeroPool(CARD_DATA) \n"));
    goto Exit;
  }

  ASSERT (MmcHostIo->HostCapability.BoundarySize >= 4 * 1024);

  CardData->RawBufferPointer = (UINT8*) (UINTN) 0xffffffff;

  DEBUG ((EFI_D_INFO, "CardData->RawBufferPointer = 0x%x \n",CardData->RawBufferPointer));
  DEBUG ((EFI_D_INFO, "requesting 0x%x pages \n",EFI_SIZE_TO_PAGES(2 * MmcHostIo->HostCapability.BoundarySize)));

  //
  // Allocated the buffer under 4G
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiBootServicesData,
                  EFI_SIZE_TO_PAGES (2 * MmcHostIo->HostCapability.BoundarySize),
                  (EFI_PHYSICAL_ADDRESS *) (&(CardData->RawBufferPointer))
                  );
  DEBUG ((EFI_D_INFO, "CardData->RawBufferPointer = 0x%x \n",CardData->RawBufferPointer));
  if (!EFI_ERROR (Status)) {
    CardData->RawBufferPointer = ZeroMem (CardData->RawBufferPointer, EFI_SIZE_TO_PAGES (2 * MmcHostIo->HostCapability.BoundarySize));
  } else {
    DEBUG ((DEBUG_ERROR, "MediaDeviceDriverBindingStart: Fail to AllocateZeroPool(2*x) \n"));
    Status =  EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  CardData->AlignedBuffer = CardData->RawBufferPointer - ((UINTN) (CardData->RawBufferPointer) & (MmcHostIo->HostCapability.BoundarySize - 1)) + MmcHostIo->HostCapability.BoundarySize;

  CardData->Signature = CARD_DATA_SIGNATURE;
  CardData->MmcHostIo  = MmcHostIo;
  CardData->Handle = Controller;
  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Loop++) {
    CardData->Partitions[Loop].Signature = CARD_PARTITION_SIGNATURE;
  }
  Status = MMCSDCardInit (CardData);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStart: Fail to MMCSDCardInit \n"));
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "MediaDeviceDriverBindingStart: MMC SD card\n"));
  Status = MMCSDBlockIoInit (CardData);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MediaDeviceDriverBindingStart: Card BlockIo init failed\n"));
    goto Exit;
  }


  Status = MediaDeviceDriverInstallBlockIo (This, CardData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MediaDeviceDriverBindingStart: Fail to install gEfiBlockIoProtocolGuid \n"));
    goto Exit;
  }

  //
  // Component name protocol
  //
  Status = AddUnicodeString (
             "eng",
             gMediaDeviceComponentName.SupportedLanguages,
             &CardData->ControllerNameTable,
             L"MMC/SD Media Device"
             );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Loop++) {
      if (!CardData->Partitions[Loop].Present) {
        continue;
      }
      gBS->UninstallMultipleProtocolInterfaces (
             CardData->Partitions[Loop].Handle,
             &gEfiBlockIoProtocolGuid,
             &CardData->Partitions[Loop].BlockIo,
             &gEfiDevicePathProtocolGuid,
             CardData->Partitions[Loop].DevPath,
             NULL
             );
    }
    goto Exit;
  }
  if (EfiEmmcCardInfoRegister != NULL) {

    //
    // assign to protocol
    //
    EfiEmmcCardInfoRegister->CardData = CardData;

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Controller,
                    &gEfiEmmcCardInfoProtocolGuid,
                    EfiEmmcCardInfoRegister,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "MediaDeviceDriverBindingStart: Install eMMC Card info protocol failed\n"));
      for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Loop++) {
        if (!CardData->Partitions[Loop].Present) {
          continue;
        }
        gBS->UninstallMultipleProtocolInterfaces (
               CardData->Partitions[Loop].Handle,
               &gEfiBlockIoProtocolGuid,
               &CardData->Partitions[Loop].BlockIo,
               &gEfiDevicePathProtocolGuid,
               CardData->Partitions[Loop].DevPath,
               NULL
               );
      }

      goto Exit;
    }

    gEfiEmmcCardInfo = EfiEmmcCardInfoRegister;
  }

  DEBUG ((DEBUG_INFO, "MediaDeviceDriverBindingStart: Started\n"));

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStart: End with failure\n"));
    if (CardData != NULL && MmcHostIo != NULL) {
      if (CardData->RawBufferPointer != NULL) {
        gBS->FreePages ((EFI_PHYSICAL_ADDRESS) (UINTN) CardData->RawBufferPointer, EFI_SIZE_TO_PAGES (MmcHostIo->HostCapability.BoundarySize * 2));
      }
      FreePool (CardData);
    }
    gBS->CloseProtocol (
           Controller,
           &gEfiMmcHostIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
  }
  return Status;
}


/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param[in]  This                   Protocol instance pointer.
  @param[in]  Controller             Handle of device to stop driver on
  @param[in]  NumberOfChildren       Number of Children in the ChildHandleBuffer
  @param[in]  ChildHandleBuffer      List of handles for the children we need to stop.

  @retval     EFI_SUCCESS            Success
  @retval     EFI_DEVICE_ERROR       Fail

**/
EFI_STATUS
EFIAPI
MediaDeviceDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                Status;
  CARD_DATA                 *CardData;
  BOOLEAN                   AllChildrenStopped;
  UINTN                     Index;
  UINTN                     Pages = 0;

  Status = EFI_SUCCESS;
  CardData = gEfiEmmcCardInfo->CardData;

  if (NumberOfChildren == 0) {
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    Controller,
                    &gEfiEmmcCardInfoProtocolGuid,
                    gEfiEmmcCardInfo,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStop: UNINSTALL gEfiEmmcCardInfoProtocolGuid FAILURE\n"));
      return Status;
    }

    FreeUnicodeStringTable (CardData->ControllerNameTable);

    Pages = (2 * (CardData->MmcHostIo->HostCapability.BoundarySize));
    if (CardData->RawBufferPointer != NULL) {
      FreePages (CardData->RawBufferPointer, EFI_SIZE_TO_PAGES(Pages));
    }

    Status = gBS->CloseProtocol (
         Controller,
         &gEfiMmcHostIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

    if (MediaDeviceDriverAllPartitionNotPresent(CardData)) {
      FreePool (CardData);
      FreePool (gEfiEmmcCardInfo);
      gEfiEmmcCardInfo = NULL;
    }

    return Status;
  }

  AllChildrenStopped = TRUE;
  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = MediaDeviceDriverUninstallBlockIo(This, CardData, ChildHandleBuffer[Index]);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStop: UNINSTALL block io FAILURE\n"));
      AllChildrenStopped = FALSE;
      break;
    }
  }

  return Status;
}


BOOLEAN
MediaDeviceDriverAllPartitionNotPresent (
  IN  CARD_DATA    *CardData
  )
{
  BOOLEAN             AllPartitionNotPresent;
  UINTN               Loop;
  MMC_PARTITION_DATA  *Partition;

  Partition = CardData->Partitions;

  AllPartitionNotPresent = TRUE;

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Partition++, Loop++) {
    if (Partition->Present) {
      AllPartitionNotPresent = FALSE;
      break;
    }
  }

  return AllPartitionNotPresent;
}


STATIC
struct {
  CONTROLLER_DEVICE_PATH    LogicalUnit;
  EFI_DEVICE_PATH_PROTOCOL  End;
} EmmcDevPathTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_CONTROLLER_DP,
      {
        sizeof (CONTROLLER_DEVICE_PATH),
        0
      },
    },
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};


/**
  MediaDeviceDriverInstallBlockIo

  @param[in]  This                    Pointer to the EFI_DRIVER_BINDING_PROTOCOL.
  @param[in]  CardData                Pointer to CARD_DATA

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_UNSUPPORTED
  @retval     EFI_SUCCESS

**/
EFI_STATUS
MediaDeviceDriverInstallBlockIo (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  CARD_DATA                       *CardData
  )
{
  EFI_STATUS                Status;
  UINT8                     Loop;
  MMC_PARTITION_DATA        *Partition;
  EFI_DEVICE_PATH_PROTOCOL  *MainPath;
  EFI_MMC_HOST_IO_PROTOCOL  *MmcHostIo = NULL;

  Partition = CardData->Partitions;

  Status = gBS->HandleProtocol (
                  CardData->Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &MainPath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Partition++, Loop++) {
    if (!Partition->Present) {
      continue;
    }

    DEBUG ((EFI_D_INFO, "MediaDeviceDriverInstallBlockIo: Installing Block I/O for partition %d\n", Loop));

    Partition->Handle = NULL;
    Partition->CardData = CardData;

    EmmcDevPathTemplate.LogicalUnit.ControllerNumber = Loop;
    Partition->DevPath =
      AppendDevicePath (
        MainPath,
        (EFI_DEVICE_PATH_PROTOCOL *) &EmmcDevPathTemplate
        );
    if (Partition->DevPath == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    Status = gBS->InstallProtocolInterface (
                    &(Partition->Handle),
                    &gEfiDevicePathProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    Partition->DevPath
                    );
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = gBS->InstallProtocolInterface (
                    &(Partition->Handle),
                    &gEfiBlockIoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &Partition->BlockIo
                    );

    //
    // Open parent controller by child
    //
    Status = gBS->OpenProtocol (
                    CardData->Handle,
                    &gEfiMmcHostIoProtocolGuid,
                    (VOID **) &MmcHostIo,
                    This->DriverBindingHandle,
                    Partition->Handle,
                    EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                    );
  }

  return Status;
}


/**
  MediaDeviceDriverUninstallBlockIo

  @param[in]  This                     Pointer to the EFI_DRIVER_BINDING_PROTOCOL.
  @param[in]  CardData                 Pointer to CARD_DATA
  @param[in]  Handle                   Handle of Partition

  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_UNSUPPORTED
  @retval     EFI_SUCCESS

**/
EFI_STATUS
MediaDeviceDriverUninstallBlockIo (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  CARD_DATA                    *CardData,
  IN  EFI_HANDLE                   Handle
  )
{
  EFI_STATUS          Status;
  UINTN               Loop;
  MMC_PARTITION_DATA  *Partition;

  Partition = CardData->Partitions;
  Status = EFI_SUCCESS;

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Partition++, Loop++) {
    if (!Partition->Present || Partition->Handle != Handle) {
     continue;
    }

    //
    // Close MmcHostIoProtocol by child
    //
    Status = gBS->CloseProtocol (
                    CardData->Handle,
                    &gEfiMmcHostIoProtocolGuid,
                    This->DriverBindingHandle,
                    Partition->Handle
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "CloseProtocol gEfiMmcHostIoProtocolGuid FAILURE \n"));
      return Status;
    }

    Status = gBS->UninstallMultipleProtocolInterfaces (
                    Partition->Handle,
                    &gEfiBlockIoProtocolGuid,
                    &Partition->BlockIo,
                    &gEfiDevicePathProtocolGuid,
                    Partition->DevPath,
                    NULL
                    );
    Partition->Present = FALSE;
    DEBUG ((EFI_D_ERROR, "MediaDeviceDriverBindingStop gEfiBlockIoProtocolGuid removed.  %x\n", Status));
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "MediaDeviceDriverUninstallBlockIo UNISTALL FAILURE \n"));
    }
    return Status;
  }

  return EFI_INVALID_PARAMETER;
}

