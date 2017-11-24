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

UPDATE_FSPM_UPD_FUNC               mMb3NUpdateFspmUpdPtr            = Mb3NUpdateFspmUpd;
DRAM_CREATE_POLICY_DEFAULTS_FUNC   mMb3NDramCreatePolicyDefaultsPtr = Mb3NDramCreatePolicyDefaults;
UPDATE_PCIE_CONFIG_FUNC            mMb3NUpdatePcieConfigPtr         = Mb3NUpdatePcieConfig;

//
// Minnow Board Next swizzling
//
UINT8 ChSwizzle_MB3N[DRAM_POLICY_NUMBER_CHANNELS][DRAM_POLICY_NUMBER_BITS] = {
  {0x00,0x06,0x04,0x05,0x01,0x03,0x02,0x07,0x08,0x09,0x0B,0x0F,0x0A,0x0D,0x0C,0x0E,0x1C,0x18,0x1A,0x1B,0x1D,0x1E,0x1F,0x19,0x12,0x13,0x14,0x11,0x10,0x16,0x17,0x15}, // Channel 0
  {0x0E,0x0F,0x0A,0x0B,0x08,0x0D,0x0C,0x09,0x07,0x04,0x05,0x00,0x01,0x03,0x02,0x06,0x12,0x15,0x14,0x17,0x10,0x13,0x11,0x16,0x1D,0x1C,0x1F,0x1B,0x1A,0x19,0x18,0x1E}, // Channel 1
  {0x0B,0x0D,0x0C,0x0F,0x09,0x08,0x0A,0x0E,0x05,0x06,0x03,0x07,0x00,0x01,0x02,0x04,0x17,0x11,0x10,0x13,0x14,0x16,0x15,0x12,0x19,0x1B,0x1A,0x18,0x1C,0x1D,0x1E,0x1F}, // Channel 2
  {0x07,0x01,0x02,0x03,0x04,0x05,0x06,0x00,0x0E,0x0D,0x0B,0x0C,0x0A,0x08,0x0F,0x09,0x1E,0x18,0x1C,0x1D,0x1A,0x19,0x1B,0x1F,0x14,0x11,0x12,0x16,0x13,0x15,0x17,0x10}  // Channel 3
};

BOARD_CHANNEL_INFO gMb3nChannelInfo[] = {
   // DeviceWidth               DramDensity               Option                    RankEnable                DescString
   // Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3    Ch 0  Ch 1  Ch 2  Ch 3
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x01, 0x01, 0x00, 0x00}, "LPDDR4  8Gbit 2 channels"}, // #1 - LPDDR4  8Gbit 2 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x01, 0x01, 0x01, 0x01}, "LPDDR4  8Gbit 4 channels"}, // #2 - LPDDR4  8Gbit 4 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x03, 0x03, 0x00, 0x00}, "LPDDR4 16Gbit 2 channels"}, // #3 - LPDDR4 16Gbit 2 channels
    {{0x01, 0x01, 0x01, 0x01}, {0x02, 0x02, 0x02, 0x02}, {0x03, 0x03, 0x03, 0x03}, {0x03, 0x03, 0x03, 0x03}, "LPDDR4 16Gbit 4 channels"}, // #4 - LPDDR4 16Gbit 4 channels
};

EFI_STATUS
EFIAPI
Mb3NUpdateFspmUpd (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN FSPM_UPD                *FspUpdRgn
  )
{
  EFI_PEI_HOB_POINTERS               Hob;
  EFI_PLATFORM_INFO_HOB             *PlatformInfo = NULL;
  DRAM_POLICY_PPI                   *DramPolicy;
  EFI_STATUS                         Status;
  MRC_PARAMS_SAVE_RESTORE           *MrcNvData;
  BOOT_VARIABLE_NV_DATA             *BootVariableNvData;
  MRC_PARAMS_SAVE_RESTORE           *MrcParamsHob;
  BOOT_VARIABLE_NV_DATA             *BootVariableNvDataHob;
  SYSTEM_CONFIGURATION               SystemConfiguration;
  UINTN                              VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;
  UINT32                             VidDid;
  UINT32                             HwconfStraps;
  UINT8                              MemoryType;

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
  } else {
    DEBUG ((DEBUG_INFO, "UpdateFspmUpd - LocatePpi(gDramPolicyPpiGuid) returned %r\n", Status));
  }

  DEBUG ((DEBUG_INFO, "UpdateFspmUpd - gEfiPlatformInfoGuid\n"));
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  //
  // Get IGD VID/DID
  //
  VidDid = MmioRead32 (MmPciBase (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0) + R_SA_IGD_VID);
  if (VidDid == 0x5A848086) {
    //
    // E3950 path
    //
    DEBUG ((DEBUG_INFO, "**** MB3N - E3950 detected!\n"));
  } else if (VidDid == 0x5A858086) {
    //
    // E3930 path
    //
    DEBUG ((DEBUG_INFO, "**** MB3N - E3930 detected!\n"));
  }

  //
  // Overrides for MinnowBoard3Next from Platfrom4 profile
  //
  //   Description    | DualRank | RankEnable | DeviceWidth | DramDenisty | SoC   | Channel
  //  ================|==========|============|=============|=============|=======|=========
  //   MT53B256M32D1  | 0x01     | 0x01       | 0x01  x16   | 0x02   8Gb  | E3930 | Ch0
  //   MT53B512M32D2  | 0x01     | 0x03       | 0x01  x16   | 0x02   8Gb  | E3950 | Ch0&1
  //   MT53B1024M32D4 | 0x01     | 0x03       | 0x00  x8    | 0x04  16Gb  |       |
  //

  //
  // Get HWCONF straps
  //
  HwconfStraps = Minnow3NextGetHwconfStraps ();
  DEBUG ((DEBUG_INFO, "**** MB3N - HWCONF straps = 0x%08X\n", HwconfStraps));

  //
  // Translate into Memory Type
  //
  MemoryType = (UINT8) ((HwconfStraps >> 6) & 0x07);
  if (MemoryType == 0) {
    DEBUG ((DEBUG_INFO, "**** MB3N - SPD based memory init requested, but converted into Memory Profile type #4!\n"));
    MemoryType = 4;
  }
  MemoryType--; // Zero base it for use as index into array

  //
  // Common items
  //
  FspUpdRgn->FspmConfig.Package                 = 0x01;
  FspUpdRgn->FspmConfig.Profile                 = 0x09; // 0x0B; // LPDDR4_2400_24_22_22
  FspUpdRgn->FspmConfig.MemoryDown              = 0x01;
  FspUpdRgn->FspmConfig.DualRankSupportEnable   = 0x01;

  //
  // Memory Type specific items
  //
  if (MemoryType < (sizeof (gMb3nChannelInfo) / sizeof (gMb3nChannelInfo[0]))) {
    DEBUG ((DEBUG_INFO, "**** MB3N - %a detected!\n", gMb3nChannelInfo[MemoryType].DescString));

    // DDR0CH0
    FspUpdRgn->FspmConfig.Ch0_RankEnable        = gMb3nChannelInfo[MemoryType].RankEnable[0];
    FspUpdRgn->FspmConfig.Ch0_DeviceWidth       = gMb3nChannelInfo[MemoryType].DeviceWidth[0];
    FspUpdRgn->FspmConfig.Ch0_DramDensity       = gMb3nChannelInfo[MemoryType].DramDensity[0];
    FspUpdRgn->FspmConfig.Ch0_Option            = gMb3nChannelInfo[MemoryType].Option[0];

    // DDR0CH1
    FspUpdRgn->FspmConfig.Ch1_RankEnable        = gMb3nChannelInfo[MemoryType].RankEnable[1];
    FspUpdRgn->FspmConfig.Ch1_DeviceWidth       = gMb3nChannelInfo[MemoryType].DeviceWidth[1];
    FspUpdRgn->FspmConfig.Ch1_DramDensity       = gMb3nChannelInfo[MemoryType].DramDensity[1];
    FspUpdRgn->FspmConfig.Ch1_Option            = gMb3nChannelInfo[MemoryType].Option[1];

    // DDR1CH0
    FspUpdRgn->FspmConfig.Ch2_RankEnable        = gMb3nChannelInfo[MemoryType].RankEnable[2];
    FspUpdRgn->FspmConfig.Ch2_DeviceWidth       = gMb3nChannelInfo[MemoryType].DeviceWidth[2];
    FspUpdRgn->FspmConfig.Ch2_DramDensity       = gMb3nChannelInfo[MemoryType].DramDensity[2];
    FspUpdRgn->FspmConfig.Ch2_Option            = gMb3nChannelInfo[MemoryType].Option[2];

    // DDR1CH1
    FspUpdRgn->FspmConfig.Ch3_RankEnable        = gMb3nChannelInfo[MemoryType].RankEnable[3];
    FspUpdRgn->FspmConfig.Ch3_DeviceWidth       = gMb3nChannelInfo[MemoryType].DeviceWidth[3];
    FspUpdRgn->FspmConfig.Ch3_DramDensity       = gMb3nChannelInfo[MemoryType].DramDensity[3];
    FspUpdRgn->FspmConfig.Ch3_Option            = gMb3nChannelInfo[MemoryType].Option[3];
  } else {
    DEBUG ((DEBUG_INFO, "**** MB3N - Memory Type 0x%02X is out of range!\n", MemoryType));
  }

  //
  // Swizzling
  //
  if (ChSwizzle_MB3N != NULL) {
    CopyMem (&(FspUpdRgn->FspmConfig.Ch0_Bit_swizzling), ChSwizzle_MB3N[0], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch1_Bit_swizzling), ChSwizzle_MB3N[1], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch2_Bit_swizzling), ChSwizzle_MB3N[2], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
    CopyMem (&(FspUpdRgn->FspmConfig.Ch3_Bit_swizzling), ChSwizzle_MB3N[3], DRAM_POLICY_NUMBER_BITS * sizeof(UINT8));
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
Mb3NDramCreatePolicyDefaults (
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

  DEBUG ((EFI_D_INFO, "*** Minnow Board 3 Next DramCreatePolicyDefaults\n"));
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

/**
  BgUpdatePcieConfig updates the PCIe config block for platform specific items.

  @param[in] PciePreMemConfig         The pointer to the PCIe premem config instance

  @retval     EFI_SUCCESS             The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
Mb3NUpdatePcieConfig (
  IN  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig
  )
{
  //
  // Minnow Board v3 Next
  //
  PciePreMemConfig->RootPort[0].Perst = W_PMU_PLTRST_B; // D20:F0 - PCIe-A
  PciePreMemConfig->RootPort[1].Perst = W_PMU_PLTRST_B; // D20:F1 - PCIe-B
  PciePreMemConfig->RootPort[2].Perst = W_PMU_PLTRST_B; // D19:F0 - PCIe-C
  PciePreMemConfig->RootPort[3].Perst = W_PMU_PLTRST_B; // D19:F1 - PCIe-D
  PciePreMemConfig->RootPort[4].Perst = W_PMU_PLTRST_B; // D19:F2 - LAN
  PciePreMemConfig->RootPort[5].Perst = W_PMU_PLTRST_B; // D19:F3 - Empty
  PciePreMemConfig->RootPort[0].Clock = W_GPIO_210;     // D20:F0 - PCIe-A
  PciePreMemConfig->RootPort[1].Clock = W_GPIO_211;     // D20:F1 - PCIe-B
  PciePreMemConfig->RootPort[2].Clock = W_GPIO_212;     // D19:F0 - PCIe-C
  PciePreMemConfig->RootPort[3].Clock = 0;              // D19:F1 - PCIe-D
  PciePreMemConfig->RootPort[4].Clock = W_GPIO_209;     // D19:F2 - LAN
  PciePreMemConfig->RootPort[5].Clock = 0;              // D19:F3 - Empty

  return EFI_SUCCESS;
}

