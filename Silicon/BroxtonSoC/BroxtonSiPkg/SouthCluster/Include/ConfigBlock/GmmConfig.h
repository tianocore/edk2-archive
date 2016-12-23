/** @file
  GMM(Gaussian Mixture Model) scoring accelerator policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _GMM_CONFIG_H_
#define _GMM_CONFIG_H_

#define GMM_CONFIG_REVISION 1

extern EFI_GUID gGmmConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to GMM.

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                           ///< Config Block Header
  UINT32               Enable                   : 1;     ///< <b>1: Enable</b>, 0: Disable
  UINT32               ClkGatingPgcbClkTrunk    : 1;     ///< GMM Clock Gating  - PGCB Clock Trunk: 0: Disable, 1: Enable;
  UINT32               ClkGatingSb              : 1;     ///< GMM Clock Gating  - Sideband: 0: Disable, 1: Enable;
  UINT32               ClkGatingSbClkTrunk      : 1;     ///< GMM Clock Gating  - Sideband Clock Trunk: 0: Disable, 1: Enable;
  UINT32               ClkGatingSbClkPartition  : 1;     ///< GMM Clock Gating  - Sideband Clock Partition: 0: Disable, 1: Enable;
  UINT32               ClkGatingCore            : 1;     ///< GMM Clock Gating  - Core: 0: Disable, 1: Enable;
  UINT32               ClkGatingDma             : 1;     ///< GMM Clock Gating  - DMA: 0: Disable, 1: Enable;
  UINT32               ClkGatingRegAccess       : 1;     ///< GMM Clock Gating  - Register Access: 0: Disable, 1: Enable;
  UINT32               ClkGatingHost            : 1;     ///< GMM Clock Gating  - Host: 0: Disable, 1: Enable;
  UINT32               ClkGatingPartition       : 1;     ///< GMM Clock Gating  - Partition: 0: Disable, 1: Enable;
  UINT32               ClkGatingTrunk           : 1;     ///< GMM Clock Gating  - Trunk: 0: Disable, 1: Enable;
  UINT32               SvPwrGatingHwAutoEnable  : 1;     ///< GMM Power Gating  - HW Autonomous Enabled: 0: Disable, 1: Enable;
  UINT32               SvPwrGatingD3HotEnable   : 1;     ///< GMM Power Gating  - D3 Hot Enabled: 0: Disable, 1: Enable;
  UINT32               SvPwrGatingI3Enable      : 1;     ///< GMM Power Gating  - I3 Enabled: 0: Disable, 1: Enable;
  UINT32               SvPwrGatingPmcReqEnable  : 1;     ///< GMM Power Gating  - PMC Request Enabled: 0: Disable, 1: Enable;
  UINT32               RsvdBits                 : 17;    ///< Reserved bits
} SC_GMM_CONFIG;

#pragma pack (pop)

#endif // _GMM_CONFIG_H_

