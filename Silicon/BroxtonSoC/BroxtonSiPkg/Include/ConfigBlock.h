/** @file
  Header file for Config Block Lib implementation.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CONFIG_BLOCK_H_
#define _CONFIG_BLOCK_H_

#include <Uefi/UefiBaseType.h>

///
/// Config Block Header
///
typedef struct _CONFIG_BLOCK_HEADER {
  EFI_GUID   Guid;                                ///< Offset 0-15  GUID of the config block
/** Offset 16-19
    Total length of this config block structure or size of config block table
    if used in table header
**/
  UINT32     Size;
  UINT8      Revision;                            ///< Offset 20    The main revision for config block
  UINT8      Reserved[3];                         ///< Offset 21-23 Reserved for future use
} CONFIG_BLOCK_HEADER;

///
/// Config Block
///
typedef struct _CONFIG_BLOCK {
  CONFIG_BLOCK_HEADER            Header;          ///< Offset 0-23  Header of config block
  //
  // Config Block Data
  //
} CONFIG_BLOCK;

///
/// Config Block Table Header
///
typedef struct _CONFIG_BLOCK_TABLE_STRUCT {
  CONFIG_BLOCK_HEADER            Header;          ///< Offset 0-23  GUID number for main entry of config block
  UINT16                         NumberOfBlocks;  ///< Offset 24-25 Number of config blocks (N)
  UINT16                         AvailableBlocks; ///< Offset 26-27 Current number of available config blocks
  UINT32                         AvailableSize;   ///< Offset 28-31 Current config block table size
///
/// Individual Config Block Structures are added here in memory as part of AddConfigBlock()
///
} CONFIG_BLOCK_TABLE_HEADER;

#endif // _CONFIG_BLOCK_H_

