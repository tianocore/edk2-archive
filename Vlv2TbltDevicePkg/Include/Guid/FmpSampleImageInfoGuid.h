/** @file
  FMP Sample updatable resource guid

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#ifndef _FMP_SAMPLE_UPDATE_IMAGE_INFO_GUID_H_
#define _FMP_SAMPLE_UPDATE_IMAGE_INFO_GUID_H_

//
// GUID definition for FMP sample ImageInfo 
//
#define FMP_SAMPLE_UPDATE_IMAGE_INFO_GUID \
  { \
    0xb9847c4e, 0xf5b6, 0x42dc, { 0xb6, 0xf4, 0xed, 0x44, 0x7, 0xb0, 0x67, 0x4c } \
  }

extern EFI_GUID gFMPSampleUpdateImageInfoGuid;
#endif
