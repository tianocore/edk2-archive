/** @file
  This is the driver that initializes the System Agent.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SaInit.h"
#include "SaRegs.h"
#include <Protocol/SaPolicy.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/MemoryAllocationLib.h>

SA_POLICY_PROTOCOL  *SaPolicy;

extern EFI_GUID gSaSsdtAcpiTableStorageGuid;

/**
  Initialize System Agent SSDT ACPI tables.

  @retval    EFI_SUCCESS         ACPI tables are initialized successfully.
  @retval    EFI_NOT_FOUND       ACPI tables not found.

**/
EFI_STATUS
InitializeSaSsdtAcpiTables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         i;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         AcpiTableKey;
  EFI_ACPI_DESCRIPTION_HEADER   *SaAcpiTable;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  FwVol       = NULL;
  SaAcpiTable = NULL;

  //
  // Locate ACPI Table protocol
  //
  DEBUG ((DEBUG_INFO, "Init SA SSDT table\n"));
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Fail to locate EfiAcpiTableProtocol.\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Locate protocol.
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
  // Looking for FV with ACPI storage file
  //
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
                      &gSaSsdtAcpiTableStorageGuid,
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
  // Free any allocated buffers
  //
  FreePool (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    DEBUG ((DEBUG_INFO, "SA SSDT table not found\n"));
    return EFI_NOT_FOUND;
  }
  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  // Read tables from the storage file.
  //
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gSaSsdtAcpiTableStorageGuid,
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
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('S', 'a', 'S', 's', 'd', 't', ' ', 0)) {
        SaAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        AcpiTableKey = 0;
        Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              SaAcpiTable,
                              SaAcpiTable->Length,
                              &AcpiTableKey
                              );
        ASSERT_EFI_ERROR (Status);
        return EFI_SUCCESS;
      }
      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;
}


/**
  This is the standard EFI driver point that detects
  whether there is an ICH southbridge in the system
  and if so, initializes the chip.

  @param[in]  ImageHandle             Handle for the image of this driver.
  @param[in]  SystemTable             Pointer to the EFI System Table.

  @retval     EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
EFIAPI
SaInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS           Status;

  DEBUG ((DEBUG_INFO, "SaInitDxe Start\n"));

  //
  // IgdOpRegion Install Initialization
  //
  DEBUG ((EFI_D_INFO, "Initializing IGD OpRegion\n"));
  IgdOpRegionInit ();

  //
  // Install System Agent Ssdt ACPI table
  //
  DEBUG ((EFI_D_INFO, "Installing System Agent Ssdt ACPI table\n"));
  Status = InitializeSaSsdtAcpiTables ();
  DEBUG ((EFI_D_INFO, "SaInitDxe End\n"));

  return EFI_SUCCESS;
}

