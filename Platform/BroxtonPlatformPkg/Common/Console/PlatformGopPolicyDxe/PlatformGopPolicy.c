/** @file
  Platform GOP Driver Policy.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Guid/SetupVariable.h>
#include <SetupMode.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>

extern EFI_BOOT_SERVICES   *gBS;

PLATFORM_GOP_POLICY_PROTOCOL  mPlatformGOPPolicy;

//
// Function implementations
//

/**
  The function will excute with as the platform policy, and gives the
  Platform Lid Status. IBV/OEM can customize this code for their specific policy action.

  @param[out]  CurrentLidStatus      Gives the current LID Status

  @retval      EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
GetPlatformLidStatus (
  OUT LID_STATUS *CurrentLidStatus
  )
{
  if (CurrentLidStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *CurrentLidStatus = LidOpen;

  return EFI_SUCCESS;
}


/**
  The function will excute and gives the Video Bios Table Size and Address.

  @param[in]  VbtAddress   Gives the Physical Address of Video BIOS Table
  @param[in]  VbtSize      Gives the Size of Video BIOS Table

  @retval     EFI_STATUS

**/

EFI_STATUS
EFIAPI
GetVbtData (
   OUT EFI_PHYSICAL_ADDRESS *VbtAddress,
   OUT UINT32 *VbtSize
)
{

  VBT_INFO                     *VbtInfo = NULL;
  EFI_PEI_HOB_POINTERS         GuidHob;
  EFI_STATUS                   Status;
   
  //
  // Get VBT data from HOB, which has been created in PEI phase.
  //
  Status = EFI_NOT_FOUND;
  DEBUG ((DEBUG_ERROR, "GOP Policy Protocol GetVbtData from HOB\n"));
  
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
     if ((GuidHob.Raw = GetNextGuidHob (&gVbtInfoGuid, GuidHob.Raw)) != NULL) {
        VbtInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
        *VbtAddress = VbtInfo->VbtAddress;
        *VbtSize = VbtInfo->VbtSize;
        Status = EFI_SUCCESS;
        DEBUG ((DEBUG_ERROR, "Found VBT.\n"));
     }
  } 

  return Status;
}


/**
  Entry point for the Platform GOP Policy Driver.

  @param[in]  ImageHandle           Image handle of this driver.
  @param[in]  SystemTable           Global system service table.

  @retval     EFI_SUCCESS           Initialization complete.
  @retval     EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.

**/

EFI_STATUS
EFIAPI
PlatformGOPPolicyEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  SYSTEM_CONFIGURATION  SystemConfiguration;
  UINTN                 VarSize;

  gBS = SystemTable->BootServices;

  gBS->SetMem (&mPlatformGOPPolicy, sizeof (PLATFORM_GOP_POLICY_PROTOCOL), 0);

  mPlatformGOPPolicy.Revision                = PLATFORM_GOP_POLICY_PROTOCOL_REVISION_01;
  mPlatformGOPPolicy.GetPlatformLidStatus    = GetPlatformLidStatus;
  mPlatformGOPPolicy.GetVbtData              = GetVbtData;

  //
  // Install protocol to allow access to this Policy.
  //
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );
  ASSERT_EFI_ERROR (Status);

  if (SystemConfiguration.GOPEnable == 1) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gPlatformGOPPolicyGuid,
                    &mPlatformGOPPolicy,
                    NULL
                    );
  }

  return Status;
}

