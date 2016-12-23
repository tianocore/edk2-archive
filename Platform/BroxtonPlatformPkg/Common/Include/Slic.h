/** @file
  This file describes the contents of the ACPI SLIC Table.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SLIC_H
#define _SLIC_H

//
// Statements that include other files
//

#include <IndustryStandard/Acpi20.h>

#pragma pack (1)

#define EFI_ACPI_SLIC_TABLE_SIGNATURE           0x43494C53  //"SLIC"
#define EFI_ACPI_SLIC_TABLE_REVISION            0x1

typedef struct {
  UINT32              StructureType;
  UINT32              LicensingDataLength;
  UINT8               bType;
  UINT8               bVersion;
  UINT16              Reserved;
  UINT32              aiKeyAlg;
  UINT32              Magic;
  UINT32              BitLength;
  UINT32              PublicKeyExponent;
  UINT8               Modulus [128];
} EFI_ACPI_SLIC_OEM_PUBLIC_KEY;

typedef struct {
  UINT32              StructureType;
  UINT32              LicensingDataLength;
  UINT32              dwVersion;
  UINT8               sOEMID [6];
  UINT64              sOEMTABLEID;
  UINT8               sWindowsFlag [8];
  UINT32              Reserved0;
  UINT8               Reserved1 [8];
  UINT8               Reserved2 [8];
  UINT8               Signature [128];
} EFI_ACPI_SLIC_SLP_MARKER;

typedef struct {
  EFI_ACPI_SLIC_OEM_PUBLIC_KEY  PublicKeyFileBlob;
  UINT8                         Signature [128];
} EFI_ACPI_SLIC_SIGNED_OEM_PUBLIC_KEY;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER          Header;
  EFI_ACPI_SLIC_OEM_PUBLIC_KEY         OemPublicKey;
  EFI_ACPI_SLIC_SLP_MARKER             SlpMarker;
} EFI_ACPI_SLIC_TABLE;

#pragma pack ()

#endif

