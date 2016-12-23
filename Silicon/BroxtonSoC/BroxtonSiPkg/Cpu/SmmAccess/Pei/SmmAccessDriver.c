/** @file
  This is the driver that publishes the SMM Access Protocol
  instance for System Agent.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmmAccessDriver.h"


/**
  This is the constructor for the SMM Access Ppi

  @param[in] FileHandle         FileHandle.
  @param[in] PeiServices        General purpose services available to every PEIM.

  @retval    EFI_SUCCESS        Protocol successfully started and installed
  @retval    EFI_UNSUPPORTED    Protocol can't be started

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *DescriptorBlock;
  SMM_ACCESS_PRIVATE_DATA         *SmmAccessPrivate;
  EFI_PEI_PPI_DESCRIPTOR          *PpiList;
  EFI_PEI_HOB_POINTERS            HobList;

  //
  // Initialize private data
  //
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (*SmmAccessPrivate),
                             (VOID **) &SmmAccessPrivate
                             );
  ASSERT_EFI_ERROR (Status);

  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (*PpiList),
                             (VOID **) &PpiList
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Build SMM related information
  //
  SmmAccessPrivate->Signature          = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;

  //
  // Get Hob list
  //
  HobList.Raw = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  ASSERT(HobList.Raw != NULL);
  DescriptorBlock = GET_GUID_HOB_DATA (HobList.Raw);
  if (DescriptorBlock == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  //
  // Use the hob to publish SMRAM capabilities
  //
  ASSERT (DescriptorBlock->NumberOfSmmReservedRegions <= MAX_SMRAM_RANGES);
  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    SmmAccessPrivate->SmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
    SmmAccessPrivate->SmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
    SmmAccessPrivate->SmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
    SmmAccessPrivate->SmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
  }

  SmmAccessPrivate->NumberRegions              = Index;
  SmmAccessPrivate->SmmAccess.Open             = Open;
  SmmAccessPrivate->SmmAccess.Close            = Close;
  SmmAccessPrivate->SmmAccess.Lock             = Lock;
  SmmAccessPrivate->SmmAccess.GetCapabilities  = GetCapabilities;
  SmmAccessPrivate->SmmAccess.LockState        = FALSE;
  SmmAccessPrivate->SmmAccess.OpenState        = FALSE;

  PpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiList->Guid  = &gPeiSmmAccessPpiGuid;
  PpiList->Ppi   = &SmmAccessPrivate->SmmAccess;

  Status      = (**PeiServices).InstallPpi (PeiServices, PpiList);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all PEIM
  and SMM agents.

  @param[in] PeiServices              General purpose services available to every PEIM
  @param[in] This                     Pointer to the SMM Access Interface
  @param[in] DescriptorIndex          Region of SMRAM to Open

  @retval    EFI_SUCCESS              The region was successfully opened.
  @retval    EFI_DEVICE_ERROR         The region could not be opened because locked by chipset.
  @retval    EFI_INVALID_PARAMETER    The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_SMM_ACCESS_PPI       *This,
  IN UINTN                    DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA     *SmmAccess;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG (( EFI_D_ERROR, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    DEBUG ((EFI_D_ERROR, "Cannot open a locked SMRAM region\n"));
    return EFI_DEVICE_ERROR;
  }


  Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6840, 0, 0xFF);
  Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6848, 0, 0xFF);

  //
  // END CHIPSET SPECIFIC CODE
  //
  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_OPEN;
  SmmAccess->SmmAccess.OpenState = TRUE;

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "close" a region of SMRAM.  This is valid for
  compatible SMRAM region.

  @param[in] PeiServices              General purpose services available to every PEIM
  @param[in] This                     Pointer to the SMM Access Interface
  @param[in] DescriptorIndex          Region of SMRAM to Close

  @retval    EFI_SUCCESS              The region was successfully closed.
  @retval    EFI_DEVICE_ERROR         The region could not be closed because locked by chipset.
  @retval    EFI_INVALID_PARAMETER    The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_SMM_ACCESS_PPI         *This,
  IN UINTN                      DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA       *SmmAccess;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_ERROR, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    DEBUG ((EFI_D_ERROR, "Cannot close a locked SMRAM region\n"));
    return EFI_DEVICE_ERROR;
  }
  if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_CLOSED) {
    return EFI_DEVICE_ERROR;
  }

  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~EFI_SMRAM_OPEN;
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= (EFI_SMRAM_CLOSED | EFI_ALLOCATED);

  SmmAccess->SmmAccess.OpenState = FALSE;

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to PEIM.

  @param[in] PeiServices             General purpose services available to every PEIM.
  @param[in] This                    Pointer to the SMM Access Interface.
  @param[in] DescriptorIndex         Region of SMRAM to Lock.

  @retval    EFI_SUCCESS             The region was successfully locked.
  @retval    EFI_DEVICE_ERROR        The region could not be locked because at least one range is still open.
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_SMM_ACCESS_PPI         *This,
  IN UINTN                      DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA       *SmmAccess;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_ERROR, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmmAccess.OpenState) {
    DEBUG ((EFI_D_ERROR, "Cannot lock SMRAM when SMRAM regions are still open\n"));
    return EFI_DEVICE_ERROR;
  }

  Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6838, 0, 0x00);

  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
  SmmAccess->SmmAccess.LockState = TRUE;

  return EFI_SUCCESS;
}


/**
  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the
  memory controller capabilities.

  @param[in]      PeiServices             General purpose services available to every PEIM.
  @param[in]      This                    Pointer to the SMRAM Access Interface.
  @param[in, out] SmramMapSize            Pointer to the variable containing size of the
                                          buffer to contain the description information.
  @param[in, out] SmramMap                Buffer containing the data describing the Smram
                                          region descriptors.

  @retval         EFI_BUFFER_TOO_SMALL    The user did not provide a sufficient buffer.
  @retval         EFI_SUCCESS             The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN PEI_SMM_ACCESS_PPI                *This,
  IN OUT   UINTN                       *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  EFI_STATUS                Status;
  SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
  UINTN                     BufferSize;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);
  BufferSize = SmmAccess->NumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize < BufferSize) {
    if (*SmramMapSize != 0) {
      DEBUG((EFI_D_ERROR, "SMRAM Map Buffer too small\n"));
    }
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, SmmAccess->SmramDesc, *SmramMapSize);
    Status = EFI_SUCCESS;
  }
  *SmramMapSize = BufferSize;

  return Status;
}

