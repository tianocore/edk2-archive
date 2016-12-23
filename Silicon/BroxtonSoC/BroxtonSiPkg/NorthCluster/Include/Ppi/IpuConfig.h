/** @file
  IPU policy definitions.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IPU_CONFIG_H_
#define _IPU_CONFIG_H_

#pragma pack(1)

#define IPU_CONFIG_REVISION 1

extern EFI_GUID gIpuConfigGuid;

typedef struct {
  CONFIG_BLOCK_HEADER  Header;         ///< Offset 0-23 Config Block Header
  UINT8                SaIpuEnable;    ///< Enable SA IPU device: 0=Disable, <b>1=Enable</b>
  UINT8                IpuAcpiMode;    ///< Set IPU ACPI mode: <b>0=AUTO</b>, 1=IGFX Child device, 2=ACPI device
  UINT32               IpuMmAdr;       ///< Address of IPU MMIO Bar IpuMmAdr: Default is <b>0x90000000</b>
} IPU_CONFIG;

#pragma pack()

#endif // _IPU_CONFIG_H_

