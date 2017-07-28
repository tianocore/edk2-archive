/** @file
  Internal and external data structures, Macros, and enumerations.

  Copyright (c) 2005 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _MMRCDATA_H_
#define _MMRCDATA_H_

#include "Spd.h"
#include "MmrcProjectDefinitions.h"

#if SIM || JTAG
#include <stdio.h>
#endif

///
/// MRC version description.
///
typedef union {
  struct{
    UINT8      Major;             ///< Major version number
    UINT8      Minor;             ///< Minor version number
    UINT8      Rev;               ///< Revision number
    UINT8      Build;             ///< Build number
  } Version;
  struct{
    UINT32     Major     : 4;        ///< Major version number
    UINT32     DevId     : 3;        ///< Field that can be used to identify dev that built test BIOS
    UINT32     TestBuild : 1;        ///< Flag that indicates that BIOS was built on a dev machine
    UINT32     Minor     : 8;        ///< Minor version number
    UINT32     Rev       : 8;        ///< Revision number
    UINT32     Build     : 8;        ///< Build number
  } Bits;
  UINT32     Data;
  UINT16     Data16[2];
  UINT8      Data8[4];
} MrcVersion;

typedef union {
  UINT8      Data;
  struct {

    UINT8    RankSelectInterleavingEnable : 1;

    UINT8    BankAddressHashingEnable : 1;

    UINT8    Ch1ClkDisable : 1;

    UINT8    Reserved : 1;

    UINT8    AddressMapping : 2;

    UINT8    Reserved0 : 2;
  } Bits;
} CHANNEL_OPTION;


#ifndef ABSOLUTE
#define ABSOLUTE                      1
#define RELATIVE                      2
#endif

#pragma pack(push)
#pragma pack(1)

typedef enum {
  BootFrequency,
  HighFrequency,
  MaxFrequencyIndex
} FREQUENCY_INDEX;

//
// Bit definitions
//
#ifndef BIT0
#define BIT0                  0x00000001
#define BIT1                  0x00000002
#define BIT2                  0x00000004
#define BIT3                  0x00000008
#define BIT4                  0x00000010
#define BIT5                  0x00000020
#define BIT6                  0x00000040
#define BIT7                  0x00000080
#define BIT8                  0x00000100
#define BIT9                  0x00000200
#define BIT10                 0x00000400
#define BIT11                 0x00000800
#define BIT12                 0x00001000
#define BIT13                 0x00002000
#define BIT14                 0x00004000
#define BIT15                 0x00008000
#define BIT16                 0x00010000
#define BIT17                 0x00020000
#define BIT18                 0x00040000
#define BIT19                 0x00080000
#define BIT20                 0x00100000
#define BIT21                 0x00200000
#define BIT22                 0x00400000
#define BIT23                 0x00800000
#define BIT24                 0x01000000
#define BIT25                 0x02000000
#define BIT26                 0x04000000
#define BIT27                 0x08000000
#define BIT28                 0x10000000
#define BIT29                 0x20000000
#define BIT30                 0x40000000
#define BIT31                 0x80000000
#endif


#pragma pack(1)

typedef enum  {
  Pfct =  0,
  PfctT,
  PfctC,
  PfctCT,
  PfctF,
  PfctFT,
  PfctFC,
  PfctFCT,
  PfctP,
  PfctPT,
  PfctPC,
  PfctPCT,
  PfctPF,
  PfctPFT,
  PfctPFC,
  PfctPFCT,
  MaxPfct,
} ENUM_PCFT_TYPE;

typedef struct {
  UINT8   SpdPresent;
  UINT8   Buffer[MAX_SPD_ADDR + 1];
} SPD_DATA;

/**
  Final training values stored on a per blueprint level. Needs to be per blueprint
  in case of a system with more than 1 level of memory per channel.

**/
typedef struct {
  UINT16    Values[MAX_BLUEPRINTS][MAX_NUM_ALGOS][MAX_RANKS][MAX_STROBES];
} TRAINING_SETTING;

typedef struct {
  UINT8  ManuIDlo;
  UINT8  ManuIDhi;
  UINT8  ManuLoc;
  UINT8  ManuDateLO;
  UINT8  ManuDateHI;
  UINT8  SerialNumber1;
  UINT8  SerialNumber2;
  UINT8  SerialNumber3;
  UINT8  SerialNumber4;
  UINT8  DimmCount;
} FASTBOOTDATA;

//
// BOOT_VARIABLE_NV_DATA
// Use this structure to store NV data that changes from boot to boot.
//
typedef struct {
  UINT16                    ScramblerSeed[MAX_CHANNELS];
  UINT16                    LP4_MR4VALUE[MAX_CHANNELS];
  UINT32                    BootVariableNvDataCrc;
} BOOT_VARIABLE_NV_DATA;

typedef struct {
  TRAINING_SETTING          Trained_Value;
  BOOLEAN                   Enabled;
  UINT32                    TotalMem;
  UINT8                     DimmCount;
  UINT8                     DimmPresent[MAX_DIMMS];
  UINT32                    SlotMem[MAX_DIMMS];
  SPD_DATA                  SpdData[MAX_DIMMS];
  UINT8                     D_Ranks[MAX_DIMMS];
  UINT8                     D_DataWidth[MAX_DIMMS];
  UINT8                     D_Banks[MAX_DIMMS];
  UINT8                     D_BusWidth[MAX_DIMMS];
  UINT8                     D_Size_SPD[MAX_DIMMS];
  UINT8                     D_Size[MAX_DIMMS];
  UINT8                     D_SizeActual[MAX_DIMMS];
  UINT8                     D_Type[MAX_DIMMS];
  UINT16                    DimmFrequency[MAX_DIMMS];
  BOOLEAN                   RankEnabled[MAX_RANKS];
  BOOLEAN                   EccEnabled;
  UINT32                    EccStrobes;
  UINT8                     MaxDq[MAX_RANKS];
  FASTBOOTDATA              FastBootData[MAX_DIMMS];
  UINT16                    DramType;
  UINT16                    CurrentPlatform;
  UINT16                    CurrentFrequency;
  UINT16                    CurrentConfiguration;
  UINT16                    CurrentDdrType;
  BOOLEAN                   ExecuteOnThisChannel;
  UINT16                    TimingData[16];
  INT16                     RMT_Data[MAX_RANKS + PSEUDO_RANKS][MAX_RMT_ELEMENTS][2];
#if RMT_PER_BIT_MARGIN
  INT16                     RMT_DQData[MAX_RANKS + PSEUDO_RANKS][MAX_RMT_ELEMENTS][MAX_BITS*MAX_STROBES][2];
#endif
  UINT8                     Tcl;
  UINT8                     VrefSmbAddress;
  UINT32                    PtCtl0;
  UINT8                     Dimm2DunitMapping[MAX_STROBES*8];
  UINT8                     Dimm2DunitReverseMapping[MAX_STROBES*8];
#if SIM && DDR4_SUPPORT
  UINT16                    TbMr5;
#endif
#if FAULTY_PART_TRACKING
  FAULTY_PART               FaultyPart[MAX_RANKS];
#endif
  UINT32                    WrVref[MAX_RANKS][MAX_STROBES];
  UINT32                    CaVref[MAX_RANKS];
  UINT32                    Storage[18];
  UINT8                     ASR_Supported;
  UINT8                     OdtHigh;
  UINT16                    LP4_MR0VALUE;
  CHANNEL_OPTION            ChOption;
} CHANNEL;

typedef struct {
  UINT32        Signature;
  UINT16        CurrentFrequency;
  UINT8         DVFSFreqIndex;
  UINT16        DDR3L_MRVALUE;
  UINT8         DDR3L_PageSize;
  UINT32        SystemMemorySize;
  UINT32        MrcVer;
  UINT32        CachedValues[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS][MAX_RANKS][MAX_STROBES];
  UINT32        CachedValuesPB[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS_PB][MAX_RANKS][MAX_STROBES][MAX_BITS];
  UINT8         BusWidth;
  CHANNEL       Channel[MAX_CHANNELS];
  UINT32        MarginCheckResultState;
  DRAM_POLICY   DramPolicyData;
  UINT32        MuxcodeNv[MaxFrequencyIndex];
  BOOLEAN       SetupMaxPiDone[MaxFrequencyIndex];
  UINT32        DataSize;
  UINT32        SaMemCfgCrc;
  UINT32        MrcParamsSaveRestoreCrc;
} MRC_PARAMS_SAVE_RESTORE;

//
// Structure for all data that is saved to NV RAM
// and restored on the subsequent boots
//
typedef struct {
  MRC_PARAMS_SAVE_RESTORE MrcParamsSaveRestore;
  BOOT_VARIABLE_NV_DATA   BootVariableNvData;
} MRC_NV_DATA_FRAME;

#pragma pack(pop)
#endif

