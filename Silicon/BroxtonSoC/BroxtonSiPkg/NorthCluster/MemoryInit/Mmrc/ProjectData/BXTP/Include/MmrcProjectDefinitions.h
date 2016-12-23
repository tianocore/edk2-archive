/** @file
  Includes all hard defines specific to a project which are NOT
  outputted by the MMRC tool. Things like the maximum number of
  channels, ranks, DIMMs, etc, should be included here. This file
  needs to be included by most MMRC components, including Mmrc.h.

  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MMRCPROJECTDEFINITIONS_H
#define _MMRCPROJECTDEFINITIONS_H

#include "MmrcProjectDefinitionsGenerated.h"
#include "MrcEfiDefinitions.h"


//
// RMT project specific definitions.
//
#define PSEUDO_RANKS                     0 // PT changed from 2 to 0. Effectively disable it
#define MAX_RMT_ELEMENTS                 7 // PT changed from 10 to 7

#endif // _MMRCPROJECTDEFINITIONS_H

