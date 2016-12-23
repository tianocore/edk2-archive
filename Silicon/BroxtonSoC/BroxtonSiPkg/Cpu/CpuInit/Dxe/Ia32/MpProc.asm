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

  page    ,132
  title   MP ASSEMBLY HOOKS

.686p
.model  flat
.data
.stack
.code
.MMX
.XMM
#if 0
_MpMtrrSynchUpEntry  PROC    NEAR    PUBLIC
    ;
    ; Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
    ;
    mov eax, cr0
    and eax, 0DFFFFFFFh
    or  eax, 040000000h
    mov cr0, eax
    ;
    ; Flush cache
    ;
    wbinvd
    ;
    ; Clear PGE flag Bit 7
    ;
    mov eax, cr4
    mov edx, eax
    and eax, 0FFFFFF7Fh
    mov cr4, eax
    ;
    ; Flush all TLBs
    ;
    mov eax, cr3
    mov cr3, eax

    mov eax, edx

    ret

_MpMtrrSynchUpEntry  ENDP

_MpMtrrSynchUpExit  PROC    NEAR    PUBLIC

    push    ebp             ; C prolog
    mov     ebp, esp
    ;
    ; Flush all TLBs the second time
    ;
    mov eax, cr3
    mov cr3, eax
    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov eax, cr0
    and eax, 09FFFFFFFh
    mov cr0, eax
    ;
    ; Set PGE Flag in CR4 if set
    ;
    mov eax, dword ptr [ebp + 8]
    mov cr4, eax

    pop ebp

    ret

_MpMtrrSynchUpExit  ENDP
#endif
  END

