/** @file
  Interface definition details between System Agent and platform drivers during DXE phase.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_POLICY_H_
#define _SA_POLICY_H_

#include <ConfigBlock.h>
#include <Protocol/SaDxeMiscConfig.h>
#include <Protocol/IgdPanelConfig.h>

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSaPolicyProtocolGuid;

//
// Protocol revision number
// Any backwards compatible changes to this protocol will result in an update in the revision number
// Major changes will require publication of a new protocol
//
#define SA_POLICY_PROTOCOL_REVISION_1 0

//
// Generic definitions for device enabling/disabling used by NC code.
//
#define DEVICE_ENABLE   1
#define DEVICE_DISABLE  0

#define NO_AUDIO   0
#define HD_AUDIO   1
#define LPE_AUDIO  2

//
// SA Policy Protocol
//
typedef struct _SA_POLICY_PROTOCOL {
  CONFIG_BLOCK_TABLE_HEADER      TableHeader;    ///< Offset 0-31
  //
  // Individual Config Block Structures are added here in memory as part of AddConfigBlock()
  //
} SA_POLICY_PROTOCOL;

#endif

