/** @file
  VT-d policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VTD_CONFIG_H_
#define _VTD_CONFIG_H_

#define VTD_CONFIG_REVISION 1

extern EFI_GUID gVtdConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to VT-d.
**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                      ///< Config Block Header
  UINT32               VtdEnable           :  1;    ///< 1: Enable, <b>0: Disable</b> VT-d
  UINT8                x2ApicEnabled       :  1;
  UINT32               RsvdBits            :  30;   ///< Reserved bits
  UINT64               RmrrUsbBaseAddr;
  UINT64               RmrrUsbLimit;
} SC_VTD_CONFIG;

#pragma pack (pop)

#endif // _VTD_CONFIG_H_

