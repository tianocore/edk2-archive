/** @file
  UEFI Driver Entry and Binding support.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MmcHostDriver.h"
#include <PiDxe.h>

//
// MMCSDIOController Driver Global Variables
//
EFI_DRIVER_BINDING_PROTOCOL gMmcHostDriverBinding = {
  MmcHostDriverBindingSupported,
  MmcHostDriverBindingStart,
  MmcHostDriverBindingStop,
  0x10,
  NULL,
  NULL
};


EFI_MMC_HOST_IO_PROTOCOL  mMmcHostIo = {
  EFI_MMC_HOST_IO_PROTOCOL_REVISION_01,
  {
    0, ///< HighSpeedSupport
    0, ///< V18Support
    0, ///< V30Support
    0, ///< V33Support
    0, ///< HS400Support
    0, ///< BusWidth4
    0, ///< BusWidth8
    0, ///< Reserved1
    0,
    0,
    0,
    0,
    0, ///< ADMA2Support
    0, ///< DmaMode
    0, ///< ReTune Timer
    0, ///< ReTune Mode
    0, ///< Reserved2
    (512 * 1024) ///<BoundarySize 512 KB
  },
  SendCommand,
  SetClockFrequency,
  SetBusWidth,
  SetHostVoltage,
  SetHostDdrMode,
  SetHostSdrMode,
  ResetMmcHost,
  EnableAutoStopCmd,
  DetectCardAndInitHost,
  SetBlockLength,
  SetupDevice,
  SetHostSpeedMode
};


/**
  Entry point for MMC Host EFI drivers.

  @param[in] ImageHandle          EFI_HANDLE
  @param[in] SystemTable          EFI_SYSTEM_TABLE

  @retval    EFI_SUCCESS          The function completed successfully
  @retval    EFI_DEVICE_ERROR     The function failed to complete

**/
EFI_STATUS
EFIAPI
MmcHostDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EfiLibInstallAllDriverProtocols (
           ImageHandle,
           SystemTable,
           &gMmcHostDriverBinding,
           ImageHandle,
           &gMmcHostComponentName,
           NULL,
           NULL
           );
}


/**
  Test to see if this MMC Host driver supports ControllerHandle.
  Any ControllerHandle that has installed will be supported.

  @param[in] This                    Protocol instance pointer
  @param[in] Controller              Handle of device to test
  @param[in] RemainingDevicePath     Not used

  @retval    EFI_SUCCESS             This driver supports this device
  @retval    EFI_UNSUPPORTED         This driver does not support this device

**/
EFI_STATUS
EFIAPI
MmcHostDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS                     Status;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  EFI_MMC_HOST_IO_PROTOCOL       *MmcHostIo;
  PCI_CLASSC                     PciClass;
  UINT32                         VidDid;
  UINT32                         Bar0 = 0;
  UINT32                         Bar1 = 0;
  UINTN                          Seg, Bus, Dev, Func;

  //
  // Verify the MMC IO Protocol, which installed by the
  // IdeController module.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  (VOID **) &MmcHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "MmcHost controller already started, Controller: 0x%016Lx\r\n",  (UINT64) (UINTN) Controller));
    Status = EFI_ALREADY_STARTED;
    return Status;
  }

  //
  // Test whether there is PCI IO Protocol attached on the controller handle.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_VENDOR_ID_OFFSET,
                        1,
                        &VidDid
                        );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        PCI_CLASSCODE_OFFSET,
                        sizeof (PCI_CLASSC) / sizeof (UINT8),
                        &PciClass
                        );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_BASE_ADDRESSREG_OFFSET,
                        1,
                        &Bar0
                        );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_BASE_ADDRESSREG_OFFSET + 4,
                        1,
                        &Bar1
                        );
  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  Status = PciIo->GetLocation (
                    PciIo,
                    &Seg,
                    &Bus,
                    &Dev,
                    &Func
                    );

  if (EFI_ERROR (Status)) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  if ((Seg != 0) || (Bus != 0) || (Dev != 28) || (Func != 0)) {
    //
    // This is not the eMMC controller, bail.
    //
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  if ((PciClass.BaseCode != PCI_CLASS_SYSTEM_PERIPHERAL) ||
     (PciClass.SubClassCode != PCI_SUBCLASS_SD_HOST_CONTROLLER) ||
     ((PciClass.PI != PCI_IF_STANDARD_HOST_NO_DMA) && (PciClass.PI != PCI_IF_STANDARD_HOST_SUPPORT_DMA))
     ) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported0: Seg %d, bus:%d, Dev:%d, Func:%d\n", Seg, Bus, Dev, Func));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported1: VidDid %08x\n", VidDid));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported1: Base Code %x\n", PciClass.BaseCode));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported1: SubClassCode %x\n", PciClass.SubClassCode));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported1: PI %x\n", PciClass.PI));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported0: MEMIO Base0 %x\n", Bar0));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingSupported0: MEMIO Base1 %x\n", Bar1));

Exit:
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  return Status;
}


/**
  Starting the MMC Host Driver

  @param[in] This                     Protocol instance pointer
  @param[in] Controller               Handle of device to start
  @param[in] RemainingDevicePath      Not used

  @retval    EFI_SUCCESS              This driver start this device
  @retval    EFI_UNSUPPORTED          This driver does not support this device
  @retval    EFI_DEVICE_ERROR         This driver cannot be started due to device Error
  @retval    EFI_OUT_OF_RESOURCES     This driver cannot allocate resources

**/
EFI_STATUS
EFIAPI
MmcHostDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  )
{
  EFI_PCI_IO_PROTOCOL                *PciIo;
  EFI_STATUS                         Status;
  MMCHOST_DATA                       *MmcHostData;
  UINT32                             Data;
  UINT16                             Data16;
  UINT32                             VidDid;
  UINT32                             Bar0 = 0;
  UINT32                             Bar1 = 0;
  UINTN                              Seg, Bus, Dev, Func;

  MmcHostData = NULL;
  Data       = 0;
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Start\n"));

  //
  // Open PCI I/O Protocol and save pointer to open protocol
  // in private data area.
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
    goto Exit;
  }

  //
  // Enable the MMC Host Controller MMIO space
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationEnable,
                    EFI_PCI_DEVICE_ENABLE,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }


  MmcHostData = (MMCHOST_DATA *) AllocateZeroPool (sizeof (MMCHOST_DATA));
  if (MmcHostData == NULL) {
    Status =  EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  MmcHostData->Signature   = MMCHOST_DATA_SIGNATURE;
  MmcHostData->PciIo       = PciIo;

  CopyMem (&MmcHostData->MmcHostIo, &mMmcHostIo, sizeof (EFI_MMC_HOST_IO_PROTOCOL));

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_VENDOR_ID_OFFSET,
                        1,
                        &VidDid
                        );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: VidDid = 0x%08x\n", VidDid));

  Status = PciIo->GetLocation (
                    PciIo,
                    &Seg,
                    &Bus,
                    &Dev,
                    &Func
                    );
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Seg %d, bus:%d,Dev:%d,Func:%d\n", Seg, Bus, Dev,Func));

  MmcHostData->PciVid = (UINT16) (VidDid & 0xffff);
  MmcHostData->PciDid = (UINT16) (VidDid >> 16);

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_BASE_ADDRESSREG_OFFSET,
                        1,
                        &Bar0
                        );

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        PCI_BASE_ADDRESSREG_OFFSET + 4,
                        1,
                        &Bar1
                        );

  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MEMIO Base0 %x\n", Bar0));
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MEMIO Base1 %x\n", Bar1));


  MmcHostData->MmcHostIo.ResetMmcHost (&MmcHostData->MmcHostIo, Reset_All);
  MmcHostData->EnableVerboseDebug = FALSE;

  PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint16,
               0,
               (UINT64) MMIO_CTRLRVER,
               1,
               &Data16
               );

  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MMIO_CTRLRVER = 0x%08x\n", Data16));

  MmcHostData->ControllerVersion = Data16 & 0xFF;
  switch (MmcHostData->ControllerVersion) {
    case 0: DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MMC Host Controller Version 1.0\n")); break;
    case 1: DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MMC Host Controller Version 2.0\n")); break;
    case 2: DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: MMC Host Controller Version 3.0\n")); break;
    default:
      DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Unknown MMC Host Controller Version, Stopping Driver!!\n"));
      goto Exit;
  }

  PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0,
               (UINT32) MMIO_CAP,
               1,
               &Data
               );
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Low MMIO_CAP = 0x%08x\n", Data));

  if ((Data & BIT18) != 0) {
    MmcHostData->MmcHostIo.HostCapability.BusWidth8 = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: BusWidth8\n"));
  }

  MmcHostData->MmcHostIo.HostCapability.BusWidth4 = TRUE;
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: BusWidth4\n"));

  if ((Data & BIT19) != 0) {
    MmcHostData->MmcHostIo.HostCapability.ADMA2Support = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: ADMA2Support\n"));
  }

  if ((Data & BIT21) != 0) {
    MmcHostData->MmcHostIo.HostCapability.HighSpeedSupport = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: HighSpeedSupport\n"));
  }

  if ((Data & BIT22) != 0) {
    MmcHostData->MmcHostIo.HostCapability.SDMASupport = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: SDMASupport\n"));
  }

  if ((Data & BIT24) != 0) {
    MmcHostData->MmcHostIo.HostCapability.V33Support = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: V33Support\n"));
  }

  if ((Data & BIT25) != 0) {
    MmcHostData->MmcHostIo.HostCapability.V30Support = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: V30Support\n"));
  }

  if ((Data & BIT26) != 0) {
    MmcHostData->MmcHostIo.HostCapability.V18Support = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: V18Support\n"));
  }

  if (((Data) & (BIT30 | BIT31)) == 0) {
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Removable Card Slot\n"));
  } else if ((Data & BIT30) && (Data & (~BIT31))) {
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Embedded Slot for One Device\n"));
  }
  MmcHostData->BaseClockInMHz = (Data >> 8) & 0xFF;

  PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0,
               (UINT32) (MMIO_CAP + 4),
               1,
               &Data
               );

  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: High MMIO_CAP = 0x%08x\n", Data));



  if ((Data & 0x1<<(32-32)) != 0) {
    MmcHostData->MmcHostIo.HostCapability.SDR50Support= TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: SDR50Support\n"));
  }

  if ((Data & 0x1<<(33-32)) != 0) {
    MmcHostData->MmcHostIo.HostCapability.SDR104Support= TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: SDR104Support\n"));
  }

  if ((Data & 0x1<<(34-32)) != 0) {
    MmcHostData->MmcHostIo.HostCapability.DDR50Support= TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: DDR50Support\n"));
  }

  if ((Data & 0x1<<(63-32)) != 0) {
    MmcHostData->MmcHostIo.HostCapability.HS400Support = TRUE;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: HS400Support\n"));
  }

  if (MmcHostData->ControllerVersion >= 2) {
    MmcHostData->MmcHostIo.HostCapability.ReTuneMode = (Data >> (46-32)) & 0x3;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart:  ReTuneMode = 0x%08x\n", MmcHostData->MmcHostIo.HostCapability.ReTuneMode));

    MmcHostData->MmcHostIo.HostCapability.ReTuneTimer = (Data>>(40-32)) & 0xF;
    DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart:  ReTuneTimer = 0x%08x\n", MmcHostData->MmcHostIo.HostCapability.ReTuneTimer));
  }


  MmcHostData->BlockLength    = BLOCK_SIZE;
  MmcHostData->IsAutoStopCmd  = TRUE;

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &MmcHostData->MmcHostIo
                  );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Component name protocol
  //
  Status = AddUnicodeString (
             "eng",
             gMmcHostComponentName.SupportedLanguages,
             &MmcHostData->ControllerNameTable,
             L"MMC Host Controller"
             );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    gBS->UninstallProtocolInterface (
           Controller,
           &gEfiMmcHostIoProtocolGuid,
           &MmcHostData->MmcHostIo
           );
  }

  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStart: Host Started\n"));

Exit:
  if (EFI_ERROR (Status)) {
    if (MmcHostData != NULL) {
      FreePool (MmcHostData);
    }
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
  Stop MMC Host driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param[in] This                  Protocol instance pointer
  @param[in] Controller            Handle of device to stop driver on
  @param[in] NumberOfChildren      Number of Children in the ChildHandleBuffer
  @param[in] ChildHandleBuffer     List of handles for the children we need to stop

  @retval    EFI_SUCCESS           This driver stop this device
  @retval    EFI_DEVICE_ERROR      This driver cannot be stop due to device Error

**/
EFI_STATUS
EFIAPI
MmcHostDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                          Status;
  EFI_MMC_HOST_IO_PROTOCOL            *MmcHostIo;
  MMCHOST_DATA                        *MmcHostData;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  (VOID **) &MmcHostIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    goto _exit_MmcHostDriverBindingStop;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  MmcHostData  = MMCHOST_DATA_FROM_THIS (MmcHostIo);

  //
  // Uninstall Block I/O protocol from the device handle
  //
  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiMmcHostIoProtocolGuid,
                  &MmcHostData->MmcHostIo
                  );
  if (EFI_ERROR (Status)) {
    goto _exit_MmcHostDriverBindingStop;
  }

  FreeUnicodeStringTable (MmcHostData->ControllerNameTable);
  FreePool (MmcHostData);

_exit_MmcHostDriverBindingStop:
  DEBUG ((EFI_D_INFO, "MmcHostDriverBindingStop exited with Status %r\n", Status));

  return Status;
}

