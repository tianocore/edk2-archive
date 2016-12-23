/** @file
  Do Platform Stage System Agent initialization.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiSaPolicyUpdate.h>
#include <Library/DebugLib.h>
#include <Library/PeiSaPolicyLib.h>
#include <Ppi/FirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>
#include <Core/Pei/PeiMain.h>
#include <Library/SteppingLib.h>


/**
  UpdatePeiSaPolicy performs SA PEI Policy initialzation

  @param[in, out] SiSaPolicyPpi            SI_SA_POLICY PPI

  @retval         EFI_SUCCESS              The policy is installed and initialized.

**/
EFI_STATUS
UpdatePeiSaPolicy (
  IN OUT   SI_SA_POLICY_PPI      *SiSaPolicyPpi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VariableSize;
  SA_MISC_CONFIG                  *MiscConfig = NULL;
  GRAPHICS_CONFIG                 *GtConfig = NULL;
  IPU_CONFIG                      *IpuPolicy = NULL;
  VOID*                           Buffer;
  UINT32                          Size;
  EFI_GUID                        PeiLogoGuid                = { 0x7BB28B99, 0x61BB, 0x11D5, 0x9A, 0x5D, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D };
  EFI_GUID                        TianmaVbtGuid              = { 0xE08CA6D5, 0x8D02, 0x43ae, 0xAB, 0xB1, 0x95, 0x2C, 0xC7, 0x87, 0xC9, 0x33 };
  EFI_GUID                        TrulyFabBVbtTypeCGuid      = { 0xDBADD769, 0xE86A, 0x4819, 0x81, 0x20, 0xE9, 0x91, 0x79, 0x2C, 0x0B, 0xC1 };
  EFI_GUID                        TrulyFabBVbtGuid           = { 0x1a03cf12, 0xf54b, 0x4227, 0xa7, 0xa4, 0x6c, 0x97, 0x11, 0x3f, 0xac, 0x71 };
  EFI_GUID                        TrulyFabBCMVbtGuid         = { 0x1fc29068, 0x17cf, 0x4613, 0x98, 0xd, 0x17, 0x2f, 0x97, 0xe7, 0xfa, 0xe5 };
  EFI_GUID                        TrulyFabBCMTypeCVbtGuid    = { 0x2246d69f, 0xd9a6, 0x4f80, 0x90, 0x38, 0x78, 0xc5, 0xcc, 0x74, 0x1d, 0xdf };
  EFI_GUID                        TrulyFabBPr1TypeCVbtGuid   = { 0x190c66ce, 0x475e, 0x4c5e, 0xb7, 0xd0, 0x2f, 0x67, 0x72, 0xba, 0x71, 0x70 };
  EFI_GUID                        TrulyFabBPr1CMTypeCVbtGuid = { 0xc1b31ace, 0x65db, 0x46d6, 0x9c, 0x7e, 0xa8, 0x2c, 0xce, 0x78, 0x45, 0x38 };
  VBT_INFO                        VbtInfo;
  SC_POLICY_PPI                   *ScPolicyPpi;
  SC_VTD_CONFIG                   *VtdConfig;
  BXT_SERIES                      SocSeries;
  BXT_STEPPING                    SocStepping;

  DEBUG ((DEBUG_INFO, "Entering Get Config Block function call from UpdatePeiSaPolicy\n"));

  Status = GetConfigBlock ((VOID *) SiSaPolicyPpi, &gSaMiscConfigGuid , (VOID *) &MiscConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiSaPolicyPpi, &gGraphicsConfigGuid, (VOID *) &GtConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) SiSaPolicyPpi, &gIpuConfigGuid, (VOID *) &IpuPolicy);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR ( Status);

  //
  // Locate SC Policy
  //
  Status = PeiServicesLocatePpi (
             &gScPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &ScPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Get System configuration variables
  //
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );
  if (!EFI_ERROR (Status)) {
    //
    // Get the Platform Configuration from SetupData
    //
    GtConfig->GttMmAdr = GTTMM_BASE_ADDRESS;
    GtConfig->GmAdr = GMADR_BASE_ADDRESS;
    GtConfig->PeiGraphicsPeimInit = SystemConfiguration.PeiGraphicsPeimInit;
    GtConfig->PmSupport = SystemConfiguration.PmSupport;
    GtConfig->EnableRenderStandby = SystemConfiguration.EnableRenderStandby;
    GtConfig->CdClock = SystemConfiguration.CdClock;
    GtConfig->PavpEnable = SystemConfiguration.PavpEnable;

    GtConfig->ForceWake = 0;
    GtConfig->PavpLock = 1;
    GtConfig->GraphicsFreqModify = 0;
    GtConfig->GraphicsFreqReq = 0;
    GtConfig->GraphicsVideoFreq = 0;
    GtConfig->PmLock = 1;
    GtConfig->DopClockGating = 1;
    GtConfig->UnsolicitedAttackOverride = 0;
    GtConfig->WOPCMSupport = 1;
    GtConfig->WOPCMSize = 0;
    GtConfig->PowerGating = 0;
    GtConfig->UnitLevelClockGating = 1;

    MiscConfig->FastBoot = 1;
    MiscConfig->DynSR = 1;

    IpuPolicy->SaIpuEnable = SystemConfiguration.IpuEn;

    SocSeries   = GetBxtSeries ();
    SocStepping = BxtStepping ();
    if ((SocSeries == BxtP && SocStepping <= BxtPB2) || (SocSeries != BxtP && SocStepping < BxtC0)) {
      if (IpuPolicy->SaIpuEnble == 1) {
        VtdConfig->VtdEnable = 0;
      }
    }

    IpuPolicy->IpuAcpiMode = SystemConfiguration.IpuAcpiMode;
    IpuPolicy->IpuMmAdr = IPUMM_BASE_ADDRESS;
  }

  PeiGetSectionFromFv (PeiLogoGuid, &Buffer, &Size);
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not locate Pei Logo"));
  }
  GtConfig->LogoPtr           = Buffer;
  GtConfig->LogoSize          = Size;
  DEBUG ((DEBUG_INFO, "LogoPtr from PeiGetSectionFromFv is 0x%x\n", Buffer));
  DEBUG ((DEBUG_INFO, "LogoSize from PeiGetSectionFromFv is 0x%x\n", Size));

  //
  // VBT selection may depend on SystemConfiguration->PanelSel value
  //
  PeiGetSectionFromFv (TianmaVbtGuid, &Buffer, &Size);

  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "Could not locate VBT"));
  }
  GtConfig->GraphicsConfigPtr = Buffer;

  //
  // Build the VBT data into HOB for DXE GOP
  //
  VbtInfo.VbtAddress = (EFI_PHYSICAL_ADDRESS) Buffer;
  VbtInfo.VbtSize = Size;
  DEBUG ((DEBUG_INFO, "VbtInfo VbtAddress is 0x%x\n", Buffer));
  DEBUG ((DEBUG_INFO, "VbtInfo VbtSize is 0x%x\n", Size));

  BuildGuidDataHob (
    &gVbtInfoGuid,
    &VbtInfo,
    sizeof (VbtInfo)
    );

  return EFI_SUCCESS;
}


/**
  PeiGetSectionFromFv finds the file in FV and gets file Address and Size

  @param[in]  NameGuid                File GUID
  @param[out] Address                 Pointer to the File Address
  @param[out] Size                    Pointer to File Size

  @retval     EFI_SUCCESS             Successfull in reading the section from FV

**/
EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32                *Size
  )
{
  EFI_STATUS  Status;
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

