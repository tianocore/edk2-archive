/** @file
  Header file for Framework PEIM to SeCUma.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_UMA_H_
#define _SEC_UMA_H_

#include <HeciRegs.h>
#include <SeCAccess.h>
#include <IndustryStandard/Pci22.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PerformanceLib.h>
#include <Library/TimerLib.h>
#include <Ppi/SeCUma.h>
#include <Ppi/Stall.h>
#include "PlatformBaseAddresses.h"
#include <ScRegs/RegsPmc.h>

#define R_MESEG_BASE                0x70  // Removed in BXT
#define B_EXCLUSION                 BIT8

//
// SEC FW communication timeout value definitions
//
#define DID_TIMEOUT_MULTIPLIER      0x1388

//
// SEC FW HOST ALIVENESS RESP timeout
//
#define HOST_ALIVENESS_RESP_TIMEOUT_MULTIPLIER  0x1388
#define MUSZV_TIMEOUT_MULTIPLIER  0x1388
#define CPURV_TIMEOUT_MULTIPLIER  0x32
#define STALL_1_MILLISECOND       1000
#define STALL_100_MICROSECONDS    100

//
// Function Prototype(s)
//
/**
  This procedure will read and return the amount of SEC UMA requested
  by SEC ROM from the HECI device.

  @param[in]  PeiServices     General purpose services available to every PEIM.

  @return Return SEC UMA Size in KBs

**/
EFI_STATUS
SeCSendUmaSize (
  IN EFI_PEI_SERVICES **PeiServices
  );

/**
  This procedure will configure the SEC Host General Status register,
  indicating that DRAM Initialization is complete and SEC FW may
  begin using the allocated SEC UMA space.

  @param PeiServices     General purpose services available to every PEIM.
  @param MrcBootMode     MRC BootMode
  @param InitStat        H_GS[27:24] Status
  @param SeCUmaBase      Memory Location ** must be with in 4GB range
  @param SeCUmaSize      EDES_TODO: Add parameter description

  @retval                EFI_SUCCESS

**/
EFI_STATUS
SeCConfigDidReg (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  MRC_BOOT_MODE_T           MrcBootMode,
  UINT8                     InitStat,
  UINT32                    SeCUmaBase,
  UINT32                    *SeCUmaSize
  );

EFI_STATUS
SeCTakeOwnerShip (
  );

/**
  This procedure will issue a Non-Power Cycle, Power Cycle, or Global Rest.

  @param ResetType         Type of reset to be issued.

  @return Return EFI_SUCCESS

**/
EFI_STATUS
PerformReset (
  UINT8                     ResetType
  );

/**
  This procedure will clear the DISB.

  @param VOID            EDES_TODO: Add parameter description

  @return Return EFI_SUCCESS

**/
EFI_STATUS
ClearDISB (
  VOID
  );

/**
  This procedure will check the exposure of SeC device.

  @param PeiServices     EDES_TODO: Add parameter description

  @return Return EFI_SUCCESS

**/
EFI_STATUS
isSeCExpose (
  IN EFI_PEI_SERVICES **PeiServices
  );

#endif

