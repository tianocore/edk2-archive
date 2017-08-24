
/** @file
  Macros that simplify accessing I2C device's registers.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_ACCESS_H_
#define _I2C_ACCESS_H_

///
/// Memory Mapped PCI Access macros
///

#include "I2CIoLibPei.h"

#define DEFAULT_PCI_BUS_NUMBER_SC   0

#define PCI_DEVICE_NUMBER_LPC       31
#define PCI_FUNCTION_NUMBER_LPC     0

#define R_LPC_ACPI_BASE             0x40        ///< ABASE, 16bit
#define R_LPC_ACPI_BASEADR          0x400       ///< ABASE, 16bit
#define B_LPC_ACPI_BASE_EN          BIT1        ///< Enable Bit
#define B_LPC_ACPI_BASE_BAR         0x0000FF80  ///< Base Address, 128 Bytes
#define V_ACPI_PM1_TMR_MAX_VAL      0x1000000   ///< The timer is 24 bit overflow
#define B_ACPI_PM1_TMR_VAL          0xFFFFFF    ///< The timer value mask

#define R_ACPI_PM1_TMR              0x08        ///< Power Management 1 Timer
#define V_ACPI_PM1_TMR_FREQUENCY    3579545     ///< Timer Frequency


#define ScLpcPciCfg8(Register)      I2CLibPeiMmioRead8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPC, 0, Register))

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PcdGet64 (PcdPciExpressBaseAddress)+ \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )
#endif
