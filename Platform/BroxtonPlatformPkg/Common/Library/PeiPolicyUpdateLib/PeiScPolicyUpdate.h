/** @file
  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SC_POLICY_UPDATE_H_
#define _PEI_SC_POLICY_UPDATE_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PiPei.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/SteppingLib.h>
#include <ScAccess.h>
#include <Library/ScPlatformLib.h>

//
// Generic definitions for device Auto/enabling/disabling used by platform
//
#define DEVICE_AUTO     2
#define DEVICE_ENABLE   1
#define DEVICE_DISABLE  0

#endif

