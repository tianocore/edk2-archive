/** @file

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BASE_PLATFORM_CMOS_LIB_H_
#define _BASE_PLATFORM_CMOS_LIB_H_

#define CMOS_ATTRIBUTE_EXCLUDE_FROM_CHECKSUM 0x1
#define CMOS_ATTRIBUTE_EXCLUDE_FROM_INIT_DATA 0x2

typedef struct {
  UINT8 CmosAddress;
  UINT8 DefaultValue;
  UINT8 Attribute;
} CMOS_ENTRY;

/**
  Funtion to return platform CMOS entry.

  @param[out]  CmosEntry        Platform CMOS entry.
  @param[out]  CmosEnryCount    Number of platform CMOS entry.

  @return Status.

**/
EFI_STATUS
EFIAPI
GetPlatformCmosEntry (
  OUT CMOS_ENTRY  **CmosEntry,
  OUT UINTN       *CmosEnryCount
  );

/**
  Funtion to check if Battery lost or CMOS cleared.

  @retval TRUE       Battery is always present.
  @retval FALSE      CMOS is cleared.

**/
BOOLEAN
EFIAPI
CheckCmosBatteryStatus (
  VOID
  );

#endif // _PLATFORM_CMOS_LIB_H_

