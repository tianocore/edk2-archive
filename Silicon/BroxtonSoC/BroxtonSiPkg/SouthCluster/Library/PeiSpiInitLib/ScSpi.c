/** @file
  SC SPI PEI Library implements the SPI Host Controller Compatibility Interface.

  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSpi.h"

/**
  Hide SPI controller before OS avoid BAR0 changed.

  @param[in]  None

  @retval
**/
VOID
HideSpiController (
  VOID
  )
{
  UINTN                                 ScSpiBase;

  ScSpiBase = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_SC,
                PCI_DEVICE_NUMBER_SPI,
                PCI_FUNCTION_NUMBER_SPI
                );

  MmioAndThenOr8 (
    ScSpiBase + R_SPI_BCR + 1,
    (UINT8)~(B_SPI_BCR_SYNC_SS >> 8),
    (B_SPI_BC_OSFH >> 8)
    );
}

/**
  PCI Enumeration is not done till later in DXE.
  Initialize SPI BAR0 to a default value till enumeration is done.
  Also enable memory space decoding for SPI.

  @param[in]  None

  @retval

**/
VOID
InitSpiBar0 (
  VOID
  )
{
  UINTN ScSpiBase;
  ScSpiBase = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_SC,
                 PCI_DEVICE_NUMBER_SPI,
                 PCI_FUNCTION_NUMBER_SPI
                 );
  MmioWrite32 (ScSpiBase + R_SPI_BASE, SC_SPI_BASE_ADDRESS);
  MmioOr32 (ScSpiBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
}

/**
  Installs SC SPI PPI

  @retval EFI_SUCCESS             SC SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
EFIAPI
InstallScSpi (
  VOID
  )
{
  EFI_STATUS                  Status;
  PEI_SPI_INSTANCE            *PeiSpiInstance;
  SPI_INSTANCE                *SpiInstance;
  EFI_PEI_PPI_DESCRIPTOR      *OldScSpiPolicyPpiDesc;

  DEBUG ((DEBUG_INFO, "InstallScSpi() Start\n"));

  ///
  /// PCI Enumeration is not done till later in DXE.
  /// Initialize SPI BAR0 to a default value till enumeration is done.
  /// Also enable memory space decoding for SPI.
  ///
  InitSpiBar0 ();

  PeiSpiInstance = (PEI_SPI_INSTANCE *) AllocateZeroPool (sizeof (PEI_SPI_INSTANCE));
  if (PeiSpiInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SpiInstance = &(PeiSpiInstance->SpiInstance);
  SpiProtocolConstructor (SpiInstance);

  PeiSpiInstance->PpiDescriptor.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiSpiInstance->PpiDescriptor.Guid  = &gScSpiPpiGuid;
  PeiSpiInstance->PpiDescriptor.Ppi   = &(SpiInstance->SpiProtocol);

  Status = PeiServicesLocatePpi (
             &gScSpiPpiGuid,
             0,
             &OldScSpiPolicyPpiDesc,
             NULL
             );
  if (EFI_ERROR (Status)) {
    //
    // Install Sc Spi PPI.
    //
    DEBUG ((DEBUG_INFO, "Locate Old ScSpiPpi fail in Post-Memory\n"));
    Status = PeiServicesInstallPpi (&PeiSpiInstance->PpiDescriptor);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Install ScSpiPpi fail in Post-Memory\n"));
    }
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // ReInstall Sc Spi PPI.
    //
    DEBUG ((DEBUG_INFO, "Re-Install ScSpiPpi in Post-Memory\n"));
    Status = PeiServicesReInstallPpi (OldScSpiPolicyPpiDesc, &PeiSpiInstance->PpiDescriptor);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Re-Install ScSpiPpi fail in Post-Memory\n"));
    }
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "InstallScSpi() End\n"));

  //
  // Hide SPI controller before OS avoid BAR0 changed.
  //
  HideSpiController ();

  return Status;
}

/**
  Acquire SC SPI mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval ScSpiBar0               Return SPI MMIO address
**/
UINT32
AcquireSpiBar0 (
  IN     SPI_INSTANCE       *SpiInstance
  )
{
  return MmioRead32 (SpiInstance->PchSpiBase + R_SPI_BASE) & ~(B_SPI_BAR0_MASK);
}

/**
  Release SC SPI mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval None
**/
VOID
ReleaseSpiBar0 (
  IN     SPI_INSTANCE       *SpiInstance
  )
{
}

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  )
{
  UINTN             SpiBaseAddress;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SPI,
                     PCI_FUNCTION_NUMBER_SPI
                     );
  if ((MmioRead8 (SpiBaseAddress + R_SPI_BCR) & B_SPI_BCR_SMM_BWP) != 0) {
    return EFI_ACCESS_DENIED;
  }
  
  ///
  /// Enable the access to the BIOS space for both read and write cycles
  ///
  MmioOr8 (
    SpiBaseAddress + R_SPI_BCR,
    B_SPI_BCR_BIOSWE
    );

  return EFI_SUCCESS;
}

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @param[in]  None

  @retval
**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  )
{
  UINTN                           SpiBaseAddress;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SPI,
                     PCI_FUNCTION_NUMBER_SPI
                     );
  ///
  /// Disable the access to the BIOS space for write cycles
  ///
  MmioAnd8 (
    SpiBaseAddress + R_SPI_BCR,
    (UINT8) (~B_SPI_BCR_BIOSWE)
    );
}
