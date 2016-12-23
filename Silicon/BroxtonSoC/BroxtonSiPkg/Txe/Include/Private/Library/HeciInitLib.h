/** @file
  Header File for HECI Init Lib functionality.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _HECI_INIT_LIB_H
#define _HECI_INIT_LIB_H

#define B_EXCLUSION BIT8

UINT32
MmIoReadDword (
  UINTN a
  );

VOID
MmIoWriteDword (
  UINTN  a,
  UINT32 b
  );

//
// Function Prototype declarations
//
EFI_STATUS
WaitForSECReady (
  VOID
  );

EFI_STATUS
EFIAPI
HeciReset (
  IN      HECI_DEVICE      HeciDev
  );

EFI_STATUS
  HeciTakeOwnerShip (
  );

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in]  HECI Device

  @retval     EFI_STATUS

**/
EFI_STATUS
InitializeHeciPrivate (
  IN HECI_DEVICE                  HeciDev
  );


/**
  CheckAndFixHeciForAccess
  This function gets HECI device PCI base address and checks for HECI device availability and provides HECI Device MBAR
  after enabling Device specific BME, MSE and SERR.

  @param[in]  HeciDev     HECI Device Number

  @retval     Corresponding HECI Device MBAR

**/
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE       HeciDev
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

EFI_STATUS
EFIAPI
Heci2SendwoACK (
  IN    OUT     UINT32  *Message,
  IN    UINT32  Length,
  IN    OUT     UINT32  *RecLength,
  IN    UINT8   HostAddress,
  IN    UINT8   SeCAddress
  );

EFI_STATUS
EFIAPI
HeciSendwoACK (
  IN OUT  UINT32  *Message,
  IN      UINT32  Length,
  IN OUT  UINT32  *RecLength,
  IN      UINT8   HostAddress,
  IN      UINT8   SeCAddress
  );

EFI_STATUS
HeciGetBootDevice (
  MBP_CURRENT_BOOT_MEDIA    *BootMediaData
  );

/**
  HeciMBP

  @param[in]  MBPData

  @return    EFI_STATUS

**/
EFI_STATUS
HeciMBP (
  UINT32 * MBPData
  );

/**
  DumpBuffer_HECI

  @param[in] Buffer1

  @return    None

**/
VOID
DumpBuffer_HECI (
  VOID *Buffer1,
  UINT8 Buffersize
  );

//
// DXE-HECI Protocol Function Prototype declarations
//
/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in,out]  Message       Pointer to the message buffer.
  @param[in]      SendLength    Length of the message in bytes.
  @param[in,out]  RecLength     Length of the message response in bytes.
  @param[in]      HostAddress   Address of the sending entity.
  @param[in]      SeCAddress    Address of the ME entity that should receive the message.

  @retval         EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendwACK (
  IN      HECI_DEVICE   HeciDev,
  IN OUT  UINT32        *Message,
  IN OUT  UINT32        Length,
  IN OUT  UINT32        *RecLength,
  IN      UINT8         HostAddress,
  IN      UINT8         SeCAddress
  );

/**
  Reads a message from the SEC across HECI.

  @param[in]      Blocking      Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out]     MessageData   Pointer to a buffer used to receive a message.
  @param[in,out]  Length        Pointer to the length of the buffer on input and the length
                                of the message on return. (in bytes)

  @retval         EFI_STATUS

**/

EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           Blocking,
  IN OUT  UINT32           *MessageBody,
  IN OUT  UINT32           *Length
  );

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in]  Message       Pointer to the message data to be sent.
  @param[in]  Length        Length of the message in bytes.
  @param[in]  HostAddress   The address of the host processor.
  @param[in]  SeCAddress    Address of the ME subsystem the message is being sent to.

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSend (
  IN     HECI_DEVICE    HeciDev,
  IN     UINT32        *Message,
  IN     UINT32         Length,
  IN     UINT8          HostAddress,
  IN     UINT8          SeCAddress
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm

  @param[in]  none

  @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  IN      HECI_DEVICE      HeciDev
  );

//
// Prototypes
//
/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] None

  @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN  HECI_DEVICE      HeciDev
  );

/**
  Heci Re-initializes it for Host

  @param[in] None

  @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  IN      HECI_DEVICE      HeciDev
  );

/**
  SeC reset and waiting for ready

  @param[in]  Delay              The biggest waiting time

  @retval     EFI_TIMEOUT        Time out
  @retval     EFI_SUCCESS        SeC ready

**/
EFI_STATUS
EFIAPI
SeCResetWait (
  IN  HECI_DEVICE   HeciDev,
  IN  UINT32        Delay
  );

/**
  Return SEC Status.

  @param[in]  SeCStatus        Pointer for status report

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciGetSeCStatus (
  IN UINT32                     *SeCStatus
  );

/**
  Return SEC Status

  @param[in]  SeCStatus        Pointer for status report

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciGetSeCMode (
  IN  HECI_DEVICE               HeciDev,
  IN UINT32                     *SeCMode
  );

/**
  Function sends bios2ish bin file to CSE through HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in]      Message        Pointer to the send message buffer.
  @param[out]     Message        Pointer to the receive message buffer.
  @param[in]      SendLength     Length of the message in bytes.
  @param[in, out] RecLength      Length of the message response in bytes.
  @param[in]      HostAddress    Address of the sending entity.
  @param[in]      SeCAddress     Address of the ME entity that should receive the message.

  @retval         EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciIshSendwAck (
  IN   HECI_DEVICE   HeciDev,
  IN   VOID          *SendMessage,
  OUT  VOID          *ReceiveMessage,
  IN   UINT32        SendLength,
  IN   OUT  UINT32   *RecLength,
  IN   UINT8      HostAddress,
  IN   UINT8      SeCAddress
  );

#endif

