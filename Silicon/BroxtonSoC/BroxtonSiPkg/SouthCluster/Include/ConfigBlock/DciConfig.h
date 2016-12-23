/** @file
  DCI policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DCI_CONFIG_H_
#define _DCI_CONFIG_H_

#define DCI_CONFIG_REVISION 1

extern EFI_GUID gDciConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to Direct Connection Interface (DCI).

**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;         ///< Config Block Header
  /**
    <b>(Test)</b> DCI enable (HDCIEN bit)
    when Enabled, allow DCI to be enabled. When Disabled, the host control is not enabling DCI feature.
    BIOS provides policy to enable or disable DCI, and user would be able to use BIOS option to change this policy.
    The user changing the setting from disable to enable, is taken as a consent from the user to enable this DCI feature.
    <b>0:Disabled</b>; 1:Enabled
  **/
  UINT32    DciEn           :  1;
  /**
    <b>(Test)</b> When set to Auto detect mode, it detects CCA being connected during BIOS post time.
    This policy only applies when DciEn is disabled.
    NOTE: this policy should not be visible to end customer.
    0: Disable AUTO mode, <b>1: Enable AUTO mode</b>
  **/
  UINT32    DciAutoDetect   :  1;
  UINT32    RsvdBits        : 30;       ///< Reserved bits
} SC_DCI_CONFIG;

#pragma pack (pop)

#endif // _DCI_CONFIG_H_

