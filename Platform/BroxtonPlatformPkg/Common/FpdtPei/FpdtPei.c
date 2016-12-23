/** @file
  This module collects FPDT boot records on S3 Resume boot path.
  And, updates the boot records into basic boot performance table.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <PeiFirmwarePerformance.h>
#include <Guid/FirmwarePerformance.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Guid/SetupVariable.h>
#include <ExtendedFirmwarePerformanceData.h>

/**
  Collect PEI boot records and update them into the basic boot performance table.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval    EFI_SUCCESS       Successfully update the Boot records.
  @retval    EFI_NOT_FOUND     PEI boot records are not found or Basic boot performance table is not found.

**/
EFI_STATUS
EFIAPI
EndOfPeiSignalPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  UINTN                            VarSize;
  UINT8                            *BootPerformanceTable;
  GUID_EVENT_RECORD                *GuidEvent;
  UINT32                           Index;
  UINT32                           TableLength;
  PEI_FIRMWARE_PERFORMANCE_HOB     *FirmwarePerformanceHob;
  PEI_GUID_EVENT_RECORD            *PeiGuidRec;
  EFI_HOB_GUID_TYPE                *GuidHob;
  SYSTEM_CONFIGURATION             SystemConfiguration;
  FIRMWARE_PERFORMANCE_VARIABLE    PerformanceVariable;
  EFI_BOOT_MODE                    BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid,
               0,
               NULL,
               (VOID **) &VariableServices
               );
    ASSERT_EFI_ERROR (Status);

    //
    // Update S3 boot records into the basic boot performance table.
    //
    VarSize = sizeof (SystemConfiguration);

    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 PLATFORM_SETUP_VARIABLE_NAME,
                                 &gEfiSetupVariableGuid,
                                 NULL,
                                 &VarSize,
                                 &SystemConfiguration
                                 );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    PerformanceVariable.BootPerformanceTablePointer = (EFI_PHYSICAL_ADDRESS) (UINTN) SystemConfiguration.BootPerformanceTablePointer;
    PerformanceVariable.S3PerformanceTablePointer   = (EFI_PHYSICAL_ADDRESS) (UINTN) SystemConfiguration.S3PerformanceTablePointer;
    BootPerformanceTable = (UINT8*) (UINTN) PerformanceVariable.BootPerformanceTablePointer;
    DEBUG ((DEBUG_INFO, "FPDT: Performance Boot Performance Table address = 0x%x\n", PerformanceVariable.BootPerformanceTablePointer));

    //
    // Get the table Length with table header and basic boot record.
    // S3 boot records will update into the basic boot performance table and override normal boot records.
    //
    TableLength = sizeof (BOOT_PERFORMANCE_TABLE);
    DEBUG ((DEBUG_INFO, "FPDT: Performance Boot Performance Table Raw size = 0x%x\n", TableLength));

    //
    // Dump PEI boot records
    //
    GuidEvent = (GUID_EVENT_RECORD *) (BootPerformanceTable + TableLength);
    GuidHob   = GetFirstGuidHob (&gPeiFirmwarePerformanceGuid);
    if (GuidHob != NULL) {
      FirmwarePerformanceHob = (PEI_FIRMWARE_PERFORMANCE_HOB *) GET_GUID_HOB_DATA (GuidHob);
      PeiGuidRec  = &(FirmwarePerformanceHob->GuidEventRecord[0]);

      for (Index = 0; Index < FirmwarePerformanceHob->NumberOfEntries; Index ++, PeiGuidRec ++, GuidEvent ++) {
        //
        // GUID Event Records from PEI phase
        //
        GuidEvent->Header.Type       = GUID_EVENT_TYPE;
        GuidEvent->Header.Length     = sizeof (GUID_EVENT_RECORD);
        GuidEvent->Header.Revision   = RECORD_REVISION_1;
        GuidEvent->ProgressID        = PeiGuidRec->ProgressID;
        GuidEvent->ApicID            = PeiGuidRec->ApicID;
        GuidEvent->Timestamp         = PeiGuidRec->Timestamp;
        CopyGuid (&GuidEvent->Guid, &PeiGuidRec->Guid);
      }

      DEBUG ((DEBUG_INFO, "FPDT: Performance S3 Boot Performance Record Number is 0x%x\n", FirmwarePerformanceHob->NumberOfEntries));
    }

    //
    // Update Table length.
    //
    ((BOOT_PERFORMANCE_TABLE *) BootPerformanceTable)->Header.Length = (UINT32)(UINTN)((UINT8 *) GuidEvent - BootPerformanceTable);
    DEBUG ((DEBUG_INFO, "FPDT: Performance Boot Performance Table updated size = 0x%x\n", ((BOOT_PERFORMANCE_TABLE *) BootPerformanceTable)->Header.Length));
  }

  return EFI_SUCCESS;
}


EFI_PEI_NOTIFY_DESCRIPTOR mEndOfPeiSignalPpiNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    EndOfPeiSignalPpiNotifyCallback
  }
};


/**
  Main entry for Firmware Performance Data Table PEIM.
  It installs EndOfPeiSignalPpi Notify function on S3 boot path.

  @param[in]  FileHandle              Handle of the file being invoked.
  @param[in]  PeiServices             Pointer to PEI Services table.

  @retval     EFI_SUCCESS             Install Notify function successfully.

**/
EFI_STATUS
EFIAPI
FpdtPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS               Status;

  if (FeaturePcdGet (PcdFirmwarePerformanceDataTableS3Support)) {
    //
    // Register for a callback once memory has been initialized.
    //
    Status = PeiServicesNotifyPpi (&mEndOfPeiSignalPpiNotifyList[0]);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

