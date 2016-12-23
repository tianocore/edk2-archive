/** @file
  Header file for SMBIOS related functions.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_SMBIOS_PROCESSOR_LIB_H_
#define _DXE_SMBIOS_PROCESSOR_LIB_H_

//
// Non-static SMBIOS table data to be filled later with a dynamically generated value
//
#define TO_BE_FILLED  0
#define TO_BE_FILLED_STRING  " "        ///< Initial value should not be NULL

//
// String references in SMBIOS tables. This eliminates the need for pointers. See spec for details.
//
#define NO_STRING_AVAILABLE  0
#define STRING_1  1
#define STRING_2  2
#define STRING_3  3
#define STRING_4  4
#define STRING_5  5
#define STRING_6  6
#define STRING_7  7

#pragma pack(1)
typedef struct {
  CHAR8 *Socket;
  CHAR8 *ProcessorManufacture;
  CHAR8 *ProcessorVersion;
  CHAR8 *SerialNumber;
  CHAR8 *AssetTag;
  CHAR8 *PartNumber;
} SMBIOS_TYPE4_STRING_ARRAY;

#define SMBIOS_TYPE4_NUMBER_OF_STRINGS  6
#pragma pack()

#endif

