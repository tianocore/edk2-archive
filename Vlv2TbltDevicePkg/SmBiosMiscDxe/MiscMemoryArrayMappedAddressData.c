/** @file
  Static data of Physical Memory Array Mapped Address. SMBIOS Type 19.

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

//
// Static (possibly build generated) Physical Memory Array Mapped Address Data.
//
MISC_SMBIOS_TABLE_DATA(EFI_MEMORY_ARRAY_START_ADDRESS_DATA, MiscMemoryArrayMappedAddress) =
{
    0,                                                      //< StartingAddress
    0,                                                      //< EndingAddress
    0,                                                      //< MemoryArrayHandle
    0x00                                                    //< Partition Width
};

