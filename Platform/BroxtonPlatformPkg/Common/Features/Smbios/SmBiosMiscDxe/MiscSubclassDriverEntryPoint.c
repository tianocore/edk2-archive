/** @file
  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data to the DataHub.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Library/SteppingLib.h>
EFI_HII_HANDLE  mHiiHandle;


/**
  Publish the Driver/firmware version info using smbios type 14, group association.

  @param[in]  Event     Event whose notification function is being invoked (Ready To Boot).
  @param[in]  Context   Pointer to the notification functions context, which is implementation dependent.

  @retval     None

**/
VOID
EFIAPI
AddSmbiosT14Callback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_SMBIOS_PROTOCOL               *SmbiosProtocol;
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER           *Record;
  UINT32                            FviTypeCount;       // number of Fvi records locates.
  UINT32                            FviT14Size;         // Size of the SMBIOS record to be installed.
  GROUP_STRUCT                      *FviTableOld;       // Pointers to discovered Fvi entries.
  GROUP_STRUCT                      *FviTableNew;
  SMBIOS_TABLE_TYPE14               *SmbiosTableType14;
  UINT8                             *GroupName;
  static UINT8                      T14FviString[sizeof (T14_FVI_STRING)] = T14_FVI_STRING;

  SmbiosProtocol          = NULL;
  FviTableNew             = NULL;

  DEBUG ((EFI_D_INFO, "Executing SMBIOS T14 callback.\n"));

  gBS->CloseEvent (Event);    // Unload this event.

  //
  // Parse the SMBIOS records for
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  ASSERT_EFI_ERROR (Status);

  FviTypeCount  = 0;
  SmbiosHandle  = 0;

  do {
    Status = SmbiosProtocol->GetNext (SmbiosProtocol, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }
    //
    // Only One EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS with the label T14FviString.
    // is allowed in the system.
    //
    if (Record->Type == EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS) {
      GroupName = ((UINT8 * )Record) + Record->Length;
      if (AsciiStrnCmp ((CHAR8 *) GroupName, (CHAR8 *) T14FviString, AsciiStrLen ((CHAR8 *) GroupName)) == 0) {
        FviTypeCount = 0;       //  mark the set as empty
        break;
      }
    }
    //
    // Locate the FviSmbiosType records.
    //
    if (Record->Type == EFI_SMBIOS_TYPE_FIRMWARE_VERSION_INFO) {
      FviTypeCount++;
      FviTableOld = FviTableNew;
      FviTableNew      = AllocateZeroPool (sizeof (GROUP_STRUCT) * FviTypeCount);
      if (FviTableNew == NULL) {
        DEBUG ((EFI_D_ERROR, "FviTableNew is NULL.\n"));
        return;
      }

      if (FviTypeCount > 1) {
        CopyMem (FviTableNew, FviTableOld, (sizeof (GROUP_STRUCT) * FviTypeCount));
      }

      if (FviTableOld != NULL) {
        gBS->FreePool (FviTableOld);
      }

      FviTableNew[FviTypeCount - 1].ItemType    = Record->Type;
      FviTableNew[FviTypeCount - 1].ItemHandle  = Record->Handle;
    }
  } while (SmbiosHandle != 0);      // End of retrieving Smbios FviSmbiosType records.


  if (FviTypeCount != 0) {

    //
    // Add the Record to the SMBIOS table.
    //
    FviT14Size = sizeof (SMBIOS_TABLE_TYPE14) + (UINT32) AsciiStrLen (T14_FVI_STRING) + (sizeof (GROUP_STRUCT) * (FviTypeCount - 1)) + 2;

    SmbiosTableType14 = AllocateZeroPool (FviT14Size);
    if (SmbiosTableType14 == NULL) {
      DEBUG ((EFI_D_ERROR, "SmbiosTableType14 is NULL.\n"));
      return;
    }

    SmbiosTableType14->Hdr.Type = EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS;
    SmbiosTableType14->Hdr.Length = (UINT8) (sizeof (SMBIOS_TABLE_TYPE14) + (sizeof (GROUP_STRUCT) * (FviTypeCount - 1)));
    SmbiosTableType14->Hdr.Handle = SMBIOS_HANDLE_PI_RESERVED;  // Assign an unused handle.
    SmbiosTableType14->GroupName  = 1;
    CopyMem (SmbiosTableType14->Group, FviTableNew, sizeof (GROUP_STRUCT) * FviTypeCount);
    CopyMem ((&SmbiosTableType14->Group[FviTypeCount].ItemType), T14FviString, sizeof (T14FviString));

    Status = SmbiosProtocol->Add (
                               SmbiosProtocol,
                               NULL,
                               &SmbiosTableType14->Hdr.Handle,
                               (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType14
                               );
    FreePool (SmbiosTableType14);
  }
    if (FviTableNew != NULL) {
    FreePool (FviTableNew);
  }
}


/**
  Standard EFI driver point.  This driver parses the mMiscSubclassDataTable
  structure and reports any generated data to the DataHub.

  @param[in]  ImageHandle         Handle for the image of this driver
  @param[in]  SystemTable         Pointer to the EFI System Table

  @retval     EFI_SUCCESS         The data was successfully reported to the Data Hub.
  @retval     EFI_DEVICE_ERROR    Can not locate any protocols

**/
EFI_STATUS
EFIAPI
MiscSubclassDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINTN                         Index;
  EFI_STATUS                    EfiStatus;
  EFI_SMBIOS_PROTOCOL           *Smbios;
  static BOOLEAN                CallbackIsInstalledT14 = FALSE;
  EFI_EVENT                     Event;

  EfiStatus = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);

  if (EFI_ERROR (EfiStatus)) {
    DEBUG ((EFI_D_ERROR, "Could not locate SMBIOS protocol.  %r\n", EfiStatus));
    return EfiStatus;
  }

  mHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 NULL,
                 MiscSubclassStrings,
                 NULL
                 );
  ASSERT (mHiiHandle != NULL);

  if (GetBxtSeries () != Bxt1) {
    for (Index = 0; Index < mMiscSubclassDataTableEntries; ++Index) {
      //
      // If the entry have a function pointer, just log the data.
      //
      if (mMiscSubclassDataTable[Index].Function != NULL) {
        EfiStatus = (*mMiscSubclassDataTable[Index].Function) (
          mMiscSubclassDataTable[Index].RecordData,
          Smbios
          );

          if (EFI_ERROR (EfiStatus)) {
            DEBUG ((EFI_D_ERROR, "Misc smbios store error.  Index=%d, ReturnStatus=%r\n", Index, EfiStatus));
            return EfiStatus;
          }
       }
    }
  } else {
    //
    // BXTM B0 SMBIOS tables
    //
    for (Index = 0; Index < mMiscSubclassDataTableEntriesB0; ++Index) {
    //
    // If the entry have a function pointer, just log the data.
    //
      if (mMiscSubclassDataTableB0[Index].Function != NULL) {
        EfiStatus = (*mMiscSubclassDataTableB0[Index].Function)(
        mMiscSubclassDataTableB0[Index].RecordData,
         Smbios
         );

        if (EFI_ERROR(EfiStatus)) {
          DEBUG((EFI_D_ERROR, "Misc smbios store error.  Index=%d, ReturnStatus=%r\n", Index, EfiStatus));
          return EfiStatus;
        }
      }
    }
  }

  //
  // Add Smbios Type 4 and Type7 information
  //
  EfiStatus = AddSmbiosProcessorAndCacheTables ();
  if (EFI_ERROR (EfiStatus)) {
    DEBUG ((EFI_D_ERROR, "Add Smbios Processor And Cache Tables, Return Status =%d\n", EfiStatus));
    return EfiStatus;
  }
  //
  // This callback will parse the SMBIOS table for FVI and create a Type 14 record.
  //
  if (CallbackIsInstalledT14 == FALSE) {
    CallbackIsInstalledT14 = TRUE;          // Prevent more than 1 callback.
    DEBUG ((EFI_D_INFO, "Create Smbios T14 callback.\n"));
    EfiStatus = EfiCreateEventReadyToBootEx (
                  TPL_CALLBACK,
                  AddSmbiosT14Callback,
                  NULL,
                  &Event
                  );
    ASSERT_EFI_ERROR (EfiStatus);
  }

  return EfiStatus;
}

