/** @file
  Policy definition for Internal Graphics Config Block.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _GRAPHICS_CONFIG_H_
#define _GRAPHICS_CONFIG_H_

#pragma pack(1)

#define GRAPHICS_CONFIG_REVISION 1

extern EFI_GUID gGraphicsConfigGuid;

typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Offset 0-23 Config Block Header
  UINT32     CdClock            : 3;
  UINT32     PeiGraphicsPeimInit: 1;   ///< 0- Disable 1- Enable
  UINT32     RsvdBits0          : 4;   ///< Reserved for future use
  UINT8      Rsvd1[2];                 ///< Offset 14
  UINT32     GttMmAdr;                 ///< Offset 16 Address of System Agent GTTMMADR: Default is <b>0xDF000000</b>
  VOID*      LogoPtr;                  ///< Offset 20 Address of Logo to be displayed in PEI
  UINT32     LogoSize;                 ///< Offset 24 Logo Size
  VOID*      GraphicsConfigPtr;        ///< Offset 28 Address of the Graphics Configuration Table
  UINT32     GmAdr;                    ///< Offset 32 Address of System Agent GMADR: Default is <b>0xC0000000</b>
  UINT8      PmSupport;
  UINT8      PavpEnable;
  UINT8      EnableRenderStandby;
  UINT8      PavpPr3;
  UINT8      ForceWake;
  UINT8      PavpLock;
  UINT8      GraphicsFreqModify;
  UINT8      GraphicsFreqReq;
  UINT8      GraphicsVideoFreq;
  UINT8      PmLock;
  UINT8      DopClockGating;
  UINT8      UnsolicitedAttackOverride;
  UINT8      WOPCMSupport;
  UINT8      WOPCMSize;
  UINT8      PowerGating;
  UINT8      UnitLevelClockGating;
} GRAPHICS_CONFIG;

#pragma pack()

#endif // _GRAPHICS_CONFIG_H_

