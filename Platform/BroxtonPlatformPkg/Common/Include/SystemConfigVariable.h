/** @file
  Definition of SystemConfig EFI variable.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SETUP_CONFIG_VARIABLE_H_
#define _SETUP_CONFIG_VARIABLE_H_

#ifndef SYSTEM_CONFIG_NAME
#define SYSTEM_CONFIG_NAME         L"SystemConfig"
#endif

extern EFI_GUID gSystemConfigGuid;

typedef struct {
    UINT8   Revision;
    UINT8   HiiExport:1;
    UINT8   Reserved:7;      // Reserved for Future Use
} SYSTEM_CONFIG;

#endif

