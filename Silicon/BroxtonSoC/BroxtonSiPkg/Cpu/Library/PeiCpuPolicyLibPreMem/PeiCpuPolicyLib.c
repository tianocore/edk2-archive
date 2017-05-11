/** @file
  This file is PeiCpuPolicy library.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiCpuPolicyLibrary.h"
#include <Library/PostCodeLib.h>

EFI_STATUS
EFIAPI
LoadCpuPreMemDefault (
  IN   VOID         *ConfigBlockPointer
  )
{


  return EFI_SUCCESS;
}


/**
  Initialize default settings for each CPU Config block

  @param[in]  ConfigBlockPointer            The buffer pointer that will be initialized as specific config block.
  @param[in]  BlockId                       Request to initialize defaults of specified config block by given Block ID.

  @retval     EFI_SUCCESS                   The given buffer has contained the defaults of requested config block.
  @retval     EFI_NOT_FOUND                 Block ID is not defined so no default Config block will be initialized.

**/
EFI_STATUS
EFIAPI
LoadCpuConfigBlockDefault (
  IN   VOID          *ConfigBlockPointer
  )
{
  CONFIG_BLOCK    *ConfigBlkPtr;

  ConfigBlkPtr = (CONFIG_BLOCK *) ConfigBlockPointer;
  DEBUG ((DEBUG_INFO, "load:GUID A=%g,GUIDB=%g\n", &(ConfigBlkPtr->Header.Guid), &gCpuConfigPreMemGuid));

  if (CompareGuid (&(ConfigBlkPtr->Header.Guid), &gCpuConfigPreMemGuid)) {
    LoadCpuPreMemDefault (ConfigBlockPointer);
  } else {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  CreateCpuConfigBlocks creates the default setting of PEI Cpu Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out]  SiCpuPolicyPpi                The pointer to get PEI Cpu Policy PPI instance.

  @retval      EFI_SUCCESS                   The policy default is initialized.
  @retval      EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
CreateCpuConfigBlocks (
  OUT  SI_CPU_POLICY_PPI      **SiCpuPolicyPpi
  )
{
  UINT32                      ConfigBlockTableTotalSize;
  UINT8                       TotalBlockCount;
  EFI_STATUS                  Status;
  UINT8                       BlockCount;
  VOID                        *ConfigBlockPointer;
  SI_CPU_POLICY_PPI           *SiCpuPolicy;
  CONFIG_BLOCK_HEADER         mBxtCpuIpBlocksPreMem [1];

  PostCode (0xC00);
  CopyMem (&mBxtCpuIpBlocksPreMem[0].Guid, &gCpuConfigPreMemGuid, sizeof (EFI_GUID));
  mBxtCpuIpBlocksPreMem[0].Size = sizeof (CPU_CONFIG_PREMEM);
  mBxtCpuIpBlocksPreMem[0].Revision = CPU_CONFIG_PREMEM_REVISION;

  TotalBlockCount = sizeof (mBxtCpuIpBlocksPreMem) / sizeof (CONFIG_BLOCK_HEADER);

  ConfigBlockTableTotalSize = sizeof (CONFIG_BLOCK_TABLE_HEADER) + TotalBlockCount * 4;
  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    ConfigBlockTableTotalSize += (UINT32) mBxtCpuIpBlocksPreMem[BlockCount].Size;
  }

  //
  // Allocate memory for the CPU Policy Ppi and Descriptor
  //
  DEBUG ((DEBUG_INFO, "CreateCpuConfigBlocksPreMem Start\n"));
  Status = CreateConfigBlockTable ((void **) &SiCpuPolicy, TotalBlockCount, ConfigBlockTableTotalSize);
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "CreateConfigBlockTable Done\n"));
  if (SiCpuPolicy == NULL) {
    ASSERT (SiCpuPolicy != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize ConfigBlockPointer to NULL
  //
  ConfigBlockPointer = NULL;

  //
  // Put IP_BLOCK_STRUCT and target structure into the right place in SI_CPU_POLICY_PPI
  //
  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    ConfigBlockPointer = (VOID *) &mBxtCpuIpBlocksPreMem[BlockCount];
    DEBUG ((DEBUG_INFO, "Addnew config block for Guid = %g \n", &(((CONFIG_BLOCK_HEADER *) ConfigBlockPointer)->Guid)));
    Status = AddConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SiCpuPolicy, (VOID *) &ConfigBlockPointer);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "Addnew config block for mBxtCpuIpBlocksPreMem[BlockCount].Guid = 0x%x \n",
      mBxtCpuIpBlocksPreMem[BlockCount].Guid));
    LoadCpuConfigBlockDefault ((VOID *) ConfigBlockPointer);
  }
  //
  // Assignment for returning CpuInitPolicy config block base address
  //
  *SiCpuPolicyPpi = SiCpuPolicy;
  PostCode (0xC2F);

  return EFI_SUCCESS;
}


/**
  CpuInstallPolicyPpi installs SiCpuPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in]  SiCpuPolicyPpi                The pointer to PEI Cpu Policy PPI instance.

  @retval     EFI_SUCCESS                   The policy is installed.
  @retval     EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
CpuInstallPolicyPpi (
  IN  SI_CPU_POLICY_PPI   *SiCpuPolicy
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *SiCpuPolicyPpiDesc;

  SiCpuPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (SiCpuPolicyPpiDesc == NULL) {
    ASSERT (SiCpuPolicyPpiDesc != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  SiCpuPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SiCpuPolicyPpiDesc->Guid  = &gSiCpuPolicyPpiGuid;
  SiCpuPolicyPpiDesc->Ppi   = SiCpuPolicy;

  //
  // Print whole SI_CPU_POLICY_PPI and serial out in PreMem.
  //
  CpuPrintPolicyPpi (SiCpuPolicy);

  //
  // Install PEI Cpu Policy PPI
  //
  Status = PeiServicesInstallPpi (SiCpuPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

