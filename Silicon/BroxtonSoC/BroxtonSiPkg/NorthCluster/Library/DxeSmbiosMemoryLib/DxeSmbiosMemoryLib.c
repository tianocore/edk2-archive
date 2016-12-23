/** @file
  This driver will determine memory configuration information from the chipset
  and memory and create SMBIOS memory structures appropriately.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmbiosMemory.h"

/**
  This library will determine memory configuration information from the chipset
  and memory and report the memory configuration info to the DataHub.

  @param[in]  ImageHandle    Handle for the image of this driver.
  @param[in]  SystemTable    Pointer to the EFI System Table.

  @retval     EFI_SUCCESS    If the data is successfully reported.
  @retval     EFI_NOT_FOUND  If the HOB list could not be located.

**/
EFI_STATUS
EFIAPI
SmbiosMemory (
  )
{
  EFI_STATUS        Status;

  Status = InstallSmbiosType16 ();
  ASSERT_EFI_ERROR (Status);

  Status = InstallSmbiosType17 ();
  ASSERT_EFI_ERROR (Status);

  Status = InstallSmbiosType19 ();
  ASSERT_EFI_ERROR (Status);

  return Status;
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
  @param[out] SmbiosHandle          A unique handle will be assigned to the SMBIOS record.

  @retval     EFI_SUCCESS           Table was added.
  @retval     EFI_OUT_OF_RESOURCES  Table was not added due to lack of system resources.

**/
EFI_STATUS
AddSmbiosEntry (
  IN  EFI_SMBIOS_TABLE_HEADER *Entry,
  IN  CHAR8                   **TableStrings,
  IN  UINT8                   NumberOfStrings,
  OUT EFI_SMBIOS_HANDLE       *SmbiosHandle
  )
{
  EFI_STATUS               Status;
  EFI_SMBIOS_TABLE_HEADER  *Record;
  CHAR8                    *StringPtr;
  UINTN                    Size;
  UINTN                    i;

  //
  // Calculate the total size of the full record
  //
  Size = Entry->Length;

  //
  // Add the size of each non-null string
  //
  if (TableStrings != NULL) {
    for (i = 0; i < NumberOfStrings; i++) {
      if (TableStrings[i] != NULL) {
        Size += AsciiStrSize (TableStrings[i]);
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
  Record = (EFI_SMBIOS_TABLE_HEADER *) AllocateZeroPool (Size);
  if (Record == NULL) {
      return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Record, Entry, Entry->Length);

  //
  // Copy the strings to the end of the record
  //
  StringPtr = ((CHAR8 *) Record) + Entry->Length;
  Size = Size - Entry->Length;
  if (TableStrings != NULL) {
    for (i = 0; i < NumberOfStrings; i++) {
      if (TableStrings[i] != NULL) {
        AsciiStrCpyS (StringPtr, Size, TableStrings[i]);
        StringPtr += AsciiStrSize (TableStrings[i]);
        Size = Size - AsciiStrSize (TableStrings[i]);
      }
    }
  }

  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbios->Add (mSmbios, NULL, SmbiosHandle, Record);

  FreePool (Record);
  return Status;
}

