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

UPDATE_FSPM_UPD_FUNC mBgUpdateFspmUpdPtr = BgUpdateFspmUpd;
DRAM_CREATE_POLICY_DEFAULTS_FUNC   mBgDramCreatePolicyDefaultsPtr = BgDramCreatePolicyDefaults;

//
// Benson Glacier swizzling
//
UINT8 ChSwizzle_BG[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS] = {
  {9,11,10,12,14,15,8,13,0,3,5,1,2,6,7,4,28,25,27,26,29,30,31,24,17,22,23,18,19,20,21,16}, // Channel 0
  {0,1,5,4,3,6,7,2,12,13,10,14,15,8,9,11,20,23,22,17,21,19,18,16,26,28,29,24,31,25,30,27}, // Channel 1
  {13,9,15,8,11,10,12,14,2,3,7,4,1,6,0,5,31,29,26,28,25,24,30,27,21,23,16,18,20,19,17,22}, // Channel 2
  {3,1,6,7,2,5,4,0,8,9,11,10,12,14,13,15,21,17,18,19,23,22,16,20,29,27,25,30,28,24,31,26}  // Channel 3
};

EFI_STATUS
EFIAPI
BgUpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  )
{
  EFI_PEI_HOB_POINTERS               Hob;
  EFI_PLATFORM_INFO_HOB             *PlatformInfo = NULL;
  DRAM_POLICY_PPI                   *DramPolicy;
  EFI_STATUS                         Status;
  MRC_NV_DATA_FRAME                 *MrcNvData;
  MRC_PARAMS_SAVE_RESTORE           *MrcParamsHob;
  BOOT_VARIABLE_NV_DATA             *BootVariableNvDataHob;
  SYSTEM_CONFIGURATION               SystemConfiguration;
  UINTN                              VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;

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

    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_RankEnable), &DramPolicy->ChDrp, sizeof(DramPolicy->ChDrp));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), &DramPolicy->ChSwizzle, sizeof (DramPolicy->ChSwizzle));

    if (((VOID *)(UINT32)DramPolicy->MrcTrainingDataPtr != 0) &&
        ((VOID *)(UINT32)DramPolicy->MrcBootDataPtr     != 0)) {
      MrcNvData = (MRC_NV_DATA_FRAME *) AllocateZeroPool (sizeof (MRC_NV_DATA_FRAME));
      MrcParamsHob = (MRC_PARAMS_SAVE_RESTORE*)((UINT32)DramPolicy->MrcTrainingDataPtr);
      BootVariableNvDataHob = (BOOT_VARIABLE_NV_DATA*)((UINT32)DramPolicy->MrcBootDataPtr);
      CopyMem(&(MrcNvData->MrcParamsSaveRestore), MrcParamsHob, sizeof (MRC_PARAMS_SAVE_RESTORE));
      CopyMem(&(MrcNvData->BootVariableNvData), BootVariableNvDataHob, sizeof (BOOT_VARIABLE_NV_DATA));
      FspUpdRgn->FspmArchUpd.NvsBufferPtr = (VOID *)(UINT32)MrcNvData;
    }

  }

  DEBUG ((DEBUG_INFO, "UpdateFspmUpd - gEfiPlatformInfoGuid\n"));
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  DEBUG ((DEBUG_INFO, "**** BG - UpdateFspmUpd,BoardId = 0x%02x\n", PlatformInfo->BoardId));
  if (PlatformInfo->BoardId != BOARD_ID_BENSON) {
    //
    // ASSERT false if BoardId isn't Benson
    //
    ASSERT (FALSE);
  }

  //
  // Overrides for Benson Glacier (Micron #MT53B512M32D2NP-062 AIT:C) from Platfrom4 profile
  //
  FspUpdRgn->FspmConfig.Package               = 0x01;
  FspUpdRgn->FspmConfig.Profile               = 0x0B; // LPDDR4_2400_24_22_22
  FspUpdRgn->FspmConfig.MemoryDown            = 0x01;
  FspUpdRgn->FspmConfig.DualRankSupportEnable = 0x01;

  FspUpdRgn->FspmConfig.Ch0_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
  FspUpdRgn->FspmConfig.Ch0_DeviceWidth       = 0x01; // x16
  FspUpdRgn->FspmConfig.Ch0_DramDensity       = 0x02; // 8Gb
  FspUpdRgn->FspmConfig.Ch0_Option            = 0x03;

  FspUpdRgn->FspmConfig.Ch1_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
  FspUpdRgn->FspmConfig.Ch1_DeviceWidth       = 0x01; // x16
  FspUpdRgn->FspmConfig.Ch1_DramDensity       = 0x02; // 8Gb
  FspUpdRgn->FspmConfig.Ch1_Option            = 0x03;

  FspUpdRgn->FspmConfig.Ch2_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
  FspUpdRgn->FspmConfig.Ch2_DeviceWidth       = 0x01; // x16
  FspUpdRgn->FspmConfig.Ch2_DramDensity       = 0x02; // 8Gb
  FspUpdRgn->FspmConfig.Ch2_Option            = 0x03;

  FspUpdRgn->FspmConfig.Ch3_RankEnable        = 0x03; // [0]: Rank 0 [1]: Rank 1
  FspUpdRgn->FspmConfig.Ch3_DeviceWidth       = 0x01; // x16
  FspUpdRgn->FspmConfig.Ch3_DramDensity       = 0x02; // 8Gb
  FspUpdRgn->FspmConfig.Ch3_Option            = 0x03;

  //
  // Swizzling
  //
  if (ChSwizzle_BG != NULL) {
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), ChSwizzle_BG[0], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch1_Bit_swizzling), ChSwizzle_BG[1], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch2_Bit_swizzling), ChSwizzle_BG[2], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch3_Bit_swizzling), ChSwizzle_BG[3], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
  }

  //
  // Disable NPK based on DciEn
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariablePpi);
  if (!EFI_ERROR (Status)) {
    VariableSize = sizeof (SYSTEM_CONFIGURATION);
    Status = VariablePpi->GetVariable (
                            VariablePpi,
                            PLATFORM_SETUP_VARIABLE_NAME,
                            &gEfiSetupVariableGuid,
                            NULL,
                            &VariableSize,
                            &SystemConfiguration
                            );
    if (!EFI_ERROR (Status)) {
      if (SystemConfiguration.DciEn == 0) {
        FspUpdRgn->FspmConfig.NpkEn = 0;
      } else if (SystemConfiguration.DciAutoDetect == 1) {
        FspUpdRgn->FspmConfig.NpkEn = 3;
      } else {
        FspUpdRgn->FspmConfig.NpkEn = 1;
      }
    }
  }

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
BgDramCreatePolicyDefaults (
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

  DEBUG ((EFI_D_INFO, "*** Benson Glacier DramCreatePolicyDefaults\n"));
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

  //
  // DRP
  //
  if (DrpPtr != NULL) {
    CopyMem (DramPolicy->ChDrp, DrpPtr, sizeof (DramPolicy->ChDrp));
  }

  //
  // Swizzling
  //
  if (ChSwizlePtr != NULL) {
    CopyMem (DramPolicy->ChSwizzle, ChSwizlePtr, sizeof (DramPolicy->ChSwizzle));
  }

  if (ReadSetupVars) {
    if (SystemConfiguration.Max2G == 0) {
      DramPolicy->SystemMemorySizeLimit = 0x800;
    }
  }

  DramPolicy->MrcTrainingDataPtr = (EFI_PHYSICAL_ADDRESS) *MrcTrainingDataAddr;
  DramPolicy->MrcBootDataPtr     = (EFI_PHYSICAL_ADDRESS) *MrcBootDataAddr;

  *DramPolicyPpi = DramPolicy;

  return EFI_SUCCESS;
}
