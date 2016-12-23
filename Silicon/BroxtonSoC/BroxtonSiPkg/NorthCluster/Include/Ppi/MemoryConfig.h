/** @file
  Policy definition of Memory Config Block.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MEMORY_CONFIG_H_
#define _MEMORY_CONFIG_H_

#pragma pack(1)

#define MEMORY_CONFIG_REVISION 1

extern EFI_GUID gMemoryConfigGuid;

typedef struct {
  CONFIG_BLOCK_HEADER  Header;    ///< Offset 0-23 Config Block Header
  UINT8                EccSupport;
  UINT16               DdrFreqLimit;
  UINT8                MaxTolud;
} MEMORY_CONFIGURATION;

#pragma pack()

#endif // _MEMORY_CONFIG_H_

