/** @file
  The function that processes the Smbios data type 0x83 for SEC.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Protocol/SeCOperation.h>
#include <Library/PrintLib.h>


EFI_STATUS
EFIAPI
AddSmbiosT0x83 (
IN VOID       *RecordData
  )
{
  EFI_STATUS              Status;
  SMBIOS_TABLE_TYPE83     *SmbiosRecord;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_MISC_OEM_TYPE_0x83  *ForType83InputData;
  SEC_OPERATION_PROTOCOL  *SeCOp;
  SEC_INFOMATION          SeCInfo;
  EFI_SMBIOS_PROTOCOL     *Smbios;

  ForType83InputData        = (EFI_MISC_OEM_TYPE_0x83 *) RecordData;

  DEBUG ((EFI_D_INFO, "OEM type 0x83.\n"));
  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);

  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE83) + 1 + 1);
  if (SmbiosRecord == NULL) {
    DEBUG ((EFI_D_ERROR, "SmbiosRecord is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE83) + 1 + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SEC_INFO;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE83);

  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;
  //
  // Get SEC VERSION
  //
  Status = gBS->LocateProtocol (
                  &gEfiSeCOperationProtocolGuid,
                  NULL,
                  (VOID **) &SeCOp
                  );
  if (!EFI_ERROR (Status)) {
    Status = SeCOp->GetPlatformSeCInfo(
                      &SeCInfo
                      );
    if (SeCInfo.SeCVerValid) {
      SmbiosRecord->SeCCapabilities.SecFwMinorVersion = SeCInfo.SeCVer.CodeMinor;
      SmbiosRecord->SeCCapabilities.SecFwMajorVersion = SeCInfo.SeCVer.CodeMajor;
      SmbiosRecord->SeCCapabilities.SecFwBuildVersion = SeCInfo.SeCVer.CodeBuildNo;
      SmbiosRecord->SeCCapabilities.SecFwHotfixVersion= SeCInfo.SeCVer.CodeHotFix;
      SmbiosRecord->SeCCapabilities.SecEnable = SeCInfo.SeCEnable;
      } else {
        SmbiosRecord->SeCCapabilities.SecFwMinorVersion = ForType83InputData->SecFwMinorVersion;
        SmbiosRecord->SeCCapabilities.SecFwMajorVersion = ForType83InputData->SecFwMajorVersion;
        SmbiosRecord->SeCCapabilities.SecFwBuildVersion = ForType83InputData->SecFwBuildVersion;
        SmbiosRecord->SeCCapabilities.SecFwHotfixVersion= ForType83InputData->SecFwHotfixVersion;
        SmbiosRecord->SeCCapabilities.SecEnable = 0;
      }
  }

  SmbiosRecord->BiosSeCCapabilities.SecBiosSetup = 1;
  //
  // AT is not supported yet.
  //
  SmbiosRecord->SeCCapabilities.ATSupport = 0;
  SmbiosRecord->BiosSeCCapabilities.ATPBA = 0;
  SmbiosRecord->BiosSeCCapabilities.ATWWAN = 0;
  SmbiosRecord->SeCCfgState.ATConfigured = 0;

  //
  //This field is the unique identifier for Intel? vPro specific SMBIOS type 131 structure
  //
  SmbiosRecord->StructureIdentifer.Identifer = IntelIdentifer;

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios-> Add (
                      Smbios,
                      NULL,
                      &SmbiosHandle,
                      (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                      );

  FreePool (SmbiosRecord);
  return Status;
}


/**
  Smbios OEM type 0x83 callback.

  @param[in]  Event        Event whose notification function is being invoked (gEfiSeCOperationProtocolGuid).
  @param[in]  Context      Pointer to the notification functions context, which is implementation dependent.

  @retval     None

**/
VOID
EFIAPI
AddSmbiosT0x83Callback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  AddSmbiosT0x83 (Context);
}


/**
  This function makes boot time changes to the contents of the
  MiscOemType0x83 (Type 0x83).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/

MISC_SMBIOS_TABLE_FUNCTION(MiscOemType0x83)
{
  EFI_STATUS                    Status;
  VOID                          *AddSmbiosT0x83CallbackNotifyReg;
  EFI_EVENT                     AddSmbiosT0x83CallbackEvent;
  SEC_OPERATION_PROTOCOL        *SeCOp;

  Status = gBS->LocateProtocol (
                  &gEfiSeCOperationProtocolGuid,
                  NULL,
                  (VOID **) &SeCOp
                  );
  if (!EFI_ERROR (Status)) {
    Status = AddSmbiosT0x83 (RecordData);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    //
    // Create a callback function when gEfiSeCOperationProtocolGuid is ready
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    AddSmbiosT0x83Callback,
                    RecordData,
                    &AddSmbiosT0x83CallbackEvent
                    );

    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->RegisterProtocolNotify (
                    &gEfiSeCOperationProtocolGuid,
                    AddSmbiosT0x83CallbackEvent,
                    &AddSmbiosT0x83CallbackNotifyReg
                    );
  }

  return EFI_SUCCESS;
}

