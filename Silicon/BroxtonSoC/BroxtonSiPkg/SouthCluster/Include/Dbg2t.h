/** @file
  Required by Microsoft Windows to report the available debug ports on the platform.

  Copyright (c) 1996 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DBG2_H
#define _DBG2_H

#include <IndustryStandard/Acpi50.h>

//
// Definitions
//
#define EFI_ACPI_CREATOR_REVISION 0x0100000D

//
// DBG2 Definitions
//
#define EFI_ACPI_OEM_DBG2_TABLE_REVISION 0x00000000
#define NUMBER_DBG_DEVICE_INFO 1
#define DEBUG_DEVICE_INFORMATION_REVISION 0x00
#define NAMESPACE_STRING_MAX_LENGTH 32
#define EFI_ACPI_OEM_TABLE_ID_2 SIGNATURE_64('I','N','T','L','D','B','G','2') ///<Oem table Id for Dbg2

//
// DBG2 ACPI define
//
#pragma pack(1)

typedef struct _DEBUG_DEVICE_INFORMATION {
  UINT8  Revision;
  UINT16 Length;
  UINT8  NumberOfGenericAddressRegisters;
  UINT16 NameSpaceStringLength;
  UINT16 NameSpaceStringOffset;
  UINT16 OemDataLength;
  UINT16 OemDataOffset;
  UINT16 PortType;
  UINT16 PortSubtype;
  UINT16 Reserved;
  UINT16 BaseAddressRegisterOffset;
  UINT16 AddressSizeOffset;
  EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE BaseAddressRegister[1];
  UINT32 AddressSize[1];
  CHAR8  NamespaceString[NAMESPACE_STRING_MAX_LENGTH];
} DEBUG_DEVICE_INFORMATION;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER            Header;
  UINT32                                 OffsetDbgDeviceInfo;
  UINT32                                 NumberDbgDeviceInfo;
  DEBUG_DEVICE_INFORMATION               DbgDeviceInfoCom1;
} EFI_ACPI_DEBUG_PORT_2_TABLE;

#pragma pack()

#endif

