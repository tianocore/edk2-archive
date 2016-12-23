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
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Guid/SmramMemoryReserve.h>


static SMM_ACCESS_PRIVATE_DATA  mSmmAccess;

/**
  This is the standard EFI driver point that detects
  whether there is an GMCH815 chipset in the system
  and if so, installs an SMM Access Protocol

  @param[in]  ImageHandle        Handle for the image of this driver
  @param[in]  SystemTable        Pointer to the EFI System Table

  @retval     EFI_SUCCESS        Protocol successfully started and installed
  @retval     EFI_UNSUPPORTED    Protocol can't be started

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *DescriptorBlock;
  VOID                            *GuidHob;

  //
  // Initialize Global variables
  //
  ZeroMem (&mSmmAccess, sizeof (mSmmAccess));

  mSmmAccess.Signature          = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;
  mSmmAccess.Handle             = NULL;

  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (!GuidHob) {
    DEBUG ((EFI_D_ERROR, "SmramMemoryReserve HOB not found\n"));
    return(EFI_NOT_FOUND);
  }

  DescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) ((UINT8 *) GuidHob + sizeof (EFI_HOB_GUID_TYPE));
  //
  // use the hob to publish SMRAM capabilities
  //
  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    mSmmAccess.SmramDesc[Index].PhysicalStart =
      DescriptorBlock->Descriptor[Index].PhysicalStart;
    mSmmAccess.SmramDesc[Index].CpuStart      =
      DescriptorBlock->Descriptor[Index].CpuStart;
    mSmmAccess.SmramDesc[Index].PhysicalSize  =
      DescriptorBlock->Descriptor[Index].PhysicalSize;
    mSmmAccess.SmramDesc[Index].RegionState   =
      DescriptorBlock->Descriptor[Index].RegionState;
  }

  mSmmAccess.NumberRegions               = Index;
  mSmmAccess.SmmAccess.Open              = Open;
  mSmmAccess.SmmAccess.Close             = Close;
  mSmmAccess.SmmAccess.Lock              = Lock;
  mSmmAccess.SmmAccess.GetCapabilities   = GetCapabilities;
  mSmmAccess.SmmAccess.LockState         = FALSE;
  mSmmAccess.SmmAccess.OpenState         = FALSE;

  //
  // Install our protocol interfaces on the device's handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmAccess.Handle,
                  &gEfiSmmAccess2ProtocolGuid,
                  &mSmmAccess.SmmAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InstallMultipleProtocolInterfaces returned %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all boot-service
  and SMM agents.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully opened
  @retval    EFI_DEVICE_ERROR        The region could not be opened because locked by chipset
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
  UINTN                     Index;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) {
    if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_LOCKED) {
      DEBUG ((EFI_D_WARN, "Cannot open a locked SMRAM region\n"));
      continue;
    }

    Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6840, 0, 0xFF);

    Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6848, 0, 0xFF);

    SmmAccess->SmramDesc[Index].RegionState &= ~(EFI_SMRAM_CLOSED |EFI_ALLOCATED);
    SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_OPEN;
  }
  SmmAccess->SmmAccess.OpenState = TRUE;

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "close" a region of SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "close" means that the memory is only visible from SMM agents,
  not from BS or RT code.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully closed
  @retval    EFI_DEVICE_ERROR        The region could not be closed because locked by chipset
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
  SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
  UINTN                     Index;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) {
    if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_LOCKED) {
      DEBUG ((EFI_D_WARN, "Cannot close a locked SMRAM region\n"));
      continue;
    }
    if (SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_CLOSED) {
      continue;
    }

    SmmAccess->SmramDesc[Index].RegionState &= ~EFI_SMRAM_OPEN;
    SmmAccess->SmramDesc[Index].RegionState |= (EFI_SMRAM_CLOSED |EFI_ALLOCATED);
  }
  SmmAccess->SmmAccess.OpenState = FALSE;

  return EFI_SUCCESS;
}


/**
  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to BS state.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully locked.
  @retval    EFI_DEVICE_ERROR        The region could not be locked because at least one range is still open.
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
  SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
  UINTN                     Index;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  for (Index = 0; Index < SmmAccess->NumberRegions; Index ++) {
    if (SmmAccess->SmmAccess.OpenState) {
      DEBUG ((EFI_D_WARN, "Cannot lock SMRAM when SMRAM regions are still open\n"));
      return EFI_DEVICE_ERROR;
    }

    Mmio64AndThenOr (MCH_BASE_ADDRESS, 0x6838, 0, 0x00);

    SmmAccess->SmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED;
  }
  SmmAccess->SmmAccess.LockState = TRUE;

  return EFI_SUCCESS;
}

/**
  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the
  memory controller capabilities.

  @param[in]      This                    Pointer to the SMRAM Access Interface.
  @param[in, out] SmramMapSize            Pointer to the variable containing size of the buffer to contain the description information
  @param[in, out] SmramMap                Buffer containing the data describing the Smram region descriptors

  @retval         EFI_BUFFER_TOO_SMALL    The user did not provide a sufficient buffer.
  @retval         EFI_SUCCESS             The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL    *This,
  IN OUT UINTN                         *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR          *SmramMap
  )
{
  EFI_STATUS                Status;
  SMM_ACCESS_PRIVATE_DATA   *SmmAccess;
  UINTN                     NecessaryBufferSize;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS(This);

  NecessaryBufferSize = SmmAccess->NumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize < NecessaryBufferSize) {
    if (*SmramMapSize != 0) {
      DEBUG((EFI_D_WARN, "SMRAM Map Buffer too small\n"));
    }
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, SmmAccess->SmramDesc, NecessaryBufferSize);
    DEBUG ((EFI_D_INFO, "SMRAM Map Buffer installed complete\n"));
    Status = EFI_SUCCESS;
  }
  *SmramMapSize = NecessaryBufferSize;

  return Status;
}

