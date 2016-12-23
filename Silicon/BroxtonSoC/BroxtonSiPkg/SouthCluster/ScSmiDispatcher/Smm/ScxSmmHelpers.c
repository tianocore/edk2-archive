/** @file
  This driver is responsible for the registration of child drivers
  and the abstraction of the SC SMI sources.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"

//
// Help handle porting bit shifts to IA-64.
//
#define BIT_ZERO  0x00000001

/**
  Publish SMI Dispatch protocols.

**/
VOID
ScSmmPublishDispatchProtocols (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  UINTN      Index;
  //
  // Install protocol interfaces.
  //
  for (Index = 0; Index < ScSmmProtocolTypeMax; Index++) {
    Status = gSmst->SmmInstallProtocolInterface (
                      &mPrivateData.InstallMultProtHandle,
                      mPrivateData.Protocols[Index].Guid,
                      EFI_NATIVE_INTERFACE,
                      &mPrivateData.Protocols[Index].Protocols.Generic
                      );
  }
  ASSERT_EFI_ERROR (Status);
}


/**
  Initialize bits that aren't necessarily related to an SMI source.

  @retval EFI_SUCCESS             SMI source initialization completed.
  @retval Asserts                 Global Smi Bit is not enabled successfully.

**/
EFI_STATUS
ScSmmInitHardware (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Clear all SMIs
  //
  ScSmmClearSmi ();

  Status = ScSmmEnableGlobalSmiBit ();
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  Enables the SC to generate SMIs. Note that no SMIs will be generated
  if no SMI sources are enabled. Conversely, no enabled SMI source will
  generate SMIs if SMIs are not globally enabled. This is the main
  switchbox for SMI generation.

  @retval EFI_SUCCESS             Enable Global Smi Bit completed

**/
EFI_STATUS
ScSmmEnableGlobalSmiBit (
  VOID
  )
{
  UINT32  SmiEn;

  SmiEn = IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_EN));

  //
  // Set the "global smi enable" bit
  //
  SmiEn |= B_SMI_EN_GBL_SMI;
  IoWrite32 ((UINTN) (AcpiBaseAddr + R_SMI_EN), SmiEn);

  return EFI_SUCCESS;
}


/**
  Clears the SMI after all SMI source have been processed.
  Note that this function will not work correctly (as it is
  written) unless all SMI sources have been processed.
  A revision of this function could manually clear all SMI
  status bits to guarantee success.

  @retval  EFI_SUCCESS             Clears the SMIs completed
  @retval  Asserts                 EOS was not set to a 1

**/
EFI_STATUS
ScSmmClearSmi (
  VOID
  )
{
  BOOLEAN   EosSet;
  BOOLEAN   SciEn;
  UINT32    Pm1Cnt;
  UINT16    Pm1Sts;
  UINT32    Gpe0aStsLow;
  UINT32    SmiSts;
  UINT32    TcoSts;

  //
  // Determine whether an ACPI OS is present (via the SCI_EN bit)
  //
  Pm1Cnt  = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT));
  SciEn   = (BOOLEAN) ((Pm1Cnt & B_ACPI_PM1_CNT_SCI_EN) == B_ACPI_PM1_CNT_SCI_EN);
  if (!SciEn) {
    //
    // Clear any SMIs that double as SCIs (when SCI_EN==0)
    //
    Pm1Sts        = IoRead16 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_STS));
    Gpe0aStsLow   = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_GPE0a_STS));

    Pm1Sts |=
      (
       B_ACPI_PM1_STS_WAK |
#ifdef PCIESC_SUPPORT
       B_ACPI_PM1_STS_WAK_PCIE0 |
#endif
       B_ACPI_PM1_STS_PRBTNOR |
       B_ACPI_PM1_STS_RTC |
       B_ACPI_PM1_STS_PWRBTN |
       B_ACPI_PM1_STS_GBL |
#ifdef PCIESC_SUPPORT
       B_ACPI_PM1_STS_WAK_PCIE3 |
       B_ACPI_PM1_STS_WAK_PCIE2 |
       B_ACPI_PM1_STS_WAK_PCIE1 |
#endif
       B_ACPI_PM1_STS_TMROF
       );

    Gpe0aStsLow |=
      (
       B_ACPI_GPE0a_STS_PME_B0 |
       B_ACPI_GPE0a_STS_BATLOW |
       B_ACPI_GPE0a_STS_PCI_EXP |
       B_ACPI_GPE0a_STS_GUNIT_SCI |
       B_ACPI_GPE0a_STS_PUNIT_SCI |
       B_ACPI_GPE0a_STS_SWGPE |
       B_ACPI_GPE0a_STS_HOT_PLUG
       );

    IoWrite16 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_STS), (UINT16) Pm1Sts);
    IoWrite32 ((UINTN) (AcpiBaseAddr + R_ACPI_GPE0a_STS), (UINT32) Gpe0aStsLow);
  }

  //
  // Clear all SMIs that are unaffected by SCI_EN
  //
  SmiSts = IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_STS));
  TcoSts = IoRead32 ((UINTN) (AcpiBaseAddr + R_TCO_STS));

  SmiSts |=
    (
     B_SMI_STS_PERIODIC |
     B_SMI_STS_TCO |
     B_SMI_STS_SWSMI_TMR |
     B_SMI_STS_APM |
     B_SMI_STS_ON_SLP_EN |
     B_SMI_STS_BIOS
     );

  TcoSts |=
    (
     B_TCO_STS_SECOND_TO |
     B_TCO_STS_TIMEOUT
     );

  GpioClearAllGpiSmiSts ();
  IoWrite32 ((UINTN) (AcpiBaseAddr + R_SMI_STS), SmiSts);

  //
  // Try to clear the EOS bit. ASSERT on an error
  //
  EosSet = ScSmmSetAndCheckEos ();
  ASSERT (EosSet);

  return EFI_SUCCESS;
}


/**
  Set the SMI EOS bit after all SMI source have been processed.

  @retval FALSE                   EOS was not set to a 1; this is an error
  @retval TRUE                    EOS was correctly set to a 1

**/
BOOLEAN
ScSmmSetAndCheckEos (
  VOID
  )
{
  UINT32  SmiEn;

  SmiEn = IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_EN));

  //
  // Reset the SC to generate subsequent SMIs
  //
  SmiEn |= B_SMI_EN_EOS;
  IoWrite32 ((UINTN) (AcpiBaseAddr + R_SMI_EN), SmiEn);

  //
  // Double check that the assert worked
  //
  SmiEn = IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_EN));

  //
  // Return TRUE if EOS is set correctly
  //
  if ((SmiEn & B_SMI_EN_EOS) == 0) {
    //
    // EOS was not set to a 1; this is an error
    //
    return FALSE;
  } else {
    //
    // EOS was correctly set to a 1
    //
    return TRUE;
  }
}


/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present

**/
BOOLEAN
ScSmmGetSciEn (
  VOID
  )
{
  BOOLEAN    SciEn;
  UINT32     Pm1Cnt;

  //
  // Determine whether an ACPI OS is present (via the SCI_EN bit)
  //
  Pm1Cnt  = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT));
  SciEn   = (BOOLEAN) ((Pm1Cnt & B_ACPI_PM1_CNT_SCI_EN) == B_ACPI_PM1_CNT_SCI_EN);

  return SciEn;
}


/**
  Read a specifying bit with the register
  These may or may not need to change w/ the SC version; they're highly IA-32 dependent, though.

  @param[in] BitDesc                 The struct that includes register address, size in byte and bit number

  @retval    TRUE                    The bit is enabled
  @retval    FALSE                   The bit is disabled

**/
BOOLEAN
ReadBitDesc (
  IN CONST SC_SMM_BIT_DESC           *BitDesc
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
#ifdef PCIESC_SUPPORT
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
#endif
  UINTN       RegSize;
  BOOLEAN     BitWasOne;
  UINTN       ShiftCount;
  UINTN       RegisterOffset;

  if ((BitDesc == NULL) || IS_BIT_DESC_NULL (*BitDesc)) {
    ASSERT (FALSE);
    return FALSE;
  }

  RegSize     = 0;
  Register    = 0;
  ShiftCount  = 0;
  BitWasOne   = FALSE;

  switch (BitDesc->Reg.Type) {
    case ACPI_ADDR_TYPE:
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          RegSize = SMM_IO_UINT8;
          break;

        case 2:
          RegSize = SMM_IO_UINT16;
          break;

        case 4:
          RegSize = SMM_IO_UINT32;
          break;

        case 8:
          RegSize = SMM_IO_UINT64;
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      //
      // Double check that we correctly read in the ACPI base address
      //
      ASSERT ((AcpiBaseAddr != 0x0));

      RegisterOffset  = BitDesc->Reg.Data.acpi;
      ShiftCount      = BitDesc->Bit;

      //
      // As current CPU Smm Io can only support at most
      // 32-bit read/write,if Operation is 64 bit,
      // we do a 32 bit operation according to BitDesc->Bit
      //
      if (RegSize == SMM_IO_UINT64) {
        RegSize = SMM_IO_UINT32;
        //
        // If the operation is for high 32 bits
        //
        if (BitDesc->Bit >= 32) {
          RegisterOffset += 4;
          ShiftCount -= 32;
        }
      }

      Status = gSmst->SmmIo.Io.Read (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 AcpiBaseAddr + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);

      if ((Register & (LShiftU64 (BIT_ZERO, ShiftCount))) != 0) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;

    case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    case GPIO_ADDR_TYPE:
      //
      // Read the register, and it with the bit to read
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 2:
          Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 4:
          Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 8:
          Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      Register = Register & (LShiftU64 (BIT0, BitDesc->Bit));
      if (Register) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;

#ifdef PCIESC_SUPPORT
    case PCIE_ADDR_TYPE:
      PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
      PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
      PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
      PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          Register = (UINT64) MmioRead8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        case 2:
          Register = (UINT64) MmioRead16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        case 4:
          Register = (UINT64) MmioRead32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      if ((Register & (LShiftU64 (BIT_ZERO, BitDesc->Bit))) != 0) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;
#endif
    case PCR_ADDR_TYPE:
      //
      // Read the register, and it with the bit to read
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          PchPcrRead8 (BitDesc->Reg.Data.Pcr.Fields.Pid, BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT8 *)  &Register);
          break;

        case 2:
          PchPcrRead16 (BitDesc->Reg.Data.Pcr.Fields.Pid, BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT16 *) &Register);
          break;

        case 4:
          PchPcrRead32 (BitDesc->Reg.Data.Pcr.Fields.Pid, BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT32 *) &Register);
          break;

        default:
        //
        // Unsupported or invalid register size
        //
        ASSERT (FALSE);
        break;
      }

      Register = Register & (LShiftU64 (BIT0, BitDesc->Bit));
      if (Register) {
        BitWasOne = TRUE;
      } else {
        BitWasOne = FALSE;
      }
      break;
    default:
      //
      // This address type is not yet implemented
      //
      ASSERT (FALSE);
      break;
  }

  return BitWasOne;
}


/**
  Write a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be wrote
  @param[in] WriteClear           If the rest bits of the register is write clear

  @retval    None

**/
VOID
WriteBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc,
  IN CONST BOOLEAN                ValueToWrite,
  IN CONST BOOLEAN                WriteClear
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
  UINT64      AndVal;
  UINT64      OrVal;
  UINT32      RegSize;
#ifdef PCIESC_SUPPORT
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
#endif
  UINTN       RegisterOffset;

  ASSERT (BitDesc != NULL);

  if ((BitDesc == NULL) || IS_BIT_DESC_NULL (*BitDesc)) {
    ASSERT (FALSE);
    return;
  }

  RegSize   = 0;
  Register  = 0;

  if (WriteClear) {
    AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit);
  } else {
    AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit));
  }

  OrVal = (LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit));

  switch (BitDesc->Reg.Type) {

    case ACPI_ADDR_TYPE:
      switch (BitDesc->SizeInBytes) {
        case 0:
          //
          // Chances are that this field didn't get initialized.
          // Check your assignments to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          RegSize = SMM_IO_UINT8;
          break;

        case 2:
          RegSize = SMM_IO_UINT16;
          break;

        case 4:
          RegSize = SMM_IO_UINT32;
          break;

        case 8:
          RegSize = SMM_IO_UINT64;
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      //
      // Double check that we correctly read in the ACPI base address
      //
      ASSERT ((AcpiBaseAddr != 0x0));

      RegisterOffset = BitDesc->Reg.Data.acpi;

      //
      // As current CPU Smm Io can only support at most
      // 32-bit read/write,if Operation is 64 bit,
      // we do a 32 bit operation according to BitDesc->Bit
      //
      if (RegSize == SMM_IO_UINT64) {
        RegSize = SMM_IO_UINT32;

        //
        // If the operation is for high 32 bits
        //
        if (BitDesc->Bit >= 32) {
          RegisterOffset += 4;

          if (WriteClear) {
            AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit - 32);
          } else {
            AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit - 32));
          }

          OrVal = LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit - 32);
        }
      }

      Status = gSmst->SmmIo.Io.Read (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 AcpiBaseAddr + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);

      Register &= AndVal;
      Register |= OrVal;

      Status = gSmst->SmmIo.Io.Write (
                                 &gSmst->SmmIo,
                                 RegSize,
                                 AcpiBaseAddr + RegisterOffset,
                                 1,
                                 &Register
                                 );
      ASSERT_EFI_ERROR (Status);
      break;

    case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    case GPIO_ADDR_TYPE:
      //
      // Read the register, or it with the bit to set, then write it back.
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 2:
          Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 4:
          Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          break;

        case 8:
          Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
          *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }

      Register &= AndVal;
      Register |= OrVal;
      //
      // Read the register, or it with the bit to set, then write it back.
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          MmioWrite8 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT8) Register);
          break;

        case 2:
          MmioWrite16 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT16) Register);
          break;

        case 4:
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
          break;

        case 8:
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
          MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio + 4, *((UINT32 *) (&Register) + 1));
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      break;

#ifdef PCIESC_SUPPORT
    case PCIE_ADDR_TYPE:
      PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
      PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
      PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
      PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
      switch (BitDesc->SizeInBytes) {

        case 0:
          //
          // Chances are that this field didn't get initialized -- check your assignments
          // to bit descriptions.
          //
          ASSERT (FALSE);
          break;

        case 1:
          MmioAndThenOr8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT8) AndVal, (UINT8) OrVal);
          break;

        case 2:
          MmioAndThenOr16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT16) AndVal, (UINT16) OrVal);
          break;

        case 4:
          MmioAndThenOr32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT32) AndVal, (UINT32) OrVal);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      break;
#endif
    case PCR_ADDR_TYPE:
      //
      // Read the register, or it with the bit to set, then write it back.
      //
      switch (BitDesc->SizeInBytes) {
        case 1:
          PchPcrAndThenOr8  ((SC_SBI_PID) BitDesc->Reg.Data.Pcr.Fields.Pid, (UINT16) BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT8) AndVal, (UINT8) OrVal);
          break;

        case 2:
          PchPcrAndThenOr16 ((SC_SBI_PID) BitDesc->Reg.Data.Pcr.Fields.Pid, (UINT16) BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT16) AndVal, (UINT16) OrVal);
          break;

        case 4:
          PchPcrAndThenOr32 ((SC_SBI_PID) BitDesc->Reg.Data.Pcr.Fields.Pid, (UINT16) BitDesc->Reg.Data.Pcr.Fields.Offset, (UINT32) AndVal, (UINT32) OrVal);
          break;

        default:
          //
          // Unsupported or invalid register size
          //
          ASSERT (FALSE);
          break;
      }
      break;
    default:
      //
      // This address type is not yet implemented
      //
      ASSERT (FALSE);
      break;
  }
}


/**
  Specific programming done before exiting SMI

**/
VOID
ScBeforeExitSmi (
  )
{

  if (BxtStepping () >= BxtPB0) {
    if ((AsmReadMsr64(0x8b) >> 32) >= 8) {
      UINT32 SpiBar0 = 0;

      SpiBar0 = (MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI, R_SPI_BASE))) & B_SPI_BASE_BAR;
      SpiBar0 |= BIT0; // Set MSR valid
      AsmWriteMsr32 (0x124, SpiBar0);
    }
  }
}

