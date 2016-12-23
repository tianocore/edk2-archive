/** @file
  Platform Configuration Update library implementation file.
  This library updates the setup data with platform overrides.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Guid/PlatformInfo.h>
#include <Guid/TpmInstance.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiPlatformConfigUpdateLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/SteppingLib.h>
#include <Ppi/DramPolicyPpi.h>

#define SETUP_NFC_Disabled 0
#define SETUP_NFC_IPT      1
#define SETUP_NFC          2

EFI_STATUS
TpmSetupPolicyInit (
  IN SYSTEM_CONFIGURATION    *SystemConfiguration
  )
{
#if FTPM_SUPPORT
  EFI_STATUS           Status;
  BOOLEAN              PttEnabledState = FALSE;
  EFI_HOB_GUID_TYPE    *FdoEnabledGuidHob = NULL;

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

  if (SystemConfiguration->TpmDetection == 0) {
    Status = PttHeciGetState (&PttEnabledState);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Get PTT enabled state failed.\n"));
    }

    if (PttEnabledState && (FdoEnabledGuidHob == NULL)) {
      SystemConfiguration->TPM = TPM_PTT;
    } else {
      DEBUG ((EFI_D_INFO, "TpmPolicyInit-TPM and TpmDetection is disabled because of FDO \n\r"));
      SystemConfiguration->TPM = TPM_DISABLE;
    }
    SystemConfiguration->TpmDetection = 1;
  }

#endif
  return EFI_SUCCESS;
}


EFI_STATUS
GetSecureNfcInfo (
  SYSTEM_CONFIGURATION  *SystemConfiguration
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_HOB_POINTERS            GuidHob;
  EFI_PLATFORM_INFO_HOB           *PlatformInfo = NULL;
  MEFWCAPS_SKU                    CurrentFeatures;
  UINT32                          EnableBitmap = 0;
  UINT32                          DisableBitmap = 0;
  BOOLEAN                         Cse_Nfc_Strap = FALSE;
  BOOLEAN                         SecureNfcInSetup = FALSE;
  BOOLEAN                         Fab_B_C = FALSE;

  DEBUG ((EFI_D_INFO, "GetSecureNfcInfo ++ \n"));

  if (SystemConfiguration->NfcSelect == SETUP_NFC_IPT) {
    SecureNfcInSetup = TRUE;
  }

  DEBUG ((EFI_D_INFO, "Old NfcSelect: %d\n", SystemConfiguration->NfcSelect));

  Status = HeciGetFwFeatureStateMsgII (&CurrentFeatures);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if (CurrentFeatures.Fields.NFC == 1) {
    Cse_Nfc_Strap = TRUE;
  }

  DEBUG ((EFI_D_INFO, "HeciGetFwFeatureStateMsgII CurrentFeatures.Fields.NFC: %d\n", CurrentFeatures.Fields.NFC));

  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      PlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  if (PlatformInfo == NULL) {
    ASSERT (PlatformInfo != NULL);
    return EFI_NOT_FOUND;
  }

  if ((!Cse_Nfc_Strap) && SecureNfcInSetup && (!Fab_B_C)){
    //
    // Enable secure NFC
    //
    DEBUG ((EFI_D_INFO, "Enable NFC\n"));
    EnableBitmap  = NFC_BITMASK;
    DisableBitmap = CLEAR_FEATURE_BIT;
  } else if ( (SecureNfcInSetup && Fab_B_C)     ||\
              (Cse_Nfc_Strap    && (!SecureNfcInSetup)) \
            ) {
    //
    // Disable secure NFC
    //
    DEBUG ((EFI_D_INFO, "Disable NFC\n"));
    EnableBitmap  = CLEAR_FEATURE_BIT;
    DisableBitmap = NFC_BITMASK;
    SystemConfiguration->NfcSelect = SETUP_NFC;
  }

  Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "NfcSelect: %d\n", SystemConfiguration->NfcSelect));

  return Status;
}


/**
  Updates Setup values from PlatformInfoHob and platform policies.

  @param  PreDefaultSetupData   A pointer to the setup data prior to being
                                placed in the default data HOB.

  @retval EFI_SUCCESS           The Setup data was updated successfully.

**/
EFI_STATUS
UpdateSetupDataValues (
  SYSTEM_CONFIGURATION     *PreDefaultSetupData
  )
{
  EFI_STATUS               Status;
  EFI_HOB_GUID_TYPE        *FdoEnabledGuidHob = NULL;
  DRAM_POLICY_PPI          *DramPolicyPpi;

  Status = PeiServicesLocatePpi (
             &gDramPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &DramPolicyPpi
             );

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Couldn't find DRAM policy PPI: %g, Status: %r.\n", &gDramPolicyPpiGuid, Status));
  } else {
    DEBUG ((EFI_D_INFO, "Overriding Memory System Config Data using DRAM Policy.\n"));
    PreDefaultSetupData->ChannelHashMask         = DramPolicyPpi->ChannelHashMask;
    PreDefaultSetupData->SliceHashMask           = DramPolicyPpi->SliceHashMask;
    PreDefaultSetupData->ChannelsSlicesEnabled   = DramPolicyPpi->ChannelsSlicesEnabled;
    PreDefaultSetupData->ScramblerSupport        = DramPolicyPpi->ScramblerSupport;
    PreDefaultSetupData->InterleavedMode         = DramPolicyPpi->InterleavedMode;
    PreDefaultSetupData->MinRefRate2xEnabled     = DramPolicyPpi->MinRefRate2xEnabled;
    PreDefaultSetupData->DualRankSupportEnabled  = DramPolicyPpi->DualRankSupportEnabled;
  }

  if (FdoEnabledGuidHob != NULL) {
    PreDefaultSetupData->SecureBoot = FALSE;
    PreDefaultSetupData->FprrEnable = FALSE;
    PreDefaultSetupData->ScBiosLock = FALSE;
    DEBUG ((EFI_D_INFO, "SPI FDO mode is enabled. Disabling SecureBoot, FprrEnable, and ScBiosLock.\n"));
  }

  Status = GetSecureNfcInfo (PreDefaultSetupData);
  ASSERT_EFI_ERROR (Status);

  Status = TpmSetupPolicyInit (PreDefaultSetupData);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

