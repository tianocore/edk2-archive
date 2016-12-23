## @file
#  This is the assembly code for EM64T MP support.
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
//RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
//procedure serializes all the AP processors through an Init sequence. It must be
//noted that APs arrive here very raw...ie: real mode, no stack.
//ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
//IS IN MACHINE CODE.
//-------------------------------------------------------------------------------------
//RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

.set L_RunLoopAndMwaitLoop32Offset, L_RunLoopAndMwaitLoop32 - L_LongModeStart
.set L_RunLoopAndMwaitLoop32JumpOffset, L_RunLoopAndMwaitLoop32Jump - L_LongModeStart
.set L_RunLoopAndMwaitLoop64Offset, L_RunLoopAndMwaitLoop64 - L_LongModeStart
.set L_RunLoopAndMwaitLoop64JumpOffset, L_RunLoopAndMwaitLoop64Jump - L_LongModeStart
.set L_LongModeStartJumpOffset, L_LongModeStartJump - L_RendezvousFunnelProcStart

ASM_GLOBAL ASM_PFX(CpuInitFloatPointUnit)

.globl ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
L_RendezvousFunnelProcStart:

// At this point CS = 0x(vv00) and ip= 0x0.
        .byte 0x66, 0x8b, 0xe8            // mov        ebp, eax

        .byte 0x8c, 0xc8                  // mov        ax,  cs
        .byte 0x8e, 0xd8                  // mov        ds,  ax
        .byte 0x8e, 0xc0                  // mov        es,  ax
        .byte 0x8e, 0xd0                  // mov        ss,  ax
        .byte 0x33, 0xc0                  // xor        ax,  ax
        .byte 0x8e, 0xe0                  // mov        fs,  ax
        .byte 0x8e, 0xe8                  // mov        gs,  ax

// Get APIC ID
//
        .byte 0x66, 0xB8
        .long 0x00000001                  // mov        eax, 1
        .byte 0x0F, 0xA2                  // cpuid
        .byte 0x66, 0xC1, 0xEB, 0x18      // shr        ebx, 24
        .byte 0x66, 0x81, 0xE3
        .long 0x000000FF                  // and        ebx, 0ffh                   // EBX is APIC ID

// If it is the first time AP wakes up, just record AP's BIST
// Otherwise, switch to protected mode.

        .byte 0xBE                        // opcode of mov si, imm16
        .short InitFlagLocation           // mov        si,  InitFlag
        .byte 0x66, 0x83, 0x3C, 0x00      // cmp        dword ptr [si], 0
        .byte 0x74, 0x14                  // jz         flat32Start

// Record BIST information
//
        .byte 0xB0, 0x08                  // mov        al,  8
        .byte 0xF6, 0xE3                  // mul        bl

        .byte 0xBE                        // mov        si, imm16
        .short BistBuffer                 // mov        si,  BistBuffer
        .byte 0x03, 0xF0                  // add        si,  ax

        .byte 0x66, 0xC7, 0x04
        .long 0x00000001                  // mov        dword ptr [si], 1           // Set Valid Flag
        .byte 0x66, 0x89, 0x6C, 0x04      // mov        dword ptr [si + 4], ebp     // Store BIST value

// Switch to flat mode.

L_flat32Start:

        .byte 0xBF                         // opcode of mov di, imm16
        .short BufferStartLocation         // mov        di, BufferStartLocation
        .byte 0x66, 0x8B, 0x35             // mov        esi,dword ptr [di]          // ESI is keeping the start address of wakeup buffer

        .byte 0xBF                         // opcode of mov di, imm16
        .short Cr3OffsetLocation           // mov        di, Cr3Location
        .byte 0x66, 0x8B, 0x0D             // mov        ecx,dword ptr [di]          // ECX is keeping the value of CR3

        .byte 0xBF                         // opcode of mov di, imm16
        .short GdtrLocation                // mov        di, GdtrProfile
        .byte 0x66                         // db         66h
        .byte 0x2E, 0x0F, 0x01, 0x15       // lgdt       fword ptr cs:[di]

        .byte 0xBF                         // opcode of mov di, imm16
        .short IdtrLocation                // mov        di, IdtrProfile
        .byte 0x66                         // db         66h
        .byte 0x2E, 0x0F, 0x01, 0x1D       // lidt       fword ptr cs:[di]

        .byte 0xBF                         // opcode of mov di, imm16
        .short L_LongModeStartJumpOffset   // Get offset of LongModeStartJump
        .byte 0x66,  0x8B, 0x3D            // mov        edi,dword ptr [di]          // EDI is keeping the LongModeStart Jump Address

        .byte 0x31,  0xC0                  // xor        ax,  ax
        .byte 0x8E,  0xD8                  // mov        ds,  ax

        .byte 0x0F,  0x20, 0xC0            // mov        eax, cr0                    // Get control register 0
        .byte 0x66,  0x83, 0xC8, 0x03      // or         eax, 000000003h             // Set PE bit (bit //0) and MP
        .byte 0x0F,  0x22, 0xC0            // mov        cr0, eax

L_FLAT32_JUMP:

        .byte 0x66,  0x67, 0xEA            // far jump
        .long 0x0                          // 32-bit offset
        .short 0x20                        // 16-bit selector

L_NemInit:                                 // 32-bit protected mode entry point
        .byte 0x66,  0xB8, 0x18, 0x00      // mov        ax,  18h
        .byte 0x66,  0x8E, 0xD8            // mov        ds,  ax
        .byte 0x66,  0x8E, 0xC0            // mov        es,  ax
        .byte 0x66,  0x8E, 0xE0            // mov        fs,  ax
        .byte 0x66,  0x8E, 0xE8            // mov        gs,  ax
        .byte 0x66,  0x8E, 0xD0            // mov        ss,  ax                     // Flat mode setup.

L_PrepareToGoLongMode64:
        .byte 0x0F,  0x20, 0xE0            // mov        eax, cr4
        .byte 0x66,  0x0D, 0x20, 0x06      // or         ax,  0620h
        .byte 0x0F,  0x22, 0xE0            // mov        cr4, eax

        .byte 0x0F,  0x22, 0xD9            // mov        cr3, ecx

        .byte 0xB9
        .long 0xC0000080                   // mov        ecx, 0c0000080h             // EFER MSR number.
        .byte 0x0F,  0x32                  // rdmsr                                  // Read EFER.
        .byte 0x0F,  0xBA, 0xE8, 0x08      // bts        eax, 8                      // Set LME=1.
        .byte 0x0F,  0x30                  // wrmsr                                  // Write EFER.

        .byte 0x0F,  0x20, 0xC0            // mov        eax, cr0                    // Read CR0.
        .byte 0x0F,  0xBA, 0xE8, 0x1F      // bts        eax, 31                     // Set PG=1.
        .byte 0x0F,  0x22, 0xC0            // mov        cr0, eax                    // Write CR0.

L_LONG_JUMP:

        .byte 0x67,  0xEA                 // far jump

L_LongModeStartJump:

        .long 0x0                         // 32-bit offset
        .short 0x38                       // 16-bit selector

L_LongModeStart:

        mov         $0x30, %ax
        mov         %ax, %ds
        mov         %ax, %es
        mov         %ax, %ss

L_WaitFirstApTaskAssigned:
//
// First INIT-SIPI-SIPI will loop here until DetailedMpInitialization function assigned for each AP
//
        pause
        cmpq        $0, CProcedureLocation(%esi)
        jz          L_WaitFirstApTaskAssigned

//
// Patch Addresses for jumping between RUN and MONITOR MWAIT loops 32-bits and Long Monde Procedure 64-bits
// Based on the running address of LongModeStart in physic memory which was actually copied by CPU DXE INIT
//
        xor         %rdx, %rdx
        mov         %edi, %eax
        add         $L_RunLoopAndMwaitLoop32Offset, %eax
        mov         %edi, %edx
        add         $L_RunLoopAndMwaitLoop32JumpOffset, %edx
        mov         %eax, (%rdx)

        mov         %rdx, %rbp                        // RBP = 32-bits compatibility mode FAR JUMP m16:32 operand pointer

        mov         %edi, %eax
        add         $L_RunLoopAndMwaitLoop64Offset, %eax
        mov         %edi, %edx
        add         $L_RunLoopAndMwaitLoop64JumpOffset, %edx
        mov         %eax, (%rdx)

        //
        // ProgramStack
        //
        xor         %rcx, %rcx
        mov         %esi, %edi
        add         $BistBuffer, %edi
        mov         (%edi,%ebx,8), %ecx               // RCX = CpuNumber

        mov         %esi, %edi
        add         $StackSizeLocation, %edi
        mov        (%edi), %rax
        inc         %rcx
        mul         %rcx                              // RAX = StackSize * (CpuNumber + 1)

        mov         %esi, %edi
        add         $StackStartAddressLocation, %edi
        mov         (%edi), %rdx
        add         %rdx, %rax                        // RAX = StackStart + StackSize * (CpuNumber + 1)

        mov         %rax, %rsp
        sub         $MonitorFilterSize, %rsp           // Reserved Monitor data space
        or          $BreakToRunApSignal, %ebx          // ebx = #Cpu run signature

        //
        // Call assembly function to initialize FPU.
        //
        mov         ASM_PFX(gCpuInitFloatPointUnit)(%rip), %rax
        sub         $0x20, %rsp
        callq       *%rax
        add         $0x20, %rsp

        //
        // Load C Function pointer and wakeup manner location
        //
        mov         %esi, %edi
        add         $CProcedureLocation, %edi
        add         $WakeUpApManner, %esi               // esi = WakeUpApManner Address Location

L_WakeUpThisAp64:

        movq        (%edi),%rax

        test        %rax, %rax
        je          L_CheckWakeUpCounterInit64

        push        %rbp
        push        %rbx
        push        %rsi
        push        %rdi

        sub         $0x20, %rsp
        callq       *%rax
        add         $0x20, %rsp

        pop         %rdi
        pop         %rsi
        pop         %rbx
        pop         %rbp

L_CheckWakeUpCounterInit64:

        cmpl        $WakeUpApCounterInit, (%esi)
        jnz         L_CheckWakeUpManner64

        //
        // Initialize MONITOR_MWAIT_DATA data structure per thread
        //
        xor         %rcx, %rcx
        mov         %rcx, (%rsp)         // BreakToRunApSignal
        mov         %rcx, 0x8(%rsp)      // HltLoopBreakCounter
        mov         %rcx, 0x10(%rsp)     // MwaitLoopBreakCounter
        mov         %rcx, 0x18(%rsp)     // RunLoopBreakCounter
        mov         %rcx, 0x20(%rsp)     // MwaitLoopBreakCounter32
        mov         %rcx, 0x28(%rsp)     // RunLoopBreakCounter32
        mov         %rcx, 0x30(%rsp)     // WakeUpApVectorChangeFlag
        mov         %rcx, 0x38(%rsp)     // MwaitTargetCstate

L_WaitWakeUpMannerAssigned:

        pause
        cmpl        $WakeUpApCounterInit, (%esi)
        jz          L_WaitWakeUpMannerAssigned

L_CheckWakeUpManner64:

        pause
        mov         (%esi), %edx
        cmp         $WakeUpApPerHltLoop, %edx
        jz          L_HltApLoop64

        cmp         $WakeUpApPerMwaitLoop, %edx
        jz          L_ApMwaitLoop64

        cmp         $WakeUpApPerRunLoop, %edx
        jz          L_CheckRunSignal64

        jmp         L_JumpToCompatibility32Mode

L_ApMwaitLoop64:

        cli
        mov         %rsp, %rax                        // Set Monitor Address
        xor         %rcx, %rcx
        xor         %rdx, %rdx
        .byte       0x0F, 1, 0xC8                     // MONITOR
        mov         0x38(%rsp), %rax                  // Mwait Target C-State per rax[7:4]
        .byte       0x0F, 1, 0xC9                     // MWAIT

L_CheckRunSignal64:

        cmp         %rbx, (%rsp)                      // Check if run signal correct?
        jnz         L_CheckWakeUpManner64             // Unknown break, go checking run manner

        jmp         L_WakeUpThisAp64                  // Jmp to execute AP task

L_HltApLoop64:
        cli
        hlt
        jmp         L_HltApLoop64                     // Jump to halt loop

L_JumpToCompatibility32Mode:

        .byte 0xFF, 0x6D, 0x00       // jmp pword ptr [rbp+0]   // Far jump to m16:32 for 32-bits compatibility mode

L_RunLoopAndMwaitLoop32Jump:

        .long  0x00                                    // m32 part of m16:32
        .short 0x20                                    // m16 part of m16:32

L_RunLoopAndMwaitLoop32:

        .byte 0x66,  0xB8, 0x18,  0x00      // mov        ax,  18h
        .byte 0x66,  0x8E, 0xD8             // mov        ds,  ax
        .byte 0x8e,  0xd0                   // mov        ss,  ax

        .byte 0xF,  0x20, 0xC0              // mov        eax, cr0                    // Read CR0.
        .byte 0xF,  0xBA, 0xF0, 0x1F        // btr        eax, 31                     // Reset PG=0.
        .byte 0xF,  0x22, 0xC0              // mov        cr0, eax                    // Write CR0.

        .byte 0xB9
        .long 0xC0000080                 // mov        ecx, 0c0000080h             // EFER MSR number.
        .byte 0xF,  0x32                 // rdmsr                                  // Read EFER.
        .byte 0xF,  0xBA, 0xF0, 0x8      // btr        eax, 8                      // Reset LME=0.
        .byte 0xF,  0x30                 // wrmsr                                  // Write EFER.

        .byte 0xF,  0x20,  0xE0          // mov        eax, cr4
        .byte 0x24,  0xDF                // and        al,  0DFh                   // Reset PAE=0 in CR4 bit 5
        .byte 0xF,  0x22,  0xE0          // mov        cr4, eax

L_CheckWakeUpManner32:

        pause
        cmpq         $WakeUpApPerMwaitLoop32, (%rsi)                    // Use rsi for esi per compling in 64-bits mode
        jnz         L_CheckRunSignal32

        cli
        mov         %esp, %eax             // Set Monitor Address
        xor         %ecx, %ecx
        xor         %edx, %edx
        .byte       0xf, 1, 0xc8           // MONITOR
        mov         0x38(%rsp), %eax       // Mwait Target C-State per eax[7:4]
        .byte       0xf, 1, 0xc9           // MWAIT


L_CheckRunSignal32:

        cmp         %ebx, (%rsp)           // Check if run signal correct?
        jnz         L_CheckWakeUpManner32  // Unknown break, go checking run manner

        .byte 0xF,  0x20,  0xE0            // mov        eax, cr4
        .byte 0xC,  0x20                   // or         al,  20h                   // Set PAE=1 in CR4 bit 5
        .byte 0xF,  0x22,  0xE0            // mov        cr4, eax

        .byte 0xB9
        .long 0xC0000080                   // mov        ecx, 0c0000080h             // EFER MSR number.
        .byte 0xF,  0x32                   // rdmsr                                  // Read EFER.
        .byte 0xF,  0xBA, 0xE8, 0x8        // bts        eax, 8                      // Set LME=1.
        .byte 0xF,  0x30                   // wrmsr                                  // Write EFER.

        .byte 0xF,  0x20,  0xC0            // mov        eax, cr0                    // Read CR0.
        .byte 0xF,  0xBA,  0xE8, 0x1F      // bts        eax, 31                     // Set PG=1.
        .byte 0xF,  0x22,  0xC0            // mov        cr0, eax                    // Write CR0.

        .byte 0x67,  0xEA                  // far jump back to 64-bits long mode

L_RunLoopAndMwaitLoop64Jump:

        .long  0x00                        // 32-bit offset
        .short 0x38                        // 16-bit selector

L_RunLoopAndMwaitLoop64:

        mov         $0x30, %ax
        mov         %ax, %ds
        mov         %ax, %ss

        jmp         L_WakeUpThisAp64

.globl ASM_PFX(gCpuInitFloatPointUnit)
ASM_PFX(gCpuInitFloatPointUnit):
.long 0x0
.long 0x0

L_RendezvousFunnelProcEnd:

//-------------------------------------------------------------------------------------
//  AsmGetAddressMap (&AddressMap);
//-------------------------------------------------------------------------------------
.set L_NemInitOffset,                 L_NemInit - L_RendezvousFunnelProcStart
.set L_FLAT32_JUMPOffset,             L_FLAT32_JUMP - L_RendezvousFunnelProcStart
.set L_LongModeStartOffset,           L_LongModeStart - L_RendezvousFunnelProcStart
.set L_LONG_JUMPOffset,               L_LONG_JUMP - L_RendezvousFunnelProcStart
.set L_RendezvousFunnelProcEndOffset, L_RendezvousFunnelProcEnd - L_RendezvousFunnelProcStart

.globl ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
        leaq        ASM_PFX(RendezvousFunnelProc)(%rip), %rax
        mov         %rax, (%rcx)
        xor         %rax, %rax
        mov         $L_NemInitOffset, %eax
        mov         %rax, 0x8(%rcx)
        mov         $L_FLAT32_JUMPOffset, %eax
        mov         %rax, 0x10(%rcx)
        mov         $L_LongModeStartOffset, %eax
        mov         %rax, 0x18(%rcx)
        mov         $L_LONG_JUMPOffset, %eax
        mov         %rax, 0x20(%rcx)
        mov         $L_RendezvousFunnelProcEndOffset, %eax
        mov         %rax, 0x28(%rcx)
        retq

#undef EFI_SUCCESS
#undef EFI_NOT_READY
#define EFI_SUCCESS   0
#define EFI_NOT_READY 0x8000000000000006

.globl ASM_PFX(AsmTryAcquireMPLock)
ASM_PFX(AsmTryAcquireMPLock):

        mov         $NotVacantFlag, %al

        xchg        %al, (%rcx)
        cmp         $VacantFlag, %al
        jz          L_Vacant

        mov         $EFI_NOT_READY, %rax
        jmp         L_Done

L_Vacant:
        mov         $EFI_SUCCESS, %rax
L_Done:
        retq

.globl ASM_PFX(AsmAcquireMPLock)
ASM_PFX(AsmAcquireMPLock):

        mov         $NotVacantFlag, %al
L_TryGetLock:
        xchg        %al, (%rcx)
        cmp         $VacantFlag, %al
        jz          L_LockObtained

        pause
        jmp         L_TryGetLock

L_LockObtained:
        retq

.globl ASM_PFX(AsmReleaseMPLock)
ASM_PFX(AsmReleaseMPLock):

        mov         $VacantFlag, %al
        xchg        %al, (%rcx)

        retq

//-------------------------------------------------------------------------------------
//AsmExchangeRole procedure follows. This procedure executed by current BSP, that is
//about to become an AP. It switches it'stack with the current AP.
//AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo);
//-------------------------------------------------------------------------------------
#define CPU_SWITCH_STATE_IDLE   0
#define CPU_SWITCH_STATE_STORED 1
#define CPU_SWITCH_STATE_LOADED 2

.globl ASM_PFX(AsmExchangeRole)
ASM_PFX(AsmExchangeRole):
        // DO NOT call other functions in this function, since 2 CPU may use 1 stack
        // at the same time. If 1 CPU try to call a functiosn, stack will be corrupted.

        push        %rax
        push        %rbx
        push        %rcx
        push        %rdx
        push        %rsi
        push        %rdi
        push        %rbp
        push        %r8
        push        %r9
        push        %r10
        push        %r11
        push        %r12
        push        %r13
        push        %r14
        push        %r15

        mov         %cr0, %rax
        push        %rax

        mov         %cr4, %rax
        push        %rax

        // rsi contains MyInfo pointer
        mov         %rcx, %rsi

        // rdi contains OthersInfo pointer
        mov         %rdx, %rdi

        //Store EFLAGS, GDTR and IDTR regiter to stack
        pushfq
        sgdt        16(%rsi)
        sidt        26(%rsi)

        // Store the its StackPointer
        mov         %rsp, 8(%rsi)

        // update its switch state to STORED
        mov         $NotVacantFlag, %al
L_TryLock1:
        xchg   %al, (%rsi)
        cmp         $VacantFlag, %al
        jz          L_LockObtained1
        pause
        jmp         L_TryLock1

L_LockObtained1:
        movb        $CPU_SWITCH_STATE_STORED, 1(%rsi)
        xchg   %al, (%rsi)

L_WaitForOtherStored:
        // wait until the other CPU finish storing its state
        mov         $NotVacantFlag, %al
L_TryLock2:
        xchg   %al, (%rdi)
        cmp         $VacantFlag, %al
        jz          L_LockObtained2
        pause
        jmp         L_TryLock2

L_LockObtained2:
        mov         1(%rdi), %bl
        xchg   %al, (%rdi)
        cmp         $CPU_SWITCH_STATE_STORED, %bl
        jb          L_WaitForOtherStored

        // Since another CPU already stored its state, load them
        // load GDTR value
        lgdt        16(%rdi)

        // load IDTR value
        lidt        26(%rdi)

        // load its future StackPointer
        mov         8(%rdi), %rsp

        // update its switch state to LOADED
        mov         $NotVacantFlag, %al
L_TryLock3:
        xchg   %al, (%rsi)
        cmp         $VacantFlag, %al
        jz          L_LockObtained3
        pause
        jmp         L_TryLock3

L_LockObtained3:
        movb        $CPU_SWITCH_STATE_LOADED, 1(%rsi)
        xchg   %al, (%rsi)

L_WaitForOtherLoaded:
        // wait until the other CPU finish loading new state,
        // otherwise the data in stack may corrupt
        mov         $NotVacantFlag, %al
L_TryLock4:
        xchg   %al, (%rdi)
        cmp         $VacantFlag, %al
        jz          L_LockObtained4
        pause
        jmp         L_TryLock4

L_LockObtained4:
        mov         1(%rdi), %bl
        xchg   %al, (%rdi)
        cmp         $CPU_SWITCH_STATE_LOADED, %bl
        jb          L_WaitForOtherLoaded

        // since the other CPU already get the data it want, leave this procedure
        popfq

        pop         %rax
        mov         %rax, %cr4

        pop         %rax
        mov         %rax, %cr0

        pop         %r15
        pop         %r14
        pop         %r13
        pop         %r12
        pop         %r11
        pop         %r10
        pop         %r9
        pop         %r8
        pop         %rbp
        pop         %rdi
        pop         %rsi
        pop         %rdx
        pop         %rcx
        pop         %rbx
        pop         %rax

        retq
