/** @file
  Source code file for the Silicon Init DXE module.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/Smbios.h>
#include <Library/SmbiosMemoryLib.h>

///
/// Module-wide global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMBIOS_PROTOCOL  *mSmbios = NULL;

VOID  *mRegistration;

EFI_EVENT                         EndOfDxeEvent;

/**
  This function gets registered as a callback to perform

  @param[in]  Event       A pointer to the Event that triggered the callback.
  @param[in]  Context     A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
SiInitDxeSmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS    Status;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &mSmbios);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error locating gEfiSmbiosProtocolGuid at SiInitDxe. Status = %r\n", Status));
    return;
  }

  Status = SmbiosMemory ();
  ASSERT_EFI_ERROR (Status);

  return;
}


/**
  This function handles PlatformInit task at the end of DXE

  @param[in]  Event       A pointer to the Event that triggered the callback.
  @param[in]  Context     A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
SiInitEndOfDxe (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  gBS->CloseEvent (Event);

  return;
}


/**
  Entry point for the driver.

  @param[in]  ImageHandle  Image Handle.
  @param[in]  SystemTable  EFI System Table.

  @retval     EFI_SUCCESS  Function has completed successfully.
  @retval     Others       All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
EFIAPI
SiInitDxe (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_EVENT     CallbackEvent;

  //
  // If EfiSmbiosProtocol already installed, invoke the callback directly.
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &mSmbios);
  if (!EFI_ERROR (Status)) {
    SiInitDxeSmbiosCallback (NULL,NULL);
  } else {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SiInitDxeSmbiosCallback,
                    NULL,
                    &CallbackEvent
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                    &gEfiSmbiosProtocolGuid,
                    CallbackEvent,
                    &mRegistration
                    );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Performing SiInitEndOfDxe after the gEfiEndOfDxeEventGroup is signaled.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SiInitEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

