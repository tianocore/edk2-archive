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

#-------------------------------------------------------------------------------------

.macro  PAUSE32
            .byte 0xF3
            .byte 0x90
.endm

.macro FJMP32 Selector, Offset
            .byte      0x066
            .byte      0x067
            .byte      0x0EA            # far jump
            .long      \Offset          # 32-bit offset
            .word      \Selector        # 16-bit selector
.endm
 
.macro FCALL32 Selector, Offset
            .byte      0x09A
            .long      \Offset          # 32-bit offset
            .word      \Selector        # 16-bit selector
.endm

#-------------------------------------------------------------------------------------
#RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
#procedure serializes all the AP processors through an Init sequence. It must be
#noted that APs arrive here very raw...ie: real mode, no stack.
#ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
#IS IN MACHINE CODE.
#-------------------------------------------------------------------------------------
#RendezvousFunnelProc (&WakeUpBuffer,MemAddress)

ASM_GLOBAL ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:

# At this point CS = 0x(vv00) and ip= 0x0.

        .byte 0x66,0x8b,0xe8          # mov        ebp, eax

        .byte 0x8c,0xc8               # mov        ax,  cs
        .byte 0x8e,0xd8               # mov        ds,  ax
        .byte 0x8e,0xc0               # mov        es,  ax
        .byte 0x8e,0xd0               # mov        ss,  ax
        .byte 0x33,0xc0               # xor        ax,  ax
        .byte 0x8e,0xe0               # mov        fs,  ax
        .byte 0x8e,0xe8               # mov        gs,  ax

# Get APIC ID
#
        .byte 0x66,  0xB8
        .long 0x00000001                 # mov        %eax, 1
        .byte 0x0F,  0xA2                # cpuid
        .byte 0x66,  0xC1, 0xEB, 0x18    # shr        %ebx, 24
        .byte 0x66,  0x81, 0xE3
        .long 0x000000FF                 # and        %ebx, 0ffh                   # EBX is keeping APIC ID

# If it is the first time AP wakes up, just record APs BIST
# Otherwise, switch to flat mode

        .byte 0xBE, 0x24, 0x0C           # mov        si,  InitFlag
        .byte 0x66, 0x83, 0x3C, 0x01     # cmp        dword ptr [si], 1
        .byte 0x75, 0x18                 # jnz        flat32Start

# Record BIST information
#
        .byte 0xB0, 0x08                 # mov        al,  8
        .byte 0xF6, 0xE3                 # mul        bl
        
        .byte 0xBE, 0x2C,  0x0C          # mov        si,  BistBuffer
        .byte 0x03, 0xF0                 # add        si,  ax

        .byte 0x66, 0xC7, 0x04
        .long 0x00000001                 # mov        dword ptr [si], 1           # Set Valid Flag
        .byte 0x66, 0x89, 0x6C, 0x04     # mov        dword ptr [si + 4], ebp     # Store BIST value        
        
        cli
        hlt
        jmp .-2
                                      
# Switch to flat mode.

flat32Start:

        .byte 0xBE, 0x1C, 0x0C           # mov        si, BufferStart
        .byte 0x66, 0x8B, 0x0C           # mov        ecx,dword ptr [si]          # ECX is keeping the start address of wakeup buffer

        .byte 0xFA                       # cli
        .byte 0xBE, 0x10, 0x0C           # mov        si, GdtrProfile
        .byte 0x66                       # db         66h
        .byte 0x2E, 0x0F, 0x01, 0x14     # lgdt       fword ptr cs:[si]
        
        .byte 0xBE, 0x16, 0x0C           # mov        si, IdtrProfile
        .byte 0x66                       # db         66h
        .byte 0x2E, 0x0F, 0x01, 0x1C     # lidt       fword ptr cs:[si]
        
                                                
        .byte 0x33, 0xC0                 # xor        ax,  ax
        .byte 0x8E, 0xD8                 # mov        ds,  ax
        .byte 0x0F, 0x20, 0xC0           # mov        eax, cr0                    # Get control register 0
        .byte 0x66, 0x83, 0xC8, 0x01     # or         eax, 000000001h             # Set PE bit (bit #0)
        .byte 0x0F, 0x22, 0xC0           # mov        cr0, eax


#step-4:

FLAT32_JUMP:
        FJMP32  0x10, 0x0                # Far jmp using code segment descriptor

PMODE_ENTRY:                             # protected mode entry point

        movw        $0x8, %ax
        movw        %ax,  %ds
        movw        %ax,  %es
        movw        %ax,  %fs
        movw        %ax,  %gs
        movw        %ax,  %ss            # Flat mode setup.

        movl        %ecx,%esi

        movl        %esi,%edi
        addl        $InitFlag, %edi
        cmpl        $2, (%edi)           # Check whether in S3 boot path
        jz          ProgramDynamicStack

ProgramStaticStack:

        xorl        %ecx, %ecx
        movl        %esi, %edi
        addl        $BistBuffer, %edi
        movl        (%edi, %ebx, 8), %ecx             # EBX = CpuNumber
        
        movl        %esi, %edi
        addl        $StackSize, %edi
        movl        (%edi), %eax
        incl        %ecx    
        mull        %ecx                              # EAX = StackSize * (CpuNumber + 1)
        
        movl        %esi, %edi
        addl        $StackStart, %edi
        movl        (%edi), %edx
        addl        %edx, %eax                        # EAX = StackStart + StackSize * (CpuNumber + 1)
        
        movl        %eax, %esp
        subl        $MonitorFilterSize, %esp          # Reserved Monitor data space
        orl         $StartupApSignal, %ebx            # EBX = #Cpu run signature
        jmp         ProgramLocalApic
        
ProgramDynamicStack:

        movl        %esi, %edi
        addl        $LockLocation, %edi
        movb        $NotVacantFlag, %al
TestLock:
        xchgb       %al, (%edi)
        cmpb        $NotVacantFlag, %al
        jz          TestLock

        movl        %esi, %edi
        addl        $StackSize, %edi
        movl        (%edi), %eax
        movl        %esi, %edi
        addl        $StackStart, %edi
        addl        (%edi), %eax
        movl        %eax, %esp
        movl        %eax, (%edi)

Releaselock:
        movb        $VacantFlag, %al
        movl        %esi, %edi
        addl        $LockLocation, %edi
        xchgb       %al, (%edi)

ProgramLocalApic:

        movl        $0x0FEE000F0, %edi
        movl        (%edi), %eax
        andl        $0x0FFFFFD0F, %eax
        orl         $0x10F, %eax
        movl        %eax, (%edi)

        movl        $0x0FEE00350, %edi
        movl        (%edi), %eax
        andl        $0x0FFFE00FF, %eax
        orl         $0x700, %eax
        movl        %eax, (%edi) 

        movl        $0x0FEE00360, %edi
        movl        (%edi), %eax
        andl        $0x0FFFE00FF, %eax
        orl         $0x10400, %eax
        movl        %eax, (%edi)

EnableXmm:
        movl        $1, %eax
        cpuid
        btl         $0x1A, %edx
        jnc         L1
        #
        # Enable XMM
        #
        movl %cr0, %eax
        orl  $2, %eax
        movl %eax, %cr0 
        movl %cr4, %eax
        orl  $0x600, %eax
        movl %eax, %cr4

L1:
        #
        # Call C Function
        #
        movl         %esi, %edi
        addl         $RendezvousProc, %edi 
        addl         $WakeUpApManner, %esi                # esi = WakeUpApManner Address Location

WakeUpThisAp:

        movl         (%edi), %eax

        testl        %eax, %eax
        jz          CheckWakeUpCounterInit

        push        %ebx
        push        %ebx
        push        %esi
        push        %edi

        subl        $0x20, %esp
        call        *%eax                           # Call C function
        addl        $0x20, %esp

        pop         %edi
        pop         %esi
        pop         %ebx
        pop         %ebx

CheckWakeUpCounterInit:
        cmpl        $ApCounterInit, (%esi)
        jnz         CheckWakeUpManner

#
# Initialize MONITOR_MWAIT_DATA data structure per thread
#
        xorl        %ecx, %ecx
        movl        %ecx, 0(%esp)         # BreakToRunApSignal
        movl        %ecx, 4(%esp)         # HltLoopBreakCounter
        movl        %ecx, 8(%esp)         # MwaitLoopBreakCounter
        movl        %ecx, 12(%esp)        # RunLoopBreakCounter
        movl        %ecx, 16(%esp)        # WakeUpApVectorChangeFlag
        movl        %ecx, 20(%esp)        # MwaitTargetCstate

CheckWakeUpManner:

        cmpl        $ApInHltLoop, (%esi)
        jz          HltApLoop

        cmpl        $ApInMwaitLoop, (%esi)
        jnz         CheckRunSignal

ApMwaitLoop:

        cli
        movl        %esp, %eax            # Set Monitor Address
        xorl        %ecx, %ecx
        xorl        %edx, %edx
        .byte       0x0f, 0x1, 0xc8       # MONITOR
        movl        20(%esp), %eax        # Mwait Target C-State per rax[7:4]
        .byte       0x0f, 0x1, 0xc9       # MWAIT

CheckRunSignal:

        cmpl        %ebx, (%esp)          # Check if run sign%al correct?
        jnz         CheckWakeUpManner     # Unknown break, go checking run manner

        jmp         WakeUpThisAp          # Jmp to execute AP task

HltApLoop:

        cli
        hlt
        jmp         HltApLoop
        
#RendezvousFunnelProc   ENDP
RendezvousFunnelProcEnd:

#-------------------------------------------------------------------------------------
#  AsmGetAddressMap (&AddressMap)
#-------------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
        pushal
        movl         %esp, %ebp
   
        movl         0x24(%ebp), %ebx
        movl         $RendezvousFunnelProcStart, (%ebx)
        movl         $(PMODE_ENTRY - RendezvousFunnelProcStart), 0x4(%ebx) 
        movl         $(FLAT32_JUMP - RendezvousFunnelProcStart), 0x8(%ebx)
        movl         $(RendezvousFunnelProcEnd - RendezvousFunnelProcStart), 0xc(%ebx)
        
        popal
        ret
#AsmGetAddressMap   ENDP

#-------------------------------------------------------------------------------------
#AsmExchangeRole procedure follows. This procedure executed by current BSP, that is 
#about to become an AP. It switches its stack with the current AP.
#AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo)#
#-------------------------------------------------------------------------------------
.equ    CPU_SWITCH_STATE_IDLE,          0
.equ    CPU_SWITCH_STATE_STORED,        1
.equ    CPU_SWITCH_STATE_LOADED,        2

ASM_GLOBAL ASM_PFX(AsmExchangeRole)
ASM_PFX(AsmExchangeRole):
        # DO NOT call other functions in this function, since 2 CPU may use 1 stack
        # at the same time. If 1 CPU try to call a functiosn, stack will be corrupted.
        pushal
        movl         %esp, %ebp

        # %esi contains MyInfo pointer
        movl         0x24(%ebp), %esi

        # %edi contains OthersInfo pointer
        movl         0x28(%ebp), %edi

        #Store EFLAGS, GDTR and IDTR regiter to stack
        pushfl
        sgdt        8(%esi)
        sidt        14(%esi)
        
        # Store the its StackPointer
        movl        %esp, 4(%esi)

        # update its switch state to STORED
        movb        $NotVacantFlag, %al
TryLock1:
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%esi), %al
        cmpb        $VacantFlag, %al
        jz          LockObtained1
        PAUSE32
        jmp         TryLock1
        
LockObtained1:
        movb        $CPU_SWITCH_STATE_STORED, 1(%esi)
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%esi), %al
WaitForOtherStored:        
        # wait until the other CPU finish storing its state
        movb        $NotVacantFlag, %al
TryLock2:
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%edi), %al
        cmpb        $VacantFlag, %al
        jz          LockObtained2
        PAUSE32
        jmp         TryLock2
        
LockObtained2:
        movb        1(%edi), %bl
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%edi),  %al
        cmpb        $CPU_SWITCH_STATE_STORED, %bl
        jb          WaitForOtherStored
        
        # Since another CPU already stored its state, load them
        # load GDTR value
        lgdt        8(%edi)
        
        # load IDTR value
        lidt        14(%edi)

        # load its future StackPointer
        movl        4(%edi), %esp
                
        # update its switch state to LOADED
        movb        $NotVacantFlag, %al
TryLock3:
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%esi), %al
        cmpb        $VacantFlag, %al
        jz          LockObtained3
        PAUSE32
        jmp         TryLock3
        
LockObtained3:
        movb        $CPU_SWITCH_STATE_LOADED, 1(%esi)
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%esi), %al

WaitForOtherLoaded:
        # wait until the other CPU finish loading new state,
        # otherwise the data in stack may corrupt
        movb        $NotVacantFlag, %al
TryLock4:
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%edi), %al
        cmpb        $VacantFlag, %al
        jz          LockObtained4
        PAUSE32
        jmp         TryLock4
        
LockObtained4:
        movb        1(%edi), %bl
        .byte 0xF0                        # opcode for lock instruction
        xchgb       (%edi), %al
        cmpb        $CPU_SWITCH_STATE_LOADED, %bl
        jb          WaitForOtherLoaded

        # since the other CPU already get the data it want, leave this procedure
        popfl

        popal
        ret
#AsmExchangeRole   ENDP

