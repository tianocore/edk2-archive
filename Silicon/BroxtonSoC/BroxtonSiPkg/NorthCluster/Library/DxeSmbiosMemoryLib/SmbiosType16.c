/** @file
  This library will determine memory configuration information from the chipset
  and memory and create SMBIOS memory structures appropriately.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmbiosMemory.h"

MEM_INFO_PROTOCOL      *mMemInfoHob;
EFI_SMBIOS_HANDLE      mSmbiosType16Handle;

//
// Physical Memory Array (Type 16) data
//
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE16 SmbiosTableType16Data = {
  { EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY, sizeof (SMBIOS_TABLE_TYPE16), 0 },
  MemoryArrayLocationSystemBoard, ///< Location
  MemoryArrayUseSystemMemory,     ///< Use
  TO_BE_FILLED,                   ///< MemoryErrorCorrection
  TO_BE_FILLED,                   ///< MaximumCapacity
  0xFFFE,                         ///< MemoryErrorInformationHandle
  TO_BE_FILLED,                   ///< NumberOfMemoryDevices
  0,                              ///< ExtendedMaximumCapacity
};


/**
  This function installs SMBIOS Table Type 16 (Physical Memory Array).

  @param[in] SmbiosProtocol        Instance of Smbios Protocol.

  @retval    EFI_SUCCESS           If the data is successfully reported.
  @retval    EFI_OUT_OF_RESOURCES  If not able to get resources.

**/
EFI_STATUS
InstallSmbiosType16 (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT8                           ChannelASlotMap;
  UINT8                           ChannelBSlotMap;
  UINT8                           BitIndex;
  UINT16                          MaxSockets;
  UINT8                           ChannelASlotNum;
  UINT8                           ChannelBSlotNum;

  //
  // Get Memory size parameters for each rank from the chipset registers
  //
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &mMemInfoHob);

  //
  // Configure the data for TYPE 16 SMBIOS Structure
  //
  //
  // Create physical array and associated data for all mainboard memory
  //
  SmbiosTableType16Data.MemoryErrorCorrection = MemoryErrorCorrectionNone;

  //
  // Get the Memory DIMM info from policy protocols
  //
  ChannelASlotMap = 0x01;
  ChannelBSlotMap = 0x01;
  ChannelASlotNum = 0;
  ChannelBSlotNum = 0;

  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ((ChannelASlotMap >> BitIndex) & BIT0) {
      ChannelASlotNum++;
    }

    if ((ChannelBSlotMap >> BitIndex) & BIT0) {
      ChannelBSlotNum++;
    }
  }
  MaxSockets = ChannelASlotNum + ChannelBSlotNum;
  if (mMemInfoHob->MemInfoData.ddrType == 0) {
    if ((MAX_RANK_CAPACITY_DDR4 * SA_MC_MAX_SIDES * MaxSockets) < SMBIOS_TYPE16_USE_EXTENDED_MAX_CAPACITY) {
      SmbiosTableType16Data.MaximumCapacity = MAX_RANK_CAPACITY_DDR4 * SA_MC_MAX_SIDES * MaxSockets;
    } else {
      SmbiosTableType16Data.MaximumCapacity = SMBIOS_TYPE16_USE_EXTENDED_MAX_CAPACITY;
      SmbiosTableType16Data.ExtendedMaximumCapacity = ((UINT64) MAX_RANK_CAPACITY_DDR4) * SA_MC_MAX_SIDES * MaxSockets * 1024; // Convert from KB to Byte
    }
  } else {
    if ((MAX_RANK_CAPACITY * SA_MC_MAX_SIDES * MaxSockets) < SMBIOS_TYPE16_USE_EXTENDED_MAX_CAPACITY) {
      SmbiosTableType16Data.MaximumCapacity = MAX_RANK_CAPACITY * SA_MC_MAX_SIDES * MaxSockets;
    } else {
      SmbiosTableType16Data.MaximumCapacity = SMBIOS_TYPE16_USE_EXTENDED_MAX_CAPACITY;
      SmbiosTableType16Data.ExtendedMaximumCapacity = ((UINT64) MAX_RANK_CAPACITY) * SA_MC_MAX_SIDES * MaxSockets * 1024; // Convert from KB to Byte
    }
  }
  SmbiosTableType16Data.NumberOfMemoryDevices = MaxSockets;

  //
  // Install SMBIOS Table Type 16
  //
  Status = AddSmbiosEntry ((EFI_SMBIOS_TABLE_HEADER *) &SmbiosTableType16Data, NULL, 0, &mSmbiosType16Handle);
  DEBUG ((DEBUG_INFO, "\nInstall SMBIOS Table Type 16"));

  return Status;
}

