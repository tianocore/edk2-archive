/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POWER_MGMT_CONFIG_H_
#define _POWER_MGMT_CONFIG_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gPowerMgmtConfigGuid;

#pragma pack(push, 1)

#define POWER_MGMT_CONFIG_REVISION  1

///
/// Define maximum number of custom ratio states supported
///
#define MAX_CUSTOM_RATIO_TABLE_ENTRIES 20

///
/// PPM Package C State Limit
///
typedef enum {
  PkgC0                 = 0,
  PkgC1,
  PkgC2,
  PkgCpuDefault = 254
} MAX_PKG_C_STATE;

///
/// Custom Power Uints.User can choose to enter in MilliWatts or Watts
///
typedef enum {
  PowerUnitWatts = 0,     ///< in Watts
  PowerUnit125MilliWatts, ///< in 125 Milli Watts. Example 11.250 W Value to use for Power limits 90
  PowerUnitMax
} CUSTOM_POWER_UNIT;

/**
  Power management Configuration for all Processor Power Management Features
  Configs are in this field. Platform code can enable/disable features through this field.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>.
  - <b>(Required)</b>: This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization.
  - <b>(Optional)</b>: This policy is recommended for validation purpose only.
**/
typedef struct {
  CONFIG_BLOCK_HEADER Header;           ///< Offset 0 GUID number for main entry of config block
  UINT32              Eist                       : 1; ///< <b>(Test)</b> Enable or Disable Intel SpeedStep Technology. 0: Disable; <b>1: Enable</b>
  UINT32              Cx                         : 1; ///< <b>(Test)</b> Enable or Disable CPU power states (C-states). 0: Disable; <b>1: Enable</b>
  UINT32              C1e                        : 1; ///< <b>(Test)</b> Enable or Disable Enhanced C-states. 0: Disable; <b>1: Enable</b>
  UINT32              C1AutoDemotion             : 1; ///< <b>(Test)</b> Enable or Disable C6/C7 auto demotion to C1. 0: Disabled; <b>1: C1 Auto demotion</b>;
  UINT32              C3AutoDemotion             : 1; ///< <b>(Test)</b> Enable or Disable C6/C7 auto demotion to C3  0: Disabled; <b>1: C3 Auto demotion</b>;
  UINT32              TurboMode                  : 1; ///< <b>(Test)</b> Enable or Disable long duration Turbo Mode. Disable; <b>1: Enable</b>
  UINT32              PowerLimit2Enable          : 1; ///< Enable or Disable short duration Turbo Mode. Disable; <b>1: Enable</b>
  UINT32              C1UnDemotion               : 1; ///< Enable or Disable C1UnDemotion. 0: Disabled; <b>1: C1 Auto undemotion</b>;
  UINT32              C3UnDemotion               : 1; ///< <b>(Test)</b> Enable or Disable C3UnDemotion. 0: Disabled; <b>1: C3 Auto undemotion</b>;
  UINT32              PkgCStateDemotion          : 1; ///< <b>(Test)</b> Enable or Disable Package Cstate Demotion. <b>Disable</b>; 1: Enable
  UINT32              PkgCStateUnDemotion        : 1; ///< <b>(Test)</b> Enable or Disable Package Cstate UnDemotion. <b>Disable</b>; 1: Enable
  UINT32              TimedMwait                 : 1; ///< <b>(Test)</b> Enable or Disable TimedMwait Support. <b>Disable</b>; 1: Enable
  UINT32              PlatformPowerLimit1        : 1; ///< MSR 0x65C[15]: PL1 Enable activates the PL1 value to limit average platform power
  UINT32              PlatformPowerLimit2        : 1; ///< MSR 0x65C[47]: PL2 Enable activates the PL2 value to limit average platform power
  UINT32              SkipSetBootPState          : 1; ///< Choose whether to skip SetBootPState function for all APs; <b>0: Do not skip</b>; 1: Skip.
  UINT32              TurboPowerLimitLock        : 1; ///< MSR 0x610[63] and 0x618[63]: Lock all Turbo settings; <b>0: Disable</b>; 1: Enable.
  UINT32              PowerLimit3Lock            : 1; ///< Package PL3 MSR 615h lock; <b>0: Disable</b>; 1: Enable.
  UINT32              PowerLimit4Lock            : 1; ///< Package PL4 MSR 601h lock; <b>0: Disable</b>; 1: Enable.
  UINT32              PowerLimit3DutyCycle       : 8; ///< Package PL3 Duty Cycle; <b>0</b>.
  UINT32              PowerLimit1Enable          : 1; ///< Enable or Disable long duration Turbo Mode
  UINT32              PowerLimit1ClampEnable     : 1; ///< Enable or Disable clamp mode for long duration Turbo
  UINT32              Rsvd1Bits                  : 4; ///< Bits reserved for DWORD alignment.
  UINT32              PlatformPowerLimit1Time    : 8; ///< MSR 0x65C[23:17]: PL1 timewindow in seconds.
  UINT32              PmgCstCfgCtrIoMwaitRedirection : 1; ///< <b>(Test)</b> Enable or Disable IO to MWAIT redirection; <b>0: Disable</b>; 1: Enable.
  UINT32              PmgCstCfgCtrlLock          : 1; ///< <b>(Test)</b> If enabled, sets MSR 0xE2[15]; 0: Disable; <b>1: Enable</b>.
  UINT32              ProcHotLock                : 1; ///< <b>(Test)</b> If enabled, sets MSR 0x1FC[23]; <b>0: Disable</b>; 1: Enable.
  UINT32              BiProcHot                  : 1; ///< <b>(Test)</b> Enable or Disable Bi-Directional PROCHOT#; 0: Disable; <b>1: Enable</b>.
  UINT32              TStates                    : 1; ///< <b>(Test)</b> Enable or Disable T states; <b>0: Disable</b>; 1: Enable.
  UINT32              DisableProcHotOut          : 1; ///< <b>(Test)</b> Enable or Disable PROCHOT# signal being driven externally; 0: Disable; <b>1: Enable</b>.
  UINT32              DisableVrThermalAlert      : 1; ///< <b>(Test)</b> Enable or Disable VR Thermal Alert; <b>0: Disable</b>; 1: Enable.
  UINT32              ProcHotResponse            : 1; ///< <b>(Test)</b> Enable or Disable PROCHOT# Response; <b>0: Disable</b>; 1: Enable.
  UINT32              AutoThermalReporting       : 1; ///< <b>(Test)</b> Enable or Disable Thermal Reporting through ACPI tables; 0: Disable; <b>1: Enable</b>.
  UINT32              ThermalMonitor             : 1; ///< <b>(Test)</b> Enable or Disable Thermal Monitor; 0: Disable; <b>1: Enable</b>.
  UINT32              EnableCCx                  : 1; ///< <b>(Optional)</b> Max Core Cstate, fused value
  UINT32              UnlimitedCstate            : 1; ///< <b>(Optional)</b> Max Core Cstate, no limit
  UINT32              EnableC1                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C1
  UINT32              EnableC3                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C3
  UINT32              EnableC6                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C6
  UINT32              EnableC7                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C7
  UINT32              EnableC8                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C8
  UINT32              EnableC9                   : 1; ///< <b>(Optional)</b> Max Core Cstate, C9
  UINT32              EnableC10                  : 1; ///< <b>(Optional)</b> Max Core Cstate, C10
  UINT32              BootPState                 : 1; ///< <b>(Optional)</b> Boot PState with HFM or LFM. 0: HFM; <b>1: LFM</b>;
  UINT32              VrConfig                   : 1; ///< <b>(Optional)</b> Enable or Disable VR Config for VCC and VNN Rails; <b>0: Disable</b>; 1: Enable.
  UINT32              Rsvd2Bits                  : 3; ///< Bits reserved for DWORD alignment.
  UINT16              CustomPowerLimit1;              ///< Custom Package Long duration turbo mode power limit in 1/8 watt units.
  UINT16              CustomPowerLimit1Time;          ///< Custom Package Long duration turbo mode time window in seconds.
  UINT16              PowerLimit1;                    ///< Package Long duration turbo mode power limit in 1/8 watt units.
  UINT16              PowerLimit2;                    ///< Package Short duration turbo mode power limit in 1/8 watt units.
  UINT16              PowerLimit3;                    ///< Package PL3 power limit in 1/8 watt units.
  UINT16              PowerLimit4;                    ///< Package PL4 power limit in 1/8 watt units.
  UINT16              PlatformPowerLimit1Power;       ///< MSR 0x65C[14:0]: Platform PL1 power in 1/8 watt units.
  UINT16              PlatformPowerLimit2Power;       ///< MSR 0x65C[46:32]]: Platform PL2 power in 1/8 watt units.
  UINT32              PowerLimit1Time;                ///< Package Long duration turbo mode time window in seconds.
  UINT32              PowerLimit3Time;                ///< Package PL3 time window range for this policy from 3ms to 64ms.
  UINT16              DdrPowerLimit;                  ///< DDR Power Limits
  UINT16              RatioLimitProgramable;          ///< RatioLimitProgramable

  /**
   - 1-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 1-Core Ratio Limit + OC Bins.
     - This 1-Core Ratio Limit Must be greater than or equal to 2-Core Ratio Limit, 3-Core Ratio Limit, 4-Core Ratio Limit.
   - 2-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 2-Core Ratio Limit + OC Bins.
     - This 2-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
   - 3-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 3-Core Ratio Limit + OC Bins.
     - This 3-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
   - 4-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 4-Core Ratio Limit + OC Bins.
     - This 4-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  **/
  UINT8               RatioLimit[4];
  /**
  TCC Activation Offset. Offset from factory set TCC activation temperature at which the Thermal Control Circuit must be activated
  TCC will be activated at TCC Activation Temperature, in volts.
  For SKL Y SKU, the recommended default for this policy is  <b>10</b>, For all other SKUs the recommended default are <b>0</b>
  @note The policy is recommended for validation purpose only.
  **/
  UINT8              TccActivationOffset;
  UINT8              TccOffsetTimeWindow;            ///< @deprecated Tcc Offset Time Window value (0 ~ 128) for Runtime Average Temperature Limit (RATL). Deprecated since revision 2
  UINT8              S3RestoreMsrSwSmiNumber;        ///< SW SMI number to restore the power Mgmt MSRs during S3 resume.

  /**
  Default power unit for following items changes to MilliWatts.
  - POWER_MGMT_CONFIG.PowerLimit1
  - POWER_MGMT_CONFIG.PowerLimit2
  - POWER_MGMT_CONFIG.PowerLimit3
  - POWER_MGMT_CONFIG.CustomPowerLimit1
  - POWER_MGMT_CONFIG.CustomPowerLimit2
  **/
  CUSTOM_POWER_UNIT  CustomPowerUnit;               ///< Power Management Custom Power Limit Unit in milli watts.
  MAX_PKG_C_STATE    PkgCStateLimit;                ///< <b>(Test)</b> This field is used to set the Max Pkg Cstate. Default set to Auto which limits the Max Pkg Cstate to deep C-state.

  /**
  custom processor ratio table desired by the platform
  **/
  UINT16             MaxRatio;                                   ///< The maximum ratio of the custom ratio table.
  UINT8              NumberOfEntries;                            ///< The number of custom ratio state entries, ranges from 2 to 16 for a valid custom ratio table.
  UINT8              Rsvd1;                                      ///< Reserved for DWORD alignment.
  UINT32             Cpuid;                                      ///< The CPU ID for which this custom ratio table applies.
  UINT16             StateRatio[MAX_CUSTOM_RATIO_TABLE_ENTRIES]; ///< The processor ratios in the custom ratio table.
#if (MAX_CUSTOM_RATIO_TABLE_ENTRIES % 2)
  UINT16             Rsvd2;                                      ///< If there is an odd number of array entries, add padding for dword alignment.
#endif
} POWER_MGMT_CONFIG;

#pragma pack(pop)
#endif

