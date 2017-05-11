/** @file
  Header file for the SMBIOS Memory library.
  This driver will determine memory configuration information from the chipset
  and memory and create SMBIOS memory structures appropriately.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMBIOS_MEMORY_H_
#define _SMBIOS_MEMORY_H_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBus.h>
#include <SaAccess.h>
#include <IndustryStandard/SmBios.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/MemInfo.h>

//
// Non-static SMBIOS table data to be filled later with a dynamically generated value
//
#define TO_BE_FILLED  0
#define TO_BE_FILLED_STRING  " "        //< Initial value should not be NULL

//
// String references in SMBIOS tables. This eliminates the need for pointers.
//
#define NO_STRING_AVAILABLE  0
#define STRING_1  1
#define STRING_2  2
#define STRING_3  3
#define STRING_4  4
#define STRING_5  5
#define STRING_6  6
#define STRING_7  7

//
// SMBIOS Table values with special meaning
//
#define SMBIOS_TYPE16_USE_EXTENDED_MAX_CAPACITY  0x80000000

#define SMBIOS_TYPE17_UNKNOWN_SIZE               0xFFFF
#define SMBIOS_TYPE17_USE_EXTENDED_SIZE          0x7FFF
#define SMBIOS_TYPE17_KB_BIT_MASK                0x8000
#define SMBIOS_TYPE17_NUMBER_OF_STRINGS               6

#define SMBIOS_TYPE19_USE_EXTENDED_ADDRESSES     0xFFFFFFFF

//
// Memory module type definition in DDR3 SPD Data
//
#define DDR_MTYPE_SPD_OFFSET  3     ///< Module type, offset 3, bits (3:0)
#define DDR_MTYPE_SPD_MASK    0x0F  ///< Module Type mask
#define DDR_MTYPE_RDIMM       0x01  ///< Registered DIMM Memory
#define DDR_MTYPE_UDIMM       0x02  ///< Unbuffered DIMM Memory
#define DDR_MTYPE_SODIMM      0x03  ///< Small Outline DIMM Memory
#define DDR_MTYPE_MICRO_DIMM  0x04  ///< Micro-DIMM Memory
#define DDR_MTYPE_MINI_RDIMM  0x05  ///< Mini Registered DIMM Memory
#define DDR_MTYPE_MINI_UDIMM  0x06  ///< Mini Unbuffered DIMM Memory

//
// Memory device type definition in DDR3 SPD Data
//
#define DDR_DTYPE_SPD_OFFSET  2     ///< Device type, offset 2, bits (7:0)
#define DDR_DTYPE_DDR3        11    ///< DDR3 memory type
#define DDR_DTYPE_DDR4        12    ///< DDR4 memory type
#define DDR_DTYPE_LPDDR3      0xF1  ///< LPDDR3 memory type

//
// Maximum rank memory size supported by the memory controller: 8GB (in terms of KB) for DDR4 and 4 GB for other types
//
#define MAX_RANK_CAPACITY       (4 * 1024 * 1024)
#define MAX_RANK_CAPACITY_DDR4  (8 * 1024 * 1024)

//
// DDR3 and DDR4 SPD Register Bytes
//
#define DDR4_SPD_BUFFER_SIZE              384
#define DDR4_SPD_MANUFACTURER_ID_LSB      320
#define DDR4_SPD_MANUFACTURER_ID_MSB      321
#define DDR4_SPD_PART_NO_START_BYTE       329
#define DDR4_SPD_PART_NO_END_BYTE         348
#define DDR4_SPD_SERIAL_NO_START_BYTE     325
#define DDR4_SPD_SERIAL_NO_END_BYTE       328
#define DDR4_SPD_MEMORY_BUS_WIDTH_BYTE    13

#define DDR3_SPD_BUFFER_SIZE              256
#define DDR3_SPD_MANUFACTURER_ID_LSB      117
#define DDR3_SPD_MANUFACTURER_ID_MSB      118
#define DDR3_SPD_PART_NO_START_BYTE       128
#define DDR3_SPD_PART_NO_END_BYTE         145
#define DDR3_SPD_SERIAL_NO_START_BYTE     122
#define DDR3_SPD_SERIAL_NO_END_BYTE       125
#define DDR3_SPD_MEMORY_BUS_WIDTH_BYTE    8

#define FREQ_800           0x00
#define FREQ_1066          0x01
#define FREQ_1333          0x02
#define FREQ_1600          0x03
#define FREQ_1866          0x04
#define FREQ_2133          0x05
#define FREQ_2666          0x07
#define FREQ_3200          0x08

enum {
  DDRType_DDR3 = 0,
  DDRType_DDR3L = 1,
  DDRType_DDR3U = 2,
  DDRType_DDR3All = 3,
  DDRType_LPDDR2 = 4,
  DDRType_LPDDR3 = 5,
  DDRType_DDR4 = 6
};

#ifndef MEMORY_ASSET_TAG
#define MEMORY_ASSET_TAG  "9876543210"
#endif
//
// Memory Module Manufacture ID List Structure
//
typedef struct {
  UINT8  Index;
  UINT8  ManufactureId;
  CHAR8  *ManufactureName;
} MEMORY_MODULE_MANUFACTURE_LIST;

#pragma pack(1)
typedef struct {
  CHAR8  *DeviceLocator;
  CHAR8  *BankLocator;
  CHAR8  *Manufacturer;
  CHAR8  *SerialNumber;
  CHAR8  *AssetTag;
  CHAR8  *PartNumber;
} SMBIOS_TYPE17_STRING_ARRAY;
#pragma pack()

//
// Module-wide global variables
//
extern MEM_INFO_PROTOCOL               *mMemInfoHob;
extern EFI_SMBIOS_PROTOCOL       *mSmbios;
extern EFI_SMBIOS_HANDLE      mSmbiosType16Handle;

extern CHAR8 *DimmToDevLocator[];
extern CHAR8 *DimmToBankLocator[];
extern MEMORY_MODULE_MANUFACTURE_LIST MemoryModuleManufactureList[];

//
// Prototypes
//
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
  @param[in]  SmbiosProtocol        Instance of Smbios Protocol
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
  );

/**
  This function installs SMBIOS Table Type 16 (Physical Memory Array).

  @retval EFI_SUCCESS            If the data is successfully reported.
  @retval EFI_OUT_OF_RESOURCES   If not able to get resources.

**/
EFI_STATUS
InstallSmbiosType16 (
  );

/**
  This function installs SMBIOS Table Type 17 (Memory Device).
  This function installs one table per memory device slot, whether populated or not.

  @retval  EFI_SUCCESS             If the data is successfully reported.
  @retval  EFI_OUT_OF_RESOURCES    If not able to get resources.
  @retval  EFI_INVALID_PARAMETER   If a required parameter in a subfunction is NULL.

**/
EFI_STATUS
InstallSmbiosType17 (
  );

/**
  This function installs SMBIOS Table Type 19 (Physical Memory Array).

  @retval  EFI_SUCCESS            If the data is successfully reported.
  @retval  EFI_OUT_OF_RESOURCES   If not able to get resources.

**/
EFI_STATUS
InstallSmbiosType19 (
  );

#endif

