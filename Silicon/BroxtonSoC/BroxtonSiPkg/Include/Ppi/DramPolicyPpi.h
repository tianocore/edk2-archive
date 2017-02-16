/** @file
  Dram Policy PPI is used for specifying platform
  related Intel silicon information and policy setting.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DRAM_POLICY_PPI_H_
#define _DRAM_POLICY_PPI_H_

#define DRAM_POLICY_NUMBER_CHANNELS         4
#define DRAM_POLICY_NUMBER_BITS             32
#define DRAM_POLICY_NUMBER_SPD_ADDRESSES    2

#pragma pack(push,1)

typedef struct {
  UINT8 RankEnable;
  UINT8 DeviceWidth;
  UINT8 DramDensity;
  UINT8 Option;
  UINT8 OdtConfig;
  UINT8 TristateClk1;
  UINT8 Mode2N;
  UINT8 OdtLevels;
} DRP_DRAM_POLICY;

typedef struct {
  UINT8                 Package;
  UINT8                 Profile;
  UINT8                 MemoryDown;
  UINT8                 DDR3LPageSize;
  UINT8                 DDR3LASR;
  EFI_PHYSICAL_ADDRESS  MrcTrainingDataPtr;
  EFI_PHYSICAL_ADDRESS  MrcBootDataPtr;
  UINT8                 ScramblerSupport;
  UINT16                ChannelHashMask;
  UINT16                SliceHashMask;
  UINT8                 InterleavedMode;
  UINT8                 ChannelsSlicesEnabled;   /// bit0 = SliceEnabled; bit1 = channelEnabled
  UINT8                 MinRefRate2xEnabled;
  UINT8                 DualRankSupportEnabled;
  UINT16                SystemMemorySizeLimit;
  UINT16                LowMemMaxVal;
  UINT16                HighMemMaxVal;
  UINT8                 DisableFastBoot;
  UINT8                 RmtMode;
  UINT8                 RmtCheckRun;
  UINT16                RmtMarginCheckScaleHighThreshold;
  UINT8                 Reserved1;
  UINT32                MsgLevelMask;
  UINT8                 SpdAddress[DRAM_POLICY_NUMBER_SPD_ADDRESSES];
  UINT8                 ChSwizzle[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS];
  DRP_DRAM_POLICY       ChDrp[DRAM_POLICY_NUMBER_CHANNELS];
  UINT8                 Reserved2;
  UINT8                 DebugMsgLevel;
  UINT8                 reserved[13];
} DRAM_POLICY_PPI;

#pragma pack(pop)

extern EFI_GUID gDramPolicyPpiGuid;

#endif // _DRAM_POLICY_PPI_H_

