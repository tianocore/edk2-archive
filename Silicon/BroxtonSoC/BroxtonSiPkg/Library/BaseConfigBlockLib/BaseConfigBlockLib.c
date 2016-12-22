/** @file
  Library functions for Config Block management.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <ConfigBlock.h>
#include <Library/ConfigBlockLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>


/**
  Create config block table.

  @param[in]     NumberOfBlocks             Number of Config Blocks.

  @retval        Size                       Size of Config Block Table Header and Offset Table.

**/
UINT32
EFIAPI
GetSizeOfConfigBlockTableHeaders (
  IN     UINT16    NumberOfBlocks
  )
{
  return (UINT32) (sizeof (CONFIG_BLOCK_TABLE_HEADER)) + (UINT32) (NumberOfBlocks * 4);
}


/**
  Create config block table.

  @param[out]    ConfigBlockTableAddress      On return, points to a pointer to the beginning of Config Block Table Address.
  @param[in]     NumberOfBlocks               Number of Config Blocks.
  @param[in]     TotalSize                    Max size to be allocated for the Config Block Table.

  @retval        EFI_INVALID_PARAMETER        Invalid Parameter.
  @retval        EFI_OUT_OF_RESOURCES         Out of resources.
  @retval        EFI_SUCCESS                  Successfully created Config Block Table at ConfigBlockTableAddress.

**/
EFI_STATUS
EFIAPI
CreateConfigBlockTable (
  OUT    VOID      **ConfigBlockTableAddress,
  IN     UINT16    NumberOfBlocks,
  IN     UINT32    TotalSize
  )
{
  CONFIG_BLOCK_TABLE_HEADER  *ConfigBlkTblAddrPtr;
  UINT32                     ConfigBlkTblHdrSize;

  ConfigBlkTblHdrSize = (UINT32) (sizeof (CONFIG_BLOCK_TABLE_HEADER));

  if ((NumberOfBlocks == 0) || (TotalSize <= (ConfigBlkTblHdrSize + 4 + sizeof (CONFIG_BLOCK)))) {
    DEBUG ((DEBUG_ERROR, "Invalid Parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  ConfigBlkTblAddrPtr = (CONFIG_BLOCK_TABLE_HEADER *)AllocateZeroPool (TotalSize);
  if (ConfigBlkTblAddrPtr == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not allocate memory.\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ConfigBlkTblAddrPtr->NumberOfBlocks = NumberOfBlocks;
  ConfigBlkTblAddrPtr->Header.Size = TotalSize;
  ConfigBlkTblAddrPtr->AvailableBlocks = NumberOfBlocks;
  ConfigBlkTblAddrPtr->AvailableSize = TotalSize - GetSizeOfConfigBlockTableHeaders (NumberOfBlocks);

  *ConfigBlockTableAddress = (VOID *) ConfigBlkTblAddrPtr;

  return EFI_SUCCESS;
}


/**
  Add config block into config block table structure.

  @param[in]     ConfigBlockTableAddress     A pointer to the beginning of Config Block Table Address.
  @param[out]    ConfigBlockAddress          On return, points to a pointer to the beginning of Config Block Address.

  @retval        EFI_OUT_OF_RESOURCES        Config Block Table is full and cannot add new Config Block or
                                             Config Block Offset Table is full and cannot add new Config Block.
  @retval        EFI_SUCCESS                 Successfully added Config Block.

**/
EFI_STATUS
EFIAPI
AddConfigBlock (
  IN     VOID      *ConfigBlockTableAddress,
  OUT    VOID      **ConfigBlockAddress
  )
{
  CONFIG_BLOCK              *TempConfigBlk;
  CONFIG_BLOCK_TABLE_HEADER *ConfigBlkTblAddrPtr;
  CONFIG_BLOCK              *ConfigBlkAddrPtr;
  UINT16                    OffsetIndex;
  UINT32                    *OffsetTblPtr;
  UINT32                    *LastUsedOffsetPtr;
  UINT32                    LastUsedOffset;
  UINT32                    ConfigBlkTblHdrSize;
  UINT16                    NumOfBlocks;
  UINT32                    ConfigBlkSize;

  OffsetTblPtr = NULL;
  LastUsedOffsetPtr = NULL;
  ConfigBlkTblHdrSize = (UINT32) (sizeof (CONFIG_BLOCK_TABLE_HEADER));

  ConfigBlkTblAddrPtr = (CONFIG_BLOCK_TABLE_HEADER *) ConfigBlockTableAddress;
  ConfigBlkAddrPtr = (CONFIG_BLOCK *) (*ConfigBlockAddress);
  NumOfBlocks = ConfigBlkTblAddrPtr->NumberOfBlocks;
  ConfigBlkSize = ConfigBlkAddrPtr->Header.Size;

  if (ConfigBlkTblAddrPtr->AvailableSize < ConfigBlkSize) {
    DEBUG ((DEBUG_ERROR, "Config Block Table is full and cannot add new Config Block.\n"));
    DEBUG ((DEBUG_ERROR, "Available size: 0x%x bytes / Requested config Block Size: 0x%x bytes\n", ConfigBlkTblAddrPtr->AvailableSize, ConfigBlkSize));
    return EFI_OUT_OF_RESOURCES;
  } else if (ConfigBlkTblAddrPtr->AvailableBlocks == 0) {
    DEBUG ((DEBUG_ERROR, "Config Block Offset Table is full and cannot add new Config Block.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  OffsetIndex = NumOfBlocks - ConfigBlkTblAddrPtr->AvailableBlocks;
  OffsetTblPtr = (UINT32 *) ((UINTN) ConfigBlkTblAddrPtr + ConfigBlkTblHdrSize + (UINTN) ((OffsetIndex) * 4));
  if (OffsetIndex == 0) {
    LastUsedOffset = 0;
  } else {
    LastUsedOffsetPtr = (UINT32 *) ((UINTN) ConfigBlkTblAddrPtr + ConfigBlkTblHdrSize + (UINTN) ((OffsetIndex - 1) * 4));
    LastUsedOffset = (UINT32) *LastUsedOffsetPtr;
  }
  *OffsetTblPtr = LastUsedOffset + ConfigBlkSize;

  ConfigBlkTblAddrPtr->AvailableBlocks--;
  ConfigBlkTblAddrPtr->AvailableSize = ConfigBlkTblAddrPtr->AvailableSize - ConfigBlkSize;

  TempConfigBlk = (CONFIG_BLOCK *) ((UINTN) ConfigBlkTblAddrPtr + ConfigBlkTblHdrSize + (UINTN) (NumOfBlocks * 4) + LastUsedOffset);
  TempConfigBlk->Header.Size = ConfigBlkSize;
  TempConfigBlk->Header.Revision = ConfigBlkAddrPtr->Header.Revision;
  TempConfigBlk->Header.Guid = ConfigBlkAddrPtr->Header.Guid;

  *ConfigBlockAddress = (VOID *) TempConfigBlk;
  DEBUG ((DEBUG_INFO, "ConfigBlock GUID: %g at address : 0x%x\n", &(TempConfigBlk->Header.Guid), (UINT32)(UINTN)*ConfigBlockAddress));
  DEBUG ((DEBUG_INFO, "AvailableBlocks: %d / AvailableSize: 0x%x bytes\n", ConfigBlkTblAddrPtr->AvailableBlocks, ConfigBlkTblAddrPtr->AvailableSize));

  return EFI_SUCCESS;
}


/**
  Retrieve a specific Config Block data by GUID.

  @param[in]    ConfigBlockTableAddress     A pointer to the beginning of Config Block Table Address.
  @param[in]    ConfigBlockGuid             A pointer to the GUID uses to search specific Config Block.
  @param[out]   ConfigBlockAddress          On return, points to a pointer to the beginning of Config Block Address.

  @retval       EFI_NOT_FOUND               Could not find the Config Block.
  @retval       EFI_SUCCESS                 Config Block found and return.

**/
EFI_STATUS
EFIAPI
GetConfigBlock (
  IN     VOID      *ConfigBlockTableAddress,
  IN     EFI_GUID  *ConfigBlockGuid,
  OUT    VOID      **ConfigBlockAddress
  )
{
  UINT16                     OffsetIndex;
  CONFIG_BLOCK               *TempConfigBlk;
  UINT32                     *OffsetTblPtr;
  CONFIG_BLOCK_TABLE_HEADER  *ConfigBlkTblAddrPtr;
  UINT32                     ConfigBlkTblHdrSize;
  UINT32                     ConfigBlkOffset;
  UINT16                     NumOfBlocks;

  OffsetTblPtr = NULL;
  ConfigBlkTblHdrSize = (UINT32) (sizeof (CONFIG_BLOCK_TABLE_HEADER));
  ConfigBlkTblAddrPtr = (CONFIG_BLOCK_TABLE_HEADER *) ConfigBlockTableAddress;
  NumOfBlocks = ConfigBlkTblAddrPtr->NumberOfBlocks;

  ConfigBlkOffset = 0;
  for (OffsetIndex = 0; OffsetIndex < NumOfBlocks; OffsetIndex++) {
    TempConfigBlk = (CONFIG_BLOCK *) ((UINTN) ConfigBlkTblAddrPtr + (UINTN) ConfigBlkTblHdrSize + (UINTN) (NumOfBlocks * 4) + (UINTN) ConfigBlkOffset);
    if (CompareGuid (&(TempConfigBlk->Header.Guid), ConfigBlockGuid)) {
      *ConfigBlockAddress = (VOID *) TempConfigBlk;
      return EFI_SUCCESS;
    }
    OffsetTblPtr = (UINT32 *) ((UINTN) ConfigBlkTblAddrPtr + ConfigBlkTblHdrSize + (UINTN) (OffsetIndex * 4));
    ConfigBlkOffset = *OffsetTblPtr;
  }
  DEBUG ((DEBUG_ERROR, "Could not find the config block.\n"));
  return EFI_NOT_FOUND;
}

