/** @file
  PCIe root port policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PCIE_CONFIG_H_
#define _SC_PCIE_CONFIG_H_

#define PCIE_RP_PREMEM_CONFIG_REVISION 2
#define PCIE_RP_CONFIG_REVISION        4

extern EFI_GUID gPcieRpPreMemConfigGuid;
extern EFI_GUID gPcieRpConfigGuid;

#pragma pack (push,1)

/**
  The SC_PCI_EXPRESS_ROOT_PORT_CONFIG describe the feature and capability of each SC PCIe root port.

**/
typedef struct {
  UINT32  Perst;  ///< PCIe reset# pin GPIO pad offset.
  UINT32  Clock;  ///< PCIe clock# ping GPIO pad offset.
} SC_PCIE_ROOT_PORT_PREMEM_CONFIG;

/**
  The SC_PCIE_PREMEM_CONFIG block describes the expected configuration of the SC PCI Express controllers

**/
typedef struct {
  /**
    Revision 1: Init version

    Revision 2: Add StartTimerTickerOfPerstAssert.

    Revision 3: Add Clock in Root Port.
  **/
  CONFIG_BLOCK_HEADER              Header;           ///< Config Block Header
  /**
    These members describe the configuration of each PCH PCIe root port.
  **/
  SC_PCIE_ROOT_PORT_PREMEM_CONFIG  RootPort[SC_MAX_PCIE_ROOT_PORTS];
  /**
    The Start Timer Ticker of PFET be asserted.
    This policy item is primary for responsiveness improvement.
    BIOS can utilize this policy item to reduce the delay time in ScConfigurePciePowerSequence()
  **/
  UINTN                            StartTimerTickerOfPfetAssert;
} SC_PCIE_PREMEM_CONFIG;

//
// SW SMI values which are used by SC Policy
//
#define SW_SMI_PCIE_ASPM_OVERRIDE 0xAA

enum SC_PCIE_SPEED {
  ScPcieAuto,
  ScPcieGen1,
  ScPcieGen2,
  ScPcieGen3
};

typedef enum  {
  ScPcieAspmDisabled,
  ScPcieAspmL0s,
  ScPcieAspmL1,
  ScPcieAspmL0sL1,
  ScPcieAspmAutoConfig,
  ScPcieAspmMax
} SC_PCIE_ASPM_CONTROL;

typedef enum {
  ScPcieL1SubstatesDisabled,
  ScPcieL1SubstatesL1_1,
  ScPcieL1SubstatesL1_2,
  ScPcieL1SubstatesL1_1_2,
  ScPcieL1SubstatesMax
} SC_PCIE_L1SUBSTATES_CONTROL;

enum SC_PCIE_COMPLETION_TIMEOUT {
  ScPcieCompletionTO_Default,
  ScPcieCompletionTO_50_100us,
  ScPcieCompletionTO_1_10ms,
  ScPcieCompletionTO_16_55ms,
  ScPcieCompletionTO_65_210ms,
  ScPcieCompletionTO_260_900ms,
  ScPcieCompletionTO_1_3P5s,
  ScPcieCompletionTO_4_13s,
  ScPcieCompletionTO_17_64s,
  ScPcieCompletionTO_Disabled
};

/**
  The SC_PCIE_CONFIG block describes the expected configuration of the PCI Express controllers

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                                 ///< Config Block Header
  UINT32               Enable                          :  2;   ///< Root Port enabling, 0: Disable; 1: Enable; 2: Auto.
  UINT32               Hide                            :  1;   ///< Whether or not to hide the configuration space of this port.
  UINT32               SlotImplemented                 :  1;   ///< Indicates whether the root port is connected to a slot.
  UINT32               HotPlug                         :  1;   ///< Indicate whether the root port is hot plug available.
  UINT32               PmSci                           :  1;   ///< Indicate whether the root port power manager SCI is enabled.
  UINT32               ExtSync                         :  1;   ///< Indicate whether the extended synch is enabled.
  UINT32               TransmitterHalfSwing            :  1;   ///< Indicate whether the Transmitter Half Swing is enabled.
  UINT32               AcsEnabled                      :  1;   ///< Indicate whether the ACS is enabled

  UINT32               RsvdBits0                       :  5;   ///< Reserved bits.
  UINT32               ClkReqSupported                 :  1;   ///< Indicate whether dedicated CLKREQ# is supported by the port.
  /**
    The ClkReq Signal mapped to this root port. Default is zero. Valid if ClkReqSupported is TRUE.
    This Number should not exceed the Maximum Available ClkReq Signals.
  **/
  UINT32               ClkReqNumber                    :  4;
  /**
    Probe CLKREQ# signal before enabling CLKREQ# based power management.
    Conforming device shall hold CLKREQ# low until CPM is enabled. This feature attempts
    to verify CLKREQ# signal is connected by testing pad state before enabling CPM.
    In particular this helps to avoid issues with open-ended PCIe slots.
    This is only applicable to non hot-plug ports.
    <b>0: Disable</b>; 1: Enable.
  **/
  UINT32               ClkReqDetect                    :  1;
  //
  // Error handlings
  //
  UINT32               AdvancedErrorReporting          :  1;   ///< Indicate whether the Advanced Error Reporting is enabled
  UINT32               UnsupportedRequestReport        :  1;   ///< Indicate whether the Unsupported Request Report is enabled.
  UINT32               FatalErrorReport                :  1;   ///< Indicate whether the Fatal Error Report is enabled.
  UINT32               NoFatalErrorReport              :  1;   ///< Indicate whether the No Fatal Error Report is enabled.
  UINT32               CorrectableErrorReport          :  1;   ///< Indicate whether the Correctable Error Report is enabled.
  UINT32               PmeInterrupt                    :  1;   ///< Indicate whether the PME Interrupt is enabled.
  UINT32               SystemErrorOnFatalError         :  1;   ///< Indicate whether the System Error on Fatal Error is enabled.
  UINT32               SystemErrorOnNonFatalError      :  1;   ///< Indicate whether the System Error on Non Fatal Error is enabled.
  UINT32               SystemErrorOnCorrectableError   :  1;   ///< Indicate whether the System Error on Correctable Error is enabled.
  UINT32               Rsvdbits1                       :  3;   ///< Reserved fields for future expansion w/o protocol change
  /**
    Determines each PCIE Port speed capability.
    0: Auto; 1: Gen1; 2: Gen2; 3: Gen3 (see: SC_PCIE_SPEED)
  **/
  UINT8                PcieSpeed;
  UINT8                PhysicalSlotNumber;                     ///< Indicates the slot number for the root port.
  UINT8                CompletionTimeout;                      ///< The completion timeout configuration of the root port (see: SC_PCIE_COMPLETION_TIMEOUT)
  UINT8                Reserved0;                              ///< Reserved byte
  UINT32               PtmEnable                       :  1;   ///< PTM enabling, <b>0: Disable</b>; 1: Enable.
  /**
    Selectable De-emphasis enabling.
    When the Link is operating at 5.0 GT/s speed, this bit selects the level of de-emphasis for an Upstream component.
    1b: -3.5 dB, 0b:-6 dB
    0: Disable; <b>1: Enable</b>.
  **/
  UINT32               SelectableDeemphasis            :  1;
  UINT32               Rsvdbits2                       : 30;   ///< Reserved Bits
  UINT32               Reserved1[1];                           ///< Reserved bytes
  //
  // Power Management
  //
  UINT8                Aspm;                                   ///< The ASPM configuration of the root port (see: SC_PCIE_ASPM_CONTROL)
  UINT8                L1Substates;                            ///< The L1 Substates configuration of the root port (see: SC_PCIE_L1SUBSTATES_CONTROL)
  UINT8                LtrEnable;                              ///< Latency Tolerance Reporting Mechanism.
  UINT8                LtrConfigLock;                          ///< <b>0: Disable</b>; 1: Enable.
  UINT16               LtrMaxSnoopLatency;                     ///< <b>(Test)</b> Latency Tolerance Reporting, Max Snoop Latency.
  UINT16               LtrMaxNoSnoopLatency;                   ///< <b>(Test)</b> Latency Tolerance Reporting, Max Non-Snoop Latency.
  UINT8                SnoopLatencyOverrideMode;               ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Mode.
  UINT8                SnoopLatencyOverrideMultiplier;         ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Multiplier.
  UINT16               SnoopLatencyOverrideValue;              ///< <b>(Test)</b> Latency Tolerance Reporting, Snoop Latency Override Value.
  UINT8                NonSnoopLatencyOverrideMode;            ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Mode.
  UINT8                NonSnoopLatencyOverrideMultiplier;      ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.
  UINT16               NonSnoopLatencyOverrideValue;           ///< <b>(Test)</b> Latency Tolerance Reporting, Non-Snoop Latency Override Value.
  UINT32               SlotPowerLimitScale : 2;                ///< <b>(Test)</b> Specifies scale used for slot power limit value. Leave as 0 to set to default.
  UINT32               SlotPowerLimitValue : 12;               ///< <b>(Test)</b> Specifies upper limit on power supplie by slot. Leave as 0 to set to default.
  UINT32               Rsvdbits3           : 18;               ///< Reserved Bits
  UINT32               Reserved2[16];                          ///< Reserved bytes
  UINT8                PcieRootRsvd0;
} SC_PCIE_ROOT_PORT_CONFIG;

/**
  The SC_PCIE_CONFIG block describes the expected configuration of the SC PCI Express controllers

**/
typedef struct {
  /**
    Revision 1: Init version
    Revision 2: Add ComplianceTestMode.
    Revision 3: Add SelectableDeemphasis
  **/
  CONFIG_BLOCK_HEADER           Header;                   ///< Config Block Header
  /**
    These members describe the configuration of each SC PCIe root port.
  **/
  SC_PCIE_ROOT_PORT_CONFIG      RootPort[SC_MAX_PCIE_ROOT_PORTS];
  /**
    This member describes whether PCIE root port Port 8xh Decode is enabled
  **/
  UINT32                        EnablePort8xhDecode              :  1;
  /**
    The Index of PCIe Port that is selected for Port8xh Decode (0 Based)
  **/
  UINT32                        ScPciePort8xhDecodePortIndex     :  5;
  /**
    This member describes whether the PCI Express Clock Gating for each root port
    is enabled by platform modules. It is enabled by default.
  **/
  UINT32                        DisableRootPortClockGating       :  1;
  /**
    This member describes whether Peer Memroy Writes are enabled on the platform
  **/
  UINT32                        EnablePeerMemoryWrite            :  1;
  /**
    This member describes the SwSmi value for override PCIe ASPM table. Default is <b>0xAA</b>
  **/
  UINT32                        AspmSwSmiNumber                  :  8;
  /**
    Compliance Mode shall be enabled when using Compliance Load Board.
    <b>0: Disable</b>, 1: Enable
  **/
  UINT32                        ComplianceTestMode               :  1;
  UINT32                        Rsvdbits                         : 15;

  UINT32                        PcieRsvdBits0                    :  2;
  UINT32                        SvRsvdbits                       : 30;
  UINT32                        Reserved[3];              ///< Reserved bytes
} SC_PCIE_CONFIG;

#pragma pack (pop)

#endif // _SC_PCIE_CONFIG_H_

