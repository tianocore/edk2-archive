/** @file
  This file is PeiSiPolicyLib library creates default settings of RC
  Policy and installs RC Policy PPI.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSiPolicyLibrary.h"

/**
  SiCreatePolicyDefaults creates the default setting of Silicon Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiPolicyPpi             The pointer to get Silicon Policy PPI instance

  @retval     EFI_SUCCESS             The policy default is initialized.
  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SiCreatePolicyDefaults (
  OUT  SI_POLICY_PPI **SiPolicyPpi
  )
{
  SI_POLICY_PPI    *SiPolicy;

  SiPolicy = (SI_POLICY_PPI *) AllocateZeroPool (sizeof (SI_POLICY_PPI));
  if (SiPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // General intialization
  //
  SiPolicy->Revision        = SI_POLICY_REVISION;

  //
  // PlatformData configuration
  //
  //
  // FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group
  // Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  // version information. The default value is type 221.
  //
  SiPolicy->FviSmbiosType   = 0xDD;

  //
  // Temporary Bus range for silicon initialization.
  //
  SiPolicy->TempPciBusMin   = 2;
  SiPolicy->TempPciBusMax   = 6;

  //
  // Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
  // Minimum size is 2MB bytes.
  //
  SiPolicy->TempMemBaseAddr = TEMP_MEM_BASE_ADDRESS;
  SiPolicy->TempMemSize     = TEMP_MEM_SIZE;

  //
  // Temporary IO Base Address for PCI devices to be used to initialize IO registers.
  // And size of temporary IO space.
  //
  SiPolicy->TempIoBaseAddr  = TEMP_IO_BASE_ADDRESS;
  SiPolicy->TempIoSize      = TEMP_IO_SIZE;

  *SiPolicyPpi = SiPolicy;

  return EFI_SUCCESS;
}


/**
  SiInstallPolicyPpi installs SiPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiPolicyPpi            The pointer to Silicon Policy PPI instance.

  @retval    EFI_SUCCESS            The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
SiInstallPolicyPpi (
  IN  SI_POLICY_PPI *SiPolicyPpi
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PPI_DESCRIPTOR  *SiPolicyPpiDesc;
  EFI_PEI_PPI_DESCRIPTOR  *OldSiPolicyPpiDesc;

  SiPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (SiPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  SiPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SiPolicyPpiDesc->Guid  = &gSiPolicyPpiGuid;
  SiPolicyPpiDesc->Ppi   = SiPolicyPpi;

  //
  // Print whole SI_POLICY_PPI and serial out.
  //
  SiPrintPolicyPpi (SiPolicyPpi);

  Status = PeiServicesLocatePpi (
             &gSiPolicyPpiGuid,
             0,
             &OldSiPolicyPpiDesc,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Locate Old gSiPolicyPpiGuid fail in Post-Memory\n"));
    Status = PeiServicesInstallPpi (SiPolicyPpiDesc);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Install gSiPolicyPpiGuid fail in Post-Memory\n"));
    }
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // ReInstall Sc Spi PPI
    //
    DEBUG ((DEBUG_INFO, "Re-Install gSiPolicyPpiGuid in Post-Memory, OldSiPolicyPpiDesc is %x, New is %x\n", OldSiPolicyPpiDesc, SiPolicyPpiDesc));
    Status = PeiServicesReInstallPpi (OldSiPolicyPpiDesc, SiPolicyPpiDesc);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Re-Install gSiPolicyPpiGuid fail in Post-Memory\n"));
    }
    ASSERT_EFI_ERROR (Status);
  }
  ASSERT_EFI_ERROR (Status);

  return Status;
}

