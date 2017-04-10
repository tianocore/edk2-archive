/** @file
  Perform related functions for SC Sata in DXE phase.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <ScInit.h>

/**
  Perform the remaining configuration on SC SATA to perform device detection
  at end of Dxe, then set the SATA SPD and PxE corresponding, and set the Register Lock

  @param[in] ScPolicy                   The SC Policy instance

  @retval    EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
ConfigureSataDxe (
  IN SC_POLICY_HOB     *ScPolicy
  )
{
  EFI_STATUS     Status;
  SC_SATA_CONFIG *SataConfig;
  UINTN          PciSataRegBase;
  UINT16         SataPortsEnabled;
  UINT32         DwordReg;
  UINTN          Index;

  DEBUG ((DEBUG_INFO, "ConfigureSataDxe() Start\n"));
  Status = GetConfigBlock ((VOID *) ScPolicy, &gSataConfigGuid, (VOID *) &SataConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // SATA PCS: Enable the port in any of below condition:
  // i.)   Hot plug is enabled
  // ii.)  A device is attached
  // iii.) Test mode is enabled
  // iv.)  Configured as eSATA port
  //
  PciSataRegBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SATA,
                     PCI_FUNCTION_NUMBER_SATA
                     );
  SataPortsEnabled = 0;
  DwordReg = MmioRead32 (PciSataRegBase + 0x94);
  DEBUG ((DEBUG_INFO, "PCS = 0x%x\n", DwordReg));
  for (Index = 0; Index < SC_MAX_SATA_PORTS; Index++) {
    if ((SataConfig->PortSettings[Index].HotPlug == TRUE) ||
        (DwordReg & (BIT16 << Index)) ||
        (SataConfig->TestMode == TRUE) ||
        (SataConfig->PortSettings[Index].External == TRUE)) {
      SataPortsEnabled |= (SataConfig->PortSettings[Index].Enable << Index);
    }
  }
  DEBUG ((DEBUG_INFO, "SataPortsEnabled = 0x%x\n", SataPortsEnabled));

  //
  // MAP - Port Mapping Register
  // PCI Offset:   90h - 93h
  // Value [23:16] Port Disabled
  //       [7:0]   Port Clock Disabled
  //
  MmioOr32 (PciSataRegBase + R_SATA_MAP, (UINT32) ((~SataPortsEnabled << N_SATA_MAP_SPD) & B_SATA_MAP_SPD));
  DEBUG ((DEBUG_INFO, "MAP = 0x%x\n", MmioRead32 (PciSataRegBase + R_SATA_MAP)));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PciSataRegBase + R_SATA_MAP),
    1,
    (VOID *) (UINTN) (PciSataRegBase + R_SATA_MAP)
    );

  MmioOr16 (PciSataRegBase + 0x94, SataPortsEnabled);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (PciSataRegBase + 0x94),
    1,
    (VOID *) (UINTN) (PciSataRegBase + 0x94)
    );

  //
  // Step 14
  // Program SATA PCI offset 9Ch [31] to 1b
  //
  MmioOr32 ((UINTN) (PciSataRegBase + R_SATA_SATAGC), BIT31);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PciSataRegBase + R_SATA_SATAGC),
    1,
    (VOID *) (UINTN) (PciSataRegBase + R_SATA_SATAGC)
    );
  DEBUG ((DEBUG_INFO, "SATAGC = 0x%x\n", MmioRead32 (PciSataRegBase + R_SATA_SATAGC)));
  DEBUG ((DEBUG_INFO, "ConfigureSataDxe() End\n"));

  return EFI_SUCCESS;
}

