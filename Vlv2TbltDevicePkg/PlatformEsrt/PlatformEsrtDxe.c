/** @file
  Non-FMP ESRT Platform Driver to produce system firmware resource to ESRT

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/  


#include "PlatformEsrtDxe.h"



EFI_SYSTEM_RESOURCE_ENTRY EsrtTemplateBuf[] = {
  // System Firmware Entry
  {
    SYSTEM_FW_CLASS_GUID,
    ESRT_FW_TYPE_SYSTEMFIRMWARE,
    0x0001,
    0x0001,
    0x0000,
    0x0000,
    LAST_ATTEMPT_STATUS_SUCCESS
  }
};

UINTN EsrtCount = sizeof(EsrtTemplateBuf) / sizeof(EFI_SYSTEM_RESOURCE_ENTRY);


/**
  Register all platform updatable firmware resourece to Esrt table 

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.

**/
EFI_STATUS 
RegisterPlatformEsrtEntry()
{
  EFI_STATUS                   Status;
  ESRT_MANAGEMENT_PROTOCOL     *EsrtManagement;
  UINTN                        Index;

  Status = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtManagement);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Index = 0; Index < EsrtCount; Index++) {
    Status = EsrtManagement->GetEsrtEntry(
                               &EsrtTemplateBuf[Index].FwClass,
                               &EsrtTemplateBuf[Index]
                               );
    if (Status == EFI_NOT_FOUND) {
      //
      // Init EsrtEntry for system firmware updatable resource
      //
      Status = EsrtManagement->RegisterEsrtEntry(&EsrtTemplateBuf[Index]);
    }
  }

  return Status;

}


/**
  Notify function for protocol ESRT management protocol. This is used to
  register system firmware updatable resourece to Esrt table 

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.

**/
VOID 
EFIAPI
PlatformEsrtNotifyFunction (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS Status;

  Status = RegisterPlatformEsrtEntry();
  DEBUG ((EFI_D_INFO, "PlatformEsrtDxe Status = 0x%x\n", Status));
}


EFI_STATUS
EFIAPI
PlatformEsrtDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_EVENT                    Event;
  VOID                         *Registration;
  ESRT_MANAGEMENT_PROTOCOL     *EsrtManagement;
  UINTN                        Index;

  for (Index = 0; Index < EsrtCount; Index++) {
    EsrtTemplateBuf[Index].CapsuleFlags = PcdGet16(PcdSystemRebootAfterCapsuleProcessFlag);
  }

  Status = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtManagement);
  if (!EFI_ERROR(Status)) {
    //
    // Directly Register Platform Updatable Resource
    //
    return RegisterPlatformEsrtEntry();
  }

  //
  // Register Callback function for updating USB Rmrr address
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PlatformEsrtNotifyFunction,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify(
                  &gEsrtManagementProtocolGuid,
                  Event,
                  &Registration
                  );

  return EFI_SUCCESS;
}
