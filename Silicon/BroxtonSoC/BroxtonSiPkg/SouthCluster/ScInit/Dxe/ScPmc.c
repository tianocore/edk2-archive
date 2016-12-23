/** @file
  Initializes SC PMC Devices.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInit.h"
#include <Protocol/PciEnumerationComplete.h>

/**
  Set PMC IO enable bit (Command register (BIT2 | BIT1 | BIT0)) after PCI enumeration

  @param[in] Event                      The Event that is being processed
  @param[in] Context                    Event Context

**/
VOID
EFIAPI
PmcPciEnumeratedCompleteNotify (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  UINTN     PmcBaseAddr;
  UINT16    AcpiBaseAddr;
  UINT32    IpcBaseAddr;

  DEBUG ((DEBUG_INFO, "Override Broxton PMC (bus 0, device 13, function 1) bar2 (offset 0x20) to PcdScAcpiIoPortBaseAddress(0x400)\n"));
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);
  PmcBaseAddr  = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PMC_IPC1,
                   PCI_FUNCTION_NUMBER_PMC_IPC1
                   );
  MmioWrite32 (PmcBaseAddr + R_PMC_ACPI_BASE, AcpiBaseAddr);

  DEBUG ((DEBUG_INFO, "Set PMC IO enable bit (Command register (BIT2 | BIT1 | BIT0)) after PCI enumeration\n"));
  MmioOr32 (PmcBaseAddr + 0x04, (BIT2 | BIT1 | BIT0));

  IpcBaseAddr = MmioRead32 (PmcBaseAddr + R_PMC_IPC1_BASE);
  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (PmcBaseAddr + R_PMC_IPC1_BASE),
    1,
    &IpcBaseAddr
    );

  DEBUG ((DEBUG_INFO, "PmcPciEnumeratedCompleteNotify IpcBaseAddr = 0x%x.\n", IpcBaseAddr));

  gBS->CloseEvent (Event);
}


/**
  Register notification function for PM configuration after PCI enumeration

  @param[in]  ScPolicy                      The SC Policy instance

  @retval     EFI_SUCCESS                   The function completed successfully

**/
EFI_STATUS
ConfigurePmcAfterPciEnum (
  IN     SC_POLICY_HOB                  *ScPolicy
  )
{
  EFI_EVENT         PCIEnumerationCompleteEvent = NULL;
  VOID              *Registration;
  EFI_STATUS        Status;

  DEBUG ((DEBUG_INFO, "ConfigurePmcAfterPciEnum () Start\n"));

  //
  // Register InternalIpcLibPciEnumeratedNotifyEvent() notify function
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PmcPciEnumeratedCompleteNotify,
                  NULL,
                  NULL,
                  &PCIEnumerationCompleteEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  PCIEnumerationCompleteEvent,
                  &Registration
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "ConfigurePmcAfterPciEnum () End\n"));

  return EFI_SUCCESS;
}

