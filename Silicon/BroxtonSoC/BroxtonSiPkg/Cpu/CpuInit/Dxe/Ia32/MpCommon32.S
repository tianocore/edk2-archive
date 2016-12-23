## @file
#  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
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

##include Htequ.inc

.equ    VacantFlag       ,              0x00
.equ    NotVacantFlag    ,              0xff
.equ    StartupApSignal  ,              0x6E755200
.equ    MonitorFilterSize,              0x40
.equ    ApCounterInit    ,              0
.equ    ApInHltLoop      ,              1
.equ    ApInMwaitLoop    ,              2
.equ    ApInRunLoop      ,              3

.equ    LockLocation     ,              0x1000 - 0x0400
.equ    StackStart       ,              LockLocation + 0x4
.equ    StackSize        ,              LockLocation + 0x8
.equ    RendezvousProc   ,              LockLocation + 0x0C
.equ    GdtrProfile      ,              LockLocation + 0x10
.equ    IdtrProfile      ,              LockLocation + 0x16
.equ    BufferStart      ,              LockLocation + 0x1C
.equ    Cr3Location      ,              LockLocation + 0x20
.equ    InitFlag         ,              LockLocation + 0x24 
.equ    WakeUpApManner   ,              LockLocation + 0x28
.equ    BistBuffer       ,              LockLocation + 0x2C

.macro  PAUSE32
        .byte 0xF3
        .byte 0x90
.endm

#-------------------------------------------------------------------------------
#  AsmAcquireMPLock (&Lock)
#-------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmAcquireMPLock)
ASM_PFX(AsmAcquireMPLock):
    pushal
    movl        %esp, %ebp

    movb        $NotVacantFlag, %al
    movl        0x24(%ebp), %ebx
TryGetLock:
    .byte 0xF0              # opcode for lock instruction
    xchgb       (%ebx), %al
    cmpb        $VacantFlag, %al
    jz          LockObtained

    PAUSE32

    jmp         TryGetLock       

LockObtained:
    popal
    ret
#AsmAcquireMPLock   ENDP

#-------------------------------------------------------------------------------
#  AsmReleaseMPLock (&Lock)
#-------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmReleaseMPLock)
ASM_PFX(AsmReleaseMPLock):
    pushal
    movl        %esp, %ebp

    movb        $VacantFlag, %al
    movl        0x24(%ebp), %ebx
    .byte 0xF0              # opcode for lock instruction
    xchgb       (%ebx), %al

    popal
    ret
#AsmReleaseMPLock   ENDP

#-------------------------------------------------------------------------------
#  AsmGetGdtrIdtr (&Gdt, &Idt)#                                                    
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmGetGdtrIdtr)
ASM_PFX(AsmGetGdtrIdtr):
    pushal
    movl        %esp, %ebp
    sgdt        GdtDesc
    leal        GdtDesc, %esi
    movl        0x24(%ebp), %edi
    movl        %esi, (%edi)

    sidt        IdtDesc
    leal        IdtDesc, %esi
    movl        0x28(%ebp), %edi
    movl        %esi, (%edi)
        
    popal
    ret
#AsmGetGdtrIdtr   ENDP

GdtDesc:                        # GDT descriptor
    .word           0x03f   # GDT limit
    .word           0x0     # GDT base and limit will be
    .word           0x0     # filled using sgdt

IdtDesc:                        # IDT descriptor
    .word           0x0     # IDT limit
    .word           0x0     # IDT base and limit will be
    .word           0x0     # filled using sidt
