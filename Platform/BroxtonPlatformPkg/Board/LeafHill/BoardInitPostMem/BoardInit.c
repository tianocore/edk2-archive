/** @file
  Board Init driver.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>

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
#include "BoardInitMiscs.h"

EFI_STATUS
EFIAPI
LeafHillPostMemInitCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );


static EFI_PEI_NOTIFY_DESCRIPTOR mLeafHillPostMemNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardPostMemInitStartGuid,
  LeafHillPostMemInitCallback
};

static EFI_PEI_PPI_DESCRIPTOR mLeafHillPostMemDonePpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardPostMemInitDoneGuid,
  NULL
};

EFI_STATUS
EFIAPI
LeafHillPostMemInitCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                       Status;
  VOID                             *Instance;
  UINT8                            BoardId;
  UINT8                            FabId;
  UINT8                            ResetType;
  UINTN                            BufferSize;
  UINT8                            MaxPkgCState;

  Status = PeiServicesLocatePpi (
             &gBoardPostMemInitDoneGuid,
             0,
             NULL,
             &Instance
             );
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO,  "Leaf Hill Post Mem Init callback: Skip\n"));
    return EFI_SUCCESS;
  }

  BoardId = (UINT8) PcdGet8 (PcdBoardId);
  FabId = (UINT8) PcdGet8 (PcdFabId);
  DEBUG ((EFI_D_INFO,  "PostMemInit: BoardId == 0x%X, FabId == 0x%X\n", BoardId, FabId));
  if (BoardId != (UINT8) BOARD_ID_LFH_CRB) {
    DEBUG ((EFI_D_INFO,  "LeafHill Post Mem Init callback: Skip\n"));
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO,  "LeafHill Post Mem Init callback\n"));

  //
  // Set init function PCD
  //
  PcdSet64 (PcdBoardPostMemInitFunc, (UINT64) (UINTN) LeafHillMultiPlatformInfoInit);
  
  //
  // Set Reset Type according to different Board
  //
  ResetType = V_RST_CNT_FULLRESET;
  PcdSet8 (PcdResetType, (UINT8) ResetType);

  //
  // Board specific VBT table.
  //
  BufferSize = sizeof (EFI_GUID);
  PcdSetPtr(PcdBoardVbtFileGuid, &BufferSize, (UINT8 *)&gPeiLeafHillVbtGuid);
  
  //
  // Set PcdSueCreek
  //
  PcdSetBool (PcdSueCreek, FALSE);

  //
  // Set PcdMaxPkgCState
  //
  MaxPkgCState = MAX_PKG_CSTATE_C2;
  PcdSet8 (PcdMaxPkgCState, (UINT8) MaxPkgCState);

  //
  // Add init steps here
  //
  //
  // Install a flag signalling a board's post mem init is done
  //
  Status = PeiServicesInstallPpi (&mLeafHillPostMemDonePpi);

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
  EFI_STATUS                        Status;

  DEBUG ((EFI_D_INFO,  "LeafHill Post Mem Init Constructor \n"));

  DEBUG ((EFI_D_INFO,  "Notify on Post Mem Init Start PPI \n"));
  Status = PeiServicesNotifyPpi (&mLeafHillPostMemNotifyList);

  return Status;
}

