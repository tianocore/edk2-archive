/** @file
  This driver module produces IDE_CONTROLLER_INIT protocol for serial ATA
  driver and will be used by IDE Bus driver to support chipset dependent timing
  information, config SATA control/status registers. This driver
  is responsible for early initialization of serial ATA controller.

  Serial ATA spec requires SATA controller compatible with parallel IDE
  controller. That's why lots of code here is the same with IDE controller
  driver. However, We need this driver to optimize timing settings for SATA
  device and set SATA config/error/status registers.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SataController.h"

//
//  EFI_DRIVER_BINDING_PROTOCOL instance
//
EFI_DRIVER_BINDING_PROTOCOL mSataControllerDriverBinding = {
  SataControllerSupported,
  SataControllerStart,
  SataControllerStop,
  1,
  NULL,
  NULL
};

extern EFI_COMPONENT_NAME2_PROTOCOL mSataControllerName;

//
// Internal function definitions
//
EFI_STATUS
CalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA      *IdentifyData,
  IN  UINT16                 *DisPioMode OPTIONAL,
  OUT UINT16                 *SelectedMode
  );

EFI_STATUS
CalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA      *IdentifyData,
  IN  UINT16                 *DisUDmaMode OPTIONAL,
  OUT UINT16                 *SelectedMode
  );


/**
  Chipset SATA Driver EntryPoint function. It follows the standard EFI driver
  model. It's called by StartImage() of DXE Core

  @param[in] ImageHandle                While the driver image loaded be the ImageLoader(),
                                        an image handle is assigned to this driver binary,
                                        all activities of the driver is tied to this ImageHandle
  @param[in] SystemTable                A pointer to the system table, for all BS(Boo Services) and
                                        RT(Runtime Services)

  @retval    EFI_SUCCESS                Function completes successfully

**/
EFI_STATUS
EFIAPI
InitializeSataControllerDriver (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mSataControllerDriverBinding,
             ImageHandle,
             NULL,
             &mSataControllerName
             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.
  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                          A pointer points to the Binding Protocol instance
  @param[in] Controller                    The handle of controller to be tested.
  @param[in] RemainingDevicePath           A pointer to the device path. Ignored by device
                                           driver but used by bus driver

  @retval    EFI_SUCCESS                   The device is supported
  @retval    EFI_UNSUPPORTED               The device is not supported

**/
EFI_STATUS
EFIAPI
SataControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  UINT32                                SataDeviceIdFound;
  EFI_DEVICE_PATH_PROTOCOL              *ParentDevicePath;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            PciData;

  //
  // SATA Controller is a device driver, and should ingore the
  // "RemainingDevicePath" according to EFI spec
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID *) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    //
    // EFI_ALREADY_STARTED is also an error
    //
    return Status;
  }
  //
  // Close the protocol because we don't use it here
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Now test the EfiPciIoProtocol
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
    return Status;
  }
  //
  // Now further check the PCI header: Base class (offset 0x0B) and
  // Sub Class (offset 0x0A). This controller should be an SATA controller
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData),
                        &PciData
                        );
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_UNSUPPORTED;
  }
  //
  // Since we already got the PciData, we can close protocol to avoid to carry it on for multiple exit points.
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Examine SATA PCI Configuration table fields
  //
  SataDeviceIdFound = FALSE;
  //
  // When found is storage device and provided by Intel then detect for right device Ids
  //
  if (PciData.Hdr.VendorId == V_SATA_VENDOR_ID) {

    if ((PciData.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE)) {
      if (PciData.Hdr.ClassCode[1] == V_SATA_CC_SCC_AHCI) {
        if (IS_BXTP_SATA_AHCI_DEVICE_ID (PciData.Hdr.DeviceId)) {
          SataDeviceIdFound = TRUE;
        }
      }
    }
  }

  if (!SataDeviceIdFound) {
    return EFI_UNSUPPORTED;
  }

  return Status;
}


/**
  This routine is called right after the .Supported() is called and returns
  EFI_SUCCESS. Notes: The supported protocols are checked but the Protocols
  are closed.

  @param[in] This                          A pointer points to the Binding Protocol instance
  @param[in] Controller                    The handle of controller to be tested. Parameter
                                           passed by the caller
  @param[in] RemainingDevicePath           A pointer to the device path. Should be ignored by
                                           device driver

  @retval    EFI_SUCCESS                   This driver is added to ControllerHandle.
  @retval    EFI_ALREADY_STARTED           This driver is already running on ControllerHandle.
  @retval    Others                        This driver does not support this device.

**/
EFI_STATUS
EFIAPI
SataControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPrivateData;
  PCI_TYPE00                            PciData;
  UINTN                                 SegNum;
  UINTN                                 BusNum;
  UINTN                                 DevNum;
  UINTN                                 FuncNum;
  UINT64                                CommandVal;

  DEBUG ((EFI_D_INFO, "SataControllerStart() Start\n"));

  SataPrivateData = NULL;
  //
  // Now test and open the EfiPciIoProtocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  //
  // Status == 0 - A normal execution flow, SUCCESS and the program proceeds.
  // Status == ALREADY_STARTED - A non-zero Status code returned. It indicates
  //           that the protocol has been opened and should be treated as a
  //           normal condition and the program proceeds. The Protocol will not
  //           opened 'again' by this call.
  // Status != ALREADY_STARTED - Error status, terminate program execution
  //
  if (EFI_ERROR (Status)) {
    //
    // EFI_ALREADY_STARTED is also an error
    //
    return Status;
  }
  //
  // Allocate SATA private data structure
  //
  SataPrivateData = AllocatePool (sizeof (EFI_SATA_CONTROLLER_PRIVATE_DATA));
  if (SataPrivateData == NULL) {
    DEBUG ((EFI_D_ERROR, "SATA Controller START ERROR: Allocating pool for IdePrivateData failed!\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  //
  // Initialize SATA private data
  //
  ZeroMem (SataPrivateData, sizeof (EFI_SATA_CONTROLLER_PRIVATE_DATA));
  SataPrivateData->Signature              = SATA_CONTROLLER_SIGNATURE;
  SataPrivateData->PciIo                  = PciIo;
  SataPrivateData->IdeInit.GetChannelInfo = IdeInitGetChannelInfo;
  SataPrivateData->IdeInit.NotifyPhase    = IdeInitNotifyPhase;
  SataPrivateData->IdeInit.SubmitData     = IdeInitSubmitData;
  SataPrivateData->IdeInit.DisqualifyMode = IdeInitDisqualifyMode;
  SataPrivateData->IdeInit.CalculateMode  = IdeInitCalculateMode;
  SataPrivateData->IdeInit.SetTiming      = IdeInitSetTiming;
  SataPrivateData->IdeInit.EnumAll        = SATA_ENUMER_ALL;

  Status = PciIo->GetLocation (
                    PciIo,
                    &SegNum,
                    &BusNum,
                    &DevNum,
                    &FuncNum
                    );
  ASSERT_EFI_ERROR (Status);

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData),
                        &PciData
                        );
  ASSERT_EFI_ERROR (Status);

  //
  // Get device capabilities
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationSupported,
                    0,
                    &CommandVal
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Enable Command Register
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationEnable,
                    CommandVal & EFI_PCI_DEVICE_ENABLE,
                    NULL
                    );
  ASSERT_EFI_ERROR (Status);
  if (PciData.Hdr.ClassCode[1] == V_SATA_CC_SCC_IDE) {
    SataPrivateData->IdeInit.ChannelCount = IDE_MAX_CHANNELS;
  } else if (PciData.Hdr.ClassCode[1] == V_SATA_CC_SCC_AHCI ||
             PciData.Hdr.ClassCode[1] == V_SATA_CC_SCC_RAID) {
    //
    // Default MAX port number
    //
    SataPrivateData->IdeInit.ChannelCount = AHCI_MAX_PORTS;
  }
  //
  // Install IDE_CONTROLLER_INIT protocol & private data to this instance
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gSataControllerDriverGuid,
                  SataPrivateData,
                  &gEfiIdeControllerInitProtocolGuid,
                  &(SataPrivateData->IdeInit),
                  NULL
                  );

Done:
  if (EFI_ERROR (Status)) {

    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    if (SataPrivateData != NULL) {
      FreePool (SataPrivateData);
    }
  }

  DEBUG ((EFI_D_INFO, "SataControllerStart() End\n"));

  return Status;
}


/**
  Stop this driver on ControllerHandle. Stop any child handles created by this driver.

  @param[in] This                    A pointer pointing to the Binding Protocol instance
  @param[in] Controller              The handle of controller to be stopped
  @param[in] NumberOfChildren        Number of child devices
  @param[in] ChildHandleBuffer       Buffer holding child device handles

  @retval    EFI_SUCCESS             This driver is removed from ControllerHandle.
  @retval    Others                  This driver is not removed from ControllerHandle.

**/
EFI_STATUS
EFIAPI
SataControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
  EFI_STATUS                          Status;
  EFI_SATA_CONTROLLER_PRIVATE_DATA    *SataPrivateData;

  DEBUG ((EFI_D_INFO, "SataControllerStop() Start\n"));

  //
  // Get private data
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gSataControllerDriverGuid,
                  (VOID **) &SataPrivateData,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (!EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
           Controller,
           &gSataControllerDriverGuid,
           SataPrivateData,
           &gEfiIdeControllerInitProtocolGuid,
           &(SataPrivateData->IdeInit),
           NULL
           );
  }
  //
  // Close protocols opened by SATA controller driver
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (SataPrivateData != NULL) {
    FreePool (SataPrivateData);
  }

  DEBUG ((EFI_D_INFO, "SataControllerStop() End\n"));

  return EFI_SUCCESS;
}


//
// Interface functions of IDE_CONTROLLER_INIT protocol
//
/**
  This function can be used to obtain information about a specified channel.
  It's usually used by IDE Bus driver during enumeration process.

  @param[in]  This                    the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in]  Channel                 Channel number (0 based, either 0 or 1)
  @param[out] Enabled                 TRUE if the channel is enabled. If the channel is disabled,
                                      then it will no be enumerated.
  @param[out] MaxDevices              The Max number of IDE devices that the bus driver can expect
                                      on this channel. For ATA/ATAPI, this number is either 1 or 2.

  @retval     EFI_SUCCESS             Function completes successfully
  @retval     Others                  Something error happened
  @retval     EFI_INVALID_PARAMETER   The Channel parameter is invalid

**/
EFI_STATUS
EFIAPI
IdeInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN   UINT8                               Channel,
  OUT  BOOLEAN                             *Enabled,
  OUT  UINT8                               *MaxDevices
  )
{
  //
  // Channel number (0 based, either 0 or 1)
  //
  EFI_SATA_CONTROLLER_PRIVATE_DATA  *SataPrivateData;

  SataPrivateData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPrivateData);

  if (Channel < This->ChannelCount) {
    *Enabled    = TRUE;
    *MaxDevices = IDE_MAX_DEVICES;
    return EFI_SUCCESS;
  } else {
    *Enabled = FALSE;
    return EFI_INVALID_PARAMETER;
  }
}


/**
  This function is called by IdeBus driver before executing certain actions.
  This allows IDE Controller Init to prepare for each action.

  @param[in] This                    the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Phase                   phase indicator defined by IDE_CONTROLLER_INIT protocol
  @param[in] Channel                 Channel number (0 based, either 0 or 1)

  @retval    EFI_SUCCESS             Function completes successfully
  @retval    EFI_INVALID_PARAMETER   Channel parameter is out of range
  @retval    EFI_UNSUPPORTED         Phase is not supported

**/
EFI_STATUS
EFIAPI
IdeInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  IN  UINT8                             Channel
  )
{
  if (Channel >= This->ChannelCount) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Phase) {
    case EfiIdeBeforeChannelEnumeration:
    case EfiIdeAfterChannelEnumeration:
    case EfiIdeBeforeChannelReset:
    case EfiIdeAfterChannelReset:
    case EfiIdeBusBeforeDevicePresenceDetection:
    case EfiIdeBusAfterDevicePresenceDetection:
    case EfiIdeResetMode:
      //
      // Do nothing at present
      //
      break;

    default:
      return EFI_UNSUPPORTED;
      break;
  }

  return EFI_SUCCESS;
}


/**
  This function is called by IdeBus driver to submit EFI_IDENTIFY_DATA data structure
  obtained from IDE deivce. This structure is used to set IDE timing

  @param[in] This                    the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel                 IDE channel number (0 based, either 0 or 1)
  @param[in] Device                  IDE device number
  @param[in] IdentifyData            A pointer to EFI_IDENTIFY_DATA data structure

  @retval    EFI_SUCCESS             Function completes successfully
  @retval    EFI_INVALID_PARAMETER   Channel or Device parameter is out of range

**/
EFI_STATUS
EFIAPI
IdeInitSubmitData (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_IDENTIFY_DATA                   *IdentifyData
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA  *SataPrivateData;

  SataPrivateData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPrivateData);

  if ((Channel >= This->ChannelCount) || (Device >= IDE_MAX_DEVICES)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Make a local copy of device's IdentifyData and mark the valid flag
  //
  if (IdentifyData != NULL) {
    CopyMem (
      &(SataPrivateData->IdentifyData[Channel][Device]),
      IdentifyData,
      sizeof (EFI_IDENTIFY_DATA)
      );
    SataPrivateData->IdentifyValid[Channel][Device] = TRUE;
  } else {
    SataPrivateData->IdentifyValid[Channel][Device] = FALSE;
  }

  return EFI_SUCCESS;
}


/**
  This function is called by IdeBus driver to disqualify unsupported operation
  mode on specfic IDE device

  @param[in] This                    The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel                 IDE channel number (0 based, either 0 or 1)
  @param[in] Device                  IDE device number
  @param[in] BadModes                Operation mode indicator

  @retval    EFI_SUCCESS             Function completes successfully
  @retval    EFI_INVALID_PARAMETER   Channel parameter or Devicde parameter is out of range,
                                     or BadModes is NULL

**/
EFI_STATUS
EFIAPI
IdeInitDisqualifyMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_ATA_COLLECTIVE_MODE             *BadModes
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA  *SataPrivateData;

  SataPrivateData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPrivateData);

  if ((Channel >= This->ChannelCount) || (BadModes == NULL) || (Device >= IDE_MAX_DEVICES)) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (
    &(SataPrivateData->DisqulifiedModes[Channel][Device]),
    BadModes,
    sizeof (EFI_ATA_COLLECTIVE_MODE)
    );

  return EFI_SUCCESS;
}


/**
  This function is called by IdeBus driver to calculate the best operation mode
  supported by specific IDE device

  @param[in]      This                    The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in]      Channel                 IDE channel number (0 based, either 0 or 1)
  @param[in]      Device                  IDE device number
  @param[in, out] SupportedModes          Modes collection supported by IDE device

  @retval         EFI_SUCCESS             Function completes successfully
  @retval         EFI_INVALID_PARAMETER   Channel parameter or Device parameter is out of range;
                                          Or SupportedModes is NULL
  @retval         EFI_NOT_READY           Identify data is not valid
  @retval         EFI_OUT_OF_RESOURCES    SupportedModes is out of range

**/
EFI_STATUS
EFIAPI
IdeInitCalculateMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL       *This,
  IN  UINT8                                  Channel,
  IN  UINT8                                  Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE             **SupportedModes
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA           *SataPrivateData;
  EFI_IDENTIFY_DATA                          *IdentifyData;
  BOOLEAN                                    IdentifyValid;
  EFI_ATA_COLLECTIVE_MODE                    *DisqulifiedModes;
  UINT16                                     SelectedMode;
  EFI_STATUS                                 Status;

  SataPrivateData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPrivateData);

  if ((Channel >= This->ChannelCount) || (SupportedModes == NULL) || (Device >= IDE_MAX_DEVICES)) {
    return EFI_INVALID_PARAMETER;
  }

  IdentifyData      = &(SataPrivateData->IdentifyData[Channel][Device]);
  DisqulifiedModes  = &(SataPrivateData->DisqulifiedModes[Channel][Device]);
  IdentifyValid     = SataPrivateData->IdentifyValid[Channel][Device];

  //
  // Make sure we've got the valid identify data of the device from SubmitData()
  //
  if (!IdentifyValid) {
    return EFI_NOT_READY;
  }

  *SupportedModes = AllocateZeroPool (sizeof (EFI_ATA_COLLECTIVE_MODE));
  if (*SupportedModes == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CalculateBestPioMode (
             IdentifyData,
             (DisqulifiedModes->PioMode.Valid ? ((UINT16 *) &(DisqulifiedModes->PioMode.Mode)) : NULL),
             &SelectedMode
             );
  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->PioMode.Valid = TRUE;
    (*SupportedModes)->PioMode.Mode  = SelectedMode;

  } else {
    (*SupportedModes)->PioMode.Valid = FALSE;
  }

  Status = CalculateBestUdmaMode (
             IdentifyData,
             (DisqulifiedModes->UdmaMode.Valid ? ((UINT16 *) &(DisqulifiedModes->UdmaMode.Mode)) : NULL),
             &SelectedMode
             );

  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->UdmaMode.Valid = TRUE;
    (*SupportedModes)->UdmaMode.Mode  = SelectedMode;

  } else {
    (*SupportedModes)->UdmaMode.Valid = FALSE;
  }
  //
  // The modes other than PIO and UDMA are not supported by SATA controller
  //
  return EFI_SUCCESS;
}


/**
  This function is called by IdeBus driver to set appropriate timing on IDE
  controller according supported operation mode

  @param[in] This                    The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel                 IDE channel number (0 based, either 0 or 1)
  @param[in] Device                  IDE device number
  @param[in] Modes                   Operation modes

  @retval    EFI_SUCCESS             This function always returns EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IdeInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_ATA_COLLECTIVE_MODE             *Modes
  )
{
  return EFI_SUCCESS;
}


/**
  This function is used to calculate the best PIO mode supported by
  specific IDE device

  @param[in]  IdentifyData            The identify data of specific IDE device
  @param[in]  DisPioMode              Disqualified PIO modes collection
  @param[out] SelectedMode            Available PIO modes collection

  @retval     EFI_SUCCESS             Function completes successfully
  @retval     EFI_UNSUPPORTED         Some invalid condition

**/
EFI_STATUS
CalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA    *IdentifyData,
  IN  UINT16               *DisPioMode OPTIONAL,
  OUT UINT16               *SelectedMode
  )
{
  UINT16  PioMode;
  UINT16  AdvancedPioMode;
  UINT16  Temp;
  UINT16  Index;
  UINT16  MinimumPioCycleTime;

  Temp = 0xff;

  PioMode = (UINT8) (IdentifyData->AtaData.obsolete_51_52[0] >> 8);

  //
  // see whether Identify Data word 64 - 70 are valid
  //
  if ((IdentifyData->AtaData.field_validity & 0x02) == 0x02) {
    AdvancedPioMode = IdentifyData->AtaData.advanced_pio_modes;
    for (Index = 0; Index < 8; Index++) {
      if ((AdvancedPioMode & 0x01) != 0) {
        Temp = Index;
      }

      AdvancedPioMode >>= 1;
    }

    if (Temp != 0xff) {
      AdvancedPioMode = (UINT16) (Temp + 3);
    } else {
      AdvancedPioMode = PioMode;
    }
    //
    // Limit the PIO mode to at most PIO4.
    //
    PioMode = (UINT16) (AdvancedPioMode < 4 ? AdvancedPioMode : 4);
    MinimumPioCycleTime = IdentifyData->AtaData.min_pio_cycle_time_with_flow_control;

    if (MinimumPioCycleTime <= 120) {
      PioMode = (UINT16) (4 < PioMode ? 4 : PioMode);
    } else if (MinimumPioCycleTime <= 180) {
      PioMode = (UINT16) (3 < PioMode ? 3 : PioMode);
    } else if (MinimumPioCycleTime <= 240) {
      PioMode = (UINT16) (2 < PioMode ? 2 : PioMode);
    } else {
      PioMode = 0;
    }
    //
    // Degrade the PIO mode if the mode has been disqualified
    //
    if (DisPioMode != NULL) {

      if (*DisPioMode < 2) {
        return EFI_UNSUPPORTED;
        //
        // no mode below ATA_PIO_MODE_BELOW_2
        //
      }

      if (PioMode >= *DisPioMode) {
        PioMode = (UINT16) (*DisPioMode - 1);
      }
    }

    if (PioMode < 2) {
      *SelectedMode = 1;
      //
      // ATA_PIO_MODE_BELOW_2;
      //
    } else {
      *SelectedMode = PioMode;
      //
      // ATA_PIO_MODE_2 to ATA_PIO_MODE_4;
      //
    }

  } else {
    //
    // Identify Data word 64 - 70 are not valid
    // Degrade the PIO mode if the mode has been disqualified
    //
    if (DisPioMode != NULL) {

      if (*DisPioMode < 2) {
        return EFI_UNSUPPORTED;
        //
        // no mode below ATA_PIO_MODE_BELOW_2
        //
      }

      if (PioMode == *DisPioMode) {
        PioMode--;
      }
    }

    if (PioMode < 2) {
      *SelectedMode = 1;
      //
      // ATA_PIO_MODE_BELOW_2;
      //
    } else {
      *SelectedMode = 2;
      //
      // ATA_PIO_MODE_2;
      //
    }

  }

  DEBUG ((EFI_D_ERROR, "CalculateBestPioMode() End\n"));

  return EFI_SUCCESS;
}


/**
  This function is used to calculate the best UDMA mode supported by
  specific IDE device

  @param[in]  IdentifyData            The identify data of specific IDE device
  @param[in]  DisUDmaMode             Disqualified UDMA modes collection
  @param[out] SelectedMode            Available UMDA modes collection

  @retval     EFI_SUCCESS             Function completes successfully
  @retval     EFI_UNSUPPORTED         Some invalid condition

**/
EFI_STATUS
CalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA    *IdentifyData,
  IN  UINT16               *DisUDmaMode OPTIONAL,
  OUT UINT16               *SelectedMode
  )
{
  UINT16  TempMode;
  UINT16  DeviceUDmaMode;

  DeviceUDmaMode = 0;
  //
  // flag for 'Udma mode is not supported'
  //
  // Check whether the WORD 88 (supported UltraDMA by drive) is valid
  //
  if ((IdentifyData->AtaData.field_validity & 0x04) == 0x00) {
    return EFI_UNSUPPORTED;
  }

  DeviceUDmaMode = IdentifyData->AtaData.ultra_dma_mode;
  DeviceUDmaMode &= 0x3f;
  TempMode = 0;
  //
  // initialize it to UDMA-0
  //
  while ((DeviceUDmaMode >>= 1) != 0) {
    TempMode++;
  }
  //
  // Degrade the UDMA mode if the mode has been disqualified
  //
  if (DisUDmaMode != NULL) {
    if (*DisUDmaMode == 0) {
      *SelectedMode = 0;
      return EFI_UNSUPPORTED;
      //
      // no mode below ATA_UDMA_MODE_0
      //
    }

    if (TempMode >= *DisUDmaMode) {
      TempMode = (UINT16) (*DisUDmaMode - 1);
    }
  }
  //
  // Possible returned mode is between ATA_UDMA_MODE_0 and ATA_UDMA_MODE_5
  //
  *SelectedMode = TempMode;

  return EFI_SUCCESS;
}

