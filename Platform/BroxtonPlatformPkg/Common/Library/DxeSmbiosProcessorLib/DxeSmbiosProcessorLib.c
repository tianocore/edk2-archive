/** @file
  Smbios Processor Information Driver which produces Smbios type 4 and 7 tables.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <SmbiosCacheInfoHob.h>
#include <SmbiosProcessorInfoHob.h>
#include <DxeSmbiosProcessorLib.h>

#define MAX_PROCESSOR_SOCKET_SUPPORTED 8

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMBIOS_HANDLE mSmbiosHandleArray[MAX_PROCESSOR_SOCKET_SUPPORTED][3];

//
// Cache Information (Type 7)
//
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE7 SmbiosTableType7Data = {
  { EFI_SMBIOS_TYPE_CACHE_INFORMATION, sizeof (SMBIOS_TABLE_TYPE7), 0 },
  TO_BE_FILLED,   ///< SocketDesignation
  TO_BE_FILLED,   ///< CacheConfiguration
  TO_BE_FILLED,   ///< MaximumCacheSize
  TO_BE_FILLED,   ///< InstalledSize
  {               ///< SupportedSRAMType
    TO_BE_FILLED, ///< Other         :1;
    TO_BE_FILLED, ///< Unknown       :1;
    TO_BE_FILLED, ///< NonBurst      :1;
    TO_BE_FILLED, ///< Burst         :1;
    TO_BE_FILLED, ///< PipelineBurst :1;
    TO_BE_FILLED, ///< Synchronous   :1;
    TO_BE_FILLED, ///< Asynchronous  :1;
    TO_BE_FILLED, ///< Reserved      :9;
  },
  {               ///< CurrentSRAMType
    TO_BE_FILLED, ///< Other         :1;
    TO_BE_FILLED, ///< Unknown       :1;
    TO_BE_FILLED, ///< NonBurst      :1;
    TO_BE_FILLED, ///< Burst         :1;
    TO_BE_FILLED, ///< PipelineBurst :1;
    TO_BE_FILLED, ///< Synchronous   :1;
    TO_BE_FILLED, ///< Asynchronous  :1;
    TO_BE_FILLED, ///< Reserved      :9;
  },
  TO_BE_FILLED,   ///< CacheSpeed
  TO_BE_FILLED,   ///< ErrorCorrectionType
  TO_BE_FILLED,   ///< SystemCacheType
  TO_BE_FILLED,   ///< Associativity
};

//
// Processor Information (Type 4)
//
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE4 SmbiosTableType4Data = {
  { EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION, sizeof (SMBIOS_TABLE_TYPE4), 0 },
  TO_BE_FILLED,             ///< Socket
  TO_BE_FILLED,             ///< ProcessorType
  TO_BE_FILLED,             ///< ProcessorFamily
  TO_BE_FILLED,             ///< ProcessorManufacture
  {                         ///< ProcessorId
    { TO_BE_FILLED },
    { TO_BE_FILLED }
  },
  TO_BE_FILLED,             ///< ProcessorVersion
  { TO_BE_FILLED },         ///< Voltage
  TO_BE_FILLED,             ///< ExternalClock
  TO_BE_FILLED,             ///< MaxSpeed
  TO_BE_FILLED,             ///< CurrentSpeed
  TO_BE_FILLED,             ///< Status
  TO_BE_FILLED,             ///< ProcessorUpgrade
  TO_BE_FILLED,             ///< L1CacheHandle
  TO_BE_FILLED,             ///< L2CacheHandle
  TO_BE_FILLED,             ///< L3CacheHandle
  TO_BE_FILLED,             ///< SerialNumber
  TO_BE_FILLED,             ///< AssetTag
  TO_BE_FILLED,             ///< PartNumber
  TO_BE_FILLED,             ///< CoreCount
  TO_BE_FILLED,             ///< EnabledCoreCount
  TO_BE_FILLED,             ///< ThreadCount
  TO_BE_FILLED,             ///< ProcessorCharacteristics
  TO_BE_FILLED,             ///< ProcessorFamily2
  TO_BE_FILLED,             ///< CoreCount2
  TO_BE_FILLED,             ///< EnabledCoreCount2
  TO_BE_FILLED,             ///< ThreadCount2
};

GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TYPE4_STRING_ARRAY SmbiosTableType4Strings = {
  TO_BE_FILLED_STRING,      ///< Socket
  TO_BE_FILLED_STRING,      ///< ProcessorManufacture
  TO_BE_FILLED_STRING,      ///< ProcessorVersion
  TO_BE_FILLED_STRING,      ///< SerialNumber
  TO_BE_FILLED_STRING,      ///< AssetTag
  TO_BE_FILLED_STRING,      ///< PartNumber
};

/**
  Assigns the next string number in sequence, or 0 if string is null or empty.

  @param[in]      String         The string pointer.
  @param[in, out] StringNumber   Pointer to the prior string number in sequence.

  @retval         StringNumber   The next string number in sequence, or 0 if string is null or empty.

**/
UINT8
AssignStringNumber (
  IN     CHAR8 *String,
  IN OUT UINT8 *StringNumber
  )
{
  if ((String == NULL) || (*String == '\0')) {
    return NO_STRING_AVAILABLE;
  } else {
    *StringNumber = *StringNumber + 1;
    return *StringNumber;
  }
}


/**
  Add an SMBIOS table entry using EFI_SMBIOS_PROTOCOL.
  Create the full table record using the formatted section plus each non-null string, plus the terminating (double) null.

  @param[in]  Entry                 The data for the fixed portion of the SMBIOS entry.
                                    The format of the data is determined by EFI_SMBIOS_TABLE_HEADER.
                                    Type. The size of the formatted area is defined by
                                    EFI_SMBIOS_TABLE_HEADER. Length and either followed by a
                                    double-null (0x0000) or a set of null terminated strings and a null.
  @param[in]  TableStrings          Set of string pointers to append onto the full record.
                                    If TableStrings is null, no strings are appended. Null strings
                                    are skipped.
  @param[in]  NumberOfStrings       Number of TableStrings to append, null strings are skipped.
  @param[in]  SmbiosProtocol        Instance of Smbios Protocol
  @param[out] SmbiosHandle          A unique handle will be assigned to the SMBIOS record.

  @retval     EFI_SUCCESS           Table was added.
  @retval     EFI_OUT_OF_RESOURCES  Table was not added due to lack of system resources.
**/
EFI_STATUS
AddSmbiosTableEntry (
  IN  EFI_SMBIOS_TABLE_HEADER *Entry,
  IN  CHAR8                   **TableStrings,
  IN  UINT8                   NumberOfStrings,
  IN  EFI_SMBIOS_PROTOCOL     *SmbiosProtocol,
  OUT EFI_SMBIOS_HANDLE       *SmbiosHandle
  )
{
  EFI_STATUS              Status;
  EFI_SMBIOS_TABLE_HEADER *Record;
  CHAR8                   *StringPtr;
  UINTN                   Size;
  UINTN                   Index;

  //
  // Calculate the total size of the full record
  //
  Size = Entry->Length;

  //
  // Add the size of each non-null string
  //
  if (TableStrings != NULL) {
    for (Index = 0; Index < NumberOfStrings; Index++) {
      if ((TableStrings[Index] != NULL) && (*TableStrings[Index] != '\0')) {
        Size += AsciiStrSize (TableStrings[Index]);
      }
    }
  }

  //
  // Add the size of the terminating double null
  // If there were any strings added, just add the second null
  //
  if (Size == Entry->Length) {
    Size += 2;
  } else {
    Size += 1;
  }

  //
  // Initialize the full record
  //
  Record = (EFI_SMBIOS_TABLE_HEADER *) AllocateZeroPool(Size);
  if (Record == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Record, Entry, Entry->Length);

  //
  // Copy the strings to the end of the record
  //
  StringPtr = ((CHAR8 *) Record) + Entry->Length;
  if (TableStrings != NULL) {
    for (Index = 0; Index < NumberOfStrings; Index++) {
      if ((TableStrings[Index] != NULL) && (*TableStrings[Index] != '\0')) {
        AsciiStrCpyS (StringPtr, Size - Entry->Length - 1, TableStrings[Index]);
        StringPtr += AsciiStrSize (TableStrings[Index]);
        Size -= AsciiStrSize (TableStrings[Index]);
      }
    }
  }

  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, SmbiosHandle, Record);

  FreePool (Record);
  return Status;
}


/**
  Installs the SMBIOS Cache Information type SMBIOS table based on the SMBIOS_CACHE_INFO

  @retval  EFI_UNSUPPORTED          Could not locate SMBIOS protocol
  @retval  EFI_OUT_OF_RESOURCES     Failed to allocate memory for SMBIOS Cache Information type.
  @retval  EFI_SUCCESS              Successfully installed SMBIOS Cache Information type.

**/
EFI_STATUS
EFIAPI
InstallSmbiosCacheInfo (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_PROTOCOL          *SmbiosProtocol;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_PEI_HOB_POINTERS         Hob;
  SMBIOS_CACHE_INFO            *CacheInfo;
  UINT8                        *HobStringBufferPtr;
  UINTN                        StringBufferLength;
  SMBIOS_TABLE_TYPE7           *SmbiosType7Record;
  UINT8                        *SmbiosStringBufferPtr;
  UINT8                        CacheLevel;

  DEBUG ((DEBUG_INFO, "InstallSmbiosProcessorInfo(): Install SMBIOS Cache Information Type.\n"));

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (SmbiosProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get Cache Information HOB
  //
  for (Hob.Raw = GetHobList (); !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    if ((GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid (&Hob.Guid->Name, &gSmbiosCacheInfoHobGuid))) {

      CacheInfo = (SMBIOS_CACHE_INFO *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      HobStringBufferPtr = Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID) + sizeof (SMBIOS_CACHE_INFO);
      StringBufferLength = Hob.Header->HobLength - (sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID) + sizeof (SMBIOS_CACHE_INFO));

      if (CacheInfo->ProcessorSocketNumber >= MAX_PROCESSOR_SOCKET_SUPPORTED) {
        DEBUG ((DEBUG_ERROR, "Error creating SMBIOS table Type 7. Socket number exceed limit\n"));
        continue;
      }

      //
      // Allocate full record, including fixed data region, and string buffer region.
      //
      SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE7) + StringBufferLength);
      SmbiosStringBufferPtr = ((UINT8 *) SmbiosType7Record) + sizeof (SMBIOS_TABLE_TYPE7);

      //
      // Initialize the Header
      //
      SmbiosType7Record->Hdr.Type = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
      SmbiosType7Record->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE7);
      SmbiosType7Record->Hdr.Handle = 0;

      //
      // Initialize fixed data region from HOB
      //
      SmbiosType7Record->SocketDesignation = CacheInfo->SocketDesignationStrIndex;
      SmbiosType7Record->CacheConfiguration = CacheInfo->CacheConfiguration;
      SmbiosType7Record->MaximumCacheSize = CacheInfo->MaxCacheSize;
      SmbiosType7Record->InstalledSize = CacheInfo->InstalledSize;
      *(UINT16 *) &SmbiosType7Record->SupportedSRAMType = CacheInfo->SupportedSramType;
      *(UINT16 *) &SmbiosType7Record->CurrentSRAMType = CacheInfo->CurrentSramType;
      SmbiosType7Record->CacheSpeed = CacheInfo->CacheSpeed;
      SmbiosType7Record->ErrorCorrectionType = CacheInfo->ErrorCorrectionType;
      SmbiosType7Record->SystemCacheType = CacheInfo->SystemCacheType;
      SmbiosType7Record->Associativity = CacheInfo->Associativity;

      //
      // Initialize string buffer region
      //
      CopyMem (SmbiosStringBufferPtr, HobStringBufferPtr, StringBufferLength);

      SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosType7Record);

      FreePool (SmbiosType7Record);

      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Error adding SMBIOS table Type 7. Status = %r\n", Status));
        return Status;
      } else {
        DEBUG ((DEBUG_ERROR, "Adding SMBIOS table Type 7 successfully.\n"));
      }

      CacheLevel = (UINT8) (CacheInfo->CacheConfiguration & 0x7);
      mSmbiosHandleArray[CacheInfo->ProcessorSocketNumber][CacheLevel] = SmbiosHandle;
    }
  }

  return EFI_SUCCESS;
}


/**
  Installs the SMBIOS Processor Information type SMBIOS table based on the SMBIOS_PROCESSOR_INFO

  @retval  EFI_UNSUPPORTED           Could not locate SMBIOS protocol
  @retval  EFI_OUT_OF_RESOURCES      Failed to allocate memory for SMBIOS Processor Information type.
  @retval  EFI_SUCCESS               Successfully installed SMBIOS Processor Information type.

**/
EFI_STATUS
EFIAPI
InstallSmbiosProcessorInfo (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS         Hob;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_PROTOCOL          *SmbiosProtocol;
  EFI_STATUS                   Status;
  SMBIOS_PROCESSOR_INFO        *ProcessorInfo;
  UINT8                        StringNumber;
  UINTN                        String1Size;

  Status = EFI_SUCCESS;
  StringNumber = 0;

  DEBUG ((DEBUG_INFO, "InstallSmbiosProcessorInfo(): Install SMBIOS Processor Information Type.\n"));

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (SmbiosProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get Processor Information HOB
  //
  for (Hob.Raw = GetHobList (); !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    if ((GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid (&Hob.Guid->Name, &gSmbiosProcessorInfoHobGuid))) {

      ProcessorInfo = (SMBIOS_PROCESSOR_INFO *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));

      if (ProcessorInfo->CurrentSocketNumber >= MAX_PROCESSOR_SOCKET_SUPPORTED) {
        DEBUG ((DEBUG_ERROR, "Error creating SMBIOS table Type 4. Socket number exceed limit\n"));
        continue;
      }

      SmbiosTableType4Strings.Socket = (CHAR8 *) PcdGetPtr (PcdSmbiosDefaultSocketDesignation);
      SmbiosTableType4Data.Socket = AssignStringNumber (SmbiosTableType4Strings.Socket, &StringNumber);

      SmbiosTableType4Data.ProcessorType = ProcessorInfo->ProcessorType;
      //
      // Fix this to use correct algorithm
      //
      SmbiosTableType4Data.ProcessorFamily = (UINT8) ProcessorInfo->ProcessorFamily;

      if (ProcessorInfo->ProcessorManufacturerStrIndex == 1) {
        SmbiosTableType4Strings.ProcessorManufacture = (CHAR8 *) (ProcessorInfo + 1);
        SmbiosTableType4Data.ProcessorManufacture = AssignStringNumber (SmbiosTableType4Strings.ProcessorManufacture, &StringNumber);

        String1Size = AsciiStrSize ((CHAR8 *) (ProcessorInfo + 1)); // Size includes the trailing /0
        SmbiosTableType4Strings.ProcessorVersion = (CHAR8 *) (SmbiosTableType4Strings.ProcessorManufacture + String1Size);
        SmbiosTableType4Data.ProcessorVersion = AssignStringNumber (SmbiosTableType4Strings.ProcessorVersion, &StringNumber);
      } else if (ProcessorInfo->ProcessorVersionStrIndex == 1) {
        SmbiosTableType4Strings.ProcessorVersion = (CHAR8 *) (ProcessorInfo + 1);
        SmbiosTableType4Data.ProcessorVersion = AssignStringNumber (SmbiosTableType4Strings.ProcessorVersion, &StringNumber);

        String1Size = AsciiStrSize ((CHAR8 *) (ProcessorInfo + 1)); // Size includes the trailing /0
        SmbiosTableType4Strings.ProcessorManufacture = (CHAR8 *) (SmbiosTableType4Strings.ProcessorVersion + String1Size);
        SmbiosTableType4Data.ProcessorManufacture = AssignStringNumber (SmbiosTableType4Strings.ProcessorManufacture, &StringNumber);
      } else {
        SmbiosTableType4Strings.ProcessorManufacture = NULL;
        SmbiosTableType4Data.ProcessorManufacture = 0;
        SmbiosTableType4Strings.ProcessorVersion = NULL;
        SmbiosTableType4Data.ProcessorVersion = 0;
      }

      *(UINT64 *) &SmbiosTableType4Data.ProcessorId = ProcessorInfo->ProcessorId;

      *(UINT8 *) &SmbiosTableType4Data.Voltage = ProcessorInfo->Voltage;

      SmbiosTableType4Data.ExternalClock = ProcessorInfo->ExternalClockInMHz;

      SmbiosTableType4Data.MaxSpeed = PcdGet16 (PcdSmbiosDefaultMaxSpeed);

      SmbiosTableType4Data.CurrentSpeed = ProcessorInfo->CurrentSpeedInMHz;

      SmbiosTableType4Data.Status = ProcessorInfo->Status;

      SmbiosTableType4Data.ProcessorUpgrade = ProcessorInfo->ProcessorUpgrade;

      SmbiosTableType4Data.L1CacheHandle = mSmbiosHandleArray[ProcessorInfo->CurrentSocketNumber][0];
      SmbiosTableType4Data.L2CacheHandle = mSmbiosHandleArray[ProcessorInfo->CurrentSocketNumber][1];
      SmbiosTableType4Data.L3CacheHandle = mSmbiosHandleArray[ProcessorInfo->CurrentSocketNumber][2];

      SmbiosTableType4Strings.SerialNumber = (CHAR8 *) PcdGetPtr (PcdSmbiosDefaultSerialNumber);
      SmbiosTableType4Data.SerialNumber = AssignStringNumber (SmbiosTableType4Strings.SerialNumber, &StringNumber);
      SmbiosTableType4Strings.AssetTag = (CHAR8 *) PcdGetPtr (PcdSmbiosDefaultAssetTag);
      SmbiosTableType4Data.AssetTag = AssignStringNumber (SmbiosTableType4Strings.AssetTag, &StringNumber);
      SmbiosTableType4Strings.PartNumber = (CHAR8 *) PcdGetPtr (PcdSmbiosDefaultPartNumber);
      SmbiosTableType4Data.PartNumber = AssignStringNumber (SmbiosTableType4Strings.PartNumber, &StringNumber);

      //
      // Fix this to use correct algorithm
      //
      SmbiosTableType4Data.CoreCount = (UINT8) ProcessorInfo->CoreCount;
      SmbiosTableType4Data.EnabledCoreCount = (UINT8) ProcessorInfo->EnabledCoreCount;
      SmbiosTableType4Data.ThreadCount = (UINT8) ProcessorInfo->ThreadCount;

      SmbiosTableType4Data.ProcessorCharacteristics = ProcessorInfo->ProcessorCharacteristics;

      SmbiosTableType4Data.ProcessorFamily2 = ProcessorInfo->ProcessorFamily;
      SmbiosTableType4Data.CoreCount2 = ProcessorInfo->CoreCount;
      SmbiosTableType4Data.EnabledCoreCount2 = ProcessorInfo->EnabledCoreCount;
      SmbiosTableType4Data.ThreadCount2 = ProcessorInfo->ThreadCount;

      Status = AddSmbiosTableEntry (
                 (EFI_SMBIOS_TABLE_HEADER *) &SmbiosTableType4Data,
                 (CHAR8 **) &SmbiosTableType4Strings,
                 SMBIOS_TYPE4_NUMBER_OF_STRINGS,
                 SmbiosProtocol,
                 &SmbiosHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Error adding SMBIOS table Type 4. Status = %r\n", Status));
      } else {
        DEBUG ((DEBUG_ERROR, "Adding SMBIOS table Type 4 successfully.\n"));
      }

    }
  }

  return EFI_SUCCESS;
}


/**
  Add Smbios Cache information (type 7) table and Processor information (type 4) table
  using the HOB info from Silicon.

  It installs Smbios type 4 and type 7 tables.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
AddSmbiosProcessorAndCacheTables (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINT8                        Index1, Index2;

  for (Index1 = 0; Index1 < MAX_PROCESSOR_SOCKET_SUPPORTED; Index1++) {
    for (Index2 = 0; Index2 < 3; Index2++) {
      mSmbiosHandleArray[Index1][Index2] = 0;
    }
  }

  Status = InstallSmbiosCacheInfo ();

  Status = InstallSmbiosProcessorInfo ();

  return Status;
}

