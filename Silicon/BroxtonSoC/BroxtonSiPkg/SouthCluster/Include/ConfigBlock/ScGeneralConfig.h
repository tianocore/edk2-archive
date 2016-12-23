/** @file
  SC General policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_GENERAL_CONFIG_H_
#define _SC_GENERAL_CONFIG_H_

#define SC_GENERAL_CONFIG_REVISION 1

extern EFI_GUID gScGeneralConfigGuid;

#pragma pack (push,1)

typedef struct {
  CONFIG_BLOCK_HEADER  Header;                     ///< Config Block Header
  UINT16               SubSystemVendorId;          ///< Default Subsystem Vendor ID of the SC devices. Default is <b>0x8086</b>
  UINT16               SubSystemId;                ///< Default Subsystem ID of the SC devices. Default is <b>0x7270</b>
  UINT16               AcpiBase;                   ///< Power management I/O base address. Default is <b>0x400</b>.
  UINT32               PmcBase;                    ///< PMC Base Address.  Default is <b>0xD1001000</b>.
  UINT32               P2sbBase;                   ///< P2SB base Address. Default is <b>0xD0000000</b>.
  UINT32               Crid            :  1;
  UINT32               S0ixSupport     :  1;
  UINT32               ResetSelect     :  4;
  UINT32               RsvdBits0       : 26;       ///< Reserved bits
} SC_GENERAL_CONFIG;

#pragma pack (pop)

#endif // _SC_GENERAL_CONFIG_H_

