/*++

Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.



Module Name:

  MiscSystemManufacturerFunction.c

Abstract:

  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data.

--*/


#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Protocol/DxeSmmReadyToLock.h>
#include <Library/NetLib.h>
#include "Library/DebugLib.h"
#include <Uefi/UefiBaseType.h>
#include <Guid/PlatformInfo.h>


extern EFI_PLATFORM_INFO_HOB *mPlatformInfo;
static EFI_SMBIOS_HANDLE     mSmbiosHandleType1;


EFI_STATUS
EFIAPI
UpdateSmbiosManuCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            *Handles;
  UINTN                 BufferSize;
  CHAR16                *MacStr;
  EFI_SMBIOS_PROTOCOL   *Smbios;
  UINTN                 SerialNumberOffset;
  CHAR8                 AsciiData[SMBIOS_STRING_MAX_LENGTH];

  gBS->CloseEvent (Event);    // Unload this event.

  DEBUG ((EFI_D_INFO, "Executing UpdateSmbiosManuCallback.\n"));

  //
  //Get handle infomation
  //
  BufferSize = 0;
  Handles = NULL;
  Status = gBS->LocateHandle (
                  ByProtocol,
                  &gEfiSimpleNetworkProtocolGuid,
                  NULL,
                  &BufferSize,
                  Handles
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    Handles = AllocateZeroPool(BufferSize);
    if (Handles == NULL) {
    return (EFI_OUT_OF_RESOURCES);
    }
    Status = gBS->LocateHandle(
                    ByProtocol,
                    &gEfiSimpleNetworkProtocolGuid,
                    NULL,
                    &BufferSize,
                    Handles
                    );
  }

  //
  //Get the MAC string
  //
  Status = NetLibGetMacString (
             *Handles,
             NULL,
             &MacStr
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (AsciiData, SMBIOS_STRING_MAX_LENGTH);
  UnicodeStrToAsciiStr (MacStr, AsciiData);

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID *) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SerialNumberOffset = 4;
  Status = Smbios->UpdateString (
                     Smbios,
                     &mSmbiosHandleType1,
                     &SerialNumberOffset,
                     AsciiData
                     );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  return EFI_SUCCESS;
}


/**

  Publish the smbios type 1.

  @param Event      Event whose notification function is being invoked (gEfiDxeSmmReadyToLockProtocolGuid).
  @param Context    Pointer to the notification functions context, which is implementation dependent.

  @retval None

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscSystemManufacturer)
{

  CHAR8                             *OptionalStrStart;
  UINTN                             ManuStrLen;
  UINTN                             VerStrLen;
  UINTN                             PdNameStrLen;
  UINTN                             SerialNumStrLen;
  UINTN                             SkuNumberStrLen;
  UINTN                        FamilyNameStrLen;
  EFI_STATUS                        Status;
  EFI_STRING                        Manufacturer;
  EFI_STRING                        ProductName;
  EFI_STRING                        Version;
  EFI_STRING                        SerialNumber;
  EFI_STRING                        SkuNumber;
  EFI_STRING                  FamilyName;
  STRING_REF                        TokenToGet;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE1                *SmbiosRecord;
  EFI_MISC_SYSTEM_MANUFACTURER      *ForType1InputData;
  CHAR16                            Buffer[40];
  CHAR16                            *MacStr;
  CHAR16                            PlatformNameBuffer[40];
  VOID                              *UpdateSmbiosManuCallbackNotifyReg;
  EFI_EVENT                         UpdateSmbiosManuCallbackEvent;
  static BOOLEAN                    CallbackIsInstalledT1 = FALSE;

  ForType1InputData = (EFI_MISC_SYSTEM_MANUFACTURER *)RecordData;
  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL || mPlatformInfo == NULL) {
    DEBUG ((EFI_D_INFO, "MISC_SMBIOS_TABLE_FUNCTION error.\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (BOARD_ID_MINNOW2_TURBOT == mPlatformInfo->BoardId) {
    //
    // Detect the board is Turbot board platform
    //
    UnicodeSPrint (PlatformNameBuffer, sizeof (PlatformNameBuffer),L"%s",L"Minnowboard Turbot ");
  } else {
    UnicodeSPrint (PlatformNameBuffer, sizeof (PlatformNameBuffer),L"%s",L"Minnowboard Max ");
  }

  //
  // Silicon Steppings
  //
  switch (PchStepping()) {
    case PchA0:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"A0 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"A0");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "A0 Stepping Detected\n"));
      break;
    case PchA1:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"A1 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"A1");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "A1 Stepping Detected\n"));
      break;
    case PchB0:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"B0 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"B0");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B0 Stepping Detected\n"));
      break;
    case PchB1:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"B1 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"B1");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B1 Stepping Detected\n"));
      break;
    case PchB2:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"B2 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"B2");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B2 Stepping Detected\n"));
      break;
    case PchB3:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"B3 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"B3");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B3 Stepping Detected\n"));
      break;
    case PchC0:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"C0 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"C0");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "C0 Stepping Detected\n"));
      break;
   case PchD0:
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s%s", PlatformNameBuffer, L"D0 PLATFORM");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer),L"%s",L"D0");
      HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "D0 Stepping Detected\n"));
      break;
    default:
      DEBUG ((EFI_D_ERROR, "Unknow Stepping Detected\n"));
      break;
    }

  if (BOARD_ID_MINNOW2_TURBOT == mPlatformInfo->BoardId) {
    UnicodeSPrint (Buffer, sizeof (Buffer),L"ADI");
    HiiSetString(mHiiHandle,STRING_TOKEN(STR_MISC_SYSTEM_MANUFACTURER), Buffer, NULL);
  }
  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_MANUFACTURER);
  Manufacturer = SmbiosMiscGetString (TokenToGet);
  ManuStrLen = StrLen(Manufacturer);
  if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME);
  ProductName = SmbiosMiscGetString (TokenToGet);
  PdNameStrLen = StrLen(ProductName);
  if (PdNameStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION);
  Version = SmbiosMiscGetString (TokenToGet);
  VerStrLen = StrLen(Version);
  if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  MacStr = L"00000000";
  SerialNumber = MacStr;
  SerialNumStrLen = StrLen(SerialNumber);
  if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }
  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_SKU_NUMBER);
  SkuNumber = SmbiosMiscGetString (TokenToGet);
  SkuNumberStrLen = StrLen(SkuNumber);
  if (SkuNumberStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }
  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_FAMILY_NAME1);
  FamilyName = SmbiosMiscGetString (TokenToGet);
  FamilyNameStrLen = StrLen(FamilyName);
  if (FamilyNameStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen + 1 + FamilyNameStrLen + 1 + 1);
  ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen + 1 + FamilyNameStrLen + 1 + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE1);

  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;

  //
  // Manu will be the 1st optional string following the formatted structure.
  //
  SmbiosRecord->Manufacturer = 1;

  //
  // ProductName will be the 2nd optional string following the formatted structure.
  //
  SmbiosRecord->ProductName = 2;

  //
  // Version will be the 3rd optional string following the formatted structure.
  //
  SmbiosRecord->Version = 3;

  //
  // Version will be the 4th optional string following the formatted structure.
  //
  SmbiosRecord->SerialNumber = 4;

  SmbiosRecord->SKUNumber= 5;
  SmbiosRecord->Family= 6;

  //
  // Unique UUID
  //
  ForType1InputData->SystemUuid.Data1 = PcdGet32 (PcdProductSerialNumber);
  ForType1InputData->SystemUuid.Data4[0] = PcdGet8 (PcdEmmcManufacturerId);

  CopyMem ((UINT8 *) (&SmbiosRecord->Uuid),&ForType1InputData->SystemUuid,16);

  SmbiosRecord->WakeUpType = (UINT8)ForType1InputData->SystemWakeupType;

  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  UnicodeStrToAsciiStr(Manufacturer, OptionalStrStart);
  UnicodeStrToAsciiStr(ProductName, OptionalStrStart + ManuStrLen + 1);
  UnicodeStrToAsciiStr(Version, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1);
  UnicodeStrToAsciiStr(SerialNumber, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1);

  UnicodeStrToAsciiStr(SkuNumber, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 +  VerStrLen + 1 + SerialNumStrLen + 1);
  UnicodeStrToAsciiStr(FamilyName, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen +1);

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios-> Add(
                      Smbios,
                      NULL,
                      &SmbiosHandle,
                      (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                      );
  FreePool(SmbiosRecord);

  //
  // gEfiSimpleNetworkProtocolGuid is ready
  //
  if (CallbackIsInstalledT1 == FALSE) {
    CallbackIsInstalledT1 = TRUE;          // Prevent more than 1 callback.
    DEBUG ((EFI_D_INFO, "Create Smbios Type1 callback.\n"));

    mSmbiosHandleType1 = SmbiosHandle;
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)UpdateSmbiosManuCallback,
                    RecordData,
                    &UpdateSmbiosManuCallbackEvent
                    );

    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;

    }

    Status = gBS->RegisterProtocolNotify (
                    &gEfiSimpleNetworkProtocolGuid,
                    UpdateSmbiosManuCallbackEvent,
                    &UpdateSmbiosManuCallbackNotifyReg
                    );
    return Status;
  }

  return EFI_SUCCESS;
}

