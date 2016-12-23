/** @file
  Register Definitions for SeC States.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_STATE_H
#define _SEC_STATE_H

//
// Ignore SEC_FW_INIT_COMPLETE status Macro
//
#define SEC_STATUS_SEC_STATE_ONLY(a)  ((a) & (~(SEC_FW_INIT_COMPLETE | SEC_FW_BOOT_OPTIONS_PRESENT)))

//
// Macro to check if SEC FW INIT is completed
//
#define SEC_STATUS_IS_SEC_FW_INIT_COMPLETE(a) (((a) & SEC_FW_INIT_COMPLETE) == SEC_FW_INIT_COMPLETE)

//
// Marco to combind the complete bit to status
//
#define SEC_STATUS_WITH_SEC_INIT_COMPLETE(a)  ((a) | SEC_FW_INIT_COMPLETE)

//
// Macro to check SEC Boot Option Present
//
#define SEC_STATUS_IS_SEC_FW_BOOT_OPTIONS_PRESENT(a)  (((a) & SEC_FW_BOOT_OPTIONS_PRESENT) == SEC_FW_BOOT_OPTIONS_PRESENT)

//
// Abstract SEC Mode Definitions
//
#define SEC_MODE_NORMAL        0x00
#define SEC_DEBUG_MODE_ALT_DIS 0x02
#define SEC_MODE_TEMP_DISABLED 0x03
#define SEC_MODE_RECOVER       0x04
#define SEC_MODE_FAILED        0x06

//
// Abstract SEC Status definitions
//
#define SEC_READY                    0x00
#define SEC_INITIALIZING             0x01
#define SEC_IN_RECOVERY_MODE         0x02
#define SEC_DISABLE_WAIT             0x06
#define SEC_TRANSITION               0x07
#define SEC_NOT_READY                0x0F
#define SEC_FW_INIT_COMPLETE         0x80
#define SEC_FW_BOOT_OPTIONS_PRESENT  0x100
#define SEC_FW_UPDATES_IN_PROGRESS   0x200

#pragma pack()

#endif // SEC_STATE_H

