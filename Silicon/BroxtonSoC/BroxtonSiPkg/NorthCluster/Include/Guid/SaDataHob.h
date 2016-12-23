/** @file
  The GUID definition for SaDataHob.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_DATA_HOB_H_
#define _SA_DATA_HOB_H_

#include <SaAccess.h>
#include <Base.h>

extern EFI_GUID gSaDataHobGuid;

#pragma pack(1)

//
// HG GPIO Data Structure
//
typedef struct {
  UINT32   CommunityOffset;  ///< GPIO Community
  UINT16   PinOffset;        ///< GPIO Pin
  BOOLEAN  Active;           ///< 0=Active Low; 1=Active High
} HG_GPIO_INFO;

//
// HG Info HOB
//
typedef struct _HG_INFO_HOB {
  UINT8                 RootPortDev;              ///< Device number used for SG
  UINT8                 RootPortFun;              ///< Function number used for SG
  UINT8                 HgEnabled;                ///< HgEnabled (0=Disabled, 1=Enabled)
  UINT16                HgSubSystemId;            ///< Hybrid Graphics Subsystem ID
  UINT16                HgDelayAfterPwrEn;        ///< Dgpu Delay after Power enable using Setup option
  UINT16                HgDelayAfterHoldReset;    ///< Dgpu Delay after Hold Reset using Setup option
  HG_GPIO_INFO          HgDgpuHoldRst;            ///< This field contain dGPU HLD RESET GPIO value and level information
  HG_GPIO_INFO          HgDgpuPwrEnable;          ///< This field contain dGPU_PWR Enable GPIO value and level information
} HG_INFO_HOB;

//
// System Agent Data Hob
//
typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;        ///< GUID Hob type structure
  UINT8                 IpuAcpiMode;           ///< IPU ACPI mode: 0=Disabled, 1=IGFX Child device, 2=ACPI device
  HG_INFO_HOB           HgInfo;                ///< HG Info HOB
} SA_DATA_HOB;

#pragma pack()
#endif

