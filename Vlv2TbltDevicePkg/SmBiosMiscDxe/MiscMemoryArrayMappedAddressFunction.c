/** @file
  Dynamic data of Physical Memory Array Mapped Address. SMBIOS Type 19.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Protocol/DataHub.h>
#include <Guid/DataHubRecords.h>
#include <Protocol/MemInfo.h>

#define MAX_SOCKETS  2
#define SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES     0xFFFFFFFF


VOID
GetType16Handle (
  IN  EFI_SMBIOS_PROTOCOL      *Smbios,
  OUT  EFI_SMBIOS_HANDLE       *Handle
  )
{
  EFI_STATUS                 Status;
  EFI_SMBIOS_TYPE            RecordType;
  EFI_SMBIOS_TABLE_HEADER    *Buffer;

  *Handle = 0;
  RecordType = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;

  Status = Smbios->GetNext (
                     Smbios,
                     Handle,
                     &RecordType,
                     &Buffer,
                     NULL
                     );
  if (!EFI_ERROR (Status)) {
    return;
  }

  *Handle = 0xFFFF;
}


/**
  This function installs SMBIOS Type 19 Table(Physical Memory Array).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/
MISC_SMBIOS_TABLE_FUNCTION (MiscMemoryArrayMappedAddress)
{
  EFI_STATUS                               Status;
  UINT64                                   TotalMemorySizeInKB;
  UINT8                                    Dimm;
  EFI_SMBIOS_HANDLE                        SmbiosHandle;
  EFI_MEMORY_ARRAY_START_ADDRESS_DATA      *ForType19InputData;
  SMBIOS_TABLE_TYPE19                      *SmbiosRecord;
  MEM_INFO_PROTOCOL                        *MemInfoHob;
  UINT16                                   Type16Handle = 0;

  TotalMemorySizeInKB = 0;

  //
  // First check for invalid parameters.
  //
  if (RecordData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ForType19InputData        = (EFI_MEMORY_ARRAY_START_ADDRESS_DATA *)RecordData;

  //
  // Two zeros following the last string.
  //
  SmbiosRecord = AllocatePool (sizeof (SMBIOS_TABLE_TYPE19) + 1);
  ZeroMem (SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE19) + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE19);

  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;

  //
  // Get Memory size parameters for each rank from the chipset registers
  //
  Status = gBS->LocateProtocol (
                  &gMemInfoProtocolGuid,
                  NULL,
                  (void **) &MemInfoHob
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Calculate the TotalMemorySizeInKB by adding the size of all populated sockets
  //
  for (Dimm = 0; Dimm < MAX_SOCKETS; Dimm++) {
    TotalMemorySizeInKB += LShiftU64 (MemInfoHob->MemInfoData.dimmSize[Dimm], 10);
  }

  if (TotalMemorySizeInKB > SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES) {
    SmbiosRecord->StartingAddress = SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES;
    SmbiosRecord->EndingAddress = SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES;
    SmbiosRecord->ExtendedEndingAddress = TotalMemorySizeInKB - 1;
  } else {
    SmbiosRecord->EndingAddress = (UINT32) (TotalMemorySizeInKB - 1);
  }

  //
  // Memory Array Handle will be the 3rd optional string following the formatted structure.
  //
  GetType16Handle(Smbios, &Type16Handle);
  SmbiosRecord->MemoryArrayHandle = Type16Handle;
  SmbiosRecord->PartitionWidth = MAX_SOCKETS;

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  // Generate Memory Array Mapped Address info (Type 19)
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

