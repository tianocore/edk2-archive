/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BdsPlatform.h"
#include "SetupMode.h"
#include <Guid/SetupVariable.h>
#include <Guid/EventGroup.h>
#include <Library/TcgPhysicalPresenceLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Guid/TpmInstance.h>
#include <Guid/PttPTPInstanceGuid.h>
#include <Library/IoLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Library/CpuPlatformLib.h>
#include <Library/CustomizedDisplayLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/TimerLib.h>
#include "Guid/Tcg2PhysicalPresenceData.h"
#include "Guid/PhysicalPresenceData.h"

extern EFI_GUID gUndiDriverImageGuid;

EFI_GUID gUefiShellFileGuid = { 0x7C04A583, 0x9E3E, 0x4f1c, {0xAD, 0x65, 0xE0, 0x52, 0x68, 0xD0, 0xB4, 0xD1 }};

EFI_USER_PROFILE_HANDLE                           mCurrentUser = NULL;
EFI_EVENT                                         mHotKeyTimerEvent = NULL;
EFI_EVENT                                         mHitHotkeyEvent = NULL;
BOOLEAN                                           mHotKeyPressed = FALSE;
VOID                                              *mHitHotkeyRegistration;

#define KEYBOARD_TIMER_INTERVAL                   200000 // 0.02s

extern EFI_STATUS
IFWIUpdateHack (
  );

EFI_STATUS
PlatformBdsConnectSimpleConsole (
  IN BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  );

VOID
EFIAPI
PlatformBdsInitHotKeyEvent (
  VOID
  );

VOID
EFIAPI
UnloadNetworkDriver (
  VOID
  );


/**
  An empty function to pass error checking of CreateEventEx ().

  This empty function ensures that EVT_NOTIFY_SIGNAL_ALL is error
  checked correctly since it is now mapped into CreateEventEx() in UEFI 2.0.

  @param[in]  Event                 Event whose notification function is being invoked.
  @param[in]  Context               The pointer to the notification function's context,
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
InstallReadyToLock (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_SMM_ACCESS2_PROTOCOL  *SmmAccess;
  EFI_ACPI_S3_SAVE_PROTOCOL *AcpiS3Save;
  EFI_EVENT                 EndOfDxeEvent;
  UINTN                     VarSize;
  SYSTEM_CONFIGURATION      SystemConfiguration;

  //
  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter
  // NOTE: We can NOT put it to PlatformBdsInit, because many boot script touch PCI BAR. :-(
  //       We have to connect PCI root bridge, allocate resource, then ExitPmAuth().
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
  // Since PI1.2.1, we need signal EndOfDxe as ExitPmAuth
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
  DEBUG ((DEBUG_INFO,"All EndOfDxe callbacks have returned successfully\n"));

  //
  // Install DxeSmmReadyToLock protocol prior to the processing of boot options
  //
  Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **) &SmmAccess);
  if (!EFI_ERROR (Status)) {

    //
    // Prepare S3 information, this MUST be done before DxeSmmReadyToLock
    //
    Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **) &AcpiS3Save);
    if (!EFI_ERROR (Status)) {
      AcpiS3Save->S3Save (AcpiS3Save, NULL);
    }

    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiDxeSmmReadyToLockProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  return;
}


//
// BDS Platform Functions
//
/**
  Recursively connect all child devices to the handle that matches a given Device Path

  @param[in] DevicePathToConnect   The device path which will be connected

  @retval    EFI_SUCCESS           All child handles have been created
  @retval    EFI_OUT_OF_RESOURCES  There is no resource to create new handles
  @retval    EFI_NOT_FOUND         Creating the child handles failed

**/
EFI_STATUS
EFIAPI
ConnectDevicePathRecursive (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *CopyOfDevicePath;
  EFI_HANDLE                   Handle;

  if (DevicePathToConnect == NULL) {
    return EFI_SUCCESS;
  }
  DevicePath = DuplicateDevicePath (DevicePathToConnect);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyOfDevicePath  = DevicePath;
  Status            = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &DevicePath, &Handle);
  if (!EFI_ERROR (Status)) {
    gBS->ConnectController (Handle, NULL, NULL, TRUE);
  }

  if (CopyOfDevicePath != NULL) {
    FreePool (CopyOfDevicePath);
  }
  return Status;
}


/**
  When FastBoot is enabled, all input devices may not be connected by default.
  Hence, when entering into Setup menu, connecting all input consoles are required.

  @param[in]  Event                  The Event this notify function registered to.
  @param[in]  Context                Pointer to the context data registerd to the
                                     Event.

  @return     None.

**/
VOID
ConnectAllConsolesForSetupMenu (
  EFI_EVENT Event,
  VOID      *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Protocol;

  Status = gBS->LocateProtocol(
                  &gSetupEnterGuid,
                  NULL,
                  &Protocol
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent (Event);

  //
  // Connect PS2 and USB KB if boot to SETUP menu
  //
  BdsLibUpdateConsoleVariable (L"ConIn", gPlatformSimpleUsbConInConsole[0].DevicePath, NULL);
  BdsLibUpdateConsoleVariable (L"ConIn", gPlatformSimplePs2ConInConsole[0].DevicePath, NULL);
  BdsLibConnectAll ();
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
  // Create boot option Bootxxxx from BootCurrent
  //
  UnicodeSPrint (BootOptionName, sizeof (BootOptionName), L"Boot%04X", BootCurrent);

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
      ((CompareGuid (GuidPoint, PcdGetPtr(PcdShellFile))) ||
       (CompareGuid (GuidPoint, &gUefiShellFileGuid)))
    ) {
    //
    // if this option is internal shell, return TRUE
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

  When FastBoot is enabled, input devices will not be connected
  by default. Hence, when booting to EFI shell, connecting input consoles are required.

  @param[in]  Event   Pointer to this event
  @param[in]  Context Event hanlder private data

  @retval             None.

**/
VOID
EFIAPI
FastBootOnReadyToBootCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  //
  // Connect PS2 and USB KB if boot to shell
  //
  if (BootCurrentIsInternalShell ()) {
    BdsLibUpdateConsoleVariable (L"ConIn", gPlatformSimpleUsbConInConsole[0].DevicePath, NULL);
    BdsLibUpdateConsoleVariable (L"ConIn", gPlatformSimplePs2ConInConsole[0].DevicePath, NULL);
    BdsLibConnectAll ();
  }
}


/**
  Update ConIn by corresponding console input behavior.

  @param                        none

  @retval                       none

**/
VOID
FastBootUpdateConInVarByConInBehavior (
  IN SYSTEM_CONFIGURATION   SystemConfiguration
  )
{
  EFI_STATUS           Status;
  VOID                 *SetupRegistration;
  EFI_EVENT            Event;

  DEBUG ((EFI_D_INFO,"ConInBehavior is %x\n", SystemConfiguration.ConInBehavior));
  switch (SystemConfiguration.ConInBehavior) {
    case PS2_CONSOLE:
      //
      // Remove all device path from ConIn first.
      // Then create ConIn for PS2 or USB base on PS2 KB connected or not.
      //
      Status = gRT->SetVariable (
                      L"ConIn",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );

      PlatformBdsConnectSimpleConsole (gPlatformSimpleUsbConInConsole);
      break;

    case RECONNECT_LAST_GOOD_INPUT_CONSOLE:
      PlatformBdsConnectConsole (gPlatformConsole);
      break;

    case WINDOWS_CONSOLE:
      //
      // Remove all device path from ConIn.
      // BIOS should not enumerate any input devices when Windows Console behavior is used
      //
      Status = gRT->SetVariable (
                      L"ConIn",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      break;

    default:
      break;
  }

  //
  // Create event to connect ConIn device for internal shell.
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             FastBootOnReadyToBootCallBack,
             NULL,
             &Event
             );

  //
  // Register notification event for enter setup event
  //
  EfiCreateProtocolNotifyEvent (
    &gSetupEnterGuid,
    TPL_CALLBACK,
    (EFI_EVENT_NOTIFY)ConnectAllConsolesForSetupMenu,
    NULL,
    &SetupRegistration
    );

}


/**
  Platform Bds init. Include the platform firmware vendor, revision
  and so crc check.

  @param    None

  @retval   None.

**/
VOID
EFIAPI
PlatformBdsInit (
  VOID
  )
{
  //
  // Before user authentication, the user identification devices need be connected
  // from the platform customized device paths
  //
  PlatformBdsConnectAuthDevice ();

  //
  // As console is not ready, the auto logon user will be identified.
  //
  BdsLibUserIdentify (&mCurrentUser);
}


EFI_STATUS
GetGopDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *PciDevicePath,
  OUT EFI_DEVICE_PATH_PROTOCOL **GopDevicePath
  )
{
  UINTN                           Index;
  EFI_STATUS                      Status;
  EFI_HANDLE                      PciDeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *TempPciDevicePath;
  UINTN                           GopHandleCount;
  EFI_HANDLE                      *GopHandleBuffer;
  UINTN                           VarSize;
  SYSTEM_CONFIGURATION            SystemConfiguration;

  if (PciDevicePath == NULL || GopDevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the GopDevicePath to be PciDevicePath
  //
  *GopDevicePath    = PciDevicePath;
  TempPciDevicePath = PciDevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &TempPciDevicePath,
                  &PciDeviceHandle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Try to connect this handle, so that GOP driver could start on this
  // device and create child handles with GraphicsOutput Protocol installed
  // on them, then we get device paths of these child handles and select
  // them as possible console device.
  //

  //
  // Select display devices
  //
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  if (SystemConfiguration.BootDisplayDevice != 0x0) {
    ACPI_ADR_DEVICE_PATH         AcpiAdr;
    EFI_DEVICE_PATH_PROTOCOL     *MyDevicePath = NULL;

    AcpiAdr.Header.Type     = ACPI_DEVICE_PATH;
    AcpiAdr.Header.SubType  = ACPI_ADR_DP;

    switch (SystemConfiguration.BootDisplayDevice) {
      case 1:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, PORT_CRT, 0);    //CRT Device
        break;
      case 2:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, PORT_B_HDMI, 0);  //HDMI Device Port B
        break;
      case 3:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, PORT_B_DP, 0);    //DP PortB
        break;
      case 4:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, PORT_C_DP, 0);    //DP PortC
        break;
      case 5:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_INTERNAL_DIGITAL, PORT_C_DP, 0);    //eDP Port C
        break;
      case 6:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_INTERNAL_DIGITAL, PORT_MIPI_A, 0);  //DSI Port A
        break;
      case 7:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_INTERNAL_DIGITAL, PORT_MIPI_C, 0);  //DSI Port C
        break;
      default:
        AcpiAdr.ADR= ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, PORT_CRT, 0);
        break;
    }

    SetDevicePathNodeLength (&AcpiAdr.Header, sizeof (ACPI_ADR_DEVICE_PATH));

    MyDevicePath = AppendDevicePathNode (MyDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &AcpiAdr);

    gBS->ConnectController (PciDeviceHandle, NULL, MyDevicePath, FALSE);

    FreePool(MyDevicePath);
  } else {
    gBS->ConnectController (PciDeviceHandle, NULL, NULL, FALSE);
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );

  if (!EFI_ERROR (Status)) {
    //
    // Add all the child handles as possible Console Device
    //
    for (Index = 0; Index < GopHandleCount; Index++) {
      Status = gBS->HandleProtocol (GopHandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **) &TempDevicePath);
      if (EFI_ERROR (Status)) {
        continue;
      }
      if (CompareMem (
            PciDevicePath,
            TempDevicePath,
            GetDevicePathSize (PciDevicePath) - END_DEVICE_PATH_LENGTH
            ) == 0) {
        //
        // In current implementation, we only enable one of the child handles
        // as console device, i.e. store one of the child handle's device
        // path to variable "ConOut"
        // In future, we could select all child handles to be console device
        //

        *GopDevicePath = TempDevicePath;
      }
    }
    gBS->FreePool (GopHandleBuffer);
  }

  return EFI_SUCCESS;
}


/**
  Search out all the platform pci or agp video device. The function may will
  find multiple video device, and return all enabled device path.

  @param[in, out] PlugInPciVgaDevicePath    Return the platform plug in pci video device
                                            path if the system have plug in pci video device
  @param[in, out] OnboardPciVgaDevicePath   Return the platform active agp video device path
                                            if the system have plug in agp video device or on
                                            chip agp device

  @retval         EFI_SUCCSS                Get all platform active video device path
  @retval         EFI_STATUS                Return the status of gBS->LocateDevicePath (),
                                            gBS->ConnectController ()
                                            and gBS->LocateHandleBuffer ()

**/
EFI_STATUS
GetPlugInPciVgaDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL     **PlugInPciVgaDevicePath,
  IN OUT EFI_DEVICE_PATH_PROTOCOL     **OnboardPciVgaDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                RootHandle;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     Index;
  UINTN                     Index1;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   PlugInPciVga;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  PCI_TYPE00                Pci;

  DevicePath = NULL;
  PlugInPciVga = TRUE;
  HandleCount = 0;
  HandleBuffer = NULL;

  //
  // Make all the PCI_IO protocols on PCI Seg 0 show up
  //
  BdsLibConnectDevicePath (gPlatformRootBridges[0]);

  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &gPlatformRootBridges[0],
                  &RootHandle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->ConnectController (RootHandle, NULL, NULL, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Start to check all the pci io to find all possible VGA device
  //
  HandleCount = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (!EFI_ERROR (Status)) {

      //
      // Check for all VGA device
      //
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            0,
                            sizeof (Pci) / sizeof (UINT32),
                            &Pci
                             );
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Here we decide which VGA device to enable in PCI bus
      //
      // The first plugin PCI VGA card device will be present as PCI VGA
      // The onchip AGP or AGP card will be present as AGP VGA
      //
      if (!IS_PCI_VGA (&Pci)) {
        continue;
      }

      //
      // Set the device as the possible console out device,
      //
      // Below code will make every VGA device to be one
      // of the possible console out device
      //
      PlugInPciVga = TRUE;
      gBS->HandleProtocol (
             HandleBuffer[Index],
             &gEfiDevicePathProtocolGuid,
             (VOID **) &DevicePath
             );

      Index1 = 0;

      while (gPlatformAllPossiblePciVgaConsole[Index1] != NULL) {
        if (CompareMem (
              DevicePath,
              gPlatformAllPossiblePciVgaConsole[Index1],
              GetDevicePathSize (gPlatformAllPossiblePciVgaConsole[Index1])
              ) == 0) {

          //
          // This device is an AGP device
          //
          *OnboardPciVgaDevicePath = DevicePath;
          PlugInPciVga = FALSE;
          break;
        }

        Index1 ++;
      }

      if (PlugInPciVga) {
        *PlugInPciVgaDevicePath = DevicePath;
      }
    }
  }

  FreePool (HandleBuffer);

  return EFI_SUCCESS;
}


/**
  Find the platform  active vga, and base on the policy to enable the vga as
  the console out device. The policy is driven by one setup variable "VBIOS".

  @param   None.

  @retval  EFI_UNSUPPORTED         There is no active vga device
  @retval  EFI_STATUS              Return the status of BdsLibGetVariableAndSize ()

**/
EFI_STATUS
PlatformBdsForceActiveVga (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *PlugInPciVgaDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OnboardPciVgaDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathFirst;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathSecond;
  EFI_DEVICE_PATH_PROTOCOL  *GopDevicePath;
  UINTN                     VarSize;
  SYSTEM_CONFIGURATION      SystemConfiguration;

  Status = EFI_SUCCESS;
  GopDevicePath = NULL;
  PlugInPciVgaDevicePath = NULL;
  OnboardPciVgaDevicePath = NULL;

  //
  // Check the policy which is the first enabled VGA
  //
  GetPlugInPciVgaDevicePath (&PlugInPciVgaDevicePath, &OnboardPciVgaDevicePath);

  DEBUG ((EFI_D_INFO,"PlugInPciVgaDevicePath: 0x%x  OnboardPciVgaDevicePath: 0x%x\n", PlugInPciVgaDevicePath, OnboardPciVgaDevicePath));

  if (PlugInPciVgaDevicePath == NULL && OnboardPciVgaDevicePath == NULL) {
    return EFI_UNSUPPORTED;
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  if ((PlugInPciVgaDevicePath == NULL && OnboardPciVgaDevicePath != NULL)) {
    DEBUG ((EFI_D_INFO, "Update onboard PCI VGA ...\n"));
    DevicePathFirst  = OnboardPciVgaDevicePath;
    DevicePathSecond = PlugInPciVgaDevicePath;
    goto UpdateConOut;
  }
  if (OnboardPciVgaDevicePath != NULL && SystemConfiguration.PrimaryVideoAdaptor == 0) {
    DEBUG ((EFI_D_ERROR, "Update onboard PCI VGA When set primary!!!...\n"));
    DevicePathFirst  = OnboardPciVgaDevicePath;
    DevicePathSecond = PlugInPciVgaDevicePath;
    goto UpdateConOut;
  }
  DEBUG ((EFI_D_ERROR,"Update plug in PCI VGA ...\n"));
  DevicePathFirst  = PlugInPciVgaDevicePath;
  DevicePathSecond = OnboardPciVgaDevicePath;

UpdateConOut:
  GetGopDevicePath (DevicePathFirst, &GopDevicePath);
  DevicePathFirst = GopDevicePath;

  Status = BdsLibUpdateConsoleVariable (
             L"ConOut",
             DevicePathFirst,
             DevicePathSecond
             );

  return Status;
}


/**
  Connect the predefined platform default console device. Always try to find
  and enable the vga device if have.

  @param[in] PlatformConsole           Predefined platform default console device array.

  @retval    EFI_SUCCESS               Success connect at least one ConIn and ConOut
                                       device, there must have one ConOut device is
                                       active vga device.

  @retval    EFI_STATUS                Return the status of
                                       BdsLibConnectAllDefaultConsoles ()

**/
EFI_STATUS
PlatformBdsConnectConsole (
  IN BDS_CONSOLE_CONNECT_ENTRY       *PlatformConsole
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *VarConout;
  EFI_DEVICE_PATH_PROTOCOL           *VarConin;
  UINTN                              DevicePathSize;

  Index = 0;
  Status = EFI_SUCCESS;
  DevicePathSize = 0;
  VarConout = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );

  VarConin = BdsLibGetVariableAndSize (
               L"ConIn",
               &gEfiGlobalVariableGuid,
               &DevicePathSize
               );

  DEBUG ((EFI_D_INFO, "Enter PlatformBdsConnectConsole()\n"));
  if (VarConout == NULL || VarConin == NULL) {
    //
    // Have chance to connect the platform default console,
    // the platform default console is the minimue device group
    // the platform should support
    //
    while (PlatformConsole[Index].DevicePath != NULL) {

      //
      // Update the console variable with the connect type
      //
      if ((PlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
        BdsLibUpdateConsoleVariable (L"ConIn", PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
        BdsLibUpdateConsoleVariable (L"ConOut", PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
        BdsLibUpdateConsoleVariable (L"ErrOut", PlatformConsole[Index].DevicePath, NULL);
      }

      Index ++;
    }
  }

  //
  // Connect to serial device. Needed by GOP driver, hence before BdsLibConnectAllDefaultConsoles().
  //
  BdsLibConnectDevicePath (gSerialIoConnect[0]);

  //
  // Make sure we have at least one active VGA, and have the right
  // active VGA in console variable
  //
  Status = PlatformBdsForceActiveVga ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Connect the all the default console with current console variable
  //
  Status = BdsLibConnectAllDefaultConsoles ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "DISPLAY INIT DONE\n"));

  return EFI_SUCCESS;
}


VOID
PlatformBdsConnectSequence (
  VOID
  )
{
  UINTN                     Index;

  Index = 0;

  //
  // Here we can get the customized platform connect sequence
  // Notes: we can connect with new variable which record the
  // last time boots connect device path sequence
  //
  while (gPlatformConnectSequence[Index] != NULL) {

    //
    // Build the platform boot option
    //
    BdsLibConnectDevicePath (gPlatformConnectSequence[Index]);
    Index ++;
  }

  //
  // Just use the simple policy to connect all devices
  // There should be no difference between debug tip and release tip, or it will be extremely hard to debug.
  //
  // There is case that IdeController driver will write boot script in driver model Start() function. It will be rejected by boot script save.
  // It is only found when DEBUG disabled, because we are using BdsLibConnectAll() when DEBUG enabled.
  //
  // So we use BdsLibConnectAll() here to make sure IdeController.Start() is invoked before InstallReadyToLock().
  // We may also consider to connect SataController only later if needed.
  //
  BdsLibConnectAll ();
}


/**
  Load the predefined driver option, OEM/IBV can customize this
  to load their own drivers

  @param[in, out] BdsDriverLists      The header of the driver option link list.

  @retval         None.

**/
VOID
PlatformBdsGetDriverOption (
  IN OUT LIST_ENTRY                  *BdsDriverLists
  )
{
  UINTN                              Index;

  Index = 0;

  //
  // Here we can get the customized platform driver option
  //
  while (gPlatformDriverOption[Index] != NULL) {

    //
    // Build the platform boot option
    //
    BdsLibRegisterNewOption (BdsDriverLists, gPlatformDriverOption[Index], NULL, L"DriverOrder");
    Index ++;
  }

}


/**
  This function is used for some critical time if the the system
  have no any boot option, and there is no time out for user to add
  the new boot option. This can also treat as the platform default
  boot option.

  @param[in, out] BdsBootOptionList           The header of the boot option link list.

  @retval         None.

**/
VOID
PlatformBdsPredictBootOption (
  IN OUT LIST_ENTRY                  *BdsBootOptionList
  )
{
  UINTN                              Index;

  Index = 0;

  //
  // Here give chance to get platform boot option data
  //
  while (gPlatformBootOption[Index] != NULL) {

    //
    // Build the platform boot option
    //
    BdsLibRegisterNewOption (BdsBootOptionList, gPlatformBootOption[Index], NULL, L"BootOrder");
    Index ++;
  }
}


/**
  Perform the platform diagnostic, such like test memory. OEM/IBV also
  can customize this fuction to support specific platform diagnostic.

  @param[in] MemoryTestLevel      The memory test intensive level
  @param[in] QuietBoot            Indicate if need to enable the quiet boot
  @param[in] BaseMemoryTest       A pointer to BdsMemoryTest()

  @retval    None.

**/
VOID
PlatformBdsDiagnostics (
  IN EXTENDMEM_COVERAGE_LEVEL    MemoryTestLevel,
  IN BOOLEAN                     QuietBoot,
  IN BASEM_MEMORY_TEST           BaseMemoryTest
  )
{
  EFI_STATUS                     Status;

  //
  // Here we can decide if we need to show
  // the diagnostics screen
  // Notes: this quiet boot code should be remove
  // from the graphic lib
  //
  if (QuietBoot) {
    EnableQuietBoot (PcdGetPtr (PcdLogoFile));

    //
    // Perform system diagnostic
    //
    Status = BaseMemoryTest (MemoryTestLevel);
    if (EFI_ERROR (Status)) {
      DisableQuietBoot ();
    }

    return;
  }

  //
  // Perform system diagnostic
  //
  Status = BaseMemoryTest (MemoryTestLevel);
}


/**
  The function will execute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.

  @param[in] DriverOptionList       The header of the driver option link list
  @param[in] BootOptionList         The header of the boot option link list
  @param[in] ProcessCapsules        A pointer to ProcessCapsules()
  @param[in] BaseMemoryTest         A pointer to BaseMemoryTest()

  @retval    None.

**/
VOID
EFIAPI
PlatformBdsPolicyBehavior (
  IN OUT LIST_ENTRY                  *DriverOptionList,
  IN OUT LIST_ENTRY                  *BootOptionList,
  IN PROCESS_CAPSULES                ProcessCapsules,
  IN BASEM_MEMORY_TEST               BaseMemoryTest
  )
{
  EFI_STATUS                         Status;
  UINT16                             Timeout;
  EFI_BOOT_MODE                      BootMode;
  BOOLEAN                            DeferredImageExist;
  UINTN                              Index;
  CHAR16                             CapsuleVarName[30];
  CHAR16                             *TempVarName;
  SYSTEM_CONFIGURATION               SystemConfiguration;
  UINTN                              VarSize;
  UINT16                             *BootOrder;
  UINTN                              BootOrderSize;
  UINT32                             UcodeRevision;
  EFI_INPUT_KEY                      Key;
  CHAR16                             *ErrorInfo = L"uCode/Punit patch is not being loaded.";
  CHAR16                             *PressKey  = L"Press any key to continue...";
  UINTN                              Tcg2PpDataSize;
  EFI_TCG2_PHYSICAL_PRESENCE         Tcg2PpData;
  EFI_PHYSICAL_PRESENCE              TcgPpData;
  UINTN                              TcgPpDataSize;

  Timeout = PcdGet16 (PcdPlatformBootTimeOut);

  VarSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Load the driver option as the driver option list
  //
  PlatformBdsGetDriverOption (DriverOptionList);

  //
  // Get current Boot Mode
  //
  BootMode = GetBootModeHob ();

  //
  // Clear all the capsule variables CapsuleUpdateData, CapsuleUpdateData1, CapsuleUpdateData2...
  // as early as possible which will avoid the next time boot after the capsule update
  // will still into the capsule loop
  //
  StrCpyS (CapsuleVarName, sizeof (CapsuleVarName) / sizeof (CHAR16), EFI_CAPSULE_VARIABLE_NAME);
  TempVarName = CapsuleVarName + StrLen (CapsuleVarName);
  Index = 0;
  while (TRUE) {
    if (Index > 0) {
      UnicodeValueToString (TempVarName, 0, Index, 0);
    }
    Status = gRT->SetVariable (
                    CapsuleVarName,
                    &gEfiCapsuleVendorGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS |
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    0,
                    (VOID *) NULL
                    );
    if (EFI_ERROR (Status)) {
      //
      // There is no capsule variables, quit
      //
      break;
    }
    Index ++;
  }

  //
  // No deferred images exist by default
  //
  DeferredImageExist = FALSE;

  if (SystemConfiguration.FastBoot == 1) {
    BootOrder = BdsLibGetVariableAndSize (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  &BootOrderSize
                  );

    if (BootOrder != NULL) {
      //
      // BootOrder exist, it means system has boot before. We can do fast boot.
      //
      BootMode = BOOT_WITH_MINIMAL_CONFIGURATION;
    }
    FastBootUpdateConInVarByConInBehavior (SystemConfiguration);
  }

  //
  //  Unload EFI network driver if it is disabled in setup
  //
  if ((!SystemConfiguration.EfiNetworkSupport) || (SystemConfiguration.FastBoot == 1)) {
    UnloadNetworkDriver ();
  }
  //
  // Display message to indicate Ucode patch fails.
  //
  UcodeRevision = GetCpuUcodeRevision ();
  if ((UcodeRevision == 0 )){
    PlatformBdsConnectConsole (gPlatformConsole);
    CreateDialog (&Key, ErrorInfo, PressKey, NULL);
    gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  //
  // This will lock physical presence flag, therefore, need to do this before signal EndOfDxe
  //
  if ((BootMode != BOOT_WITH_MINIMAL_CONFIGURATION) &&
      (BootMode != BOOT_ASSUMING_NO_CONFIGURATION_CHANGES) &&
      (BootMode != BOOT_ON_FLASH_UPDATE) &&
      (BootMode != BOOT_IN_RECOVERY_MODE)) {

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
      PlatformBdsConnectConsole (gPlatformConsole);
    }

    if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20DtpmGuid) ||
      CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gTpmDeviceInstanceTpm20PttPtpGuid)) {
      Tcg2PhysicalPresenceLibProcessRequest (NULL);
    } else if (CompareGuid (PcdGetPtr (PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)) {
      TcgPhysicalPresenceLibProcessRequest ();
    }
  }

  //
  // Close boot script and install ready to lock.
  // This needs to be done before option rom dispatched.
  //
  InstallReadyToLock ();

  //
  // Go the different platform policy with different boot mode
  // Notes: this part code can be change with the table policy
  //
  DEBUG ((EFI_D_INFO, "PlatformBdsPolicyBehavior()__BootMode = %d\n", BootMode));
  switch (BootMode) {
    case BOOT_WITH_MINIMAL_CONFIGURATION:
      PlatformBdsConnectSimpleConsole (gPlatformSimpleOnChipPciVgaConOutConsole);
      //
      // Connect boot device here to give time to read keyboard.
      //
      Index = 0;
      while (gPlatformSimpleBootOption[Index] != NULL) {
        BdsLibConnectDevicePath (gPlatformSimpleBootOption[Index]);
        ConnectDevicePathRecursive (gPlatformSimpleBootOption[Index]);
        Index ++;
      }
      break;

    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
      //
      // In no-configuration boot mode, we can connect the
      // console directly.
      //
      BdsLibConnectAllDefaultConsoles ();
      PlatformBdsDiagnostics (IGNORE, TRUE, BaseMemoryTest);

      //
      // Perform some platform specific connect sequence
      //
      PlatformBdsConnectSequence ();

      //
      // As console is ready, perform user identification again.
      //
      if (mCurrentUser == NULL) {
        PlatformBdsUserIdentify (&mCurrentUser, &DeferredImageExist);
        if (DeferredImageExist) {
          //
          // After user authentication, the deferred drivers was loaded again.
          // Here, need to ensure the deferred images are connected.
          //
          BdsLibConnectAllDefaultConsoles ();
          PlatformBdsConnectSequence ();
        }
      }

      //
      // Notes: current time out = 0 can not enter the
      // front page
      //
      PlatformBdsEnterFrontPageWithHotKey (Timeout, FALSE);

      //
      // Check the boot option with the boot option list
      //
      BdsLibBuildOptionFromVar (BootOptionList, L"BootOrder");
      break;

    case BOOT_ON_FLASH_UPDATE:
      //
      // Boot with the specific configuration
      //
      PlatformBdsConnectConsole (gPlatformConsole);
      PlatformBdsDiagnostics (EXTENSIVE, FALSE, BaseMemoryTest);
      BdsLibConnectAll ();

      //
      // Perform user identification
      //
      if (mCurrentUser == NULL) {
        PlatformBdsUserIdentify (&mCurrentUser, &DeferredImageExist);
        if (DeferredImageExist) {
          //
          // After user authentication, the deferred drivers was loaded again.
          // Here, need to ensure the deferred images are connected.
          //
          BdsLibConnectAll ();
        }
      }

      ProcessCapsules (BOOT_ON_FLASH_UPDATE);
      break;

    case BOOT_IN_RECOVERY_MODE:
      //
      // In recovery mode, just connect platform console
      // and show up the front page
      //
      PlatformBdsConnectConsole (gPlatformConsole);
      PlatformBdsDiagnostics (EXTENSIVE, FALSE, BaseMemoryTest);
      BdsLibConnectAll ();

      //
      // Perform user identification
      //
      if (mCurrentUser == NULL) {
        PlatformBdsUserIdentify (&mCurrentUser, &DeferredImageExist);
        if (DeferredImageExist) {
          //
          // After user authentication, the deferred drivers was loaded again.
          // Here, need to ensure the deferred drivers are connected.
          //
          BdsLibConnectAll ();
        }
      }

      //
      // In recovery boot mode, we still enter to the
      // frong page now
      //
      PlatformBdsEnterFrontPageWithHotKey (Timeout, FALSE);
      break;

    case BOOT_WITH_FULL_CONFIGURATION:
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    case BOOT_WITH_DEFAULT_SETTINGS:
    default:
      //
      // Connect platform console
      //
      Status = PlatformBdsConnectConsole (gPlatformConsole);
      if (EFI_ERROR (Status)) {
        //
        // Here OEM/IBV can customize with defined action
        //
        PlatformBdsNoConsoleAction ();
      }

      //
      // Perform some platform specific connect sequence
      //
      PlatformBdsConnectSequence ();

      //
      // Perform memory test as appropriate in order to promote any untested memory
      //
      PlatformBdsDiagnostics (IGNORE, TRUE, BaseMemoryTest);

      //
      // Do a pre-delay so Hard Disk can spin up and see more logo.
      //
      gBS->Stall (SystemConfiguration.HddPredelay * 1000000);

      //
      // Perform user identification
      //
      if (mCurrentUser == NULL) {
        PlatformBdsUserIdentify (&mCurrentUser, &DeferredImageExist);
        if (DeferredImageExist) {
          //
          // After user authentication, the deferred drivers was loaded again.
          // Here, need to ensure the deferred drivers are connected.
          //
          Status = PlatformBdsConnectConsole (gPlatformConsole);
          if (EFI_ERROR (Status)) {
            PlatformBdsNoConsoleAction ();
          }
          PlatformBdsConnectSequence ();
        }
      }

      //
      // Give one chance to enter the setup if we
      // have the time out
      //
      PlatformBdsEnterFrontPageWithHotKey (Timeout, FALSE);

      //
      // In default boot mode, always find all boot
      // option and do enumerate all the default boot option
      //
      if (Timeout == 0) {
        BdsLibBuildOptionFromVar (BootOptionList, L"BootOrder");
        if (IsListEmpty(BootOptionList)) {
          PlatformBdsPredictBootOption (BootOptionList);
        }

        return;
      }

      //
      // Here we have enough time to do the enumeration of boot device
      //
      BdsLibEnumerateAllBootOption (BootOptionList);
      break;
    }
    return;
}


/**
  Hook point after a boot attempt succeeds. We don't expect a boot option to
  return, so the UEFI 2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in this case. This
  is also a platform implementation and can be customized by IBV/OEM.

  @param[in] Option          Pointer to Boot Option that succeeded to boot.

  @retval    None.

**/
VOID
EFIAPI
PlatformBdsBootSuccess (
  IN  BDS_COMMON_OPTION             *Option
  )
{
  CHAR16  *TmpStr;

  //
  // If Boot returned with EFI_SUCCESS and there is not in the boot device
  // select loop then we need to pop up a UI and wait for user input.
  //
  TmpStr =  Option->StatusString;
  if (TmpStr != NULL) {
    BdsLibOutputStrings (gST->ConOut, TmpStr, Option->Description, L"\n\r", NULL);
    FreePool(TmpStr);
  }
}


/**
  Hook point after a boot attempt fails.

  @param[in] Option           Pointer to Boot Option that failed to boot.
  @param[in] Status           Status returned from failed boot.
  @param[in] ExitData         Exit data returned from failed boot.
  @param[in] ExitDataSize     Exit data size returned from failed boot.

  @retval    None.

**/
VOID
EFIAPI
PlatformBdsBootFail (
  IN  BDS_COMMON_OPTION  *Option,
  IN  EFI_STATUS         Status,
  IN  CHAR16             *ExitData,
  IN  UINTN              ExitDataSize
  )
{
  CHAR16  *TmpStr;

  //
  // If Boot returned with failed status then we need to pop up a UI and wait
  // for user input.
  //
  TmpStr = Option->StatusString;
  if (TmpStr != NULL) {
    BdsLibOutputStrings (gST->ConOut, TmpStr, Option->Description, L"\n\r", NULL);
    FreePool (TmpStr);
  }
}


EFI_STATUS
PlatformBdsNoConsoleAction (
  VOID
  )
{
  return EFI_SUCCESS;
}


/**
  This function locks the block.

  @param[in] Base          The base address flash region to be locked.

**/
VOID
BdsLockFv (
  IN EFI_PHYSICAL_ADDRESS  Base
  )
{
  EFI_FV_BLOCK_MAP_ENTRY      *BlockMap;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  UINT8                       Data;
  UINT32                      BlockLength;
  UINTN                       Index;

  BaseAddress = Base - 0x400000 + 2;
  FvHeader    = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) (Base));
  BlockMap    = &(FvHeader->BlockMap[0]);

  while ((BlockMap->NumBlocks != 0) && (BlockMap->Length != 0)) {
    BlockLength = BlockMap->Length;
    for (Index = 0; Index < BlockMap->NumBlocks; Index++) {
      Data = MmioOr8 ((UINTN) BaseAddress, 0x03);
      BaseAddress += BlockLength;
    }
    BlockMap++;
  }
}


VOID
EFIAPI
PlatformBdsLockNonUpdatableFlash (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS  Base;

  Base = (EFI_PHYSICAL_ADDRESS) PcdGet32 (PcdFlashFvOBBBase);
  if (Base > 0) {
    BdsLockFv (Base);
  }

  Base = (EFI_PHYSICAL_ADDRESS) PcdGet32 (PcdFlashFvIBBMBase);
  if (Base > 0) {
    BdsLockFv (Base);
  }
}


/**
  Lock the ConsoleIn device in system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

  @param[in]  Password          Password used to lock ConIn device.

  @retval     EFI_SUCCESS       Lock the Console In Splitter virtual handle successfully.
  @retval     EFI_UNSUPPORTED   Password not found.

**/
EFI_STATUS
EFIAPI
LockKeyboards (
  IN  CHAR16    *Password
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Connect the predefined platform default authentication devices.

  This function connects the predefined device path for authentication device,
  and if the predefined device path has child device path, the child handle will
  be connected too. But the child handle of the child will not be connected.

**/
VOID
EFIAPI
PlatformBdsConnectAuthDevice (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        HandleIndex;
  UINTN                        HandleCount;
  EFI_HANDLE                   *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL     *ChildDevicePath;
  EFI_USER_MANAGER_PROTOCOL    *Manager;

  Status = gBS->LocateProtocol (
                  &gEfiUserManagerProtocolGuid,
                  NULL,
                  (VOID **) &Manager
                  );
  if (EFI_ERROR (Status)) {
    //
    // As user manager protocol is not installed, the authentication devices
    // should not be connected.
    //
    return ;
  }

  Index = 0;
  while (gUserAuthenticationDevice[Index] != NULL) {
    //
    // Connect the platform customized device paths
    //
    BdsLibConnectDevicePath (gUserAuthenticationDevice[Index]);
    Index++;
  }

  //
  // Find and connect the child device paths of the platform customized device paths
  //
  HandleBuffer = NULL;
  for (Index = 0; gUserAuthenticationDevice[Index] != NULL; Index++) {
    HandleCount = 0;
    Status = gBS->LocateHandleBuffer (
                    AllHandles,
                    NULL,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    ASSERT (!EFI_ERROR (Status));

    //
    // Find and connect the child device paths of gUserIdentificationDevice[Index]
    //
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
      ChildDevicePath = NULL;
      Status = gBS->HandleProtocol (
                      HandleBuffer[HandleIndex],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **) &ChildDevicePath
                      );
      if (EFI_ERROR (Status) || ChildDevicePath == NULL) {
        continue;
      }

      if (CompareMem (
            ChildDevicePath,
            gUserAuthenticationDevice[Index],
            (GetDevicePathSize (gUserAuthenticationDevice[Index]) - sizeof (EFI_DEVICE_PATH_PROTOCOL))
            ) != 0) {
        continue;
      }
      gBS->ConnectController (HandleBuffer[HandleIndex], NULL, NULL, TRUE);
    }
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}


/**
  This function is to identify a user, and return whether deferred images exist.

  @param[out]  User               Point to user profile handle.
  @param[out]  DeferredImageExist On return, points to TRUE if the deferred image
                                  exist or FALSE if it did not exist.

**/
VOID
EFIAPI
PlatformBdsUserIdentify (
  OUT EFI_USER_PROFILE_HANDLE        *User,
  OUT BOOLEAN                        *DeferredImageExist
  )
{
  EFI_STATUS                         Status;
  EFI_DEFERRED_IMAGE_LOAD_PROTOCOL   *DeferredImage;
  UINTN                              HandleCount;
  EFI_HANDLE                         *HandleBuf;
  UINTN                              Index;
  UINTN                              DriverIndex;
  EFI_DEVICE_PATH_PROTOCOL           *ImageDevicePath;
  VOID                               *DriverImage;
  UINTN                              ImageSize;
  BOOLEAN                            BootOption;

  //
  // Perform user identification
  //
  do {
    Status = BdsLibUserIdentify (User);
  } while (EFI_ERROR (Status));

  //
  // After user authentication now, try to find whether deferred image exists
  //
  HandleCount = 0;
  HandleBuf   = NULL;
  *DeferredImageExist = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDeferredImageLoadProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuf
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuf[Index],
                    &gEfiDeferredImageLoadProtocolGuid,
                    (VOID **) &DeferredImage
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Find whether deferred image exists in this instance.
      //
      DriverIndex = 0;
      Status = DeferredImage->GetImageInfo(
                                DeferredImage,
                                DriverIndex,
                                &ImageDevicePath,
                                (VOID **) &DriverImage,
                                &ImageSize,
                                &BootOption
                                );
      if (!EFI_ERROR (Status)) {
        //
        // The deferred image is found.
        //
        FreePool (HandleBuf);
        *DeferredImageExist = TRUE;
        return ;
      }
    }
  }

  FreePool (HandleBuf);
}

UINTN gHotKey = 0;


EFI_STATUS
ShowProgressHotKey (
  IN UINT16                     TimeoutDefault
  )
{
  CHAR16                        *TmpStr;
  UINT16                        TimeoutRemain;
  EFI_STATUS                    Status;
  EFI_INPUT_KEY                 Key;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;

  if (TimeoutDefault == 0) {
    return EFI_TIMEOUT;
  }

  DEBUG ((EFI_D_INFO, "\n\nStart showing progress bar... Press any key to stop it! ...Zzz....\n"));

  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  SetMem (&Color, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);

  //
  // Clear the progress status bar first
  //
  TmpStr = L"Start boot option";
  PlatformBdsShowProgress (Foreground, Background, TmpStr, Color, 0, 0);

  //
  // TimeoutDefault is determined by PcdPlatformBootTimeOut in PcdsDynamicHii.Default.dsc
  //
  TimeoutRemain = TimeoutDefault;

  while (TimeoutRemain != 0) {
    DEBUG ((EFI_D_INFO, "Showing progress bar...Remaining %d second!\n", TimeoutRemain));

    Status = WaitForSingleEvent (gST->ConIn->WaitForKey, ONE_SECOND/10);
    if (Status != EFI_TIMEOUT) {
      break;
    }
    TimeoutRemain--;

    //
    // Show progress
    //
    if (TmpStr != NULL) {
      PlatformBdsShowProgress (
        Foreground,
        Background,
        TmpStr,
        Color,
        ((TimeoutDefault - TimeoutRemain) * 100 / TimeoutDefault),
        0
        );
    }
  }

  //
  // Timeout expired
  //
  if (TimeoutRemain == 0) {
    return EFI_TIMEOUT;
  }

  //
  // User pressed some key
  //
  Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    //
    // User pressed enter, equivalent to select "continue"
    //
    return EFI_TIMEOUT;
  }

  //
  // F2 --  Front Page
  // F5 --  Device Manager
  // F7 --  Boot Manager
  // do not use F8. generally people assume it is windows safe mode key.
  // F9 --  Boot order
  //
  DEBUG ((EFI_D_INFO, "[Key Pressed]: ScanCode 0x%x\n", Key.ScanCode));
  switch (Key.ScanCode) {
    case SCAN_F2:
      gHotKey = 0;
      break;

    case SCAN_F5:
      gHotKey = FRONT_PAGE_KEY_DEVICE_MANAGER;
      break;

    case SCAN_F7:
      gHotKey = FRONT_PAGE_KEY_BOOT_MANAGER;
      break;

    case SCAN_F9:
      gHotKey = FRONT_PAGE_KEY_BOOT_MAINTAIN;
      break;

    default:
      //
      //set gHotKey to continue so that flow will not go into CallFrontPage
      //
      gHotKey = FRONT_PAGE_KEY_CONTINUE;
      return EFI_TIMEOUT;
      break;
  }

  return EFI_SUCCESS;
}


/**
  This function is the main entry of the platform setup entry.
  The function will present the main menu of the system setup,
  this is the platform reference part and can be customize.


  @param[in] TimeoutDefault        The fault time out value before the system
                                   continue to boot.
  @param[in] ConnectAllHappened    The indicator to check if the connect all have
                                   already happened.

**/
VOID
PlatformBdsEnterFrontPageWithHotKey (
  IN UINT16                       TimeoutDefault,
  IN BOOLEAN                      ConnectAllHappened
  )
{
  EFI_STATUS                         Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *SimpleTextOut;
  UINTN                              BootTextColumn;
  UINTN                              BootTextRow;

  GraphicsOutput = NULL;
  SimpleTextOut = NULL;

  PERF_START (NULL, "BdsTimeOut", "BDS", 0);
  //
  // Indicate if we need connect all in the platform setup
  //
  if (ConnectAllHappened) {
    gConnectAllHappened = TRUE;
  }

  if (!mModeInitialized) {
    //
    // After the console is ready, get current video resolution
    // and text mode before launching setup at first time.
    //
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &GraphicsOutput
                    );
    if (EFI_ERROR (Status)) {
      GraphicsOutput = NULL;
    }

    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiSimpleTextOutProtocolGuid,
                    (VOID **) &SimpleTextOut
                    );
    if (EFI_ERROR (Status)) {
      SimpleTextOut = NULL;
    }

    if (GraphicsOutput != NULL) {
      //
      // Get current video resolution and text mode.
      //
      mBootHorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
      mBootVerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
    }

    if (SimpleTextOut != NULL) {
      Status = SimpleTextOut->QueryMode (
                                SimpleTextOut,
                                SimpleTextOut->Mode->Mode,
                                &BootTextColumn,
                                &BootTextRow
                                );
      mBootTextModeColumn = (UINT32) BootTextColumn;
      mBootTextModeRow    = (UINT32) BootTextRow;
    }

    //
    // Get user defined text mode for setup.
    //
    mSetupHorizontalResolution = PcdGet32 (PcdSetupVideoHorizontalResolution);
    mSetupVerticalResolution   = PcdGet32 (PcdSetupVideoVerticalResolution);
    mSetupTextModeColumn       = PcdGet32 (PcdSetupConOutColumn);
    mSetupTextModeRow          = PcdGet32 (PcdSetupConOutRow);

    mModeInitialized           = TRUE;
  }

  if (TimeoutDefault != 0xffff) {
    Status = ShowProgressHotKey (TimeoutDefault);

    //
    // Ensure screen is clear when switch Console from Graphics mode to Text mode
    //
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    gST->ConOut->ClearScreen (gST->ConOut);

    if (EFI_ERROR (Status)) {
      //
      // Timeout or user press enter to continue
      //
      goto Exit;
    }
  }
  InitBMPackage ();
  do {
    BdsSetConsoleMode (TRUE);
    InitializeFrontPage (FALSE);

    //
    // Update Front Page strings
    //
    UpdateFrontPageStrings ();

    Status = EFI_SUCCESS;
    gCallbackKey = 0;

    if (gHotKey == 0) {
      Status = CallFrontPage ();
    } else {
      gCallbackKey = gHotKey;
      gHotKey = 0;
    }

    //
    // If gCallbackKey is greater than 1 and less or equal to 5,
    // it will launch configuration utilities.
    // 2 = set language
    // 3 = boot manager
    // 4 = device manager
    // 5 = boot maintenance manager
    //
    if (gCallbackKey != 0) {
      REPORT_STATUS_CODE (
        EFI_PROGRESS_CODE,
        (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP)
        );
    }

    //
    // Based on the key that was set, we can determine what to do
    //
    switch (gCallbackKey) {
      //
      // The first 4 entries in the Front Page are to be GUARANTEED to remain constant so IHV's can
      // describe to their customers in documentation how to find their setup information (namely
      // under the device manager and specific buckets)
      //
      // These entries consist of the Continue, Select language, Boot Manager, and Device Manager
      //
      case FRONT_PAGE_KEY_CONTINUE:
        //
        // User hit continue
        //
        break;

      case FRONT_PAGE_KEY_LANGUAGE:
        //
        // User made a language setting change - display front page again
        //
        break;

      case FRONT_PAGE_KEY_BOOT_MANAGER:
        FreeBMPackage ();
        //
        // User chose to run the Boot Manager
        //
        CallBootManager ();
        InitBMPackage ();
        break;

      case FRONT_PAGE_KEY_DEVICE_MANAGER:
        //
        // Display the Device Manager
        //
        do {
          CallDeviceManager ();
        } while (gCallbackKey == FRONT_PAGE_KEY_DEVICE_MANAGER);
        break;

      case FRONT_PAGE_KEY_BOOT_MAINTAIN:
        //
        // Display the Boot Maintenance Manager
        //
        BdsStartBootMaint ();
        break;
      }

  } while (((UINTN) gCallbackKey) != FRONT_PAGE_KEY_CONTINUE);

  //
  // Will leave browser, check any reset required change is applied? if yes, reset system
  //
  SetupResetReminder ();
  FreeBMPackage ();
Exit:
  //
  // Automatically load current entry
  // Note: The following lines of code only execute when Auto boot
  // takes affect
  //
  PERF_END (NULL, "BdsTimeOut", "BDS", 0);
}


EFI_STATUS
PlatformBdsConnectSimpleConsole (
  IN BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_DEVICE_PATH_PROTOCOL           *VarConout;
  EFI_DEVICE_PATH_PROTOCOL           *VarConin;
  UINTN                              DevicePathSize;

  Index = 0;
  Status = EFI_SUCCESS;
  DevicePathSize = 0;
  VarConout = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );
  VarConin = BdsLibGetVariableAndSize (
               L"ConIn",
               &gEfiGlobalVariableGuid,
               &DevicePathSize
               );
  DEBUG ((EFI_D_INFO, "Enter PlatformBdsConnectSimpleConsole()\n"));
  if (VarConout == NULL || VarConin == NULL) {
    //
    // Have chance to connect the platform default console,
    // the platform default console is the minimum device group
    // the platform should support
    //
    while (PlatformConsole[Index].DevicePath != NULL) {

      //
      // Update the console variable with the connect type
      //
      if ((PlatformConsole[Index].ConnectType & CONSOLE_IN) == CONSOLE_IN) {
        BdsLibUpdateConsoleVariable (L"ConIn", PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & CONSOLE_OUT) == CONSOLE_OUT) {
        BdsLibUpdateConsoleVariable (L"ConOut", PlatformConsole[Index].DevicePath, NULL);
      }

      if ((PlatformConsole[Index].ConnectType & STD_ERROR) == STD_ERROR) {
        BdsLibUpdateConsoleVariable (L"ErrOut", PlatformConsole[Index].DevicePath, NULL);
      }

      Index ++;
    }
  }

  //
  // Make sure we have at least one active VGA, and have the right
  // active VGA in console variable
  //
  Status = PlatformBdsForceActiveVga ();

  //
  // Connect ConIn first to give keyboard time to parse hot key event.
  //
  Status = BdsLibConnectConsoleVariable (L"ConIn");
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // It seems impossible not to have any ConOut device on platform,
  // so we check the status here.
  //
  Status = BdsLibConnectConsoleVariable (L"ConOut");
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Timer handler to convert the key from USB.

  @param[in]  Event               Indicates the event that invoke this function.
  @param[in]  Context             Indicates the calling context.

**/
VOID
EFIAPI
HotKeyTimerHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  EFI_INPUT_KEY                 Key;

  Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  if (EFI_ERROR (Status)) {
    return;
  }

  switch (Key.ScanCode) {
    case SCAN_F2:
      gHotKey = 0;
      mHotKeyPressed = TRUE;
      break;

    case SCAN_F5:
      gHotKey = FRONT_PAGE_KEY_DEVICE_MANAGER;
      mHotKeyPressed = TRUE;
      break;

    case SCAN_F7:
      gHotKey = FRONT_PAGE_KEY_BOOT_MANAGER;
      mHotKeyPressed = TRUE;
      break;

    case SCAN_F9:
      gHotKey = FRONT_PAGE_KEY_BOOT_MAINTAIN;
      mHotKeyPressed = TRUE;
      break;
    }

  if (mHotKeyPressed) {
    gBS->SetTimer (mHotKeyTimerEvent, TimerCancel, 0);
    gBS->CloseEvent (mHotKeyTimerEvent);
    mHotKeyTimerEvent = NULL;
  }

  return;
}


/**
  Callback function for SimpleTextInEx protocol install events.

  @param[in] Event           The event that is signalled.
  @param[in] Context         Not used here.

**/
VOID
EFIAPI
HitHotkeyEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                         Status;

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  HotKeyTimerHandler,
                  NULL,
                  &mHotKeyTimerEvent
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = gBS->SetTimer (
                  mHotKeyTimerEvent,
                  TimerPeriodic,
                  KEYBOARD_TIMER_INTERVAL
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  return;
}


VOID
EFIAPI
PlatformBdsInitHotKeyEvent (
  VOID
  )
{
  EFI_STATUS      Status;

  //
  // Register Protocol notify for Hotkey service
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  HitHotkeyEvent,
                  NULL,
                  &mHitHotkeyEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSimpleTextInputExProtocolGuid,
                  mHitHotkeyEvent,
                  &mHitHotkeyRegistration
                  );
  ASSERT_EFI_ERROR (Status);
}


VOID
EFIAPI
UnloadNetworkDriver (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     DriverImageHandleCount;
  EFI_HANDLE                *DriverImageHandleBuffer;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_GUID                  *NameGuid;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *LastDeviceNode;

  DriverImageHandleCount  = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &DriverImageHandleCount,
                  &DriverImageHandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  for (Index = 0; Index < DriverImageHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    DriverImageHandleBuffer[Index],
                    &gEfiLoadedImageProtocolGuid,
                    (VOID **) &LoadedImage
                    );
    if (LoadedImage->FilePath == NULL) {
      continue;
    }

    TempDevicePath = LoadedImage->FilePath;
    LastDeviceNode = TempDevicePath;
    while (!IsDevicePathEnd (TempDevicePath)) {
      LastDeviceNode = TempDevicePath;
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode (
                  (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode
                  );
    if ((NameGuid != NULL) && (CompareGuid (NameGuid, &gUndiDriverImageGuid))) {
      Status = gBS->UnloadImage (DriverImageHandleBuffer[Index]);
      ASSERT_EFI_ERROR (Status);
      break;
    }
  }
}

