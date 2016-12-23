/** @file
  Implementation file for SeC functionality.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <MkhiMsgs.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  Check if SeC is enabled

  @param[in] VOID                Parameter is VOID

  @retval   EFI_SUCCESS          Command succeeded

**/
EFI_STATUS
SeCLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  return Status;
}


/**
  Send Get Firmware SKU Request to SEC.

  @param[in]  FwCapsSku                 Return Data from Get Firmware Capabilities MKHI Request.

  @retval     EFI_UNSUPPORTED           Current SEC mode doesn't support this function.
  @retval     EFI_SUCCESS               Command succeeded.
  @retval     EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT               HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL      Message Buffer is too small for the Acknowledge.

**/
EFI_STATUS
HeciGetFwCapsSku (
  IN SECFWCAPS_SKU       *FwCapsSku
  )
{
  EFI_STATUS               Status;
  GEN_GET_FW_CAPSKU        MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK  MsgGenGetFwCapsSkuAck;

  Status = HeciGetFwCapsSkuMsg (&MsgGenGetFwCapsSku, &MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Command) == FWCAPS_GET_RULE_CMD) &&
      ((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.IsResponse) == 1) &&
      (MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Result == 0)) {
    *FwCapsSku = MsgGenGetFwCapsSkuAck.Data.FWCapSku;
  }

  return EFI_SUCCESS;
}


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
  )
{
  EFI_STATUS  Status;

  Status = HeciGetPlatformTypeMsg (RuleData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Send Get Firmware Version Request to SEC.

  @param[in, out] MsgGenGetFwVersionAckData          Return themessage of FW version.

  @retval         EFI_UNSUPPORTED                    Current SEC mode doesn't support this function.
  @retval         EFI_SUCCESS                        Command succeeded.
  @retval         EFI_DEVICE_ERROR                   HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT                        HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL               Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetFwVersion (
  IN OUT GEN_GET_FW_VER_ACK_DATA      *MsgGenGetFwVersionAckData
  )
{
  EFI_STATUS          Status;
  GEN_GET_FW_VER_ACK  MsgGenGetFwVersionAck;

  Status = HeciGetFwVersionMsg (&MsgGenGetFwVersionAck);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((MsgGenGetFwVersionAck.MKHIHeader.Fields.Command == GEN_GET_FW_VERSION_CMD) &&
      (MsgGenGetFwVersionAck.MKHIHeader.Fields.IsResponse == 1) &&
      (MsgGenGetFwVersionAck.MKHIHeader.Fields.Result == 0)) {
    *MsgGenGetFwVersionAckData = MsgGenGetFwVersionAck.Data;
  }

  return EFI_SUCCESS;
}


/**
  Host client gets Firmware update info from SEC client.

  @param[in, out] SECCapability           Structure of FirmwareUpdateInfo.

  @retval         EFI_SUCCESS             Command succeeded.

**/
EFI_STATUS
HeciGetSeCFwInfo (
  IN OUT SEC_CAP *SECCapability
  )
{
  EFI_STATUS               Status;
  DXE_SEC_POLICY_PROTOCOL  *mDxePlatformSeCPolicy;
  SECFWCAPS_SKU            FwCapsSku;

  //
  // Get the SEC platform policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformSeCPolicyGuid, NULL, (VOID **) &mDxePlatformSeCPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No SEC Platform Policy Protocol available"));
    return EFI_UNSUPPORTED;
  }

  SECCapability->SeCEnabled = 1;

  FwCapsSku.Data          = mDxePlatformSeCPolicy->SeCConfig.FwCapsSku;
  if (FwCapsSku.Fields.IntelAT) {
    SECCapability->AtSupported = 1;
  }

  if (FwCapsSku.Fields.KVM) {
  }

  if (mDxePlatformSeCPolicy->SeCConfig.PlatformBrand == INTEL_AMT_BRAND) {
  }

  if (mDxePlatformSeCPolicy->SeCConfig.PlatformBrand == INTEL_STAND_MANAGEABILITY_BRAND) {
  }

  SECCapability->SeCMinorVer  = mDxePlatformSeCPolicy->SeCVersion.CodeMinor;
  SECCapability->SeCMajorVer  = mDxePlatformSeCPolicy->SeCVersion.CodeMajor;
  SECCapability->SeCBuildNo   = mDxePlatformSeCPolicy->SeCVersion.CodeBuildNo;
  SECCapability->SeCHotFixNo  = mDxePlatformSeCPolicy->SeCVersion.CodeHotFix;

  return Status;
}


/**
  Dummy return for SeC signal event use.

  @param[in] Event                  The event that triggered this notification function.
  @param[in] ParentImageHandle      Pointer to the notification functions context.

  @retval    EFI_SUCCESS            Always return EFI_SUCCESS.

**/
EFI_STATUS
SeCEmptyEvent (
  IN EFI_EVENT           Event,
  IN void                *ParentImageHandle
  )
{
  return EFI_SUCCESS;
}


/**
  Get AT State Information From Stored SEC platform policy.

  @param[in, out] AtState                            Pointer to AT State Information.
  @param[in, out] AtLastTheftTrigger                 Pointer to Variable holding the cause of last AT Stolen Stae.
  @param[in, out] AtLockState                        Pointer to variable indicating whether AT is locked or not.
  @param[in, out] AtAmPref                           Pointer to variable indicating whether TDTAM or PBA should be used.

  @retval         EFI_UNSUPPORTED                    Current SEC mode doesn't support this function.
  @retval         EFI_SUCCESS                        Command succeeded.
  @retval         EFI_DEVICE_ERROR                   HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT                        HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL               Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
GetAtStateInfo (
  IN OUT UINT8                 *AtState,
  IN OUT UINT8                 *AtLastTheftTrigger,
  IN OUT UINT16                *AtLockState,
  IN OUT UINT16                *AtAmPref
  )
{
  EFI_STATUS               Status;
  DXE_SEC_POLICY_PROTOCOL  *mDxePlatformSeCPolicy;

  //
  // Get the SEC platform policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformSeCPolicyGuid, NULL, (VOID **) &mDxePlatformSeCPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No SEC Platform Policy Protocol available"));
    return EFI_UNSUPPORTED;
  }

  *AtState            = mDxePlatformSeCPolicy->AtConfig.AtState;
  *AtLastTheftTrigger = mDxePlatformSeCPolicy->AtConfig.AtLastTheftTrigger;
  *AtLockState        = mDxePlatformSeCPolicy->AtConfig.AtLockState;
  *AtAmPref           = mDxePlatformSeCPolicy->AtConfig.AtAmPref;

  return EFI_SUCCESS;
}

