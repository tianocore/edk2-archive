/** @file
  This file provides services for Pei SA policy default initialization.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSaPolicyLibrary.h"

//
// @todo: Remove the duplicate definition of the GUIDs and make it a location function.
// Because mBxtSaIpBlocks' initializer must be a constant, we need to define those GUID for it.
//
#define SA_MISC_CONFIG_GUID             { 0xc5c9145f, 0x61fb, 0x4abe, { 0x88, 0x0a, 0xf2, 0x56, 0x89, 0x9f, 0x40, 0xb0}}
#define GRAPHICS_CONFIG_GUID            { 0x0319c56b, 0xc43a, 0x42f1, { 0x80, 0xbe, 0xca, 0x5b, 0xd1, 0xd5, 0xc9, 0x28}}
#define MEMORY_CONFIG_GUID              { 0x26cf084c, 0xc9db, 0x41bb, { 0x92, 0xc6, 0xd1, 0x97, 0xb8, 0xa1, 0xe4, 0xbf}}
#define IPU_CONFIG_GUID                 { 0x67eeefd0, 0x9e42, 0x48c8, { 0xbd, 0xab, 0xfd, 0x0d, 0x23, 0x69, 0x88, 0x0b}}
#define SA_PRE_MEM_CONFIG_GUID          { 0x7200eef0, 0xbe7f, 0x4061, { 0x93, 0xe3, 0x3c, 0xd0, 0x36, 0x7c, 0xe1, 0x51}}
#if (ENBDT_PF_ENABLE == 1)
#define HYBRID_GRAPHICS_CONFIG_GUID     { 0x0b7e694d, 0xb909, 0x4097, { 0x9c, 0x03, 0x5e, 0x72, 0x84, 0x89, 0xf7, 0x09}}
#endif

GLOBAL_REMOVE_IF_UNREFERENCED CONFIG_BLOCK_HEADER  mBxtSaIpBlocks[] = {
  // Block GUID                  Block Size,                          Revision
  {SA_MISC_CONFIG_GUID,          sizeof (SA_MISC_CONFIG),              SA_MISC_CONFIG_REVISION,           {0, 0, 0}},
  {GRAPHICS_CONFIG_GUID,         sizeof (GRAPHICS_CONFIG),             GRAPHICS_CONFIG_REVISION,          {0, 0, 0}},
  {IPU_CONFIG_GUID,              sizeof (IPU_CONFIG),                  IPU_CONFIG_REVISION,               {0, 0, 0}},
#if (ENBDT_PF_ENABLE == 1)
  {HYBRID_GRAPHICS_CONFIG_GUID,  sizeof (HYBRID_GRAPHICS_CONFIG),      HYBRID_GRAPHICS_CONFIG_REVISION,   {0, 0, 0}},
#endif
  {MEMORY_CONFIG_GUID,           sizeof (MEMORY_CONFIGURATION),        MEMORY_CONFIG_REVISION,            {0, 0, 0}}
};
GLOBAL_REMOVE_IF_UNREFERENCED CONFIG_BLOCK_HEADER  mBxtSaIpPreMemBlocks[] = {
  // Block GUID                 Block Size,                           Revision
  {SA_PRE_MEM_CONFIG_GUID,       sizeof (SA_PRE_MEM_CONFIG),           SA_PRE_MEM_CONFIG_REVISION,        { 0, 0, 0}}
};

//
// Function call to Load defaults for Individial IP Blocks
//
EFI_STATUS
EFIAPI
LoadSaMiscDefault (
  IN   VOID          *ConfigBlockPointer
  )
{
  SA_MISC_CONFIG            *MiscConfig;

  MiscConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "MiscConfig->Header.Guid = %g\n", MiscConfig->Header.Guid));
  DEBUG ((DEBUG_INFO, "MiscConfig->Header.Size = 0x%x\n", MiscConfig->Header.Size));

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LoadIpuDefault (
  IN VOID          *ConfigBlockPointer
  )
{
  IPU_CONFIG        *IpuPolicy;

  IpuPolicy = ConfigBlockPointer;
  DEBUG ((DEBUG_INFO, "IpuPolicy->Header.Guid = %g\n", IpuPolicy->Header.Guid));
  DEBUG ((DEBUG_INFO, "IpuPolicy->Header.Size = 0x%x\n", IpuPolicy->Header.Size));

  IpuPolicy->SaIpuEnable = 1;
  IpuPolicy->IpuMmAdr = 0xCF000000;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LoadGraphicsDefault (
  IN VOID    *ConfigBlockPointer
  )
{
  GRAPHICS_CONFIG                *GtConfig;

  GtConfig = ConfigBlockPointer;

  //
  // Initialize the Graphics configuration
  //
  GtConfig->GmAdr               = 0xA0000000;
  GtConfig->GttMmAdr            = 0xBF000000;
  GtConfig->EnableRenderStandby = 1;
  GtConfig->PavpEnable          = 1;
  GtConfig->PmSupport           = 1;
  GtConfig->PavpPr3             = 1;
  GtConfig->PeiGraphicsPeimInit = 0;
  //
  // Initialize the CdClock to 675 Mhz
  //
  GtConfig->CdClock             = 3;

  return EFI_SUCCESS;
}


#if (ENBDT_PF_ENABLE == 1)
EFI_STATUS
EFIAPI
LoadHybridGraphicsDefault (
  IN VOID    *ConfigBlockPointer
  )
{
  HYBRID_GRAPHICS_CONFIG          *HgConfig;

  HgConfig = ConfigBlockPointer;

  //
  // Initialize the Hybrid Graphics configuration
  //
  HgConfig->HgEnabled              = 0x0;
  HgConfig->HgDelayAfterPwrEn      = 300;
  HgConfig->HgDelayAfterHoldReset  = 100;

  return EFI_SUCCESS;
}
#endif


EFI_STATUS
EFIAPI
LoadSaPreMemDefault (
  IN VOID    *ConfigBlockPointer
  )
{
  SA_PRE_MEM_CONFIG              *SaPreMemConfig;

  SaPreMemConfig = ConfigBlockPointer;

  //
  // Initialize the Graphics configuration
  //
  SaPreMemConfig->GttSize             = 3;
  SaPreMemConfig->IgdDvmt50PreAlloc   = 2;
  SaPreMemConfig->InternalGraphics    = 1;
  SaPreMemConfig->PrimaryDisplay      = 0;
  SaPreMemConfig->ApertureSize        = 2;

  return EFI_SUCCESS;
}


/**
  Initialize default settings for each SA Config block.

  @param[in] ConfigBlockPointer        The buffer pointer that will be initialized as specific config block.
  @param[in] BlockId                   Request to initialize defaults of specified config block by given Block ID.

  @retval    EFI_SUCCESS               The given buffer has contained the defaults of requested config block.
  @retval    EFI_NOT_FOUND             Block ID is not defined so no default Config block will be initialized.

**/
EFI_STATUS
EFIAPI
LoadConfigBlockDefault (
  IN   VOID          *ConfigBlockPointer,
  IN   EFI_GUID      BlockGuid
  )
{
  if (CompareGuid (&BlockGuid, &gSaMiscConfigGuid)) {
    LoadSaMiscDefault (ConfigBlockPointer);
  } else {
    if (CompareGuid (&BlockGuid, &gGraphicsConfigGuid)) {
      LoadGraphicsDefault (ConfigBlockPointer);
    } else {
      if (CompareGuid (&BlockGuid, &gIpuConfigGuid)) {
        LoadIpuDefault (ConfigBlockPointer);
      } else {
        if (CompareGuid (&BlockGuid, &gSaPreMemConfigGuid)) {
          LoadSaPreMemDefault (ConfigBlockPointer);
        }
#if (ENBDT_PF_ENABLE == 1)
        else {
          if (CompareGuid (&BlockGuid, &gHybridGraphicsConfigGuid)) {
            LoadHybridGraphicsDefault (ConfigBlockPointer);
        }
#endif
          else {
            return EFI_NOT_FOUND;
          }


#if (ENBDT_PF_ENABLE == 1)
     }
#endif
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Creates the Pre-Mem Config Blocks for SA Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] SiSaPolicyPpi           The pointer to get SI/SA Policy PPI instance.

  @retval         EFI_SUCCESS             The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SaCreatePreMemConfigBlocks(
  IN OUT  SI_SA_POLICY_PPI          **SiSaPolicyPpi
  )
{
  UINT32                            TotalBlockSize;
  UINT16                            TotalBlockCount;
  UINT16                            BlockCount;
  VOID                              *ConfigBlockPointer;
  EFI_STATUS                        Status;
  SI_SA_POLICY_PPI                  *SaInitPolicy;
  UINT32                            ConfigBlockHdrSize;
  UINT32                            RequiredSize;

  SaInitPolicy = NULL;
  TotalBlockCount = sizeof (mBxtSaIpPreMemBlocks) / sizeof (CONFIG_BLOCK_HEADER);
  DEBUG ((DEBUG_INFO, "TotalBlockCount = 0x%x\n", TotalBlockCount));

  TotalBlockSize = 0;

  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    TotalBlockSize += (UINT32) mBxtSaIpPreMemBlocks[BlockCount].Size;
    DEBUG ((DEBUG_INFO, "TotalBlockSize after adding  Block[0x%x]= 0x%x\n", BlockCount, TotalBlockSize));
  }
  DEBUG ((DEBUG_INFO, "TotalBlockSize Final = 0x%x\n", TotalBlockSize));

  ConfigBlockHdrSize = GetSizeOfConfigBlockTableHeaders ((UINT16) TotalBlockCount);

  RequiredSize = ConfigBlockHdrSize + TotalBlockSize;

  Status = CreateConfigBlockTable ((VOID *) &SaInitPolicy, TotalBlockCount, RequiredSize);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize Policy Revision
  //
  SaInitPolicy->TableHeader.Header.Revision = SA_POLICY_PPI_REVISION;
  //
  // Initialize ConfigBlockPointer to NULL
  //
  ConfigBlockPointer = NULL;
  //
  // Loop to identify each config block from mBxtSaIpPreMemBlocks[] Table and add each of them
  //
  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    ConfigBlockPointer = (VOID *) &mBxtSaIpPreMemBlocks[BlockCount];
    Status = AddConfigBlock ((VOID *) SaInitPolicy, (VOID *) &ConfigBlockPointer);
    ASSERT_EFI_ERROR (Status);
    LoadConfigBlockDefault ((VOID *) ConfigBlockPointer, mBxtSaIpPreMemBlocks[BlockCount].Guid);
  }
  //
  // Assignment for returning SaInitPolicy config block base address
  //
  *SiSaPolicyPpi = SaInitPolicy;

  return EFI_SUCCESS;
}


/**
  Creates the Config Blocks for SA Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] SiSaPolicyPpi           The pointer to get SI/SA Policy PPI instance.

  @retval         EFI_SUCCESS             The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
CreateConfigBlocks (
  IN OUT  SI_SA_POLICY_PPI          **SiSaPolicyPpi
  )
{
  UINT32                            TotalBlockSize;
  UINT16                            TotalBlockCount;
  UINT16                            BlockCount;
  VOID                              *ConfigBlockPointer;
  EFI_STATUS                        Status;
  SI_SA_POLICY_PPI                  *SaInitPolicy;
  UINT32                            ConfigBlockHdrSize;
  UINT32                            RequiredSize;

  SaInitPolicy = NULL;
  TotalBlockCount = sizeof (mBxtSaIpBlocks) / sizeof (CONFIG_BLOCK_HEADER);
  DEBUG ((DEBUG_INFO, "TotalBlockCount = 0x%x\n", TotalBlockCount));

  TotalBlockSize = 0;

  for (BlockCount = 0 ; BlockCount < TotalBlockCount; BlockCount++) {
    TotalBlockSize += (UINT32 )mBxtSaIpBlocks[BlockCount].Size;
    DEBUG ((DEBUG_INFO, "TotalBlockSize after adding  Block[0x%x]= 0x%x\n", BlockCount, TotalBlockSize));
  }
  DEBUG ((DEBUG_INFO, "TotalBlockSize Final = 0x%x\n", TotalBlockSize));

  ConfigBlockHdrSize = GetSizeOfConfigBlockTableHeaders ((UINT16) TotalBlockCount);

  RequiredSize = ConfigBlockHdrSize + TotalBlockSize;

  Status = CreateConfigBlockTable ((VOID *) &SaInitPolicy, TotalBlockCount, RequiredSize);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize Policy Revision
  //
  SaInitPolicy->TableHeader.Header.Revision = SA_POLICY_PPI_REVISION;
  //
  // Initialize ConfigBlockPointer to NULL
  //
  ConfigBlockPointer = NULL;
  //
  // Loop to identify each config block from mBxtSaIpBlocks[] Table and add each of them
  //
  for (BlockCount = 0 ; BlockCount < TotalBlockCount; BlockCount++) {
    ConfigBlockPointer = (VOID *) &mBxtSaIpBlocks[BlockCount];
    Status = AddConfigBlock ((VOID *) SaInitPolicy, (VOID *) &ConfigBlockPointer);
    ASSERT_EFI_ERROR (Status);
    LoadConfigBlockDefault ((VOID *) ConfigBlockPointer, mBxtSaIpBlocks[BlockCount].Guid);
  }
  //
  // Assignment for returning SaInitPolicy config block base address
  //
  *SiSaPolicyPpi = SaInitPolicy;

  return EFI_SUCCESS;
}


/**
  Install PPI SiSaPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiSaPolicyPpi              Pointer of policy structure.

  @retval    EFI_SUCCESS                The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES       Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SiSaInstallPolicyPpi (
  IN  SI_SA_POLICY_PPI           *SiSaPolicyPpi
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *SiSaInstallPolicyPpiDesc;

  SiSaInstallPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (SiSaInstallPolicyPpiDesc != NULL);
  if (SiSaInstallPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the PPI
  //
  SiSaInstallPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SiSaInstallPolicyPpiDesc->Guid   = &gSiSaPolicyPpiGuid;
  SiSaInstallPolicyPpiDesc->Ppi    = SiSaPolicyPpi;

  //
  // Install PEI SA Policy PPI
  //
  Status = PeiServicesInstallPpi (SiSaInstallPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  SaInstallPreMemPolicyPpi installs Sa Pre Mem PolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SaPreMemPolicyPpi             The pointer to SA PREMEM Policy PPI instance.

  @retval    EFI_SUCCESS                   The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SaInstallPreMemPolicyPpi (
  IN  SI_SA_POLICY_PPI             *SaPolicyPpi
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_PPI_DESCRIPTOR                *SaPreMemPolicyPpiDesc;

  SaPreMemPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (SaPreMemPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  SaPreMemPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SaPreMemPolicyPpiDesc->Guid  = &gSiSaPreMemPolicyPpiGuid;
  SaPreMemPolicyPpiDesc->Ppi   = SaPolicyPpi;

  //
  // Install PREMEM Policy PPI
  //
  Status = PeiServicesInstallPpi (SaPreMemPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

