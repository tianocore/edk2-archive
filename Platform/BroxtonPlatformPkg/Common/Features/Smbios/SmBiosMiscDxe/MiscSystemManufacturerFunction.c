/** @file
  This driver parses the mMiscSubclassDataTable structure and reports
  any generated data.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include <Library/SteppingLib.h>
#include "MiscSubclassDriver.h"
#include <Protocol/DxeSmmReadyToLock.h>
#include <Library/PrintLib.h>
#include <BoardFunctionsDxe.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>
#include <IndustryStandard/Emmc.h>
#include <Protocol/SdMmcPassThru.h>
#include <Protocol/DevicePath.h>

/**
  Return the description for network boot device.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetNetworkDescription (
  IN EFI_HANDLE                  Handle
  )
{
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  MAC_ADDR_DEVICE_PATH           *Mac;
  VLAN_DEVICE_PATH               *Vlan;
  EFI_DEVICE_PATH_PROTOCOL       *Ip;
  EFI_DEVICE_PATH_PROTOCOL       *Uri;
  CHAR16                         *Description;
  UINTN                          DescriptionSize;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  gImageHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath,
                  gImageHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status) || (DevicePath == NULL)) {
    return NULL;
  }

  //
  // The PXE device path is like:
  //   ....../Mac(...)[/Vlan(...)][/Wi-Fi(...)]
  //   ....../Mac(...)[/Vlan(...)][/Wi-Fi(...)]/IPv4(...)
  //   ....../Mac(...)[/Vlan(...)][/Wi-Fi(...)]/IPv6(...)
  //
  // The HTTP device path is like:
  //   ....../Mac(...)[/Vlan(...)][/Wi-Fi(...)]/IPv4(...)/Uri(...)
  //   ....../Mac(...)[/Vlan(...)][/Wi-Fi(...)]/IPv6(...)/Uri(...)
  //
  while (!IsDevicePathEnd (DevicePath) &&
         ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
          (DevicePathSubType (DevicePath) != MSG_MAC_ADDR_DP))
         ) {
    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (IsDevicePathEnd (DevicePath)) {
    return NULL;
  }

  Mac = (MAC_ADDR_DEVICE_PATH *) DevicePath;
  DevicePath = NextDevicePathNode (DevicePath);

  //
  // Locate the optional Vlan node
  //
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_VLAN_DP)
      ) {
    Vlan = (VLAN_DEVICE_PATH *) DevicePath;
    DevicePath = NextDevicePathNode (DevicePath);
  } else {
    Vlan = NULL;
  }

  //
  // Skip the optional Wi-Fi node
  //
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_WIFI_DP)
      ) {
    DevicePath = NextDevicePathNode (DevicePath);
  }

  //
  // Locate the IP node
  //
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      ((DevicePathSubType (DevicePath) == MSG_IPv4_DP) ||
       (DevicePathSubType (DevicePath) == MSG_IPv6_DP))
      ) {
    Ip = DevicePath;
    DevicePath = NextDevicePathNode (DevicePath);
  } else {
    Ip = NULL;
  }

  //
  // Locate the URI node
  //
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_URI_DP)
      ) {
    Uri = DevicePath;
    DevicePath = NextDevicePathNode (DevicePath);
  } else {
    Uri = NULL;
  }

  //
  // Build description like below:
  //   "PXEv6 (MAC:112233445566 VLAN1)"
  //   "HTTPv4 (MAC:112233445566)"
  //
  DescriptionSize = sizeof (L"112233445566");
  Description     = AllocatePool (DescriptionSize);
  ASSERT (Description != NULL);
  UnicodeSPrint (
    Description, DescriptionSize,
    L"%02x%02x%02x%02x%02x%02x",
    Mac->MacAddress.Addr[0], Mac->MacAddress.Addr[1], Mac->MacAddress.Addr[2],
    Mac->MacAddress.Addr[3], Mac->MacAddress.Addr[4], Mac->MacAddress.Addr[5]
    );
  return Description;
}

CHAR16 *
GetMacAddressString(
  )
{
  EFI_HANDLE    *Handles;
  UINTN         HandleCount;
  UINT8         Index;
  CHAR16        *MacAddressString = NULL;
  
  //
  // Parse load file protocol
  //
  gBS->LocateHandleBuffer (
       ByProtocol,
       &gEfiLoadFileProtocolGuid,
       NULL,
       &HandleCount,
       &Handles
       );
  for (Index = 0; Index < HandleCount; Index++) {

    MacAddressString = GetNetworkDescription (Handles[Index]);

    if (MacAddressString != NULL) {
      break;
    }
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  return MacAddressString;
}

/**
  Publish the smbios type 1.

  @param[in]  Event     Event whose notification function is being invoked (gEfiDxeSmmReadyToLockProtocolGuid).
  @param[in]  Context   Pointer to the notification functions context, which is implementation dependent.

  @retval     None

**/
VOID
EFIAPI
AddSmbiosManuCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  CHAR8                             *OptionalStrStart;
  UINTN                             ManuStrLen;
  UINTN                             VerStrLen;
  UINTN                             PdNameStrLen;
  UINTN                             SerialNumStrLen;
  UINTN                             SkuNumberStrLen;
  UINTN                             FamilyNameStrLen;
  EFI_STATUS                        Status;
  EFI_STRING                        Manufacturer;
  EFI_STRING                        ProductName;
  EFI_STRING                        Version;
  EFI_STRING                        SerialNumber;
  EFI_STRING                        SkuNumber;
  EFI_STRING                        FamilyName;
  STRING_REF                        TokenToGet;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE1                *SmbiosRecord;
  EFI_MISC_SYSTEM_MANUFACTURER      *ForType1InputData;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  CHAR16                            Buffer[40];
  CHAR16                            PlatformNameBuffer[40];
  CHAR16                            SerialNumberBuffer[sizeof (L"112233445566")];
  EFI_PEI_HOB_POINTERS              GuidHob;
  GET_BOARD_NAME                    GetBoardNameFunc;
  EFI_PLATFORM_INFO_HOB             *PlatformInfo = NULL;
  CHAR16                            *MacAddressString = NULL;
  STATIC BOOLEAN                    mType1Installed = FALSE;

  if (mType1Installed == TRUE) {
    return;
  }

  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      PlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  ForType1InputData = (EFI_MISC_SYSTEM_MANUFACTURER *) Context;

  //
  // First check for invalid parameters.
  //
  if (Context == NULL) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);

  UnicodeSPrint (PlatformNameBuffer, sizeof (PlatformNameBuffer), L"%s", L"Broxton ");

  //
  // Update Board Name from PCD
  //
  if (PlatformInfo != NULL) {
    GetBoardNameFunc = (GET_BOARD_NAME) (UINTN) PcdGet64 (PcdGetBoardNameFunc);
    UnicodeSPrint (PlatformNameBuffer, sizeof (PlatformNameBuffer), L"%s", GetBoardNameFunc(PlatformInfo->BoardId));
  }

  //
  // Silicon Steppings
  //
  switch (BxtStepping ()) {
    case BxtA0:
    case BxtPA0:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"A0 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"A0");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "A0 Stepping Detected\n"));
      break;
    case BxtA1:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"A1 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"A1");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "A1 Stepping Detected\n"));
      break;
    case BxtB0:
    case BxtPB0:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"B0 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"B0");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B0 Stepping Detected\n"));
      break;
    case BxtB1:
    case BxtPB1:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"B1 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"B1");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B1 Stepping Detected\n"));
      break;
    case BxtPB2:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"B2 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"B2");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "B2 Stepping Detected\n"));
      break;
    case BxtC0:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"C0 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"C0");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "C0 Stepping Detected\n"));
      break;
    case BxtPE0:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s%s", PlatformNameBuffer, L"E0 PLATFORM");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME), Buffer, NULL);
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"E0");
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_VERSION), Buffer, NULL);
      DEBUG ((EFI_D_ERROR, "E0 Stepping Detected\n"));
      break;
    default:
      DEBUG ((EFI_D_ERROR, "Unknow Stepping Detected\n"));
      break;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_MANUFACTURER);
  Manufacturer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  ManuStrLen = StrLen (Manufacturer);
  if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_PRODUCT_NAME);
  ProductName = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  PdNameStrLen = StrLen (ProductName);
  if (PdNameStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION);
  Version = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  VerStrLen = StrLen (Version);
  if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  MacAddressString = GetMacAddressString();
  if ( MacAddressString != NULL) {
    UnicodeSPrint (SerialNumberBuffer, sizeof (L"112233445566"), L"%s", GetMacAddressString());
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SYSTEM_SERIAL_NUMBER), SerialNumberBuffer, NULL);
  }
  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_SERIAL_NUMBER);
  SerialNumber = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SerialNumStrLen = StrLen (SerialNumber);
  if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_SKU_NUMBER);
  SkuNumber = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SkuNumberStrLen = StrLen (SkuNumber);
  if (SkuNumberStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_FAMILY_NAME1);
  FamilyName = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  FamilyNameStrLen = StrLen (FamilyName);
  if (FamilyNameStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen + 1 + FamilyNameStrLen + 1 + 1);
  if (SmbiosRecord == NULL) {
    DEBUG ((EFI_D_ERROR, "SmbiosRecord is NULL.\n"));
    return;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen + 1 + FamilyNameStrLen + 1 + 1);

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
  //ForType1InputData->SystemUuid.Data1 = PcdGet32 (PcdProductSerialNumber);
  //ForType1InputData->SystemUuid.Data4[0] = PcdGet8 (PcdEmmcManufacturerId);
  ForType1InputData->SystemUuid.Data1 = (UINT32)MacAddressString [0] + (((UINT32)MacAddressString [1]) << 16);
  ForType1InputData->SystemUuid.Data2 = (UINT16)MacAddressString [2];
  ForType1InputData->SystemUuid.Data3 = (UINT16)MacAddressString [3];
  ForType1InputData->SystemUuid.Data4[0] = (UINT8)MacAddressString [4];
  ForType1InputData->SystemUuid.Data4[1] = (UINT8)(MacAddressString [4] >> 8);
  ForType1InputData->SystemUuid.Data4[2] = (UINT8)MacAddressString [5];
  ForType1InputData->SystemUuid.Data4[3] = (UINT8)(MacAddressString [5] >> 8);
  
  CopyMem ((UINT8 *) (&SmbiosRecord->Uuid),&ForType1InputData->SystemUuid,16);

  SmbiosRecord->WakeUpType = (UINT8) ForType1InputData->SystemWakeupType;

  OptionalStrStart = (CHAR8 *) (SmbiosRecord + 1);
  UnicodeStrToAsciiStr (Manufacturer, OptionalStrStart);
  UnicodeStrToAsciiStr (ProductName, OptionalStrStart + ManuStrLen + 1);
  UnicodeStrToAsciiStr (Version, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1);
  UnicodeStrToAsciiStr (SerialNumber, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1);

  UnicodeStrToAsciiStr (SkuNumber, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 +  VerStrLen + 1 + SerialNumStrLen + 1);
  UnicodeStrToAsciiStr (FamilyName, OptionalStrStart + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + SkuNumberStrLen +1);
  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                     );
  FreePool (SmbiosRecord);
  
  mType1Installed = TRUE;
  
  return;
}


/**
  This function makes boot time changes to the contents of the
  MiscSystemManufacturer (Type 1).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION(MiscSystemManufacturer)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  static BOOLEAN                CallbackIsInstalledManu = FALSE;
  EFI_EVENT                     AddSmbiosManuCallbackEvent;


  if (CallbackIsInstalledManu == FALSE) {
    CallbackIsInstalledManu = TRUE;          // Prevent more than 1 callback.
    DEBUG ((EFI_D_INFO, "Create Smbios Manu callback.\n"));



  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             (EFI_EVENT_NOTIFY)AddSmbiosManuCallback,
             RecordData,
             &AddSmbiosManuCallbackEvent
             );

  }

  return Status;  
}

