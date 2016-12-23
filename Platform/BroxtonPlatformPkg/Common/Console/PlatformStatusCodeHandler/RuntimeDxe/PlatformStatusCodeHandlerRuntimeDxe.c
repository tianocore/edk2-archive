/** @file
  Platform status code implementation.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformStatusCodeHandlerRuntimeDxe.h"

EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol       = NULL;
EFI_EVENT                 mExitBootServicesEvent     = NULL;

BOOLEAN
IsFastBootEnabled (
  VOID
  )
{
  BOOLEAN                 FastBootEnabledStatus;
  EFI_PEI_HOB_POINTERS    Hob;

  FastBootEnabledStatus = FALSE;
  Hob.Raw = GetFirstGuidHob (&gFastBootFunctionEnabledHobGuid);
  if (Hob.Raw != NULL) {
    FastBootEnabledStatus = TRUE;
  }

  return FastBootEnabledStatus;
}


/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param[in]  Event         Event whose notification function is being invoked.
  @param[in]  Context       Pointer to the notification function's context, which is
                            always zero in current implementation.

**/
VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  if (FeaturePcdGet (PcdStatusCodeUseBeep)) {
    mRscHandlerProtocol->Unregister (BeepStatusCodeReportWorker);
  }

  if ((FeaturePcdGet (PcdStatusCodeUseSerialPortPlatform)) &&
      (FeaturePcdGet (PcdStatusCodeUnregisterSerialPort))) {
    mRscHandlerProtocol->Unregister (SerialStatusCodeReportWorker);
  }
}


/**
  Entry point of DXE Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param[in]  ImageHandle       The firmware allocated handle for the EFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval     EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
StatusCodeHandlerRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;

  Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mRscHandlerProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  if (FeaturePcdGet (PcdStatusCodeUseBeep)) {
    if (!IsFastBootEnabled ()) {
      mRscHandlerProtocol->Register (BeepStatusCodeReportWorker, TPL_HIGH_LEVEL);
    }
  }

  if (FeaturePcdGet (PcdStatusCodeUseSerialPortPlatform)) {
    mRscHandlerProtocol->Register (SerialStatusCodeReportWorker, TPL_HIGH_LEVEL);
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UnregisterBootTimeHandlers,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );

  return EFI_SUCCESS;
}

