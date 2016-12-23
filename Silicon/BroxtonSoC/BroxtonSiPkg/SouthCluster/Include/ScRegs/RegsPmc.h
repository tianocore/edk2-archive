/** @file
  Register names for PMC device.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CHV_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_PCH_" without <generation_name> inserted.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REGS_PMC_H_
#define _REGS_PMC_H_

///
/// PMC Registers
///
#define PMC_BASE_ADDRESS                    PcdGet32 (PcdPmcGcrBaseAddress)
#define GCR_BASE_ADDRESS                    PcdGet32 (PcdPmcGcrBaseAddress)

#define PCI_DEVICE_NUMBER_PMC               13
#define PCI_FUNCTION_NUMBER_PMC_SSRAM       3

///
/// PMC LPC1 Controller Registers (D13:F1)
///
#define PCI_DEVICE_NUMBER_PMC_IPC1          PCI_DEVICE_NUMBER_PMC
#define PCI_FUNCTION_NUMBER_PMC_IPC1        1
#define R_PMC_IPC1_BASE                     0x10  ///< BAR0
#define R_PMC_ACPI_BASE                     0x20  ///< BAR2
#define PMC_GCR_GEN_PMCON1                  0x20

///
/// PMC PWM Modules
/// PCI Config Space Registers
///
#define PCI_DEVICE_NUMBER_PMC_PWM           26
#define PCI_FUNCTION_NUMBER_PMC_PWM         0

#define B_PMC_IOSF2OCP_PCICFGCTRL3_BAR1_DISABLE3  BIT7


#endif

