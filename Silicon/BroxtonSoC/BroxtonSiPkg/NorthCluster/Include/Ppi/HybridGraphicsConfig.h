/** @file
  Hybrid Graphics policy definitions.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HYBRID_GRAPHICS_CONFIG_H_
#define _HYBRID_GRAPHICS_CONFIG_H_

#pragma pack(1)

#define HYBRID_GRAPHICS_CONFIG_REVISION 1

extern EFI_GUID gHybridGraphicsConfigGuid;

//
// HG GPIO Data Structure
//
typedef struct {
  UINT32   CommunityOffset;  ///< GPIO Community
  UINT16   PinOffset;        ///< GPIO Pin
  BOOLEAN  Active;           ///< 0=Active Low; 1=Active High
} HG_GPIO;

//
// Defines the Switchable Graphics configuration parameters for System Agent.
//
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Offset 0-23 Config Block Header
  UINT8                 RootPortDev;              ///< Device number used for SG
  UINT8                 RootPortFun;              ///< Function number used for SG
  UINT8                 HgEnabled;                ///< HgEnabled (0=Disabled, 1=Enabled)
  UINT16                HgSubSystemId;            ///< Hybrid Graphics Subsystem ID
  UINT16                HgDelayAfterPwrEn;        ///< Dgpu Delay after Power enable using Setup option
  UINT16                HgDelayAfterHoldReset;    ///< Dgpu Delay after Hold Reset using Setup option
  HG_GPIO               HgDgpuHoldRst;            ///< This field contain dGPU HLD RESET GPIO value and level information
  HG_GPIO               HgDgpuPwrEnable;          ///< This field contain dGPU_PWR Enable GPIO value and level information
} HYBRID_GRAPHICS_CONFIG;

#pragma pack()

#endif // _HYBRID_GRAPHICS_CONFIG_H_

