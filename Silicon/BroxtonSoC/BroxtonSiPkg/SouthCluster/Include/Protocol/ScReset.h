/** @file
  SC Reset Protocol.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_RESET_H_
#define _SC_RESET_H_

#define RESET_PROTOCOL_GUID \
  { \
    0xdb63592c, 0xb8cc, 0x44c8, 0x91, 0x8c, 0x51, 0xf5, 0x34, 0x59, 0x8a, 0x5a \
  }
#define RESET_CALLBACK_PROTOCOL_GUID \
  { \
    0x3a3300ab, 0xc929, 0x487d, 0xab, 0x34, 0x15, 0x9b, 0xc1, 0x35, 0x62, 0xc0 \
  }
extern EFI_GUID                             gScResetProtocolGuid;
extern EFI_GUID                             gScResetCallbackProtocolGuid;

///
/// Forward reference for ANSI C compatibility
///
typedef struct _SC_RESET_PROTOCOL          SC_RESET_PROTOCOL;
typedef struct _SC_RESET_CALLBACK_PROTOCOL SC_RESET_CALLBACK_PROTOCOL;

///
/// Related Definitions
///
///
/// SC Reset Types
///
typedef enum {
  ColdReset,
  WarmReset,
  ShutdownReset,
  PowerCycleReset,
  GlobalReset,
  GlobalResetWithEc
} SC_RESET_TYPE;

///
/// Member functions
///
/**
  Execute SC Reset from the host controller.

  @param[in] This                    Pointer to the SC_RESET_PROTOCOL instance.
  @param[in] ScResetType             SC Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                     PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval    EFI_SUCCESS             Successfully completed.
  @retval    EFI_INVALID_PARAMETER   If ResetType is invalid.

**/
typedef
EFI_STATUS
(EFIAPI *SC_RESET) (
  IN     SC_RESET_PROTOCOL       * This,
  IN     SC_RESET_TYPE           ScResetType
  );

/**
  Execute call back function for SC Reset.

  @param[in] ScResetType             SC Reset Types which includes PowerCycle, Globalreset.

  @retval    EFI_SUCCESS             The callback function has been done successfully
  @retval    EFI_NOT_FOUND           Failed to find SC Reset Callback protocol. Or, none of
                                     callback protocol is installed.
  @retval    Others                  Do not do any reset from SC.

**/
typedef
EFI_STATUS
(EFIAPI *SC_RESET_CALLBACK) (
  IN     SC_RESET_TYPE           ScResetType
  );

///
/// Interface structure for the SC Reset Protocol
///
struct _SC_RESET_PROTOCOL {
  SC_RESET Reset;
};

/**
  This protocol is used to execute South Cluster Reset from the host controller.
**/
struct _SC_RESET_CALLBACK_PROTOCOL {
  SC_RESET_CALLBACK  ResetCallback;
};

#endif

