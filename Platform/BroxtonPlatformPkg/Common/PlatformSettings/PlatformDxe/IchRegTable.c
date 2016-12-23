/** @file
  Register initialization table for SC.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include <Library/EfiRegTableLib.h>
#include <Library/SteppingLib.h>

extern EFI_PLATFORM_INFO_HOB      mPlatformInfo;

#define R_EFI_PCI_SVID 0x2C

/**
  Updates the mSubsystemIdRegs table, and processes it.
  This should program the Subsystem Vendor and Device IDs.

**/
VOID
InitializeSubsystemIds (
  )
{
  EFI_REG_TABLE  *RegTablePtr;
  UINT32         SubsystemVidDid;
  UINT32         SubsystemAudioVidDid;
  UINTN          Stepping;
  EFI_REG_TABLE  SubsystemIdRegsPCIESC [] = {
    //
    // Program SVID and SID for PCI devices.
    // Combine two 16 bit PCI_WRITE into one 32 bit PCI_WRITE in order to boost performance
    //
    PCI_WRITE (
      SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_EFI_PCI_SVID, EfiPciWidthUint32,
      V_SC_DEFAULT_SVID_SID, OPCODE_FLAG_S3SAVE
      ),
    PCI_WRITE (
      SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_EFI_PCI_SVID, EfiPciWidthUint32,
      V_SC_DEFAULT_SVID_SID, OPCODE_FLAG_S3SAVE
    ),

    #ifdef PCIESC_SUPPORT
    PCI_WRITE (
      DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCIE_ROOT_PORT_1, R_PCIE_SVID, EfiPciWidthUint32,
      V_SC_DEFAULT_SVID_SID, OPCODE_FLAG_S3SAVE
    ),
    #endif

    TERMINATE_TABLE
  };

  EFI_REG_TABLE  SubsystemIdRegs [] = {
    PCI_WRITE (
      SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_EFI_PCI_SVID, EfiPciWidthUint32,
      V_SC_DEFAULT_SVID_SID, OPCODE_FLAG_S3SAVE
    ),
    PCI_WRITE (
      SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_EFI_PCI_SVID, EfiPciWidthUint32,
      V_SC_DEFAULT_SVID_SID, OPCODE_FLAG_S3SAVE
    ),

    TERMINATE_TABLE
  };

  SubsystemVidDid = mPlatformInfo.SsidSvid;
  SubsystemAudioVidDid = mPlatformInfo.SsidSvid;

  Stepping = BxtStepping ();
  if ((Stepping != BxtA0) && (Stepping != BxtA1)) {
    RegTablePtr = SubsystemIdRegsPCIESC;
  } else {
    RegTablePtr = SubsystemIdRegs;
  }

  //
  // Check Board ID GPI to see if we need to use alternate SSID
  //
  // While we are not at the end of the table
  //
  while (RegTablePtr->Generic.OpCode != OP_TERMINATE_TABLE) {
    //
    // If the data to write is the original SSID
    //
    if (RegTablePtr->PciWrite.Data == ((V_SC_DEFAULT_SID << 16) | V_INTEL_VENDOR_ID)) {
      //
      // Then overwrite it to use the alternate SSID
      //
      RegTablePtr->PciWrite.Data = SubsystemVidDid;
    }
    //
    // Go to next table entry
    //
    RegTablePtr ++;
  }

  //
  // Program the SSVID/SSDID
  //
  ProcessRegTablePci (RegTablePtr, mPciRootBridgeIo, NULL);
}

