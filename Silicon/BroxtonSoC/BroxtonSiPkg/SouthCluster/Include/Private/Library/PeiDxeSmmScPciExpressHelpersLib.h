/** @file
  Header file for PCH PCI Express helpers library.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_DXE_SMM_PCH_PCI_EXPRESS_HELPERS_LIB_H_
#define _PEI_DXE_SMM_PCH_PCI_EXPRESS_HELPERS_LIB_H_

#include <Ppi/ScPolicy.h>
#include <Ppi/ScPcieDeviceTable.h>

//
// Function prototypes
//
/**
  Find the Offset to a given Capabilities ID.
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number
  @param[in] CapId                CAPID to search for

  @retval    0                    CAPID not found
  @retval    Other                CAPID found, Offset of desired CAPID

**/
UINT8
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   CapId
  );

/**
  Search and return the offset of desired Pci Express Capability ID.
  CAPID list:
    0x0001 = Advanced Error Rreporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number
  @param[in] CapId                Extended CAPID to search for

  @retval    0                    CAPID not found
  @retval    Other                CAPID found, Offset of desired CAPID

**/
UINT16
PcieFindExtendedCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT16  CapId
  );

/**
  This function returns PID according to Root Port Number.

  @param[in] RpPort        Root Port Number

  @retval    SC_SBI_PID    Returns PID for SBI Access

**/
SC_SBI_PID
GetRpSbiPid (
  IN  UINTN  RpPort
  );

/**
  This function returns  the Config Hide bit according to to Root Port Number.

  @param[in]    RpPort          Root Port Number

  @retval       UINT32          Port Config Hide bit

**/
UINT32
GetRpConfigHideBit (
  IN UINTN              RpPort
  );

/**
  This function reads Pci Config register via SBI Access.

  @param[in]  RpDevice            Root Port Device Number
  @param[in]  RpPort              Root Port Number
  @param[in]  Offset              Offset of Config register
  @param[out] *Data32             Value of Config register

  @retval     EFI_SUCCESS         SBI Read successful.

**/
EFI_STATUS
PchSbiRpPciRead32 (
  IN    UINTN   RpDevice,
  IN    UINTN   RpPort,
  IN    UINTN   Offset,
  OUT   UINT32  *Data32
  );

/**
  This function And then Or Pci Config register via SBI Access.

  @param[in]  RpDevice            Root Port Device Number
  @param[in]  RpPort              Root Port Number
  @param[in]  Offset              Offset of Config register
  @param[in]  Data32And           Value of Config register to be And-ed
  @param[in]  Data32AOr           Value of Config register to be Or-ed

  @retval     EFI_SUCCESS         SBI Read and Write successful.

**/
EFI_STATUS
PchSbiRpPciAndThenOr32 (
  IN  UINTN   RpDevice,
  IN  UINTN   RpPort,
  IN  UINTN   Offset,
  IN  UINT32  Data32And,
  IN  UINT32  Data32Or
  );

/**
  This function returns the maximum number of ClkReq signals available.

  @retval    UINT8                Returns maximum number of ClkReq signals

**/
UINT8
GetClkReqMax (
  VOID
  );

/**
  This returns ClkReq Number from Port Number.

  @param[in]    PortIndex     PCIe Port Number (Zero Base. Please use 23 for GBe)

  @retval       ClkReq Number

**/
UINT8
GetPortClkReqNumber (
  IN      UINT8           PortIndex
  );

/**
  This function assigns a ClkReq signal to Pcie ports and returns updated ClkReq Count.

  @param[in]    PcieConfig      PCH Pcie Configuration
  @param[in]    PortIndex       PCIe Port Number (Zero Base)

  @retval       EFI_SUCCESS     Successfully set ClkReq Number to Root Port

**/
EFI_STATUS
SetPortClkReqNumber (
  IN      SC_PCIE_CONFIG         *PcieConfig,
  IN      UINT8                   PortIndex
  );

/**
  Set Common clock to Root port and Endpoint PCI device.

  @param[in] Bus1                 Root port Pci Bus Number
  @param[in] Device1              Root port Pci Device Number
  @param[in] Function1            Root port Pci Function Number
  @param[in] Bus2                 Endpoint Pci Bus Number
  @param[in] Device2              Endpoint Pci Device Number

  @retval    EFI_UNSUPPORTED      Unsupported operation.
  @retval    EFI_SUCCESS          VC mapping correctly initialized

**/
EFI_STATUS
PcieSetCommonClock (
  IN UINT8   Bus1,
  IN UINT8   Device1,
  IN UINT8   Function1,
  IN UINT8   Bus2,
  IN UINT8   Device2
  );

/**
  This function enables the CLKREQ# PM on all the end point functions

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] RootDevice           Rootport Device Number
  @param[in] RootFunction         Rootport Function Number

  @retval    None

**/
VOID
PcieSetClkreq (
  IN  UINT8   EndPointBus,
  IN  UINT8   EndPointDevice,
  IN  UINT8   RootDevice,
  IN  UINT8   RootFunction
  );

/**
  This function get or set the Max Payload Size on all the end point functions

  @param[in] EndPointBus          The Bus Number of the Endpoint
  @param[in] EndPointDevice       The Device Number of the Endpoint
  @param[in] MaxPayload           The Max Payolad Size of the root port
  @param[in] Operation            True:  Set the Max Payload Size on all the end point functions
                                  False: Get the Max Payload Size on all the end point functions

  @retval    EFI_SUCCESS          Successfully completed.

**/
EFI_STATUS
PcieMaxPayloadSize (
  IN      UINT8           EndPointBus,
  IN      UINT8           EndPointDevice,
  IN OUT  UINT16          *MaxPayload,
  IN      BOOLEAN         Operation
  );

/**
  This function disable the forwarding of EOI messages unless it discovers
  an IOAPIC behind this root port.

  @param[in] RootBus              The Bus Number of the root port
  @param[in] RootDevice           The Device Number of the root port
  @param[in] RootFunction         The Function Number of the root port
  @param[in] EndPointBus          The Bus Number of the Endpoint
  @param[in] EndPointDevice       The Device Number of the Endpoint

  @retval    EFI_UNSUPPORTED      Unsupported operation.
  @retval    EFI_SUCCESS          Successfully completed.

**/
EFI_STATUS
PcieSetEoiFwdDisable (
  IN  UINT8   RootBus,
  IN  UINT8   RootDevice,
  IN  UINT8   RootFunction,
  IN  UINT8   EndPointBus,
  IN  UINT8   EndPointDevice
  );

/**
  This function performs the Power Management settings for root port and downstream device

  @param[in] RootBus                          Pci Bus Number of the root port
  @param[in] RootDevice                       Pci Device Number of the root port
  @param[in] RootFunction                     Pci Function Number of the root port
  @param[in] NumOfDevAspmOverride             Number of Device specific ASPM policy override items
  @param[in] DevAspmOverride                  Pointer to array of Device specific ASPM policy override items
  @param[in] TempBusNumberMin                 Minimal temp bus number that can be assigned to the root port (as secondary
                                              bus number) and its down stream switches
  @param[in] TempBusNumberMax                 Maximal temp bus number that can be assigned to the root port (as subordinate
                                              bus number) and its down stream switches
  @param[in] RootPortConfig                   Pcie Power Optimizer Configuration
  @param[in, out] L1SubstatesSupported        L1 substates supported on the root port
  @param[in] PolicyRevision                   Policy revision for codes compatibility
  @param[in] FirstRPToSetPm                   Indicates if this is the first root port to be set
  @param[in] L1SupportedInAllEnabledPorts     Check if L1 is supported in all enabled ports
  @param[in] ClkreqSupportedInAllEnabledPorts Check if clkreq is supported in all enabled ports
  @param[out] LtrSupported                    Return to check if all endpoints support LTR
  @param[in] AllowRpAspmProgramming           Allow fine grain control on when the RP ASPM programming is to be done,
                                              particularly used by the RST PCIe storage remapping feature

  @retval    EFI_SUCCESS                      The function completed successfully
  @retval    EFI_UNSUPPORTED                  The pointer to the Port PCI Express Capability Structure is not found

**/
EFI_STATUS
PcieSetPm (
  IN  UINT8                                RootBus,
  IN  UINT8                                RootDevice,
  IN  UINT8                                RootFunction,
  IN  UINT32                               NumOfDevAspmOverride,
  IN  CONST SC_PCIE_DEVICE_OVERRIDE        *DevAspmOverride,
  IN  UINT8                                TempBusNumberMin,
  IN  UINT8                                TempBusNumberMax,
  IN  CONST SC_PCIE_ROOT_PORT_CONFIG       *RootPortConfig,
  IN OUT BOOLEAN                           *L1SubstatesSupported,
  IN  UINT8                                PolicyRevision,
  IN  BOOLEAN                              FirstRPToSetPm,
  IN  BOOLEAN                              L1SupportedInAllEnabledPorts,
  IN  BOOLEAN                              ClkreqSupportedInAllEnabledPorts,
  OUT BOOLEAN                              *LtrSupported,
  IN  BOOLEAN                              AllowRpAspmProgramming
  );

/**
  This function checks if the root port and downstream device support Clkreq per port, ASPM L1 and L1 substates.

  @param[in]      RootBus                     Pci Bus Number of the root port
  @param[in]      RootDevice                  Pci Device Number of the root port
  @param[in]      RootFunction                Pci Function Number of the root port
  @param[in]      NumOfDevAspmOverride        Number of Device specific ASPM policy override items
  @param[in]      DevAspmOverride             Pointer to array of Device specific ASPM policy override items
  @param[in]      TempBusNumberMin            Minimal temp bus number that can be assigned to the root port (as secondary
                                              bus number) and its down stream switches
  @param[in]      TempBusNumberMax            Maximal temp bus number that can be assigned to the root port (as subordinate
                                              bus number) and its down stream switches
  @param[in]      RootPortConfig              Pcie Power Optimizer Configuration
  @param[in, out] L1SubstatesSupported        Flag to indicate if L1 Substates are supported
  @param[in]      PolicyRevision              Revision of the policy
  @param[in, out] AspmVal                     Aspm value for both rootport and end point devices
  @param[in, out] ClkreqPerPortSupported      Clkreq support for both rootport and endpoint devices
  @param[out]     LtrSupported                Return to check if all endpoints support LTR

  @retval         EFI_SUCCESS                 The function completed successfully
  @retval         EFI_UNSUPPORTED             The pointer to the Port PCI Express Capability Structure is not found

**/
EFI_STATUS
PcieCheckPmConfig (
  IN  UINT8                                RootBus,
  IN  UINT8                                RootDevice,
  IN  UINT8                                RootFunction,
  IN  UINT32                               NumOfDevAspmOverride,
  IN  SC_PCIE_DEVICE_OVERRIDE             *DevAspmOverride,
  IN  UINT8                                TempBusNumberMin,
  IN  UINT8                                TempBusNumberMax,
  IN  CONST SC_PCIE_ROOT_PORT_CONFIG      *RootPortConfig,
  IN OUT BOOLEAN                           *L1SubstatesSupported,
  IN  UINT8                                PolicyRevision,
  IN OUT UINT16                            *AspmVal,
  IN OUT BOOLEAN                           *ClkreqPerPortSupported,
  OUT BOOLEAN                              *LtrSupported
  );

/**
  Initializes the root port and its down stream devices

  @param[in]  RootPortBus             Pci Bus Number of the root port
  @param[in]  RootPortDevice          Pci Device Number of the root port
  @param[in]  RootPortFunc            Pci Function Number of the root port
  @param[in]  TempBusNumberMin        Minimal temp bus number that can be assigned to the root port (as secondary
                                      bus number) and its down stream switches
  @param[in]  TempBusNumberMax        Maximal temp bus number that can be assigned to the root port (as subordinate
                                      bus number) and its down stream switches
  @param[out] PortWithMaxPayload128   At least one Port with MaxPayload set to 128 bits

  @retval     EFI_SUCCESS             Successfully completed
  @retval     EFI_NOT_FOUND           Can not find device.

**/
EFI_STATUS
PchPcieInitRootPortDownstreamDevices (
  IN UINT8                          RootPortBus,
  IN UINT8                          RootPortDevice,
  IN UINT8                          RootPortFunc,
  IN UINT8                          TempBusNumberMin,
  IN UINT8                          TempBusNumberMax,
  OUT BOOLEAN                       *PortWithMaxPayload128
  );

#endif // _PEI_DXE_SMM_PCH_PCI_EXPRESS_HELPERS_LIB_H_

