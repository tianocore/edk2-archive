/** @file
  P2SB policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _P2SB_CONFIG_H_
#define _P2SB_CONFIG_H_

#define P2SB_CONFIG_REVISION 1

extern EFI_GUID gP2sbConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to P2SB Interface.

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                  ///< Config Block Header
  /**
    <b>(Test)</b> P2SB
    <b>0:Hide</b>; 1:UnHide
  **/
  UINT32               P2sbUnhide    : 1;
  UINT32               RsvdBytes     :31;       ///< Reserved bytes
} SC_P2SB_CONFIG;

#pragma pack (pop)

#endif // _P2SB_CONFIG_H_

