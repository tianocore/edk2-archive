/** @file
  This library will determine memory configuration information from the chipset
  and memory and create SMBIOS memory structures appropriately.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmbiosMemory.h"

//
// Memory Array Mapped Address (Type 19) data
//
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE19 SmbiosTableType19Data = {
  { EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS, sizeof (SMBIOS_TABLE_TYPE19), 0 },
  0,            ///< StartingAddress
  TO_BE_FILLED, ///< EndingAddress
  TO_BE_FILLED, ///< MemoryArrayHandle
  TO_BE_FILLED, ///< PartitionWidth
  0,            ///< ExtendedStartingAddress
  0,            ///< ExtendedEndingAddress
};


/**
  This function installs SMBIOS Table Type 19 (Physical Memory Array).

  @retval  EFI_SUCCESS            If the data is successfully reported.
  @retval  EFI_OUT_OF_RESOURCES   If not able to get resources.

**/
EFI_STATUS
InstallSmbiosType19 (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT64                          TotalMemorySizeInKB;
  UINT8                           Dimm;
  BOOLEAN                         SlotPresent;
  EFI_SMBIOS_HANDLE               SmbiosHandle;

  TotalMemorySizeInKB = 0;

  //
  // Get Memory size parameters for each rank from the chipset registers
  //
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &mMemInfoHob);

  //
  // Calculate the TotalMemorySizeInKB by adding the size of all populated sockets
  //
  for (Dimm = 0; Dimm < MAX_SOCKETS; Dimm++) {
    //
    // Use channel slot map to check whether the Socket is supported in this SKU, some SKU only has 2 Sockets totally
    //
    SlotPresent = FALSE;
    if (mMemInfoHob->MemInfoData.DimmPresent[Dimm]) {
      SlotPresent = TRUE;
    }
    if (SlotPresent) {
      TotalMemorySizeInKB += LShiftU64 (mMemInfoHob->MemInfoData.dimmSize[Dimm], 10);
    }
  }

  if (TotalMemorySizeInKB > SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES) {
    SmbiosTableType19Data.StartingAddress = SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES;
    SmbiosTableType19Data.EndingAddress = SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES;
    SmbiosTableType19Data.ExtendedEndingAddress = TotalMemorySizeInKB - 1;
  } else {
    SmbiosTableType19Data.EndingAddress = (UINT32) (TotalMemorySizeInKB - 1);
  }
  SmbiosTableType19Data.MemoryArrayHandle = mSmbiosType16Handle;
  SmbiosTableType19Data.PartitionWidth = MAX_SOCKETS;

  //
  // Generate Memory Array Mapped Address info (TYPE 19)
  //
  Status = AddSmbiosEntry ((EFI_SMBIOS_TABLE_HEADER *) &SmbiosTableType19Data, NULL, 0, &SmbiosHandle);
  DEBUG ((DEBUG_INFO, "\nInstall SMBIOS Table Type 19"));

  return Status;
}

