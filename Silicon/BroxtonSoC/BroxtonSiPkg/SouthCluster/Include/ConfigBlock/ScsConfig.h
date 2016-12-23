/** @file
  Scs policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SCS_CONFIG_H_
#define _SCS_CONFIG_H_

#define SCS_CONFIG_REVISION 3

extern EFI_GUID gScsConfigGuid;

#pragma pack (push,1)

//
// Device Operating Mode
//
typedef enum {
  ScDisabled = 0,
  ScPciMode  = 1,
  ScAcpiMode = 2,
  ScDevModeMax
} SC_DEV_MODE;

//
// SCC eMMC Host Speed
//
enum SCC_EMMC_MODE {
  SccEmmcHs400 = 0,
  SccEmmcHs200 = 1,
  SccEmmcDdr50 = 2,
};

/**
  The SDIO_REG_CONFIG block is the structure defined in SC_SCS_CONFIG for SC.
  @note: the order defined below is for SDIO DLL registers settings, and MUST not change.
  Items defined will be accessed by its index in ConfigureSdioDll module

**/
typedef struct {
  UINT32  TxCmdCntl;
  UINT32  TxDataCntl1;
  UINT32  TxDataCntl2;
  UINT32  RxCmdDataCntl1;
  UINT32  RxCmdDataCntl2;
} SDIO_REG_CONFIG;

/**
  The SDCARD_REG_CONFIG block is the structure defined in SC_SCS_CONFIG for SC.
  @note: the order defined below is for SDCARD DLL registers settings, and MUST not change.
  Items defined will be accessed by its index in ConfigureSdioDll module

**/
typedef struct {
  UINT32  TxCmdCntl;
  UINT32  TxDataCntl1;
  UINT32  TxDataCntl2;
  UINT32  RxCmdDataCntl1;
  UINT32  RxStrobeCntl;
  UINT32  RxCmdDataCntl2;
} SDCARD_REG_CONFIG;

/**
  The EMMC_REG_CONFIG block is the structure defined in SC_SCS_CONFIG for SC.
  @note: the order defined below is for EMMC DLL registers settings, and MUST not change.
  Items defined will be accessed by its index in ConfigureSdioDll module
**/
typedef struct {
  UINT32  TxCmdCntl;
  UINT32  TxDataCntl1;
  UINT32  TxDataCntl2;
  UINT32  RxCmdDataCntl1;
  UINT32  RxStrobeCntl;
  UINT32  RxCmdDataCntl2;
  UINT32  MasterSwCntl;
} EMMC_REG_CONFIG;

//
// SCC eMMC Trace Length
//
#define SCC_EMMC_LONG_TRACE_LEN  0
#define SCC_EMMC_SHORT_TRACE_LEN 1

/**
  The SC_SCS_CONFIG block describes Storage and Communication Subsystem (SCS) settings for SC.
  @note: the order defined below is per the PCI BDF sequence, and MUST not change.
  Items defined will be accessed by its index in ScInit module
**/
typedef struct {
/**
    Revision 1: Init version
    Revision 2: Add SccEmmcTraceLength
    Revision 3: Add SDIO_REG_CONFIG and SDCARD_REG_CONFIG and EMMC_REG_CONFIG structures.
**/
  CONFIG_BLOCK_HEADER   Header;                   ///< Config Block Header //ChangeLog, Revision 1: Init version, Revision 2: Add SccEmmcTraceLength
  SC_DEV_MODE           SdcardEnable;             ///< Determine if SD Card is enabled - 0: Disabled, <b>1: Enabled</b>. Default is <b>ScPciMode</b>
  SC_DEV_MODE           EmmcEnable;               ///< Determine if eMMC is enabled - 0: Disabled, <b>1: Enabled</b>. Default is <b>ScPciMode</b>
  SC_DEV_MODE           UfsEnable;                ///< Determine if UFS is enabled - <b>0: Disabled</b>, 1: Enabled.
  SC_DEV_MODE           SdioEnable;               ///< Determine if SDIO is enabled - 0: Disabled, <b>1: Enabled</b>. Default is <b>ScPciMode</b>
  UINT32                EmmcHostMaxSpeed   : 2;   ///< Determine eMMC Mode. Default is <b>0: HS400</b>, 1: HS200, 2:DDR50
  UINT32                GppLock            : 1;   ///< Determine if GPP is locked <b>0: Disabled</b>; 1: Enabled
  UINT32                SccEmmcTraceLength : 2;   ///< Determine EMMC Trace length - 0: Longer Trace, Validated on Rvp <b>1: Shorter trace, Validated on FFD</b>. Default is <b>SCC_EMMC_LONG_TRACE_LEN</b>
  UINT32                RsvdBits           : 27;  ///< Reserved bits
  SDIO_REG_CONFIG       SdioRegDllConfig;         ///< SDIO DLL Configure structure
  SDCARD_REG_CONFIG     SdcardRegDllConfig;       ///< SDCARD DLL Configure structure
  EMMC_REG_CONFIG       EmmcRegDllConfig;         ///< eMMC DLL Configure structure
} SC_SCS_CONFIG;

#pragma pack (pop)

#endif // _SCS_CONFIG_H_

