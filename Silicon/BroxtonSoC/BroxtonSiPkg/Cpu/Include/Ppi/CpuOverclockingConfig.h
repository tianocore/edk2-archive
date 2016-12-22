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

#ifndef _CPU_OVERCLOCKING_CONFIG_H_
#define _CPU_OVERCLOCKING_CONFIG_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gCpuOverclockingConfigGuid;

#pragma pack(push, 1)

#define CPU_OVERCLOCKING_CONFIG_REVISION 1

/**
  Overclocking Configuration controls which use the CPU overclocking mailbox interface are defined in this structure.
  Platform code can pass in data to the mailbox through this structure.

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                        ///< Offset 0 GUID number for main entry of config block
  UINT32               CoreVoltageMode      :  1;     ///< Core voltage mode; <b>0: Adaptive</b>; 1: Override.
  UINT32               RingVoltageMode      :  1;     ///< CLR voltage mode; <b>0: Adaptive</b>; 1:Override
  UINT32               OcSupport            :  1;     ///< Over clocking support; <b>0: Disable</b>; 1: Enable.
  UINT32               RsvdBits             : 29;     ///< Bits reserved for DWORD alignment.
  INT16                CoreVoltageOffset;             ///< The voltage offset applied to the core while operating in turbo mode.
  UINT16               CoreVoltageOverride;           ///< The core voltage override which is applied to the entire range of cpu core frequencies.
  UINT16               CoreExtraTurboVoltage;         ///< Extra Turbo voltage applied to the cpu core when the cpu is operating in turbo mode.
  UINT16               CoreMaxOcTurboRatio;           ///< Maximum core turbo ratio override allows to increase CPU core frequency beyond the fused max turbo ratio limit.
  INT16                RingVoltageOffset;             ///< The voltage offset applied to CLR while operating in turbo mode.
  UINT16               RingVoltageOverride;           ///< The clr voltage override which is applied to the entire range of cpu frequencies.
  UINT16               RingExtraTurboVoltage;         ///< Extra Turbo voltage applied to clr.
  UINT16               RingMaxOcTurboRatio;           ///< Maximum clr turbo ratio override allows to increase CPU clr frequency beyond the fused max turbo ratio limit.
} CPU_OVERCLOCKING_CONFIG;

#pragma pack(pop)
#endif

