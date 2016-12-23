/** @file
  Core BIOS Messages.

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef CORE_BIOS_MSG_H
#define CORE_BIOS_MSG_H

#include <MkhiMsgs.h>

#pragma pack(1)

#define CBM_RESET_REQ_CMD         0x0B
#define CBM_RESET_REQ_CMD_ACK     0x8B
#define CBM_END_OF_POST_CMD       0x0C
#define CBM_END_OF_POST_CMD_ACK   0x8C
#define GEN_SET_DEBUG_MEMORY_CMD  0x11

#define FWU_QUERY_STATUS_CMD      0x02
#define FWU_QUERY_STATUS_GROUP_ID 0x06

#define HECI_CLIENT_CORE_MSG_DISPATCHER 0x07
#define HOST_FIXED_ADDRESS              0x00

typedef union _HECI_MESSAGE_HEADER {
  UINT32  Data;
  struct {
    UINT32  SeCAddress : 8;
    UINT32  HostAddress : 8;
    UINT32  Length : 9;
    UINT32  Reserved : 6;
    UINT32  MessageComplete : 1;
  } Fields;
} HECI_MESSAGE_HEADER;

//
// Reset request message
//
typedef struct _CBM_RESET_REQ_DATA {
  UINT8 RequestOrigin;
  UINT8 ResetType;
} CBM_RESET_REQ_DATA;

typedef struct _CBM_RESET_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
  CBM_RESET_REQ_DATA  Data;
} CBM_RESET_REQ;

typedef struct _MKHI_CBM_RESET_REQ {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_REQ       Msg;
} MKHI_CBM_RESET_REQ;

typedef struct _FWUPDATE_QUERY_UPDATE_STATUS_ACK_DATA {
  UINT32 PercentComplete;
  UINT32 CurrentStage;
  UINT32 TotalStages;
  UINT32 LastUpdateStatus;
} FWUPDATE_QUERY_UPDATE_STATUS_ACK_DATA;

typedef union _FWU_INFO_FLAGS {
  UINT32 Data;
  struct {
    UINT32 RecoveryMode : 2;
    UINT32 IpuNeeded : 1;
    UINT32 FwuInitDone : 1;
    UINT32 FwuInProgress : 1;
    UINT32 SuInProgress : 1;
    UINT32 NewFtTestS : 1;
    UINT32 SafeBootCnt : 4;
    UINT32 FsbFlag : 1;
    UINT32 LivePingNeeded : 1;
    UINT32 ResumeUpdateNeeded : 1;
    UINT32 RollbackNeededMode : 2;
    UINT32 ResetNeeded : 2;
    UINT32 SuState : 4;
    UINT32 Reserve: 10;
  } Fields;
} FWU_INFO_FLAGS;

typedef struct _MKHI_FWUPDATE_QUERY_STATUS_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
} MKHI_FWUPDATE_QUERY_SATUS_REQ;

typedef struct _MKHI_FWUPDATE_QUERY_STATUS_ACK {
  MKHI_MESSAGE_HEADER                   MKHIHeader;
  FWUPDATE_QUERY_UPDATE_STATUS_ACK_DATA QueryAckData;
  UINT32                                ResetType;
  FWU_INFO_FLAGS                        Flags;
}MKHI_FWUPDATE_QUERY_STATUS_ACK;

//
// Reset request ack message
//
typedef struct _CBM_RESET_ACK {
  MKHI_MESSAGE_HEADER MKHIHeader;
} CBM_RESET_ACK;

typedef struct _MKHI_CBM_RESET_ACK {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_ACK       Msg;
} MKHI_CBM_RESET_ACK;

//
// ASF Watch Dog Timer
//
#define ASF_MANAGEMENT_CONTROL    0x02
#define ASF_SUB_COMMAND_START_WDT 0x13
#define ASF_SUB_COMMAND_STOP_WDT  0x14

//
// ASF Start Watch Dog
//
typedef struct _ASF_START_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
  UINT8 TimeoutLow;
  UINT8 TimeoutHigh;
  UINT8 EventSensorType;
  UINT8 EventType;
  UINT8 EventOffset;
  UINT8 EventSourceType;
  UINT8 EventSeverity;
  UINT8 SensorDevice;
  UINT8 SensorNumber;
  UINT8 Entity;
  UINT8 EntityInstance;
  UINT8 EventData[5];
} ASF_START_WDT;

#define ASF_START_WDT_LENGTH  0x14

//
// ASF Stop Watch Dog
//
typedef struct _ASF_STOP_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
} ASF_STOP_WDT;

#define ASF_STOP_WDT_LENGTH 0x04

//
// HECI Header Definitions for Core BIOS Messages
//
#define CBM_END_OF_POST_HECI_HDR          0x80080001
#define CBM_END_OF_POST_RESPONSE_HECI_HDR 0x80010001
#define CBM_RESET_REQUEST_HECI_HDR        0x80030001
#define CBM_RESET_RESPONSE_HECI_HDR       0x80020001

//
// Enumerations used in Core BIOS Messages
//
// End Of Post Codes.
//
#define CBM_EOP_EXITING_G3  0x01
#define CBM_EOP_RESERVED    0x02
#define CBM_EOP_EXITING_S3  0x03
#define CBM_EOP_EXITING_S4  0x04
#define CBM_EOP_EXITING_S5  0x05

//
// Reset Request Origin Codes.
//
#define CBM_RR_REQ_ORIGIN_BIOS_MEMORY_INIT  0x01
#define CBM_RR_REQ_ORIGIN_BIOS_POST         0x02

//
// Reset Type Codes.
//
#define CBM_HRR_GLOBAL_RESET  0x01

//
// Reset Response Codes.
//
#define CBM_HRR_RES_REQ_NOT_ACCEPTED  0x01

//
// definitions for ICC MEI Messages
//
#define IBEX_PEAK_PLATFORM    0x00010000
#define COUGAR_POINT_PLATFORM 0x00020000

typedef enum {
  ICC_STATUS_SUCCESS                        = 0,
  ICC_STATUS_SUCCESS_WAITING_FOR_RESET,
  ICC_STATUS_INCORRECT_API_VERSION,
  ICC_STATUS_INVALID_FUNCTION,
  ICC_STATUS_INVALID_BUFFER_LENGTH,
  ICC_STATUS_INVALID_PARAMETERS,
  ICC_STATUS_FLASH_WEAR_OUT_VIOLATION,
  ICC_STATUS_CLOCK_REQ_ENTRY_VIOLATION,
  ICC_STATUS_STATIC_REGISTER_MASK_VIOLATION,
  ICC_STATUS_DYNAMIC_REGISTER_MASK_VIOLATION,
  ICC_STATUS_IMMEDIATE_REQUIRES_POWER_CYCLE,
  ICC_STATUS_ILLEGAL_RECORD_ID,
  ICC_STATUS_ENABLED_CLOCK_MASK_VIOLATION,
  ICC_STATUS_INVALID                        = 0xFFFFFFFF
} ICC_MEI_CMD_STATUS;

typedef union _ICC_CLOCK_ENABLES_CONTROL_MASK {
  UINT32  Dword;
  struct {
    UINT32  Flex0 : 1;
    UINT32  Flex1 : 1;
    UINT32  Flex2 : 1;
    UINT32  Flex3 : 1;
    UINT32  Reserved1 : 3;
    UINT32  PCI_Clock0 : 1;
    UINT32  PCI_Clock1 : 1;
    UINT32  PCI_Clock2 : 1;
    UINT32  PCI_Clock3 : 1;
    UINT32  PCI_Clock4 : 1;
    UINT32  Reserved2 : 4;
    UINT32  SRC0 : 1;
    UINT32  SRC1 : 1;
    UINT32  SRC2 : 1;
    UINT32  SRC3 : 1;
    UINT32  SRC4 : 1;
    UINT32  SRC5 : 1;
    UINT32  SRC6 : 1;
    UINT32  SRC7 : 1;
    UINT32  CSI_SRC8 : 1;
    UINT32  CSI_DP : 1;
    UINT32  PEG_A : 1;
    UINT32  PEG_B : 1;
    UINT32  DMI : 1;
    UINT32  Reserved3 : 3;
  } Fields;
} ICC_CLOCK_ENABLES_CONTROL_MASK;

typedef enum {
  LOCK_ICC_REGISTERS                        = 0x2,
  SET_CLOCK_ENABLES                         = 0x3,
  GET_ICC_PROFILE                           = 0x4,
  SET_ICC_PROFILE                           = 0x5,
  GET_ICC_CLOCKS_CAPABILITIES               = 0x6,
  GET_OEM_CLOCK_RANGE_DEFINITION_RECORD     = 0x7,
  GET_ICC_RECORD                            = 0x8,
  READ_ICC_REGISTER                         = 0x9,
  WRITE_ICC_REGISTER                        = 0xa,
  WRITE_UOB_RECORD                          = 0xb
} ICC_MEI_COMMAND_ID;

typedef struct {
  UINT32              ApiVersion;
  ICC_MEI_COMMAND_ID  IccCommand;
  ICC_MEI_CMD_STATUS  IccResponse;
  UINT32              BufferLength;
  UINT32              Reserved;
} ICC_HEADER;

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT8       SupportedProfilesNumber;
  UINT8       IccProfileSoftStrap;
  UINT8       IccProfileIndex;
  UINT8       Padding;
} ICC_GET_PROFILE_RESPONSE;

typedef union {
  ICC_GET_PROFILE_MESSAGE   message;
  ICC_GET_PROFILE_RESPONSE  response;
} ICC_GET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       ProfileBIOS;
  UINT8       PaddingA;
  UINT16      PaddingB;
} ICC_SET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_PROFILE_RESPONSE;

typedef union {
  ICC_SET_PROFILE_MESSAGE   message;
  ICC_SET_PROFILE_RESPONSE  response;
} ICC_SET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT32      ClockEnables;
  UINT32      ClockEnablesMask;
  UINT32      Params;
  UINT64      Nonce;
} ICC_SET_CLK_ENABLES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_CLK_ENABLES_RESPONSE;

typedef union {
  ICC_SET_CLK_ENABLES_MESSAGE   message;
  ICC_SET_CLK_ENABLES_RESPONSE  response;
} ICC_SET_CLK_ENABLES_BUFFER;

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_CLK_CAPABILITIES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT32      VersionNumber;
  UINT8       IccHwSku;
  UINT8       Reserved;
  UINT16      MaxSusramRecordSize;
  UINT64      IccSkuEnforcementTable;
  UINT32      IccBootStatusReport;
} ICC_GET_CLK_CAPABILITIES_RESPONSE;

typedef union {
  ICC_GET_CLK_CAPABILITIES_MESSAGE  message;
  ICC_GET_CLK_CAPABILITIES_RESPONSE response;
} ICC_GET_CLK_CAPABILITIES_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       AccessMode;
  UINT8       PaddingA;
  UINT16      PaddingB;
  UINT64      Nonce;
  UINT32      RegisterMask[3];
} ICC_LOCK_REGISTERS_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT64      Nonce;
  UINT32      RegisterMask[3];
} ICC_LOCK_REGISTERS_RESPONSE;

typedef union {
  ICC_LOCK_REGISTERS_MESSAGE  message;
  ICC_LOCK_REGISTERS_RESPONSE response;
} ICC_LOCK_REGISTERS_BUFFER;

typedef union _ICC_CLOCK_ENABLES_PARAMS {
  UINT32  Dword;
  struct {
    UINT32  RetainToResumeFromSx : 1;
    UINT32  Reserved : 31;
  } Fields;
} ICC_CLOCK_ENABLES_PARAMS;

//
// TR MEI Messages
//
typedef struct _TR_BIOS_PARAM_REQUEST {
  UINT8 Command;
  UINT8 PollingTimeout;
  UINT8 SMBusECMsgLen;
  UINT8 SMBusECMsgPEC;
  UINT8 DimmNumber;
} TR_BIOS_PARAM_REQUEST;

typedef struct _MKHI_MPC_BIOS_PARAM_REQUEST {
  HECI_MESSAGE_HEADER   Header;
  TR_BIOS_PARAM_REQUEST TrBiosParamRequest;
} MKHI_TR_BIOS_PARAM_REQUEST;

#define TR_HECI_CONFIG_MSG_CMD_ID 0

#define EC_MSG_LEN_1  1
#define EC_MSG_LEN_2  2
#define EC_MSG_LEN_5  5
#define EC_MSG_LEN_9  9
#define EC_MSG_LEN_10 10
#define EC_MSG_LEN_14 14
#define EC_MSG_LEN_20 20

typedef enum {
  TIME_100MS                                = 1,
  TIME_200MS,
  TIME_300MS,
  TIME_400MS
} POLLING_TIME_OUT;

typedef enum {
  PEC_DISABLED                              = 0,
  PEC_ENABLED
} PEC_SWITCH;

//
// SPI MEI Messages
//
#define HMRFPO_ENABLE_CMD_ID      0x01
#define HMRFPO_LOCK_CMD_ID        0x02
#define HMRFPO_GET_STATUS_CMD_ID  0x03

typedef enum {
  HMRFPO_ENABLE_SUCCESS                     = 0,
  HMRFPO_ENABLE_LOCKED,
  HMRFPO_NVAR_FAILURE,
  HMRFOP_ATP_POLICY,
  HMRFPO_ENABLE_UNKNOWN_FAILURE
} HMRFPO_ENABLE_STATUS;

typedef struct _MKHI_HMRFPO_ENABLE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce;
} MKHI_HMRFPO_ENABLE;

typedef struct _HMRFPO_ENABLE {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_ENABLE  Msg;
} HMRFPO_ENABLE;

typedef struct _MKHI_HMRFPO_ENABLE_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              FactoryDefaultBase;
  UINT32              FactoryDefaultLimit;
  UINT8               Status;
  UINT8               Rsvd[3];
} MKHI_HMRFPO_ENABLE_RESPONSE;

typedef struct _HMRFPO_ENABLE_RESPONSE {
  HECI_MESSAGE_HEADER         Header;
  MKHI_HMRFPO_ENABLE_RESPONSE Msg;
} HMRFPO_ENABLE_RESPONSE;

typedef enum {
  HMRFPO_LOCK_SUCCESS                       = 0,
  HMRFPO_LOCK_FAILURE
} HMRFPO_LOCK_STATUS;

typedef struct _MKHI_HMRFPO_LOCK {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_LOCK;

typedef struct _HMRFPO_LOCK {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_LOCK    Msg;
} HMRFPO_LOCK;

typedef struct _MKHI_HMRFPO_LOCK_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce;
  UINT32              FactoryDefaultBase;
  UINT32              FactoryDefaultLimit;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_LOCK_RESPONSE;

typedef struct _HMRFPO_LOCK_RESPONSE {
  HECI_MESSAGE_HEADER       Header;
  MKHI_HMRFPO_LOCK_RESPONSE Data;
} HMRFPO_LOCK_RESPONSE;

typedef struct _MKHI_HMRFPO_GET_STATUS {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_GET_STATUS;

typedef struct _HMRFPO_GET_STATUS {
  HECI_MESSAGE_HEADER     Header;
  MKHI_HMRFPO_GET_STATUS  Msg;
} HMRFPO_GET_STATUS;

typedef struct _MKHI_HMRFPO_GET_STATUS_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_GET_STATUS_RESPONSE;

typedef struct _HMRFPO_GET_STATUS_RESPONSE {
  HECI_MESSAGE_HEADER             Header;
  MKHI_HMRFPO_GET_STATUS_RESPONSE Data;
} HMRFPO_GET_STATUS_RESPONSE;

#define HMRFPO_LOCKED   1
#define HMRFPO_ENABLED  2

//
// ME State Control
//
#define EFI_SEC_STATE_STALL_1_SECOND 1000000
#define EFI_SEC_STATE_MAX_TIMEOUT    20000000
//
// KVM support
//
#define EFI_KVM_MESSAGE_COMMAND 0x08
#define EFI_KVM_BYTE_COUNT      0x06
#define EFI_KVM_QUERY_REQUES    0x01
#define EFI_KVM_QUERY_RESPONSE  0x02
#define EFI_KVM_VERSION         0x10

#define EFI_KVM_STALL_1_SECOND  1000000   // Stall 1 second
#define EFI_KVM_MAX_WAIT_TIME   (60 * 8)  // 8 Mins
typedef enum {
  QUERY_REQUEST                             = 0,
  CANCEL_REQUEST
} QUERY_TYPE;

typedef struct _AMT_QUERY_KVM_REQUEST {
  UINT8   Command;
  UINT8   ByteCount;
  UINT8   SubCommand;
  UINT8   VersionNumber;
  UINT32  QueryType;
} AMT_QUERY_KVM_REQUEST;

typedef enum {
  KVM_SESSION_ESTABLISHED                   = 1,
  KVM_SESSION_CANCELLED
} RESPONSE_CODE;

typedef struct _AMT_QUERY_KVM_RESPONSE {
  UINT8   Command;
  UINT8   ByteCount;
  UINT8   SubCommand;
  UINT8   VersionNumber;
  UINT32  ResponseCode;
} AMT_QUERY_KVM_RESPONSE;

typedef struct _SET_DEBUG_MEMORY_DATA {
  UINT32  BiosDebugMemoryAddress;
  UINT32  BiosDebugMemorySize;
  UINT32  SeCVeDebugMemoryAddress;
  UINT32  SeCVeDebugMemorySize;
} SET_DEBUG_MEMORY_DATA;

typedef struct _GEN_SET_DEBUG_MEMORY {
  MKHI_MESSAGE_HEADER   MKHIHeader;
  SET_DEBUG_MEMORY_DATA Data;
} GEN_SET_DEBUG_MEMORY;

typedef struct _GEN_SET_DEBUG_MEMORY_ACK {
  MKHI_MESSAGE_HEADER Header;
} GEN_SET_DEBUG_MEMORY_ACK;

#pragma pack()

#endif

