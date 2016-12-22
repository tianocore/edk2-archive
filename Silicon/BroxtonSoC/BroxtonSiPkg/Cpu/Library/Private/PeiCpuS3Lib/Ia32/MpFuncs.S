## @file
#  This is the assembly code for MP support.
#
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

#include  "MpEqu.h"

  .text
  ASM_FUNCTION_REMOVE_IF_UNREFERENCED

//-------------------------------------------------------------------------------------
//S3RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
//procedure serializes all the AP processors through an Init sequence. It must be
//noted that APs arrive here very raw...ie: real mode, no stack.
//ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
//IS IN MACHINE CODE.
//-------------------------------------------------------------------------------------
//S3RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

.globl ASM_PFX(S3RendezvousFunnelProc)
ASM_PFX(S3RendezvousFunnelProc):
L_S3RendezvousFunnelProcStart:

        .code16

// At this point CS = 0x(vv00) and ip= 0x0.
        mov   %eax, %ebp
        mov   %cs, %ax
        mov   %ax, %ds
        mov   %ax, %es
        mov   %ax, %ss
        xor   %ax, %ax
        mov   %ax, %fs
        mov   %ax, %gs

// Get APIC ID
        mov   $1, %eax
        cpuid
        shr   $24, %ebx
        and   $0xff, %ebx       // EBX is APIC ID

// If it is the first time AP wakes up, just record AP's BIST
// Otherwise, switch to protected mode.
        
        mov   $StartStateLocation, %si
        cmpl  $0, (%si)
        jnz   L_SkipRecordBist

// Record BIST information
//        
        mov   $8, %al
        mul   %bl
        mov   $BistBufferLocation, %si
        add   %ax, %si

        movl  $1, (%si)         // Set Valid Flag
        mov   %ebp, 4(%si)      // Store BIST value

L_SkipRecordBist:
// Switch to flat mode.

        mov   $BufferStartLocation, %si
        mov   (%si), %ebx

        mov   $PmodeOffsetLocation, %di
        mov   (%di), %eax
        mov   %ax, %di
        sub   $6, %di
        add   %ebx, %eax
        mov   %eax, (%di)

        mov    $0, %esi
        mov    $GdtrLocation, %si
        lgdtl  %cs:(%esi)

        xor   %ax, %ax
        mov   %ax, %ds

        mov   %cr0, %eax
        or    $0x00000003, %eax
        mov   %eax, %cr0

        .byte  0x66, 0x67, 0xea                        // far jump
        .long  0x00                                    // 32-bit offset
        .short 0x20                                    // 16-bit selector

        .code32
L_NemInit:                                             // protected mode entry point
        mov   $0x18, %ax
        mov   %ax, %ds
        mov   %ax, %es
        mov   %ax, %fs
        mov   %ax, %gs
        mov   %ax, %ss

        mov   %ebx, %esi

        mov   %esi, %edi
        add   $StartStateLocation, %edi
        mov   $1, %eax
        movl  %eax, (%edi)

        mov   %esi, %edi
        add   $LockLocation, %edi
        mov   $NotVacantFlag, %eax
L_TestLock:
        xchg  %eax, (%edi)
        cmp   $NotVacantFlag, %eax
        jz    L_TestLock

L_ProgramStack:

        mov   %esi, %edi
        add   $StackSizeLocation, %edi
        mov   (%edi), %eax
        mov   %esi, %edi
        add   $StackStartAddressLocation, %edi
        add   (%edi), %eax
        mov   %eax, %esp
        movl  %eax, (%edi)

L_Releaselock:

        mov   $VacantFlag, %eax
        mov   %esi, %edi
        add   $LockLocation, %edi
        xchg  %eax, (%edi)

L_CProcedureInvoke:

        mov   %esi, %edi
        add   $CArgumentLocation, %edi
        mov   (%edi), %eax
        push  %eax

        mov   %esi, %edi
        add   $CProcedureLocation, %edi
        mov   (%edi), %eax

        call  *%eax
        add   $4, %esp

L_InterlockedIncrementFinishedCount:
        mov  %esi, %edi
        add  $FinishedCountAddressLocation, %edi
        lock incl (%edi)

1:
        cli

        hlt
        jmp   1b


//-------------------------------------------------------------------------------------
//  S3SemaphoreStartAddress
//-------------------------------------------------------------------------------------

L_S3SemaphoreStartAddress:
        push %ebp
        mov  %esp, %ebp
        mov  0x8(%ebp), %eax
1:
        cmpl  $0, (%eax)
        jz   1f

        pause
        jmp  1b

1:
        pop %ebp
        ret

//-------------------------------------------------------------------------------
//  S3AsmGetAddressMap
//-------------------------------------------------------------------------------------
.set L_NemInitOffset,                 L_NemInit - L_S3RendezvousFunnelProcStart
.set L_SemaphoreOffset,               L_S3SemaphoreStartAddress - L_S3RendezvousFunnelProcStart

.globl  ASM_PFX(S3AsmGetAddressMap)
ASM_PFX(S3AsmGetAddressMap):
        mov  $L_S3RendezvousFunnelProcStart, %eax
        ret


//-------------------------------------------------------------------------------
//  S3AsmGetPmodeOffset
//-------------------------------------------------------------------------------------
.globl  ASM_PFX(S3AsmGetPmodeOffset)
ASM_PFX(S3AsmGetPmodeOffset):
        mov  $L_NemInitOffset, %eax
        ret

//-------------------------------------------------------------------------------
//  S3AsmGetSemaphoreCheckOffset
//-------------------------------------------------------------------------------------
.globl  ASM_PFX(S3AsmGetSemaphoreCheckOffset)
ASM_PFX(S3AsmGetSemaphoreCheckOffset):
        mov  $L_SemaphoreOffset, %eax
        ret