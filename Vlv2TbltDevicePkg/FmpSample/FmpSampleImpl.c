/** @file
  Implement updatable firmware resource for Fmp Sample

  Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "FmpSample.h"

/**
  Returns information about the current firmware image(s) of the device.

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]      This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in, out] ImageInfoSize      A pointer to the size, in bytes, of the ImageInfo buffer. 
                                     On input, this is the size of the buffer allocated by the caller.
                                     On output, it is the size of the buffer returned by the firmware 
                                     if the buffer was large enough, or the size of the buffer needed
                                     to contain the image(s) information if the buffer was too small.
  @param[in, out] ImageInfo          A pointer to the buffer in which firmware places the current image(s)
                                     information. The information is an array of EFI_FIRMWARE_IMAGE_DESCRIPTORs.
  @param[out]     DescriptorVersion  A pointer to the location in which firmware returns the version number
                                     associated with the EFI_FIRMWARE_IMAGE_DESCRIPTOR. 
  @param[out]     DescriptorCount    A pointer to the location in which firmware returns the number of
                                     descriptors or firmware images within this device.
  @param[out]     DescriptorSize     A pointer to the location in which firmware returns the size, in bytes,
                                     of an individual EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     PackageVersion     A version number that represents all the firmware images in the device.
                                     The format is vendor specific and new version must have a greater value
                                     than the old version. If PackageVersion is not supported, the value is
                                     0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version comparison
                                     is to be performed using PackageVersionName. A value of 0xFFFFFFFD indicates
                                     that package version update is in progress.
  @param[out]     PackageVersionName A pointer to a pointer to a null-terminated string representing the
                                     package version name. The buffer is allocated by this function with 
                                     AllocatePool(), and it is the caller's responsibility to free it with a call
                                     to FreePool().

  @retval EFI_SUCCESS                The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL       The ImageInfo buffer was too small. The current buffer size 
                                     needed to hold the image(s) information is returned in ImageInfoSize.                                               
  @retval EFI_INVALID_PARAMETER      ImageInfoSize is NULL.
  @retval EFI_DEVICE_ERROR           Valid information could not be returned. Possible corrupted image.

**/
EFI_STATUS
EFIAPI FmpGetImageInfo(
  IN        EFI_FIRMWARE_MANAGEMENT_PROTOCOL*This,
  IN OUT    UINTN                           *ImageInfoSize,
  IN OUT    EFI_FIRMWARE_IMAGE_DESCRIPTOR   *ImageInfo,
  OUT       UINT32                          *DescriptorVersion,
  OUT       UINT8                           *DescriptorCount,
  OUT       UINTN                           *DescriptorSize,
  OUT       UINT32                          *PackageVersion,
  OUT       CHAR16                          **PackageVersionName
  ) 
{
  FMP_SAMPLE_PRIVATE_DATA *FmpSamplePrivate;

  if(ImageInfoSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*ImageInfoSize < sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR)) {
    *ImageInfoSize = sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR);
    return EFI_BUFFER_TOO_SMALL;
  }

  if (ImageInfo == NULL 
   || DescriptorVersion == NULL
   || DescriptorCount == NULL
   || DescriptorSize == NULL
   || PackageVersion == NULL
   || PackageVersionName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FmpSamplePrivate = FMP_SAMPLE_PRIVATE_DATA_FROM_FMP(This);

  *ImageInfoSize      = sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR) * FmpSamplePrivate->DescriptorCount;
  *DescriptorSize     = sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  *DescriptorCount    = FmpSamplePrivate->DescriptorCount;
  *DescriptorVersion  = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
  //
  // Do not support package version
  //
  *PackageVersion     = 0xFFFFFFFF;
  *PackageVersionName = NULL;

  //
  // LanMacUpdate supports 1 ImageInfo descriptor
  //
  ImageInfo[0].ImageIndex  = 0x01;
  ImageInfo[0].ImageTypeId = FmpSamplePrivate->EsrtInfo.FwClass;
  ImageInfo[0].ImageId     = 0x01;
  ImageInfo[0].ImageIdName = NULL;
  ImageInfo[0].VersionName = NULL; 
  ImageInfo[0].Size        = sizeof(FmpSamplePrivate->UpdateData);
  ImageInfo[0].AttributesSupported = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED | IMAGE_ATTRIBUTE_IN_USE;
  ImageInfo[0].AttributesSetting   = IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED | IMAGE_ATTRIBUTE_IN_USE;
  ImageInfo[0].Compatibilities     = 0x0;
  ImageInfo[0].Version             = FmpSamplePrivate->EsrtInfo.FwVersion;
  ImageInfo[0].LowestSupportedImageVersion = FmpSamplePrivate->EsrtInfo.LowestSupportedFwVersion;
  ImageInfo[0].LastAttemptVersion          = FmpSamplePrivate->EsrtInfo.LastAttemptVersion;
  ImageInfo[0].LastAttemptStatus           = FmpSamplePrivate->EsrtInfo.LastAttemptStatus;
  ImageInfo[0].HardwareInstance            = 0;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI FmpGetImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  OUT  VOID                         *Image,
  IN  OUT  UINTN                        *ImageSize
  )
{
  FMP_SAMPLE_PRIVATE_DATA *FmpSamplePrivate;

  if (Image == NULL || ImageSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FmpSamplePrivate = FMP_SAMPLE_PRIVATE_DATA_FROM_FMP(This);

  if (ImageIndex == 0 || ImageIndex > FmpSamplePrivate->DescriptorCount || ImageSize == NULL || Image == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*ImageSize < sizeof(FmpSamplePrivate->UpdateData)) {
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem(Image, &FmpSamplePrivate->UpdateData, sizeof(FmpSamplePrivate->UpdateData));
  *ImageSize  = sizeof(FmpSamplePrivate->UpdateData);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI FmpSetImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL                 *This,
  IN  UINT8                                            ImageIndex,
  IN  CONST VOID                                       *Image,
  IN  UINTN                                            ImageSize,
  IN  CONST VOID                                       *VendorCode,
  IN  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS    Progress,
  OUT CHAR16                                           **AbortReason
  )
{
  EFI_STATUS              Status;
  FMP_SAMPLE_PRIVATE_DATA *FmpSamplePrivate;

  if (Image == NULL || AbortReason == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  FmpSamplePrivate = FMP_SAMPLE_PRIVATE_DATA_FROM_FMP(This);
  *AbortReason     = NULL;

  if (ImageIndex == 0 || ImageIndex > FmpSamplePrivate->DescriptorCount || ImageSize != sizeof(FmpSamplePrivate->UpdateData) || Image == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Update Version in NV after successful flash image update . 
  //
  Status = gRT->SetVariable(
                    L"UpdateData",
                    &gFMPSampleUpdateImageInfoGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(FmpSamplePrivate->UpdateData),
                    (FMP_SAMPLE_UDPATE_DATA *)Image
                    );

  FmpSamplePrivate->EsrtInfo.FwVersion = ((FMP_SAMPLE_UDPATE_DATA *)Image)->Version;
  FmpSamplePrivate->EsrtInfo.LastAttemptVersion = ((FMP_SAMPLE_UDPATE_DATA *)Image)->Version;
  if (EFI_ERROR(Status)) {
    FmpSamplePrivate->EsrtInfo.LastAttemptStatus = LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL;
  } else {
    FmpSamplePrivate->EsrtInfo.LastAttemptStatus = LAST_ATTEMPT_STATUS_SUCCESS;
  }

  Status = gRT->SetVariable(
                  L"FmpInfo",
                  &gFMPSampleUpdateImageInfoGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(FmpSamplePrivate->EsrtInfo),
                  &FmpSamplePrivate->EsrtInfo
                  );

  return Status;
}

EFI_STATUS
EFIAPI FmpCheckImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  CONST VOID                        *Image,
  IN  UINTN                             ImageSize,
  OUT UINT32                            *ImageUpdatable
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI FmpGetPackageInfo(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL *This,
  OUT UINT32                           *PackageVersion,
  OUT CHAR16                           **PackageVersionName,
  OUT UINT32                           *PackageVersionNameMaxLen,
  OUT UINT64                           *AttributesSupported,
  OUT UINT64                           *AttributesSetting
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI FmpSetPackageInfo(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL   *This,
  IN  CONST VOID                         *Image,
  IN  UINTN                              ImageSize,
  IN  CONST VOID                         *VendorCode,
  IN  UINT32                             PackageVersion,
  IN  CONST CHAR16                       *PackageVersionName
  )
{
  return EFI_UNSUPPORTED;
}

/*++
  
  Routine Description:
  
     Initialize FmpSampleDriver private data structure.
  
  Arguments:
  

     FmpSamplePrivate     - private data structure to be initialized.
  
  Returns:

  --*/
EFI_STATUS
InitializePrivateData(
  IN FMP_SAMPLE_PRIVATE_DATA *FmpSamplePrivate
  )
{
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;
  UINTN         DataSize;

  //
  // Get current Boot Mode
  //
  BootMode = GetBootModeHob ();
  
  //
  // The NV Variable is not available in RECOVERY Mode
  //
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return EFI_UNSUPPORTED;
  }

  FmpSamplePrivate->Signature       = FMP_SAMPLE_PRIVATE_DATA_SIGNATURE;
  FmpSamplePrivate->Handle          = NULL;
  FmpSamplePrivate->DescriptorCount = 1;
  CopyMem(&FmpSamplePrivate->Fmp, &mFirmwareManagementProtocol, sizeof(EFI_FIRMWARE_MANAGEMENT_PROTOCOL));

  DataSize = sizeof(FmpSamplePrivate->EsrtInfo);
  Status = gRT->GetVariable(
                  L"FmpInfo",
                  &gFMPSampleUpdateImageInfoGuid,
                  NULL, 
                  &DataSize,
                  &FmpSamplePrivate->EsrtInfo
                  );
  
  if (EFI_ERROR(Status)) {
    FmpSamplePrivate->EsrtInfo.CapsuleFlags             = 0;
    FmpSamplePrivate->EsrtInfo.FwClass                  = gFMPSampleUpdateImageInfoGuid;
    FmpSamplePrivate->EsrtInfo.FwType                   = ESRT_FW_TYPE_DEVICEFIRMWARE;
    FmpSamplePrivate->EsrtInfo.FwVersion                = CURRENT_FIRMWARE_VERSION;
    FmpSamplePrivate->EsrtInfo.LastAttemptStatus        = LAST_ATTEMPT_STATUS_SUCCESS;
    FmpSamplePrivate->EsrtInfo.LastAttemptVersion       = 0;
    FmpSamplePrivate->EsrtInfo.LowestSupportedFwVersion = LOWEST_SUPPORTED_FIRMWARE_VERSION;
    Status = gRT->SetVariable(
                    L"FmpInfo",
                    &gFMPSampleUpdateImageInfoGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(FmpSamplePrivate->EsrtInfo),
                    &FmpSamplePrivate->EsrtInfo
                    );

  } else if (DataSize != sizeof(FmpSamplePrivate->EsrtInfo)) {
    Status = EFI_INVALID_PARAMETER;
  }

  DataSize = sizeof(FmpSamplePrivate->UpdateData);
  Status = gRT->GetVariable(
                   L"UpdateData",
                   &gFMPSampleUpdateImageInfoGuid,
                   NULL,
                   &DataSize,
                   &FmpSamplePrivate->UpdateData
                   );

  if (EFI_ERROR(Status)) {
    FmpSamplePrivate->UpdateData.Version = 0;
    FmpSamplePrivate->UpdateData.Data    = 0;

    Status = gRT->SetVariable(
                    L"UpdateData",
                    &gFMPSampleUpdateImageInfoGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(FmpSamplePrivate->UpdateData),
                    &FmpSamplePrivate->UpdateData
                    );
  }
  return Status;
}

