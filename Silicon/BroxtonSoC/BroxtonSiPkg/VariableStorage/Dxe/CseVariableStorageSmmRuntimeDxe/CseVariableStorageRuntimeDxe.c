/** @file
  Implements CSE Variable Storage Services and installs
  an instance of the VariableStorage Runtime DXE protocol.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CseVariableStorage.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>

//
// Module globals
//
static EFI_EVENT      mVirtualAddressChangeEvent = NULL;

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It converts pointers to a new virtual address.

  @param[in]  Event        Event whose notification function is being invoked.
  @param[in]  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
CseVariableStorageAddressChangeEvent (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  CSE_VARIABLE_FILE_TYPE  Type;

  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetAuthenticatedSupport);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetAuthenticatedVariable);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetId);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetNextVariableName);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetStorageUsage);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.GetVariable);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.RegisterWriteServiceReadyCallback);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.SetVariable);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol.WriteServiceIsReady);
  EfiConvertPointer (0x0, (VOID **) &mCseVariableStorageProtocol);

  for (Type = (CSE_VARIABLE_FILE_TYPE) 0; Type < CseVariableFileTypeMax; Type++) {
    EfiConvertPointer (0x0, (VOID **) &mCseVariableFileInfo[Type]->FileName);
    EfiConvertPointer (0x0, (VOID **) &mCseVariableFileInfo[Type]->HeaderRegionBase);
    EfiConvertPointer (0x0, (VOID **) &mCseVariableFileInfo[Type]);
  }
}


/**
  Provide DXE functionality of CSE NVM variable storage services.

  @param[in]  ImageHandle  The image handle.
  @param[in]  SystemTable  The system table.

  @retval     EFI_SUCCESS  The protocol was installed successfully.
  @retval     Others       Protocol could not be installed.

**/
EFI_STATUS
EFIAPI
CseVariableStorageDxeRuntimeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle             = NULL;
  EFI_HOB_GUID_TYPE   *FdoEnabledGuidHob = NULL;

  DEBUG ((EFI_D_INFO, "\n\n======\nCSE Variable Storage DXE Protocol Initialization\n======\n\n"));

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

  if (FdoEnabledGuidHob == NULL) {
    Status = CseVariableStorageCommonInitialize ();
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    CseVariableStorageAddressChangeEvent,
                    NULL,
                    &gEfiEventVirtualAddressChangeGuid,
                    &mVirtualAddressChangeEvent
                    );
    ASSERT_EFI_ERROR (Status);
  }
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gVariableStorageProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCseVariableStorageProtocol
                  );

  return Status;
}

