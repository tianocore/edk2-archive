/** @file
  Multiplatform initialization header file.
  This file includes package header files, library classes.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _AURORA_TYPEC_LIB_H_
#define _AURORA_TYPEC_LIB_H_

#include <PiDxe.h>
#include <FrameworkPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/I2CLib.h>

//
// Parade Tech PS8750 TypeC MUX
//
#define PARADE_MUX_I2C_BUS      0x01
#define A_GENERAL               0x08
#define   R_FIRMWARE_VERSION      0x90
#define A_STATUS                0x09
#define   R_DP_AUX_SNOOP_BW       0x10
#define   R_DP_AUX_SNOOP_LC       0x11
#define   R_DP_AUX_SNOOP_L0       0x12
#define   R_DP_AUX_SNOOP_L1       0x13
#define   R_DP_AUX_SNOOP_L2       0x14
#define   R_DP_AUX_SNOOP_L3       0x15
#define   R_DP_AUX_SNOOP_D3       0x1E
#define   R_MUX_STATUS            0x80
#define   R_MUX_DP_TRAINING       0x83
#define   R_MUX_DP_AUX_INTERCEPT  0x85
#define   R_MUX_DP_EQ_CONFIG      0x86
#define   R_MUX_DP_OUTPUT_CONFIG  0x87
#define   R_MUX_HPD_ASSERT        0xBC
#define   R_CC_STATUS_1           0xEC
#define   R_CC_STATUS_2           0xED
#define   R_CC_STATUS_3           0xEE
#define MUX_TABLE_NULL            0xFFFF
#define MUX_RETRY_COUNT           0x03
#define MUX_TABLE_STRING_LENGTH   32

typedef struct {
  UINT8         Address;
  UINT8         Register;
  UINT16        Data;
  CHAR8         String[MUX_TABLE_STRING_LENGTH];
} MUX_PROGRAMMING_TABLE;

typedef struct {
  //
  // These UINT8 elements need to match the MUX_PROGRAMMING_TABLE list so we can use the Index to reference them
  //
  UINT8         FirmwareVersion;  // Offset 0
  UINT8         CcStatus1;        // Offset 1
  UINT8         CcStatus2;        // Offset 2
  UINT8         CcStatus3;        // Offset 3
  UINT8         MuxStatus;        // Offset 4
  UINT8         HpdAssert;        // Offset 5
  UINT8         DpTraining;       // Offset 6
  UINT8         DpAuxIntercept;   // Offset 7
  UINT8         DpEqConfig;       // Offset 8
  UINT8         DpOutputConfig;   // Offset 9
} MUX_DATA_TABLE;

#endif

