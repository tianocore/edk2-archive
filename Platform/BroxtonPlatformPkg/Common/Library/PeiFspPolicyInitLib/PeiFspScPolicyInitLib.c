/** @file
  Implementation of Fsp SC Policy Initialization.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>
#include <Ppi/ScPolicy.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiScPolicyLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/SteppingLib.h>
#include <Library/MmPciLib.h>
#include <Library/ScPlatformLib.h>

extern EFI_GUID gEfiBootMediaHobGuid;
//
// Generic definitions for device Auto/enabling/disabling used by platform
//
#define DEVICE_AUTO     2
#define DEVICE_ENABLE   1
#define DEVICE_DISABLE  0

/**
  Check it's eMMC boot path or not.

  @retval TRUE                       eMMC Boot path
  @retval FALSE                      Not eMMC boot path

**/
BOOLEAN
IseMMCBoot(
  VOID
  )
{
  VOID                                  *HobList;
  MBP_CURRENT_BOOT_MEDIA                *BootMediaData;

  DEBUG ((EFI_D_INFO, "IseMMCBoot Start!\n"));
  HobList = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  if (HobList != NULL) {
    DEBUG ((EFI_D_INFO, "IseMMCBoot HobList != NULL\n"));
    BootMediaData = GET_GUID_HOB_DATA (HobList);
    if (BootMediaData->PhysicalData == BOOT_FROM_EMMC) {
      DEBUG ((EFI_D_INFO, "BootMediaData->PhysicalData ==  IseMMCBoot\n"));
      return TRUE;
    } else {
      DEBUG ((EFI_D_INFO, "Not boot from eMMC\n"));
      return FALSE;
    }
  }
  return FALSE;
}


/**
  Check it's SPI boot path or not.

  @retval TRUE                       SPI Boot path
  @retval FALSE                      Not SPI boot path

**/
BOOLEAN
IsSpiBoot(
  VOID
  )
{
  VOID                                  *HobList;
  MBP_CURRENT_BOOT_MEDIA                *BootMediaData;

  DEBUG ((EFI_D_INFO, "IsSpiBoot Start!\n"));
  HobList = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  if (HobList != NULL) {
    DEBUG ((EFI_D_INFO, "IsSpiBoot HobList != NULL\n"));
    BootMediaData = GET_GUID_HOB_DATA (HobList);
    if (BootMediaData->PhysicalData == BOOT_FROM_SPI) {
      DEBUG ((EFI_D_INFO, "BootMediaData->PhysicalData ==  IsSpiBoot\n"));
      return TRUE;
    } else {
      DEBUG ((EFI_D_INFO, "Not boot from SPI\n"));
      return FALSE;
    }
  }
  return FALSE;
}


/**
  Performs FSP SC PEI Policy pre mem initialization.

  @param[in, out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspScPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  DEBUG ((DEBUG_INFO, "Wrapper-PeiFspScPolicyInitPreMem - Start\n"));
  DEBUG ((DEBUG_INFO, "Wrapper-PeiFspScPolicyInitPreMem - End\n"));

  return EFI_SUCCESS;
}


/**
  Performs FSP SC PEI Policy initialization.

  @param[in, out]  UpdDataRgnPtr       Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspScPolicyInit (
  IN OUT FSPS_UPD           *FspsUpd
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_HOB_POINTERS             Hob;
  EFI_PLATFORM_INFO_HOB            *PlatformInfo;
  UINTN                            VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SYSTEM_CONFIGURATION             *SystemConfiguration = NULL;
  UINT8                            Index;
  UINT8                            PortIndex;
  UINT32                           SpiHsfsReg;
  UINT32                           SpiFdodReg;
  UINT8                            DevIndex;
  UINT8                            HdaIndex;
  BOOLEAN                          FlashProtectionEnabled;
  SC_POLICY_PPI                    *ScPolicy;
  SC_FLASH_PROTECTION_CONFIG       *FlashProtectionConfig;
  UINTN                            HeciBaseAddress;
  UINT32                           SecMode;

  ScPolicy = NULL;

  DEBUG ((DEBUG_INFO, "PeiFspScPolicyInit - Start\n"));

  //
  // Locate ScPolicyPpi
  //
  Status = PeiServicesLocatePpi (
             &gScPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &ScPolicy
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "PeiFspScPolicyInit - Start\n"));

  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  ASSERT (SystemConfiguration != NULL);

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "Pass setup option to FspsUpd...\n"));

    SpiHsfsReg = MmioRead32 (SPI_BASE_ADDRESS + R_SPI_HSFS);
    if ((SpiHsfsReg & B_SPI_HSFS_FDV) == B_SPI_HSFS_FDV) {
      MmioWrite32 (SPI_BASE_ADDRESS + R_SPI_FDOC, V_SPI_FDOC_FDSS_FSDM);
      SpiFdodReg = MmioRead32 (SPI_BASE_ADDRESS + R_SPI_FDOD);
      if (SpiFdodReg == V_SPI_FDBAR_FLVALSIG) {
      }
    }

    //
    // Set ACPI and P2SB Base Addresses
    //
    FspsUpd->FspsConfig.ResetSelect          = (UINT8) PcdGet8 (PcdResetType);
    FspsUpd->FspsConfig.CRIDSettings         = SystemConfiguration->CRIDSettings;

    //
    // Set HPET configurations
    //
    FspsUpd->FspsConfig.Hpet                 = SystemConfiguration->Hpet;
    FspsUpd->FspsConfig.HpetBdfValid         = 0x01;
    FspsUpd->FspsConfig.HpetBusNumber        = 0xFA;
    FspsUpd->FspsConfig.HpetDeviceNumber     = 0x0F;
    FspsUpd->FspsConfig.HpetFunctionNumber   = 0;

    //
    // Set IOAPIC configurations
    //
    FspsUpd->FspsConfig.IoApicId             = 0x01;
    FspsUpd->FspsConfig.IoApicBdfValid       = 1;
    FspsUpd->FspsConfig.IoApicBusNumber      = 0xFA;
    FspsUpd->FspsConfig.IoApicDeviceNumber   = 0x1F;
    FspsUpd->FspsConfig.IoApicFunctionNumber = 0;

    //
    // Set PCI express configuration according to setup value
    //
    FspsUpd->FspsConfig.PcieClockGatingDisabled = SystemConfiguration->PcieClockGatingDisabled;
    FspsUpd->FspsConfig.PcieRootPort8xhDecode   = SystemConfiguration->PcieRootPort8xhDecode;
    FspsUpd->FspsConfig.Pcie8xhDecodePortIndex  = SystemConfiguration->Pcie8xhDecodePortIndex;
    FspsUpd->FspsConfig.PcieRootPortPeerMemoryWriteEnable         = SystemConfiguration->PcieRootPortPeerMemoryWriteEnable;

    for (PortIndex = 0; PortIndex < SC_MAX_PCIE_ROOT_PORTS; PortIndex++) {
      FspsUpd->FspsConfig.PcieRootPortEn[PortIndex]                           = SystemConfiguration->PcieRootPortEn[PortIndex];
      FspsUpd->FspsConfig.PcieRpSlotImplemented[PortIndex]                    = TRUE;
      FspsUpd->FspsConfig.PhysicalSlotNumber[PortIndex]                       = (UINT8) PortIndex;
      FspsUpd->FspsConfig.PcieRpAspm[PortIndex]                               = SystemConfiguration->PcieRootPortAspm[PortIndex];
      FspsUpd->FspsConfig.PcieRpL1Substates[PortIndex]                        = SystemConfiguration->PcieRootPortL1SubStates[PortIndex];
      FspsUpd->FspsConfig.PcieRpAcsEnabled[PortIndex]                         = SystemConfiguration->PcieRootPortACS[PortIndex];
      FspsUpd->FspsConfig.PcieRpPmSci[PortIndex]                              = SystemConfiguration->PcieRootPortPMCE[PortIndex];
      FspsUpd->FspsConfig.PcieRpHotPlug[PortIndex]                            = SystemConfiguration->PcieRootPortHPE[PortIndex];
      FspsUpd->FspsConfig.AdvancedErrorReporting[PortIndex]                   = FALSE;
      FspsUpd->FspsConfig.UnsupportedRequestReport[PortIndex]                 = SystemConfiguration->PcieRootPortURE[PortIndex];
      FspsUpd->FspsConfig.FatalErrorReport[PortIndex]                         = SystemConfiguration->PcieRootPortFEE[PortIndex];
      FspsUpd->FspsConfig.NoFatalErrorReport[PortIndex]                       = SystemConfiguration->PcieRootPortNFE[PortIndex];
      FspsUpd->FspsConfig.CorrectableErrorReport[PortIndex]                   = SystemConfiguration->PcieRootPortCEE[PortIndex];
      FspsUpd->FspsConfig.PmeInterrupt[PortIndex]                             = 0;
      FspsUpd->FspsConfig.SystemErrorOnFatalError[PortIndex]                  = SystemConfiguration->PcieRootPortSFE[PortIndex];
      FspsUpd->FspsConfig.SystemErrorOnNonFatalError[PortIndex]               = SystemConfiguration->PcieRootPortSNE[PortIndex];
      FspsUpd->FspsConfig.SystemErrorOnCorrectableError[PortIndex]            = SystemConfiguration->PcieRootPortSCE[PortIndex];
      FspsUpd->FspsConfig.PcieRpTransmitterHalfSwing[PortIndex]               = SystemConfiguration->PcieRootPortTHS[PortIndex];
      FspsUpd->FspsConfig.PcieRpCompletionTimeout[PortIndex]                  = ScPcieCompletionTO_Default;
      FspsUpd->FspsConfig.PcieRpSpeed[PortIndex]                              = SystemConfiguration->PcieRootPortSpeed[PortIndex];
      FspsUpd->FspsConfig.PcieRpSelectableDeemphasis[PortIndex]               = SystemConfiguration->PcieRootPortSelectableDeemphasis[PortIndex];

      //
      // LTR settings
      //
      FspsUpd->FspsConfig.PcieRpLtrEnable[PortIndex]                          = SystemConfiguration->PchPcieLtrEnable[PortIndex];
      FspsUpd->FspsConfig.PcieRpLtrConfigLock[PortIndex]                      = SystemConfiguration->PchPcieLtrConfigLock[PortIndex];
      FspsUpd->FspsConfig.PcieRpLtrMaxSnoopLatency[PortIndex]                 = SystemConfiguration->PcieLtrMaxSnoopLatency[PortIndex];
      FspsUpd->FspsConfig.PcieRpLtrMaxNonSnoopLatency[PortIndex]              = SystemConfiguration->PcieLtrMaxNoSnoopLatency[PortIndex];
      FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideMode[PortIndex]           = SystemConfiguration->PchPcieSnoopLatencyOverrideMode[PortIndex];
      FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideMultiplier[PortIndex]     = SystemConfiguration->PchPcieSnoopLatencyOverrideMultiplier[PortIndex];
      FspsUpd->FspsConfig.PcieRpSnoopLatencyOverrideValue[PortIndex]          = SystemConfiguration->PchPcieSnoopLatencyOverrideValue[PortIndex];
      FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideMode[PortIndex]        = SystemConfiguration->PchPcieNonSnoopLatencyOverrideMode[PortIndex];
      FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideMultiplier[PortIndex]  = SystemConfiguration->PchPcieNonSnoopLatencyOverrideMultiplier[PortIndex];
      FspsUpd->FspsConfig.PcieRpNonSnoopLatencyOverrideValue[PortIndex]       = SystemConfiguration->PchPcieNonSnoopLatencyOverrideValue[PortIndex];
      FspsUpd->FspsConfig.PtmEnable[PortIndex]                                = TRUE;
    }
#if (ENBDT_PF_ENABLE == 1)
    FspsUpd->FspsConfig.PcieRpClkReqSupported[0] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [0] = 2;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[1] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [1] = 3;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[2] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [2] = 0;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[3] = FALSE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [3] = 0xF;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[4] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [4] = 0x1;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[5] = FALSE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [5] = 0xF;
#endif
    FspsUpd->FspsConfig.PcieRpClkReqSupported[0] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [0] = 2;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[1] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [1] = 3;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[2] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [2] = 1;
    FspsUpd->FspsConfig.PcieRpClkReqSupported[4] = TRUE;
    FspsUpd->FspsConfig.PcieRpClkReqNumber   [4] = 0;

    //
    // Set SATA configuration according to setup value
    //
    FspsUpd->FspsConfig.EnableSata               = SystemConfiguration->Sata;
    FspsUpd->FspsConfig.SataMode                 = SystemConfiguration->SataInterfaceMode;
    FspsUpd->FspsConfig.SataSalpSupport          = SystemConfiguration->SataSalp;
    for (PortIndex = 0; PortIndex < SC_MAX_SATA_PORTS; PortIndex++) {
      FspsUpd->FspsConfig.SataPortsEnable[PortIndex] = SystemConfiguration->SataPort[PortIndex];
      FspsUpd->FspsConfig.SataPortsHotPlug[PortIndex]          = SystemConfiguration->SataHotPlug[PortIndex];
      FspsUpd->FspsConfig.SataPortsSpinUp[PortIndex]           = SystemConfiguration->SataSpinUp[PortIndex];
      FspsUpd->FspsConfig.SataPortsExternal[PortIndex]         = FALSE;
      FspsUpd->FspsConfig.SataPortsDevSlp[PortIndex]           = SystemConfiguration->PxDevSlp[PortIndex];
      FspsUpd->FspsConfig.SataPortsEnableDitoConfig[PortIndex] = SystemConfiguration->EnableDitoConfig[PortIndex];
      FspsUpd->FspsConfig.SataPortsDmVal[PortIndex]            = SystemConfiguration->DmVal[PortIndex];
      FspsUpd->FspsConfig.SataPortsDitoVal[PortIndex]          = SystemConfiguration->DitoVal[PortIndex];
      FspsUpd->FspsConfig.SataPortsSolidStateDrive[PortIndex]  = SystemConfiguration->SataType[PortIndex];
    }
    FspsUpd->FspsConfig.SataTestMode                           = SystemConfiguration->SataTestMode;

    //
    // Flash Security Recommendation,
    // Intel strongly recommends that BIOS sets the BIOS Interface Lock Down bit. Enabling this bit
    // will mitigate malicious software attempts to replace the system BIOS option ROM with its own code.
    // BIOS lock down bit is enabled by default as per Platform policy, except that when system is in recovery mode or FDO is enabled. In this case this will be disabled as part of Firmware Update / Recovery update
    //

    HeciBaseAddress = MmPciBase (
                        SEC_BUS,
                        SEC_DEVICE_NUMBER,
                        HECI_FUNCTION_NUMBER
                        );
    SecMode = MmioRead32 (HeciBaseAddress + R_SEC_FW_STS0);
    DEBUG ((DEBUG_INFO, " BIOS lock -CSE Status :%x in Register Value :0x%08X and 0x%08X \n", (SecMode & 0xF0000), SecMode, HeciBaseAddress));

    if ((GetBxtSeries() == BxtP)&& ((SEC_MODE_NORMAL == (SecMode & 0xF0000)))) {
      FspsUpd->FspsConfig.BiosInterface = TRUE;
      FspsUpd->FspsConfig.BiosLock      = SystemConfiguration->ScBiosLock;
      DEBUG ((DEBUG_INFO, "ScBiosLock = %d\n", SystemConfiguration->ScBiosLock));
      if (SystemConfiguration->ScBiosLock) {
        FspsUpd->FspsConfig.SpiEiss   = TRUE;
      } else {
        FspsUpd->FspsConfig.SpiEiss   = FALSE;
      }
    } else {
      FspsUpd->FspsConfig.BiosInterface = FALSE;
      FspsUpd->FspsConfig.BiosLock      = FALSE;
      FspsUpd->FspsConfig.SpiEiss       = FALSE;
    }
    FspsUpd->FspsConfig.RtcLock         = SystemConfiguration->RtcLock;
    //
    // Set HDA configuration according to setup value
    //
    FspsUpd->FspsConfig.HdaEnable                     = SystemConfiguration->ScHdAudio;
    FspsUpd->FspsConfig.DspEnable                     = SystemConfiguration->ScHdAudioDsp;
    FspsUpd->FspsConfig.Mmt                           = SystemConfiguration->ScHdAudioMmt;
    FspsUpd->FspsConfig.Hmt                           = SystemConfiguration->ScHdAudioHmt;
    FspsUpd->FspsConfig.HdAudioIoBufferOwnership      = SystemConfiguration->ScHdAudioIoBufferOwnership;
    FspsUpd->FspsConfig.HdaVerbTableEntryNum          = PcdGet8(HdaVerbTableEntryNum);
    FspsUpd->FspsConfig.HdaVerbTablePtr               = (UINT32)PcdGet64(PcdHdaVerbTablePtr);

    FspsUpd->FspsConfig.HdAudioDspUaaCompliance       = 1;

    FspsUpd->FspsConfig.BiosCfgLockDown               = SystemConfiguration->ScHdAudioBiosCfgLockDown;
    FspsUpd->FspsConfig.HDAudioPwrGate                = SystemConfiguration->ScHdAudioPwrGate;
    FspsUpd->FspsConfig.HDAudioClkGate                = SystemConfiguration->ScHdAudioClkGate;
    FspsUpd->FspsConfig.Pme                           = SystemConfiguration->ScHdAudioPme;
    FspsUpd->FspsConfig.DspEndpointDmic               = SystemConfiguration->ScHdAudioNhltEndpointDmic;
    FspsUpd->FspsConfig.DspEndpointBluetooth          = SystemConfiguration->ScHdAudioNhltEndpointBt;

    if (FspsUpd->FspsConfig.HdAudioIoBufferOwnership == ScHdaIoBufOwnerHdaLink) {
     FspsUpd->FspsConfig.DspEndpointI2sSkp           = FALSE;
     FspsUpd->FspsConfig.DspEndpointI2sHp            = FALSE;
    } else {
     FspsUpd->FspsConfig.DspEndpointI2sSkp           = SystemConfiguration->ScHdAudioNhltEndpointI2sSKP;
     FspsUpd->FspsConfig.DspEndpointI2sHp            = SystemConfiguration->ScHdAudioNhltEndpointI2sHP;
    }
    FspsUpd->FspsConfig.HdAudioVcType                 = SystemConfiguration->SvHdaVcType;
    FspsUpd->FspsConfig.HdAudioLinkFrequency          = SystemConfiguration->HdAudioLinkFrequency;
    FspsUpd->FspsConfig.HdAudioIDispLinkFrequency     = SystemConfiguration->IDispLinkFrequency;
    FspsUpd->FspsConfig.HdAudioIDispLinkTmode         = SystemConfiguration->IDispLinkTmode;
    for (HdaIndex = 0; HdaIndex < HDAUDIO_FEATURES; HdaIndex++) {
     FspsUpd->FspsConfig.DspFeatureMask |= (UINT32) (SystemConfiguration->ScHdAudioFeature[HdaIndex] ? (1 << HdaIndex) : 0);
    }
    for(HdaIndex = 0; HdaIndex < HDAUDIO_PP_MODULES; HdaIndex++) {
     FspsUpd->FspsConfig.DspPpModuleMask |= (UINT32) (SystemConfiguration->ScHdAudioPostProcessingMod[HdaIndex] ? (1 << HdaIndex) : 0);
    }
    FspsUpd->FspsConfig.ResetWaitTimer = 300;


  //
  // Configure Interrupt Polarity
  //
  FspsUpd->FspsConfig.IPC[0] = (UINT32)
    ((V_ITSS_SB_IPC_ACTIVE_LOW << 31) + // IRQ 31
    (V_ITSS_SB_IPC_ACTIVE_LOW << 30) +  // IRQ 30
    (V_ITSS_SB_IPC_ACTIVE_LOW << 29) +  // IRQ 29
    (V_ITSS_SB_IPC_ACTIVE_LOW << 28) +  // IRQ 28
    (V_ITSS_SB_IPC_ACTIVE_LOW << 27) +  // IRQ 27
    (V_ITSS_SB_IPC_ACTIVE_LOW << 26) +  // IRQ 26
    (V_ITSS_SB_IPC_ACTIVE_LOW << 25) +  // IRQ 25
    (V_ITSS_SB_IPC_ACTIVE_LOW << 24) +  // IRQ 24
    (V_ITSS_SB_IPC_ACTIVE_LOW << 23) +  // IRQ 23
    (V_ITSS_SB_IPC_ACTIVE_LOW << 22) +  // IRQ 22
    (V_ITSS_SB_IPC_ACTIVE_LOW << 21) +  // IRQ 21
    (V_ITSS_SB_IPC_ACTIVE_LOW << 20) +  // IRQ 20
    (V_ITSS_SB_IPC_ACTIVE_LOW << 19) +  // IRQ 19
    (V_ITSS_SB_IPC_ACTIVE_LOW << 18) +  // IRQ 18
    (V_ITSS_SB_IPC_ACTIVE_LOW << 17) +  // IRQ 17
    (V_ITSS_SB_IPC_ACTIVE_LOW << 16) +  // IRQ 16
    (V_ITSS_SB_IPC_ACTIVE_LOW << 15) +  // IRQ 15
    (V_ITSS_SB_IPC_ACTIVE_LOW << 14) +  // IRQ 14
    (V_ITSS_SB_IPC_ACTIVE_LOW << 13) +  // IRQ 13
    (V_ITSS_SB_IPC_ACTIVE_HIGH << 12) + // IRQ 12
    (V_ITSS_SB_IPC_ACTIVE_LOW << 11) +  // IRQ 11
    (V_ITSS_SB_IPC_ACTIVE_LOW << 10) +  // IRQ 10
    (V_ITSS_SB_IPC_ACTIVE_LOW << 9) +   // IRQ  9
    (V_ITSS_SB_IPC_ACTIVE_HIGH << 8) +  // IRQ  8
    (V_ITSS_SB_IPC_ACTIVE_LOW << 7) +   // IRQ  7
    (V_ITSS_SB_IPC_ACTIVE_LOW << 6) +   // IRQ  6
    (V_ITSS_SB_IPC_ACTIVE_LOW << 5) +   // IRQ  5
    (V_ITSS_SB_IPC_ACTIVE_LOW << 4) +   // IRQ  4
    (V_ITSS_SB_IPC_ACTIVE_LOW << 3) +   // IRQ  3
    (V_ITSS_SB_IPC_ACTIVE_HIGH << 2) +  // IRQ  2
    (V_ITSS_SB_IPC_ACTIVE_HIGH << 1) +  // IRQ  1
    (V_ITSS_SB_IPC_ACTIVE_HIGH << 0))   // IRQ  0
    ;

  FspsUpd->FspsConfig.IPC[1] = (UINT32)
    ((V_ITSS_SB_IPC_ACTIVE_LOW << 31) + // IRQ 63
    (V_ITSS_SB_IPC_ACTIVE_LOW << 30) +  // IRQ 62
    (V_ITSS_SB_IPC_ACTIVE_LOW << 29) +  // IRQ 61
    (V_ITSS_SB_IPC_ACTIVE_LOW << 28) +  // IRQ 60
    (V_ITSS_SB_IPC_ACTIVE_LOW << 27) +  // IRQ 59
    (V_ITSS_SB_IPC_ACTIVE_LOW << 26) +  // IRQ 58
    (V_ITSS_SB_IPC_ACTIVE_LOW << 25) +  // IRQ 57
    (V_ITSS_SB_IPC_ACTIVE_LOW << 24) +  // IRQ 56
    (V_ITSS_SB_IPC_ACTIVE_LOW << 23) +  // IRQ 55
    (V_ITSS_SB_IPC_ACTIVE_LOW << 22) +  // IRQ 54
    (V_ITSS_SB_IPC_ACTIVE_LOW << 21) +  // IRQ 53
    (V_ITSS_SB_IPC_ACTIVE_LOW << 20) +  // IRQ 52
    (V_ITSS_SB_IPC_ACTIVE_LOW << 19) +  // IRQ 51
    (V_ITSS_SB_IPC_ACTIVE_LOW << 18) +  // IRQ 50
    (V_ITSS_SB_IPC_ACTIVE_LOW << 17) +  // IRQ 49
    (V_ITSS_SB_IPC_ACTIVE_LOW << 16) +  // IRQ 48
    (V_ITSS_SB_IPC_ACTIVE_LOW << 15) +  // IRQ 47
    (V_ITSS_SB_IPC_ACTIVE_LOW << 14) +  // IRQ 46
    (V_ITSS_SB_IPC_ACTIVE_LOW << 13) +  // IRQ 45
    (V_ITSS_SB_IPC_ACTIVE_LOW << 12) +  // IRQ 44
    (V_ITSS_SB_IPC_ACTIVE_LOW << 11) +  // IRQ 43
    (V_ITSS_SB_IPC_ACTIVE_LOW << 10) +  // IRQ 42
    (V_ITSS_SB_IPC_ACTIVE_LOW << 9) +   // IRQ 41
    (V_ITSS_SB_IPC_ACTIVE_LOW << 8) +   // IRQ 40
    (V_ITSS_SB_IPC_ACTIVE_LOW << 7) +   // IRQ 39
    (V_ITSS_SB_IPC_ACTIVE_LOW << 6) +   // IRQ 38
    (V_ITSS_SB_IPC_ACTIVE_LOW << 5) +   // IRQ 37
    (V_ITSS_SB_IPC_ACTIVE_LOW << 4) +   // IRQ 36
    (V_ITSS_SB_IPC_ACTIVE_LOW << 3) +   // IRQ 35
    (V_ITSS_SB_IPC_ACTIVE_LOW << 2) +   // IRQ 34
    (V_ITSS_SB_IPC_ACTIVE_LOW << 1) +   // IRQ 33
    (V_ITSS_SB_IPC_ACTIVE_LOW << 0))    // IRQ 32
    ;
    FspsUpd->FspsConfig.IPC[2] = 0xFFFFFFFF;
    FspsUpd->FspsConfig.IPC[3] = 0xFFFFFFFF;
  
    //
    // Set GMM configuration according to setup value
    //
    FspsUpd->FspsConfig.Gmm                      = SystemConfiguration->Gmm;
    FspsUpd->FspsConfig.ClkGatingPgcbClkTrunk    = SystemConfiguration->GmmCgPGCBEnabled;
    FspsUpd->FspsConfig.ClkGatingSb              = SystemConfiguration->GmmCgSBDEnabled;
    FspsUpd->FspsConfig.ClkGatingSbClkTrunk      = SystemConfiguration->GmmCgSBTEnabled;
    FspsUpd->FspsConfig.ClkGatingSbClkPartition  = SystemConfiguration->GmmCgSBPEnabled;
    FspsUpd->FspsConfig.ClkGatingCore            = SystemConfiguration->GmmCgCoreEnabled;
    FspsUpd->FspsConfig.ClkGatingDma             = SystemConfiguration->GmmCgDmaEnabled;
    FspsUpd->FspsConfig.ClkGatingRegAccess       = SystemConfiguration->GmmCgRAEnabled;
    FspsUpd->FspsConfig.ClkGatingHost            = SystemConfiguration->GmmCgHostEnabled;
    FspsUpd->FspsConfig.ClkGatingPartition       = SystemConfiguration->GmmCgPEnabled;
    FspsUpd->FspsConfig.ClkGatingTrunk           = SystemConfiguration->GmmCgTEnabled;

    //
    // Set ISH configuration according to setup value.
    //
    FspsUpd->FspsConfig.IshEnable   = SystemConfiguration->ScIshEnabled;

    //
    // Set USB Device 21 configuration according to setup value
    //
    FspsUpd->FspsConfig.DisableComplianceMode      = SystemConfiguration->DisableComplianceMode;

    //
    // Set xHCI (USB 3.0) configuration according to setup value
    //
    FspsUpd->FspsConfig.Usb30Mode                  = SystemConfiguration->ScUsb30Mode;
    FspsUpd->FspsConfig.SsicPortEnable[0]          = SystemConfiguration->Ssic1Support;
    FspsUpd->FspsConfig.SsicPortEnable[1]          = SystemConfiguration->Ssic2Support;
    FspsUpd->FspsConfig.SsicRate[0]                = SystemConfiguration->Ssic1Rate;
    FspsUpd->FspsConfig.SsicRate[1]                = SystemConfiguration->Ssic2Rate;
    FspsUpd->FspsConfig.DlanePwrGating             = SystemConfiguration->SsicDlanePg;
    FspsUpd->FspsConfig.UsbOtg                     = SystemConfiguration->ScUsbOtg;

    //
    // Overcurrent applies to walk-up xHCI ports only - not SSIC or HSIC
    //
    //  OC0: Used for the OTG port (port 0)
    //  OC1: Used for the 2 host walk-up ports
    //
#if (ENBDT_PF_ENABLE == 1)
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[0] = ScUsbOverCurrentPin0;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[1] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[2] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[3] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[4] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[5] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[6] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[7] = ScUsbOverCurrentPinSkip;

    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[0] = ScUsbOverCurrentPin0;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[1] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[2] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[3] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[4] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[5] = ScUsbOverCurrentPin1;

    for (DevIndex = 0; DevIndex < GetScXhciMaxUsb2PortNum (); DevIndex++) {
      FspsUpd->FspsConfig.PortUsb20Enable[DevIndex]= SystemConfiguration->PortUsb20[DevIndex];
    }
    for (DevIndex = 0; DevIndex < GetScXhciMaxUsb3PortNum (); DevIndex++) {
      FspsUpd->FspsConfig.PortUsb30Enable[DevIndex]= SystemConfiguration->PortUsb30[DevIndex];
    }
#else
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[0] = ScUsbOverCurrentPin0;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[1] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[2] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[3] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[4] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[5] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[6] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs20bOverCurrentPin[7] = ScUsbOverCurrentPinSkip;

    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[0] = ScUsbOverCurrentPin0;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[1] = ScUsbOverCurrentPin1;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[2] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[3] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[4] = ScUsbOverCurrentPinSkip;
    FspsUpd->FspsConfig.PortUs30bOverCurrentPin[5] = ScUsbOverCurrentPinSkip;

    FspsUpd->FspsConfig.PortUsb20Enable[0] = TRUE;
    FspsUpd->FspsConfig.PortUsb20Enable[1] = TRUE;
    FspsUpd->FspsConfig.PortUsb20Enable[2] = TRUE;
    FspsUpd->FspsConfig.PortUsb20Enable[3] = FALSE;
    FspsUpd->FspsConfig.PortUsb20Enable[4] = FALSE;
    FspsUpd->FspsConfig.PortUsb20Enable[5] = FALSE;
    FspsUpd->FspsConfig.PortUsb20Enable[6] = FALSE;
    FspsUpd->FspsConfig.PortUsb20Enable[7] = FALSE;

    FspsUpd->FspsConfig.PortUsb30Enable[0] = TRUE;
    FspsUpd->FspsConfig.PortUsb30Enable[1] = TRUE;
    FspsUpd->FspsConfig.PortUsb30Enable[2] = FALSE;
    FspsUpd->FspsConfig.PortUsb30Enable[3] = FALSE;
    FspsUpd->FspsConfig.PortUsb30Enable[4] = FALSE;
    FspsUpd->FspsConfig.PortUsb30Enable[5] = FALSE;
#endif

    //
    // Set LPSS configuration according to setup value.
    //
    FspsUpd->FspsConfig.I2c0Enable    = SystemConfiguration->LpssI2C0Enabled;
    FspsUpd->FspsConfig.I2c1Enable    = SystemConfiguration->LpssI2C1Enabled;
    FspsUpd->FspsConfig.I2c2Enable    = SystemConfiguration->LpssI2C2Enabled;
    FspsUpd->FspsConfig.I2c3Enable    = SystemConfiguration->LpssI2C3Enabled;
    FspsUpd->FspsConfig.I2c4Enable    = SystemConfiguration->LpssI2C4Enabled;
    FspsUpd->FspsConfig.I2c5Enable    = SystemConfiguration->LpssI2C5Enabled;
    FspsUpd->FspsConfig.I2c6Enable    = SystemConfiguration->LpssI2C6Enabled;
    FspsUpd->FspsConfig.I2c7Enable    = SystemConfiguration->LpssI2C7Enabled;
    FspsUpd->FspsConfig.Hsuart0Enable = SystemConfiguration->LpssHsuart0Enabled;
    FspsUpd->FspsConfig.Hsuart1Enable = SystemConfiguration->LpssHsuart1Enabled;
    FspsUpd->FspsConfig.Hsuart2Enable = SystemConfiguration->LpssHsuart2Enabled;
    FspsUpd->FspsConfig.Hsuart3Enable = SystemConfiguration->LpssHsuart3Enabled;
    FspsUpd->FspsConfig.Spi0Enable    = SystemConfiguration->LpssSpi0Enabled;
    FspsUpd->FspsConfig.Spi1Enable    = SystemConfiguration->LpssSpi1Enabled;
    FspsUpd->FspsConfig.Spi2Enable    = SystemConfiguration->LpssSpi2Enabled;
    FspsUpd->FspsConfig.Uart2KernelDebugBaseAddress = SystemConfiguration->Uart2KernelDebugBaseAddress;
    for (DevIndex = 0; DevIndex < LPSS_I2C_DEVICE_NUM; DevIndex++) {
      FspsUpd->FspsConfig.I2cClkGateCfg[DevIndex] = SystemConfiguration->LpssI2cClkGateCfg[DevIndex];
    }
    for (DevIndex = 0; DevIndex < LPSS_HSUART_DEVICE_NUM; DevIndex++) {
      FspsUpd->FspsConfig.HsuartClkGateCfg[DevIndex] = SystemConfiguration->LpssHsuartClkGateCfg[DevIndex];
    }
    for (DevIndex = 0; DevIndex < LPSS_SPI_DEVICE_NUM; DevIndex++) {
      FspsUpd->FspsConfig.SpiClkGateCfg[DevIndex] = SystemConfiguration->LpssSpiClkGateCfg[DevIndex];
    }
    //
    // Kernel Debugger (e.g. WinDBG) Enable
    //
    FspsUpd->FspsConfig.OsDbgEnable      = (BOOLEAN) SystemConfiguration->OsDbgEnable;
    FspsUpd->FspsConfig.LPSS_S0ixEnable  = (BOOLEAN) SystemConfiguration->LowPowerS0Idle;

    FspsUpd->FspsConfig.DciEn            = SystemConfiguration->DciEn;
    FspsUpd->FspsConfig.DciAutoDetect    = SystemConfiguration->DciAutoDetect;

    //
    // Set SCS configuration according to setup value.
    //
    FspsUpd->FspsConfig.SdcardEnabled    = SystemConfiguration->SccSdcardEnabled;
    FspsUpd->FspsConfig.UfsEnabled       = 0;
    FspsUpd->FspsConfig.eMMCEnabled      = SystemConfiguration->ScceMMCEnabled;
    FspsUpd->FspsConfig.SdioEnabled      = SystemConfiguration->SccSdioEnabled;
    FspsUpd->FspsConfig.eMMCHostMaxSpeed = (UINT8) PcdGet8 (PcdeMMCHostMaxSpeed);

    FspsUpd->FspsConfig.GppLock          = SystemConfiguration->GPPLock;
    FspsUpd->FspsConfig.SdioTxCmdCntl          = 0x505;
    FspsUpd->FspsConfig.SdioTxDataCntl1        = 0xE;
    FspsUpd->FspsConfig.SdioTxDataCntl2        = 0x22272828;
    FspsUpd->FspsConfig.SdioRxCmdDataCntl1     = 0x16161616;
    FspsUpd->FspsConfig.SdioRxCmdDataCntl2     = 0x10000;
    FspsUpd->FspsConfig.SdcardTxCmdCntl        = 0x505;
    FspsUpd->FspsConfig.SdcardTxDataCntl1      = 0xA13;
    FspsUpd->FspsConfig.SdcardTxDataCntl2      = 0x24242828;
    FspsUpd->FspsConfig.SdcardRxCmdDataCntl1   = 0x73A3637;
    FspsUpd->FspsConfig.SdcardRxStrobeCntl     = 0x0;
    FspsUpd->FspsConfig.SdcardRxCmdDataCntl2   = 0x10000;
    FspsUpd->FspsConfig.EmmcTxCmdCntl          = 0x505;
    FspsUpd->FspsConfig.EmmcTxDataCntl1        = 0xC11;
    FspsUpd->FspsConfig.EmmcTxDataCntl2        = 0x1C2A2927;
    FspsUpd->FspsConfig.EmmcRxCmdDataCntl1     = 0x000D162F;
    FspsUpd->FspsConfig.EmmcRxStrobeCntl       = 0x0a0a;
    FspsUpd->FspsConfig.EmmcRxCmdDataCntl2     = 0x1003b;
    FspsUpd->FspsConfig.EmmcMasterSwCntl       = 0x001;

    if (SystemConfiguration->ScceMMCEnabled == DEVICE_AUTO) {
      if (IseMMCBoot ()) {
        FspsUpd->FspsConfig.eMMCEnabled = DEVICE_ENABLE;
      } else {
        FspsUpd->FspsConfig.eMMCEnabled = DEVICE_DISABLE;
      }
    }

    //
    //Update P2sbUnhide Policy
    //
    FspsUpd->FspsConfig.P2sbUnhide = 1;
    FspsUpd->FspsConfig.VtdEnable = SystemConfiguration->VTdEnable;

    //
    // Power management Configuration
    //
    if (SystemConfiguration->Cg8254 == 0) {
      FspsUpd->FspsConfig.Timer8254ClkSetting = FALSE;
    }
    FspsUpd->FspsConfig.PowerButterDebounceMode = SystemConfiguration->PowerButterDebounceMode;
    //
    // Set Flash Protection configuration according to setup data
    //
    if ((GetBxtSeries() == BxtP) && (IsSpiBoot ())) {
      //
      // Configure Flash Protection Range Registers
      //
      FlashProtectionEnabled = SystemConfiguration->FprrEnable == TRUE ? TRUE : FALSE;

      //
      // Enabling Flash Protection Range Registers
      // Enable FPRR policy and set up ranges on non-Capsule Update flow with Flash Wear-Out Protection enabled
      // PrintFlashProtectionConfig() dumps FPRR information during ScPrintPolicyPpi()
      // Enable the FPRR Bit  is enabled by default as per Platform policy, except that when system is in recovery mode or FDO is enabled. In this case this will be disabled as part of Firmware Update / Recovery update
      //
      if (FlashProtectionEnabled && (SEC_MODE_NORMAL == (SecMode & 0xF0000))) {
        //
        // Flash Protection Range Register initialization
        //
        for (Index = 0; Index < SC_FLASH_PROTECTED_RANGES; Index++) {
          FspsUpd->FspsConfig.WriteProtectionEnable[Index]  = SystemConfiguration->ScBiosLock == TRUE ? TRUE : FALSE;
          FspsUpd->FspsConfig.ReadProtectionEnable[Index]   = FALSE;
        }

        //
        // Assign FPRR ranges
        //
        Status = GetConfigBlock ((VOID *) ScPolicy, &gFlashProtectionConfigGuid, (VOID *) &FlashProtectionConfig);
        ASSERT_EFI_ERROR (Status);
        if (!EFI_ERROR (Status)) {
          FspsUpd->FspsConfig.ProtectedRangeBase[0]    = FlashProtectionConfig->ProtectRange[0].ProtectedRangeBase;
          FspsUpd->FspsConfig.ProtectedRangeLimit[0]   = FlashProtectionConfig->ProtectRange[0].ProtectedRangeLimit;
          FspsUpd->FspsConfig.ProtectedRangeBase[1]    = FlashProtectionConfig->ProtectRange[1].ProtectedRangeBase;
          FspsUpd->FspsConfig.ProtectedRangeLimit[1]   = FlashProtectionConfig->ProtectRange[1].ProtectedRangeLimit;
        }
      } // if (FlashProtectionEnabled)
    } // if ((GetBxtSeries() == BxtP) && (IsSpiBoot ()))
  } // if (Status == EFI_SUCCESS)
  DEBUG ((DEBUG_INFO, "PeiFspScPolicyInit - End\n"));
  return EFI_SUCCESS;
}

