/** @file
  Definitions for HECI driver.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_CORE_H
#define _HECI_CORE_H

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/PerformanceLib.h>
#include <Library/TimerLib.h>
#include <SeCState.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>
#include <SeCAccess.h>
#include <IndustryStandard/Pci22.h>
#include <Library/MmPciLib.h>
#include <Library/SeCLib.h>

//
// HECI bus function version
//
#define HBM_MINOR_VERSION 0
#define HBM_MAJOR_VERSION 1

#define STALL_1_MILLISECOND       1000
#define CSE_WAIT_TIMEOUT          50

//
// Local/Private functions not part of EFIAPI for HECI
//
/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in]  None

  @retval     EFI_STATUS

**/
EFI_STATUS
WaitForSECInputReady (
  VOID
  );

/**
  Calculate if the circular buffer has overflowed.

  @param[in]  ReadPointer      Location of the read pointer.
  @param[in]  WritePointer     Location of the write pointer.

  @retval     Number of filled slots.

**/
UINT8
FilledSlots (
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer
  );

/**
  Calculate if the circular buffer has overflowed.

  @param[in]  ReadPointer     Value read from host/me read pointer
  @param[in]  WritePointer    Value read from host/me write pointer
  @param[in]  BufferDepth     Value read from buffer depth register

  @retval     EFI_STATUS

**/
EFI_STATUS
OverflowCB (
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer,
  IN      UINT32                    BufferDepth
  );

/**
  SeCAlivenessRequest

  @param[in]  HeciMemBar
  @param[in]  Request

  @retval     EFI_STATUS

**/
EFI_STATUS
SeCAlivenessRequest (
  IN      UINTN                       *HeciMemBar,
  IN      UINT32                       Request
  );

#endif // _HECI_CORE_H

