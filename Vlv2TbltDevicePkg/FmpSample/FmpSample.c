/** @file
  Implement updatable firmware resource for Fmp Sample

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "FmpSample.h"

//
// FmpSample driver private data
//
FMP_SAMPLE_PRIVATE_DATA *mFmpSamplePrivate = NULL;


EFI_FIRMWARE_MANAGEMENT_PROTOCOL mFirmwareManagementProtocol = {
  FmpGetImageInfo,
  FmpGetImage,
  FmpSetImage,
  FmpCheckImage,
  FmpGetPackageInfo,
  FmpSetPackageInfo
};


EFI_STATUS
EFIAPI
FmpSampleMain (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;

  //
  // Initialize LanMacPrivateData
  //
  mFmpSamplePrivate = AllocateZeroPool (sizeof(FMP_SAMPLE_PRIVATE_DATA));
  if (mFmpSamplePrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = InitializePrivateData(mFmpSamplePrivate);
  if (EFI_ERROR(Status)) {
    FreePool(mFmpSamplePrivate);
    mFmpSamplePrivate = NULL;
  }

  //
  // Install FMP protocol.
  //
  Status = gBS->InstallProtocolInterface (
                  &mFmpSamplePrivate->Handle,
                  &gEfiFirmwareManagementProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mFmpSamplePrivate->Fmp
                  );
  if (EFI_ERROR (Status)) {
    FreePool(mFmpSamplePrivate);
    mFmpSamplePrivate = NULL;
    return Status;
  }

  return Status;
}


/**
  This is the default unload handle for FmpSample drivers

  Update driver does not open virtual handler by driver. So can not use
  Disconnect to call DriverBinding stop. Here we directly uninstall FMP
  and free resource

  @param[in]  ImageHandle       The drivers' driver image.

  @retval EFI_SUCCESS           The image is unloaded.
  @retval Others                Failed to unload the image.

**/
EFI_STATUS
EFIAPI
FmpSampleUnload (
  IN EFI_HANDLE             ImageHandle
  )
{
  //
  // If the driver has been connected
  //
  if (mFmpSamplePrivate != NULL) {
    //
    // Uninstall FMP protocol
    //
    gBS->UninstallMultipleProtocolInterfaces(
           mFmpSamplePrivate->Handle,
           &gEfiFirmwareManagementProtocolGuid,
           &mFmpSamplePrivate->Fmp,
           NULL
           );

    FreePool(mFmpSamplePrivate);
  }

  return EFI_SUCCESS;
}
