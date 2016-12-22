/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BIOS_GUARD_CONFIG_H_
#define _BIOS_GUARD_CONFIG_H_

#include "BiosGuardDefinitions.h"

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gBiosGuardConfigGuid;

#pragma pack(push, 1)

#define BIOS_GUARD_CONFIG_REVISION 1

/**
  Platform policies for BIOS Guard Configuration for all processor security features configuration.
  Platform code can pass relevant configuration data through this structure.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;             ///< Offset 0 GUID number for main entry of config block
  BGUP_HEADER          BgupHeader;         ///< BIOS Guard update package header that will be packaged along with BIOS Guard script and update data.
  BGPDT                Bgpdt;              ///< BIOS Guard Platform Data Table contains all the platform data that will be parsed by BIOS Guard module.
  UINT64               BgpdtHash[4];       ///< Hash of the BGPDT that will be programmed to PLAT_FRMW_PROT_HASH_0/1/2/3 MSR.
  UINT8                EcCmdDiscovery;     ///< EC Command discovery.
  UINT8                EcCmdProvisionEav;  ///< EC Command Provision Eav.
  UINT8                EcCmdLock;          ///< EC Command Lock.
  UINT8                Rsvd;               ///< Reserved for DWORD alignment.
  BIOSGUARD_LOG        BiosGuardLog;       ///< BIOS Guard log.
} BIOS_GUARD_CONFIG;

#pragma pack(pop)
#endif

