/** @file
  This file defines the PowerMgmtInitDone Protocol.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POWER_MGMT_INIT_DONE_H_
#define _POWER_MGMT_INIT_DONE_H_

///
/// Define PPM INFO protocol GUID
///
#define POWER_MGMT_INIT_DONE_PROTOCOL_GUID \
  { \
    0xd71db106, 0xe32d, 0x4225, 0xbf, 0xf4, 0xde, 0x6d, 0x77, 0x87, 0x17, 0x61 \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gPowerMgmtInitDoneProtocolGuid;

typedef struct _EFI_POWER_MGMT_INIT_DONE_PROTOCOL EFI_POWER_MGMT_INIT_DONE_PROTOCOL;

#endif

