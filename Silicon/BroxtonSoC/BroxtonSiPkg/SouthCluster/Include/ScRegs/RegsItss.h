/** @file
  Register names for Interrupt Timer Sub System (ITSS) module.
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

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REGS_ITSS_H_
#define _REGS_ITSS_H_

///
/// Interrupt Timer Sub System (ITSS) Module Registers
///
///
/// LPSS Private Sideband Registers
///
#define R_ITSS_PORT_ID              0xD0 ///< ITSS port ID

#define R_ITSS_SB_PARC              0x3100 ///< PIRQA Routing Control
#define R_ITSS_SB_PBRC              0x3101 ///< PIRQB Routing Control
#define R_ITSS_SB_PCRC              0x3102 ///< PIRQC Routing Control
#define R_ITSS_SB_PDRC              0x3103 ///< PIRQD Routing Control
#define R_ITSS_SB_PERC              0x3104 ///< PIRQE Routing Control
#define R_ITSS_SB_PFRC              0x3105 ///< PIRQF Routing Control
#define R_ITSS_SB_PGRC              0x3106 ///< PIRQG Routing Control
#define R_ITSS_SB_PHRC              0x3107 ///< PIRQH Routing Control

#define N_ITSS_SB_REN                7
#define V_ITSS_SB_REN_ENABLE        (0  <<N_ITSS_SB_REN)
#define V_ITSS_SB_REN_DISABLE       (1  <<N_ITSS_SB_REN)

#define N_ITSS_SB_IR                 0
#define V_ITSS_SB_IR_IRQ3           (3  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ3
#define V_ITSS_SB_IR_IRQ4           (4  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ4
#define V_ITSS_SB_IR_IRQ5           (5  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ5
#define V_ITSS_SB_IR_IRQ6           (6  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ6
#define V_ITSS_SB_IR_IRQ7           (7  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ7
#define V_ITSS_SB_IR_IRQ9           (9  <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ9
#define V_ITSS_SB_IR_IRQ10          (10 <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ10
#define V_ITSS_SB_IR_IRQ11          (11 <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ11
#define V_ITSS_SB_IR_IRQ12          (12 <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ12
#define V_ITSS_SB_IR_IRQ14          (14 <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ14
#define V_ITSS_SB_IR_IRQ15          (15 <<N_ITSS_SB_IR) ///< Route this PIC IRQ to APIC IRQ15

#define R_ITSS_SB_PIR0              0x3140 ///< PCI Interrupt Route 0  - Device 31 Interrupt Pin Routing
#define R_ITSS_SB_PIR1              0x3142 ///< PCI Interrupt Route 1  - Device 29 Interrupt Pin Routing
#define R_ITSS_SB_PIR2              0x3144 ///< PCI Interrupt Route 2  - Device 28 Interrupt Pin Routing
#define R_ITSS_SB_PIR3              0x3146 ///< PCI Interrupt Route 3  - Device 23 Interrupt Pin Routing
#define R_ITSS_SB_PIR4              0x3148 ///< PCI Interrupt Route 4  - Device 22 Interrupt Pin Routing
#define R_ITSS_SB_PIR5              0x314A ///< PCI Interrupt Route 5  - reserved
#define R_ITSS_SB_PIR6              0x314C ///< PCI Interrupt Route 6  - reserved
#define R_ITSS_SB_PIR7              0x314E ///< PCI Interrupt Route 7  - reserved
#define R_ITSS_SB_PIR8              0x3150 ///< PCI Interrupt Route 8  - reserved
#define R_ITSS_SB_PIR9              0x3152 ///< PCI Interrupt Route 9  - reserved
#define R_ITSS_SB_PIR10             0x3154 ///< PCI Interrupt Route 10 - reserved
#define R_ITSS_SB_PIR11             0x3156 ///< PCI Interrupt Route 11 - reserved
#define R_ITSS_SB_PIR12             0x3158 ///< PCI Interrupt Route 12 - reserved

#define N_ITSS_SB_IDR                   12                 ///< Pin INT D Routing Control
#define V_ITSS_SB_IDR_PIRQA             (0 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ A
#define V_ITSS_SB_IDR_PIRQB             (1 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ B
#define V_ITSS_SB_IDR_PIRQC             (2 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ C
#define V_ITSS_SB_IDR_PIRQD             (3 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ D
#define V_ITSS_SB_IDR_PIRQE             (4 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ E
#define V_ITSS_SB_IDR_PIRQF             (5 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ F
#define V_ITSS_SB_IDR_PIRQG             (6 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ G
#define V_ITSS_SB_IDR_PIRQH             (7 <<N_ITSS_SB_IDR)  ///< Route Pin INT D to PIC PIRQ H

#define N_ITSS_SB_ICR                   8                  ///< Pin INT C Routing Control
#define V_ITSS_SB_ICR_PIRQA             (0 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ A
#define V_ITSS_SB_ICR_PIRQB             (1 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ B
#define V_ITSS_SB_ICR_PIRQC             (2 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ C
#define V_ITSS_SB_ICR_PIRQD             (3 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ D
#define V_ITSS_SB_ICR_PIRQE             (4 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ E
#define V_ITSS_SB_ICR_PIRQF             (5 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ F
#define V_ITSS_SB_ICR_PIRQG             (6 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ G
#define V_ITSS_SB_ICR_PIRQH             (7 <<N_ITSS_SB_ICR)  ///< Route Pin INT C to PIC PIRQ H

#define N_ITSS_SB_IBR                   4                  ///< Pin INT B Routing Control
#define V_ITSS_SB_IBR_PIRQA             (0 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ A
#define V_ITSS_SB_IBR_PIRQB             (1 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ B
#define V_ITSS_SB_IBR_PIRQC             (2 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ C
#define V_ITSS_SB_IBR_PIRQD             (3 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ D
#define V_ITSS_SB_IBR_PIRQE             (4 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ E
#define V_ITSS_SB_IBR_PIRQF             (5 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ F
#define V_ITSS_SB_IBR_PIRQG             (6 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ G
#define V_ITSS_SB_IBR_PIRQH             (7 <<N_ITSS_SB_IBR)  ///< Route Pin INT B to PIC PIRQ H

#define N_ITSS_SB_IAR                   0                  ///< Pin A Routing Control
#define V_ITSS_SB_IAR_PIRQA             (0 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ A
#define V_ITSS_SB_IAR_PIRQB             (1 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ B
#define V_ITSS_SB_IAR_PIRQC             (2 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ C
#define V_ITSS_SB_IAR_PIRQD             (3 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ D
#define V_ITSS_SB_IAR_PIRQE             (4 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ E
#define V_ITSS_SB_IAR_PIRQF             (5 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ F
#define V_ITSS_SB_IAR_PIRQG             (6 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ G
#define V_ITSS_SB_IAR_PIRQH             (7 <<N_ITSS_SB_IAR)  ///< Route Pin INT A to PIC PIRQ H

#define R_ITSS_SB_IPC0              0x3200  ///< Interrupt Polarity Control 0 - IRQ 31...0
#define R_ITSS_SB_IPC1              0x3204  ///< Interrupt Polarity Control 0 - IRQ 63...32
#define R_ITSS_SB_IPC2              0x3208  ///< Interrupt Polarity Control 0 - IRQ 95...64
#define R_ITSS_SB_IPC3              0x320C  ///< Interrupt Polarity Control 0 - IRQ 119..96

#define R_PCR_ITSS_NMICSTS    0x3330 ///< NMI Delivery Control and Status
#define S_PCR_ITSS_NMICSTS    4
#define N_PCR_ITSS_NMI2SMIEN  2
#define N_PCR_ITSS_NMI2SMISTS 3

#define R_ITSS_SB_MMC              0x3334   ///< Master Message Control Register

#define V_ITSS_SB_IPC_ACTIVE_HIGH   0       ///< IRQx will be active high (default)
#define V_ITSS_SB_IPC_ACTIVE_LOW    1       ///< IRQx will be active low

#endif

