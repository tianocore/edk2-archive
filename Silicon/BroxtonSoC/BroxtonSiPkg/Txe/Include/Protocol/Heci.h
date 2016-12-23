/** @file
  EFI HECI Protocol.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_HECI_H
#define _EFI_HECI_H

#include <SeCState.h>
#include <SeCChipset.h>

typedef struct _EFI_HECI_PROTOCOL EFI_HECI_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_SENDWACK) (
  IN      HECI_DEVICE      HeciDev,
  IN OUT  UINT32           *Message,
  IN OUT  UINT32           Length,
  IN OUT  UINT32           *RecLength,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_READ_MESSAGE) (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           Blocking,
  IN      UINT32           *MessageBody,
  IN OUT  UINT32           *Length
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_SEND_MESSAGE) (
  IN      HECI_DEVICE      HeciDev,
  IN      UINT32           *Message,
  IN      UINT32           Length,
  IN      UINT8            HostAddress,
  IN      UINT8            SECAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_RESET) (
  IN      HECI_DEVICE      HeciDev
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_INIT) (
   IN  HECI_DEVICE      HeciDev
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_REINIT) (
  IN      HECI_DEVICE      HeciDev
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_RESET_WAIT) (
  IN    HECI_DEVICE      HeciDev,
  IN    UINT32           Delay
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_GET_SEC_STATUS) (
  IN UINT32                       *Status
  );

typedef
EFI_STATUS
(EFIAPI *EFI_HECI_GET_SEC_MODE) (
  IN HECI_DEVICE               HeciDev,
  IN UINT32                    *Mode
  );

typedef struct _EFI_HECI_PROTOCOL {
  EFI_HECI_SENDWACK        SendwACK;
  EFI_HECI_READ_MESSAGE    ReadMsg;
  EFI_HECI_SEND_MESSAGE    SendMsg;
  EFI_HECI_RESET           ResetHeci;
  EFI_HECI_INIT            InitHeci;
  EFI_HECI_RESET_WAIT      SeCResetWait;
  EFI_HECI_REINIT          ReInitHeci;
  EFI_HECI_GET_SEC_STATUS  GetSeCStatus;
  EFI_HECI_GET_SEC_MODE    GetSeCMode;
} EFI_HECI_PROTOCOL;

extern EFI_GUID gEfiHeciProtocolGuid;
extern EFI_GUID gEfiHeciSmmProtocolGuid;
extern EFI_GUID gEfiHeciSmmRuntimeProtocolGuid;
extern EFI_GUID gEfiCseEndofPostGuid;
extern EFI_GUID gEfiCseEndofServicesGuid;

#endif // _EFI_HECI_H

