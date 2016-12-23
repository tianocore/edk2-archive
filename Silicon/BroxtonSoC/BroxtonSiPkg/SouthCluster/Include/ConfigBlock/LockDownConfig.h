/** @file
  Lock down policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _LOCK_DOWN_CONFIG_H_
#define _LOCK_DOWN_CONFIG_H_

#define LOCK_DOWN_CONFIG_REVISION 2

extern EFI_GUID gLockDownConfigGuid;

#pragma pack (push,1)

typedef struct {

  CONFIG_BLOCK_HEADER  Header;                        ///< Config Block Header
  UINT32               GlobalSmi          :  1;
  UINT32               BiosInterface      :  1;
  UINT32               RtcLock            :  1;
  UINT32               BiosLock           :  1;
  UINT32               SpiEiss            :  1;
  UINT32               BiosLockSwSmiNumber:  8;
  UINT32               TcoLock            :  1;
  UINT32               RsvdBits0          :  18;          ///< Reserved bits
} SC_LOCK_DOWN_CONFIG;

#pragma pack (pop)

#endif // _LOCK_DOWN_CONFIG_H_

