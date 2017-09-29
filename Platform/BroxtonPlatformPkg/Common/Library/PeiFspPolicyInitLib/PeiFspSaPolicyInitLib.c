/** @file
  Implementation of Fsp SA Policy Initialization.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SaMiscConfig.h>
#include <Ppi/MemoryConfig.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmbusLib.h>
#include <Library/MmPciLib.h>
#include <Library/ConfigBlockLib.h>
#include <IndustryStandard/Pci.h>
#include <Ppi/DramPolicyPpi.h>
#include <ScAccess.h>
#include <Ppi/FirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>
#include <Core/Pei/PeiMain.h>


/**
  PeiGetSectionFromFv finds the file in FV and gets file Address and Size

  @param[in]  NameGuid               File GUID
  @param[out] Address                Pointer to the File Address
  @param[out] Size                   Pointer to File Size

  @retval     EFI_SUCCESS            Successfull in reading the section from FV

**/
EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32               *Size
  )
{
  EFI_STATUS                           Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI          *FvPpi;
  EFI_FV_FILE_INFO                     FvFileInfo;
  PEI_CORE_INSTANCE                    *PrivateData;
  UINTN                                CurrentFv;
  PEI_CORE_FV_HANDLE                   *CoreFvHandle;
  EFI_PEI_FILE_HANDLE                  VbtFileHandle;
  EFI_GUID                             *VbtGuid;
  EFI_COMMON_SECTION_HEADER            *Section;
  CONST EFI_PEI_SERVICES               **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS (PeiServices);

  Status = PeiServicesLocatePpi (
             &gEfiFirmwareFileSystem2Guid,
             0,
             NULL,
             (VOID **) &FvPpi
             );
  ASSERT_EFI_ERROR (Status);

  CurrentFv = PrivateData->CurrentPeimFvCount;
  CoreFvHandle = &(PrivateData->Fv[CurrentFv]);

  Status = FvPpi->FindFileByName (FvPpi, &NameGuid, &CoreFvHandle->FvHandle, &VbtFileHandle);
  if (!EFI_ERROR (Status) && VbtFileHandle != NULL) {

    DEBUG ((DEBUG_INFO, "Find SectionByType \n"));

    Status = FvPpi->FindSectionByType (FvPpi, EFI_SECTION_RAW, VbtFileHandle, (VOID **) &VbtGuid);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "GetFileInfo \n"));
      Status = FvPpi->GetFileInfo (FvPpi, VbtFileHandle, &FvFileInfo);
      Section = (EFI_COMMON_SECTION_HEADER *) FvFileInfo.Buffer;
      if (IS_SECTION2 (Section)) {
        ASSERT (SECTION2_SIZE (Section) > 0x00FFFFFF);
        *Size = SECTION2_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
        *Address = ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER2));
      } else {
        *Size = SECTION_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER);
        *Address = ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER));
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Performs FSP SA PEI Policy initialization in pre-memory.

  @param[in, out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  SYSTEM_CONFIGURATION             *SystemConfiguration;
  UINTN                            VariableSize = 0;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  EFI_STATUS                       Status;

  DEBUG ((DEBUG_INFO, "Wrapper-PeiFspSaPolicyInitPreMem - Start\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  if (Status == EFI_SUCCESS) {
    FspmUpd->FspmConfig.IgdDvmt50PreAlloc = SystemConfiguration->IgdDvmt50PreAlloc;
    FspmUpd->FspmConfig.IgdApertureSize = SystemConfiguration->IgdApertureSize;
    FspmUpd->FspmConfig.GttSize = SystemConfiguration->GTTSize;
    FspmUpd->FspmConfig.Igd = SystemConfiguration->Igd;
    FspmUpd->FspmConfig.PrimaryVideoAdaptor = SystemConfiguration->PrimaryVideoAdaptor;

    FspmUpd->FspmConfig.FwTraceEn          = 1;
    FspmUpd->FspmConfig.FwTraceDestination = 4;
    FspmUpd->FspmConfig.RecoverDump        = 0;
    FspmUpd->FspmConfig.Msc0Size           = 0;
    FspmUpd->FspmConfig.Msc0Wrap           = 1;
    FspmUpd->FspmConfig.Msc1Size           = 0;
    FspmUpd->FspmConfig.Msc1Wrap           = 1;
    FspmUpd->FspmConfig.PtiMode            = 1;
    FspmUpd->FspmConfig.PtiTraining        = 0;
    FspmUpd->FspmConfig.PtiSpeed           = 2;
    FspmUpd->FspmConfig.PunitMlvl          = 1;
    FspmUpd->FspmConfig.PmcMlvl            = 1;
    FspmUpd->FspmConfig.SwTraceEn          = 0;
  }

  DEBUG ((DEBUG_INFO, "Wrapper-PeiFspSaPolicyInitPreMem - End\n"));
  return EFI_SUCCESS;
}


/**
  Performs FSP SA PEI Policy initialization.

  @param[in, out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.

**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyInit (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  EFI_STATUS                       Status;
  VOID                             *Buffer;
  UINT32                           Size;
  EFI_GUID                         PeiLogoGuid = gPeiLogoGuid;
  EFI_GUID                         PeiVbtGuid;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SYSTEM_CONFIGURATION             *SystemConfiguration;
  UINTN                            VariableSize = 0;
  EFI_BOOT_MODE                    BootMode;



  Status = PeiServicesGetBootMode (&BootMode);

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "Update North Cluster FspsUpd from setup option...\n"));

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  if (Status == EFI_SUCCESS) {
    FspsUpd->FspsConfig.PmSupport = SystemConfiguration->PmSupport;
    FspsUpd->FspsConfig.EnableRenderStandby = SystemConfiguration->EnableRenderStandby;
    FspsUpd->FspsConfig.CdClock = SystemConfiguration->CdClock;
    FspsUpd->FspsConfig.PavpEnable = SystemConfiguration->PavpEnable;
    FspsUpd->FspsConfig.PeiGraphicsPeimInit = SystemConfiguration->PeiGraphicsPeimInit;
    FspsUpd->FspsConfig.IpuEn = SystemConfiguration->IpuEn;
    FspsUpd->FspsConfig.IpuAcpiMode = SystemConfiguration->IpuAcpiMode;
  }

  //
  // Update VbtGuid.
  //
  CopyMem (&PeiVbtGuid, PcdGetPtr (PcdBoardVbtFileGuid), sizeof (EFI_GUID));

  //
  // Update UPD:LogoPtr
  //
  PeiGetSectionFromFv (PeiVbtGuid, &Buffer, &Size);
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not locate VBT"));
  }

  if (BootMode == BOOT_ON_S3_RESUME) {
    FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32) NULL;
  } else {
    FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32) Buffer;
  }
  DEBUG ((DEBUG_INFO, "VbtPtr from PeiGetSectionFromFv is 0x%x\n", FspsUpd->FspsConfig.GraphicsConfigPtr));
  DEBUG ((DEBUG_INFO, "VbtSize from PeiGetSectionFromFv is 0x%x\n", Size));

  PeiGetSectionFromFv (PeiLogoGuid, &Buffer, &Size);
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not locate Logo"));
  }
  FspsUpd->FspsConfig.LogoPtr  = (UINT32) Buffer;
  FspsUpd->FspsConfig.LogoSize          = Size;
  DEBUG ((DEBUG_INFO, "LogoPtr from PeiFspSaPolicyInit PeiGetSectionFromFv is 0x%x\n", Buffer));
  DEBUG ((DEBUG_INFO, "LogoSize from PeiFspSaPolicyInit PeiGetSectionFromFv is 0x%x\n", Size));

  return EFI_SUCCESS;
}

