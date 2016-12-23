/** @file
  BaseBoard manufacturer information boot time changes.
  SMBIOS type 2.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Guid/PlatformInfo.h>
#include <Library/SteppingLib.h>

/**
  This function makes boot time changes to the contents of the
  MiscBaseBoardManufacturer (Type 2).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION (MiscBaseBoardManufacturer)
{
  CHAR8                             *OptionalStrStart;
  UINTN                             ManuStrLen;
  UINTN                             ProductStrLen;
  UINTN                             VerStrLen;
  UINTN                             AssertTagStrLen;
  UINTN                             SerialNumStrLen;
  UINTN                             ChassisStrLen;
  EFI_STATUS                        Status;
  EFI_STRING                        Manufacturer;
  EFI_STRING                        Product;
  EFI_STRING                        Version;
  EFI_STRING                        SerialNumber;
  EFI_STRING                        AssertTag;
  EFI_STRING                        Chassis;
  STRING_REF                        TokenToGet;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE2                *SmbiosRecord;
  EFI_MISC_BASE_BOARD_MANUFACTURER  *ForType2InputData;
  EFI_PEI_HOB_POINTERS              GuidHob;
  EFI_PLATFORM_INFO_HOB             *PlatformInfo = NULL;

  ForType2InputData = (EFI_MISC_BASE_BOARD_MANUFACTURER *) RecordData;
  ZeroMem (&GuidHob, sizeof (EFI_PEI_HOB_POINTERS));
  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      PlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  if (PlatformInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "PlatformInfo is NULL.\n"));
    return EFI_NOT_FOUND;
  }

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_MANUFACTURER);
  Manufacturer = HiiGetPackageString(&gEfiCallerIdGuid, TokenToGet, NULL);
  ManuStrLen = StrLen (Manufacturer);
  if (ManuStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_PRODUCT_NAME1);
  Product = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  ProductStrLen = StrLen (Product);
  if (ProductStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  switch (BxtStepping ()) {
    case BxtA0:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_A0);
      break;
    case BxtA1:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_A1);
      break;
    case BxtB0:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_B0);
      break;
    case BxtB1:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_B1);
      break;
    case BxtC0:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_C0);
      break;
    default:
      TokenToGet = STRING_TOKEN (STR_MISC_SYSTEM_VERSION_A0);
      break;
  }

  Version = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  VerStrLen = StrLen (Version);
  if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_SERIAL_NUMBER);
  SerialNumber = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SerialNumStrLen = StrLen (SerialNumber);
  if (SerialNumStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_ASSET_TAG);
  AssertTag = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  AssertTagStrLen = StrLen (AssertTag);
  if (AssertTagStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BASE_BOARD_CHASSIS_LOCATION);
  Chassis = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  ChassisStrLen = StrLen (Chassis);
  if (ChassisStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return EFI_UNSUPPORTED;
  }

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE2) + ManuStrLen + 1 + ProductStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + AssertTagStrLen + 1 + ChassisStrLen +1 + 1);
  if (SmbiosRecord == NULL) {
    DEBUG ((EFI_D_ERROR, "SmbiosRecord is NULL.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE2) + ManuStrLen + 1 + ProductStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + AssertTagStrLen + 1 + ChassisStrLen +1 + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE2);
  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;
  //
  // Manu will be the 1st optional string following the formatted structure.
  //
  SmbiosRecord->Manufacturer = 1;
  //
  // ProductName will be the 2st optional string following the formatted structure.
  //
  SmbiosRecord->ProductName  = 2;
  //
  // Version will be the 3rd optional string following the formatted structure.
  //
  SmbiosRecord->Version = 3;
  //
  // SerialNumber will be the 4th optional string following the formatted structure.
  //
  SmbiosRecord->SerialNumber = 4;
  //
  // AssertTag will be the 5th optional string following the formatted structure.
  //
  SmbiosRecord->AssetTag = 5;

  //
  // LocationInChassis will be the 6th optional string following the formatted structure.
  //
  SmbiosRecord->LocationInChassis = 6;
  SmbiosRecord->FeatureFlag = (*(BASE_BOARD_FEATURE_FLAGS*) &(ForType2InputData->BaseBoardFeatureFlags));
  SmbiosRecord->ChassisHandle  = 0;
  SmbiosRecord->BoardType      = (UINT8) ForType2InputData->BaseBoardType;
  SmbiosRecord->NumberOfContainedObjectHandles = 0;

  OptionalStrStart = (CHAR8 *) (SmbiosRecord + 1);
  //
  // Since we fill NumberOfContainedObjectHandles = 0 for simple, just after this filed to fill string
  //
  UnicodeStrToAsciiStr (Manufacturer, OptionalStrStart);
  UnicodeStrToAsciiStr (Product, OptionalStrStart + ManuStrLen + 1);
  UnicodeStrToAsciiStr (Version, OptionalStrStart + ManuStrLen + 1 + ProductStrLen + 1);
  UnicodeStrToAsciiStr (SerialNumber, OptionalStrStart + ManuStrLen + 1 + ProductStrLen + 1 + VerStrLen + 1);
  UnicodeStrToAsciiStr (AssertTag, OptionalStrStart + ManuStrLen + 1 + ProductStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1);
  UnicodeStrToAsciiStr (Chassis, OptionalStrStart + ManuStrLen + 1 + ProductStrLen + 1 + VerStrLen + 1 + SerialNumStrLen + 1 + AssertTagStrLen + 1);
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

