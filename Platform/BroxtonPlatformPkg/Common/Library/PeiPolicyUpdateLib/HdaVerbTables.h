/** @file
  Header file for HDA Verb Tables.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HDA_VERBTABLES_H_
#define _HDA_VERBTABLES_H_

#include <Ppi/ScPolicy.h>

enum HDAUDIO_CODEC_SELECT {
  HdaCodecPlatformOnboard = 0,
  HdaCodecExternalKit     = 1
};

extern HDAUDIO_VERB_TABLE  HdaVerbTableAlc298;
extern HDAUDIO_VERB_TABLE  HdaVerbTableAlc662;

#endif // _HDA_VERBTABLES_H_

