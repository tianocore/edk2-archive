;; @file
;  This is the assembly code for EM64T MP support.
;
;  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>
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

include  MpEqu.inc
CpuInitFloatPointUnit PROTO C

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure serializes all the AP processors through an Init sequence. It must be
;noted that APs arrive here very raw...ie: real mode, no stack.
;ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
;IS IN MACHINE CODE.
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

text      SEGMENT

RendezvousFunnelProc   PROC  PUBLIC
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
        dd 000000FFh                  ; and        ebx, 0ffh                   ; EBX is APIC ID

; If it is the first time AP wakes up, just record AP's BIST
; Otherwise, switch to protected mode.

        db 0BEh                       ; opcode of mov si, imm16
        dw InitFlagLocation           ; mov        si,  InitFlag
        db 66h,  83h, 3Ch, 00h        ; cmp        dword ptr [si], 0
        db 74h                        ; opcode of jz
        db flat32Start - ($ + 1)      ; jz         flat32Start

; Record BIST information
;
        db 0B0h, 08h                  ; mov        al,  8
        db 0F6h, 0E3h                 ; mul        bl

        db 0BEh                       ; opcode of mov si, imm16
        dw BistBuffer                 ; mov        si,  BistBuffer
        db 03h,  0F0h                 ; add        si,  ax

        db 66h,  0C7h, 04h
        dd 00000001h                  ; mov        dword ptr [si], 1           ; Set Valid Flag
        db 66h,  89h,  6Ch,  04h      ; mov        dword ptr [si + 4], ebp     ; Store BIST value

;
; Switch to flat mode.
;
flat32Start::

        db 0BFh                       ; opcode of mov di, imm16
        dw BufferStartLocation        ; mov        di, BufferStartLocation
        db 66h,  8Bh, 35h             ; mov        esi,dword ptr [di]          ; ESI is keeping the start address of wakeup buffer

        db 0BFh                       ; opcode of mov di, imm16
        dw Cr3OffsetLocation          ; mov        di, Cr3Location
        db 66h,  8Bh, 0Dh             ; mov        ecx,dword ptr [di]          ; ECX is keeping the value of CR3

        db 0BFh                       ; opcode of mov di, imm16
        dw GdtrLocation               ; mov        di, GdtrProfile
        db 66h                        ; db         66h
        db 2Eh,  0Fh, 01h, 15h        ; lgdt       fword ptr cs:[di]

        db 0BFh                       ; opcode of mov di, imm16
        dw IdtrLocation               ; mov        di, IdtrProfile
        db 66h                        ; db         66h
        db 2Eh,  0Fh, 01h, 1Dh        ; lidt       fword ptr cs:[di]

        db 0BFh                       ; opcode of mov di, imm16
        dw LongModeStartJump - RendezvousFunnelProcStart                       ; Get offset of LongModeStartJump
        db 66h,  8Bh, 3Dh             ; mov        edi,dword ptr [di]          ; EDI is keeping the LongModeStart Jump Address

        db 31h,  0C0h                 ; xor        ax,  ax
        db 8Eh,  0D8h                 ; mov        ds,  ax

        db 0Fh,  20h, 0C0h            ; mov        eax, cr0                    ; Get control register 0
        db 66h,  83h, 0C8h, 03h       ; or         eax, 000000003h             ; Set PE bit (bit #0) and MP
        db 0Fh,  22h, 0C0h            ; mov        cr0, eax

FLAT32_JUMP::

        db 66h,  67h, 0EAh            ; far jump
        dd 0h                         ; 32-bit offset
        dw 20h                        ; 16-bit selector

NemInit::                             ; 32-bits protected mode entry point

        db 66h,  0B8h, 18h,  00h      ; mov        ax,  18h
        db 66h,  8Eh,  0D8h           ; mov        ds,  ax
        db 66h,  8Eh,  0C0h           ; mov        es,  ax
        db 66h,  8Eh,  0E0h           ; mov        fs,  ax
        db 66h,  8Eh,  0E8h           ; mov        gs,  ax
        db 66h,  8Eh,  0D0h           ; mov        ss,  ax                     ; Flat mode setup.


PrepareToGoLongMode64::

        db 0Fh,  20h,  0E0h           ; mov        eax, cr4
        db 66h,  0Dh,  020h, 06h      ; or         ax,  0620h                  ; Set PAE=1, OSFXSR=1, OSXMMEXCPT=1.
        db 0Fh,  22h,  0E0h           ; mov        cr4, eax

        db 0Fh,  22h,  0D9h           ; mov        cr3, ecx

        db 0B9h
        dd 0C0000080h                 ; mov        ecx, 0c0000080h             ; EFER MSR number.
        db 0Fh,  32h                  ; rdmsr                                  ; Read EFER.
        db 0Fh,  0BAh, 0E8h, 08h      ; bts        eax, 8                      ; Set LME=1.
        db 0Fh,  30h                  ; wrmsr                                  ; Write EFER.

        db 0Fh,  20h,  0C0h           ; mov        eax, cr0                    ; Read CR0.
        db 0Fh,  0BAh, 0E8h, 1Fh      ; bts        eax, 31                     ; Set PG=1.
        db 0Fh,  22h,  0C0h           ; mov        cr0, eax                    ; Write CR0.

LONG_JUMP::

        db 67h,  0EAh                 ; far jump

LongModeStartJump:

        dd 0h                         ; 32-bit offset
        dw 38h                        ; 16-bit selector


LongModeStart::

        mov         ax,  30h
        mov         ds,  ax
        mov         es,  ax
        mov         ss,  ax

WaitFirstApTaskAssigned::
;
; First INIT-SIPI-SIPI will loop here until DetailedMpInitialization function assigned for each AP
;
        pause
        cmp         qword ptr [esi+CProcedureLocation], 0
        jz          WaitFirstApTaskAssigned

;
; Patch Addresses for jumping between RUN and MONITOR MWAIT loops 32-bits and Long Monde Procedure 64-bits
; Based on the running address of LongModeStart in physic memory which was actually copied by CPU DXE INIT
;
        xor         rdx, rdx
        mov         eax, edi
        add         eax, RunLoopAndMwaitLoop32 - LongModeStart
        mov         edx, edi
        add         edx, RunLoopAndMwaitLoop32Jump - LongModeStart
        mov         dword ptr [rdx], eax

        mov         rbp, rdx                          ; RBP = 32-bits compatibility mode FAR JUMP m16:32 operand pointer

        mov         eax, edi
        add         eax, RunLoopAndMwaitLoop64 - LongModeStart
        mov         edx, edi
        add         edx, RunLoopAndMwaitLoop64Jump - LongModeStart
        mov         dword ptr [rdx], eax

;
; ProgramStack
;
        xor         rcx, rcx
        mov         edi, esi
        add         edi, BistBuffer
        mov         ecx, dword ptr [edi + 8 * ebx]    ; RCX = CpuNumber

        mov         edi, esi
        add         edi, StackSizeLocation
        mov         rax, qword ptr [edi]
        inc         rcx
        mul         rcx                               ; RAX = StackSize * (CpuNumber + 1)

        mov         edi, esi
        add         edi, StackStartAddressLocation
        mov         rdx, qword ptr [edi]
        add         rax, rdx                          ; RAX = StackStart + StackSize * (CpuNumber + 1)

        mov         rsp, rax
        sub         rsp, MonitorFilterSize            ; Reserved Monitor data space
        or          ebx, BreakToRunApSignal           ; ebx = #Cpu run signature

;
; Call assembly function to initialize FPU.
;
        mov         rax, CpuInitFloatPointUnit
        sub         rsp, 20h
        call        rax
        add         rsp, 20h

;
; Load C Function pointer and wakeup manner location
;
        mov         edi, esi
        add         edi, CProcedureLocation
        add         esi, WakeUpApManner               ; esi = WakeUpApManner Address Location

WakeUpThisAp64::

        mov         rax, qword ptr [edi]

        test        rax, rax
        jz          CheckWakeUpCounterInit64

        push        rbp
        push        rbx
        push        rsi
        push        rdi

        sub         rsp, 20h
        call        rax
        add         rsp, 20h

        pop         rdi
        pop         rsi
        pop         rbx
        pop         rbp

CheckWakeUpCounterInit64::

        cmp         dword ptr [esi], WakeUpApCounterInit
        jnz         CheckWakeUpManner64

;
; Initialize MONITOR_MWAIT_DATA data structure per thread
;
        xor         rcx, rcx
        mov         qword ptr [rsp +  0], rcx         ; BreakToRunApSignal
        mov         qword ptr [rsp +  8], rcx         ; HltLoopBreakCounter
        mov         qword ptr [rsp + 16], rcx         ; MwaitLoopBreakCounter
        mov         qword ptr [rsp + 24], rcx         ; RunLoopBreakCounter
        mov         qword ptr [rsp + 32], rcx         ; MwaitLoopBreakCounter32
        mov         qword ptr [rsp + 40], rcx         ; RunLoopBreakCounter32
        mov         qword ptr [rsp + 48], rcx         ; WakeUpApVectorChangeFlag
        mov         qword ptr [rsp + 56], rcx         ; MwaitTargetCstate

WaitWakeUpMannerAssigned::

        pause
        cmp         dword ptr [esi], WakeUpApCounterInit
        jz          WaitWakeUpMannerAssigned

CheckWakeUpManner64::

        pause
        mov         edx, dword ptr [esi]
        cmp         edx, WakeUpApPerHltLoop
        jz          HltApLoop64

        cmp         edx, WakeUpApPerMwaitLoop
        jz          ApMwaitLoop64

        cmp         edx, WakeUpApPerRunLoop
        jz          CheckRunSignal64

        jmp         JumpToCompatibility32Mode

ApMwaitLoop64::

        cli
        mov         rax, rsp                          ; Set Monitor Address
        xor         rcx, rcx
        xor         rdx, rdx
        DB          0fh, 1, 0c8h                      ; MONITOR
        mov         rax, qword ptr [rsp + 56]         ; Mwait Target C-State per rax[7:4]
        DB          0fh, 1, 0c9h                      ; MWAIT

CheckRunSignal64::

        cmp         qword ptr [rsp], rbx              ; Check if run signal correct?
        jnz         CheckWakeUpManner64               ; Unknown break, go checking run manner

        jmp         WakeUpThisAp64                    ; Jmp to execute AP task

HltApLoop64::

        cli
        hlt
        jmp         HltApLoop64                       ; Jump to halt loop


JumpToCompatibility32Mode::

        db 0FFh, 6Dh, 0     ; jmp pword ptr [rbp+0]   ; Far jump to m16:32 for 32-bits compatibility mode

RunLoopAndMwaitLoop32Jump:

        dd 0h                                         ; m32 part of m16:32
        dw 20h                                        ; m16 part of m16:32

RunLoopAndMwaitLoop32::

        db 66h,  0B8h, 18h,  00h      ; mov        ax,  18h
        db 66h,  8Eh,  0D8h           ; mov        ds,  ax
        db 8eh,  0d0h                 ; mov        ss,  ax

        db 0Fh,  20h,  0C0h           ; mov        eax, cr0                    ; Read CR0.
        db 0Fh,  0BAh, 0F0h, 1Fh      ; btr        eax, 31                     ; Reset PG=0.
        db 0Fh,  22h,  0C0h           ; mov        cr0, eax                    ; Write CR0.

        db 0B9h
        dd 0C0000080h                 ; mov        ecx, 0c0000080h             ; EFER MSR number.
        db 0Fh,  32h                  ; rdmsr                                  ; Read EFER.
        db 0Fh,  0BAh, 0F0h, 08h      ; btr        eax, 8                      ; Reset LME=0.
        db 0Fh,  30h                  ; wrmsr                                  ; Write EFER.

        db 0Fh,  20h,  0E0h           ; mov        eax, cr4
        db 24h,  0DFh                 ; and        al,  0DFh                   ; Reset PAE=0 in CR4 bit 5
        db 0Fh,  22h,  0E0h           ; mov        cr4, eax

CheckWakeUpManner32::

        pause
        cmp         dword ptr [rsi], WakeUpApPerMwaitLoop32                    ; Use rsi for esi per compling in 64-bits mode
        jnz         CheckRunSignal32

        cli
        mov         eax, esp                          ; Set Monitor Address
        xor         ecx, ecx
        xor         edx, edx
        DB          0fh, 1, 0c8h                      ; MONITOR
        mov         eax, dword ptr [rsp + 56]         ; Mwait Target C-State per eax[7:4]
        DB          0fh, 1, 0c9h                      ; MWAIT


CheckRunSignal32::

        cmp         dword ptr [rsp], ebx              ; Check if run signal correct?
        jnz         CheckWakeUpManner32               ; Unknown break, go checking run manner

        db 0Fh,  20h,  0E0h           ; mov        eax, cr4
        db 0Ch,  20h                  ; or         al,  20h                    ; Set PAE=1 in CR4 bit 5
        db 0Fh,  22h,  0E0h           ; mov        cr4, eax

        db 0B9h
        dd 0C0000080h                 ; mov        ecx, 0c0000080h             ; EFER MSR number.
        db 0Fh,  32h                  ; rdmsr                                  ; Read EFER.
        db 0Fh,  0BAh, 0E8h, 08h      ; bts        eax, 8                      ; Set LME=1.
        db 0Fh,  30h                  ; wrmsr                                  ; Write EFER.

        db 0Fh,  20h,  0C0h           ; mov        eax, cr0                    ; Read CR0.
        db 0Fh,  0BAh, 0E8h, 1Fh      ; bts        eax, 31                     ; Set PG=1.
        db 0Fh,  22h,  0C0h           ; mov        cr0, eax                    ; Write CR0.

        db 67h,  0EAh                 ; far jump back to 64-bits long mode

RunLoopAndMwaitLoop64Jump:

        dd 0h                         ; 32-bit offset
        dw 38h                        ; 16-bit selector

RunLoopAndMwaitLoop64::

        mov         ax,  30h
        mov         ds,  ax
        mov         ss,  ax

        jmp         WakeUpThisAp64

RendezvousFunnelProc   ENDP
RendezvousFunnelProcEnd::


;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
AsmGetAddressMap   PROC   PUBLIC

        mov         rax, offset RendezvousFunnelProcStart
        mov         qword ptr [rcx], rax
        mov         qword ptr [rcx+8h], NemInit - RendezvousFunnelProcStart
        mov         qword ptr [rcx+10h], FLAT32_JUMP - RendezvousFunnelProcStart
        mov         qword ptr [rcx+18h], LongModeStart - RendezvousFunnelProcStart
        mov         qword ptr [rcx+20h], LONG_JUMP - RendezvousFunnelProcStart
        mov         qword ptr [rcx+28h], RendezvousFunnelProcEnd - RendezvousFunnelProcStart

        ret

AsmGetAddressMap   ENDP

AsmAcquireMPLock   PROC    PUBLIC

        mov         al, NotVacantFlag
TryGetLock:
        xchg        al, byte ptr [rcx]
        cmp         al, VacantFlag
        jz          LockObtained

        pause
        jmp         TryGetLock

LockObtained:
        ret

AsmAcquireMPLock   ENDP

AsmReleaseMPLock   PROC    PUBLIC

        mov         al, VacantFlag
        xchg        al, byte ptr [rcx]

        ret

AsmReleaseMPLock   ENDP

;-------------------------------------------------------------------------------------
;AsmExchangeRole procedure follows. This procedure executed by current BSP, that is
;about to become an AP. It switches it'stack with the current AP.
;AsmExchangeRole (IN   CPU_EXCHANGE_INFO    *MyInfo, IN   CPU_EXCHANGE_INFO    *OthersInfo);
;-------------------------------------------------------------------------------------
CPU_SWITCH_STATE_IDLE          equ        0
CPU_SWITCH_STATE_STORED        equ        1
CPU_SWITCH_STATE_LOADED        equ        2

AsmExchangeRole   PROC    PUBLIC
        ; DO NOT call other functions in this function, since 2 CPU may use 1 stack
        ; at the same time. If 1 CPU try to call a functiosn, stack will be corrupted.

        push        rax
        push        rbx
        push        rcx
        push        rdx
        push        rsi
        push        rdi
        push        rbp
        push        r8
        push        r9
        push        r10
        push        r11
        push        r12
        push        r13
        push        r14
        push        r15

        mov         rax, cr0
        push        rax

        mov         rax, cr4
        push        rax

        ; rsi contains MyInfo pointer
        mov         rsi, rcx

        ; rdi contains OthersInfo pointer
        mov         rdi, rdx

        ;Store EFLAGS, GDTR and IDTR regiter to stack
        pushfq
        sgdt        fword ptr [rsi + 16]
        sidt        fword ptr [rsi + 26]

        ; Store the its StackPointer
        mov         qword ptr [rsi + 8], rsp

        ; update its switch state to STORED
        mov         al, NotVacantFlag
TryLock1:
        xchg        al, byte ptr [rsi]
        cmp         al, VacantFlag
        jz          LockObtained1
        pause
        jmp         TryLock1

LockObtained1:
        mov         byte ptr [rsi + 1], CPU_SWITCH_STATE_STORED
        xchg        al, byte ptr [rsi]

WaitForOtherStored::
        ; wait until the other CPU finish storing its state
        mov         al, NotVacantFlag
TryLock2:
        xchg        al, byte ptr [rdi]
        cmp         al, VacantFlag
        jz          LockObtained2
        PAUSE32
        jmp         TryLock2

LockObtained2:
        mov         bl, byte ptr [rdi + 1]
        xchg        al, byte ptr [rdi]
        cmp         bl, CPU_SWITCH_STATE_STORED
        jb          WaitForOtherStored

        ; Since another CPU already stored its state, load them
        ; load GDTR value
        lgdt        fword ptr [rdi + 16]

        ; load IDTR value
        lidt        fword ptr [rdi + 26]

        ; load its future StackPointer
        mov         rsp, qword ptr [rdi + 8]

        ; update its switch state to LOADED
        mov         al, NotVacantFlag
TryLock3:
        xchg        al, byte ptr [rsi]
        cmp         al, VacantFlag
        jz          LockObtained3
        PAUSE32
        jmp         TryLock3

LockObtained3:
        mov         byte ptr [rsi+1], CPU_SWITCH_STATE_LOADED
        xchg        al, byte ptr [rsi]

WaitForOtherLoaded::
        ; wait until the other CPU finish loading new state,
        ; otherwise the data in stack may corrupt
        mov         al, NotVacantFlag
TryLock4:
        xchg        al, byte ptr [rdi]
        cmp         al, VacantFlag
        jz          LockObtained4
        PAUSE32
        jmp         TryLock4

LockObtained4:
        mov         bl, byte ptr [rdi+1]
        xchg        al, byte ptr [rdi]
        cmp         bl, CPU_SWITCH_STATE_LOADED
        jb          WaitForOtherLoaded

        ; since the other CPU already get the data it want, leave this procedure
        popfq

        pop         rax
        mov         cr4, rax

        pop         rax
        mov         cr0, rax

        pop         r15
        pop         r14
        pop         r13
        pop         r12
        pop         r11
        pop         r10
        pop         r9
        pop         r8
        pop         rbp
        pop         rdi
        pop         rsi
        pop         rdx
        pop         rcx
        pop         rbx
        pop         rax

        ret
AsmExchangeRole   ENDP

AsmWaitMem     PROC    PUBLIC
AsmWaitMemS:
        pause
        cmp    qword ptr [rcx], rdx
        jz     AsmWaitMemS
        ret

AsmWaitMem      ENDP

text    ENDS

END

