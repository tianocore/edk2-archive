/** @file
  This file is the library for SA DXE Policy initialzation.

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <DxeSaPolicyUpdate.h>


/**
  Get data for platform policy from setup options.

  @param[in] DxeSaPolicy              The pointer to get SA Policy protocol instance
  @param[in] SystemConfiguration      The pointer to get System Setup

  @retval    EFI_SUCCESS              Operation success.

**/
EFI_STATUS
EFIAPI
UpdateDxeSaPolicy (
  IN OUT SA_POLICY_PROTOCOL *SaPolicy,
  IN SYSTEM_CONFIGURATION   *SystemConfiguration
  )
{
#if defined (ENBDT_PF_ENABLE) && (ENBDT_PF_ENABLE == 1) // For ApolloLake
  PLATFORM_GOP_POLICY_PROTOCOL *GopPolicy;
  VBT_INFO                     *VbtInfo = NULL;
  EFI_PHYSICAL_ADDRESS         VbtAddress;
  UINT32                       Size;
  EFI_PEI_HOB_POINTERS         GuidHob;
#endif
  EFI_STATUS                   Status;
  SA_DXE_MISC_CONFIG           *SaDxeMiscConfig = NULL;
  IGD_PANEL_CONFIG             *IgdPanelConfig = NULL;

  DEBUG ((DEBUG_INFO, "UpdateDxeSaPolicy\n"));

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gIgdPanelConfigGuid, (VOID *) &IgdPanelConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gSaDxeMiscConfigGuid, (VOID *) &SaDxeMiscConfig);
  ASSERT_EFI_ERROR (Status);
#if defined (ENBDT_PF_ENABLE) && (ENBDT_PF_ENABLE == 1) // For ApolloLake
  DEBUG ((DEBUG_INFO, "Locate GopPolicy and GetVbtData\n"));

  //
  // Locate the GOP Policy Protocol.
  //
  GopPolicy = NULL;
  Status = gBS->LocateProtocol (
                  &gPlatformGOPPolicyGuid,
                  NULL,
                  (VOID **)&GopPolicy
                  );

  if (EFI_ERROR (Status) || (GopPolicy == NULL)) {
    return Status;
  }

  //
  // Get VBT data
  //
  VbtAddress      = 0;
  Size            = 0;
  DEBUG ((DEBUG_INFO, "GetVbtData\n"));
  Status          = GopPolicy->GetVbtData (&VbtAddress, &Size);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gVbtInfoGuid, GuidHob.Raw)) != NULL) {
      VbtInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
      VbtInfo->VbtAddress = VbtAddress;
      VbtInfo->VbtSize = Size;
      Status = EFI_SUCCESS;
    }
  } else {
    Status = EFI_NOT_FOUND;
    return Status;
  }
#endif

  IgdPanelConfig->PFITStatus     = SystemConfiguration->PanelScaling;
  SaDxeMiscConfig->S0ixSupported = SystemConfiguration->LowPowerS0Idle;
  IgdPanelConfig->PanelSelect    = SystemConfiguration->VbtSelect;

  return EFI_SUCCESS;
}

