/** @file
  This file contains Processor Power Management ACPI related functions for
  processors.

  <b>Acronyms:</b>
     PPM: Processor Power Management
     TM:  Thermal Monitor
     IST: Intel(R) Speedstep technology
     HT:  Hyper-Threading Technology

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtCommon.h"

/**
  Update the SSDT table pointers and config DWORD CFGD with the PpmFlags current configuration value

**/
VOID
PatchCpuSsdtTable (
  VOID
  )
{
  UINT8        *CurrPtr;
  UINT32       *Signature;
  SSDT_LAYOUT  *SsdtPackage;

  //
  // Locate the SSDT package
  //
  SsdtPackage = NULL;
  CurrPtr     = (UINT8 *) mCpuSsdtTable;
  for (CurrPtr = (UINT8 *) mCpuSsdtTable; CurrPtr <= ((UINT8 *) mCpuSsdtTable + mCpuSsdtTable->Length); CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('S', 'S', 'D', 'T')) {
      //
      // Update the SSDT table pointers for dynamically loaded tables
      //
      SsdtPackage = (SSDT_LAYOUT *) CurrPtr;
      //
      // Set the P-State SSDT table information
      //
      SsdtPackage->Cpu0IstAddr  = (UINT32) (UINTN) mCpu0IstTable;
      SsdtPackage->Cpu0IstLen   = mCpu0IstTable->Length;
      SsdtPackage->ApIstAddr    = (UINT32) (UINTN) mApIstTable;
      SsdtPackage->ApIstLen     = mApIstTable->Length;
      //
      // Set the C-State SSDT table information
      //
      SsdtPackage->Cpu0CstAddr  = (UINT32) (UINTN) mCpu0CstTable;
      SsdtPackage->Cpu0CstLen   = mCpu0CstTable->Length;
      SsdtPackage->ApCstAddr    = (UINT32) (UINTN) mApCstTable;
      SsdtPackage->ApCstLen     = mApCstTable->Length;
      DEBUG ((DEBUG_INFO, "\n Cpu0Ist       :%x   :%x \n",SsdtPackage->Cpu0IstAddr,SsdtPackage->Cpu0CstLen));
      DEBUG ((DEBUG_INFO, "\n Cpu0Cst       :%x   :%x \n",SsdtPackage->Cpu0CstAddr,SsdtPackage->Cpu0CstLen));
    }
    //
    // Update the CPU GlobalNvs area
    //
    if ((*CurrPtr == AML_EXT_REGION_OP) && *Signature == SIGNATURE_32 ('P', 'N', 'V', 'S')) {
      ASSERT_EFI_ERROR (*(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) == 0xFFFF0000);
      ASSERT_EFI_ERROR (*(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
      //
      // CPU Global NVS Area address
      //
      *(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) = (UINT32) (UINTN) gCpuGlobalNvsAreaProtocol->Area;
      //
      // CPU Global NVS Area size
      //
      *(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (CPU_GLOBAL_NVS_AREA);
      break;
    }
  }
  //
  // Assert if we didn't update the SSDT table
  //
  ASSERT (SsdtPackage != NULL);

  return;
}


/**
  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @retval EFI_SUCCESS            On success
  @retval EFI_NOT_FOUND          Required firmware volume not found
  @retval EFI_OUT_OF_RESOURCES   No enough resoruces (such as out of memory).
  @retval Appropiate failure code on error

**/
EFI_STATUS
InitCpuAcpiTable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                          NumberOfHandles;
  EFI_FV_FILETYPE                FileType;
  UINT32                         FvStatus;
  EFI_FV_FILE_ATTRIBUTES         Attributes;
  UINTN                          Size;
  UINTN                          i;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *FwVol;
  INTN                           Instance;
  EFI_ACPI_TABLE_VERSION         Version;
  EFI_ACPI_COMMON_HEADER         *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER    *TempTable;
  UINTN                          AcpiTableHandle;

  FwVol = NULL;

  //
  // Locate Firmware volume protocol.
  // There is little chance we can't find an FV protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Look for FV with ACPI storage file
  //
  FwVol = NULL;
  for (i = 0; i < NumberOfHandles; i++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gCpuAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  // Free any allocated buffers
  //
  FreePool (HandleBuffer);
  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
  //
  // Read tables from the storage file.
  //
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gCpuAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      //
      // Check the table ID to modify the table
      //
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId) {
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)):
          mCpu0IstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
            //
            // Patch the native _PSS package with the EIST values
            //
            Status = AcpiPatchPss ();
            if (EFI_ERROR (Status)) {
              return Status;
            }
          }
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'C', 's', 't', 0)):
          mCpu0CstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'T', 's', 't', 0)):
          mCpu0TstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'I', 's', 't', 0, 0, 0)):
          mApIstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'C', 's', 't', 0, 0, 0)):
          mApCstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'T', 's', 't', 0, 0, 0)):
          mApTstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', 'S', 's', 'd', 't', 0)):
          mCpuSsdtTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        default:
          break;
      }
      Instance++; // Increment the instance
      CurrentTable = NULL;
    }
  }
  //
  // Copy CpuoIst SSDT if EIST is enabled
  //
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
    TempTable = AllocateReservedPool (mCpu0IstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mCpu0IstTable, mCpu0IstTable->Length);
    FreePool (mCpu0IstTable);
    mCpu0IstTable = TempTable;
    AcpiChecksum (mCpu0IstTable, mCpu0IstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                           mAcpiTable,
                           mCpu0IstTable,
                           mCpu0IstTable->Length,
                           &AcpiTableHandle
                           );
  }
  //
  // If we are CMP, then the PPM tables are dynamically loaded:
  //   We need to publish the CpuPm table to the ACPI tables, and move the CST
  //   tables that are dynamically loaded to a separate location so that we can fix the
  //   addresses in the CpuPm table.
  // Otherwise (non-CMP):
  //   We need to publish CPU 0 tables only, and CST tables only if CST is enabled
  //
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
    //
    // Copy tables to our own location and checksum them
    //
    TempTable = AllocateReservedPool (mApIstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mApIstTable, mApIstTable->Length);
    FreePool (mApIstTable);
    mApIstTable = TempTable;
    AcpiChecksum (mApIstTable, mApIstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    TempTable = AllocateReservedPool (mCpu0CstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mCpu0CstTable, mCpu0CstTable->Length);
    FreePool (mCpu0CstTable);
    mCpu0CstTable = TempTable;
    AcpiChecksum (mCpu0CstTable, mCpu0CstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    TempTable = AllocateReservedPool (mApCstTable->Length);
    if (TempTable == NULL) {
      ASSERT (TempTable != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (TempTable, mApCstTable, mApCstTable->Length);
    FreePool (mApCstTable);
    mApCstTable = TempTable;
    AcpiChecksum (mApCstTable, mApCstTable->Length, EFI_FIELD_OFFSET (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
  } else {
    //
    // CMP disabled, so statically load the tables
    //
    // Add CST SSDT if C states are enabled
    //
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                             mAcpiTable,
                             mCpu0CstTable,
                             mCpu0CstTable->Length,
                             &AcpiTableHandle
                             );
    }
    //
    // Since we are UP, there is no need for the CPU 1 tables
    //
    //
    // Free all tables, since they have been copied into ACPI tables by ACPI support protocol
    //
    FreePool (mCpu0CstTable);
    FreePool (mApIstTable);
    FreePool (mApCstTable);
  }
  //
  // Update the CpuSsdt SSDT table in the ACPI tables.
  //
  PatchCpuSsdtTable ();
  AcpiTableHandle = 0;
  Status = mAcpiTable->InstallAcpiTable (
                         mAcpiTable,
                         mCpuSsdtTable,
                         mCpuSsdtTable->Length,
                         &AcpiTableHandle
                         );
  FreePool (mCpuSsdtTable);
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TSTATES) {
    //
    // Load the Cpu0Tst SSDT table in the ACPI tables
    //
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                           mAcpiTable,
                           mCpu0TstTable,
                           mCpu0TstTable->Length,
                           &AcpiTableHandle
                           );
    FreePool (mCpu0TstTable);
    //
    // If the CMP is enabled then load the ApTst SSDT table in the ACPI tables
    //
    if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                             mAcpiTable,
                             mApTstTable,
                             mApTstTable->Length,
                             &AcpiTableHandle
                             );
    }
  }
  FreePool (mApTstTable);

  return Status;
}


/**
  Configure the FACP for C state support

**/
VOID
ConfigureFadtCStates (
  VOID
  )
{
  EFI_STATUS                                 Status;
  EFI_ACPI_DESCRIPTION_HEADER                *Table;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE  *FadtPointer;
  UINTN                                      Handle;

  //
  // Locate table with matching ID
  //
  InitializeAslUpdateLib ();

  Handle = 0;
  Status = LocateAcpiTableBySignature (
             EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
             (EFI_ACPI_DESCRIPTION_HEADER **)  &Table,
             &Handle
             );
  //
  // Can't have ACPI without FADT, so safe to assert
  //
  ASSERT (Table->Signature == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE);
  FadtPointer = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *) Table;

  //
  // Configure C states
  //
  if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C3) {
    //
    // Enable C3 in FADT.
    //
    FadtPointer->PLvl3Lat = FADT_C3_LATENCY;
  }
  //
  // Update the table
  //
  Status = mAcpiTable->InstallAcpiTable (
                         mAcpiTable,
                         Table,
                         Table->Length,
                         &Handle
                         );
  FreePool (Table);

  return;
}

