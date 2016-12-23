/** @file
  Interface definition details between SEC and platform drivers during DXE phase.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_PLATFORM_POLICY_H_
#define _SEC_PLATFORM_POLICY_H_

//
// SEC policy provided by platform for DXE phase
//
#define DXE_PLATFORM_SEC_POLICY_GUID \
  { \
    0xf8bff014, 0x18fb, 0x4ef9, 0xb1, 0xc, 0xae, 0x22, 0x73, 0x8d, 0xbe, 0xed \
  }

#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION    1
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_2  2
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_3  3
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_4  4
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_5  5
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_6  6
#define DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_7  7

extern EFI_GUID gDxePlatformSeCPolicyGuid;

#pragma pack(1)

#define TR_CONFIG_EC_MSG_LEN_1  1
#define TR_CONFIG_EC_MSG_LEN_2  2
#define TR_CONFIG_EC_MSG_LEN_5  5
#define TR_CONFIG_EC_MSG_LEN_9  9
#define TR_CONFIG_EC_MSG_LEN_10 10
#define TR_CONFIG_EC_MSG_LEN_14 14
#define TR_CONFIG_EC_MSG_LEN_20 20

#define TR_CONFIG_PEC_DISABLED  0
#define TR_CONFIG_PEC_ENABLED   1

typedef struct {
  UINT8  TrEnabled;
  UINT8  SMBusECMsgLen;
  UINT8  SMBusECMsgPEC;
  UINT8  DimmNumber;
  UINT8  *SmbusAddress;
} TR_CONFIG;

typedef struct {
  UINT8   AtState;
  UINT8   AtLastTheftTrigger;
  UINT16  AtLockState;
  UINT16  AtAmPref;
} AT_CONFIG;

typedef struct {
  UINT16  CodeMinor;
  UINT16  CodeMajor;
  UINT16  CodeBuildNo;
  UINT16  CodeHotFix;
} SEC_VERSION;

typedef struct {
  ///
  /// Byte 0, bit definition for functionality enable/disable
  ///
  UINT8     SeCFwDownGrade : 1;        ///< 0: Disabled, 1: Enabled
  UINT8     SeCLocalFwUpdEnabled : 1;  ///< 0: Disabled, 1: Enabled
  UINT8     Reserved : 1;
  UINT8     Reserved1 : 1;
  UINT8     EndOfPostEnabled : 1; ///< 0: Disabled; 1: Enabled
  UINT8     Reserved2 : 3;
  ///
  /// Byte 1-3 Reserved for other bit definitions in future
  ///
  UINT8     ByteReserved1[3];

  UINT8     HeciCommunication;
  UINT8     PlatformBrand;
  UINT8     SeCFwImageType;
  ///
  /// Byte 7-15
  ///
  UINT32    FwCapsSku;
  UINT8     ByteReserved[5];

  ///
  /// Thermal Reporting Configuration to SEC
  ///
  TR_CONFIG *TrConfig;
  BOOLEAN   ITouchEnabled;
} SEC_CONFIG;

typedef enum {
  MSG_EOP_ERROR             = 0,
  MSG_SEC_FW_UPDATE_FAILED,
  MSG_ASF_BOOT_DISK_MISSING,
  MSG_KVM_TIMES_UP,
  MSG_KVM_REJECTED,
  MSG_HMRFPO_LOCK_FAILURE,
  MSG_HMRFPO_UNLOCK_FAILURE,
  MSG_SEC_FW_UPDATE_WAIT,
  MSG_ILLEGAL_CPU_PLUGGED_IN,
  MSG_KVM_WAIT,
  MAX_ERROR_ENUM
} SEC_ERROR_MSG_ID;

typedef
VOID
(EFIAPI *SEC_REPORT_ERROR) (
  SEC_ERROR_MSG_ID
  );

#pragma pack()

typedef struct _DXE_SEC_POLICY_PROTOCOL {
  UINT8            Revision;
  SEC_CONFIG       SeCConfig;
  SEC_VERSION      SeCVersion;
  AT_CONFIG        AtConfig;
  SEC_REPORT_ERROR SeCReportError;
} DXE_SEC_POLICY_PROTOCOL;

#endif

