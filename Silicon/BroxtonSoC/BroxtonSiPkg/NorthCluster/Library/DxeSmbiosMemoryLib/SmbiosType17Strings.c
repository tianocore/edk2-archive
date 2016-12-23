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

GLOBAL_REMOVE_IF_UNREFERENCED CHAR8 *DimmToDevLocator[] = {
  "ChannelA-DIMM0",
  "ChannelA-DIMM1",
  "ChannelB-DIMM0",
  "ChannelB-DIMM1"
};

GLOBAL_REMOVE_IF_UNREFERENCED CHAR8 *DimmToBankLocator[] = {
  "BANK 0",
  "BANK 1",
  "BANK 2",
  "BANK 3"
};

GLOBAL_REMOVE_IF_UNREFERENCED MEMORY_MODULE_MANUFACTURE_LIST MemoryModuleManufactureList[] = {
  {0,    0x2c, "Micron"},
  {0,    0xad, "SK Hynix"},
  {0,    0xce, "Samsung"},
  {1,    0x4f, "Transcend"},
  {1,    0x98, "Kingston"},
  {2,    0xfe, "Elpida"},
  {0xff, 0xff, 0}
};

