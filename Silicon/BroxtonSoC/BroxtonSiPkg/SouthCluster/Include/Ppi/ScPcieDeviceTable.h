/** @file
  SC policy PPI produced by a platform driver specifying PCIe device overrides.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PCIE_DEVICE_TABLE_H_
#define _SC_PCIE_DEVICE_TABLE_H_

//
// PCIe device table PPI GUID.
//
extern EFI_GUID  gScPcieDeviceTablePpiGuid;

typedef struct _SC_PCIE_DEVICE_OVERRIDE SC_PCIE_DEVICE_OVERRIDE;

typedef enum {
  ScPcieOverrideDisabled             = 0,
  ScPcieL1L2Override                 = 0x01,
  ScPcieL1SubstatesOverride          = 0x02,
  ScPcieL1L2AndL1SubstatesOverride   = 0x03,
  ScPcieLtrOverride                  = 0x04
} SC_PCIE_OVERRIDE_CONFIG;

struct _SC_PCIE_DEVICE_OVERRIDE {
  UINT16  VendorId;                    ///< The vendor Id of Pci Express card ASPM setting override, 0xFFFF means any Vendor ID
  UINT16  DeviceId;                    ///< The Device Id of Pci Express card ASPM setting override, 0xFFFF means any Device ID
  UINT8   RevId;                       ///< The Rev Id of Pci Express card ASPM setting override, 0xFF means all steppings
  UINT8   BaseClassCode;               ///< The Base Class Code of Pci Express card ASPM setting override, 0xFF means all base class
  UINT8   SubClassCode;                ///< The Sub Class Code of Pci Express card ASPM setting override, 0xFF means all sub class
  UINT8   EndPointAspm;                ///< Override device ASPM (see: PCH_PCIE_ASPM_CONTROL)
                                       ///< Bit 1 must be set in OverrideConfig for this field to take effect
  UINT16  OverrideConfig;              ///< The override config bitmap (see: PCH_PCIE_OVERRIDE_CONFIG).
  UINT16  L1SubstatesCapOffset;
  UINT8   L1SubstatesCapMask;
  UINT8   L1sCommonModeRestoreTime;
  UINT8   L1sTpowerOnScale;
  UINT8   L1sTpowerOnValue;
  UINT16  SnoopLatency;
  UINT16  NonSnoopLatency;
  UINT32  Reserved;
};

#endif // SC_PCIE_DEVICE_TABLE_H_

