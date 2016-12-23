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
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/PerformanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Heci.h>
#include <Protocol/ScReset.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>
#include <SeCAccess.h>

/**
  Send Core BIOS Reset Request Message through HECI.

  @param[in] ResetOrigin             Reset source.
  @param[in] ResetType               Global or Host reset.

  @retval    EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval    EFI_SUCCESS             Command succeeded.
  @retval    EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval    EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciSendCbmResetRequest (
  IN  UINT8                             ResetOrigin,
  IN  UINT8                             ResetType
  )
{
  EFI_HECI_PROTOCOL         *Heci;
  EFI_STATUS                Status;
  UINT32                    HeciLength;
  CBM_RESET_REQ             CbmResetRequest;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CbmResetRequest.MKHIHeader.Data               = 0;
  CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
  CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
  CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
  CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
  CbmResetRequest.MKHIHeader.Fields.Result      = 0;
  CbmResetRequest.Data.RequestOrigin            = ResetOrigin;
  CbmResetRequest.Data.ResetType                = ResetType;

  HeciLength = sizeof (CBM_RESET_REQ);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &CbmResetRequest,
                   HeciLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send Reset Request - %r\n", Status));
  }

  return Status;
}


/**
  Called by each FWU request API when there's a flow control message expected on the link.

  @param[in] None

  @retval    EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval    EFI_SUCCESS             Command succeeded.
  @retval    EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval    EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciSecToHostFlowControl (
  )
{
  EFI_STATUS            Status;
  EFI_HECI_PROTOCOL     *Heci;
  HBM_FLOW_CONTROL_MSG  FlowCtrlMsg;
  UINT32                MsgLen = sizeof (HBM_FLOW_CONTROL_MSG);

  DEBUG ((EFI_D_INFO, "####Sec to Heci flow control####\n"));

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // First get flow control from SEC
  //
  ZeroMem (&FlowCtrlMsg, sizeof (HBM_FLOW_CONTROL_MSG));
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &FlowCtrlMsg,
                   &MsgLen
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "#####Fwuflow control: wait for SEC failed with status:%d.\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Called by each FWU request API when host needs to inform SEC it's ready to accept any input.

  @param[in]  SecAddress              The dynamic sec address the flow control.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciHostToSecFlowControl (
  IN UINT8 SecAddress
  )
{
  EFI_STATUS                  Status;
  EFI_HECI_PROTOCOL           *Heci;
  HBM_FLOW_CONTROL_MSG        FlowCtrlMsg;

  DEBUG ((EFI_D_INFO, "####Host to SeC flow control####\n"));

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Send flow control to SEC
  //
  ZeroMem (&FlowCtrlMsg, sizeof (HBM_FLOW_CONTROL_MSG));
  FlowCtrlMsg.Cmd = HBM_CMD_FLOW_CONTROL;
  FlowCtrlMsg.HostAddress = BIOS_FIXED_HOST_ADDR + 1;
  FlowCtrlMsg.SecAddress = SecAddress;
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &FlowCtrlMsg,
                   sizeof (HBM_FLOW_CONTROL_MSG),
                   BIOS_FIXED_HOST_ADDR,
                   HECI_HBM_MSG_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "#####Fwuflow control: Send to SEC failed with status:%d.\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Called by each FWU request API when both sec and host needs to inform each other  it's ready to accept any input.

  @param[in]  SecAddress              Returns the SecAddress to be used by other FWU APIs.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciBiDirectionFlowControl (
  IN   UINT8 SecAddress
  )
{
  EFI_STATUS                 Status;
  EFI_HECI_PROTOCOL          *Heci;
  HBM_FLOW_CONTROL_MSG       FlowCtrlMsg;
  UINT32                     MsgLen = sizeof (HBM_FLOW_CONTROL_MSG);

  DEBUG ((EFI_D_INFO, "####Bi-Directional flow control####\n"));

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // First get flow control from SEC
  //
  gBS->Stall (200);

  ZeroMem (&FlowCtrlMsg, sizeof (HBM_FLOW_CONTROL_MSG));
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32*)&FlowCtrlMsg,
                   &MsgLen
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "#####Fwuflow control: wait for SEC failed with status:%d.\n", Status));
    return Status;
  }

  //
  // Then send flow control to SEC
  //
  ZeroMem (&FlowCtrlMsg, sizeof (HBM_FLOW_CONTROL_MSG));
  FlowCtrlMsg.Cmd = HBM_CMD_FLOW_CONTROL;
  FlowCtrlMsg.HostAddress = BIOS_FIXED_HOST_ADDR + 1;
  FlowCtrlMsg.SecAddress = SecAddress;
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &FlowCtrlMsg,
                   sizeof (HBM_FLOW_CONTROL_MSG),
                   BIOS_FIXED_HOST_ADDR,
                   HECI_HBM_MSG_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "#####Fwuflow control: Send to SEC failed with status:%d.\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Setup a dynamic connection for FWU.

  @param[in, out]  SecAddress              Returns the SecAddress to be used by other FWU APIs
  @param[in]       MaxBufferSize           Specifies the maximum buffer size the FWU link allows.

  @retval          EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval          EFI_SUCCESS             Command succeeded.
  @retval          EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval          EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciConnectFwuInterface (
  IN OUT UINT8  *SecAddress,
  OUT UINT32    *MaxBufferSize
  )
{
  EFI_STATUS                   Status;
  EFI_HECI_PROTOCOL            *Heci;
  HBM_ENUM_MSG                 EnumMsg;
  HBM_ENUM_MSG_REPLY           EnumMsgReply;
  HBM_CLIENT_PROP_MSG          PropMsg;
  HBM_CLIENT_PROP_MSG_REPLY    PropMsgReply;
  HBM_CONNECT_MSG              ConnectMsg;
  HBM_CONNECT_MSG_REPLY        ConnectMsgReply;
  OEM_UUID                     FwuClientGuid;
  UINT32                       MsgReplyLen = 0;
  UINT8                        AddrIdx = 0;
  UINTN                        EnumIdx = 0;
  UINT8                        GuidData4Array[8] = {0x8F, 0x78, 0x60, 0x01, 0x15, 0xA3, 0x43, 0x27};

  FwuClientGuid.Data1   = 0x309DCDE8;
  FwuClientGuid.Data2   = 0xCCB1;
  FwuClientGuid.Data3   = 0x4062;
  CopyMem (&FwuClientGuid.Data4[0], &GuidData4Array[0], 8);

  DEBUG ((EFI_D_INFO, "HeciConnectFwuInterface +++.\n"));

  //
  // Check NULL Pointer
  //
  if (SecAddress == NULL) {
    DEBUG ((EFI_D_ERROR, "Invalid SecAdress assigned.\n"));
    return EFI_ABORTED;
  }
  *SecAddress = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SecAddress == NULL) {
    DEBUG ((EFI_D_ERROR, "Invalid SecAdress assigned.\n"));
    return EFI_ABORTED;
  }

  ZeroMem (&EnumMsg, sizeof (HBM_ENUM_MSG));
  ZeroMem (&EnumMsgReply, sizeof (HBM_ENUM_MSG_REPLY));
  EnumMsg.Cmd = HBM_CMD_ENUM;

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &EnumMsg,
                   sizeof (HBM_ENUM_MSG),
                   BIOS_FIXED_HOST_ADDR,
                   HECI_HBM_MSG_ADDR
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to send HBM_ENUM_MSG.\n"));
    return Status;
  }

  MsgReplyLen = sizeof (HBM_ENUM_MSG_REPLY);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &EnumMsgReply,
                   &MsgReplyLen
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HeciHBMEnum failed with ReadMsg, Status is: %d.\n", Status));
    return EFI_ABORTED;
  }

  if (EnumMsgReply.CmdReply != HBM_CMD_ENUM_REPLY ) {
    DEBUG ((EFI_D_ERROR, "HBM enum get invalid reply:%d\n", EnumMsgReply.CmdReply));
    return EFI_ABORTED;
  }

  for (AddrIdx = 8;  AddrIdx > 0; AddrIdx--) {
    DEBUG ((EFI_D_INFO, "Idx:%d, Value:%08x\n", 8 - AddrIdx, EnumMsgReply.ValidAddresses[AddrIdx - 1]));
  }

  ZeroMem (&PropMsg, sizeof (HBM_CLIENT_PROP_MSG));
  ZeroMem (&PropMsgReply, sizeof (HBM_CLIENT_PROP_MSG_REPLY));

  for (EnumIdx = 0; EnumIdx <= 255; EnumIdx++) {
    ZeroMem (&PropMsg, sizeof (HBM_CLIENT_PROP_MSG));
    ZeroMem (&PropMsgReply, sizeof (HBM_CLIENT_PROP_MSG_REPLY));

    PropMsg.Cmd = HBM_CMD_CLIENT_PROP;
    PropMsg.Address = (UINT8) EnumIdx;

    Status = Heci->SendMsg (
                     HECI1_DEVICE,
                     (UINT32 *) &PropMsg,
                     sizeof (HBM_CLIENT_PROP_MSG),
                     BIOS_FIXED_HOST_ADDR,
                     HECI_HBM_MSG_ADDR
                     );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Failed to send HBM_CLIENT_PROP_MSG.\n"));
      return Status;
    }

    MsgReplyLen = sizeof (HBM_CLIENT_PROP_MSG_REPLY);
    Status = Heci->ReadMsg (
                     HECI1_DEVICE,
                     BLOCKING,
                     (UINT32 *) &PropMsgReply,
                     &MsgReplyLen
                     );

    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }
    if (PropMsgReply.Status != 0) {
      continue;
    }
    if (!CompareMem (&FwuClientGuid, &PropMsgReply.ProtocolName, sizeof (OEM_UUID))) {
      DEBUG ((EFI_D_ERROR, "####Match:%d - Guid:%08x-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x.\n", \
             EnumIdx,   \
             PropMsgReply.ProtocolName.Data1, \
             PropMsgReply.ProtocolName.Data2, \
             PropMsgReply.ProtocolName.Data3, \
             PropMsgReply.ProtocolName.Data4[0], \
             PropMsgReply.ProtocolName.Data4[1], \
             PropMsgReply.ProtocolName.Data4[2], \
             PropMsgReply.ProtocolName.Data4[3], \
             PropMsgReply.ProtocolName.Data4[4], \
             PropMsgReply.ProtocolName.Data4[5], \
             PropMsgReply.ProtocolName.Data4[6], \
             PropMsgReply.ProtocolName.Data4[7]  \
            ));
      *SecAddress = PropMsgReply.Address;
      *MaxBufferSize = PropMsgReply.MaxMessageLength;
      break;
    }
  }  //end for.

  if (*SecAddress == 0) {
    DEBUG ((EFI_D_ERROR, "Failed to retrieve SEC address for FWU.\n"));
    return EFI_ABORTED;
  } else {
    DEBUG ((EFI_D_INFO, "####Connection property#####\n"));
    DEBUG ((EFI_D_INFO, "Address:%d, Protcol Ver:%d, MaxConnections:%d, FixedAddress:%d, SglRcvBuf:%d, MTU:%d.\n", \
           PropMsgReply.Address,  PropMsgReply.ProtocolVersion, PropMsgReply.MaximumConnections, \
           PropMsgReply.FixedAddress, PropMsgReply.SingleRecvBuffer, PropMsgReply.MaxMessageLength));

  }

  //
  // Now try to connect to the FWU update interace.
  //
  ZeroMem (&ConnectMsg, sizeof (HBM_CONNECT_MSG));
  ZeroMem (&ConnectMsgReply, sizeof (HBM_CONNECT_MSG_REPLY));

  ConnectMsg.Cmd = HBM_CMD_CONNECT;
  ConnectMsg.SecAddress = *SecAddress;
  ConnectMsg.HostAddress = BIOS_FIXED_HOST_ADDR + 1;

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &ConnectMsg,
                   sizeof (HBM_CONNECT_MSG),
                   BIOS_FIXED_HOST_ADDR,
                   HECI_HBM_MSG_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FwuConnect Send failed with status:%d.\n", Status));
    return Status;
  }

  MsgReplyLen = sizeof (HBM_CONNECT_MSG_REPLY);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &ConnectMsgReply,
                   &MsgReplyLen
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FwuConnect Recv failed with status:%d.\n", Status));
    return Status;
  }

  if (ConnectMsgReply.CmdReply != HBM_CMD_CONNECT_REPLY) {
    DEBUG ((EFI_D_ERROR, "####Reply Msg of connect is not HBM_CMD_CONNECT_REPLY.\n"));
    return EFI_ABORTED;
  }

  if (ConnectMsgReply.Status == 0 || ConnectMsgReply.Status == 2) { //Connection setup succeed, or already connected.
    DEBUG ((EFI_D_ERROR, "######CONNECTION SETUP SUCCESSFULLY######: \n"));
  } else {
    DEBUG ((EFI_D_ERROR, "####Failed to setup connection. Aborted with Status:%d.\n", Status));
    return EFI_ABORTED;
  }

  Status = HeciSecToHostFlowControl ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "HeciConnectFwuInterface ---.\n"));

  return EFI_SUCCESS;
}


/**
  Get firmware version from FWU interface instead of MKHI.

  @param[out]   Version                 Returns the version number of current running SEC FW.
  @param[in]    SecAddress              Dynamic sec address for FWU connection.

  @retval       EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval       EFI_SUCCESS             Command succeeded.
  @retval       EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval       EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciSendFwuGetVersionMsg (
  OUT VERSION  *Version,
  IN  UINT8    SecAddress
  )
{
  EFI_STATUS                 Status;
  EFI_HECI_PROTOCOL          *Heci;
  FWU_GET_VERSION_MSG        Msg;
  FWU_GET_VERSION_MSG_REPLY  MsgReply;
  UINT32                     ReplyLength = sizeof (FWU_GET_VERSION_MSG_REPLY);

  DEBUG ((EFI_D_INFO, "####HeciSendFwuGetVersion +++\n"));
  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = HeciHostToSecFlowControl (SecAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "###HeciSendFwuGetVersion: flow control fail.\n"));
  }

  Msg.MessageType = FWU_GET_VERSION;
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32*)&Msg,
                   sizeof(FWU_GET_VERSION_MSG),
                   BIOS_FIXED_HOST_ADDR + 1,
                   SecAddress
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FwuGetVersion Error: Failed to send FWU_GET_VERSION_MSG.\n"));
    return Status;
  }

  ZeroMem (&MsgReply, sizeof (FWU_GET_VERSION_MSG_REPLY));

  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32*)&MsgReply,
                   &ReplyLength
                   );
  //
  // What we get might be a flow control message, or the get version reply. Treat accordingly.
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HeciSendFwuGetVersionMsg failed with ReadMsg #1, Status is: %d.\n", Status));
    return EFI_ABORTED;
  }

  if (ReplyLength == 8) {
    //
    // Got a flow control message. proceed to get the FWU version message.
    //
    ZeroMem (&MsgReply, sizeof (FWU_GET_VERSION_MSG_REPLY));
    ReplyLength = sizeof (FWU_GET_VERSION_MSG_REPLY);
    Status = Heci->ReadMsg (
                     HECI1_DEVICE,
                     BLOCKING,
                     (UINT32 *) &MsgReply,
                     &ReplyLength
                     );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "HeciSendFwuGetVersionMsg failed with ReadMsg #2, Status is: %d.\n", Status));
      return EFI_ABORTED;
    } else {
      if (MsgReply.MessageType == FWU_GET_VERSION_REPLY) {
        CopyMem (Version, &MsgReply.CodeVersion, sizeof (VERSION));
        return EFI_SUCCESS;
      } else {
        DEBUG ((EFI_D_ERROR, "HeciSendFwuGetVersionMsg: the readmsg after flow control does not return fwu version reply.\n"));
        return EFI_ABORTED;
      }
    }
  } else {
    //
    // It's get version reply already. Get the version information and then retrieve the flow control msg from SEC.
    //
    if (MsgReply.MessageType == FWU_GET_VERSION_REPLY) {
      CopyMem (Version, &MsgReply.CodeVersion, sizeof (VERSION));
      Status = HeciSecToHostFlowControl ();
      return Status;
    } else {
      //
      // Any exception, we mark this as failed.
      //
      return EFI_ABORTED;
    }
  }  // endif.
}


/**
  Send Get Firmware SKU Request to Sec.

  @param[in, out] MsgGenGetFwCapsSku        Return message for Get Firmware Capability SKU.
  @param[in, out] MsgGenGetFwCapsSkuAck     Return message for Get Firmware Capability SKU ACK.

  @retval         EFI_UNSUPPORTED           Current Sec mode doesn't support this function.
  @retval         EFI_SUCCESS               Command succeeded.
  @retval         EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT               HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetFwCapsSkuMsg (
  IN OUT GEN_GET_FW_CAPSKU       *MsgGenGetFwCapsSku,
  IN OUT GEN_GET_FW_CAPS_SKU_ACK *MsgGenGetFwCapsSkuAck
  )
{
  EFI_STATUS         Status;
  UINT32             Length;
  EFI_HECI_PROTOCOL  *Heci;
  UINT32             SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetFwCapsSku->MKHIHeader.Data               = 0;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetFwCapsSku->MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetFwCapsSku->Data.RuleId                   = 0;
  Length = sizeof (GEN_GET_FW_CAPSKU);

  //
  // Send Get FW SKU Request to SEC
  //
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) MsgGenGetFwCapsSku,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_CAPS_SKU_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) MsgGenGetFwCapsSkuAck,
                   &Length
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}


/**
  Send Get Firmware Version Request to Sec.

  @param[in, out] MsgGenGetFwVersionAck   Return themessage of FW version.

  @retval         EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval         EFI_SUCCESS             Command succeeded.
  @retval         EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval         EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval         EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetFwVersionMsg (
  IN OUT GEN_GET_FW_VER_ACK     *MsgGenGetFwVersionAck
  )
{
  EFI_STATUS         Status;
  UINT32             Length;
  GEN_GET_FW_VER     *MsgGenGetFwVersion;
  GEN_GET_FW_VER     GenGetFwVersion;
  EFI_HECI_PROTOCOL  *Heci;
  UINT32             SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Allocate MsgGenGetFwVersion data structure
  //
  MsgGenGetFwVersion = &GenGetFwVersion;
  MsgGenGetFwVersion->MKHIHeader.Data = 0;
  MsgGenGetFwVersion->MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  MsgGenGetFwVersion->MKHIHeader.Fields.Command = GEN_GET_FW_VERSION_CMD;
  MsgGenGetFwVersion->MKHIHeader.Fields.IsResponse = 0;
  Length = sizeof (GEN_GET_FW_VER);

  //
  // Send Get Firmware Version Request to SEC
  //
  Status = Heci->SendMsg (
                  HECI1_DEVICE,
                  (UINT32 *) MsgGenGetFwVersion,
                  Length,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_VER_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) MsgGenGetFwVersionAck,
                   &Length
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}


/**
  Sends a message to Sec to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE HECI message needs to be sent after all OROMs finish their initialization.

  @param[in]  Nonce                   Nonce received in previous HMRFPO_ENABLE Response Message.
  @param[out] Result                  HMRFPO_ENABLE response.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciHmrfpoEnable (
  IN  UINT64                          Nonce,
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                   Status;
  EFI_HECI_PROTOCOL            *Heci;
  MKHI_HMRFPO_ENABLE           HmrfpoEnableRequest;
  MKHI_HMRFPO_ENABLE_RESPONSE  HmrfpoEnableResponse;
  UINT32                       HeciLength;
  UINT32                       SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoEnableRequest.MkhiHeader.Data               = 0;
  HmrfpoEnableRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.Command     = HMRFPO_ENABLE_CMD_ID;
  HmrfpoEnableRequest.MkhiHeader.Fields.IsResponse  = 0;
  HmrfpoEnableRequest.Nonce                         = Nonce;

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &HmrfpoEnableRequest,
                   HeciLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_ENABLE_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_ENABLE_RESPONSE);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &HmrfpoEnableResponse,
                   &HeciLength
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Read HMRFPO_ENABLE_CMD_ID Result - %r\n", Status));
    return Status;
  }

  *Result = HmrfpoEnableResponse.Status;

  return Status;
}


/**
  Sends a message to Sec to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce                   Random number generated by Ignition Sec FW. When BIOS.
                                      want to unlock region it should use this value.
                                      in HMRFPO_ENABLE Request Message.
  @param[out] FactoryDefaultBase      The base of the factory default calculated from the start of the ME region.
                                      BIOS sets a Protected Range (PR) register "Protected Range Base" field with this value.
                                      + the base address of the region.
  @param[out] FactoryDefaultLimit     The length of the factory image.
                                      BIOS sets a Protected Range (PR) register "Protected Range Limit" field with this value.
  @param[out] Result                  Status report.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciHmrfpoLock (
  OUT UINT64                          *Nonce,
  OUT UINT32                          *FactoryDefaultBase,
  OUT UINT32                          *FactoryDefaultLimit,
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                 Status;
  EFI_HECI_PROTOCOL          *Heci;
  MKHI_HMRFPO_LOCK           HmrfpoLockRequest;
  MKHI_HMRFPO_LOCK_RESPONSE  HmrfpoLockResponse;
  UINT32                     HeciLength;
  UINT32                     SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoLockRequest.MkhiHeader.Data               = 0;
  HmrfpoLockRequest.MkhiHeader.Fields.GroupId     = MKHI_SPI_GROUP_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.Command     = HMRFPO_LOCK_CMD_ID;
  HmrfpoLockRequest.MkhiHeader.Fields.IsResponse  = 0;

  HeciLength = sizeof (MKHI_HMRFPO_LOCK);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &HmrfpoLockRequest,
                   HeciLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_LOCK_CMD_ID Request - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_LOCK_RESPONSE);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &HmrfpoLockResponse,
                   &HeciLength
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to read HMRFPO_LOCK_CMD_ID response - %r.\n", Status));
    return Status;
  }

  *Nonce                = HmrfpoLockResponse.Nonce;
  *FactoryDefaultBase   = HmrfpoLockResponse.FactoryDefaultBase;
  *FactoryDefaultLimit  = HmrfpoLockResponse.FactoryDefaultLimit;
  *Result               = HmrfpoLockResponse.Status;

  return Status;
}


/**
  System BIOS sends this message to get status for HMRFPO_LOCK message.

  @param[out] Result                  HMRFPO_GET_STATUS response.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciHmrfpoGetStatus (
  OUT UINT8                           *Result
  )
{
  EFI_STATUS                       Status;
  EFI_HECI_PROTOCOL                *Heci;
  MKHI_HMRFPO_GET_STATUS           HmrfpoGetStatusRequest;
  MKHI_HMRFPO_GET_STATUS_RESPONSE  HmrfpoGetStatusResponse;
  UINT32                           HeciLength;
  UINT32                           SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  HmrfpoGetStatusRequest.MkhiHeader.Data              = 0;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.GroupId    = MKHI_SPI_GROUP_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.Command    = HMRFPO_GET_STATUS_CMD_ID;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.IsResponse = 0;

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &HmrfpoGetStatusRequest,
                   HeciLength,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send HMRFPO_GET_STATUS_CMD_ID - %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (MKHI_HMRFPO_GET_STATUS_RESPONSE);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &HmrfpoGetStatusResponse,
                   &HeciLength
                   );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Read HMRFPO_GET_STATUS_CMD_ID Result - %r\n", Status));
  }
  *Result = HmrfpoGetStatusResponse.Status;

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to query the local firmware update interface status.

  @param[out] RuleData                1 - local firmware update interface enable.
                                      0 - local firmware update interface disable.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetLocalFwUpdate (
  OUT UINT32         *RuleData
  )
{
  EFI_STATUS                   Status;
  UINT32                       Length;
  EFI_HECI_PROTOCOL            *Heci;
  GEN_GET_LOCAL_FW_UPDATE      MsgGenGetLocalFwUpdate;
  GEN_GET_LOCAL_FW_UPDATE_ACK  MsgGenGetLocalFwUpdatekuAck;
  UINT32                       SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetLocalFwUpdate.Data.RuleId                  = 7;
  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE);

  //
  // Send Get Local FW update Request to SEC
  //
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgGenGetLocalFwUpdate,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgGenGetLocalFwUpdatekuAck,
                   &Length
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  *RuleData = MsgGenGetLocalFwUpdatekuAck.Data.RuleData;

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP) on the boot
  where host wants to enable or disable the local firmware update interface.
  The firmware allows a single update once it receives the enable command.

  @param[in] RuleData                1 - local firmware update interface enable
                                     0 - local firmware update interface disable

  @retval    EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval    EFI_SUCCESS             Command succeeded.
  @retval    EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval    EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval    EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciSetLocalFwUpdate (
  IN UINT8         RuleData
  )
{
  EFI_STATUS                   Status;
  UINT32                       Length;
  EFI_HECI_PROTOCOL            *Heci;
  GEN_SET_LOCAL_FW_UPDATE      MsgGenSetLocalFwUpdate;
  GEN_SET_LOCAL_FW_UPDATE_ACK  MsgGenSetLocalFwUpdateAck;
  UINT32                       SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenSetLocalFwUpdate.MKHIHeader.Data              = 0;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.Command    = FWCAPS_SET_RULE_CMD;
  MsgGenSetLocalFwUpdate.MKHIHeader.Fields.IsResponse = 0;
  MsgGenSetLocalFwUpdate.Data.RuleId                  = 7;
  MsgGenSetLocalFwUpdate.Data.RuleDataLen             = 4;
  MsgGenSetLocalFwUpdate.Data.RuleData                = RuleData;
  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE);

  //
  // Send Get Local FW update Request to SEC
  //
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgGenSetLocalFwUpdate,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_SET_LOCAL_FW_UPDATE_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgGenSetLocalFwUpdateAck,
                   &Length
                   );

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to enable the Sec State. The firmware allows a single
  update once it receives the enable command. Once firmware receives this message,
  the firmware will be in normal mode after a global reset.

  @param[in] None

  @retval    EFI_UNSUPPORTED      Current Sec mode doesn't support this function.
  @retval    EFI_SUCCESS          ME enabled message sent.

**/
EFI_STATUS
HeciSetSeCEnableMsg (
  IN VOID
  )
{
  EFI_STATUS            Status;
  EFI_HECI_PROTOCOL     *Heci;
  HECI_FWS_REGISTER     SeCFirmwareStatus;
  UINTN                 HeciPciAddressBase;
  UINT16                TimeOut;

  TimeOut = 0;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HeciPciAddressBase = PCI_LIB_ADDRESS (
                         SEC_BUS,
                         SEC_DEVICE_NUMBER,
                         HECI_FUNCTION_NUMBER,
                         0
                         );
  PciWrite8 (HeciPciAddressBase + R_GEN_STS + 3, 0x20);
  do {
    SeCFirmwareStatus.ul = PciRead32 (HeciPciAddressBase + R_SEC_FW_STS0);
    gBS->Stall (EFI_SEC_STATE_STALL_1_SECOND);
    TimeOut++;
  } while ((SeCFirmwareStatus.r.FwInitComplete != SEC_FIRMWARE_COMPLETED) && (TimeOut > EFI_SEC_STATE_MAX_TIMEOUT));


  Status = HeciSetSeCDisableMsg (1);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HeciSetSeCDisableMsg Status = %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Send Global reset
  //
  HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
  CpuDeadLoop ();

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to disable the Sec State. The firmware allows a single
  update once it receives the disable command Once firmware receives this message,
  the firmware will work in "Soft Temporary Disable" mode (HFS[19:16] = 3) after a
  global reset. Note, this message is not allowed when AT firmware is enrolled/configured.

  @param[in] RuleData               Rule data
                                    0 - Disable
                                    1 - Enable

  @retval EFI_UNSUPPORTED           Current Sec mode doesn't support this function
  @retval EFI_SUCCESS               Sec is disabled

**/
EFI_STATUS
HeciSetSeCDisableMsg (
  IN UINT8 ruleData
  )
{
  EFI_STATUS         Status;
  UINT32             Length;
  EFI_HECI_PROTOCOL  *Heci;
  GEN_SET_FW_CAPSKU  MsgSeCStateControl;
  UINT32             SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode(HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgSeCStateControl.MKHIHeader.Data               = 0;
  MsgSeCStateControl.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgSeCStateControl.MKHIHeader.Fields.Command     = FWCAPS_SET_RULE_CMD;
  MsgSeCStateControl.MKHIHeader.Fields.IsResponse  = 0;
  MsgSeCStateControl.Data.RuleId.Data              = 6;
  MsgSeCStateControl.Data.RuleDataLen              = 4;
  MsgSeCStateControl.Data.RuleData                 = ruleData;

  Length = sizeof (GEN_SET_FW_CAPSKU);

  Status = Heci->SendwACK (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgSeCStateControl,
                   Length,
                   &Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );

  HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
  CpuDeadLoop ();

  return Status;
}


/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get platform type.
  One of usages is to utilize this command to determine if the platform runs in
  Consumer or Corporate SKU size firmware.

  @param[out] RuleData                PlatformBrand,
                                      IntelMeFwImageType,
                                      SuperSku,
                                      PlatformTargetUsageType

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetPlatformTypeMsg (
  OUT PLATFORM_TYPE_RULE_DATA   *RuleData
  )
{
  EFI_STATUS                 Status;
  UINT32                     Length;
  EFI_HECI_PROTOCOL          *Heci;
  GEN_GET_PLATFORM_TYPE      MsgGenGetPlatformType;
  GEN_GET_PLATFORM_TYPE_ACK  MsgGenGetPlatformTypeAck;
  UINT32                     SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetPlatformType.MKHIHeader.Data               = 0;
  MsgGenGetPlatformType.MKHIHeader.Fields.GroupId     = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetPlatformType.MKHIHeader.Fields.Command     = FWCAPS_GET_RULE_CMD;
  MsgGenGetPlatformType.MKHIHeader.Fields.IsResponse  = 0;
  MsgGenGetPlatformType.Data.RuleId                   = 0x1D;
  Length = sizeof (GEN_GET_PLATFORM_TYPE);

  //
  // Send Get Platform Type Request to SEC
  //
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgGenGetPlatformType,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_PLATFORM_TYPE_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgGenGetPlatformTypeAck,
                   &Length
                   );

  *RuleData = MsgGenGetPlatformTypeAck.Data.RuleData;

  return Status;
}


/**
  The firmware will respond to GET OEM TAG message even after the End of Post (EOP).

  @param[out] RuleData                Default is zero. Tool can create the OEM specific OEM TAG data.

  @retval     EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.
  @retval     EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge.

**/
EFI_STATUS
HeciGetOemTagMsg (
  OUT UINT32         *RuleData
  )
{
  EFI_STATUS               Status;
  UINT32                   Length;
  EFI_HECI_PROTOCOL        *Heci;
  GEN_GET_OEM_TAG_MSG      MsgGenGetOemTagMsg;
  GEN_GET_OEM_TAG_MSG_ACK  MsgGenGetOemTagMsgAck;
  UINT32                   SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode(HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgGenGetOemTagMsg.MKHIHeader.Data              = 0;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  MsgGenGetOemTagMsg.MKHIHeader.Fields.IsResponse = 0;
  MsgGenGetOemTagMsg.Data.RuleId                  = 0x2B;
  Length = sizeof (GEN_GET_OEM_TAG_MSG);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgGenGetOemTagMsg,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_OEM_TAG_MSG_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgGenGetOemTagMsgAck,
                   &Length
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *RuleData = MsgGenGetOemTagMsgAck.RuleData;

  return Status;
}

EFI_STATUS
HeciGetFwFeatureStateMsg (
  OUT SECFWCAPS_SKU                    *RuleData
  )
{
  EFI_STATUS                     Status;
  UINT32                         Length;
  GEN_GET_FW_FEATURE_STATUS      GetFwFeatureStatus;
  GEN_GET_FW_FEATURE_STATUS_ACK  GetFwFeatureStatusAck;
  UINT32                         SeCMode;
  EFI_HECI_PROTOCOL              *Heci;

  DEBUG ((EFI_D_INFO, "HeciGetFwFeatureStateMsg ++\n"));
  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode(HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetFwFeatureStatus.MKHIHeader.Data              = 0;
  GetFwFeatureStatus.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  GetFwFeatureStatus.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  GetFwFeatureStatus.MKHIHeader.Fields.IsResponse = 0;
  GetFwFeatureStatus.Data.RuleId                  = 0x20;

  Length = sizeof (GEN_GET_FW_FEATURE_STATUS);
  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &GetFwFeatureStatus,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (GEN_GET_FW_FEATURE_STATUS_ACK);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &GetFwFeatureStatusAck,
                   &Length
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  RuleData->Data = GetFwFeatureStatusAck.RuleData.Data;
  DEBUG ((EFI_D_INFO, "HeciGetFwFeatureStateMsg --\n"));

  return Status;
}


EFI_STATUS
HeciSeCUnconfigurationMsg (
  OUT UINT32    *CmdStatus
  )
{
  EFI_STATUS           Status;
  UINT32               Length;
  EFI_HECI_PROTOCOL    *Heci;
  MKHI_MESSAGE_HEADER  MsgSeCUnConfigure;
  MKHI_MESSAGE_HEADER  MsgSeCUnConfigureAck;
  UINT32               SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgSeCUnConfigure.Data               = 0;
  MsgSeCUnConfigure.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MsgSeCUnConfigure.Fields.Command     = SEC_UNCONFIGURATION_CMD;
  MsgSeCUnConfigure.Fields.IsResponse  = 0;

  Length = sizeof (MKHI_MESSAGE_HEADER);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgSeCUnConfigure,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (MKHI_MESSAGE_HEADER);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgSeCUnConfigureAck,
                   &Length
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  *CmdStatus = MsgSeCUnConfigureAck.Fields.Reserved;
  return Status;

}


EFI_STATUS
HeciSeCUnconfigurationStatusMsg (
  IN UINT32    *CmdStatus
  )
{
  EFI_STATUS           Status;
  UINT32               Length;
  EFI_HECI_PROTOCOL    *Heci;
  MKHI_MESSAGE_HEADER  MsgSeCUnConfigureStatus;
  MKHI_MESSAGE_HEADER  MsgSeCUnConfigureStatusAck;
  UINT32               SeCMode;

  Status = gBS->LocateProtocol (
                  &gEfiHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetSeCMode(HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MsgSeCUnConfigureStatus.Data               = 0;
  MsgSeCUnConfigureStatus.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MsgSeCUnConfigureStatus.Fields.Command     = SEC_UNCONFIGURATION_STATUS;
  MsgSeCUnConfigureStatus.Fields.IsResponse  = 0;

  Length = sizeof (MKHI_MESSAGE_HEADER);

  Status = Heci->SendMsg (
                   HECI1_DEVICE,
                   (UINT32 *) &MsgSeCUnConfigureStatus,
                   Length,
                   BIOS_FIXED_HOST_ADDR,
                   HECI_CORE_MESSAGE_ADDR
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (MKHI_MESSAGE_HEADER);
  Status = Heci->ReadMsg (
                   HECI1_DEVICE,
                   BLOCKING,
                   (UINT32 *) &MsgSeCUnConfigureStatusAck,
                   &Length
                   );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  *CmdStatus = MsgSeCUnConfigureStatusAck.Fields.Result;
  return Status;
}



