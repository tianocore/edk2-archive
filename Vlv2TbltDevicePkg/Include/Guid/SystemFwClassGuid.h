/** @file
  System FW Class guid

  Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#ifndef _SYSTEM_FW_CLASS_GUID_H_
#define _SYSTEM_FW_CLASS_GUID_H_

//
// GUID definition for System FW Class 
//
#define SYSTEM_FW_CLASS_GUID \
  { \
    0x819b858e, 0xc52c, 0x402f, { 0x80, 0xe1, 0x5b, 0x31, 0x1b, 0x6c, 0x19, 0x59 }\
  }

extern EFI_GUID gSystemFwClassGuid;
#endif
