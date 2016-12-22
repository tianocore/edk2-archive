;; @file
;  This is the assembly code for MP support.
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

include  MpEqu.inc

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure serializes all the AP processors through an Init sequence. It must be
;noted that APs arrive here very raw...ie: real mode, no stack.
;ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
;IS IN MACHINE CODE.
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

.686p
.model  flat
.code

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

;-------------------------------------------------------------------------------
;  AsmAcquireMPLock (&Lock);
;-------------------------------------------------------------------------------
AsmAcquireMPLock   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        mov         al, NotVacantFlag
        mov         ebx, dword ptr [ebp+24h]
TryGetLock:
        xchg        al, byte ptr [ebx]
        cmp         al, VacantFlag
        jz          LockObtained

        PAUSE32
        jmp         TryGetLock

LockObtained:
        popad
        ret
AsmAcquireMPLock   ENDP

;-------------------------------------------------------------------------------
;  AsmReleaseMPLock (&Lock);
;-------------------------------------------------------------------------------------
AsmReleaseMPLock   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        mov         al, VacantFlag
        mov         ebx, dword ptr [ebp+24h]
        xchg        al, byte ptr [ebx]

        popad
        ret
AsmReleaseMPLock   ENDP

RendezvousFunnelProc   PROC  PUBLIC
RendezvousFunnelProcStart::

;Step-1: Grab a lock. At this point CS = 0x(vv00) and ip= 0x0.
        db 66h,  08bh, 0e8h                        ; mov        ebp, eax
        db 8ch,0c8h                                ; mov        ax,cs
        db 8eh,0d8h                                ; mov        ds,ax
        db 8eh,0c0h                                ; mov        es,ax
        db 8eh,0d0h                                ; mov        ss,ax
        db 33h,0c0h                                ; xor        ax,ax
        db 8eh,0e0h                                ; mov        fs,ax
        db 8eh,0e8h                                ; mov        gs,ax
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
        dw StartStateLocation         ; mov        si,  StartState
        db 66h,  83h, 3Ch, 00h        ; cmp        dword ptr [si], 0
        db 75h                        ; opcode of jnz
        db SkipRecordBist - ($ + 1)   ; jnz         SkipRecordBist

; Record BIST information
;
        db 0B0h, 08h                  ; mov        al,  8
        db 0F6h, 0E3h                 ; mul        bl

        db 0BEh                       ; opcode of mov si, imm16
        dw BistBufferLocation         ; mov        si,  BistBufferLocation
        db 03h,  0F0h                 ; add        si,  ax

        db 66h,  0C7h, 04h
        dd 00000001h                  ; mov        dword ptr [si], 1           ; Set Valid Flag
        db 66h,  89h,  6Ch,  04h      ; mov        dword ptr [si + 4], ebp     ; Store BIST value

SkipRecordBist::
        db 0BEh                                    ; opcode of mov si, mem16
        dw BufferStartLocation                     ; mov        si, BufferStartLocation
        db 66h,  8Bh, 1Ch                          ; mov        ebx,dword ptr [si]

        db 0BFh                                    ; opcode of mov di, mem16
        dw PmodeOffsetLocation                     ; mov        di, PmodeOffsetLocation
        db 66h,  8Bh, 05h                          ; mov        eax,dword ptr [di]
        db 8Bh,  0F8h                              ; mov        di, ax
        db 83h,  0EFh,06h                          ; sub        di, 06h
        db 66h,  03h, 0C3h                         ; add        eax, ebx
        db 66h,  89h, 05h                          ; mov        dword ptr [di],eax

        db 0BEh                                    ; opcode of mov si, mem16
        dw GdtrLocation                            ; mov        si, GdtrLocation
        db 66h                                     ; db         66h
        db 2Eh,  0Fh, 01h, 14h                     ; lgdt       fword ptr cs:[si]

        db 0BEh                                    ; opcode of mov si, mem16
        dw IdtrLocation                            ; mov        si, IdtrProfile
        db 66h                                     ; db         66h
        db 2Eh,  0Fh, 01h, 1Ch                     ; lidt       fword ptr cs:[si]

        db 33h,  0C0h                              ; xor        ax,  ax
        db 8Eh,  0D8h                              ; mov        ds,  ax

        db 0Fh,  20h, 0C0h                         ; mov        eax, cr0                        ;Get control register 0
        db 66h,  83h, 0C8h, 03h                    ; or         eax, 000000003h                 ;Set PE bit (bit #0) & MP
        db 0Fh,  22h, 0C0h                         ; mov        cr0, eax

        db 66h,  67h, 0EAh                         ; far jump
        dd 0h                                      ; 32-bit offset
BspCS::
        dw 00h                                     ; 16-bit selector

NemInit::                                          ; protected mode entry point

        db 66h,  0B8h                            ; mov        ax, 18h
BspDS::
        dw 00h
        db 66h,  8Eh,  0D8h                       ; mov        ds, ax
        db 66h,  8Eh,  0C0h                       ; mov        es, ax
        db 66h,  8Eh,  0E0h                       ; mov        fs, ax
        db 66h,  8Eh,  0E8h                       ; mov        gs, ax
        db 66h,  8Eh,  0D0h                       ; mov        ss, ax            ; Flat mode setup.

        mov  esi, ebx

        mov  edi, esi
        add  edi, StartStateLocation
        mov  eax, 1
        mov  dword ptr [edi], eax

        mov  edi, esi
        add  edi, LockLocation
        mov  eax, NotVacantFlag
TestLock::
        xchg dword ptr [edi], eax
        cmp  eax, NotVacantFlag
        jz   TestLock

ProgramStack::

        mov  edi, esi
        add  edi, StackSizeLocation
        mov  eax, dword ptr [edi]
        mov  edi, esi
        add  edi, StackStartAddressLocation
        add  eax, dword ptr [edi]
        mov  esp, eax
        mov  dword ptr [edi], eax

Releaselock::

        mov  eax, VacantFlag
        mov  edi, esi
        add  edi, LockLocation
        xchg dword ptr [edi], eax

CProcedureInvoke::

        mov  edi, esi
        add  edi, CArgumentLocation
        mov  eax, dword ptr [edi]
        push eax

        mov  edi, esi
        add  edi, CProcedureLocation
        mov  eax, dword ptr [edi]


        push    ecx
        push    eax
        push    edx

        mov     ecx, 0121h
        rdmsr
        test        eax, eax
        jnz          SkipAcpiTimerWA
        mov     eax, 00010408h  ; Bit 16 is enable and 15:0 address
        mov     edx, 2FBA2E25h
        wrmsr
SkipAcpiTimerWA:
        pop   edx
        pop   eax
        pop   ecx


        call eax
        add  esp, 4

InterlockedIncrementFinishedCount::
        mov  edi, esi
        add  edi, FinishedCountAddressLocation
        lock    inc     dword ptr [edi]
        cli
        hlt
        jmp  $-2

RendezvousFunnelProc   ENDP

SemaphoreStartAddress PROC C, SemaphoreAddress:PTR DWORD
        mov  eax, SemaphoreAddress
@@:
        cmp  dword ptr [eax], 0
        jz   @F

        PAUSE32
        jmp  @B
@@:
        ret
SemaphoreStartAddress ENDP

RendezvousFunnelProcEnd::


;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
AsmGetAddressMap   PROC  near C  PUBLIC

        mov eax, RendezvousFunnelProcStart
        ret

AsmGetAddressMap   ENDP

AsmGetPmodeOffset PROC  near C PUBLIC

        mov eax, NemInit - RendezvousFunnelProcStart
        ret

AsmGetPmodeOffset ENDP

AsmGetSemaphoreCheckOffset PROC  near C PUBLIC
        mov eax, SemaphoreStartAddress - RendezvousFunnelProcStart
        ret
AsmGetSemaphoreCheckOffset ENDP

AsmPatchRendezvousCode  PROC near C PUBLIC
        mov         eax, dword ptr [esp + 4]
        push        esi
        push        edi
        mov         edi, eax
        mov          ax,  cs
        mov         esi, edi
        add         esi, BspCS - RendezvousFunnelProcStart
        mov         word ptr [esi], ax
        mov          ax,  ds
        mov         esi, edi
        add         esi, BspDS - RendezvousFunnelProcStart
        mov         word ptr [esi], ax
        pop         edi
        pop         esi
        xor         eax, eax
        ret
AsmPatchRendezvousCode ENDP

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
        mov         byte ptr [esi], CPU_SWITCH_STATE_STORED

WaitForOtherStored::
        ; wait until the other CPU finish storing its state
        cmp         byte ptr [edi], CPU_SWITCH_STATE_STORED
        jz          OtherStored
        PAUSE32
        jmp         WaitForOtherStored

OtherStored::
        ; Since another CPU already stored its state, load them
        ; load GDTR value
        lgdt        fword ptr [edi+8]

        ; load IDTR value
        lidt        fword ptr [edi+14]

        ; load its future StackPointer
        mov         esp, dword ptr [edi+4]

        ; update the other CPU's switch state to LOADED
        mov         byte ptr [edi], CPU_SWITCH_STATE_LOADED

WaitForOtherLoaded::
        ; wait until the other CPU finish loading new state,
        ; otherwise the data in stack may corrupt
        cmp         byte ptr [esi], CPU_SWITCH_STATE_LOADED
        jz          OtherLoaded
        PAUSE32
        jmp         WaitForOtherLoaded

OtherLoaded::
        ; since the other CPU already get the data it want, leave this procedure
        popfd

        popad
        ret
AsmExchangeRole   ENDP

END

