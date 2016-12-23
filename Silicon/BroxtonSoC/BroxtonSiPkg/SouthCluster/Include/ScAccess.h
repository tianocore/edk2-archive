/** @file
  Macros that simplify accessing SC devices's PCI registers.
  @note: Tthese macros assume the SC device is on BUS 0
  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_ACCESS_H_
#define _SC_ACCESS_H_

#include <Library/IoLib.h>
#include <PcieRegs.h>
#include "ScLimits.h"
#include "ScReservedResources.h"

#ifndef STALL_ONE_MICRO_SECOND
  #define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
  #define STALL_ONE_SECOND 1000000
#endif

//
// The default PCI bus number
//
#define DEFAULT_PCI_BUS_NUMBER_SC  0

//
// Default Vendor ID and Subsystem ID
//
#define V_INTEL_VENDOR_ID 0x8086      ///< Default Intel Vendor ID
#define V_SC_DEFAULT_SID  0x7270      ///< Default Intel Subsystem ID
#define V_SC_DEFAULT_SVID_SID  (V_INTEL_VENDOR_ID + (V_SC_DEFAULT_SID << 16))   ///< Default INTEL Vendor ID and Subsystem ID
//
// Include device register definitions
//
#include "ScRegs/RegsLpss.h"
#include "ScRegs/RegsItss.h"
#include "ScRegs/RegsPcie.h"
#include "ScRegs/RegsPcu.h"
#include "ScRegs/RegsPmc.h"
#include "ScRegs/RegsIsh.h"
#include "ScRegs/RegsSata.h"
#include "ScRegs/RegsScc.h"
#include "ScRegs/RegsSpi.h"
#include "ScRegs/RegsUsb.h"
#include "ScRegs/RegsHda.h"
#include "ScRegs/RegsSmbus.h"
#include "ScRegs/RegsLpc.h"
#include "ScRegs/RegsPcr.h"
#include "ScRegs/RegsP2sb.h"
#include "ScRegs/RegsGpio.h"

#define IS_EMMC_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_SCC_EMMC_DEVICE_ID_0) || \
      (DeviceId == V_SCC_EMMC_DEVICE_ID_1) || \
      (DeviceId == V_SCC_EMMC_DEVICE_ID_2) || \
      (DeviceId == V_SCC_EMMC_DEVICE_ID_3) \
    )

#define IS_BXTP_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_SATA_DEVICE_ID_BXTP_AHCI) \
    )

#define IS_USB_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_DEVICE_ID_0) \
    )

//
//  PCIE Device ID macros
//
#define IS_BXT_P_PCIE_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT1) || \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT2) || \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT3) || \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT4) || \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT5) || \
      (DeviceId == V_BXT_P_PCIE_DEVICE_ID_PORT6) \
    )

#define IS_BXT_PCIE_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_BXT_PCIE_DEVICE_ID_PORT1) || \
      (DeviceId == V_BXT_PCIE_DEVICE_ID_PORT2) \
    )

#define IS_PCIE_DEVICE_ID(DeviceId) \
    ( \
      IS_BXT_P_PCIE_DEVICE_ID(DeviceId) || \
      IS_BXT_PCIE_DEVICE_ID(DeviceId) \
    )

//
// Memory Mapped PCI Access macros
//
//
// PCI Device MM Base
//
#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
  ((UINTN) PcdGet64 (PcdPciExpressBaseAddress) + \
   (UINTN) (Bus << 20) + \
   (UINTN) (Device << 15) + \
   (UINTN) (Function << 12) + \
   (UINTN) (Register) \
  )
#endif

#ifdef SATA_SUPPORT

//
// SATA device 0x13, Function 0
//
#define SataPciCfg32(Register) MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SATA, PCI_FUNCTION_NUMBER_SATA, Register))

#define SataPciCfg32Or(Register, OrData) \
  MmioOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  OrData \
  )

#define SataPciCfg32And(Register, AndData) \
  MmioAnd32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData \
  )

#define SataPciCfg32AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData, \
  OrData \
  )

#define SataPciCfg16(Register) MmioRead16 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SATA, PCI_FUNCTION_NUMBER_SATA, Register))

#define SataPciCfg16Or(Register, OrData) \
  MmioOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  OrData \
  )

#define SataPciCfg16And(Register, AndData) \
  MmioAnd16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData \
  )

#define SataPciCfg16AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData, \
  OrData \
  )

#define SataPciCfg8(Register)  MmioRead8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SATA, PCI_FUNCTION_NUMBER_SATA, Register))

#define SataPciCfg8Or(Register, OrData) \
  MmioOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  OrData \
  )

#define SataPciCfg8And(Register, AndData) \
  MmioAnd8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData \
  )

#define SataPciCfg8AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_SC, \
  PCI_DEVICE_NUMBER_SATA, \
  PCI_FUNCTION_NUMBER_SATA, \
  Register), \
  AndData, \
  OrData \
  )

#endif

//
// Device IDs that are PCH Server specific
//
#define IS_BXTP_SC_LPC_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_SC_LPC_DEVICE_ID_BXT_P_0) \
    )
#endif

/**
  PCR boot script accessing macro
  Those macros are only available for DXE phase.
**/
#define PCR_BOOT_SCRIPT_WRITE(Width, Pid, Offset, Count, Buffer) \
          S3BootScriptSaveMemWrite (Width, SC_PCR_ADDRESS (Pid, Offset), Count, Buffer); \
          S3BootScriptSaveMemPoll (Width, SC_PCR_ADDRESS (Pid, Offset), Buffer, Buffer, 1, 1);

#define PCR_BOOT_SCRIPT_READ_WRITE(Width, Pid, Offset, DataOr, DataAnd) \
          S3BootScriptSaveMemReadWrite (Width, SC_PCR_ADDRESS (Pid, Offset), DataOr, DataAnd); \
          S3BootScriptSaveMemPoll (Width, SC_PCR_ADDRESS (Pid, Offset), DataOr, DataOr, 1, 1);


