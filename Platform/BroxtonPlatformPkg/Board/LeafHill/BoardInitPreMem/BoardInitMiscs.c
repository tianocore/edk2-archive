/** @file
  This file does Multiplatform initialization.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitMiscs.h"
#include "MmrcData.h"

UPDATE_FSPM_UPD_FUNC mLhUpdateFspmUpdPtr = LhUpdateFspmUpd;
DRAM_CREATE_POLICY_DEFAULTS_FUNC   mLhDramCreatePolicyDefaultsPtr = LhDramCreatePolicyDefaults;

EFI_STATUS
EFIAPI
LhUpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  )
{
  EFI_PEI_HOB_POINTERS           Hob;
  EFI_PLATFORM_INFO_HOB          *PlatformInfo = NULL;
  DRAM_POLICY_PPI                *DramPolicy;
  EFI_STATUS                     Status;
  MRC_PARAMS_SAVE_RESTORE        *MrcNvData;
  BOOT_VARIABLE_NV_DATA          *BootVariableNvData;
  MRC_PARAMS_SAVE_RESTORE        *MrcParamsHob;
  BOOT_VARIABLE_NV_DATA          *BootVariableNvDataHob;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gDramPolicyPpiGuid,
                             0,
                             NULL,
                             (VOID **) &DramPolicy
                             );

  if (!EFI_ERROR (Status)) {
    FspUpdRgn->FspmConfig.Package                           = DramPolicy->Package;
    FspUpdRgn->FspmConfig.Profile                           = DramPolicy->Profile;
    FspUpdRgn->FspmConfig.MemoryDown                        = DramPolicy->MemoryDown;
    FspUpdRgn->FspmConfig.DDR3LPageSize                     = DramPolicy->DDR3LPageSize;
    FspUpdRgn->FspmConfig.DDR3LASR                          = DramPolicy->DDR3LASR;
    FspUpdRgn->FspmConfig.MemorySizeLimit                   = DramPolicy->SystemMemorySizeLimit;
    FspUpdRgn->FspmConfig.DIMM0SPDAddress                   = DramPolicy->SpdAddress[0];
    FspUpdRgn->FspmConfig.DIMM1SPDAddress                   = DramPolicy->SpdAddress[1];
    FspUpdRgn->FspmConfig.DDR3LPageSize                     = DramPolicy->DDR3LPageSize;
    FspUpdRgn->FspmConfig.DDR3LASR                          = DramPolicy->DDR3LASR;
    FspUpdRgn->FspmConfig.HighMemoryMaxValue                = DramPolicy->HighMemMaxVal;
    FspUpdRgn->FspmConfig.LowMemoryMaxValue                 = DramPolicy->LowMemMaxVal;
    FspUpdRgn->FspmConfig.DisableFastBoot                   = DramPolicy->DisableFastBoot;
    FspUpdRgn->FspmConfig.RmtMode                           = DramPolicy->RmtMode;
    FspUpdRgn->FspmConfig.RmtCheckRun                       = DramPolicy->RmtCheckRun;
    FspUpdRgn->FspmConfig.RmtMarginCheckScaleHighThreshold  = DramPolicy->RmtMarginCheckScaleHighThreshold;
    FspUpdRgn->FspmConfig.MsgLevelMask                      = DramPolicy->MsgLevelMask;

    FspUpdRgn->FspmConfig.ChannelHashMask                   = DramPolicy->ChannelHashMask;
    FspUpdRgn->FspmConfig.SliceHashMask                     = DramPolicy->SliceHashMask;
    FspUpdRgn->FspmConfig.ChannelsSlicesEnable              = DramPolicy->ChannelsSlicesEnabled;
    FspUpdRgn->FspmConfig.ScramblerSupport                  = DramPolicy->ScramblerSupport;
    FspUpdRgn->FspmConfig.InterleavedMode                   = DramPolicy->InterleavedMode;
    FspUpdRgn->FspmConfig.MinRefRate2xEnable                = DramPolicy->MinRefRate2xEnabled;
    FspUpdRgn->FspmConfig.DualRankSupportEnable             = DramPolicy->DualRankSupportEnabled;

    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_RankEnable), &DramPolicy->ChDrp, sizeof (DramPolicy->ChDrp));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), &DramPolicy->ChSwizzle, sizeof (DramPolicy->ChSwizzle));

    if (((VOID *)(UINT32)DramPolicy->MrcTrainingDataPtr != 0) &&
        ((VOID *)(UINT32)DramPolicy->MrcBootDataPtr     != 0)) {
      DEBUG ((DEBUG_INFO, "UpdateFspmUpd - NvsBufferPtr\n"));
      MrcNvData          = (MRC_PARAMS_SAVE_RESTORE *) AllocateZeroPool (sizeof (MRC_PARAMS_SAVE_RESTORE));
      BootVariableNvData = (BOOT_VARIABLE_NV_DATA *) AllocateZeroPool (sizeof (BOOT_VARIABLE_NV_DATA));

      MrcParamsHob          = (MRC_PARAMS_SAVE_RESTORE*)((UINT32)DramPolicy->MrcTrainingDataPtr);
      BootVariableNvDataHob = (BOOT_VARIABLE_NV_DATA*)((UINT32)DramPolicy->MrcBootDataPtr);

      CopyMem(MrcNvData, MrcParamsHob, sizeof (MRC_PARAMS_SAVE_RESTORE));
      CopyMem(BootVariableNvData, BootVariableNvDataHob, sizeof (BOOT_VARIABLE_NV_DATA));
      FspUpdRgn->FspmArchUpd.NvsBufferPtr        = (VOID *)(UINT32)MrcNvData;
      FspUpdRgn->FspmConfig.VariableNvsBufferPtr = (VOID *)(UINT32)BootVariableNvData;
    }

  }
  //
  // override RankEnable settings for Minnow
  //
  FspUpdRgn->FspmConfig.Ch0_RankEnable   = 1;
  FspUpdRgn->FspmConfig.Ch1_RankEnable   = 1;
  FspUpdRgn->FspmConfig.Ch2_RankEnable   = 1;
  FspUpdRgn->FspmConfig.Ch3_RankEnable   = 1;

  DEBUG ((DEBUG_INFO, "UpdateFspmUpd - gEfiPlatformInfoGuid\n"));
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  DEBUG ((DEBUG_INFO, "***** LeafHill - UpdateFspmUpd,BoardId = %d\n", PlatformInfo->BoardId));

  if (PlatformInfo->BoardId != BOARD_ID_LFH_CRB) {
    //
    // ASSERT false if BoardId isn't LeafHill
    //
    ASSERT (FALSE);
  }

  FspUpdRgn->FspmConfig.Package         = 1;
  FspUpdRgn->FspmConfig.Profile         = 11;
  FspUpdRgn->FspmConfig.MemoryDown      = 1;
  FspUpdRgn->FspmConfig.DDR3LPageSize   = 0;
  FspUpdRgn->FspmConfig.DDR3LASR        = 0;
  FspUpdRgn->FspmConfig.MemorySizeLimit = 0;
  FspUpdRgn->FspmConfig.DIMM0SPDAddress = 0;
  FspUpdRgn->FspmConfig.DIMM1SPDAddress = 0;
  FspUpdRgn->FspmConfig.DDR3LPageSize   = 0;
  FspUpdRgn->FspmConfig.DDR3LASR        = 0;

  FspUpdRgn->FspmConfig.Ch0_RankEnable   = 3;
  FspUpdRgn->FspmConfig.Ch0_DeviceWidth  = 1;
  FspUpdRgn->FspmConfig.Ch0_DramDensity  = 2;
  FspUpdRgn->FspmConfig.Ch0_Option       = 3;

  FspUpdRgn->FspmConfig.Ch1_RankEnable   = 3;
  FspUpdRgn->FspmConfig.Ch1_DeviceWidth  = 1;
  FspUpdRgn->FspmConfig.Ch1_DramDensity  = 2;
  FspUpdRgn->FspmConfig.Ch1_Option       = 3;

  FspUpdRgn->FspmConfig.Ch2_RankEnable   = 3;
  FspUpdRgn->FspmConfig.Ch2_DeviceWidth  = 1;
  FspUpdRgn->FspmConfig.Ch2_DramDensity  = 2;
  FspUpdRgn->FspmConfig.Ch2_Option       = 3;

  FspUpdRgn->FspmConfig.Ch3_RankEnable   = 3;
  FspUpdRgn->FspmConfig.Ch3_DeviceWidth  = 1;
  FspUpdRgn->FspmConfig.Ch3_DramDensity  = 2;
  FspUpdRgn->FspmConfig.Ch3_Option       = 3;

  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[0]   = 0x09;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[1]   = 0x0e;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[2]   = 0x0c;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[3]   = 0x0d;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[4]   = 0x0a;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[5]   = 0x0b;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[6]   = 0x08;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[7]   = 0x0f;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[8]   = 0x05;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[9]   = 0x06;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[10]  = 0x01;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[11]  = 0x00;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[12]  = 0x02;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[13]  = 0x07;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[14]  = 0x04;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[15]  = 0x03;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[16]  = 0x1a;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[17]  = 0x1f;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[18]  = 0x1c;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[19]  = 0x1b;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[20]  = 0x1d;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[21]  = 0x19;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[22]  = 0x18;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[23]  = 0x1e;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[24]  = 0x14;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[25]  = 0x16;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[26]  = 0x17;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[27]  = 0x11;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[28]  = 0x12;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[29]  = 0x13;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[30]  = 0x10;
  FspUpdRgn->FspmConfig.Ch0_Bit_swizzling[31]  = 0x15;

  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[0]   = 0x06;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[1]   = 0x07;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[2]   = 0x05;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[3]   = 0x04;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[4]   = 0x03;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[5]   = 0x01;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[6]   = 0x00;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[7]   = 0x02;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[8]   = 0x0c;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[9]   = 0x0a;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[10]  = 0x0b;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[11]  = 0x0d;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[12]  = 0x0e;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[13]  = 0x08;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[14]  = 0x09;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[15]  = 0x0f;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[16]  = 0x14;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[17]  = 0x10;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[18]  = 0x16;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[19]  = 0x15;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[20]  = 0x12;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[21]  = 0x11;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[22]  = 0x13;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[23]  = 0x17;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[24]  = 0x1e;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[25]  = 0x1c;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[26]  = 0x1d;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[27]  = 0x19;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[28]  = 0x18;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[29]  = 0x1a;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[30]  = 0x1b;
  FspUpdRgn->FspmConfig.Ch1_Bit_swizzling[31]  = 0x1f;

  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[0]   = 0x0f;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[1]   = 0x09;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[2]   = 0x08;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[3]   = 0x0b;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[4]   = 0x0c;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[5]   = 0x0d;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[6]   = 0x0e;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[7]   = 0x0a;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[8]   = 0x05;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[9]   = 0x02;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[10]  = 0x00;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[11]  = 0x03;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[12]  = 0x06;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[13]  = 0x07;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[14]  = 0x01;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[15]  = 0x04;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[16]  = 0x19;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[17]  = 0x1c;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[18]  = 0x1e;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[19]  = 0x1f;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[20]  = 0x1a;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[21]  = 0x1b;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[22]  = 0x18;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[23]  = 0x1d;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[24]  = 0x14;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[25]  = 0x17;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[26]  = 0x16;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[27]  = 0x15;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[28]  = 0x12;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[29]  = 0x13;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[30]  = 0x10;
  FspUpdRgn->FspmConfig.Ch2_Bit_swizzling[31]  = 0x11;

  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[0]   = 0x03;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[1]   = 0x04;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[2]   = 0x06;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[3]   = 0x05;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[4]   = 0x00;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[5]   = 0x01;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[6]   = 0x02;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[7]   = 0x07;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[8]   = 0x0b;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[9]   = 0x0a;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[10]  = 0x08;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[11]  = 0x09;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[12]  = 0x0e;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[13]  = 0x0c;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[14]  = 0x0f;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[15]  = 0x0d;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[16]  = 0x11;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[17]  = 0x17;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[18]  = 0x13;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[19]  = 0x10;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[20]  = 0x15;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[21]  = 0x16;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[22]  = 0x14;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[23]  = 0x12;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[24]  = 0x1c;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[25]  = 0x1d;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[26]  = 0x1a;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[27]  = 0x19;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[28]  = 0x1e;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[29]  = 0x1b;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[30]  = 0x18;
  FspUpdRgn->FspmConfig.Ch3_Bit_swizzling[31]  = 0x1f;

  FspUpdRgn->FspmConfig.ChannelHashMask       = 0;
  FspUpdRgn->FspmConfig.SliceHashMask         = 0;
  FspUpdRgn->FspmConfig.ChannelsSlicesEnable  = 0;
  FspUpdRgn->FspmConfig.ScramblerSupport      = 1;
  FspUpdRgn->FspmConfig.InterleavedMode       = 0;
  FspUpdRgn->FspmConfig.MinRefRate2xEnable    = 0;
  FspUpdRgn->FspmConfig.DualRankSupportEnable = 1;

  return EFI_SUCCESS;
}


/**
  DramCreatePolicyDefaults creates the default setting of Dram Policy.

  @param[out] DramPolicyPpi           The pointer to get Dram Policy PPI instance

  @retval     EFI_SUCCESS             The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
LhDramCreatePolicyDefaults (
  IN  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi,
  OUT DRAM_POLICY_PPI                  **DramPolicyPpi,
  IN  IAFWDramConfig                   *DramConfigData,
  IN  UINTN                            *MrcTrainingDataAddr,
  IN  UINTN                            *MrcBootDataAddr,
  IN  UINT8                            BoardId
  )
{
  DRAM_POLICY_PPI                     *DramPolicy;
  SYSTEM_CONFIGURATION                SystemConfiguration;
  UINTN                               VariableSize;
  EFI_STATUS                          Status;
  DRP_DRAM_POLICY                     *DrpPtr;
  UINT8                               (*ChSwizlePtr)[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS];
  PlatfromDramConf                    *DramConfig;
  BOOLEAN                             ReadSetupVars;
  EFI_PLATFORM_INFO_HOB               *PlatformInfoHob = NULL;
  EFI_PEI_HOB_POINTERS                Hob;

  DEBUG ((EFI_D_INFO, "*** Leaf Hill DramCreatePolicyDefaults\n"));
  DramPolicy = (DRAM_POLICY_PPI *) AllocateZeroPool (sizeof (DRAM_POLICY_PPI));
  if (DramPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  ReadSetupVars = FALSE;
  DrpPtr        = NULL;
  ChSwizlePtr   = NULL;
  DramConfig    = NULL;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          PLATFORM_SETUP_VARIABLE_NAME,
                          &gEfiSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &SystemConfiguration
                          );

#if !(ONLY_USE_SMIP_DRAM_POLICY == 1)
  Status = EFI_UNSUPPORTED;
#endif

  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Using setup options data for DRAM policy\n"));
    ReadSetupVars = TRUE;
    DramPolicy->ChannelHashMask         = SystemConfiguration.ChannelHashMask;
    DramPolicy->SliceHashMask           = SystemConfiguration.SliceHashMask;
    DramPolicy->ChannelsSlicesEnabled   = SystemConfiguration.ChannelsSlicesEnabled;
    DramPolicy->ScramblerSupport        = SystemConfiguration.ScramblerSupport;
    DramPolicy->InterleavedMode         = SystemConfiguration.InterleavedMode;
    DramPolicy->MinRefRate2xEnabled     = SystemConfiguration.MinRefRate2xEnabled;
    DramPolicy->DualRankSupportEnabled  = SystemConfiguration.DualRankSupportEnabled;
  }

  DramConfig = &(DramConfigData->PlatformDram4);

  DEBUG ((EFI_D_INFO, "Using smip platform override: %d\n", DramConfigData->Platform_override));
  switch (DramConfigData->Platform_override) {
    case 0:
      DramConfig = &(DramConfigData->PlatformDram0);
      break;
    case 1:
      DramConfig = &(DramConfigData->PlatformDram1);
      break;
    case 2:
      DramConfig = &(DramConfigData->PlatformDram2);
      break;
    case 3:
      DramConfig = &(DramConfigData->PlatformDram3);
      break;
    case 4:
      DramConfig = &(DramConfigData->PlatformDram4);
      break;
    default:
      //
      // Do nothing if the override value does not exist. 0xFF is the
      // default Platform_override value when no override is selected
      //
      break;
    }

  DramPolicy->Package                            = DramConfig->Package;
  DramPolicy->Profile                            = DramConfig->Profile;
  DramPolicy->MemoryDown                         = DramConfig->MemoryDown;
  DramPolicy->DDR3LPageSize                      = DramConfig->DDR3LPageSize;
  DramPolicy->DDR3LASR                           = DramConfig->DDR3LASR;
  DramPolicy->SystemMemorySizeLimit              = DramConfig->MemorySizeLimit;
  DramPolicy->SpdAddress[0]                      = DramConfig->SpdAddress0;
  DramPolicy->SpdAddress[1]                      = DramConfig->SpdAddress1;
  DramPolicy->DDR3LPageSize                      = DramConfig->DDR3LPageSize;
  DramPolicy->DDR3LASR                           = DramConfig->DDR3LASR;
  DramPolicy->HighMemMaxVal                      = DramConfig->HighMemMaxVal;
  DramPolicy->LowMemMaxVal                       = DramConfig->LowMemMaxVal;
  DramPolicy->DisableFastBoot                    = DramConfig->DisableFastBoot;
  DramPolicy->RmtMode                            = DramConfig->RmtMode;
  DramPolicy->RmtCheckRun                        = DramConfig->RmtCheckRun;
  DramPolicy->RmtMarginCheckScaleHighThreshold   = DramConfig->RmtMarginCheckScaleHighThreshold;

  DramPolicy->MsgLevelMask                       = DramConfigData->Message_level_mask;
  DrpPtr                                         = (DRP_DRAM_POLICY *) (&(DramConfig->Ch0RankEnabled));
  ChSwizlePtr                                    = (UINT8(*)[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS]) (&(DramConfig->Ch0_Bit00_swizzling));

  if (!ReadSetupVars) {
    DEBUG ((EFI_D_INFO, "Using smip data for DRAM policy\n"));
    DramPolicy->ChannelHashMask         = DramConfig->ChannelHashMask;
    DramPolicy->SliceHashMask           = DramConfig->SliceHashMask;
    DramPolicy->ChannelsSlicesEnabled   = DramConfig->ChannelsSlicesEnabled;
    DramPolicy->ScramblerSupport        = DramConfig->ScramblerSupport;
    DramPolicy->InterleavedMode         = DramConfig->InterleavedMode;
    DramPolicy->MinRefRate2xEnabled     = DramConfig->MinRefRate2xEnabled;
    DramPolicy->DualRankSupportEnabled  = DramConfig->DualRankSupportEnabled;
  }

  if (DrpPtr != NULL) {
    CopyMem (DramPolicy->ChDrp, DrpPtr, sizeof (DramPolicy->ChDrp));
  }

  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          PLATFORM_SETUP_VARIABLE_NAME,
                          &gEfiSetupVariableGuid,
                          NULL,
                          &VariableSize,
                          &SystemConfiguration
                          );

  if (!EFI_ERROR (Status)) {
    if (SystemConfiguration.Max2G == 0) {
      DramPolicy->SystemMemorySizeLimit = 0x800;
    }
  }

  if (ChSwizlePtr != NULL) CopyMem (DramPolicy->ChSwizzle, ChSwizlePtr, sizeof (DramPolicy->ChSwizzle));

  DramPolicy->MrcTrainingDataPtr = (EFI_PHYSICAL_ADDRESS) *MrcTrainingDataAddr;
  DramPolicy->MrcBootDataPtr     = (EFI_PHYSICAL_ADDRESS) *MrcBootDataAddr;

  //
  // WA for MH board to 6GB. We just apply it if memory size has not been override in smip XML.
  //
  if (DramPolicy->SystemMemorySizeLimit == 0) {
    DramPolicy->SystemMemorySizeLimit = 0x1800;
    if ((DramPolicy->ChDrp[2].RankEnable == 0) && (DramPolicy->ChDrp[3].RankEnable == 0)) {  //half config
      DramPolicy->SystemMemorySizeLimit /= 2;
    }
  }

  //
  // Get Platform Info HOB
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfoHob = GET_GUID_HOB_DATA (Hob.Raw);

  if (PlatformInfoHob->BoardRev == FAB_ID_D) {
    DramPolicy->SystemMemorySizeLimit = 0;
    DEBUG ((EFI_D_INFO, "Detect Micron memory\n"));
  }

  *DramPolicyPpi = DramPolicy;

  return EFI_SUCCESS;
}

