/** @file
  The header file of CSRT.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#define EFI_ACPI_CSRT_TABLE_REVISION 0x00000000
#define NUMBER_RESOURCE_GROUP_INFO 1 //2
#define MAX_NO_CHANNEL1_SUPPORTED 7
#define MAX_NO_CHANNEL2_SUPPORTED 9
#define NAMESPACE_STRING_MAX_LENGTH 16

//
// Ensure proper structure formats
//
#pragma pack (1)


typedef struct _SHARED_INFO_SECTION {
  UINT16 MajVersion;
  UINT16 MinVersion;
  UINT32 MMIOLowPart;
  UINT32 MMIOHighPart;
  UINT32 IntGSI;
  UINT8  IntPol;
  UINT8  IntMode;
  UINT8  NoOfCh;
  UINT8  DMAAddressWidth;
  UINT16 BaseReqLine;
  UINT16 NoOfHandSig;
  UINT32 MaxBlockTransferSize;
} SHARED_INFO_SECTION;

typedef struct _RESOURCE_GROUP_HEADER {
  UINT32               Length;
  UINT32               VendorId;
  UINT32               SubVendorId;
  UINT16               DeviceId;
  UINT16               SubDeviceId;
  UINT16               Revision;
  UINT16               Reserved;
  UINT32               SharedInfoLength;
  SHARED_INFO_SECTION  SharedInfoSection;
} RESOURCE_GROUP_HEADER;

typedef struct _RESOURCE_DESCRIPTOR {
  UINT32 Length;
  UINT16 ResourceType;
  UINT16 ResourceSubType;
  UINT32 UUID;
} RESOURCE_DESCRIPTOR;

typedef struct {
  RESOURCE_GROUP_HEADER          ResourceGroupHeaderInfo;
  RESOURCE_DESCRIPTOR            ResourceDescriptorInfo[MAX_NO_CHANNEL1_SUPPORTED];
} RESOURCE_GROUP_INFO1;

typedef struct {
  RESOURCE_GROUP_HEADER          ResourceGroupHeaderInfo;
  RESOURCE_DESCRIPTOR            ResourceDescriptorInfo[MAX_NO_CHANNEL2_SUPPORTED];
} RESOURCE_GROUP_INFO2;

//
// DBGP structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  RESOURCE_GROUP_INFO1           ResourceGroupsInfo1;
  RESOURCE_GROUP_INFO2           ResourceGroupsInfo2;
} EFI_ACPI_CSRT_TABLE;

#pragma pack ()

