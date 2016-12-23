/** @file
  HECI Smm driver.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Protocol/SmmAccess2.h>
#include <Protocol/SmmEndOfDxe.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <HeciSmm.h>
#include <Private/Library/HeciInitLib.h>
#include <SeCAccess.h>
#include <library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PerformanceLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/Heci2Pm.h>

UINT32 mHeci_Length = 0;
UINT32 mHeci_LeftSize = 0;
UINT32 mHeci_WriteSize = 0;
UINT32 mHeci_SeCAddress = 0;
UINT32 mHeci_HostAddress = 0;
UINT32 mHeci_HeciBar = 0;
UINT32 *mHeci_Message = NULL;
BOOLEAN mHeci_Complete = TRUE;
BOOLEAN mAtRuntime = FALSE;

extern  UINT32  mMonoCount;
extern UINT8  mHmacSha256Key[32];

/**
  Disable HECI Interrupt.

 **/
VOID HeciDisableInterrupt (
  VOID
  );

/**
  Clear HECI Interrupt flag.

 **/
VOID HeciClearInterrupt (
  VOID
  );

/**
  Enabe HECI interrupt.

 **/
VOID HeciEnableInterrupt (
  VOID
  );

/**
  Heci Interrupt Handle

**/

VOID
HeciInterruptHandle (
  VOID
  );

/**
  Receive Heci interrupt handle.

  @param[in, out]     Meesge
  @param[in, out]     Length

  @retval             Status

 **/
EFI_STATUS
EFIAPI
HeciReceiveInterrupt (
  IN OUT  UINT8  *Message,
  IN OUT  UINT32  *Length
  );

/**
  Check is Heci Cse Response.

  @retval        Bool value

 **/
BOOLEAN
IsHeciCseResponse (
  VOID
  );

EFI_STATUS
EfiHeciReadMessage (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           Blocking,
  IN      UINT32           *MessageBody,
  IN OUT  UINT32           *Length
  );

EFI_STATUS
EFIAPI
Heci2SendwACK (
  IN OUT  UINT32  *Message,
  IN      UINT32  Length,
  IN OUT  UINT32  *RecLength,
  IN      UINT8   HostAddress,
  IN      UINT8   SeCAddress
  );

BOOLEAN
EFIAPI
AtRuntime (
  VOID
  );

/**
  Send Power Gating message.

  @retval            Status

 **/
EFI_STATUS
SendPGIEntry (
  VOID
  )
{
  UINT32                                Size;
  UINT32                                MessageBody[20];
  HECI_MESSAGE_HEADER                   MessageHeader;
  volatile UINT32                       *WriteBuffer;
  volatile HECI_HOST_CONTROL_REGISTER   *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER    *SecControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + SEC_CSR_HA);
  WriteBuffer = (UINT32 *) (UINTN) (mHeci_HeciBar + H_CB_WW);

  //
  // Prepare message header
  //
  MessageHeader.Data = 0x80040000;
  MessageBody[0] = 0x0000000A;
  *WriteBuffer = MessageHeader.Data;
  *WriteBuffer = MessageBody[0];

  //
  // Send the Interrupt;
  //
  HostControlReg->r.H_IG = 1;

  while (HostControlReg->r.H_IS == 0) {
    MicroSecondDelay (10000);
  }

  while (!IsHeciCseResponse ()) {
    MicroSecondDelay (10000);
  }

  HeciDisableInterrupt ();
  HeciClearInterrupt ();

  Size = sizeof (UINT32);
  HeciReceiveInterrupt ((UINT8 *) &MessageBody, &Size);
  ASSERT (MessageBody[0] == 0x0000008A);// retry

  HeciEnableInterrupt ();

  return EFI_SUCCESS;
}


BOOLEAN
EFIAPI
IsHeci2Idle (
  VOID
  )
{
  return (Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC) & B_HECI_DEVIDLEC_DEVIDLE) == B_HECI_DEVIDLEC_DEVIDLE;
}

/**
  Set HECI2 to idle (D0i3).

  @params   None.

  @retval   None.

**/
VOID
EFIAPI
SetHeci2Idle (
  VOID
  )
{
  //
  // No need to continue if HECI2 is already in idle or in BIOS boot time, i.e., before ExitBootService.
  // HECI2 should keep in Idle only in OS environment.
  //
  if (IsHeci2Idle() || !AtRuntime()) {
    return;
  }

  DEBUG ((EFI_D_INFO, "SetHeci2Idle: HECI2 was active. Setting HECI2 to idle...\n"));

  Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC) = (B_HECI_DEVIDLEC_DEVIDLE | B_HECI_DEVIDLEC_IR);
  while ((Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC) & B_HECI_DEVIDLEC_CIP) == B_HECI_DEVIDLEC_CIP);
}


/**
  Bring out HECI2 out of D0i3.

  @params  None.

  @retval  None.

 **/
VOID
EFIAPI
SetHeci2Active (
  VOID
  )
{
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;

  DEBUG ((EFI_D_INFO, "SetHeci2Active: Setting HECI2 to active...\n"));

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER *) (UINTN) (mHeci_HeciBar + H_CSR);

  while ((Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC) & B_HECI_DEVIDLEC_CIP) == B_HECI_DEVIDLEC_CIP);

  if (!IsHeci2Idle ()) {
    return;
  }

  Mmio32And (mHeci_HeciBar, R_HECI_DEVIDLEC, (UINT32) (~(B_HECI_DEVIDLEC_DEVIDLE | B_HECI_DEVIDLEC_RR)));

  while ((Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC) & B_HECI_DEVIDLEC_CIP) == B_HECI_DEVIDLEC_CIP);

  //
  // Clear interrupt status (if any).
  //
  if (HostControlReg->r.H_DEVIDLEC_IS == 1) {
    HostControlReg->r.H_DEVIDLEC_IS = 1;
  }

  DEBUG ((EFI_D_INFO, "SetHeci2Active: HECI2 DEVIDLEC register value (should be active) = 0x%x.\n", Mmio32 (mHeci_HeciBar, R_HECI_DEVIDLEC)));
}


UINTN
EFIAPI
GetHeci2Bar (
  VOID
  )
{
  return mHeci_HeciBar;
}


VOID
EFIAPI
SetHeci2Bar (
  UINTN  Heci2Bar
  )
{
  mHeci_HeciBar = (UINT32) Heci2Bar;
}


/**
  Handle for send the Heci2 message.

  @retval              bool value

 **/
BOOLEAN HeciSendHandle (
  VOID
  )
{
  UINT32                                 MaxBuffer;
  UINT32                                 Size;
  UINTN                                  Index;
  UINT32                                 *MessageBody;
  HECI_MESSAGE_HEADER                    MessageHeader;
  volatile UINT32                        *WriteBuffer;
  volatile HECI_HOST_CONTROL_REGISTER    *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER     *SecControlReg;

  if (mHeci_LeftSize == 0) {
    return TRUE;
  }
  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + SEC_CSR_HA);
  WriteBuffer = (UINT32 *) (UINTN) (mHeci_HeciBar + H_CB_WW);
  MessageBody = (UINT32*) mHeci_Message;

  MaxBuffer = HostControlReg->r.H_CBD - 0x10;

  while (mHeci_LeftSize > 0) {
    Size = (mHeci_LeftSize > MaxBuffer) ? MaxBuffer : mHeci_LeftSize;
    mHeci_LeftSize -= Size;
    //
    // Prepare message header
    //
    MessageHeader.Data = 0;
    MessageHeader.Fields.SeCAddress   = mHeci_SeCAddress;
    MessageHeader.Fields.HostAddress = mHeci_HostAddress;
    MessageHeader.Fields.MessageComplete = (mHeci_LeftSize > 0) ? 0 : 1;
    MessageHeader.Fields.Length = (UINT32) ((mHeci_LeftSize > 0) ? Size * sizeof (UINT32) : mHeci_Length - mHeci_WriteSize * sizeof (UINT32));
    *WriteBuffer = MessageHeader.Data;
    for (Index = 0; Index < Size; Index ++) {
      *WriteBuffer = MessageBody[Index + mHeci_WriteSize];
    }
    mHeci_WriteSize += Size;
    //
    // Send the Interrupt;
    //
    HostControlReg->r.H_IG = 1;

    while (HostControlReg->r.H_IS == 0) {
      MicroSecondDelay (10000);
    }
  }
  HeciDisableInterrupt ();
  HeciClearInterrupt ();
  HeciEnableInterrupt ();

  return (mHeci_LeftSize == 0);
}

UINT32   *CommandDataBuffer = NULL;
UINTN    CommandDataBufferSize = 0;
UINTN    CommandDataBufferHead = 0;
UINTN    CommandDataBufferTail = 0;
UINT32 * CommandMessage = NULL;


/**
  Store Data into Queue.

  @param[in]  Data
  @param[in]  DataLength

**/
VOID
InsertToQueue (
  UINT32 * Data,
  UINTN    DataLength
  )
{
  UINTN    TempLength;

  if (CommandDataBufferSize + CommandDataBufferTail - CommandDataBufferHead < DataLength) {
    ASSERT (FALSE);
    return;
  }
  if (CommandDataBufferHead + DataLength <= CommandDataBufferSize) {
    CopyMem (&CommandDataBuffer[CommandDataBufferHead], Data, DataLength * sizeof (UINT32));
    CommandDataBufferHead += DataLength;
    CommandDataBufferHead %= CommandDataBufferSize;
  } else {
    TempLength = CommandDataBufferSize - CommandDataBufferHead;
    InsertToQueue (Data, TempLength);
    InsertToQueue (Data + TempLength, DataLength - TempLength);
  }
}


/**
  Get Data from Queue.

  @param[in]  Data
  @param[in]  DataLength

**/
VOID
RemoveFromQueue (
  UINT32 * Data,
  UINTN    DataLength
  )
{
  UINTN    TempLength;

  if (CommandDataBufferTail + DataLength <= CommandDataBufferSize) {
    CopyMem (Data, &CommandDataBuffer[CommandDataBufferTail], DataLength * sizeof (UINT32));
    CommandDataBufferTail += DataLength;
    CommandDataBufferTail %= CommandDataBufferSize;
  } else {
    TempLength = CommandDataBufferSize - CommandDataBufferTail;
    RemoveFromQueue (Data, TempLength);
    RemoveFromQueue (Data + TempLength, DataLength - TempLength);
  }
}


/**
  Init Heci Interrupt Buffer.

**/
VOID HeciInterruptInit (
  VOID
  )
{
  UINT32  TempHeciBar = 0;

  TempHeciBar = Heci2PciRead32 (R_HECIMBAR0) & 0xFFFFFFF0;
  if (TempHeciBar != 0xFFFFFFF0 ) {  //No need to assign when the device is disabled
    mHeci_HeciBar = TempHeciBar;
  }
  if (CommandDataBuffer != NULL) {
    return;
  }

  CommandDataBuffer = AllocatePages (EFI_SIZE_TO_PAGES (MAX_HECI_BUFFER_SIZE) * 4);
  CommandDataBufferSize = MAX_HECI_BUFFER_SIZE * 4 / sizeof (UINT32);
  CommandMessage = AllocatePages (EFI_SIZE_TO_PAGES (MAX_HECI_BUFFER_SIZE));
}


/**
  Store Command into buffer.

  @param[in]  Data
  @param[in]  DataLength
  @param[in]  MsgLength
  @param[in]  CseAddress
  @param[in]  HostAddress

**/
VOID StoreCommand (
  IN UINT32 *Data,
  IN UINT32 DataLength,
  IN UINT32 MsgLength,
  IN UINT32 CseAddress,
  IN UINT32 HostAddress
  )
{
  if (CommandDataBufferSize + CommandDataBufferTail - CommandDataBufferHead < (DataLength + 4)) {
    ASSERT (FALSE);
  }

  InsertToQueue (&DataLength, 1);
  InsertToQueue (&MsgLength, 1);
  InsertToQueue (&CseAddress, 1);
  InsertToQueue (&HostAddress, 1);
  InsertToQueue (Data, DataLength);
}


/**
  Load the next Command from buffer.

  @param[in]  DataSize
  @param[in]  MsgLength
  @param[in]  CseAddress
  @param[in]  HostAddress

  @retval  Data

**/
UINT32 *
LoadCommand (
  IN UINT32 *DataSize,
  IN UINT32 *MsgLength,
  IN UINT32 *CseAddress,
  IN UINT32 *HostAddress
  )
{
  UINT32   *Data;

  if (CommandDataBuffer[CommandDataBufferTail] == (UINT32) -1) {
    CommandDataBufferTail = 0;
  }

  RemoveFromQueue (DataSize, 1);
  RemoveFromQueue (MsgLength, 1);
  RemoveFromQueue (CseAddress, 1);
  RemoveFromQueue (HostAddress, 1);
  RemoveFromQueue (CommandMessage, *DataSize);
  Data = CommandMessage;

  return Data;
}


BOOLEAN mProxyStopPhase = FALSE;
VOID CheckCommandQueue (
  VOID
  )
{
  if (CommandDataBufferHead == CommandDataBufferTail) {
    return;
  }
  if (mHeci_LeftSize != 0) {
    return;
  }
  if (!mHeci_Complete) {
    return;
  }
  mHeci_Message = LoadCommand(&mHeci_LeftSize, &mHeci_Length, &mHeci_SeCAddress, &mHeci_HostAddress);
  mHeci_WriteSize = 0;
  mHeci_Complete = FALSE;
  if (mProxyStopPhase) {
    if (*((UINT8 *) mHeci_Message) == 4) {
      HeciSendHandle ();
      while (!mHeci_Complete) {
        HeciInterruptHandle ();
        MicroSecondDelay (10000);
      }
    }
  } else {
    HeciSendHandle ();
  }
}


BOOLEAN mBlackHolePhase = FALSE;

BOOLEAN InBXTBlackHolePhase (
  )
{
  return mBlackHolePhase;
}

VOID CheckBXTBlackHole (
  UINT8     *Data
  )
{
  HECI2_BIOS_MASSAGE   *Message;

  Message = (HECI2_BIOS_MASSAGE *) Data;
  if (Message->header.cmd_id == HECI2_GET_PROXY_STATE_CMD_ID) {
    if (Message->header.req_resp == 0) {
      mBlackHolePhase = TRUE;
    } else {
      mBlackHolePhase = FALSE;
    }
  }
}


/**
  Interrupt Handle for HECI send message.

  @param[in]  Message
  @param[in]  Length
  @param[in]  HostAddress
  @param[in]  SeCAddress

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendInterrupt (
  IN UINT8                    *Message,
  IN UINT32                   Length,
  IN UINT8                    HostAddress,
  IN UINT8                    SeCAddress
  )
{
  UINT32    LeftSize;

  CheckBXTBlackHole (Message);

  //
  // The first DWORD used for send MessageHeader, so useable Buffer Size should Be MaxBuffer -1;
  //
  LeftSize = (Length + 3) / 4;

  StoreCommand ((UINT32 *) Message, LeftSize, Length, SeCAddress, HostAddress);
  CheckCommandQueue ();

  return EFI_SUCCESS;
}


/**
  Interrupt Handle for HECI Receive message.

  @param[in]  Message
  @param[in]  Length

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciReceiveInterrupt (
  IN OUT  UINT8  *Message,
  IN OUT  UINT32  *Length
  )
{
  UINTN                                ReadSize;
  UINTN                                Size;
  UINTN                                Index;
  UINT32                               *MessageBody;
  UINT32                               Data;
  HECI_MESSAGE_HEADER                  MessageHeader;
  volatile UINT32                      *ReadBuffer;
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER   *SecControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + SEC_CSR_HA);
  ReadBuffer = (UINT32 *) (UINTN) (mHeci_HeciBar + SEC_CB_RW);

  Size = 0;
  ReadSize = 0;
  MessageBody = (UINT32 *) Message;

  MessageHeader.Data = *ReadBuffer;
  if (*Length > MessageHeader.Fields.Length) {
    *Length = MessageHeader.Fields.Length;
  }
  for (Index = 0; Index < (*Length + 3) / 4; Index ++) {
    MessageBody[Index + ReadSize] = *ReadBuffer;
  }
  for (;Index < (MessageHeader.Fields.Length + 3) / 4; Index ++) {
    Data = *ReadBuffer;
  }
  CheckBXTBlackHole (Message);

  return EFI_SUCCESS;
}


/**
  Disable Heci interrupt.

**/
VOID
HeciDisableInterrupt (
  VOID
  )
{
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);

  HostControlReg->r.H_IE = 0;
}


/**
  Enable Heci Interrupt.

**/
VOID
HeciEnableInterrupt (
  VOID
  )
{
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);

  HostControlReg->r.H_IE = 1;
}


/**
  Clear Heci Interrupt Flag.

**/
VOID
HeciClearInterrupt (
  VOID
  )
{
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);

  HostControlReg->r.H_IS = 1;
}


/**
  Check dies Heci Interrupt Occur.

  @retval  BOOLEAN

**/
BOOLEAN
HeciInterruptOccur (
  VOID
  )
{
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + H_CSR);

  return (HostControlReg->r.H_IS == 1);
}


/**
  is there Heci Response?

  @retval  BOOLEAN

**/
BOOLEAN
IsHeciCseResponse (
  VOID
  )
{
  volatile HECI_SEC_CONTROL_REGISTER  *SecControlReg;

  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (mHeci_HeciBar + SEC_CSR_HA);

  return (SecControlReg->r.SEC_CBRP_HRA != SecControlReg->r.SEC_CBWP_HRA);
}


/**
  Heci Interrupt Handle.

**/
UINT8  Msg[400];

VOID
HeciInterruptHandle (
  VOID
  )
{
  UINT32      MsgLength;

  if (IsHeciCseResponse ()) {
    HeciDisableInterrupt ();
    HeciClearInterrupt ();
    MsgLength = sizeof (Msg);
    HeciReceiveInterrupt (Msg, &MsgLength);
    mHeci_Complete = TRUE;
    HeciEnableInterrupt ();
  } else {
    if (HeciSendHandle ()) {
    }
  }
  CheckCommandQueue ();

  if ((CommandDataBufferHead == CommandDataBufferTail) && mHeci_Complete) {
    SetHeci2Idle ();
  }
}


VOID ProxyStopNotify (
  VOID
  )
{
  HeciPciWrite32 (PCI_CFG_GS3, (PCI_CFG_GS3_PANIC_SUPPORTED | PCI_CFG_GS3_PANIC));
  mProxyStopPhase = TRUE;
}


VOID FlashCommandQueue (
  VOID
  )
{
  SetHeci2Active ();
  while (CommandDataBufferHead != CommandDataBufferTail) {
    CheckCommandQueue ();
  }
}


/**
  SMI Callback function to inform CSE to take NVM ownership.

  @param[in]  DispatchHandle
  @param[in]  RegisterContext
  @param[in]  CommBuffer
  @param[in]  CommBufferSize

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
SystemResetCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  ProxyStopNotify ();
  HeciInterruptHandle ();
  mHeci_Complete = TRUE;
  FlashCommandQueue ();

  return EFI_SUCCESS;
}


/**
  Registering SMI Callback function for System Reset Notification.

  @param[in]  None

  @retval     None

**/
VOID InitSystemResetHandle (
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT    SwContext;
  EFI_HANDLE                     SwHandle;

  //
  // Get the Sw dispatch protocol and register SMI callback functions.
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
  ASSERT_EFI_ERROR (Status);
  SwContext.SwSmiInputValue = (UINTN) HECI_SYSTEM_RESET_NOTIFY;
  Status = SwDispatch->Register (SwDispatch, SystemResetCallback, &SwContext, &SwHandle);
  ASSERT_EFI_ERROR (Status);
}

UINTN Tag = 0;


/**
  SMM handle to dispatch request.

  @param[in]  DispatchHandle
  @param[in]  RegisterContext
  @param[in]  CommBuffer
  @param[in]  CommBufferSize

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSmmHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  )
{
  EFI_STATUS                                       Status=EFI_SUCCESS;
  SMM_HECI_COMMUNICATE_HEADER                      *SmmHeciFunctionHeader;
  SMM_HECI_SEND_MESSAGE_BODY                       *SendMessageBody;
  SMM_HECI_READ_MESSAGE_BODY                       *ReadMessageBody;
  SMM_HECI_SEND_MESSAGE_W_ACK_BODY                 *SendMessageWAckBody;
  EFI_HANDLE                                        HeciHandle;
  UINT32                                            SecMode;

  //
  // If input is invalid, stop processing this SMI
  //
  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return Status;
  }

  SecMode  = HeciPciRead32 (R_SEC_FW_STS0);
  if (SEC_MODE_NORMAL != (SecMode & 0xF0000)) {
    return EFI_UNSUPPORTED;
  }

  SmmHeciFunctionHeader = (VOID *) CommBuffer;
  switch (SmmHeciFunctionHeader->Function) {
    case SMM_HECI_FUNCTION_SEND_MESSAGE_WITH_ACK:
      DEBUG ((EFI_D_ERROR, "SMM_HECI_FUNCTION_SEND_MESSAGE_WITH_ACK \n"));
      if (!((CommandDataBufferHead == CommandDataBufferTail) && mHeci_Complete)) {
        Status = EFI_NOT_READY;
        DEBUG ((EFI_D_ERROR, "SMM_HECI_FUNCTION_SEND_MESSAGE_WITH_ACK EFI_NOT_READY\n"));
        break;
      }
      SendMessageWAckBody = (SMM_HECI_SEND_MESSAGE_W_ACK_BODY *) SmmHeciFunctionHeader->Data;
      Status = Heci2SendwACK (
                 (UINT32 *) SendMessageWAckBody->MessageData,
                 SendMessageWAckBody->Length,
                 &SendMessageWAckBody->RecLength,
                 (UINT8) SendMessageWAckBody->HostAddress,
                 (UINT8) SendMessageWAckBody->CSEAddress
                 );
      DEBUG ((EFI_D_ERROR, "SMM_HECI_FUNCTION_SEND_MESSAGE_WITH_ACK %x %x\n", Status, *(UINT32*)SendMessageWAckBody->MessageData));
      break;
    case SMM_HECI_FUNCTION_READ_MESSAGE:
      ReadMessageBody = (SMM_HECI_READ_MESSAGE_BODY *) SmmHeciFunctionHeader->Data;
      Status = EfiHeciReadMessage (
                 HECI2_DEVICE,
                 0,
                 &ReadMessageBody->Length,
                 (UINT32 *) &ReadMessageBody->MessageData
                 );
      break;
    case SMM_HECI_FUNCTION_SEND_MESSAGE:
      SetHeci2Active ();
      SendMessageBody = (SMM_HECI_SEND_MESSAGE_BODY *) SmmHeciFunctionHeader->Data;
      HeciSendInterrupt (SendMessageBody->MessageData, SendMessageBody->Length, (UINT8)SendMessageBody->HostAddress, (UINT8)SendMessageBody->CSEAddress);
      break;

    case SMM_HECI_FUNCTION_GET_STATUS:
      if (IsHeci2Idle ()) {
        SmmHeciFunctionHeader->Data[0] = 0;
      } else {
        if (HeciInterruptOccur () || IsHeciCseResponse ()) {
          HeciInterruptHandle ();
        }
        if (InBXTBlackHolePhase ()) {
          SmmHeciFunctionHeader->Data[0] = 0;
        } else {
          if ((CommandDataBufferHead == CommandDataBufferTail) && mHeci_Complete) {
            SmmHeciFunctionHeader->Data[0] = 0;
            SetHeci2Idle ();
          } else {
            SmmHeciFunctionHeader->Data[0] = 1;
          }
        }
      }
      break;
    case SMM_HECI_MESSAGE_END_OF_POST:
      DEBUG ((EFI_D_INFO, "HeciSmmHandler SMM_HECI_MESSAGE_END_OF_POST\n"));
      InitSystemResetHandle ();
      HeciHandle = NULL;
      Status = gSmst->SmmInstallProtocolInterface (
                        &HeciHandle,
                        &gEfiCseEndofPostGuid,
                        EFI_NATIVE_INTERFACE,
                        NULL
                        );
      break;
    case SMM_HECI_MESSAGE_END_OF_SERVICES:
      DEBUG ((EFI_D_INFO, "HeciSmmHandler SMM_HECI_MESSAGE_END_OF_SERVICES\n"));
      HeciHandle = NULL;
      Status = gSmst->SmmInstallProtocolInterface (
                        &HeciHandle,
                        &gEfiCseEndofServicesGuid,
                        EFI_NATIVE_INTERFACE,
                        NULL
                        );
      mAtRuntime = TRUE;
      break;
    default:
      Status = EFI_UNSUPPORTED;
  }
  SmmHeciFunctionHeader->ReturnStatus = Status;

  return EFI_SUCCESS;
}


/**
  SmmEndOfDxeCallback

  @param[in]  Protocol
  @param[in]  Interface
  @param[in]  Handle

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
SmmEndOfDxeCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  return EFI_SUCCESS;
}


/**
  Send Heci message with response.

  @param[in]       HeciDev         HECI device
  @param[in, out]  Message         Message Data
  @param[in, out]  Length          Message Data Length
  @param[in, out]  RecLength       Return message buffer length
  @param[in]       HostAddress     Host Address
  @param[in]       SECAddress      CSE Address

  @retval          EFI_SUCCESS     Send message success.
  @retval          Others          Send message failed.

**/
EFI_STATUS
EfiHeciSendwack (
  IN      HECI_DEVICE      HeciDev,
  IN OUT  UINT32           *Message,
  IN OUT  UINT32           Length,
  IN OUT  UINT32           *RecLength,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  )
{
  EFI_STATUS    Status;

  SetHeci2Active ();
  Status = Heci2SendwACK (
             Message,
             Length,
             RecLength,
             HostAddress,
             SECAddress
             );
  SetHeci2Idle ();

  return Status;
}


/**
  Read the Heci message

  @param[in]      HeciDev         HECI Device ID.
  @param[in]      Blocking        Indicate does waiting for response.
  @param[in]      MessageBody     Message data buffer.
  @param[in, out] Length          Message data buffer size.

  @retval         EFI_SUCCESS     Send message success.
  @retval         Others          Send message failed.

**/
EFI_STATUS
EfiHeciReadMessage (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           Blocking,
  IN      UINT32           *MessageBody,
  IN OUT  UINT32           *Length
  )
{
  if (IsHeciCseResponse ()) {
    HeciInterruptHandle ();
    return EFI_SUCCESS;
  }

  return EFI_NOT_READY;
}


/**
  Send Heci message without response.

  @param[in]    HeciDev       HECI Device ID.
  @param[in]    Message       Message Data.
  @param[in]    Length        Message Data length.
  @param[in]    HostAddress   Host Address.
  @param[in]    SECAddress    CSE Address.

  @retval       EFI_SUCCESS   Send message success.
  @retval       Others        Send message failed.

**/
EFI_STATUS
EfiHeciSendMessage (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           *Message,
  IN      UINT32           Length,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  )
{
  SetHeci2Active ();
  HeciSendInterrupt ((UINT8 *) Message, Length, HostAddress, SECAddress);

  return EFI_SUCCESS;
}


/**
  Reset the heci device.

  @param[in]  HeciDev        HECI Device ID.

  @retval     EFI_SUCCESS    Reset HECI success.
  @retval     Others         Reset HECI failed.

**/
EFI_STATUS
EfiHeciReset (
  IN      HECI_DEVICE      HeciDev
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Init the heci device.

  @param[in]   HeciDev       HECI Device ID.

  @retval      EFI_SUCCESS   Init HECI success.
  @retval      Others        Init HECI failed.

**/
EFI_STATUS
EfiHeciInit (
  IN    HECI_DEVICE      HeciDev
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Reinit the heci device.

  @param[in]   HeciDev       HECI Device ID.

  @retval      EFI_SUCCESS   Reinit HECI success.
  @retval      Others        Reinit HECI failed.

**/
EFI_STATUS
EfiHeciReinit (
  IN    HECI_DEVICE      HeciDev
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Reset the Heci device and waiting for Delay time.

  @param[in]   HeciDev       HECI Device ID.
  @param[in]   Delay         The time waiting for reset.

  @retval      EFI_SUCCESS   Reset success.
  @retval      Others        Reset failed.

**/
EFI_STATUS
EfiHeciResetWait (
  IN    HECI_DEVICE      HeciDev,
  IN    UINT32           Delay
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Get the Heci status.

  @param[out]  Status        HECI Staus.

  @retval      EFI_SUCCESS   Get status success.
  @retval      Others        Get status failed.

**/
EFI_STATUS
EfiHeciGetSecStatus (
  IN UINT32                       *Status
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Get the heci mode.

  @param[in]   HeciDev       HECI Device ID.
  @param[out]  Mode          Heci Mode

  @retval      EFI_SUCCESS   Get mode success.
  @retval      Others        Get mode failed.

**/
EFI_STATUS
EfiHeciGetSecMode (
  IN HECI_DEVICE      HeciDev,
  IN UINT32           *Mode
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Return whether at runtime.

  @param    None.

  @retval   TRUE    If at runtime.
            FALSE   Not at runtime.

**/
BOOLEAN
EFIAPI
AtRuntime (
  VOID
  )
{
  return mAtRuntime;
}


EFI_HECI_PROTOCOL mHeciSmmProtocol = {
  EfiHeciSendwack,
  EfiHeciReadMessage,
  EfiHeciSendMessage,
  EfiHeciReset,
  EfiHeciInit,
  EfiHeciResetWait,
  EfiHeciReinit,
  EfiHeciGetSecStatus,
  EfiHeciGetSecMode
};

EFI_HECI2_PM_PROTOCOL mHeci2PmSmmProtocol = {
  IsHeci2Idle,
  SetHeci2Active,
  SetHeci2Idle,
  GetHeci2Bar,
  SetHeci2Bar,
  AtRuntime
};

/**
  SmmPlatformHeci2ProxyHandler

  @param[in]      DispatchHandle
  @param[in]      RegisterContext
  @param[in,out]  CommBuffer
  @param[in,out]  CommBufferSize

  @retval         EFI_STATUS

**/
EFI_STATUS
SmmPlatformHeci2ProxyHandler (
  IN     EFI_HANDLE                 DispatchHandle,
  IN     CONST VOID                 *RegisterContext,
  IN OUT VOID                       *CommBuffer,
  IN OUT UINTN                      *CommBufferSize
  )
{
  HeciInterruptInit ();
  if (HeciInterruptOccur ()) {
    HeciInterruptHandle ();
  }

  return EFI_SUCCESS;
}


/**
  Variable Driver main entry point. The Variable driver places the 4 EFI
  runtime services in the EFI System Table and installs arch protocols
  for variable read and write services being available. It also registers
  a notification function for an EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval    EFI_SUCCESS    Variable service successfully initialized.

**/
EFI_STATUS
EFIAPI
HeciSmmInitialize (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              HeciHandle;
  EFI_HANDLE                              SmmHandle;
  VOID                                    *SmmEndOfDxeRegistration;
  MBP_SMM_TRUSTED_KEY                     SmmTrustedKey;

  //
  // Install the Smm Variable Protocol on a new handle.
  //
  if (Heci2PciRead16 (R_SEC_DevID_VID) == 0xFFFF) {
    return EFI_SUCCESS;
  }

  if (mMonoCount == 0) {
    //
    // Since the is the first message, get the SMM trusted key for this boot
    //
    Status = HeciGetSMMTrustedKey (&SmmTrustedKey);

    if (!EFI_ERROR (Status)) {
      CopyMem (mHmacSha256Key, SmmTrustedKey.SmmTrustedKey, sizeof (mHmacSha256Key));
      mMonoCount = SmmTrustedKey.MonotonicCounter + 1;
      DEBUG ((EFI_D_INFO, "HMAC Monotonic Count = %d\n", mMonoCount));
    } else {
      DEBUG ((EFI_D_ERROR, "Unable to get the SMM trusted key. Cannot send HECI2 transactions.\n"));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
  }

  //
  // Install the Smm Variable Protocol on a new handle.
  //
  HeciHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &HeciHandle,
                    &gEfiHeciSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mHeciSmmProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  HeciHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &HeciHandle,
                    &gEfiHeci2PmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mHeci2PmSmmProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register SMM variable SMI handler
  //
  Status = gSmst->SmiHandlerRegister (HeciSmmHandler, &gEfiHeciSmmProtocolGuid, &HeciHandle);
  ASSERT_EFI_ERROR (Status);

  SmmHandle = NULL;
  Status = gSmst->SmiHandlerRegister (SmmPlatformHeci2ProxyHandler, NULL, &SmmHandle);

  //
  // Register EFI_SMM_END_OF_DXE_PROTOCOL_GUID notify function.
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmEndOfDxeProtocolGuid,
                    SmmEndOfDxeCallback,
                    &SmmEndOfDxeRegistration
                    );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, " Start Init Heci2 SMI Interrupt\n"));

  PERF_START_EX (NULL, NULL, NULL, 0, 0x8100);
  HeciReset (HECI2_DEVICE);
  PERF_END_EX (NULL, NULL, NULL, 0, 0x8101);

  HeciInterruptInit ();
  HeciDisableInterrupt ();
  HeciClearInterrupt ();
  HeciEnableInterrupt ();
  DEBUG ((EFI_D_INFO, "Start set HIDM %x %x\n", Heci2PciRead16(0), Heci2PciRead16(2)));
  Heci2PciWrite32 (0xa0, 2);

  return EFI_SUCCESS;
}


