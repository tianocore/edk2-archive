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
.data
.stack
.code
.MMX
.XMM

 include  Htequ.inc
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
        db 0f0h                       ; opcode for lock instruction
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
        db 0f0h                       ; opcode for lock instruction
        xchg        al, byte ptr [ebx]

        popad
        ret
AsmReleaseMPLock   ENDP

;-------------------------------------------------------------------------------
;  AsmGetGdtrIdtr (&Gdt, &Idt);
;-------------------------------------------------------------------------------------
AsmGetGdtrIdtr   PROC  near C  PUBLIC

        pushad
        mov         ebp,esp

        sgdt        fword ptr GdtDesc
        lea         esi, GdtDesc
        mov         edi, dword ptr [ebp+24h]
        mov         dword ptr [edi], esi

        sidt        fword ptr IdtDesc
        lea         esi, IdtDesc
        mov         edi, dword ptr [ebp+28h]
        mov         dword ptr [edi], esi

        popad
        ret
AsmGetGdtrIdtr   ENDP

GdtDesc::                             ; GDT descriptor
                    DW      03fh      ; GDT limit
                    DW      0h        ; GDT base and limit will be
                    DW      0h        ; filled using sgdt

IdtDesc::                             ; IDT descriptor
                    DW      0h        ; IDT limit
                    DW      0h        ; IDT base and limit will be
                    DW      0h        ; filled using sidt

END

