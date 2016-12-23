/** @file
  ISH policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _ISH_CONFIG_H_
#define _ISH_CONFIG_H_

#define ISH_CONFIG_REVISION 1

extern EFI_GUID gIshConfigGuid;

#pragma pack (push,1)

//
// The SC_ISH_CONFIG block describes Integrated Sensor Hub device.
//
typedef struct {
  CONFIG_BLOCK_HEADER  Header;              ///< Config Block Header
  UINT32               Enable   :  1;       ///< ISH Controler 0: Disable; <b>1: Enable</b>.
  UINT32               RsvdBits0: 31;       ///< Reserved Bits
} SC_ISH_CONFIG;

#pragma pack (pop)

#endif // _ISH_CONFIG_H_

