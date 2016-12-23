/** @file
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IAFW_SMIP_H_
#define _IAFW_SMIP_H_


#pragma pack(push,1)


typedef struct _Reserved_
{
  UINT8  ReservedByte;

} Reserved;

typedef struct _Bcu_
{
  UINT8  BattCurrLimit12;
  UINT8  BattTimeLimit12;
  UINT8  BattTimeLimit3;
  UINT8  BattTimeDb;
  UINT8  BrstCfgOuts;
  UINT8  BrstCfgActs;
} Bcu;

typedef struct _IAFWBatteryConfig_
{
  UINT8   sbctrev;
  UINT8   fpo;
  UINT8   fpo1;
  UINT8   dbiingpio;
  UINT8   dbioutgpio;
  UINT8   batchptyp;
  UINT16  ia_apps_run;
  UINT8   batiddbibase;
  UINT8   batidanlgbase;
  UINT8   ia_apps_cap;
  UINT16  vbattfreqcap;
  UINT8   capfreqidx;
  UINT8   rsvd;
  UINT8   battidx;
  UINT8   ia_apps_to_use;
  UINT8   turbochrg;
  UINT8   iabbth;
  UINT32  rsvd4;
  UINT8   btformat;
  UINT8   b1class;
  UINT8   b1idtolrnc;
  UINT16  b1idres;
  UINT8   b1type;
  UINT16  b1cap;
  UINT16  b1vmax;
  UINT16  b1chrterm;
  UINT16  b1lowbatt;
  UINT8   b1safeul;
  UINT8   b1safell;
  UINT16  b1t6ul;
  UINT16  b1t6fcv;
  UINT16  b1t6fci;
  UINT16  b1t6mcvstart;
  UINT16  b1t6mcvstop;
  UINT16  b1t6mci;
  UINT16  b1t5ul;
  UINT16  b1t5fcv;
  UINT16  b1t5fci;
  UINT16  b1t5mcvstart;
  UINT16  b1t5mcvstop;
  UINT16  b1t5mci;
  UINT16  b1t4ul;
  UINT16  b1t4fcv;
  UINT16  b1t4fci;
  UINT16  b1t4mcvstart;
  UINT16  b1t4mcvstop;
  UINT16  b1t4mci;
  UINT16  b1t3ul;
  UINT16  b1t3fcv;
  UINT16  b1t3fci;
  UINT16  b1t3mcvstart;
  UINT16  b1t3mcvstop;
  UINT16  b1t3mci;
  UINT16  b1t2ul;
  UINT16  b1t2fcv;
  UINT16  b1t2fci;
  UINT16  b1t2mcvstart;
  UINT16  b1t2mcvstop;
  UINT16  b1t2mci;
  UINT16  b1t1ul;
  UINT16  b1t1fcv;
  UINT16  b1t1fci;
  UINT16  b1t1mcvstart;
  UINT16  b1t1mcvstop;
  UINT16  b1t1mci;
  UINT16  b1t1ll;
  UINT8   b2class;
  UINT8   b2idtolrnc;
  UINT16  b2idres;
  UINT8   b2type;
  UINT16  b2cap;
  UINT16  b2vmax;
  UINT16  b2chrterm;
  UINT16  b2lowbatt;
  UINT8   b2safeul;
  UINT8   b2safell;
  UINT16  b2t6ul;
  UINT16  b2t6fcv;
  UINT16  b2t6fci;
  UINT16  b2t6mcvstart;
  UINT16  b2t6mcvstop;
  UINT16  b2t6mci;
  UINT16  b2t5ul;
  UINT16  b2t5fcv;
  UINT16  b2t5fci;
  UINT16  b2t5mcvstart;
  UINT16  b2t5mcvstop;
  UINT16  b2t5mci;
  UINT16  b2t4ul;
  UINT16  b2t4fcv;
  UINT16  b2t4fci;
  UINT16  b2t4mcvstart;
  UINT16  b2t4mcvstop;
  UINT16  b2t4mci;
  UINT16  b2t3ul;
  UINT16  b2t3fcv;
  UINT16  b2t3fci;
  UINT16  b2t3mcvstart;
  UINT16  b2t3mcvstop;
  UINT16  b2t3mci;
  UINT16  b2t2ul;
  UINT16  b2t2fcv;
  UINT16  b2t2fci;
  UINT16  b2t2mcvstart;
  UINT16  b2t2mcvstop;
  UINT16  b2t2mci;
  UINT16  b2t1ul;
  UINT16  b2t1fcv;
  UINT16  b2t1fci;
  UINT16  b2t1mcvstart;
  UINT16  b2t1mcvstop;
  UINT16  b2t1mci;
  UINT16  b2t1ll;
  UINT8   b3class;
  UINT8   b3idtolrnc;
  UINT16  b3idres;
  UINT8   b3type;
  UINT16  b3cap;
  UINT16  b3vmax;
  UINT16  b3chrterm;
  UINT16  b3lowbatt;
  UINT8   b3safeul;
  UINT8   b3safell;
  UINT16  b3t6ul;
  UINT16  b3t6fcv;
  UINT16  b3t6fci;
  UINT16  b3t6mcvstart;
  UINT16  b3t6mcvstop;
  UINT16  b3t6mci;
  UINT16  b3t5ul;
  UINT16  b3t5fcv;
  UINT16  b3t5fci;
  UINT16  b3t5mcvstart;
  UINT16  b3t5mcvstop;
  UINT16  b3t5mci;
  UINT16  b3t4ul;
  UINT16  b3t4fcv;
  UINT16  b3t4fci;
  UINT16  b3t4mcvstart;
  UINT16  b3t4mcvstop;
  UINT16  b3t4mci;
  UINT16  b3t3ul;
  UINT16  b3t3fcv;
  UINT16  b3t3fci;
  UINT16  b3t3mcvstart;
  UINT16  b3t3mcvstop;
  UINT16  b3t3mci;
  UINT16  b3t2ul;
  UINT16  b3t2fcv;
  UINT16  b3t2fci;
  UINT16  b3t2mcvstart;
  UINT16  b3t2mcvstop;
  UINT16  b3t2mci;
  UINT16  b3t1ul;
  UINT16  b3t1fcv;
  UINT16  b3t1fci;
  UINT16  b3t1mcvstart;
  UINT16  b3t1mcvstop;
  UINT16  b3t1mci;
  UINT16  b3t1ll;
  UINT8   b4class;
  UINT8   b4idtolrnc;
  UINT16  b4idres;
  UINT8   b4type;
  UINT16  b4cap;
  UINT16  b4vmax;
  UINT16  b4chrterm;
  UINT16  b4lowbatt;
  UINT8   b4safeul;
  UINT8   b4safell;
  UINT16  b4t6ul;
  UINT16  b4t6fcv;
  UINT16  b4t6fci;
  UINT16  b4t6mcvstart;
  UINT16  b4t6mcvstop;
  UINT16  b4t6mci;
  UINT16  b4t5ul;
  UINT16  b4t5fcv;
  UINT16  b4t5fci;
  UINT16  b4t5mcvstart;
  UINT16  b4t5mcvstop;
  UINT16  b4t5mci;
  UINT16  b4t4ul;
  UINT16  b4t4fcv;
  UINT16  b4t4fci;
  UINT16  b4t4mcvstart;
  UINT16  b4t4mcvstop;
  UINT16  b4t4mci;
  UINT16  b4t3ul;
  UINT16  b4t3fcv;
  UINT16  b4t3fci;
  UINT16  b4t3mcvstart;
  UINT16  b4t3mcvstop;
  UINT16  b4t3mci;
  UINT16  b4t2ul;
  UINT16  b4t2fcv;
  UINT16  b4t2fci;
  UINT16  b4t2mcvstart;
  UINT16  b4t2mcvstop;
  UINT16  b4t2mci;
  UINT16  b4t1ul;
  UINT16  b4t1fcv;
  UINT16  b4t1fci;
  UINT16  b4t1mcvstart;
  UINT16  b4t1mcvstop;
  UINT16  b4t1mci;
  UINT16  b4t1ll;
} IAFWBatteryConfig;

typedef struct _PlatfromMultiBOMConf_
{
  UINT8  PanelSel;
  UINT8  WorldCameraSel;
  UINT8  UserCameraSel;
  UINT8  AudioSel;
  UINT8  ModemSel;
  UINT8  TouchSel;
  UINT8  WifiSel;
} PlatfromMultiBOMConf;

typedef struct _IAFWMultiBOMConfig_
{
  PlatfromMultiBOMConf  PlatfromMultiBOM0;
  PlatfromMultiBOMConf  PlatfromMultiBOM1;
  PlatfromMultiBOMConf  PlatfromMultiBOM2;
  PlatfromMultiBOMConf  PlatfromMultiBOM3;
  PlatfromMultiBOMConf  PlatfromMultiBOM4;
} IAFWMultiBOMConfig;

typedef struct _PlatfromFeatureConf_
{
  UINT8  OsSelection;
  UINT8  Vibrator;
  UINT8  Ssic1Support;
  UINT8  ScUsbOtg;
} PlatfromFeatureConf;

typedef struct _IAFWFeatureConfig_
{
  PlatfromFeatureConf  PlatfromFeatureConf0;
  PlatfromFeatureConf  PlatfromFeatureConf1;
  PlatfromFeatureConf  PlatfromFeatureConf2;
  PlatfromFeatureConf  PlatfromFeatureConf3;
  PlatfromFeatureConf  PlatfromFeatureConf4;
} IAFWFeatureConfig;

typedef struct _PlatfromDramConf_
{
  UINT8  Package;
  UINT8  Profile;
  UINT8  MemoryDown;
  UINT8  DDR3LPageSize;
  UINT8  DDR3LASR ;
  UINT8  ScramblerSupport;
  UINT16 ChannelHashMask;
  UINT16 SliceHashMask;
  UINT8  InterleavedMode;
  UINT8  ChannelsSlicesEnabled;
  UINT8  MinRefRate2xEnabled;
  UINT8  DualRankSupportEnabled;
  UINT8  DisableFastBoot;
  UINT8  RmtMode;
  UINT8  RmtCheckRun;
  UINT16 RmtMarginCheckScaleHighThreshold;
  UINT16 MemorySizeLimit;
  UINT16 LowMemMaxVal;
  UINT16 HighMemMaxVal;
  UINT8  SpdAddress0;
  UINT8  SpdAddress1;
  UINT8  Ch0RankEnabled;
  UINT8  Ch0DeviceWidth;
  UINT8  Ch0DramDensity;
  UINT8  Ch0Option;
  UINT8  Ch0OdtConfig;
  UINT8  Ch0TristateClk1;
  UINT8  Ch0Mode2N;
  UINT8  Ch0OdtLevels;
  UINT8  Ch1RankEnabled;
  UINT8  Ch1DeviceWidth;
  UINT8  Ch1DramDensity;
  UINT8  Ch1Option;
  UINT8  Ch1OdtConfig;
  UINT8  Ch1TristateClk1;
  UINT8  Ch1Mode2N;
  UINT8  Ch1OdtLevels;
  UINT8  Ch2RankEnabled;
  UINT8  Ch2DeviceWidth;
  UINT8  Ch2DramDensity;
  UINT8  Ch2Option;
  UINT8  Ch2OdtConfig;
  UINT8  Ch2TristateClk1;
  UINT8  Ch2Mode2N;
  UINT8  Ch2OdtLevels;
  UINT8  Ch3RankEnabled;
  UINT8  Ch3DeviceWidth;
  UINT8  Ch3DramDensity;
  UINT8  Ch3Option;
  UINT8  Ch3OdtConfig;
  UINT8  Ch3TristateClk1;
  UINT8  Ch3Mode2N;
  UINT8  Ch3OdtLevels;
  UINT8  Ch0_Bit00_swizzling;
  UINT8  Ch0_Bit01_swizzling;
  UINT8  Ch0_Bit02_swizzling;
  UINT8  Ch0_Bit03_swizzling;
  UINT8  Ch0_Bit04_swizzling;
  UINT8  Ch0_Bit05_swizzling;
  UINT8  Ch0_Bit06_swizzling;
  UINT8  Ch0_Bit07_swizzling;
  UINT8  Ch0_Bit08_swizzling;
  UINT8  Ch0_Bit09_swizzling;
  UINT8  Ch0_Bit10_swizzling;
  UINT8  Ch0_Bit11_swizzling;
  UINT8  Ch0_Bit12_swizzling;
  UINT8  Ch0_Bit13_swizzling;
  UINT8  Ch0_Bit14_swizzling;
  UINT8  Ch0_Bit15_swizzling;
  UINT8  Ch0_Bit16_swizzling;
  UINT8  Ch0_Bit17_swizzling;
  UINT8  Ch0_Bit18_swizzling;
  UINT8  Ch0_Bit19_swizzling;
  UINT8  Ch0_Bit20_swizzling;
  UINT8  Ch0_Bit21_swizzling;
  UINT8  Ch0_Bit22_swizzling;
  UINT8  Ch0_Bit23_swizzling;
  UINT8  Ch0_Bit24_swizzling;
  UINT8  Ch0_Bit25_swizzling;
  UINT8  Ch0_Bit26_swizzling;
  UINT8  Ch0_Bit27_swizzling;
  UINT8  Ch0_Bit28_swizzling;
  UINT8  Ch0_Bit29_swizzling;
  UINT8  Ch0_Bit30_swizzling;
  UINT8  Ch0_Bit31_swizzling;
  UINT8  Ch1_Bit00_swizzling;
  UINT8  Ch1_Bit01_swizzling;
  UINT8  Ch1_Bit02_swizzling;
  UINT8  Ch1_Bit03_swizzling;
  UINT8  Ch1_Bit04_swizzling;
  UINT8  Ch1_Bit05_swizzling;
  UINT8  Ch1_Bit06_swizzling;
  UINT8  Ch1_Bit07_swizzling;
  UINT8  Ch1_Bit08_swizzling;
  UINT8  Ch1_Bit09_swizzling;
  UINT8  Ch1_Bit10_swizzling;
  UINT8  Ch1_Bit11_swizzling;
  UINT8  Ch1_Bit12_swizzling;
  UINT8  Ch1_Bit13_swizzling;
  UINT8  Ch1_Bit14_swizzling;
  UINT8  Ch1_Bit15_swizzling;
  UINT8  Ch1_Bit16_swizzling;
  UINT8  Ch1_Bit17_swizzling;
  UINT8  Ch1_Bit18_swizzling;
  UINT8  Ch1_Bit19_swizzling;
  UINT8  Ch1_Bit20_swizzling;
  UINT8  Ch1_Bit21_swizzling;
  UINT8  Ch1_Bit22_swizzling;
  UINT8  Ch1_Bit23_swizzling;
  UINT8  Ch1_Bit24_swizzling;
  UINT8  Ch1_Bit25_swizzling;
  UINT8  Ch1_Bit26_swizzling;
  UINT8  Ch1_Bit27_swizzling;
  UINT8  Ch1_Bit28_swizzling;
  UINT8  Ch1_Bit29_swizzling;
  UINT8  Ch1_Bit30_swizzling;
  UINT8  Ch1_Bit31_swizzling;
  UINT8  Ch2_Bit00_swizzling;
  UINT8  Ch2_Bit01_swizzling;
  UINT8  Ch2_Bit02_swizzling;
  UINT8  Ch2_Bit03_swizzling;
  UINT8  Ch2_Bit04_swizzling;
  UINT8  Ch2_Bit05_swizzling;
  UINT8  Ch2_Bit06_swizzling;
  UINT8  Ch2_Bit07_swizzling;
  UINT8  Ch2_Bit08_swizzling;
  UINT8  Ch2_Bit09_swizzling;
  UINT8  Ch2_Bit10_swizzling;
  UINT8  Ch2_Bit11_swizzling;
  UINT8  Ch2_Bit12_swizzling;
  UINT8  Ch2_Bit13_swizzling;
  UINT8  Ch2_Bit14_swizzling;
  UINT8  Ch2_Bit15_swizzling;
  UINT8  Ch2_Bit16_swizzling;
  UINT8  Ch2_Bit17_swizzling;
  UINT8  Ch2_Bit18_swizzling;
  UINT8  Ch2_Bit19_swizzling;
  UINT8  Ch2_Bit20_swizzling;
  UINT8  Ch2_Bit21_swizzling;
  UINT8  Ch2_Bit22_swizzling;
  UINT8  Ch2_Bit23_swizzling;
  UINT8  Ch2_Bit24_swizzling;
  UINT8  Ch2_Bit25_swizzling;
  UINT8  Ch2_Bit26_swizzling;
  UINT8  Ch2_Bit27_swizzling;
  UINT8  Ch2_Bit28_swizzling;
  UINT8  Ch2_Bit29_swizzling;
  UINT8  Ch2_Bit30_swizzling;
  UINT8  Ch2_Bit31_swizzling;
  UINT8  Ch3_Bit00_swizzling;
  UINT8  Ch3_Bit01_swizzling;
  UINT8  Ch3_Bit02_swizzling;
  UINT8  Ch3_Bit03_swizzling;
  UINT8  Ch3_Bit04_swizzling;
  UINT8  Ch3_Bit05_swizzling;
  UINT8  Ch3_Bit06_swizzling;
  UINT8  Ch3_Bit07_swizzling;
  UINT8  Ch3_Bit08_swizzling;
  UINT8  Ch3_Bit09_swizzling;
  UINT8  Ch3_Bit10_swizzling;
  UINT8  Ch3_Bit11_swizzling;
  UINT8  Ch3_Bit12_swizzling;
  UINT8  Ch3_Bit13_swizzling;
  UINT8  Ch3_Bit14_swizzling;
  UINT8  Ch3_Bit15_swizzling;
  UINT8  Ch3_Bit16_swizzling;
  UINT8  Ch3_Bit17_swizzling;
  UINT8  Ch3_Bit18_swizzling;
  UINT8  Ch3_Bit19_swizzling;
  UINT8  Ch3_Bit20_swizzling;
  UINT8  Ch3_Bit21_swizzling;
  UINT8  Ch3_Bit22_swizzling;
  UINT8  Ch3_Bit23_swizzling;
  UINT8  Ch3_Bit24_swizzling;
  UINT8  Ch3_Bit25_swizzling;
  UINT8  Ch3_Bit26_swizzling;
  UINT8  Ch3_Bit27_swizzling;
  UINT8  Ch3_Bit28_swizzling;
  UINT8  Ch3_Bit29_swizzling;
  UINT8  Ch3_Bit30_swizzling;
  UINT8  Ch3_Bit31_swizzling;
  UINT8  reserved[14];
} PlatfromDramConf;

typedef struct _IAFWDramConfig_
{
  UINT32            Platform_override;
  UINT32            BRAM_Flushing;
  UINT32            Message_level_mask;
  PlatfromDramConf  PlatformDram0;
  PlatfromDramConf  PlatformDram1;
  PlatfromDramConf  PlatformDram2;
  PlatfromDramConf  PlatformDram3;
  PlatfromDramConf  PlatformDram4;
  PlatfromDramConf  PlatformDram5;
} IAFWDramConfig;

// XML original class name: Pin
typedef struct _GpioConfig_
{
  UINT32  half0;
  UINT32  half1;
} GpioConfig;

typedef struct _GpioProfile_
{
  UINT32  offset;
  UINT32  numGpioEntries;
} GpioProfile;

typedef struct _GpioTable_
{
  UINT32      marker;
  GpioConfig  *gpioCfg;
} GpioTable;

// XML original class name: PlatformConfig
typedef struct _IAFWPlatformConfig_
{
  UINT32       numProfiles;
  GpioProfile  *profile;
} IAFWPlatformConfig;

typedef struct _BoardINFO_
{
  UINT8  BOARDID;
  UINT8  FABID;
} BoardINFO;

typedef struct _IAFWSOCSkuInfo_
{
  BoardINFO  Board0;
} IAFWSOCSkuInfo;

typedef struct _IafwBiosKnobs_
{
  UINT8  Reserved1;
  UINT8  Reserved2;
  UINT8  Reserved3;
  UINT8  Reserved4;
  UINT8  Reserved5;
  UINT8  Reserved6;
  UINT8  BootMode;
  UINT8  HifType;
} IafwBiosKnobs;

typedef struct _IafwSmipLayout_
{
  UINT32              IafwSmipSignature;
  UINT8               IafwSmipVersion;
  IafwBiosKnobs       Reserved;
  Bcu                 BatteryCtrlUnit;
  IAFWBatteryConfig   BatteryConfig;
  IAFWMultiBOMConfig  MultiBOMConfig;
  IAFWFeatureConfig   FeatureConfig;
  IAFWDramConfig      DramConfig;
  IAFWSOCSkuInfo      SOCSkuInfo;
  IAFWPlatformConfig  PlatformConfig;
  UINT32              IafwSmipFooter;
} IafwSmipLayout;

#pragma pack(pop)
#endif

