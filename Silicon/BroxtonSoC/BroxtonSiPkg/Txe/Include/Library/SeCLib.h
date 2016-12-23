/** @file
  Header file for SeC functionality.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_LIB_H_
#define _SEC_LIB_H_

#include "SeCPolicyLib.h"
#include "HeciMsgLib.h"

/**
  Check if SeC is enabled.

  @param[in] VOID                 Parameter is VOID

  @retval    EFI_SUCCESS          Command succeeded

**/
EFI_STATUS
SeCLibInit (
  VOID
  );

/**
  Host client gets Firmware update info from SEC client

  @param[in, out] SECCapability       Structure of FirmwareUpdateInfo

  @retval         EFI_SUCCESS         Command succeeded

**/
EFI_STATUS
HeciGetSeCFwInfo (
  IN OUT SEC_CAP *SECCapability
  );

/**
  Send Get Firmware SKU Request to SEC.

  @param[in] FwCapsSku                 Return Data from Get Firmware Capabilities MKHI Request.

  @retval    EFI_UNSUPPORTED           Current SEC mode doesn't support this function.
  @retval    EFI_SUCCESS               Command succeeded.
  @retval    EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally.
  @retval    EFI_TIMEOUT               HECI does not return the buffer before timeout.
  @retval    EFI_BUFFER_TOO_SMALL      Message Buffer is too small for the Acknowledge.

**/
EFI_STATUS
HeciGetFwCapsSku (
  IN SECFWCAPS_SKU       *FwCapsSku
  );

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  4M or 8M size firmware.

  @param[out] RuleData                PlatformBrand,
                                      IntelSeCFwImageType,
                                      SuperSku,
                                      PlatformTargetMarketType,
                                      PlatformTargetUsageType

  @retval     EFI_UNSUPPORTED         Current SEC mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge.

**/
EFI_STATUS
HeciGetPlatformType (
  OUT PLATFORM_TYPE_RULE_DATA   *RuleData
  );

/**
  Send Get Firmware Version Request to SEC.

  @param[in,out] MsgGenGetFwVersionAckData       Return themessage of FW version.

  @retval        EFI_UNSUPPORTED                 Current SEC mode doesn't support this function.
  @retval        EFI_SUCCESS                     Command succeeded.
  @retval        EFI_DEVICE_ERROR                HECI Device error, command aborts abnormally.
  @retval        EFI_TIMEOUT                     HECI does not return the buffer before timeout.
  @retval        EFI_BUFFER_TOO_SMALL            Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetFwVersion (
  IN OUT GEN_GET_FW_VER_ACK_DATA      *MsgGenGetFwVersionAckData
  );

/**
  Dummy return for SeC signal event use.

  @param[in] Event                  The event that triggered this notification function.
  @param[in] ParentImageHandle      Pointer to the notification functions context.

  @return    EFI_SUCCESS            Always return EFI_SUCCESS.

**/
EFI_STATUS
SeCEmptyEvent (
  IN EFI_EVENT           Event,
  IN void                *ParentImageHandle
  );

/**
  Get AT State Information From Stored SEC platform policy.

  @param[in, out] AtState                     Pointer to AT State Information.
  @param[in, out] AtLastTheftTrigger          Pointer to Variable holding the cause of last AT Stolen Stae.
  @param[in, out] AtLockState                 Pointer to variable indicating whether AT is locked or not.
  @param[in, out] AtAmPref                    Pointer to variable indicating whether TDTAM or PBA should be used.

  @retval         EFI_UNSUPPORTED             Current SEC mode doesn't support this function.
  @retval         EFI_SUCCESS                 Command succeeded.
  @retval         EFI_DEVICE_ERROR            HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT                 HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL        Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
GetAtStateInfo (
  IN OUT UINT8                  *AtState,
  IN OUT UINT8                  *AtLastTheftTrigger,
  IN OUT UINT16                 *AtLockState,
  IN OUT UINT16                 *AtAmPref
  );

#endif

