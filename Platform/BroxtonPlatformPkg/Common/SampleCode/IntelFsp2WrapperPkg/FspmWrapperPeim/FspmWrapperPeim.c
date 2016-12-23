/** @file
  This will be invoked only once. It will call FspMemoryInit API,
  register TemporaryRamDonePpi to call TempRamExit API, and register MemoryDiscoveredPpi
  notify to call FspSiliconInit API.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/FspPlatformInfoLib.h>
#include <Library/FspHobProcessLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Ppi/FspSiliconInitDone.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/TemporaryRamDone.h>
#include <FspmUpd.h>

BOOLEAN    ImageInMemory = FALSE;


VOID
EFIAPI
FspPolicyInitPreMem (
  IN OUT FSPM_UPD       *FspmUpd
  );
/**
  This function is called after PEI core discover memory and finish migration.

  @param[in] PeiServices    Pointer to PEI Services Table.
  @param[in] NotifyDesc     Pointer to the descriptor for the Notification event that
                            caused this function to execute.
  @param[in] Ppi            Pointer to the PPI data associated with this function.

  @retval EFI_STATUS        Always return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PeiMemoryDiscoveredNotify (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mPeiPreMemNotifyDesc[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    PeiMemoryDiscoveredNotify
  }
};

EFI_PEI_PPI_DESCRIPTOR mFspTempRamExitPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gFspTempRamExitGuid,
    NULL
  }
};


/**
  Call FspMemoryInit API.

  @return Status returned by FspMemoryInit API.

**/
EFI_STATUS
PeiFspMemoryInit (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  FSP_INFO_HEADER           *FspmHeaderPtr;
  EFI_STATUS                Status;
  UINT64                    TimeStampCounterStart;
  VOID                      *FspHobListPtr;
  VOID                      *HobData;
  FSPM_UPD                  *FspmUpdDataPtr;
  UINTN                     *SourceData;

  DEBUG ((DEBUG_INFO, "PeiFspMemoryInit enter\n"));

  FspHobListPtr = NULL;

  //
  // Copy default FSP-M UPD data from Flash
  //
  FspmHeaderPtr = (FSP_INFO_HEADER *) FspFindFspHeader (PcdGet32 (PcdFspmBaseAddress));

  DEBUG ((DEBUG_INFO, "PcdFspmBaseAddress = 0x%X\n", PcdGet32 (PcdFspmBaseAddress)));
  DEBUG ((DEBUG_INFO, "FspmHeaderPtr = 0x%X\n", FspmHeaderPtr));
  DEBUG ((DEBUG_INFO, "FspmHeaderPtr->CfgRegionSize = 0x%X\n", FspmHeaderPtr->CfgRegionSize));

  FspmUpdDataPtr = (FSPM_UPD *) AllocateZeroPool ((UINTN) FspmHeaderPtr->CfgRegionSize);
  ASSERT (FspmUpdDataPtr != NULL);
  SourceData = (UINTN *) ((UINTN) FspmHeaderPtr->ImageBase + (UINTN) FspmHeaderPtr->CfgRegionOffset);
  CopyMem (FspmUpdDataPtr, SourceData, (UINTN)FspmHeaderPtr->CfgRegionSize);

  DEBUG ((DEBUG_INFO, "FspWrapperPlatformInitPreMem enter\n"));
  FspmUpdDataPtr = UpdateFspUpdConfigs (PeiServices, FspmUpdDataPtr);
  FspPolicyInitPreMem (FspmUpdDataPtr);

  DEBUG ((DEBUG_INFO, "  NvsBufferPtr        - 0x%x\n", FspmUpdDataPtr->FspmArchUpd.NvsBufferPtr));
  DEBUG ((DEBUG_INFO, "  StackBase           - 0x%x\n", FspmUpdDataPtr->FspmArchUpd.StackBase));
  DEBUG ((DEBUG_INFO, "  StackSize           - 0x%x\n", FspmUpdDataPtr->FspmArchUpd.StackSize));
  DEBUG ((DEBUG_INFO, "  BootLoaderTolumSize - 0x%x\n", FspmUpdDataPtr->FspmArchUpd.BootLoaderTolumSize));
  DEBUG ((DEBUG_INFO, "  BootMode            - 0x%x\n", FspmUpdDataPtr->FspmArchUpd.BootMode));
  DEBUG ((DEBUG_INFO, "  HobListPtr          - 0x%x\n", &FspHobListPtr));
  TimeStampCounterStart = AsmReadTsc ();
  Status = CallFspMemoryInit (FspmUpdDataPtr, &FspHobListPtr);
  PERF_START_EX (&gFspApiPerformanceGuid, "EventRec", NULL, TimeStampCounterStart, 0xD000);
  PERF_END_EX (&gFspApiPerformanceGuid, "EventRec", NULL, 0, 0xD07F);
  DEBUG ((DEBUG_INFO, "Total time spent executing FspMemoryInitApi: %d millisecond\n", DivU64x32(GetTimeInNanoSecond(AsmReadTsc() - TimeStampCounterStart), 1000000)));
  // Reset the system if FSP API returned FSP_STATUS_RESET_REQUIRED status
  if ((Status & ~(0xF)) == (FSP_STATUS_RESET_REQUIRED_COLD & ~(0xF))) {
    DEBUG ((DEBUG_INFO, "FspMemoryInitApi requested reset 0x%x\n", Status));
    CallFspWrapperResetSystem ((UINT32) Status);
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - Failed to execute FspMemoryInitApi(), Status = %r\n", Status));
  }
  DEBUG ((DEBUG_INFO, "FspMemoryInit status: 0x%x\n", Status));
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "  FspHobListPtr (returned) - 0x%x\n", FspHobListPtr));
  ASSERT (FspHobListPtr != NULL);

  FspHobProcessForMemoryResource (FspHobListPtr);

  //
  // FspHobList is not complete at this moment.
  // Save FspHobList pointer to hob, so that it can be got later
  //
  HobData = BuildGuidHob (
              &gFspHobGuid,
              sizeof (VOID *)
              );
  ASSERT (HobData != NULL);
  CopyMem (HobData, &FspHobListPtr, sizeof (FspHobListPtr));

  return Status;
}

/**
  TemporaryRamDone() disables the use of Temporary RAM. If present, this service is invoked
  by the PEI Foundation after the EFI_PEI_PERMANANT_MEMORY_INSTALLED_PPI is installed.

  @retval EFI_SUCCESS           Use of Temporary RAM was disabled.
  @retval EFI_INVALID_PARAMETER Temporary RAM could not be disabled.

**/
EFI_STATUS
EFIAPI
PeiMemoryDiscoveredNotify (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  UINT64                      TimeStampCounterStart;
  EFI_STATUS                  Status;
  VOID                        *TempRamExitParam;

  DEBUG ((DEBUG_INFO, "PeiTemporaryRamDone enter\n"));

  TempRamExitParam = GetTempRamExitParam ();
  TimeStampCounterStart = AsmReadTsc ();
  PERF_START_EX (&gFspApiPerformanceGuid, "EventRec", NULL, 0, 0xB000);
  Status = CallTempRamExit (TempRamExitParam);
  PERF_END_EX (&gFspApiPerformanceGuid, "EventRec", NULL, 0, 0xB07F);
  DEBUG ((DEBUG_INFO, "Total time spent executing FspTempRamExitApi: %d millisecond\n", DivU64x32 (GetTimeInNanoSecond (AsmReadTsc () - TimeStampCounterStart), 1000000)));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR - Failed to execute FspTempRamExitApi(), Status = %r\n", Status));
  }
  DEBUG ((DEBUG_INFO, "TempRamExit status: 0x%x\n", Status));
  ASSERT_EFI_ERROR (Status);

  //
  // Install gFspTempRamExitGuid to notify wrapper.
  //
  Status = PeiServicesInstallPpi (mFspTempRamExitPpiList);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Do FSP initialization.

  @return FSP initialization status.

**/
EFI_STATUS
EFIAPI
FspmWrapperInit (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS           Status;

  if (!ImageInMemory) {
    Status = PeiFspMemoryInit (PeiServices);
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // Register MemoryDiscovered Notify to run TempRamExit
    //
    Status = PeiServicesNotifyPpi (&mPeiPreMemNotifyDesc[0]);
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}

/**
  This is the entrypoint of PEIM

  @param[in] FileHandle  Handle of the file being invoked.
  @param[in] PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS if it completed successfully.

**/
EFI_STATUS
EFIAPI
FspmWrapperPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  Status = (*PeiServices)->RegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    ImageInMemory = TRUE;
  } else if (Status == EFI_NOT_FOUND) {
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG((DEBUG_INFO, "FspmWrapperPeimEntryPoint\n"));
  FspmWrapperInit (PeiServices);

  return EFI_SUCCESS;
}

