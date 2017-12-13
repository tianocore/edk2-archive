/** @file
  This file is SampleCode of the library for Intel PCH PEI Policy initialization.

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiScPolicyUpdate.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Guid/PlatformInfo.h>
#include <Library/PeiScPolicyLib.h>
#include <Library/HeciMsgLib.h>
#include <SeCAccess.h>
#include <SeCChipset.h>
#include <Library/ConfigBlockLib.h>
#include <SeCState.h>
#include <Library/MmPciLib.h>

extern EFI_GUID gEfiBootMediaHobGuid;

typedef struct {
  UINT32  IbbOffset;
  UINT32  IbbSize;
  UINT32  ObbOffset;
  UINT32  ObbSize;
} IBB_OBB_INFORMATION;

/**
  Add verb table helper function.
  This function calculates verb table number and shows verb table information.

  @param[in, out] VerbTableEntryNum      Input current VerbTable number and output the number after adding new table
  @param[in, out] VerbTableArray         Pointer to array of VerbTable
  @param[in]      VerbTable              VerbTable which is going to add into array

**/
STATIC
VOID
InternalAddVerbTable (
  IN OUT  UINT8                   *VerbTableEntryNum,
  IN OUT  UINT32                  *VerbTableArray,
  IN      HDAUDIO_VERB_TABLE      *VerbTable
  )
{
  if (VerbTable == NULL) {
    DEBUG ((DEBUG_INFO, "InternalAddVerbTable wrong input: VerbTable == NULL\n"));
    return;
  }

  VerbTableArray[*VerbTableEntryNum] = (UINT32) VerbTable;
  *VerbTableEntryNum += 1;

  DEBUG ((DEBUG_INFO,
    "Add verb table for VendorDeviceId = 0x%08X (size = %d DWords)\n",
    VerbTable->VerbTableHeader.VendorDeviceId,
    VerbTable->VerbTableHeader.DataDwords)
    );
  return;
}


/**
  Check it's eMMC boot path or not.

  @retval TRUE                       eMMC Boot path
  @retval FALSE                      Not eMMC boot path

**/
BOOLEAN
IseMMCBoot (
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
IsSpiBoot (
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
  Calculate the Address of Boot Partition 1.

  @param[out] Address               The address

  @retval     EFI_SUCCESS           The operation completed successfully.
  @retval     EFI_DEVICE_ERROR

**/
EFI_STATUS
EFIAPI
FindBootPartition1 (
  OUT UINT32                    *Address
  )
{
  UINT32  SecondBPFlashLinearAddress;
  UINT32  BiosAddr;

  //
  //Compute Second BP FlashLinearAddress
  //
  SecondBPFlashLinearAddress = 0x1000;

  DEBUG ((DEBUG_INFO, "SecondBPFlashLinearAddress = %X\n", SecondBPFlashLinearAddress));
  //
  // Calculate Boot partition #2 physical address
  // FLA[26:0] <= (Flash_Regionn_Limit) - (FFFF_FFFCh - bios_address)
  //
  BiosAddr = GetSpiFlashRegionLimit (BIOS) + 0xFFC - SecondBPFlashLinearAddress;
  *Address = 0xFFFFFFFC - BiosAddr;

  DEBUG ((DEBUG_INFO, "system BP1 Address = %X\n", *Address));

  return EFI_SUCCESS;
}

/**
  Calculate the Address of Boot Partition 2.

  @param[out] Address               The address

  @retval     EFI_SUCCESS           The operation completed successfully.
  @retval     EFI_DEVICE_ERROR

**/
EFI_STATUS
EFIAPI
FindBootPartition2 (
  OUT UINT32                    *Address
  )
{
  UINT32  SecondBPFlashLinearAddress;
  UINT32  BiosAddr;

  //
  // Compute Second BP FlashLinearAddress
  //
  if (HeciPciRead16 (R_SEC_DevID_VID) != 0xFFFF) {
    //
    // BP2 linear address is the midpoint between BIOS base and expansion data base
    //
    SecondBPFlashLinearAddress = (GetSpiFlashRegionBase (BIOS) + GetSpiFlashRegionBase (DeviceExpansion1)) / 2;
  } else {
    //
    // W/A for non-secure boot
    //
    SecondBPFlashLinearAddress = FixedPcdGet32 (PcdFlashAreaSize) >> 1;
  }

  DEBUG ((DEBUG_INFO, "SecondBPFlashLinearAddress = %X\n", SecondBPFlashLinearAddress));
  //
  // Calculate Boot partition #2 physical address
  // FLA[26:0] <= (Flash_Regionn_Limit) - (FFFF_FFFCh - bios_address)
  //
  BiosAddr = GetSpiFlashRegionLimit (BIOS) + 0xFFC - SecondBPFlashLinearAddress;
  *Address = 0xFFFFFFFC - BiosAddr;

  DEBUG ((DEBUG_INFO, "FlashRegionLimit = %X\n", *Address));

  return EFI_SUCCESS;
}


/**
  This function performs PCH PEI Policy initialization.

  @param[in, out] ScPolicy                The PCH Policy PPI instance

  @retval         EFI_SUCCESS             The PPI is installed and initialized.
  @retval         EFI ERRORS              The PPI is not successfully installed.
  @retval         EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
UpdatePeiScPolicy (
  IN OUT SC_POLICY_PPI *ScPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_HOB_POINTERS            Hob;
  EFI_PLATFORM_INFO_HOB           *PlatformInfo;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINT16                          BoardId;
  UINTN                           VariableSize;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINT8                           PortIndex;
  UINT32                          SpiHsfsReg;
  UINT32                          SpiFdodReg;
  UINT8                           DevIndex;
  UINT8                           HdaIndex;
  SC_GENERAL_CONFIG               *GeneralConfig;
  SC_SATA_CONFIG                  *SataConfig;
  SC_PCIE_CONFIG                  *PcieConfig;
  SC_USB_CONFIG                   *UsbConfig;
  SC_HPET_CONFIG                  *HpetConfig;
  SC_IOAPIC_CONFIG                *IoApicConfig;
  SC_HDAUDIO_CONFIG               *HdaConfig;
  SC_GMM_CONFIG                   *GmmConfig;
  SC_PM_CONFIG                    *PmConfig;
  SC_LOCK_DOWN_CONFIG             *LockDownConfig;
  SC_LPSS_CONFIG                  *LpssConfig;
  SC_SCS_CONFIG                   *ScsConfig;
  SC_VTD_CONFIG                   *VtdConfig;
  SC_ISH_CONFIG                   *IshConfig;
  SC_FLASH_PROTECTION_CONFIG      *FlashProtectionConfig;
  SC_DCI_CONFIG                   *DciConfig;
  EFI_HOB_GUID_TYPE               *FdoEnabledGuidHob = NULL;
  SC_INTERRUPT_CONFIG             *InterruptConfig;

  //
  // The PlatformInfoHob is default at this point, which is initialized after memory installed in PlatformInit.c
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfo = GET_GUID_HOB_DATA (Hob.Raw);

  BoardId = PlatformInfo->BoardId;

  //
  // Retrieve Setup variable
  //
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );
  ASSERT_EFI_ERROR (Status);

  //
  // Install SC Policy PPI. As we depend on SC Init PPI so we are executed after
  // ScInit PEIM. Thus we can insure SC Initialization is performed when we install the SC Policy PPI,
  // as ScInit PEIM registered a notification function on our policy PPI.
  //
  // For better code structure / modularity, we should use a notification function on SC Init PPI to perform
  // actions that depend on ScInit PEIM's initialization.
  //
  DEBUG ((DEBUG_INFO, "UpdatePeiScPolicy() - Start\n"));
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gScGeneralConfigGuid, (VOID *) &GeneralConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gSataConfigGuid, (VOID *) &SataConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gPcieRpConfigGuid, (VOID *) &PcieConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gHpetConfigGuid, (VOID *) &HpetConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gIoApicConfigGuid, (VOID *) &IoApicConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gHdAudioConfigGuid, (VOID *) &HdaConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gGmmConfigGuid, (VOID *) &GmmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gPmConfigGuid, (VOID *) &PmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gLpssConfigGuid, (VOID *) &LpssConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gScsConfigGuid, (VOID *) &ScsConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gIshConfigGuid, (VOID *) &IshConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gFlashProtectionConfigGuid, (VOID *) &FlashProtectionConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gDciConfigGuid, (VOID *) &DciConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gInterruptConfigGuid, (VOID *) &InterruptConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Read ACPI and P2SB Base Addresses
  //
  GeneralConfig->PmcBase       = (UINT32) PcdGet32 (PcdPmcGcrBaseAddress);
  GeneralConfig->AcpiBase      = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);
  GeneralConfig->P2sbBase      = (UINT32) PcdGet32 (PcdP2SBBaseAddress);
  GeneralConfig->Crid          = SystemConfiguration.CRIDSettings;
  GeneralConfig->ResetSelect   = (UINT8) PcdGet8 (PcdResetType);
  HpetConfig->Enable           = SystemConfiguration.Hpet;
  HpetConfig->Base             = HPET_BASE_ADDRESS;
  HpetConfig->BdfValid         = 0x01;
  HpetConfig->BusNumber        = 0xFA;
  HpetConfig->DeviceNumber     = 0x0F;
  HpetConfig->FunctionNumber   = 0;
  IoApicConfig->IoApicId       = 0x01;
  IoApicConfig->BdfValid       = 1;
  IoApicConfig->BusNumber      = 0xFA;
  IoApicConfig->DeviceNumber   = 0x1F;
  IoApicConfig->FunctionNumber = 0;

  SpiHsfsReg = MmioRead32 (SPI_BASE_ADDRESS + R_SPI_HSFS);
  if ((SpiHsfsReg & B_SPI_HSFS_FDV) == B_SPI_HSFS_FDV) {
    MmioWrite32 (SPI_BASE_ADDRESS + R_SPI_FDOC, V_SPI_FDOC_FDSS_FSDM);
    SpiFdodReg = MmioRead32 (SPI_BASE_ADDRESS + R_SPI_FDOD);
    if (SpiFdodReg == V_SPI_FDBAR_FLVALSIG) {
    }
  }

  //
  // Update PCIe config
  //
  PcieConfig->DisableRootPortClockGating   = SystemConfiguration.PcieClockGatingDisabled;
  PcieConfig->EnablePort8xhDecode          = SystemConfiguration.PcieRootPort8xhDecode;
  PcieConfig->ScPciePort8xhDecodePortIndex = SystemConfiguration.Pcie8xhDecodePortIndex;
  PcieConfig->EnablePeerMemoryWrite        = SystemConfiguration.PcieRootPortPeerMemoryWriteEnable;
  PcieConfig->ComplianceTestMode           = SystemConfiguration.PcieComplianceMode;
  for (PortIndex = 0; PortIndex < SC_MAX_PCIE_ROOT_PORTS; PortIndex++) {
    PcieConfig->RootPort[PortIndex].Enable                        = SystemConfiguration.PcieRootPortEn[PortIndex];
    PcieConfig->RootPort[PortIndex].SlotImplemented               = TRUE;
    PcieConfig->RootPort[PortIndex].PhysicalSlotNumber            = (UINT8) PortIndex;
    PcieConfig->RootPort[PortIndex].Aspm                          = SystemConfiguration.PcieRootPortAspm[PortIndex];
    PcieConfig->RootPort[PortIndex].L1Substates                   = SystemConfiguration.PcieRootPortL1SubStates[PortIndex];
    PcieConfig->RootPort[PortIndex].AcsEnabled                    = SystemConfiguration.PcieRootPortACS[PortIndex];
    PcieConfig->RootPort[PortIndex].PmSci                         = SystemConfiguration.PcieRootPortPMCE[PortIndex];
    PcieConfig->RootPort[PortIndex].HotPlug                       = SystemConfiguration.PcieRootPortHPE[PortIndex];
    PcieConfig->RootPort[PortIndex].AdvancedErrorReporting        = FALSE;
    PcieConfig->RootPort[PortIndex].UnsupportedRequestReport      = SystemConfiguration.PcieRootPortURE[PortIndex];
    PcieConfig->RootPort[PortIndex].FatalErrorReport              = SystemConfiguration.PcieRootPortFEE[PortIndex];
    PcieConfig->RootPort[PortIndex].NoFatalErrorReport            = SystemConfiguration.PcieRootPortNFE[PortIndex];
    PcieConfig->RootPort[PortIndex].CorrectableErrorReport        = SystemConfiguration.PcieRootPortCEE[PortIndex];
    PcieConfig->RootPort[PortIndex].PmeInterrupt                  = 0;
    PcieConfig->RootPort[PortIndex].SystemErrorOnFatalError       = SystemConfiguration.PcieRootPortSFE[PortIndex];
    PcieConfig->RootPort[PortIndex].SystemErrorOnNonFatalError    = SystemConfiguration.PcieRootPortSNE[PortIndex];
    PcieConfig->RootPort[PortIndex].SystemErrorOnCorrectableError = SystemConfiguration.PcieRootPortSCE[PortIndex];
    PcieConfig->RootPort[PortIndex].TransmitterHalfSwing          = SystemConfiguration.PcieRootPortTHS[PortIndex];
    PcieConfig->RootPort[PortIndex].CompletionTimeout             = ScPcieCompletionTO_Default;
    PcieConfig->RootPort[PortIndex].PcieSpeed                     = SystemConfiguration.PcieRootPortSpeed[PortIndex];
    PcieConfig->RootPort[PortIndex].SelectableDeemphasis          = SystemConfiguration.PcieRootPortSelectableDeemphasis[PortIndex];
    // LTR settings
    PcieConfig->RootPort[PortIndex].LtrEnable                         = SystemConfiguration.PchPcieLtrEnable[PortIndex];
    PcieConfig->RootPort[PortIndex].LtrConfigLock                     = SystemConfiguration.PchPcieLtrConfigLock[PortIndex];
    PcieConfig->RootPort[PortIndex].LtrMaxSnoopLatency                = SystemConfiguration.PcieLtrMaxSnoopLatency[PortIndex];
    PcieConfig->RootPort[PortIndex].LtrMaxNoSnoopLatency              = SystemConfiguration.PcieLtrMaxNoSnoopLatency[PortIndex];
    PcieConfig->RootPort[PortIndex].SnoopLatencyOverrideMode          = SystemConfiguration.PchPcieSnoopLatencyOverrideMode[PortIndex];
    PcieConfig->RootPort[PortIndex].SnoopLatencyOverrideMultiplier    = SystemConfiguration.PchPcieSnoopLatencyOverrideMultiplier[PortIndex];
    PcieConfig->RootPort[PortIndex].SnoopLatencyOverrideValue         = SystemConfiguration.PchPcieSnoopLatencyOverrideValue[PortIndex];
    PcieConfig->RootPort[PortIndex].NonSnoopLatencyOverrideMode       = SystemConfiguration.PchPcieNonSnoopLatencyOverrideMode[PortIndex];
    PcieConfig->RootPort[PortIndex].NonSnoopLatencyOverrideMultiplier = SystemConfiguration.PchPcieNonSnoopLatencyOverrideMultiplier[PortIndex];
    PcieConfig->RootPort[PortIndex].NonSnoopLatencyOverrideValue      = SystemConfiguration.PchPcieNonSnoopLatencyOverrideValue[PortIndex];
    PcieConfig->RootPort[PortIndex].PtmEnable = TRUE;
  }
#if (ENBDT_PF_ENABLE == 1)
  PcieConfig->RootPort[0].ClkReqSupported = TRUE;
  PcieConfig->RootPort[0].ClkReqNumber    = 2;
  PcieConfig->RootPort[1].ClkReqSupported = TRUE;
  PcieConfig->RootPort[1].ClkReqNumber    = 3;
  PcieConfig->RootPort[2].ClkReqSupported = TRUE;
  PcieConfig->RootPort[2].ClkReqNumber    = 0;
  PcieConfig->RootPort[3].ClkReqSupported = FALSE;
  PcieConfig->RootPort[3].ClkReqNumber    = 0xF;
  PcieConfig->RootPort[4].ClkReqSupported = TRUE;
  PcieConfig->RootPort[4].ClkReqNumber    = 0x1;
  PcieConfig->RootPort[5].ClkReqSupported = FALSE;
  PcieConfig->RootPort[5].ClkReqNumber    = 0xF;
#endif

  PcieConfig->RootPort[0].ClkReqSupported = TRUE;
  PcieConfig->RootPort[0].ClkReqNumber    = 2;
  PcieConfig->RootPort[1].ClkReqSupported = TRUE;
  PcieConfig->RootPort[1].ClkReqNumber    = 3;
  PcieConfig->RootPort[2].ClkReqSupported = TRUE;
  PcieConfig->RootPort[2].ClkReqNumber    = 1;
  PcieConfig->RootPort[4].ClkReqSupported = TRUE;
  PcieConfig->RootPort[4].ClkReqNumber    = 0;

  //
  // Update SATA config
  //
  SataConfig->Enable   = SystemConfiguration.Sata;
  SataConfig->SataMode = SystemConfiguration.SataInterfaceMode;
  for (PortIndex = 0; PortIndex < SC_MAX_SATA_PORTS; PortIndex++) {
    if (SystemConfiguration.SataTestMode == TRUE) {
      SataConfig->PortSettings[PortIndex].Enable = TRUE;
    } else {
      SataConfig->PortSettings[PortIndex].Enable = SystemConfiguration.SataPort[PortIndex];
    }
    SataConfig->PortSettings[PortIndex].HotPlug          = SystemConfiguration.SataHotPlug[PortIndex];
    SataConfig->PortSettings[PortIndex].SpinUp           = SystemConfiguration.SataSpinUp[PortIndex];
    SataConfig->PortSettings[PortIndex].External         = FALSE;
    SataConfig->PortSettings[PortIndex].DevSlp           = SystemConfiguration.PxDevSlp[PortIndex];
    SataConfig->PortSettings[PortIndex].EnableDitoConfig = SystemConfiguration.EnableDitoConfig[PortIndex];
    SataConfig->PortSettings[PortIndex].DmVal            = SystemConfiguration.DmVal[PortIndex];
    SataConfig->PortSettings[PortIndex].DitoVal          = SystemConfiguration.DitoVal[PortIndex];
    SataConfig->PortSettings[PortIndex].SolidStateDrive  = SystemConfiguration.SataType[PortIndex];
  }
  SataConfig->SalpSupport = SystemConfiguration.SataSalp;
  SataConfig->TestMode    = SystemConfiguration.SataTestMode;

  //
  // Flash Security Recommendation,
  // Intel strongly recommends that BIOS sets the BIOS Interface Lock Down bit. Enabling this bit
  // will mitigate malicious software attempts to replace the system BIOS option ROM with its own code.
  // BIOS lock down bit is enabled by default as per Platform policy, except that when system is in recovery mode or FDO is enabled. In this case this will be disabled as part of Firmware Update / Recovery update
  //
  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

  if ((GetBxtSeries() == BxtP) && (FdoEnabledGuidHob == NULL)) {
    LockDownConfig->BiosInterface = TRUE;
    LockDownConfig->BiosLock      = SystemConfiguration.ScBiosLock;
    if (SystemConfiguration.ScBiosLock) {
      LockDownConfig->SpiEiss     = TRUE;
  } else {
      LockDownConfig->SpiEiss     = FALSE;
  }
  } else {
    DEBUG ((DEBUG_INFO, " BIOS lock is not done  -CSE Status -FDO ASSERTED\n"));
    LockDownConfig->BiosInterface = FALSE;
    LockDownConfig->BiosLock      = FALSE;
    LockDownConfig->SpiEiss       = FALSE;
  }
  LockDownConfig->RtcLock         = SystemConfiguration.RtcLock;
  LockDownConfig->TcoLock         = SystemConfiguration.TcoLock;

  HdaConfig->Enable               = SystemConfiguration.ScHdAudio;
  HdaConfig->DspEnable            = SystemConfiguration.ScHdAudioDsp;
  HdaConfig->Mmt                  = SystemConfiguration.ScHdAudioMmt;
  HdaConfig->Hmt                  = SystemConfiguration.ScHdAudioHmt;
  HdaConfig->IoBufferOwnership    = SystemConfiguration.ScHdAudioIoBufferOwnership;
  HdaConfig->BiosCfgLockDown      = SystemConfiguration.ScHdAudioBiosCfgLockDown;
  HdaConfig->PwrGate              = SystemConfiguration.ScHdAudioPwrGate;
  HdaConfig->ClkGate              = SystemConfiguration.ScHdAudioClkGate;
  HdaConfig->Pme                  = SystemConfiguration.ScHdAudioPme;
  HdaConfig->DspEndpointDmic      = SystemConfiguration.ScHdAudioNhltEndpointDmic;
  HdaConfig->DspEndpointBluetooth = SystemConfiguration.ScHdAudioNhltEndpointBt;
  if (HdaConfig->IoBufferOwnership == ScHdaIoBufOwnerHdaLink) {
    HdaConfig->DspEndpointI2sSkp = FALSE;
    HdaConfig->DspEndpointI2sHp  = FALSE;
  } else {
    HdaConfig->DspEndpointI2sSkp   = SystemConfiguration.ScHdAudioNhltEndpointI2sSKP;
    HdaConfig->DspEndpointI2sHp    = SystemConfiguration.ScHdAudioNhltEndpointI2sHP;
  }
  HdaConfig->RsvdBits3 = SystemConfiguration.ScHdAduioRsvd1;
  HdaConfig->VcType                = SystemConfiguration.SvHdaVcType;
  HdaConfig->HdAudioLinkFrequency  = SystemConfiguration.HdAudioLinkFrequency;
  HdaConfig->IDispLinkFrequency    = SystemConfiguration.IDispLinkFrequency;
  HdaConfig->IDispLinkTmode        = SystemConfiguration.IDispLinkTmode;
  for(HdaIndex = 0; HdaIndex < HDAUDIO_FEATURES; HdaIndex++) {
    HdaConfig->DspFeatureMask |= (UINT32) (SystemConfiguration.ScHdAudioFeature[HdaIndex] ? (1 << HdaIndex) : 0);
  }
  for(HdaIndex = 0; HdaIndex < HDAUDIO_PP_MODULES; HdaIndex++) {
    HdaConfig->DspPpModuleMask |= (UINT32) (SystemConfiguration.ScHdAudioPostProcessingMod[HdaIndex] ? (1 << HdaIndex) : 0);
  }
  HdaConfig->ResetWaitTimer = 300;

  //
  // Update GMM config
  //
  GmmConfig->Enable                  = SystemConfiguration.Gmm;
  GmmConfig->ClkGatingPgcbClkTrunk   = SystemConfiguration.GmmCgPGCBEnabled;
  GmmConfig->ClkGatingSb             = SystemConfiguration.GmmCgSBDEnabled;
  GmmConfig->ClkGatingSbClkTrunk     = SystemConfiguration.GmmCgSBTEnabled;
  GmmConfig->ClkGatingSbClkPartition = SystemConfiguration.GmmCgSBPEnabled;
  GmmConfig->ClkGatingCore           = SystemConfiguration.GmmCgCoreEnabled;
  GmmConfig->ClkGatingDma            = SystemConfiguration.GmmCgDmaEnabled;
  GmmConfig->ClkGatingRegAccess      = SystemConfiguration.GmmCgRAEnabled;
  GmmConfig->ClkGatingHost           = SystemConfiguration.GmmCgHostEnabled;
  GmmConfig->ClkGatingPartition      = SystemConfiguration.GmmCgPEnabled;
  GmmConfig->ClkGatingTrunk          = SystemConfiguration.GmmCgTEnabled;
  if (SystemConfiguration.GmmPgHwAutoEnabled) {
    GmmConfig->SvPwrGatingHwAutoEnable = TRUE;
    GmmConfig->SvPwrGatingD3HotEnable  = FALSE;
    GmmConfig->SvPwrGatingI3Enable     = FALSE;
    GmmConfig->SvPwrGatingPmcReqEnable = FALSE;
  } else {
    GmmConfig->SvPwrGatingHwAutoEnable = FALSE;
    GmmConfig->SvPwrGatingD3HotEnable  = SystemConfiguration.GmmPgD3HotEnabled;
    GmmConfig->SvPwrGatingI3Enable     = SystemConfiguration.GmmPgI3Enabled;
    GmmConfig->SvPwrGatingPmcReqEnable = SystemConfiguration.GmmPgPMCREnabled;
  }
  //
  // Set ISH configuration according to setup value.
  //
  IshConfig->Enable   = SystemConfiguration.ScIshEnabled;

  //
  // USB Device 21 configuration
  //
  UsbConfig->DisableComplianceMode      = SystemConfiguration.DisableComplianceMode;

  //
  // xHCI (USB 3.0) related settings from setup variable
  //
  UsbConfig->Usb30Settings.Mode = SystemConfiguration.ScUsb30Mode;

  UsbConfig->SsicConfig.SsicPort[0].Enable = SystemConfiguration.Ssic1Support;
  UsbConfig->SsicConfig.SsicPort[1].Enable = SystemConfiguration.Ssic2Support;
  UsbConfig->SsicConfig.SsicPort[0].Rate   = SystemConfiguration.Ssic1Rate;
  UsbConfig->SsicConfig.SsicPort[1].Rate   = SystemConfiguration.Ssic2Rate;
  UsbConfig->SsicConfig.DlanePwrGating     = SystemConfiguration.SsicDlanePg;
  UsbConfig->XdciConfig.Enable             = SystemConfiguration.ScUsbOtg;
  //
  // Overcurrent applies to walk-up xHCI ports only - not SSIC or HSIC
  //
  //  OC0: Used for the OTG port (port 0)
  //  OC1: Used for the 2 host walk-up ports
  //
#if (ENBDT_PF_ENABLE == 1)
  UsbConfig->PortUsb20[0].OverCurrentPin = ScUsbOverCurrentPin0;
  UsbConfig->PortUsb20[1].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[2].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[3].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[4].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[5].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[6].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[7].OverCurrentPin = ScUsbOverCurrentPinSkip;

  UsbConfig->PortUsb30[0].OverCurrentPin = ScUsbOverCurrentPin0;
  UsbConfig->PortUsb30[1].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb30[2].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb30[3].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb30[4].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb30[5].OverCurrentPin = ScUsbOverCurrentPin1;

  for (DevIndex = 0; DevIndex < GetScXhciMaxUsb2PortNum (); DevIndex++) {
    UsbConfig->PortUsb20[DevIndex].Enable = SystemConfiguration.PortUsb20[DevIndex];
  }
  for (DevIndex = 0; DevIndex < GetScXhciMaxUsb3PortNum (); DevIndex++) {
    UsbConfig->PortUsb30[DevIndex].Enable = SystemConfiguration.PortUsb30[DevIndex];
  }
#else
  UsbConfig->PortUsb20[0].OverCurrentPin = ScUsbOverCurrentPin0;
  UsbConfig->PortUsb20[1].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[2].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb20[3].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[4].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[5].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[6].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[7].OverCurrentPin = ScUsbOverCurrentPinSkip;

  UsbConfig->PortUsb30[0].OverCurrentPin = ScUsbOverCurrentPin0;
  UsbConfig->PortUsb30[1].OverCurrentPin = ScUsbOverCurrentPin1;
  UsbConfig->PortUsb30[2].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb30[3].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb30[4].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb30[5].OverCurrentPin = ScUsbOverCurrentPinSkip;
  UsbConfig->PortUsb20[0].Enable = TRUE;
  UsbConfig->PortUsb20[1].Enable = TRUE;
  UsbConfig->PortUsb20[2].Enable = TRUE;
  UsbConfig->PortUsb20[3].Enable = FALSE;
  UsbConfig->PortUsb20[4].Enable = FALSE;
  UsbConfig->PortUsb20[5].Enable = FALSE;
  UsbConfig->PortUsb20[6].Enable = FALSE;
  UsbConfig->PortUsb20[7].Enable = FALSE;

  UsbConfig->PortUsb30[0].Enable = TRUE;
  UsbConfig->PortUsb30[1].Enable = TRUE;
  UsbConfig->PortUsb30[2].Enable = FALSE;
  UsbConfig->PortUsb30[3].Enable = FALSE;
  UsbConfig->PortUsb30[4].Enable = FALSE;
  UsbConfig->PortUsb30[5].Enable = FALSE;
#endif
  //
  // Set LPSS configuration according to setup value.
  //
  LpssConfig->I2c0Enable    = SystemConfiguration.LpssI2C0Enabled;
  LpssConfig->I2c1Enable    = SystemConfiguration.LpssI2C1Enabled;
  LpssConfig->I2c2Enable    = SystemConfiguration.LpssI2C2Enabled;
  LpssConfig->I2c3Enable    = SystemConfiguration.LpssI2C3Enabled;
  LpssConfig->I2c4Enable    = SystemConfiguration.LpssI2C4Enabled;
  LpssConfig->I2c5Enable    = SystemConfiguration.LpssI2C5Enabled;
  LpssConfig->I2c6Enable    = SystemConfiguration.LpssI2C6Enabled;
  LpssConfig->I2c7Enable    = SystemConfiguration.LpssI2C7Enabled;
  LpssConfig->Hsuart0Enable = SystemConfiguration.LpssHsuart0Enabled;
  LpssConfig->Hsuart1Enable = SystemConfiguration.LpssHsuart1Enabled;
  LpssConfig->Hsuart2Enable = SystemConfiguration.LpssHsuart2Enabled;
  LpssConfig->Hsuart3Enable = SystemConfiguration.LpssHsuart3Enabled;
  LpssConfig->Spi0Enable    = SystemConfiguration.LpssSpi0Enabled;
  LpssConfig->Spi1Enable    = SystemConfiguration.LpssSpi1Enabled;
  LpssConfig->Spi2Enable    = SystemConfiguration.LpssSpi2Enabled;
  LpssConfig->Uart2KernelDebugBaseAddress = SystemConfiguration.Uart2KernelDebugBaseAddress;
  for (DevIndex = 0; DevIndex < LPSS_I2C_DEVICE_NUM; DevIndex ++) {
    LpssConfig->I2cClkGateCfg[DevIndex] = SystemConfiguration.LpssI2cClkGateCfg[DevIndex];
  }
  for (DevIndex = 0; DevIndex < LPSS_HSUART_DEVICE_NUM; DevIndex ++) {
    LpssConfig->HsuartClkGateCfg[DevIndex] = SystemConfiguration.LpssHsuartClkGateCfg[DevIndex];
  }
  for (DevIndex = 0; DevIndex < LPSS_SPI_DEVICE_NUM; DevIndex ++) {
    LpssConfig->SpiClkGateCfg[DevIndex] = SystemConfiguration.LpssSpiClkGateCfg[DevIndex];
  }
  //
  // Kernel Debugger (e.g. WinDBG) Enable
  //
  LpssConfig->OsDbgEnable = (BOOLEAN) SystemConfiguration.OsDbgEnable;
  LpssConfig->S0ixEnable  = (BOOLEAN) SystemConfiguration.LowPowerS0Idle;
  //
  // Set SCS configuration according to setup value.
  //
  ScsConfig->SdcardEnable       = SystemConfiguration.SccSdcardEnabled;
  ScsConfig->UfsEnable          = 0;
  ScsConfig->EmmcEnable         = SystemConfiguration.ScceMMCEnabled;
  ScsConfig->SdioEnable         = SystemConfiguration.SccSdioEnabled;
  ScsConfig->EmmcHostMaxSpeed   = (UINT8) PcdGet8 (PcdeMMCHostMaxSpeed);
  ScsConfig->GppLock            = SystemConfiguration.GPPLock;
  ScsConfig->SccEmmcTraceLength = SCC_EMMC_LONG_TRACE_LEN;

  if (SystemConfiguration.ScceMMCEnabled == DEVICE_AUTO) {
    if (IseMMCBoot ()) {
      ScsConfig->EmmcEnable = DEVICE_ENABLE;
    } else {
      ScsConfig->EmmcEnable = DEVICE_DISABLE;
    }
  }

  VtdConfig->VtdEnable = SystemConfiguration.VTdEnable;
  //
  // Power management Configuration
  //
  if (SystemConfiguration.Cg8254 == 0) {
    PmConfig->Timer8254ClkGateEn = FALSE;
  }
  PmConfig->PowerButterDebounceMode = SystemConfiguration.PowerButterDebounceMode;


  DciConfig->DciEn         = SystemConfiguration.DciEn;
  DciConfig->DciAutoDetect = SystemConfiguration.DciAutoDetect;

  return Status;
}

