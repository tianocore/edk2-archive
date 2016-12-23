;; @file
;  IA32 architecture MSRs.
;
;  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
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

IA32_MTRR_CAP                 EQU 0FEh
MTRR_PHYS_BASE_0              EQU 0200h
MTRR_PHYS_MASK_0              EQU 0201h
MTRR_PHYS_BASE_1              EQU 0202h
MTRR_PHYS_MASK_1              EQU 0203h
MTRR_PHYS_BASE_2              EQU 0204h
MTRR_PHYS_MASK_2              EQU 0205h
MTRR_PHYS_BASE_3              EQU 0206h
MTRR_PHYS_MASK_3              EQU 0207h
MTRR_PHYS_BASE_4              EQU 0208h
MTRR_PHYS_MASK_4              EQU 0209h
MTRR_PHYS_BASE_5              EQU 020Ah
MTRR_PHYS_MASK_5              EQU 020Bh
MTRR_PHYS_BASE_6              EQU 020Ch
MTRR_PHYS_MASK_6              EQU 020Dh
MTRR_PHYS_BASE_7              EQU 020Eh
MTRR_PHYS_MASK_7              EQU 020Fh
MTRR_PHYS_BASE_8              EQU 0210h
MTRR_PHYS_MASK_8              EQU 0211h
MTRR_PHYS_BASE_9              EQU 0212h
MTRR_PHYS_MASK_9              EQU 0213h
MTRR_FIX_64K_00000            EQU 0250h
MTRR_FIX_16K_80000            EQU 0258h
MTRR_FIX_16K_A0000            EQU 0259h
MTRR_FIX_4K_C0000             EQU 0268h
MTRR_FIX_4K_C8000             EQU 0269h
MTRR_FIX_4K_D0000             EQU 026Ah
MTRR_FIX_4K_D8000             EQU 026Bh
MTRR_FIX_4K_E0000             EQU 026Ch
MTRR_FIX_4K_E8000             EQU 026Dh
MTRR_FIX_4K_F0000             EQU 026Eh
MTRR_FIX_4K_F8000             EQU 026Fh
MTRR_DEF_TYPE                 EQU 02FFh

MTRR_MEMORY_TYPE_UC           EQU 00h
MTRR_MEMORY_TYPE_WC           EQU 01h
MTRR_MEMORY_TYPE_WT           EQU 04h
MTRR_MEMORY_TYPE_WP           EQU 05h
MTRR_MEMORY_TYPE_WB           EQU 06h

MTRR_DEF_TYPE_E               EQU 0800h
MTRR_DEF_TYPE_FE              EQU 0400h
MTRR_PHYSMASK_VALID           EQU 0800h

;
; Define the high 32 bits of MTRR masking
; This should be read from CPUID EAX = 080000008h, EAX bits [7:0]
; But for most platforms this will be a fixed supported size so it is 
; fixed to save space.
;
MTRR_PHYS_MASK_VALID          EQU 0800h
MTRR_PHYS_MASK_HIGH           EQU 00000000Fh      ; For 36 bit addressing
;MTRR_PHYS_MASK_HIGH           EQU 0000000FFh      ; For 40 bit addressing

IA32_MISC_ENABLE              EQU 1A0h
FAST_STRING_ENABLE_BIT        EQU 01h

CR0_CACHE_DISABLE             EQU 040000000h
CR0_NO_WRITE                  EQU 020000000h

IA32_PLATFORM_ID              EQU 017h 
IA32_BIOS_UPDT_TRIG           EQU 079h 
IA32_BIOS_SIGN_ID             EQU 08Bh 
PLATFORM_INFO                 EQU 0CEh
NO_EVICT_MODE                 EQU 2E0h
NO_EVICTION_ENABLE_BIT        EQU 01h

;
; MSR definitions
;
MSR_IA32_PLATFORM_ID          EQU 0017h
MSR_APIC_BASE                 EQU 001Bh
MSR_SOCKET_ID                 EQU 0039h
MSR_IA32_FEATURE_CONTROL      EQU 003Ah
MSR_CLOCK_CST_CONFIG_CONTROL  EQU 00E2h
MSR_CLOCK_FLEX_MAX            EQU 0194h
MSR_IA32_PERF_STS             EQU 0198h
MSR_IA32_PERF_CTL             EQU 0199h
MSR_IA32_MISC_ENABLES         EQU 01A0h
MSR_IA32_MC8_MISC2            EQU 0288h
MSR_IA32_MC7_CTL              EQU 041Ch


CSR_SVID_SDID                       	  EQU 02Ch            ; D0:F0:R2Ch
DEAFULT_SVID_SDID                       EQU 80868086h       ; DWORD Access & Write Once ONLY

;
; Processor MSR definitions
;
MSR_BBL_CR_CTL3                         EQU 011Eh ; L2 cache configuration MSR
B_MSR_BBL_CR_CTL3_L2_NOT_PRESENT        EQU 23    ; L2 not present
B_MSR_BBL_CR_CTL3_L2_ENABLED            EQU  8    ; L2 enabled
B_MSR_BBL_CR_CTL3_L2_HARDWARE_ENABLED   EQU  0    ; L2 hardware enabled


;
; Local APIC Register Equates
;
LOCAL_APIC_ID_REG             EQU 0FEE00020h
APIC_ICR_HI                   EQU 0FEE00310h
APIC_ICR_LO                   EQU 0FEE00300h
ANDICRMask                    EQU 0FFF32000h      ; AND mask for ICR Saving reserved bits
ORSelfINIT                    EQU 000004500h      ; OR mask to send INIT IPI to itself
ORAllButSelf                  EQU 0000C0000h      ; OR mask to set dest field = "All But Self"

;
; Cache control macro
;
DISABLE_CACHE macro
    mov   eax, cr0
    or    eax, CR0_CACHE_DISABLE + CR0_NO_WRITE
    wbinvd
    mov   cr0, eax
endm

ENABLE_CACHE  macro
    mov   eax, cr0
    and   eax, NOT (CR0_CACHE_DISABLE + CR0_NO_WRITE)
    wbinvd
    mov   cr0, eax
endm
      

BLOCK_LENGTH_BYTES  EQU  2048

UpdateHeaderStruc  STRUC
  dHeaderVersion        dd  ?           ; Header version#
  dUpdateRevision       dd  ?           ; Update revision#
  dDate                 dd  ?           ; Date in binary (08/13/07 as 0x08132007)
  dProcessorSignature   dd  ?           ; CPU type, family, model, stepping
  dChecksum             dd  ?           ; Checksum
  dLoaderRevision       dd  ?           ; Update loader version#
  dProcessorFlags       dd  ?           ; Processor Flags
  dDataSize             dd  ?           ; Size of encrypted data
  dTotalSize            dd  ?           ; Total size of update in bytes
  bReserved             db  12 dup(?)   ; 12 bytes reserved
UpdateHeaderStruc	ENDS

HobStruc STRUC
  Sign	               dd  1           	; Signiture#
  CarBase              dd  1            ; Cache As Ram Base Address
  CarSize              dd  1            ; Cache As Ram Size
  IBBSource            dd  1            ; IBBM Address in SRAM
  IBBBase              dd  1            ; IBBM Base in CAR.
  IBBSize              dd  1            ; IBBM Size
  IBBLSource           dd  1            ; IBBL Address in SRAM
  IBBLBase             dd  1            ; IBBL Base in CAR.
  IBBLSize             dd  1            ; IBBL Size
  FITBase              dd  1            ; FIT Base Address
  StackHeapBase        dd  1            ; STACK&HEAP Base .
  StackHeapSize        dd  1            ; STACK&HEAP Size  
HobStruc ENDS
