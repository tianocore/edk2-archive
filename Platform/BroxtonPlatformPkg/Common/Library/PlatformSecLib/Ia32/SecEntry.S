## @file
#  This is the code that goes from real-mode to protected mode.
#  It consumes the reset vector.
#
#  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
#
##

.nolist
  .include "Platform_S.inc"
  .include "Ia32_S.inc"
  .include "Chipset_S.inc"
  .include "SecCore_S.inc"
.list

ASM_GLOBAL ASM_PFX(SecStartup)

.text

###############################################################################
#
# Macro functions
#
###############################################################################
.macro CALL_MMX RoutineLabel

  movl    $0f, %esi
  movd    %esi, %mm7                    # Save ReturnAddress into MM7
  jmp     \RoutineLabel
0:

.endm

.macro RET_ESI

  movd    %mm7, %esi                    # Restore ESP from MM7
  jmp     *%esi

.endm


.align 4
###############################################################################
#
# Routine for Module Entry
#
###############################################################################
ASM_GLOBAL ASM_PFX (_ModuleEntryPoint)
ASM_PFX (_ModuleEntryPoint):

  STATUS_CODE $0x02
  STATUS_CODE $0x03

  CALL_MMX  PlatformInitialization
  STATUS_CODE $0x04

  #
  # Set BIT16 and BIT17 in REG_SB_BIOS_CONFIG, Port 0x4, Offset 0x6.
  # These bits need to be set before setting bits [1:0] in BIOS_RESET_CPL
  # so that PUNIT will not power gate DFX.
  #
  movl    $0xCF8, %edx                  # Config MDR
  movl    $0x800000d4, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx                  # Set BIT16 and BIT17
  movl    $0x30000, %eax
  outl    %eax, %dx

  movl    $0xCF8, %edx                  # Config MCR
  movl    $0x800000d0, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx                  # Write_opcode + portID + offset
  movl    $0x070406F0, %eax
  outl    %eax, %dx

  #
  # Set BIOS_RESET_DONE (BIT0) and BIOS_ALL_DONE (BIT1) in
  # PUNIT.BIOS_RESET_CPL register, Port 0x4, Offset 0x5.
  #
  movl    $0xCF8, %edx                  # Config MCD
  movl    $0x800000d4, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx                  # Set BIT0 and BIT1
  movl    $3, %eax
  outw    %ax, %dx

  movl    $0xCF8, %edx                  # Config MCR
  movl    $0x800000d0, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx                  # Write_opcode + portID + offset
  movl    $0x070405F0, %eax
  outl    %eax, %dx

  STATUS_CODE $0x0A
  CALL_MMX  EstablishStack              # For CPU SV

  STATUS_CODE $0x0B

  jmp  ASM_PFX (CallPeiCoreEntryPoint)

# end of _ModuleEntryPoint


###############################################################################
#
# Routine for Protected Mode EntryPoint
#
###############################################################################
ASM_GLOBAL ASM_PFX (ProtectedModeEntryPoint)
ASM_PFX (ProtectedModeEntryPoint):

  RET_ESI

# end of ProtectedModeEntryPoint


###############################################################################
#
# Routine for Platform Initialization
#
###############################################################################
ASM_GLOBAL ASM_PFX (PlatformInitialization)
ASM_PFX (PlatformInitialization):


  #
  # Program PCIEXBAR and enable it in 0/0/0 
  # Lo - Offset 0x60
  # Hi - Offset 0x64
  #

  movl    $0x80000060, %eax
  movw    $0xCF8, %dx
  outl    %eax, %dx
  movl    $(CPU_HEC_BASE | CPU_HEC_EN), %eax
  movw    $0xCFC, %dx
  outl    %eax, %dx

  #
  # Program and enable all known base addresses
  #

  #
  # Program and enable MCH base address.
  #
  movl    $R_MCH_BASE, %edi
  movl    $(MCH_BASE_ADDRESS + B_MCH_BASE_ADDRESS_EN), (%edi)

  #
  # Program and enable SPI base address.
  # B0:D13:F2
  movl    $0xCF8, %edx               # Config SPI Base
  movl    $0x8006A010, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx
  movl    $SPI_BASE_ADDRESS, %eax
  outl    %eax, %dx

  movl    $0xCF8, %edx               # Config SPI Base
  movl    $0x8006A004, %eax
  outl    %eax, %dx

  movl    $0xCFC, %edx
  inl     %dx, %eax
  orl     $2, %eax                   # enable memory space
  outl    %eax, %dx

  movl    $R_PMC_ACPI_BASE, %edi
  movw    $ACPI_BASE_ADDRESS, (%edi)

  #
  # Program PMC Bar0(IPC1 4KB and GCR 4KB) and Bar1(SSRAM, 4KB), are 64bit bars.
  #
  movl    $R_PMC_MMIO_BAR0, %edi
  movl    $IPC1_BASE_ADDRESS, (%edi)

  movl    $(R_PMC_MMIO_BAR0 + R_PMC_MMIO_BAR1_OFFSET_4), %edi
  movl    $0x00, (%edi)

  movl    $R_PMC_MMIO_BAR1, %edi
  movl    $SSRAM_BASE_ADDRESS, (%edi)

  movl    $( R_PMC_MMIO_BAR1 + R_PMC_MMIO_BAR1_OFFSET_4), %edi
  movl    $0x00, (%edi)
  
  #
  # Enable Bus IO space decode
  #
  movl    $R_PMC_PCI_CMD, %edi
  movl    $0x07, (%edi)

  # 
  # WA for ACPI PM1 timer BXT 0 and 1
  #
  movl    $0x121, %ecx
  movl    $(BIT16 + ACPI_BASE_ADDRESS + R_ACPI_PM1_TMR), %eax  # Bit 16 is enable and 15:0 address
  movl    $0x2FBA2E25, %edx
  wrmsr

  #
  # End program and enable all known base addresses
  #

  #
  # HPET memory address enable
  #

  movl    $R_P2SB_HPTC, %edi
  movb    $HPTC_AE, (%edi)

  #
  # Check RTC power well first
  #
  movl    $(PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1), %edi        # PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1
  movw    (%edi), %ax
  testw   $0x200, %ax                                          # B_PMC_GEN_PMCON_GEN_RST_STS
  jz      check_RTC_PWR_STS

force_cold_boot_path:
  movw    %ax, %cx                                             # Save

  movw    $(ACPI_BASE_ADDRESS + R_ACPI_PM1_CNT), %dx
  inw     %dx, %ax
  andw    $(~V_ACPI_PM1_CNT_S5), %ax                           # Clear sleep type field SLP_TYP [12:10]
  outw    %ax, %dx

  movw    %cx, %ax                                             # restore

check_RTC_PWR_STS:
  testw   $0x4, %ax                                            # B_PMC_GEN_PMCON_RTC_PWR_STS
  jz      no_RTC_pwr_failure  

  #
  # According to CHV BIOS Specification, the following sequence must be programmed
  # in order to ensure RTC state has been initialized.
  #
  # The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set before memory initialization.
  # This will ensure that the RTC state machine has been initialized.
  #  1. If the RTC_PWR_STS bit is set, steps 2 through 5 should be executed.
  #  2. Set RTC Register 0Ah[6:4] to '110' or '111'.
  #  3. Set RTC Register 0Bh[7].
  #  4. Set RTC Register 0Ah[6:4] to '010'.
  #  5. Clear RTC Register 0Bh[7].

init_RTC_state_machine:

  #
  # Set RTC Register 0Ah[6:4] to '110' or '111'.
  #
  movb    $0x0A, %al
  outb    %al, $0x70
  nop                                   # Delay
  nop                                   # Delay
  movb    $0x66, %al
  outb    %al, $0x71
  nop                                   # Delay
  nop                                   # Delay

  #
  # Set RTC Register 0Bh[7].
  #
  movb    $0x0B, %al
  outb    %al, $0x70
  nop                                   # Delay
  nop                                   # Delay
  inb     $0x71, %al
  nop                                   # Delay
  nop                                   # Delay
  orb     $0x80, %al
  outb    %al, $0x71
  nop                                   # Delay
  nop                                   # Delay

  #
  # Set RTC Register 0Ah[6:4] to '010'.
  #
  movb    $0x0A, %al
  outb    %al, $0x70
  nop                                   # Delay
  nop                                   # Delay
  movb    $0x26, %al
  outb    %al, $0x71
  nop                                   # Delay
  nop                                   # Delay

  #
  # Clear RTC Register 0Bh[7].
  #
  movb    $0x0B, %al
  outb    %al, $0x70
  nop                                   # Delay
  nop                                   # Delay
  inb     $0x71, %al
  nop                                   # Delay
  nop                                   # Delay
  andb    $(~0x80), %al
  outb    %al, $0x71
  nop                                   # Delay
  nop                                   # Delay

no_RTC_pwr_failure:
  #
  # Enable SPI Prefetch
  #
  movl    $(SPI_BASE_ADDRESS + R_SPI_BCR), %edi
  orl     $0x08, (%edi)                 # Bits [3:2] = '10' - enable prefetching and caching.

  #
  # Program 8259 Interrupt Controller to disable all interrupts
  #
  movb    $0xFF, %al
  outb    %al, $0x21                    # Mask off all interrupts in master 8259
  outb    %al, $0xa1                    # Mask off all interrupts in slave 8259

  #
  # Halt TCO Timer
  #
  movw    $(ACPI_BASE_ADDRESS + R_TCO_CNT), %dx
  inw     %dx, %ax
  orw     $B_TCO_CNT_TMR_HLT, %ax
  outw    %ax, %dx

.if 1
  # Do nothing
.else
  #
  # Clear the Second Timeout Status bit by writing 1
  #
  movl    $(ACPI_BASE_ADDRESS + R_TCO_STS), %edx
  inl     %dx, %eax
  orl     $B_TCO_STS_SECOND_TO, %eax
  outl    %eax, %dx
.endif

  #
  # Check to see if 0xCF9 Global Reset bit is set. if set clear it.
  #
  movl    $(PMC_BASE_ADDRESS + R_PMC_PMIR), %edi
  movl    (%edi), %eax
  testl   $B_PMC_PMIR_CF9GR, %eax     # Check whether 0xCF9 Global Reset bit is set
  jz      GlobalresetClear                # If no, continue
  andl    $(~B_PMC_PMIR_CF9GR), %eax  # Clear 0xCF9 Global Reset bit
  movl    %eax, (%edi)

GlobalresetClear:
  #
  # Clear HPET Timer 0 Lower and Upper Comparator Value.
  #
  xorl    %eax, %eax
  movl    $HPET_COMP_1, %esi
  movl    %eax, (%esi)
  movl    $HPET_COMP_2, %esi
  movl    %eax, (%esi)

  #
  # Read Bunit.BMISC BIT1 to check F-segment set
  # Determine if INIT or Hard Reset
  #
  movl    $(MCH_BASE_ADDRESS + BUNIT_BMISC), %edi
  movl    (%edi), %eax

  testl   $B_BMISC_RFSDRAM, %eax        # Check bit offset 1
  jnz     L1

reset:
  #
  # Do a hard Reset if INIT.
  #
  movb    $6, %al
  movw    $0xCF9, %dx
  outb    %al, %dx                      # Hard reset
  jmp     .

L1:



  RET_ESI

# end of PlatformInitialization


###############################################################################
#
# Routine to Establish Stack
#
# STATUS_CODE $0x0A
#
###############################################################################
ASM_GLOBAL ASM_PFX (EstablishStack)
ASM_PFX (EstablishStack):

  #
  # Enable STACK
  #
  # To be programmed...
.if 0     # equate not defined
  movl    ASM_PFX(PcdGet32 (PcdTemporaryRamBase)), %esp
  addl    ASM_PFX(PcdGet32 (PcdTemporaryRamSize)), %esp
  subl    $4, %esp

  # Pass NEM address into the PEI Core
  #  push    PhysBase

  push    ASM_PFX(PcdGet32 (PcdTemporaryRamBase))

  # Dispatch table

  push    -(LAST_ADDRESS - offset MICROCODE_DISPATCH_DESCRIPTOR)

  # Pass stack size into the PEI Core
  push    ASM_PFX(PcdGet32 (PcdTemporaryRamSize))
.endif
  RET_ESI

# end of EstablishStack


###############################################################################
#
# Routine to Call PeiCore EntryPoint
#
# STATUS_CODE $0x0B
#
###############################################################################
ASM_GLOBAL ASM_PFX (CallPeiCoreEntryPoint)
ASM_PFX (CallPeiCoreEntryPoint):
 
  #
  # Set stack top pointer
  #
  movl    %ds:CarBase(%ebp), %esp
  addl    %ds:CarSize(%ebp), %esp

  #
  # Push CPU count to stack first, then AP's (if there is one)
  # BIST status, and then BSP's
  #

  #
  # Here work around for BIST
  #
  # Get number of BSPs
  movd    %mm1, %ecx
  movzbl  %ch, %ecx

  # Save number of BSPs
  pushl   %ecx

GetSBSPBist:
  # Save SBSP BIST 
  movd    %mm0, %eax 
  pushl   %eax

  # Save SBSP APIC ID
  movd    %mm1, %eax 
  shrl    $BSPApicIDSaveStart, %eax     # Resume APIC ID 
  pushl   %eax

TransferToSecStartup:

  # Switch to "C" code
  STATUS_CODE $0x0C

  #
  # Pass entry point of the PEI core
  #
  movl    $PEI_CORE_ENTRY_BASE, %edi    # 0FFFFFFE0h
  pushl   %ds:(%edi)

  #
  # Pass BFV into the PEI Core
  #
  push    %ds:IBBBase(%ebp)             # 0FFFFFFFCh

  #
  # ECPoverride: SecStartup entry point needs 4 parameters
  #
  movl    %ds:CarBase(%ebp), %edi
  addl    %ds:CarSize(%ebp), %edi
  subl    $0x10000, %edi
  pushl   %edi
  movl    $0x10000, %edi
  pushl   %edi
  #
  # Pass stack size into the PEI Core
  #


  #
  # Pass Control into the PEI Core
  #
  call    ASM_PFX(SecStartup)
# end of CallPeiCoreEntryPoint


###############################################################################
#
# Routine for StartUp Ap
#
###############################################################################
ASM_GLOBAL ASM_PFX (StartUpAp)
ASM_PFX (StartUpAp):

  movl    $HPET_COMP_2, %esi
  lock    incb    (%esi)

  DISABLE_CACHE
  #
  # Halt the AP and wait for the next SIPI
  #
Ap_Halt:
  cli
B3:
  hlt
  jmp     B3
  ret
# end of StartUpAp


.data
MtrrInitTable:
    .word  MTRR_DEF_TYPE
    .word  MTRR_FIX_64K_00000
    .word  MTRR_FIX_16K_80000
    .word  MTRR_FIX_16K_A0000
    .word  MTRR_FIX_4K_C0000
    .word  MTRR_FIX_4K_C8000
    .word  MTRR_FIX_4K_D0000
    .word  MTRR_FIX_4K_D8000
    .word  MTRR_FIX_4K_E0000
    .word  MTRR_FIX_4K_E8000
    .word  MTRR_FIX_4K_F0000
    .word  MTRR_FIX_4K_F8000

.equ MtrrCountFixed, ((. - MtrrInitTable) / 2)

    .word  MTRR_PHYS_BASE_0
    .word  MTRR_PHYS_MASK_0
    .word  MTRR_PHYS_BASE_1
    .word  MTRR_PHYS_MASK_1
    .word  MTRR_PHYS_BASE_2
    .word  MTRR_PHYS_MASK_2
    .word  MTRR_PHYS_BASE_3
    .word  MTRR_PHYS_MASK_3
    .word  MTRR_PHYS_BASE_4
    .word  MTRR_PHYS_MASK_4
    .word  MTRR_PHYS_BASE_5
    .word  MTRR_PHYS_MASK_5
    .word  MTRR_PHYS_BASE_6
    .word  MTRR_PHYS_MASK_6
    .word  MTRR_PHYS_BASE_7
    .word  MTRR_PHYS_MASK_7
    .word  MTRR_PHYS_BASE_8
    .word  MTRR_PHYS_MASK_8
    .word  MTRR_PHYS_BASE_9
    .word  MTRR_PHYS_MASK_9
.equ MtrrCount,      ((. - MtrrInitTable) / 2)
