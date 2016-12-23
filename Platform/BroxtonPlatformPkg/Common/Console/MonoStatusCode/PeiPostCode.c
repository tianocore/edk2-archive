/** @file
  Worker functions for Pei PostCode.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformStatusCodesInternal.h"

STATUS_CODE_TO_DATA_MAP mPostCodeProgressMap[] = {
  //
  // PEI
  //
  //Regular boot
  //
  { PEI_CORE_STARTED, 0x10 },
  { PEI_CAR_CPU_INIT, 0x11 },
  //
  // reserved for CPU 0x12 - 0x14
  //
  { PEI_CAR_NB_INIT, 0x15 },
  //
  // reserved for NB 0x16 - 0x18
  //
  { PEI_CAR_SB_INIT, 0x19 },
  //
  // reserved for SB 0x1A - 0x1C
  //
  { PEI_MEMORY_SPD_READ, 0x1D },
  { PEI_MEMORY_PRESENCE_DETECT, 0x1E },
  { PEI_MEMORY_TIMING, 0x1F},
  { PEI_MEMORY_CONFIGURING, 0x20 },
  { PEI_MEMORY_INIT, 0x21 },
  //
  // reserved for OEM use: 0x22 - 0x2F
  // reserved for AML use: 0x30
  //
  { PEI_MEMORY_INSTALLED, 0x31 },
  { PEI_CPU_INIT,  0x32 },
  { PEI_CPU_CACHE_INIT, 0x33 },
  { PEI_CPU_BSP_SELECT, 0x34 },
  { PEI_CPU_AP_INIT, 0x35 },
  { PEI_CPU_SMM_INIT, 0x36 },
  { PEI_MEM_NB_INIT, 0x37 },
  //
  // reserved for NB 0x38 - 0x3A
  //
  { PEI_MEM_SB_INIT, 0x3B },
  //
  // reserved for SB 0x3C - 0x3E
  // reserved for OEM use: 0x3F - 0x4E
  //
  { PEI_DXE_IPL_STARTED, 0x4F },
  //
  //Recovery
  //
  { PEI_RECOVERY_AUTO, 0xF0 },
  { PEI_RECOVERY_USER, 0xF1 },
  { PEI_RECOVERY_STARTED, 0xF2 },
  { PEI_RECOVERY_CAPSULE_FOUND, 0xF3 },
  { PEI_RECOVERY_CAPSULE_LOADED, 0xF4 },
  //
  //S3
  //
  { PEI_S3_BOOT_SCRIPT, 0xE1 },
  { PEI_S3_OS_WAKE, 0xE3 },

  {0,0}
};

STATUS_CODE_TO_DATA_MAP mPostCodeErrorMap[] = {
  //
  // PEI
  //
  //Regular boot
  //
  { PEI_MEMORY_INVALID_TYPE, 0x50 },
  { PEI_MEMORY_INVALID_SPEED, 0x50 },
  { PEI_MEMORY_SPD_FAIL, 0x51 },
  { PEI_MEMORY_INVALID_SIZE, 0x52 },
  { PEI_MEMORY_MISMATCH, 0x52 },
  { PEI_MEMORY_NOT_DETECTED, 0x53 },
  { PEI_MEMORY_NONE_USEFUL, 0x53 },
  { PEI_MEMORY_ERROR, 0x54 },
  { PEI_MEMORY_NOT_INSTALLED, 0x55 },
  { PEI_CPU_INVALID_TYPE, 0x56 },
  { PEI_CPU_INVALID_SPEED, 0x56 },
  { PEI_CPU_MISMATCH, 0x57 },
  { PEI_CPU_SELF_TEST_FAILED, 0x58 },
  { PEI_CPU_CACHE_ERROR, 0x58 },
  { PEI_CPU_MICROCODE_UPDATE_FAILED, 0x59 },
  { PEI_CPU_NO_MICROCODE, 0x59 },
  { PEI_CPU_INTERNAL_ERROR, 0x5A },
  { PEI_CPU_ERROR, 0x5A },
  { PEI_RESET_NOT_AVAILABLE,0x5B },
  //
  //reserved for IBV use: 0x5C - 0x5F
  //Recovery
  //
  { PEI_RECOVERY_PPI_NOT_FOUND, 0xF8 },
  { PEI_RECOVERY_NO_CAPSULE, 0xF9 },
  { PEI_RECOVERY_INVALID_CAPSULE, 0xFA },
  //
  //reserved for IBV use: 0xFB - 0xFF
  //S3 Resume
  //
  { PEI_MEMORY_S3_RESUME_FAILED, 0xE8 },
  { PEI_S3_RESUME_PPI_NOT_FOUND, 0xE9 },
  { PEI_S3_BOOT_SCRIPT_ERROR, 0xEA },
  { PEI_S3_OS_WAKE_ERROR, 0xEB },
  //
  //reserved for IBV use: 0xEC - 0xEF
  //

  {0,0}
};

STATUS_CODE_TO_DATA_MAP *mPostCodeStatusCodesMap[] = {
  mPostCodeProgressMap,
  mPostCodeErrorMap
};

UINT32
FindPostCodeData (
  IN STATUS_CODE_TO_DATA_MAP *Map,
  IN EFI_STATUS_CODE_VALUE   Value
  )
{
  while (Map->Value != 0) {
    if (Map->Value == Value) {
      return Map->Data;
    }
    Map++;
  }
  return 0;
}


/**
  Get PostCode from status code type and value.

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or
                               software entity. This includes information about the class and
                               subclass that is used to classify the entity as well as an operation.
                               For progress codes, the operation is the current activity.
                               For error codes, it is the exception.For debug codes,it is not defined at this time.

  @return     PostCode

**/
UINT32
EFIAPI
GetPostCodeFromStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  )
{
  UINT32    CodeTypeIndex;

  CodeTypeIndex = STATUS_CODE_TYPE (CodeType) - 1;

  if (CodeTypeIndex >= sizeof (mPostCodeStatusCodesMap) / sizeof (mPostCodeStatusCodesMap[0])) {
    return 0;
  }

  return FindPostCodeData (mPostCodeStatusCodesMap[CodeTypeIndex], Value);
}

