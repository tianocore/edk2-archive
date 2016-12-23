/** @file
  Prototype of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VT_D_DXE_H_
#define _VT_D_DXE_H_

//
// Include files
//
#include <PiPei.h>
#include <DmaRemappingTable.h>
#include <ScAccess.h>
#include <Uefi.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PeiDxeSmmMmPciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/EventGroup.h>
#include <CpuRegs.h>
#include <SaRegs.h>
#include <ScRegs/RegsPcu.h>
#include <PlatformBaseAddresses.h>

#define VTD_ECAP_REG            0x10
#define IR                      BIT3
#define VTD_RMRR_USB_LENGTH     0x20000

#define EFI_MSR_XAPIC_BASE      0x1B
#define R_SA_MCHBAR             0x48
//
// VT-d Engine base address.
//
#define R_SA_MCHBAR_VTD1_OFFSET  0x6C88  ///< DMA Remapping HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET  0x6C80  ///< DMA Remapping HW UNIT2 for all other - PEG, USB, SATA etc

/**
  Locate the VT-d ACPI tables data file and update it based on current configuration and capabilities.

  @retval EFI_SUCCESS        VT-d initialization complete
  @retval EFI_UNSUPPORTED    VT-d is disabled by policy or not supported

**/
EFI_STATUS
VtdInit (
  VOID
  );

/**
  ReadyToBoot callback routine to update DMAR.

**/
VOID
UpdateDmarOnReadyToBoot (
  BOOLEAN VtEnable
  );

#endif

