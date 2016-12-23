/** @file
  Early SC platform initialization.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/PcdLib.h>
#include <Library/PciExpressLib.h>
#include <Library/PeiScPolicyLib.h>
#include <Library/SideBandLib.h>
#include <Library/PeiScPolicyUpdateLib.h>
#include <Library/SteppingLib.h>
#include <Library/ScPlatformLib.h>
#include "PlatformInit.h"
#include <Ppi/ScPolicy.h>
#include <ScAccess.h>
#include <PlatformBaseAddresses.h>
#include <Ppi/ScPcieDeviceTable.h>
#include <Ppi/SiPolicyPpi.h>
#include <Library/MmPciLib.h>


VOID
ScPolicySetupInit (
  IN CONST EFI_PEI_SERVICES **PeiServices
  );
#ifndef __GNUC__
#pragma warning (push)
#pragma warning (disable : 4245)
#pragma warning (pop)
#endif


UINT8
ReadCmosBank1Byte (
  IN UINT8                      Address
  )
{
  UINT8                           Data;

  IoWrite8 (R_RTC_EXT_INDEX, Address);
  Data = IoRead8 (R_RTC_EXT_TARGET);

  return Data;
}


VOID
WriteCmosBank1Byte (
  IN UINT8                     Address,
  IN UINT8                     Data
  )
{
  IoWrite8 (R_RTC_EXT_INDEX, Address);
  IoWrite8 (R_RTC_EXT_TARGET, Data);
}


EFI_STATUS
InstallPeiScUsbPolicy (
  IN CONST EFI_PEI_SERVICES **PeiServices
  );


VOID
CheckPowerOffNow (
  VOID
  )
{
  UINT16  Pm1Sts;
  UINT16  AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Read and check the ACPI registers
  //
  Pm1Sts = IoRead16 (AcpiBaseAddr + R_ACPI_PM1_STS);
  if ((Pm1Sts & B_ACPI_PM1_STS_PWRBTN) == B_ACPI_PM1_STS_PWRBTN) {
    IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_STS, B_ACPI_PM1_STS_PWRBTN);
    IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, V_ACPI_PM1_CNT_S5);
    IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_CNT, V_ACPI_PM1_CNT_S5 + B_ACPI_PM1_CNT_SLP_EN);
  }
}


VOID
ClearPowerState (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  )
{
  UINT16  Data16;
  UINT32  Data32;
  UINT16  AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Clear the GPE and PM enable
  //
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_EN, (UINT16) 0x00);
  IoWrite32 (AcpiBaseAddr + R_ACPI_GPE0a_EN, (UINT32) 0x00);

  //
  // Halt the TCO timer
  //
  Data16 = IoRead16 (AcpiBaseAddr + R_TCO_CNT);
  Data16 |= B_TCO_CNT_TMR_HLT;
  IoWrite16 (AcpiBaseAddr + R_TCO_CNT, Data16);

  //
  // NMI NOW bit is "Write '1' to clear"
  //

  //
  // Before we clear the TO status bit here we need to save the results in a CMOS bit for later use.
  //
  Data32 = IoRead16 (AcpiBaseAddr + R_TCO_STS);
  if ((Data32 & B_TCO_STS_SECOND_TO) == B_TCO_STS_SECOND_TO) {
#if (defined(HW_WATCHDOG_TIMER_SUPPORT) && (HW_WATCHDOG_TIMER_SUPPORT != 0))
    WriteCmosBank1Byte (
      EFI_CMOS_PERFORMANCE_FLAGS,
      ReadCmosBank1Byte (EFI_CMOS_PERFORMANCE_FLAGS) | B_CMOS_TCO_WDT_RESET
      );
#endif
  }

}


/**
  Clear any SMI status or wake status left over from boot.

**/
VOID
ClearSmiAndWake (
  VOID
  )
{
  UINT16  Pm1Sts;
  UINT32  Gpe0Sts;
  UINT32  SmiSts;
  UINT16  AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Read the ACPI registers
  //
  Pm1Sts  = IoRead16 (AcpiBaseAddr + R_ACPI_PM1_STS);
  Gpe0Sts = IoRead32 (AcpiBaseAddr + R_ACPI_GPE0a_STS);
  SmiSts  = IoRead32 (AcpiBaseAddr + R_SMI_STS);

  //
  // Register Wake up reason for S4.  This information is used to notify
  // WinXp of wake up reason because S4 wake up path doesn't keep SCI.
  // This is important for Viiv(Quick resume) platform.
  //

  //
  // First Clear CMOS S4 Wake up flag.
  //
  WriteCmosBank1Byte (EFI_CMOS_S4_WAKEUP_FLAG_ADDRESS, 0);

  //
  // Check wake up reason and set CMOS accordingly.  Currently checks
  // Power button, USB, PS/2.
  // Note : PS/2 wake up is using GPI13 (IO_PME).  This must be changed depending
  // on board design.
  //
  if ((Pm1Sts & B_ACPI_PM1_STS_PWRBTN) || (Gpe0Sts & (B_ACPI_GPE0a_STS_CORE_GPIO | B_ACPI_GPE0a_STS_SUS_GPIO))) {
    WriteCmosBank1Byte (EFI_CMOS_S4_WAKEUP_FLAG_ADDRESS, 1);
  }

  //
  // Clear any SMI or wake state from the boot
  //
  Pm1Sts = (B_ACPI_PM1_STS_PRBTNOR | B_ACPI_PM1_STS_PWRBTN);

  Gpe0Sts |=
    (
      B_ACPI_GPE0a_STS_CORE_GPIO |
      B_ACPI_GPE0a_STS_SUS_GPIO |
      B_ACPI_GPE0a_STS_PME_B0 |
      B_ACPI_GPE0a_STS_BATLOW |
      B_ACPI_GPE0a_STS_PCI_EXP |
      B_ACPI_GPE0a_STS_GUNIT_SCI |
      B_ACPI_GPE0a_STS_PUNIT_SCI |
      B_ACPI_GPE0a_STS_SWGPE |
      B_ACPI_GPE0a_STS_HOT_PLUG
    );

  SmiSts |=
    (
      B_SMI_STS_PERIODIC |
      B_SMI_STS_TCO |
      B_SMI_STS_SWSMI_TMR |
      B_SMI_STS_APM |
      B_SMI_STS_ON_SLP_EN |
      B_SMI_STS_BIOS
    );

  //
  // Write them back
  //
  IoWrite16 (AcpiBaseAddr + R_ACPI_PM1_STS, Pm1Sts);
  IoWrite32 (AcpiBaseAddr + R_ACPI_GPE0a_STS, Gpe0Sts);
  IoWrite32 (AcpiBaseAddr + R_SMI_STS, SmiSts);
}


/**
  Provide hard reset PPI service.
  To generate full hard reset, write 0x0E to ICH RESET_GENERATOR_PORT (0xCF9).

  @param[in]  PeiServices       General purpose services available to every PEIM.

  @retval     Not return        System reset occurred.
  @retval     EFI_DEVICE_ERROR  Device error, could not reset the system.

**/
EFI_STATUS
EFIAPI
IchReset (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  IoWrite8 (
    R_RST_CNT,
    V_RST_CNT_HARDSTARTSTATE
    );

  IoWrite8 (
    R_RST_CNT,
    V_RST_CNT_HARDRESET
    );

  //
  // System reset occurred, should never reach at this line.
  //
  ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);

  return EFI_DEVICE_ERROR;
}


EFI_STATUS
PlatformScInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration,
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINT16                      PlatformType
  )
{
  BXT_SERIES BxtSeries;
  //
  // SC Policy Initialization based on Setup variable.
  //
  ScPolicySetupInit (PeiServices);

  //
  // Install PCIe device override table
  //
  BxtSeries = GetBxtSeries ();
  if (BxtSeries == BxtP) {
    //
    // Set TCO Base Address
    //
    SetTcoBase (PcdGet16 (PcdTcoBaseAddress));
  }

  return EFI_SUCCESS;
}


VOID
ScPolicySetupInit (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS                  Status;
  SC_POLICY_PPI               *ScPolicyPpi;

  //
  // Install SC Policy PPI. As we depend on SC Init PPI so we are executed after
  // ScInit PEIM. Thus we can insure SC Initialization is performed when we install the SC Policy PPI,
  // as ScInit PEIM registered a notification function on our policy PPI.
  //
  // For better code structure / modularity, we should use a notification function on SC Init PPI to perform
  // actions that depend on ScInit PEIM's initialization.
  //
  DEBUG ((EFI_D_INFO, "ScPolicySetupInit() - Start\n"));
  ScCreateConfigBlocks (&ScPolicyPpi);

  UpdatePeiScPolicy (ScPolicyPpi);

  //
  // Install SC Policy PPI
  //
  Status = ScInstallPolicyPpi (ScPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "ScPolicySetupInit() - End\n"));
}

