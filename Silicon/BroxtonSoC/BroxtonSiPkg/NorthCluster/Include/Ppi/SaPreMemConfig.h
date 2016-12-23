/** @file
  Policy definition for SA Pre-Mem Config Block.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_PRE_MEM_CONFIG_H_
#define _SA_PRE_MEM_CONFIG_H_

#pragma pack(1)

#define SA_PRE_MEM_CONFIG_REVISION 1

extern EFI_GUID gSaPreMemConfigGuid;

typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Offset 0-23 Config Block Header
  UINT8                 IgdDvmt50PreAlloc;
  UINT8                 PrimaryDisplay;
  UINT8                 ApertureSize;
  UINT8                 InternalGraphics;
  UINT16                GttSize;                  ///< Selection of iGFX GTT Memory size: 1=2MB, 2=4MB, <b>3=8MB</b>
} SA_PRE_MEM_CONFIG;

#pragma pack()

#endif // _SA_PRE_MEM_CONFIG_H_

