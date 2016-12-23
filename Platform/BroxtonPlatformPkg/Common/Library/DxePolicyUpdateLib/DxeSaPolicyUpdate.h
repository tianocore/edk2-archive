/** @file
  Header file for SA DXE Policy initialzation.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DXE_SA_POLICY_UPDATE_H_
#define _DXE_SA_POLICY_UPDATE_H_

#include <PiDxe.h>
#include <CpuRegs.h>
#include <Guid/SaDataHob.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SaPolicy.h>
#include <Library/MmPciLib.h>

#if defined (ENBDT_PF_ENABLE) && (ENBDT_PF_ENABLE == 1) // For ApolloLake
#include <Protocol/PlatformGopPolicy.h>
#endif

#include <Library/ConfigBlockLib.h>

#endif

