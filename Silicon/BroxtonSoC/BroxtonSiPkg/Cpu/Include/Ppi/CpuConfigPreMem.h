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

#ifndef _CPU_CONFIG_PREMEM_H_
#define _CPU_CONFIG_PREMEM_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gCpuConfigPreMemGuid;

#pragma pack(push, 1)

//
// CPU_CONFIG_PREMEM revisions
//
#define CPU_CONFIG_PREMEM_REVISION 1

/**
  Platform Policies for CPU features configuration Platform code can enable/disable/configure features through this structure.

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                              ///< Offset 0 GUID number for main entry of config block
  UINT32               BistOnReset                     : 1; ///< Enable or Disable BIST on Reset; <b>0: Disable</b>; 1 Enable.
  UINT32               Txt                             : 1; ///< Enable or Disable TXT; <b>0: Disable</b>; 1: Enable.
  UINT32               SkipStopPbet                    : 1; ///< Skip Stop PBET Timer <b>0: Disabled</b>, 1: Enabled
  UINT32               BiosGuard                       : 1; ///< Enable or Disable BIOS Guard; 0: Disable, <b>1:Enable</b>
  UINT32               EnableC6Dram                    : 1; ///< C6DRAM Memory. <b>0: C6DRAM Disabled</b>; 1: C6DRAM Enabled.
  UINT32               FlashWearOutProtection          : 1; ///< Flash Wear-Out Protection; <b>0: Disable</b>; 1: Enable
  UINT32               RsvdBits                        :25; ///< Bits reserved for DWORD alignment.
  UINT16               TotalFlashSize;                      ///< Total Flash Size on the system in KB
  UINT16               BiosSize;                            ///< BIOS Size in KB
} CPU_CONFIG_PREMEM;

#pragma pack(pop)
#endif

