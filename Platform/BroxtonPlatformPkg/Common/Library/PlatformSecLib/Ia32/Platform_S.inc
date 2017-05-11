# @file
#  Platform Specific Definitions.
#
#  Copyright (c) 2008 - 2017, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
#
#

#INCLUDE FlashMap.inc #ECP porting

.equ MKF_SOFTSDV_FLAG,              0

# Set "MINIMUM_BOOT_SUPPORT" flag allows BIOS boot as minimum feature in SEC phase.
.equ MINIMUM_BOOT_SUPPORT,          0               # ="0", Normal Boot#
                                                    # ="1", Minimum Feature Boot
# "RESET_IN_SEC" flag allows BIOS doing RESET in SEC phase
.equ RESET_IN_SEC,                  0               # ="0", RESET occurs in OemIohInit.c
                                                    # ="1", RESET occurs in SEC phase

.equ EARLY_MICROCODE_SUPPORT,       1
.equ DETERMINISTIC_BSP_SUPPORT,     0
.equ DEBUG,                         1

#
# IO port to access the upper 128-byte of RTC RAM
#
.equ RTC_UPPER_INDEX,               0x72
.equ RTC_UPPER_DATA,                0x73

#
# Offset of data stored in the upper 128-byte of RTC RAM.
#
.equ CMOS_CPU_BSP_SELECT,           0x10            # BspSelection
.equ CMOS_CPU_UP_MODE,              0x11            # UpBootSelection

#
# Cpu Ratio and Vid stored in the upper 128-byte of RTC RAM.
#
.equ CMOS_CPU_RATIO_OFFSET,         0x12            # ProcessorFlexibleRatio
.equ CMOS_CPU_CORE_HT_OFFSET,       0x13            # ProcessorHyperThreadingEnable & EnableCoresInSbsp & EnableCoresInNbsp

#
# CPU Feature
#
.equ CMOS_CPU_BIST_OFFSET,          0x15            # ProcessorBistEnable
.equ CMOS_CPU_VMX_OFFSET,           0x16            # ProcessorVmxEnable

#
# Port80 Selection
#
.equ CMOS_PORT80_OFFSET,            0x17            # Port80Route


.equ PEI_CORE_ENTRY_BASE,           0xFFFFFFE0
.equ FV_MAIN_BASE,                  0xFFFFFFFC

.equ MAX_NR_BUS,                    0xFF
.equ MAX_NR_CPU_SOCKETS,            2               # DP example, MP may have 4 or more

.equ BIT0,                          0x01
.equ BIT1,                          0x02
.equ BIT2,                          0x04
.equ BIT3,                          0x08
.equ BIT4,                          0x10
.equ BIT5,                          0x20
.equ BIT6,                          0x40
.equ BIT7,                          0x80
.equ BIT8,                          0x100
.equ BIT9,                          0x200
.equ BIT10,                         0x400
.equ BIT11,                         0x800
.equ BIT12,                         0x1000
.equ BIT13,                         0x2000
.equ BIT14,                         0x4000
.equ BIT15,                         0x8000
.equ BIT16,                         0x10000
.equ BIT17,                         0x20000
.equ BIT18,                         0x40000
.equ BIT19,                         0x80000
.equ BIT20,                         0x100000
.equ BIT23,                         0x800000
.equ BIT31,                         0x80000000
# Bit definition in MM1
.equ BadCMOSDetected,               (BIT0 << 17)
.equ BSPApicIDSaveStart,            24
