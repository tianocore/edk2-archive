/** @file
  Policy definition of IGD Panel Config Block.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IGD_PANEL_CONFIG_H_
#define _IGD_PANEL_CONFIG_H_

#pragma pack(1)

#define IGD_PANEL_CONFIG_REVISION 1

extern EFI_GUID gIgdPanelConfigGuid;

//
// IGD Panel Features
//
typedef struct {
    CONFIG_BLOCK_HEADER  Header;         ///< Offset 0-23 Config Block Header
    UINT8                PFITStatus;
    UINT8                IgdThermalSupport;
    UINT8                ALSEnabled;
#if (ENBDT_PF_ENABLE == 1)
    UINT8                PanelSelect;   ///< 0=eDP, >=1 for MIPI
#endif
} IGD_PANEL_CONFIG;

#pragma pack()

#endif // _IGD_PANEL_CONFIG_H_

