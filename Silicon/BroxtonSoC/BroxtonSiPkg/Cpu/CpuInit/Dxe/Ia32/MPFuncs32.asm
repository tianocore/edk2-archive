;; @file
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

.686p
.model  flat
.code

include  Htequ.inc
;-------------------------------------------------------------------------------------
FJMP32  MACRO   Selector, Offset
            DB      066h
            DB      067h
            DB      0EAh            ; far jump
            DD      Offset          ; 32-bit offset
            DW      Selector        ; 16-bit selector
            ENDM
FCALL32 MACRO   Selector, Offset
            DB      09Ah
            DD      Offset          ; 32-bit offset
            DW      Selector        ; 16-bit selector
            ENDM
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure serializes all the AP processors through an Init sequence. It must be
;noted that APs arrive here very raw...ie: real mode, no stack.
;ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
;IS IN MACHINE CODE.
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

RendezvousFunnelProc   PROC  near C  PUBLIC
RendezvousFunnelProcStart::


; At this point CS = 0x(vv00) and ip= 0x0.

        db 66h,  08bh, 0e8h           ; mov        ebp, eax

        db 8ch,  0c8h                 ; mov        ax,  cs
        db 8eh,  0d8h                 ; mov        ds,  ax
        db 8eh,  0c0h                 ; mov        es,  ax
        db 8eh,  0d0h                 ; mov        ss,  ax
        db 33h,  0c0h                 ; xor        ax,  ax
        db 8eh,  0e0h                 ; mov        fs,  ax
        db 8eh,  0e8h                 ; mov        gs,  ax

; Get APIC ID
;
        db 66h,  0B8h
        dd 00000001h                  ; mov        eax, 1
        db 0Fh,  0A2h                 ; cpuid
        db 66h,  0C1h, 0EBh, 18h      ; shr        ebx, 24
        db 66h,  81h,  0E3h
        dd 000000FFh                  ; and        ebx, 0ffh                   ; EBX is keeping APIC ID

; If it is the first time AP wakes up, just record AP's BIST
; Otherwise, switch to flat mode

        db 0BEh, 24h, 0Ch             ; mov        si,  InitFlag
        db 66h,  83h, 3Ch, 01h        ; cmp        dword ptr [si], 1
        db 75h                        ; opcode     jnz
        db flat32Start - ($ + 1)      ; jnz        flat32Start

; Record BIST information
;
        db 0B0h, 08h                  ; mov        al,  8
        db 0F6h, 0E3h                 ; mul        bl

        db 0BEh, 2Ch,  0Ch            ; mov        si,  BistBuffer
        db 03h,  0F0h                 ; add        si,  ax

        db 66h,  0C7h, 04h
        dd 00000001h                  ; mov        dword ptr [si], 1           ; Set Valid Flag
        db 66h,  89h,  6Ch,  04h      ; mov        dword ptr [si + 4], ebp     ; Store BIST value

;       cli
;       hlt
;       jmp $-2

; Switch to flat mode.

flat32Start::

        db 0BEh, 1Ch, 0Ch             ; mov        si, BufferStart
        db 66h,  8Bh, 0Ch             ; mov        ecx,dword ptr [si]          ; ECX is keeping the start address of wakeup buffer

        db 0FAh                       ; cli
        db 0BEh, 10h, 0Ch             ; mov        si, GdtrProfile
        db 66h                        ; db         66h
        db 2Eh,0Fh, 01h, 14h          ; lgdt       fword ptr cs:[si]

        db 0BEh, 16h, 0Ch             ; mov        si, IdtrProfile
        db 66h                        ; db         66h
        db 2Eh,0Fh, 01h, 1Ch          ; lidt       fword ptr cs:[si]


        db 33h, 0C0h                  ; xor        ax,  ax
        db 8Eh, 0D8h                  ; mov        ds,  ax
        db 0Fh, 20h, 0C0h             ; mov        eax, cr0                    ; Get control register 0
        db 66h, 83h, 0C8h, 01h        ; or         eax, 000000001h             ; Set PE bit (bit #0)
        db 0Fh, 22h, 0C0h             ; mov        cr0, eax


;step-4:

FLAT32_JUMP::
        FJMP32  010h,0h               ; Far jmp using code segment descriptor

PMODE_ENTRY::                         ; protected mode entry point

        mov         ax,  8h
        mov         ds,  ax
        mov         es,  ax
        mov         fs,  ax

WaitFirstApTaskAssigned::
;
; First INIT-SIPI-SIPI will loop here until DetailedMpInitialization function assigned for each AP
;

        pause
        cmp         dword ptr [ecx+RendezvousProc], 0
        jz          WaitFirstApTaskAssigned

        mov         gs,  ax
        mov         ss,  ax           ; Flat mode setup.

        mov         esi, ecx

        mov         edi, esi
        add         edi, InitFlag
        cmp         dword ptr [edi], 2                ; Check whether in S3 boot path
        jz          ProgramDynamicStack

ProgramStaticStack::

        xor         ecx, ecx
        mov         edi, esi
        add         edi, BistBuffer
        mov         ecx, dword ptr [edi + 8 * ebx]    ; EBX = CpuNumber

        mov         edi, esi
        add         edi, StackSize
        mov         eax, dword ptr [edi]
        inc         ecx
        mul         ecx                               ; EAX = StackSize * (CpuNumber + 1)

        mov         edi, esi
        add         edi, StackStart
        mov         edx, dword ptr [edi]
        add         eax, edx                          ; EAX = StackStart + StackSize * (CpuNumber + 1)

        mov         esp, eax
        sub         esp, MonitorFilterSize            ; Reserved Monitor data space
        or          ebx, StartupApSignal              ; ebx = #Cpu run signature
        jmp         ProgramLocalApic

ProgramDynamicStack::

        mov         edi, esi
        add         edi, LockLocation
        mov         al,  NotVacantFlag
TestLock::
        xchg        byte ptr [edi], al
        cmp         al,  NotVacantFlag
        jz          TestLock

        mov         edi, esi
        add         edi, StackSize
        mov         eax, dword ptr [edi]
        mov         edi, esi
        add         edi, StackStart
        add         eax, dword ptr [edi]
        mov         esp, eax
        mov         dword ptr [edi], eax

Releaselock::
        mov         al,  VacantFlag
        mov         edi, esi
        add         edi, LockLocation
        xchg        byte ptr [edi], al

ProgramLocalApic::

        mov         edi, 0FEE000F0h
        mov         eax, dword ptr [edi]
        and         eax, 0FFFFFD0Fh
        or          eax, 10Fh
        mov         dword ptr [edi], eax

        mov         edi, 0FEE00350h
        mov         eax, dword ptr [edi]
        and         eax, 0FFFE00FFh
        or          eax, 700h
        mov         dword ptr [edi], eax

        mov         edi, 0FEE00360h
        mov         eax, dword ptr [edi]
        and         eax, 0FFFE00FFh
        or          eax, 10400h
        mov         dword ptr [edi], eax

EnableXmm::
        mov        eax, 1
        cpuid
        bt         edx,1Ah
        jnc        @F
        ;
        ; Enable XMM
        ;
        mov eax,cr0
        or  eax, 2
        mov cr0, eax
        mov eax, cr4
        or eax, 600h
        mov cr4, eax

@@:
        ;
        ; Call C Function
        ;
        mov         edi, esi
        add         edi, RendezvousProc
        add         esi, WakeUpApManner               ; esi = WakeUpApManner Address Location

WakeUpThisAp::

        mov         eax, dword ptr [edi]

        test        eax, eax
        jz          CheckWakeUpCounterInit

        push        ebx
        push        ebx
        push        esi
        push        edi

        sub         esp, 20h
        call        eax                           ; Call C function
        add         esp, 20h

        pop         edi
        pop         esi
        pop         ebx
        pop         ebx

CheckWakeUpCounterInit::
        cmp         dword ptr [esi], ApCounterInit
        jnz         CheckWakeUpManner

;
; Initialize MONITOR_MWAIT_DATA data structure per thread
;
        xor         ecx, ecx
        mov         dword ptr [esp +  0], ecx         ; BreakToRunApSignal
        mov         dword ptr [esp +  4], ecx         ; HltLoopBreakCounter
        mov         dword ptr [esp +  8], ecx         ; MwaitLoopBreakCounter
        mov         dword ptr [esp + 12], ecx         ; RunLoopBreakCounter
        mov         dword ptr [esp + 16], ecx         ; MwaitLoopBreakCounter32
        mov         dword ptr [esp + 20], ecx         ; RunLoopBreakCounter32
        mov         dword ptr [esp + 24], ecx         ; WakeUpApVectorChangeFlag
        mov         dword ptr [esp + 28], ecx         ; MwaitTargetCstate

WaitWakeUpMannerAssigned::
        pause
        cmp         dword ptr [esi], ApCounterInit
        jz          WaitWakeUpMannerAssigned

CheckWakeUpManner::

        cmp         dword ptr [esi], ApInHltLoop
        jz          HltApLoop

        cmp         dword ptr [esi], ApInMwaitLoop
        jnz         CheckRunSignal

ApMwaitLoop::

        cli
        mov         eax, esp                          ; Set Monitor Address
        xor         ecx, ecx
        xor         edx, edx
        DB          0fh, 1, 0c8h                      ; MONITOR
        mov         eax, dword ptr [esp + 28]         ; Mwait Target C-State per rax[7:4]
        DB          0fh, 1, 0c9h                      ; MWAIT

CheckRunSignal::

        cmp         dword ptr [esp], ebx              ; Check if run signal correct?
        jnz         CheckWakeUpManner                 ; Unknown break, go checking run manner

        jmp         WakeUpThisAp                      ; Jmp to execute AP task

HltApLoop::

        cli
        hlt
        jmp         HltApLoop

RendezvousFunnelProc   ENDP
RendezvousFunnelProcEnd::
;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
AsmGetAddressMap   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        mov         ebx, dword ptr [ebp+24h]
        mov         dword ptr [ebx], RendezvousFunnelProcStart
        mov         dword ptr [ebx+4h], PMODE_ENTRY - RendezvousFunnelProcStart
        mov         dword ptr [ebx+8h], FLAT32_JUMP - RendezvousFunnelProcStart
        mov         dword ptr [ebx+0ch], RendezvousFunnelProcEnd - RendezvousFunnelProcStart

        popad
        ret
AsmGetAddressMap   ENDP

;-------------------------------------------------------------------------------------
;AsmExchangeRole procedure follows. This procedure executed by current BSP, that is
;about to become an AP. It switches it'stack with the current AP.
;AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo);
;-------------------------------------------------------------------------------------
CPU_SWITCH_STATE_IDLE          equ        0
CPU_SWITCH_STATE_STORED        equ        1
CPU_SWITCH_STATE_LOADED        equ        2

AsmExchangeRole   PROC  near C  PUBLIC
        ; DO NOT call other functions in this function, since 2 CPU may use 1 stack
        ; at the same time. If 1 CPU try to call a functiosn, stack will be corrupted.
        pushad
        mov         ebp,esp

        ; esi contains MyInfo pointer
        mov         esi, dword ptr [ebp+24h]

        ; edi contains OthersInfo pointer
        mov         edi, dword ptr [ebp+28h]

        ;Store EFLAGS, GDTR and IDTR regiter to stack
        pushfd
        sgdt        fword ptr [esi+8]
        sidt        fword ptr [esi+14]

        ; Store the its StackPointer
        mov         dword ptr [esi+4],esp

        ; update its switch state to STORED
        mov         al, NotVacantFlag
TryLock1:
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [esi]
        cmp         al, VacantFlag
        jz          LockObtained1
        PAUSE32
        jmp         TryLock1

LockObtained1:
        mov         byte ptr [esi+1], CPU_SWITCH_STATE_STORED
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [esi]

WaitForOtherStored::
        ; wait until the other CPU finish storing its state
        mov         al, NotVacantFlag
TryLock2:
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [edi]
        cmp         al, VacantFlag
        jz          LockObtained2
        PAUSE32
        jmp         TryLock2

LockObtained2:
        mov         bl, byte ptr [edi+1]
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [edi]
        cmp         bl, CPU_SWITCH_STATE_STORED
        jb          WaitForOtherStored

        ; Since another CPU already stored its state, load them
        ; load GDTR value
        lgdt        fword ptr [edi+8]

        ; load IDTR value
        lidt        fword ptr [edi+14]

        ; load its future StackPointer
        mov         esp, dword ptr [edi+4]

        ; update its switch state to LOADED
        mov         al, NotVacantFlag
TryLock3:
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [esi]
        cmp         al, VacantFlag
        jz          LockObtained3
        PAUSE32
        jmp         TryLock3

LockObtained3:
        mov         byte ptr [esi+1], CPU_SWITCH_STATE_LOADED
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [esi]

WaitForOtherLoaded::
        ; wait until the other CPU finish loading new state,
        ; otherwise the data in stack may corrupt
        mov         al, NotVacantFlag
TryLock4:
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [edi]
        cmp         al, VacantFlag
        jz          LockObtained4
        PAUSE32
        jmp         TryLock4

LockObtained4:
        mov         bl, byte ptr [edi+1]
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [edi]
        cmp         bl, CPU_SWITCH_STATE_LOADED
        jb          WaitForOtherLoaded

        ; since the other CPU already get the data it want, leave this procedure
        popfd

        popad
        ret
AsmExchangeRole   ENDP
END

