/** @file
  EFI HECI PPI.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_PPI_H
#define _HECI_PPI_H

#include <SeCState.h>
#include <Protocol/Heci.h>

typedef struct _EFI_HECI_PPI EFI_HECI_PPI;

///
/// The interface functions are for sending/receiving HECI messages between host and CSE in PEI phase.
///
typedef struct _EFI_HECI_PPI {
  EFI_HECI_SENDWACK        SendwACK;
  EFI_HECI_READ_MESSAGE    ReadMsg;
  EFI_HECI_SEND_MESSAGE    SendMsg;
  EFI_HECI_RESET           ResetHeci;
  EFI_HECI_INIT            InitHeci;
  EFI_HECI_RESET_WAIT      SeCResetWait;
  EFI_HECI_REINIT          ReInitHeci;
  EFI_HECI_GET_SEC_STATUS  GetSeCStatus;
  EFI_HECI_GET_SEC_MODE    GetSeCMode;
} EFI_HECI_PPI;

extern EFI_GUID gEfiHeciPpiGuid;

#endif // _HECI_PPI_H

