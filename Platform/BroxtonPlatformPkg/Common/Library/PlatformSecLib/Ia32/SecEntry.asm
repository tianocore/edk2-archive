;; @file
;  This is the code that goes from real-mode to protected mode.
;  It consumes the reset vector.
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

  INCLUDE Platform.inc
  INCLUDE Ia32.inc
  INCLUDE Chipset.inc
  INCLUDE SecCore.inc

  ;EXTRN   PcdGet32(PcdTemporaryRamSize):DWORD

.686p
.xmm
.model small, c

 EXTRN   SecStartup:NEAR

_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'
                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE

CALL_MMX macro   RoutineLabel

  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:

endm

RET_ESI  macro

  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi

endm


align 4
_ModuleEntryPoint PROC NEAR PUBLIC

  STATUS_CODE (02h)
  STATUS_CODE (03h)

  CALL_MMX  PlatformInitialization
  STATUS_CODE (04h)

  ;
  ; Set BIT16 and BIT17 in REG_SB_BIOS_CONFIG, Port 0x4, Offset 0x6.
  ; These bits need to be set before setting bits [1:0] in BIOS_RESET_CPL
  ; so that PUNIT will not power gate DFX.
  ;
  mov     edx, 0CF8h               ; Config MDR
  mov     eax, 800000d4h
  out     dx,  eax

  mov     edx, 0CFCh               ; Set BIT16 and BIT17
  mov     eax, 30000h
  out     dx,  eax

  mov     edx, 0CF8h               ; Config MCR
  mov     eax, 800000d0h
  out     dx, eax

  mov     edx, 0CFCh
  mov     eax, 0070406f0h          ; Write_opcode + portID + offset
  out     dx,  eax

  ;
  ; Set BIOS_RESET_DONE (BIT0) and BIOS_ALL_DONE (BIT1) in
  ; PUNIT.BIOS_RESET_CPL register, Port 0x4, Offset 0x5.
  ;
  mov     edx, 0CF8h               ; Config MCD
  mov     eax, 800000d4h
  out     dx,  eax

  mov     edx, 0CFCh
  mov     eax, 3                   ; Set BIT0 and BIT1
  out     dx,  ax

  mov     edx, 0CF8h               ; Config MCR
  mov     eax, 800000d0h
  out     dx,  eax

  mov     edx, 0CFCh
  mov     eax, 0070405f0h          ; Write_opcode + portID + offset
  out     dx,  eax

  STATUS_CODE (0Ah)
;  CALL_MMX  EstablishStack         ; For CPU SV

  STATUS_CODE (0Bh)

  jmp  CallPeiCoreEntryPoint

_ModuleEntryPoint ENDP

ProtectedModeEntryPoint PROC NEAR PUBLIC

  RET_ESI

ProtectedModeEntryPoint  ENDP

PlatformInitialization    PROC    NEAR    PRIVATE

  ;
  ; Program PCIEXBAR and enable it in 0/0/0
  ; Lo - Offset 0x60
  ; Hi - Offset 0x64
  ;

  mov      eax, 080000060h
  mov      dx,  0CF8h
  out      dx,  eax
  mov      eax, CPU_HEC_BASE OR CPU_HEC_EN
  mov      dx,  0CFCh
  out      dx,  eax


  ;
  ; Program and enable all known base addresses
  ;

  ;
  ; Program and enable MCH base address.
  ;
  mov     edi, R_MCH_BASE
  mov     Dword Ptr [edi], MCH_BASE_ADDRESS + B_MCH_BASE_ADDRESS_EN

  ;
  ; Program and enable SPI base address.
  ; B0:D13:F2
  mov     edx, 0CF8h               ; Config SPI Base
  mov     eax, 8006A010h
  out     dx, eax

  mov     edx, 0CFCh
  mov     eax, SPI_BASE_ADDRESS
  out     dx,  eax

  mov     edx, 0CF8h               ; Config SPI Base
  mov     eax, 8006A004h
  out     dx, eax

  mov     edx, 0CFCh
  in    eax, dx
  or      eax, 2h                   ; enable memory space
  out     dx,  eax

  ;
  ; Program and enable ACPI base address.
  ;
  mov     edi, R_PMC_ACPI_BASE
  mov     Word Ptr [edi], ACPI_BASE_ADDRESS

  ;
  ; Program D13:F1, PMC Bar0(IPC1) and Bar1, are 64bit bars.
  ; This should be moved into C code - no need to init this early..
  ;
  mov     edi, R_PMC_MMIO_BAR0
  mov     Dword Ptr [edi], IPC1_BASE_ADDRESS

  mov     edi, R_PMC_MMIO_BAR0 + 4h
  mov     Dword Ptr [edi], 0h

  mov     edi, R_PMC_MMIO_BAR1
  mov     Dword Ptr [edi], IPC1_BASE2_ADDRESS

  mov     edi, R_PMC_MMIO_BAR1 + 4h
  mov     Dword Ptr [edi], 0h

  ; Enable Bus IO space decode
  mov     edi, R_PMC_PCI_CMD
  mov     Word Ptr [edi], 07h

  ; BXT HSDES 1958937
  ; WA for ACPI PM1 timer BXT 0 and 1
  mov     ecx, 0121h
  mov     eax, BIT16 + ACPI_BASE_ADDRESS + R_ACPI_PM1_TMR  ; Bit 16 is enable and 15:0 address
  mov     edx, 2FBA2E25h
  wrmsr

  ;
  ; HPET memory address enable
  ;

  mov     edi, R_P2SB_HPTC
  mov     Byte Ptr [edi], HPTC_AE


  ;
  ; Check RTC power well first
  ;
  mov     edi, PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1
  mov     ax,  WORD PTR [edi]
  test    ax,  0FFFFh               ; B_PMC_GEN_PMCON_GEN_RST_STS
  jz      check_RTC_PWR_STS
  test    ax,  0200h               ; B_PMC_GEN_PMCON_GEN_RST_STS
  jz      check_RTC_PWR_STS

force_cold_boot_path:
  mov     cx, ax                   ; Save
  mov     dx, ACPI_BASE_ADDRESS + R_ACPI_PM1_CNT
  in      ax, dx
  and     ax, NOT (V_ACPI_PM1_CNT_S5) ; Clear sleep type field SLP_TYP [12:10]
  out     dx, ax

  mov     ax, cx                   ; restore

check_RTC_PWR_STS:
  test    ax, 004h                 ; B_PMC_GEN_PMCON_RTC_PWR_STS
  jz      no_RTC_pwr_failure

  ;
  ; According to CHV BIOS Specification, the following sequence must be programmed
  ; in order to ensure RTC state has been initialized.
  ;
  ; The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set before memory initialization.
  ; This will ensure that the RTC state machine has been initialized.
  ;  1. If the RTC_PWR_STS bit is set, steps 2 through 5 should be executed.
  ;  2. Set RTC Register 0Ah[6:4] to '110' or '111'.
  ;  3. Set RTC Register 0Bh[7].
  ;  4. Set RTC Register 0Ah[6:4] to '010'.
  ;  5. Clear RTC Register 0Bh[7].

init_RTC_state_machine:

  ;
  ; Set RTC Register 0Ah[6:4] to '110' or '111'.
  ;
  mov     al, 0Ah
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  mov     al, 066h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Set RTC Register 0Bh[7].
  ;
  mov     al, 0Bh
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  in      al, 071h
  nop                  ; Delay
  nop                  ; Delay
  or      al, 080h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Set RTC Register 0Ah[6:4] to '010'.
  ;
  mov     al, 0Ah
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  mov     al, 026h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Clear RTC Register 0Bh[7].
  ;
  mov     al, 0Bh
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  in      al, 071h
  nop                  ; Delay
  nop                  ; Delay
  and     al, NOT 080h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

no_RTC_pwr_failure:
  ;
  ; Enable SPI Prefetch
  ;

  mov     edi, SPI_BASE_ADDRESS + R_SPI_BCR
  or      Dword Ptr [edi], 08h     ; Bits [3:2] = '10' - enable prefetching and caching.

  ;
  ; Program 8259 Interrupt Controller to disable all interrupts
  ;
  mov     al, 0FFh
  out     21h, al                  ; Mask off all interrupts in master 8259
  out     0a1h, al                 ; Mask off all interrupts in slave 8259

  ;
  ; Halt TCO Timer
  ;
  mov     dx, ACPI_BASE_ADDRESS + R_TCO_CNT
  in      ax, dx
  or      ax, B_TCO_CNT_TMR_HLT
  out     dx, ax

IF 1
  ; Do nothing
ELSE
  ;
  ; Clear the Second Timeout Status bit by writing 1
  ;
  mov     dx, ACPI_BASE_ADDRESS + R_TCO_STS
  in      eax, dx
  or      eax, B_TCO_STS_SECOND_TO
  out     dx, eax
ENDIF

  ;
  ; Check to see if 0xCF9 Global Reset bit is set. if set clear it.
  ;
  mov     edi, PMC_BASE_ADDRESS + R_PMC_PMIR
  mov     eax, DWORD PTR [edi]
  test    eax, B_PMC_PMIR_CF9GR ; Check whether 0xCF9 Global Reset bit is set
  jz      GlobalresetClear         ; If no, continue
  and     eax, NOT (B_PMC_PMIR_CF9GR) ; Clear 0xCF9 Global Reset bit
  mov     DWORD PTR [edi], eax

GlobalresetClear:
  ;
  ; Clear HPET Timer 0 Lower and Upper Comparator Value.
  ;

  xor     eax, eax
  mov     esi, HPET_COMP_1
  mov     Dword Ptr [esi], eax
  mov     esi, HPET_COMP_2
  mov     Dword ptr [esi], eax

  ;
  ; Read Bunit.BMISC BIT1 to check F-segment set
  ; Determine if INIT or Hard Reset
  ;
  mov     edi, MCH_BASE_ADDRESS + BUNIT_BMISC
  mov     eax, Dword Ptr [edi]


  test    eax, B_BMISC_RFSDRAM                ; Check bit offset 1
  jnz      @f

reset:
  ;
  ; Do a hard Reset if INIT.
  ;
  mov     al,  6
  mov     dx,  0cf9h
  out     dx,  al                  ; Hard reset
  jmp     $

@@:

  RET_ESI

PlatformInitialization    ENDP




;  STATUS_CODE (09h)
EstablishStack    PROC    NEAR    PRIVATE

  ;
  ; Enable STACK
  ;
  ; To be programmed...
If 0     ; equate not defined
  mov     esp, PcdGet32 (PcdTemporaryRamBase)
  add     esp, PcdGet32 (PcdTemporaryRamSize)
  sub     esp, 4

  ; Pass NEM address into the PEI Core
  ;  push    PhysBase

  push    PcdGet32 (PcdTemporaryRamBase)

  ; Dispatch table

  push  -(LAST_ADDRESS - offset MICROCODE_DISPATCH_DESCRIPTOR)

  ; Pass stack size into the PEI Core
  push    PcdGet32 (PcdTemporaryRamSize)
endif
  RET_ESI

EstablishStack    ENDP


;  STATUS_CODE (0Bh)
CallPeiCoreEntryPoint   PROC    NEAR    PRIVATE

  ;
  ; Set stack top pointer
  ;
  mov     esp, (HobStruc PTR ds:[ebp]).StackHeapBase;
  add     esp, (HobStruc PTR ds:[ebp]).StackHeapSize

  ;
  ; Push CPU count to stack first, then AP's (if there is one)
  ; BIST status, and then BSP's
  ;

  ;
  ; Here work around for BIST
  ;
  ; Get number of BSPs
  mov     ch, 01 ; for client we have only one BSP
  movzx   ecx, ch

  ; Save number of BSPs
  push    ecx

GetSBSPBist:
  ; Save SBSP BIST
  movd    eax, mm0
  push    eax

  ; Save SBSP APIC ID
  movd    eax, mm1
  shr     eax, BSPApicIDSaveStart       ; Resume APIC ID
  push    eax


TransferToSecStartup:

  ; Switch to "C" code
  STATUS_CODE (0Ch)
  ;jmp $

  ; ECPoverride: SecStartup entry point needs 4 parameters

  ;
  ; Pass entry point of the PEI core
  ;
  mov     edi, PEI_CORE_ENTRY_BASE      ; 0FFFFFFE0h
  push    DWORD PTR ds:[edi]

  ;
  ; Pass BFV into the PEI Core
  ;
  push (HobStruc PTR ds:[ebp]).IBBBase        ; 0FFFFFFFCh

  ;
  ; Pass stack size into the PEI Core
  ;
  Push (HobStruc PTR ds:[ebp]).StackHeapBase   ;calc TempRamBase

  push (HobStruc PTR ds:[ebp]).StackHeapSize   ;calc TempRamBase

  ;
  ; Pass Control into the PEI Core
  ;
  call SecStartup
CallPeiCoreEntryPoint   ENDP

StartUpAp       PROC    NEAR

  mov     esi, HPET_COMP_2
  lock    inc  byte ptr [esi]

  DISABLE_CACHE
;
; Halt the AP and wait for the next SIPI
;
Ap_Halt:
  cli
@@:
  hlt
  jmp     @B
  ret
StartUpAp       ENDP


MtrrInitTable   LABEL BYTE
    DW  MTRR_DEF_TYPE
    DW  MTRR_FIX_64K_00000
    DW  MTRR_FIX_16K_80000
    DW  MTRR_FIX_16K_A0000
    DW  MTRR_FIX_4K_C0000
    DW  MTRR_FIX_4K_C8000
    DW  MTRR_FIX_4K_D0000
    DW  MTRR_FIX_4K_D8000
    DW  MTRR_FIX_4K_E0000
    DW  MTRR_FIX_4K_E8000
    DW  MTRR_FIX_4K_F0000
    DW  MTRR_FIX_4K_F8000

MtrrCountFixed EQU (($ - MtrrInitTable) / 2)

    DW  MTRR_PHYS_BASE_0
    DW  MTRR_PHYS_MASK_0
    DW  MTRR_PHYS_BASE_1
    DW  MTRR_PHYS_MASK_1
    DW  MTRR_PHYS_BASE_2
    DW  MTRR_PHYS_MASK_2
    DW  MTRR_PHYS_BASE_3
    DW  MTRR_PHYS_MASK_3
    DW  MTRR_PHYS_BASE_4
    DW  MTRR_PHYS_MASK_4
    DW  MTRR_PHYS_BASE_5
    DW  MTRR_PHYS_MASK_5
    DW  MTRR_PHYS_BASE_6
    DW  MTRR_PHYS_MASK_6
    DW  MTRR_PHYS_BASE_7
    DW  MTRR_PHYS_MASK_7
    DW  MTRR_PHYS_BASE_8
    DW  MTRR_PHYS_MASK_8
    DW  MTRR_PHYS_BASE_9
    DW  MTRR_PHYS_MASK_9
MtrrCount      EQU (($ - MtrrInitTable) / 2)


;TopOfCar  DD  DATA_STACK_BASE_ADDRESS + DATA_STACK_SIZE

_TEXT_PROTECTED_MODE    ENDS
END

