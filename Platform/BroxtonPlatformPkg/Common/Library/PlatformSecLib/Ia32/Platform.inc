;; @file
;  Platform Specific Definitions.
;
;  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>
;
;  This program and the accompanying materials
;  are licensed and made available under the terms and conditions of the BSD License
;  which accompanies this distribution.  The full text of the license may be found at
;  http://opensource.org/licenses/bsd-license.php.
;
;  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
;  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
;
;;

;INCLUDE FlashMap.inc ;ECP porting

MKF_SOFTSDV_FLAG              EQU   0

; Set "MINIMUM_BOOT_SUPPORT" flag allows BIOS boot as minimum feature in SEC phase.
MINIMUM_BOOT_SUPPORT          EQU 0               ; ="0", Normal Boot;
                                                  ; ="1", Minimum Feature Boot
; "RESET_IN_SEC" flag allows BIOS doing RESET in SEC phase
RESET_IN_SEC                  EQU 0               ; ="0", RESET occurs in OemIohInit.c
                                                  ; ="1", RESET occurs in SEC phase

EARLY_MICROCODE_SUPPORT       EQU 1
DETERMINISTIC_BSP_SUPPORT     EQU 0
DEBUG                         EQU 1

;
; IO port to access the upper 128-byte of RTC RAM
;
RTC_UPPER_INDEX               EQU 072h
RTC_UPPER_DATA                EQU 073h

;
; Offset of data stored in the upper 128-byte of RTC RAM.
;
CMOS_CPU_BSP_SELECT           EQU 010h            ; BspSelection
CMOS_CPU_UP_MODE              EQU 011h            ; UpBootSelection

;
; Cpu Ratio and Vid stored in the upper 128-byte of RTC RAM.
;
CMOS_CPU_RATIO_OFFSET         EQU 012h            ; ProcessorFlexibleRatio
CMOS_CPU_CORE_HT_OFFSET       EQU 013h            ; ProcessorHyperThreadingEnable & EnableCoresInSbsp & EnableCoresInNbsp

;
; CPU Feature
;
CMOS_CPU_BIST_OFFSET          EQU 015h            ; ProcessorBistEnable
CMOS_CPU_VMX_OFFSET           EQU 016h            ; ProcessorVmxEnable

;
; Port80 Selection
;
CMOS_PORT80_OFFSET         	  EQU 017h            ; Port80Route

;
;Flash layout map
;
PEICODE_REGION_BASE_ADDRESS   EQU FLASH_BASE
PEICODE_REGION_SIZE           EQU FLASH_SIZE
PEICODE_REGION_SIZE_MASK      EQU (NOT (PEICODE_REGION_SIZE - 1))


PEI_CORE_ENTRY_BASE           EQU 0FFFFFFE0h
FV_MAIN_BASE                  EQU 0FFFFFFFCh

MAX_NR_BUS                    EQU 0FFh
MAX_NR_CPU_SOCKETS            EQU 2               ; DP example, MP may have 4 or more

BIT0                          EQU 01h
BIT1                          EQU 02h
BIT2                          EQU 04h
BIT3                          EQU 08h
BIT4                          EQU 10h
BIT5                          EQU 20h
BIT6                          EQU 40h
BIT7                          EQU 80h
BIT8                          EQU 100h
BIT9                          EQU 200h
BIT10                         EQU 400h
BIT11                         EQU 800h
BIT12                         EQU 1000h
BIT13                         EQU 2000h
BIT14                         EQU 4000h
BIT15                         EQU 8000h
BIT16                         EQU 10000h
BIT17                         EQU 20000h
BIT18                         EQU 40000h
BIT19                         EQU 80000h
BIT20                         EQU 0100000h
BIT23                         EQU 0800000h
BIT31                         EQU 080000000h
; Bit definition in MM1
BadCMOSDetected               EQU (BIT0 shl 17)
BSPApicIDSaveStart            EQU 24
