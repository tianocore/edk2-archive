/** @file
  This file include all platform action at BDS stage which can be customized by IBV/OEM.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformBootManager.h"
#include <Guid/EventGroup.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/AcpiS3Save.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/UsbIo.h>
#include <Protocol/VariableLock.h>
#include <Protocol/GenericMemoryTest.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/TcgPhysicalPresenceLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Library/HobLib.h>
#include <Guid/EventGroup.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/Tcg2PhysicalPresenceData.h>
#include <Guid/PhysicalPresenceData.h>
#include <Guid/TpmInstance.h>
#include <Guid/PttPTPInstanceGuid.h>


#define TIMEOUT_COMMAND 100000
#define BIOS_COLOR_CODING_BAR_HEIGHT  40

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HII_HANDLE                gPlatformBdsLibStringPackHandle;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_BOOT_MODE                 mBootMode;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GUID  *mLibTerminalType[] = {
  &gEfiPcAnsiGuid,
  &gEfiVT100Guid,
  &gEfiVT100PlusGuid,
  &gEfiVTUTF8Guid
};

//
// Internal shell mode
//
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mShellModeColumn;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mShellModeRow;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mShellHorizontalResolution;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mShellVerticalResolution;

CHAR16  *mConsoleVar[] = {L"ConIn", L"ConOut"};

extern USB_CLASS_FORMAT_DEVICE_PATH mUsbClassKeyboardDevicePath;
extern BOOLEAN                      mAnyKeypressed;

/**
  The handle on the path we get might be not the display device.
  We must check it.

  @todo fix the parameters

  @retval  TRUE         PCI class type is VGA.
  @retval  FALSE        PCI class type isn't VGA.
**/
BOOLEAN
IsVgaHandle (
  IN EFI_HANDLE Handle
)
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE00 Pci;
  EFI_STATUS Status;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo);

  if (!EFI_ERROR (Status)) {
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof (Pci) / sizeof (UINT32),
                          &Pci);

    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "  PCI CLASS CODE    = 0x%x\n", Pci.Hdr.ClassCode [2]));
      DEBUG ((DEBUG_ERROR, "  PCI SUBCLASS CODE = 0x%x\n", Pci.Hdr.ClassCode [1]));

      if (IS_PCI_VGA (&Pci) || IS_PCI_OLD_VGA (&Pci)) {
        DEBUG ((DEBUG_ERROR, "  \nPCI VGA Device Found\n"));
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.
**/
CHAR16 *
DevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}

/**
  An empty function to pass error checking of CreateEventEx ().

  This empty function ensures that EVT_NOTIFY_SIGNAL_ALL is error
  checked correctly since it is now mapped into CreateEventEx() in UEFI 2.0.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.
**/
VOID
EFIAPI
InternalBdsEmptyCallbackFuntion (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  return;
}

VOID
ExitPmAuth (
  VOID
  )
{
  EFI_HANDLE                 Handle;
  EFI_STATUS                 Status;
  EFI_ACPI_S3_SAVE_PROTOCOL  *AcpiS3Save;
  EFI_EVENT                  EndOfDxeEvent;

  DEBUG((DEBUG_INFO,"ExitPmAuth ()- Start\n"));

  //
  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter
  // We can NOT put it to PlatformBdsInit, because many boot script touch PCI BAR.
  // We have to connect PCI root bridge, allocate resource, then ExitPmAuth().
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gExitPmAuthProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Since PI1.2.1, we need signal EndOfDxe as ExitPmAuth.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  InternalBdsEmptyCallbackFuntion,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->SignalEvent (EndOfDxeEvent);
  gBS->CloseEvent (EndOfDxeEvent);
  DEBUG((DEBUG_INFO,"All EndOfDxe callbacks have returned successfully\n"));

  //
  // Prepare S3 information, this MUST be done before ExitPmAuth/EndOfDxe.
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **)&AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }

  //
  // NOTE: We need install DxeSmmReadyToLock directly here because many boot script is added via ExitPmAuth/EndOfDxe callback.
  // If we install them at same callback, these boot script will be rejected because BootScript Driver runs first to lock them done.
  // So we seperate them to be 2 different events, ExitPmAuth is last chance to let platform add boot script. DxeSmmReadyToLock will
  // make boot script save driver lock down the interface.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG((DEBUG_INFO,"ExitPmAuth ()- End\n"));
}

VOID
ConnectRootBridge (
  BOOLEAN Recursive
  )
{
  UINTN        RootBridgeHandleCount;
  EFI_HANDLE   *RootBridgeHandleBuffer;
  UINTN        RootBridgeIndex;

  RootBridgeHandleCount = 0;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciRootBridgeIoProtocolGuid,
         NULL,
         &RootBridgeHandleCount,
         &RootBridgeHandleBuffer
         );
  for (RootBridgeIndex = 0; RootBridgeIndex < RootBridgeHandleCount; RootBridgeIndex++) {
    gBS->ConnectController (RootBridgeHandleBuffer[RootBridgeIndex], NULL, NULL, Recursive);
  }
}


BOOLEAN
IsGopDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  while (!IsDevicePathEndType (DevicePath)) {
    if (DevicePathType (DevicePath) == ACPI_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == ACPI_ADR_DP) {
      return TRUE;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }
  return FALSE;
}

/**
  Connect the USB short form device path.

  @param DevicePath   USB short form device path.

  @retval EFI_SUCCESS           Successfully connected the USB device.
  @retval EFI_NOT_FOUND         Cannot connect the USB device.
  @retval EFI_INVALID_PARAMETER The device path is invalid.
**/
EFI_STATUS
ConnectUsbShortFormDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *Handles;
  UINTN                   HandleCount;
  UINTN                   Index;
  EFI_PCI_IO_PROTOCOL     *PciIo;
  UINT8                   Class[3];
  BOOLEAN                 AtLeastOneConnected;

  //
  // Check the passed in parameters.
  //
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
      ((DevicePathSubType (DevicePath) != MSG_USB_CLASS_DP) && (DevicePathSubType (DevicePath) != MSG_USB_WWID_DP))
     ) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the usb host controller firstly, then connect with the remaining device path.
  //
  
  AtLeastOneConnected = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
                  
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Check whether the Pci device is the wanted usb host controller.
      //
      Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 0x09, 3, &Class);
      if (!EFI_ERROR (Status) &&
          ((PCI_CLASS_SERIAL == Class[2]) && (PCI_CLASS_SERIAL_USB == Class[1]))
         ) {
        Status = gBS->ConnectController (
                        Handles[Index],
                        NULL,
                        DevicePath,
                        FALSE
                        );
        if (!EFI_ERROR(Status)) {
          AtLeastOneConnected = TRUE;
        }
      }
    }
  }

  return AtLeastOneConnected ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  Update the ConIn variable if Ps2 keyboard is connected.
**/
VOID
EnumPs2Keyboard (
  VOID
  )
{
  UINTN                     DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *VarConIn;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInstance;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  BOOLEAN                   Ps2Keyboard;

  Ps2Keyboard = FALSE;
  GetEfiGlobalVariable2 (L"ConIn",    (VOID **) &VarConIn,    NULL);

  //
  // If ConIn variable is empty, need to enumerate PS/2 keyboard device path.
  //
  do {
    DevicePathInstance = GetNextDevicePathInstance (
                           &VarConIn,
                           &DevicePathSize
                           );

    if (DevicePathInstance == NULL) {
      //
      // The instance is NULL, it means the VarConIn is null, escape the DO loop,
      // and need to add PS/2 keyboard dev path.
      //
      break;
    }

    Next = DevicePathInstance;
    while (!IsDevicePathEndType(Next)) {
      //
      // Checking the device path to see the PS/2 keyboard existance.
      //
      if ((Next->Type    == ACPI_DEVICE_PATH) &&
          (Next->SubType == ACPI_DP         ) &&
          (((ACPI_HID_DEVICE_PATH *) Next)->HID == EISA_PNP_ID (0x0303))) {
        //
        // PS/2 keyboard already exists.
        //
        DEBUG ((DEBUG_INFO, "[EnumPs2Keyboard] PS2 keyboard path exists\n"));
        Ps2Keyboard = TRUE;
        break;
      }
      Next = NextDevicePathNode (Next);
    }

    if (DevicePathInstance != NULL) {
      FreePool (DevicePathInstance);
    }
  } while (VarConIn != NULL);

  //
  // PS/2 keyboard device path does not exist, so try detecting ps2 keyboard
  // and add-in its device path.
  //
  if (!Ps2Keyboard) {
    DEBUG ((DEBUG_INFO, "[EnumPs2Keyboard] Adding detected PS2 keyboard to ConIn.\n"));
  }

  if (VarConIn != NULL) {
    FreePool (VarConIn);
  }
}

/**
  Update the ConIn variable with USB Keyboard device path,if its not already exists in ConIn.
**/
VOID
EnumUsbKeyboard (
  VOID
  )
{
  UINTN                     DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *VarConIn;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInstance;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  BOOLEAN                   UsbKeyboard;

  UsbKeyboard = FALSE;
  GetEfiGlobalVariable2 (L"ConIn", (VOID **)&VarConIn, NULL);

  //
  // If ConIn variable is empty, need to enumerate USB keyboard device path.
  //
  do {
    DevicePathInstance = GetNextDevicePathInstance (
                           &VarConIn,
                           &DevicePathSize
                           );

    if (DevicePathInstance == NULL) {
      //
      // The instance is NULL, it means the VarConIn is null, escape the DO loop,
      // and need to add USB keyboard dev path.
      //
      break;
    }

    Next = DevicePathInstance;
    while (!IsDevicePathEndType(Next)) {
      //
      // Checking the device path to see the USB keyboard existance.
      //
      if ((Next->Type    == MESSAGING_DEVICE_PATH) &&
          (Next->SubType == MSG_USB_CLASS_DP) &&
          (((USB_CLASS_DEVICE_PATH *) Next)->DeviceClass == CLASS_HID) &&
          (((USB_CLASS_DEVICE_PATH *) Next)->DeviceSubClass == SUBCLASS_BOOT) &&
          (((USB_CLASS_DEVICE_PATH *) Next)->DeviceProtocol == PROTOCOL_KEYBOARD)) {
          	
        DEBUG ((DEBUG_INFO, "[EnumUsbKeyboard] USB keyboard path exists\n"));
        UsbKeyboard = TRUE;

        break;
      }
      Next = NextDevicePathNode (Next);
    }

    if (DevicePathInstance != NULL) {
      FreePool (DevicePathInstance);
    }
  } while (VarConIn != NULL);

  //
  //  USB keyboard device path does not exist, So add it to the ConIn.
  //
  if (!UsbKeyboard) {
    DEBUG ((DEBUG_INFO, "[EnumUsbKeyboard] Adding USB keyboard device path to ConIn.\n"));
      EfiBootManagerUpdateConsoleVariable (ConIn, (EFI_DEVICE_PATH_PROTOCOL *) &mUsbClassKeyboardDevicePath, NULL);
   }

  if (VarConIn != NULL) {
    FreePool (VarConIn);
  }
}

/**
  Return whether the device is trusted console.

  @param Device  The device to be tested.

  @retval TRUE   The device can be trusted.
  @retval FALSE  The device cannot be trusted.
**/
BOOLEAN
IsTrustedConsole (
  EFI_DEVICE_PATH_PROTOCOL  *Device
  )
{
  if (IsGopDevicePath (Device)) {
    return TRUE;
  }

  if (CompareMem (Device, &mKeyboardDevicePath, GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) &mKeyboardDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
    return TRUE;
  }

  if (CompareMem (Device, &mUsbClassKeyboardDevicePath, GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) &mUsbClassKeyboardDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
    return TRUE;
  }

  return FALSE;
}

VOID
ProcessTcgPp (
  VOID
  )
{
  EFI_STATUS                         Status;
  UINTN                              Tcg2PpDataSize;
  EFI_TCG2_PHYSICAL_PRESENCE         Tcg2PpData;
  EFI_PHYSICAL_PRESENCE              TcgPpData;
  UINTN                              TcgPpDataSize;
    
  //
  // Initialize physical presence variable.
  //
  if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid) ||
    CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gTpmDeviceInstanceTpm20PttPtpGuid)) {
    Tcg2PpDataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE);
    Status = gRT->GetVariable (
                    TCG2_PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiTcg2PhysicalPresenceGuid,
                    NULL,
                    &Tcg2PpDataSize,
                    &Tcg2PpData
                    );

    if (EFI_ERROR (Status)) {
      ZeroMem ((VOID *) &Tcg2PpData, sizeof (Tcg2PpData));
      Tcg2PpDataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE);
      Status = gRT->SetVariable (
                      TCG2_PHYSICAL_PRESENCE_VARIABLE,
                      &gEfiTcg2PhysicalPresenceGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      Tcg2PpDataSize,
                      &Tcg2PpData
                      );
    }
  } else if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    TcgPpDataSize = sizeof (EFI_PHYSICAL_PRESENCE);
    Status = gRT->GetVariable (
                    PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiPhysicalPresenceGuid,
                    NULL,
                    &TcgPpDataSize,
                    &TcgPpData
                    );

    if (EFI_ERROR (Status)) {
      ZeroMem ((VOID *) &TcgPpData, sizeof (TcgPpData));
      TcgPpDataSize = sizeof (EFI_PHYSICAL_PRESENCE);
      Status = gRT->SetVariable (
                      PHYSICAL_PRESENCE_VARIABLE,
                      &gEfiPhysicalPresenceGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      TcgPpDataSize,
                      &TcgPpData
                      );
    }
  }

  if (!EFI_ERROR (Status) && ((Tcg2PpData.PPRequest != TCG2_PHYSICAL_PRESENCE_NO_ACTION) || (TcgPpData.PPRequest != TCG_PHYSICAL_PRESENCE_NO_ACTION))) {
  //
  // If it requests any action on TCG, need to connect console to display information.
  //
    EfiBootManagerConnectAllDefaultConsoles();
  }

  if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid) ||
    CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gTpmDeviceInstanceTpm20PttPtpGuid)) {
    Tcg2PhysicalPresenceLibProcessRequest (NULL);
  } else if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
    TcgPhysicalPresenceLibProcessRequest ();
  }
}

/**
  Remove all GOP device path instance from DevicePath and add the Gop to the DevicePath.
**/
EFI_DEVICE_PATH_PROTOCOL *
UpdateDevicePath (
  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  EFI_DEVICE_PATH_PROTOCOL *Gop
  )
{
  UINTN                    Size;
  UINTN                    GopSize;
  EFI_DEVICE_PATH_PROTOCOL *Temp;
  EFI_DEVICE_PATH_PROTOCOL *Return;
  EFI_DEVICE_PATH_PROTOCOL *Instance;
  BOOLEAN                  Exist;

  Exist = FALSE;
  Return = NULL;
  GopSize = GetDevicePathSize (Gop);
  do {
    Instance = GetNextDevicePathInstance (&DevicePath, &Size);
    if (Instance == NULL) {
      break;
    }
    if (!IsGopDevicePath (Instance) ||
        (Size == GopSize && CompareMem (Instance, Gop, GopSize) == 0)
       ) {
      if (Size == GopSize && CompareMem (Instance, Gop, GopSize) == 0) {
        Exist = TRUE;
      }
      Temp = Return;
      Return = AppendDevicePathInstance (Return, Instance);
      if (Temp != NULL) {
        FreePool (Temp);
      }
    }
    FreePool (Instance);
  } while (DevicePath != NULL);

  if (!Exist) {
    Temp = Return;
    Return = AppendDevicePathInstance (Return, Gop);
    gBS->FreePool (Temp);
  }
  return Return;
}

/**
  Check if current BootCurrent variable is internal shell boot option.

  @retval  TRUE         BootCurrent is internal shell.
  @retval  FALSE        BootCurrent is not internal shell.
**/
BOOLEAN
BootCurrentIsInternalShell (
  VOID
)
{
  UINTN                         VarSize;
  UINT16                        BootCurrent;
  CHAR16                        BootOptionName[16];
  UINT8                         *BootOption;
  UINT8                         *Ptr;
  EFI_DEVICE_PATH_PROTOCOL      *BootDevicePath;
  BOOLEAN                       Result;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_GUID                      *GuidPoint;

  BootOption     = NULL;
  BootDevicePath = NULL;
  Result         = FALSE;

  //
  // Get BootCurrent variable
  //
  VarSize = sizeof (UINT16);
  Status = gRT->GetVariable (
              L"BootCurrent",
              &gEfiGlobalVariableGuid,
              NULL,
              &VarSize,
              &BootCurrent
              );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Create boot option Bootxxxx from BootCurrent.
  //
  UnicodeSPrint (BootOptionName, sizeof(BootOptionName), L"Boot%04x", BootCurrent);

  GetEfiGlobalVariable2 (BootOptionName, (VOID **) &BootOption, &VarSize);
  if (BootOption == NULL || VarSize == 0) {
    return FALSE;
  }

  Ptr = BootOption;
  Ptr += sizeof (UINT32);
  Ptr += sizeof (UINT16);
  Ptr += StrSize ((CHAR16 *) Ptr);
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  LastDeviceNode = TempDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
     LastDeviceNode = TempDevicePath;
     TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  GuidPoint = EfiGetNameGuidFromFwVolDevicePathNode (
                (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode
                );
  if ((GuidPoint != NULL) &&
      (CompareGuid (GuidPoint, &gUefiShellFileGuid))
    ) {
    //
    // If this option is internal shell, return TRUE.
    //
    Result = TRUE;
  }

  if (BootOption != NULL) {
    FreePool (BootOption);
    BootOption = NULL;
  }

  return Result;
}

/**
  ReadyToBoot callback to set video and text mode for internal shell boot.
  That will not connect USB controller while CSM and FastBoot are disabled, we need to connect them
  before booting to Shell for showing USB devices in Shell.

  When FastBoot is enabled and Windows Console is the chosen Console behavior, input devices will not be connected
  by default. Hence, when booting to EFI shell, connecting input consoles are required.

  @param  Event   Pointer to this event
  @param  Context Event hanlder private data

  @retval None.
**/
VOID
EFIAPI
OnReadyToBootCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  DEBUG ((EFI_D_INFO, "OnReadyToBootCallBack\n"));

  if (BootCurrentIsInternalShell ()) {

  }
}

/**
  Platform Bds init. Incude the platform firmware vendor, revision
  and so crc check.
**/
VOID
EFIAPI
PlatformBootManagerBeforeConsole (
  VOID
  )
{
  EFI_STATUS                          Status;
  UINTN                               Index;
  EFI_DEVICE_PATH_PROTOCOL            *VarConOut;
  EFI_DEVICE_PATH_PROTOCOL            *VarConIn;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *ConsoleOut;
  EFI_DEVICE_PATH_PROTOCOL            *Temp;
  EFI_DEVICE_PATH_PROTOCOL            *Instance;
  EFI_DEVICE_PATH_PROTOCOL            *Next;
  EFI_HANDLE                          Handle;
  EFI_EVENT                           Event;
  EFI_GUID                            *TerminalGuid;
  UINT8                               TerminalType;
  UINTN                               InstanceSize;
  
  DEBUG ((EFI_D_INFO, "PlatformBootManagerBeforeConsole\n"));

  Status = EFI_SUCCESS;

  //
  // Append Usb Keyboard short form DevicePath into "ConInDev".
  //
  EfiBootManagerUpdateConsoleVariable (
    ConInDev,
    (EFI_DEVICE_PATH_PROTOCOL *) &mUsbClassKeyboardDevicePath,
    NULL
    );

  //
  // Get user defined text mode for internal shell only once.
  //
  mShellHorizontalResolution = PcdGet32 (PcdSetupVideoHorizontalResolution);
  mShellVerticalResolution   = PcdGet32 (PcdSetupVideoVerticalResolution);
  mShellModeColumn           = PcdGet32 (PcdSetupConOutColumn);
  mShellModeRow              = PcdGet32 (PcdSetupConOutRow);

  //
  // Create event to set proper video resolution and text mode for internal shell.
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             OnReadyToBootCallBack,
             NULL,
             &Event
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Connect Root Bridge to make PCI BAR resource allocated and all PciIo created.
  //
  ConnectRootBridge (FALSE);

  //
  // Update ConOut variable accordign to the PrimaryDisplay setting.
  //
  GetEfiGlobalVariable2 (L"ConOut", (VOID **)&ConsoleOut, NULL);

  //
  // Add IGD to ConOut.
  //
  Handle = NULL;
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) &mPlatformIGDDevice;
  Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &TempDevicePath, &Handle);
  if (!EFI_ERROR (Status) &&
    IsDevicePathEnd (TempDevicePath) &&
    IsVgaHandle (Handle)) {
  }
  DEBUG ((EFI_D_INFO, "GOP Device Handle - 0x%x\n", Handle));

  if (Handle != NULL) {
    //
    // Connect the GOP driver.
    //
    gBS->ConnectController (Handle, NULL, NULL, TRUE);

    //
    // Get the GOP device path.
    // NOTE: We may get a device path that contains Controller node in it.
    //
    TempDevicePath = EfiBootManagerGetGopDevicePath (Handle);
    DEBUG ((EFI_D_INFO, "GOP device path - 0x%x\n", TempDevicePath));
    if (TempDevicePath != NULL) {
      DEBUG ((EFI_D_INFO, "GOP device string - %S\n", DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*)TempDevicePath) ));
      Temp = ConsoleOut;
      ConsoleOut = UpdateDevicePath (ConsoleOut, TempDevicePath);
      if (Temp != NULL) {
        FreePool (Temp);
      }
      FreePool (TempDevicePath);
      Status = gRT->SetVariable (
                      L"ConOut",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      GetDevicePathSize (ConsoleOut),
                      ConsoleOut
                      );
    }
  }

  gBS->FreePool (ConsoleOut);

  //
  // Fill ConIn/ConOut in Full Configuration boot mode.
  //
  DEBUG ((DEBUG_ERROR, "PlatformBootManagerInit - %x\n", mBootMode));
  if (mBootMode == BOOT_WITH_FULL_CONFIGURATION ||
      mBootMode == BOOT_WITH_DEFAULT_SETTINGS ||
      mBootMode == BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS ||
      mBootMode == BOOT_IN_RECOVERY_MODE) {

    GetEfiGlobalVariable2 (L"ConOut", (VOID **)&VarConOut, NULL);   if (VarConOut != NULL) { FreePool (VarConOut); }
    GetEfiGlobalVariable2 (L"ConIn", (VOID **)&VarConIn, NULL);    if (VarConIn  != NULL) { FreePool (VarConIn);  }

    if (VarConOut == NULL || VarConIn == NULL) {
      //
      // Only fill ConIn/ConOut when ConIn/ConOut is empty because we may drop to Full Configuration boot mode in non-first boot.
      // Update ConOutDevicePath (just in case it is wrong at build phase).
      // To be enabled later.
      //
      for (Index = 0; mPlatformConsole[Index].DevicePath != NULL; Index++) {
        //
        // Update the console variable with the connect type.
        //
        if ((mPlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
          EfiBootManagerUpdateConsoleVariable (ConIn, mPlatformConsole[Index].DevicePath, NULL);
        }
        if ((mPlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
          EfiBootManagerUpdateConsoleVariable (ConOut, mPlatformConsole[Index].DevicePath, NULL);
        }
        if ((mPlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
          EfiBootManagerUpdateConsoleVariable (ErrOut, mPlatformConsole[Index].DevicePath, NULL);
        }
      }
    } else {
      if (mBootMode == BOOT_WITH_DEFAULT_SETTINGS) {
        //
        // Get default Terminal Type.
        //
        TerminalGuid = &gEfiPcAnsiGuid;
        TerminalType = PcdGet8 (PcdDefaultTerminalType);
        if (TerminalType < 4) {
          TerminalGuid = mLibTerminalType[TerminalType];
        }

        GetEfiGlobalVariable2 (L"ConIn", (VOID **)&VarConIn, NULL);
        Instance      = GetNextDevicePathInstance (&VarConIn, &InstanceSize);
        InstanceSize -= END_DEVICE_PATH_LENGTH;

        while (Instance != NULL) {
          Next = Instance;
          while (!IsDevicePathEndType (Next)) {
            Next = NextDevicePathNode (Next);
            if (DevicePathType (Next) == MESSAGING_DEVICE_PATH && DevicePathSubType (Next) == MSG_VENDOR_DP) {
              //
              // Restoring default serial device path.
              //
              EfiBootManagerUpdateConsoleVariable (ConIn, NULL, Instance);
              EfiBootManagerUpdateConsoleVariable (ConOut, NULL, Instance);
            }
          }
          FreePool(Instance);
          Instance      = GetNextDevicePathInstance (&VarConIn, &InstanceSize);
          InstanceSize -= END_DEVICE_PATH_LENGTH;
        }
      }
    }
  }

  EnumPs2Keyboard ();
  EnumUsbKeyboard ();

  //
  // Dynamically register hot key: F2/F7/Enter.
  //
  RegisterDefaultBootOption ();
  RegisterStaticHotkey ();

  //
  // Connect Root Bridge to make PCI BAR resource allocated.
  // Then exit PM auth before Legacy OPROM run.
  //
  ConnectRootBridge (FALSE);
  
  ProcessTcgPp ();

  ExitPmAuth ();
}

/**
  Connect with predeined platform connect sequence,
  the OEM/IBV can customize with their own connect sequence.

  @param[in] BootMode          Boot mode of this boot.
**/
VOID
ConnectSequence (
  IN EFI_BOOT_MODE         BootMode
  )
{
  EfiBootManagerConnectAll ();
}

/**
  The function is to consider the boot order which is not in our expectation.
  In the case that we need to re-sort the boot option.

  @retval  TRUE         Need to sort Boot Option.
  @retval  FALSE        Don't need to sort Boot Option.
**/
BOOLEAN
IsNeedSortBootOption (
  VOID
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         BootOptionCount;

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

  //
  // If setup is the first priority in boot option, we need to sort boot option.
  //
  if ((BootOptionCount > 1) &&
      (((StrnCmp (BootOptions->Description, L"Enter Setup", StrLen (L"Enter Setup"))) == 0) ||
       ((StrnCmp (BootOptions->Description, L"BootManagerMenuApp", StrLen (L"BootManagerMenuApp"))) == 0))) {
    return TRUE;
  }

  return FALSE;
}

/**
  The function will excute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.

  @param DriverOptionList - The header of the driver option link list
  @param BootOptionList   - The header of the boot option link list
  @param ProcessCapsules  - A pointer to ProcessCapsules()
  @param BaseMemoryTest   - A pointer to BaseMemoryTest()
**/
VOID
EFIAPI
PlatformBootManagerAfterConsole (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     LocalBootMode;
  BOOLEAN                           RequireSoftECCInit;
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenMemoryTest;
  
  DEBUG ((EFI_D_INFO, "PlatformBootManagerAfterConsole\n"));

  //
  // Run memory test code at this point.
  //
  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **) &GenMemoryTest
                  );

  if (!EFI_ERROR (Status)) {
    Status = GenMemoryTest->MemoryTestInit (
                              GenMemoryTest,
                              IGNORE,
                              &RequireSoftECCInit
                              );
  } 

  //
  // Get current Boot Mode.
  //
  LocalBootMode = mBootMode;
  DEBUG ((DEBUG_ERROR, "Current local bootmode - %x\n", LocalBootMode));

  //
  // Go the different platform policy with different boot mode.
  // Notes: this part code can be change with the table policy.
  //
  switch (LocalBootMode) {

    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
    case BOOT_WITH_MINIMAL_CONFIGURATION:
    case BOOT_ON_S4_RESUME:
      //
      // Perform some platform specific connect sequence.
      //
      ConnectSequence (LocalBootMode);
  
      break;
  
    case BOOT_WITH_FULL_CONFIGURATION:
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    case BOOT_WITH_DEFAULT_SETTINGS:
    default:
      //
      // Perform some platform specific connect sequence.
      //
      ConnectSequence (LocalBootMode);
  
      //
      // Only in Full Configuration boot mode we do the enumeration of boot device.
      //
      EfiBootManagerRefreshAllBootOption ();
  
      break;
  }
  
  if (IsNeedSortBootOption()) {
    EfiBootManagerSortLoadOptionVariable (LoadOptionTypeBoot, (SORT_COMPARE)CompareBootOption);
  }
}
