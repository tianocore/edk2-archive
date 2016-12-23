/** @file
  Implementation file for Heci Message functionality.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>
#include <Ppi/HeciPpi.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>
#include <SeCAccess.h>
#include <IndustryStandard/Pci22.h>

/**
  Calculate if the circular buffer has overflowed.

  @param[in] ReadPointer       Location of the read pointer.
  @param[in] WritePointer      Location of the write pointer.

  @return    Number of filled slots.

**/
UINT8
FilledSlots (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer
  )
{
  UINT8    FilledSlots;

  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}


/**
  Calculate if the circular buffer has overflowed

  @param[in] ReadPointer             Value read from host/me read pointer.
  @param[in] WritePointer            Value read from host/me write pointer.
  @param[in] BufferDepth             Value read from buffer depth register.

  @retval    EFI_DEVICE_ERROR        The circular buffer has overflowed.
  @retval    EFI_SUCCESS             The circular buffer does not overflowed.

**/
EFI_STATUS
OverflowCB (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer,
  IN  UINT32 BufferDepth
  )
{
  UINT8     FilledSlots;

  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  //
  // test for overflow
  //
  if (FilledSlots > ((UINT8) BufferDepth)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Send Get Firmware SKU Request to SEC.

  @param[in, out] FwCapsSku                 SEC Firmware Capability SKU.

  @retval         EFI_UNSUPPORTED           Current SEC mode doesn't support this function.
  @retval         EFI_SUCCESS               Command succeeded.
  @retval         EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT               HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL      Message Buffer is too small for the Acknowledge.

**/
EFI_STATUS
PeiHeciGetFwCapsSkuMsg (
  IN OUT SECFWCAPS_SKU             *FwCapsSku
  )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPS_SKU_BUFFER      MsgGenGetFwCapsSku;
  UINT32                          Length;
  UINT32                          RecvLength;
  UINT32                          SeCMode;

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetFwCapsSku.Request.MKHIHeader.Data = 0;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.GroupId = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.Command = FWCAPS_GET_RULE_CMD;
  MsgGenGetFwCapsSku.Request.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetFwCapsSku.Request.Data.RuleId = 0;
  Length = sizeof (GEN_GET_FW_CAPSKU);
  RecvLength = sizeof (GEN_GET_FW_CAPS_SKU_ACK);

  //
  // Send Get FW SKU Request to SEC
  //
  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32 *) &MsgGenGetFwCapsSku,
             Length,
             &RecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  if (!EFI_ERROR (Status) && ((MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Command) == FWCAPS_GET_RULE_CMD) &&
    ((MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.IsResponse) == 1) &&
    (MsgGenGetFwCapsSku.Response.MKHIHeader.Fields.Result == 0)) {
    *FwCapsSku = MsgGenGetFwCapsSku.Response.Data.FWCapSku;
  }

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX. One of usages is to utilize
  this command to determine if the platform runs in Consumer or Corporate SKU
  size firmware.

  @param[out] RuleData                PlatformBrand,
                                      IntelMeFwImageType,
                                      SuperSku,
                                      PlatformTargetUsageType

  @retval     EFI_UNSUPPORTED         Current SEC mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge.

**/
EFI_STATUS
PeiHeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  GEN_GET_PLATFORM_TYPE_BUFFER    MsgGenGetPlatformType;
  UINT32                          SeCMode;

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR(Status) || (MeMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetPlatformType.Request.MKHIHeader.Data = 0;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.GroupId = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.Command = FWCAPS_GET_RULE_CMD;
  MsgGenGetPlatformType.Request.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetPlatformType.Request.Data.RuleId = 0x1D;
  Length = sizeof (GEN_GET_PLATFORM_TYPE);
  RecvLength = sizeof (GEN_GET_PLATFORM_TYPE_ACK);

  //
  // Send Get Platform Type Request to SEC
  //
  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32 *) &MsgGenGetPlatformType,
             Length,
             &RecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );
  if (!EFI_ERROR (Status)) {
    *RuleData = MsgGenGetPlatformType.Response.Data.RuleData;
  }

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX. To Get Firmware Version Request to SEC.

  @param[in,out] MsgGenGetFwVersionAck    Return themessage of FW version.

  @retval        EFI_UNSUPPORTED          Current SEC mode doesn't support this function.
  @retval        EFI_SUCCESS              Command succeeded.
  @retval        EFI_DEVICE_ERROR         HECI Device error, command aborts abnormally.
  @retval        EFI_TIMEOUT              HECI does not return the buffer before timeout.
  @retval        EFI_BUFFER_TOO_SMALL     Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
PeiHeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK     *MsgGenGetFwVersionAck
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  GEN_GET_FW_VER_ACK_BUFFER       *MsgGenGetFwVersion;
  UINT32                          SeCMode;

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR(Status) || (MeMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetFwVersion.Request.MKHIHeader.Data = 0;
  MsgGenGetFwVersion.Request.MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  MsgGenGetFwVersion.Request.MKHIHeader.Fields.Command = GEN_GET_FW_VERSION_CMD;
  MsgGenGetFwVersion.Request.MKHIHeader.Fields.IsResponse = 0;
  Length = sizeof (GEN_GET_FW_VER);
  RecvLength = sizeof (GEN_GET_FW_VER_ACK);

  //
  // Send Get Firmware Version Request to SEC
  //
  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32 *) &MsgGenGetFwVersion,
             Length,
             &RecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  if (!EFI_ERROR (Status)) {
    memcpy (MsgGenGetFwVersionAck, &(MsgGenGetFwVersion.Response), RecvLength);
  }

  return Status;
}

