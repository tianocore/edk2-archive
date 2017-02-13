/** @file
  SC SPI SMM Driver implements the SPI Host Controller Compatibility Interface.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSpi.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED SPI_INSTANCE          *mSpiInstance;

///
/// mPchSpiResvMmioAddr keeps the reserved MMIO range assiged to SPI.
/// In SMM it always set back the reserved MMIO address to SPI BAR0 to ensure the MMIO range
/// won't overlap with SMRAM range, and trusted.
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                mSpiResvMmioAddr;

/**
  SPI Runtime SMM Module Entry Point.

  Introduction:
    The SPI SMM module provide a standard way for other modules to use the PCH SPI Interface in SMM.

  Pre:
    EFI_SMM_BASE2_PROTOCOL
    Documented in System Management Mode Core Interface Specification.

  Result:
    The SPI SMM driver produces PCH_SPI_PROTOCOL with GUID
    gPchSmmSpiProtocolGuid which is different from SPI RUNTIME driver.

  Integration Check List:
    This driver supports Descriptor Mode only.
    This driver supports Hardware Sequence only.
    When using SMM SPI Protocol to perform flash access in an SMI handler,
    and the SMI occurrence is asynchronous to normal mode code execution,
    proper synchronization mechanism must be applied, e.g. disable SMI before
    the normal mode SendSpiCmd() starts and re-enable SMI after
    the normal mode SendSpiCmd() completes.
    @note The implementation of SendSpiCmd() uses GBL_SMI_EN in
    SMI_EN register (ABase + 30h) to disable and enable SMIs. But this may
    not be effective as platform may well set the SMI_LOCK bit (i.e., PMC PCI Offset A0h [4]).
    So the synchronization at caller level is likely needed.

  @param[in] ImageHandle             Image handle of this driver.
  @param[in] SystemTable             Global system service table.

  @retval    EFI_SUCCESS             Initialization complete.
  @retval    EFI_UNSUPPORTED         The chipset is unsupported by this driver.
  @retval    EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval    EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InstallScSpi (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Init PCH spi reserved MMIO address.
  //
  mSpiResvMmioAddr = SC_SPI_BASE_ADDRESS;

  //
  // Allocate pool for SPI protocol instance
  //
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (SPI_INSTANCE),
                    (VOID **) &mSpiInstance
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mSpiInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) mSpiInstance, sizeof (SPI_INSTANCE));

  //
  // Initialize the SPI protocol instance
  //
  Status = SpiProtocolConstructor (mSpiInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install the SMM SC_SPI_PROTOCOL interface
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &(mSpiInstance->Handle),
                    &gScSmmSpiProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(mSpiInstance->SpiProtocol)
                    );
  if (EFI_ERROR (Status)) {
    gSmst->SmmFreePool (mSpiInstance);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Acquire SC spi mmio address.
  It is not expected for this BAR0 to change because the SPI device is hidden
  from the OS for SKL PCH LP/H B stepping and above (refer to section 3.5.1),
  but if it is ever different from the preallocated address, reassign it back.
  In SMM, it always override the BAR0 and returns the reserved MMIO range for SPI.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval    PchSpiBar0           Return SPI MMIO address

**/
UINT32
AcquireSpiBar0 (
  IN     SPI_INSTANCE       *SpiInstance
  )
{
  UINT32                          SpiBar0;

  //
  // Save original SPI MMIO address
  //
  SpiBar0 = MmioRead32 (SpiInstance->PchSpiBase + R_SPI_BASE) & B_SPI_BASE_BAR;

  if (SpiBar0 != mSpiResvMmioAddr) {
    //
    // Temporary disable MSE, and override with SPI reserved MMIO address, then enable MSE.
    //
    MmioAnd8 (SpiInstance->PchSpiBase + PCI_COMMAND_OFFSET, (UINT8) ~EFI_PCI_COMMAND_MEMORY_SPACE);
    MmioWrite32 (SpiInstance->PchSpiBase + R_SPI_BASE, mSpiResvMmioAddr);
    MmioOr8 (SpiInstance->PchSpiBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  }

  //
  // SPIBAR0 will be different before and after PCI enum so need to get it from SPI BAR0 reg.
  //
  return mSpiResvMmioAddr;
}


/**
  Release SC spi mmio address. Do nothing.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval     None

**/
VOID
ReleaseSpiBar0 (
  IN     SPI_INSTANCE       *SpiInstance
  )
{
}


/**
  This function is a hook for Spi to disable BIOS Write Protect.

  @param[in] None

  @retval    EFI_SUCCESS             The protocol instance was properly initialized
  @retval    EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  )
{
  UINTN     SpiBaseAddress;
  UINT32    Data32;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SPI,
                     PCI_FUNCTION_NUMBER_SPI
                     );

  //
  // Set BIOSWE bit (SPI PCI Offset DCh [0]) = 1b
  // Enable the access to the BIOS space for both read and write cycles
  //
  MmioOr8 (
    SpiBaseAddress + R_SPI_BCR,
    B_SPI_BCR_BIOSWE
    );

  return EFI_SUCCESS;
}


/**
  This function is a hook for Spi to enable BIOS Write Protect.

  @param[in] None

  @retval    None

**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  )
{
  UINTN     SpiBaseAddress;
  UINT32    Data32;

  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_SPI,
                     PCI_FUNCTION_NUMBER_SPI
                     );

  //
  // Clear BIOSWE bit (SPI PCI Offset DCh [0]) = 0b
  // Disable the access to the BIOS space for write cycles
  //
  MmioAnd8 (
    SpiBaseAddress + R_SPI_BCR,
    (UINT8) (~N_SPI_BCR_BIOSWE)
    );
}

