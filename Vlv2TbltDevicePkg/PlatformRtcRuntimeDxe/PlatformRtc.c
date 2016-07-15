/** @file
  Platform Real Time Clock Set Time driver.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformRtc.h"

EFI_SET_TIME mOriginalSetTime;


/**
  Save RTC Time to UEFI variable, and Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.

**/
EFI_STATUS
EFIAPI
PlatformRtcSetTime (
  IN EFI_TIME                *Time
  )
{
  EFI_STATUS      Status;
  EFI_TIME        RtcTime;
  EFI_TIME        RtcTime2;
  UINTN           DataSize;

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RtcTime = *Time;
  //
  // Save user setting time to variable.
  //
  if (RtcTime.Year != PcdGet16 (PcdMinimalValidYear)) {
    DataSize = sizeof (EFI_TIME);
    Status = gRT->GetVariable(
                    L"SystemRtcTime",
                    &gSystemRtcTimeVariableGuid,
                    NULL,
                    &DataSize,
                    &RtcTime2
                    );
    if (EFI_ERROR(Status)) {
      Status =  EfiSetVariable (
                  SYSTEM_TIME_NAME,
                  &gSystemRtcTimeVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (RtcTime),
                  &RtcTime
                  );
      ASSERT(Status == EFI_SUCCESS);
    } else {
        if (!((RtcTime.Year == RtcTime2.Year) && (RtcTime.Month == RtcTime2.Month) && (RtcTime.Day == RtcTime2.Day) &&
            (RtcTime.Hour == RtcTime2.Hour) && (RtcTime.Minute == RtcTime2.Minute) && (RtcTime.Second == RtcTime2.Second))) {
          //
          // If the time to be set is the same as the saved RTC time, we do not need save the RTC time again.
          //
          Status =  EfiSetVariable (
                      SYSTEM_TIME_NAME,
                      &gSystemRtcTimeVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      sizeof (RtcTime),
                      &RtcTime
                      );
          ASSERT(Status == EFI_SUCCESS);
        }
    }
  }
  
  return mOriginalSetTime(Time);
}

VOID
EFIAPI
PlatformRtcSetTimeVirtualAddressChangeEvent (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  gRT->ConvertPointer (0, (VOID **) &mOriginalSetTime);
}

/**
  The User Entry Point for Platform RTC module.

  This is the entry point for Platform RTC module. It installs the UEFI runtime service
  including Platform SetTime().

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
InitializePlatformRtc (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS     Status;
  EFI_EVENT      Event;

  //
  // Make sure we can handle virtual address changes.
  //
  Event = NULL;
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PlatformRtcSetTimeVirtualAddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  mOriginalSetTime = gRT->SetTime;

  gRT->SetTime = PlatformRtcSetTime;

  return EFI_SUCCESS;
}

