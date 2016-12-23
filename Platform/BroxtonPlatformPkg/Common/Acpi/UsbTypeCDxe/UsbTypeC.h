/** @file
  Header file for implements Overclocking Interface for OS Application ie Iron city.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __USBTYPEC__H__
#define __USBTYPEC__H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <PiDxe.h>

#pragma pack (1)
typedef struct  {
  UINT16   Version;         // PPM -> OPM, Version JJ.M.N (JJ - major version, M - minor version, N - sub-minor version). Version 2.1.3 is represented with value 0x0213
  UINT16   Reserved;        // Reversed
  UINT8    CCI[4];          // PPM->OPM CCI indicator
  UINT8    Control[8];      // OPM->PPM Control message
  UINT8    MsgIn[16];       // PPM->OPM Message In, 128 bits, 16 bytes
  UINT8    MsgOut[16];      // OPM->PPM Message Out
} USBTYPEC_OPM_BUF ;

#pragma pack ()

#ifdef __cplusplus
}
#endif
#endif

