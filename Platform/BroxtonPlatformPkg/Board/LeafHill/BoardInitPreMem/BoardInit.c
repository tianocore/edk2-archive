/** @file
  Board Init driver.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/PlatformInfo_Aplk.h>
#include <Ppi/BoardInitSignalling.h>
#include "BoardInit.h"
#include "PlatformId.h"
#include "BoardInitMiscs.h"

EFI_STATUS
EFIAPI
LeafHillPreMemInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN PEI_BOARD_PRE_MEM_INIT_PPI *This
  );

static PEI_BOARD_PRE_MEM_INIT_PPI mLeafHillPreMemInitPpiInstance = {
  LeafHillPreMemInit
};

static EFI_PEI_PPI_DESCRIPTOR mLeafHillPreMemInitPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardPreMemInitPpiGuid,
  &mLeafHillPreMemInitPpiInstance
};

static EFI_PEI_PPI_DESCRIPTOR mLeafHillPreMemInitDonePpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardPreMemInitDoneGuid,
  NULL
};

EFI_STATUS
EFIAPI
LeafHillPreMemInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN PEI_BOARD_PRE_MEM_INIT_PPI *This
  )
{
  EFI_STATUS                       Status;
  VOID                             *Instance;
  UINT8                            BoardId;
  UINT8                            FabId;

  BoardId = 0;
  FabId = 0;
  Status = PeiServicesLocatePpi (
             &gBoardPreMemInitDoneGuid,
             0,
             NULL,
             &Instance
             );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // Pre Mem Board Init
  //
  Status = LeafHillGetEmbeddedBoardIdFabId (PeiServices, &BoardId, &FabId);
  if (BoardId != (UINT8) BOARD_ID_LFH_CRB) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO,  "This is LeafHill CRB.\n"));

  PcdSet8 (PcdBoardId, BoardId);
  PcdSet8 (PcdFabId, FabId);

  //
  // Set board specific function as dynamic PCD to be called by common platform code
  //
  PcdSet64 (PcdUpdateFspmUpdFunc, (UINT64) (UINTN) mLhUpdateFspmUpdPtr);
  PcdSet64 (PcdDramCreatePolicyDefaultsFunc, (UINT64) (UINTN) mLhDramCreatePolicyDefaultsPtr);

  //
  // Install a flag signalling a board is detected and pre-mem init is done
  //
  Status = PeiServicesInstallPpi (&mLeafHillPreMemInitDonePpi);

  return EFI_SUCCESS;
}


/**
  This function performs Board initialization in Pre-Memory.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).

**/
EFI_STATUS
EFIAPI
LeafHillInitConstructor (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  VOID                             *Ppi;
  EFI_PEI_PPI_DESCRIPTOR           *PeiPpiDescriptor;
  UINTN                            Instance;

  DEBUG ((EFI_D_INFO,  "LeafHill Pre Mem Init Constructor \n"));

  Status = PeiServicesLocatePpi (
             &gBoardPreMemInitDoneGuid,
             0,
             &PeiPpiDescriptor,
             &Ppi
             );
  if (!EFI_ERROR (Status)) {
    //
    // Board detection previously done, so this is a re-invocation shadowed in memory.
    // Reinstall PPIs to eliminate PPI descriptors in torn down temp RAM.
    //
    //
    // Reinstall PreMemInit Done PPI
    //
    DEBUG ((EFI_D_INFO,  "Reinstall Pre Mem Init Done PPI\n"));
    Status = PeiServicesReInstallPpi (
               PeiPpiDescriptor,
               &mLeafHillPreMemInitDonePpi
               );
    ASSERT_EFI_ERROR (Status);

    //
    // Reinstall all instances of Pre Mem Init PPIs.
    // These PPIs are no longer used so it doesn't matter which board's instance is finally installed.
    // According to PeiServicesReInstallPpi behavior:
    // The first run of this loop would replace all descrioptors with a singe in-RAM descriptor;
    // Subsequent runs of this loop will only replace the first (already in-RAM) descriptor.
    // As long as all descriptors are in ram, we are fine.
    //
    Instance = 0;
    do {
      Status = PeiServicesLocatePpi (
                 &gBoardPreMemInitPpiGuid,
                 Instance,
                 &PeiPpiDescriptor,
                 &Ppi
                 );
      if (Status == EFI_NOT_FOUND) {
        break;
      }
      ASSERT_EFI_ERROR (Status);
      DEBUG ((EFI_D_INFO,  "Reinstall Pre Mem Init PPI\n"));
      Status = PeiServicesReInstallPpi (
                 PeiPpiDescriptor,
                 &mLeafHillPreMemInitPpi
                 );
      ASSERT_EFI_ERROR (Status);
      Instance++;
    } while (TRUE);
    return Status;
  }
  DEBUG ((EFI_D_INFO,  "Install Pre Mem Init PPI \n"));
  Status = PeiServicesInstallPpi (&mLeafHillPreMemInitPpi);

  return Status;
}

