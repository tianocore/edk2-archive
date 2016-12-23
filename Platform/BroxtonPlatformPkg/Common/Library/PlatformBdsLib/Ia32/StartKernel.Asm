;; @file
;  This is the ASM for starting a linux kernel.
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

  .686P
  .MMX
  .MODEL SMALL
  .CODE

AsmStartLinuxKernel PROC  near C  PUBLIC
  mov     eax, [esp+4]
  sub     esp, 010h
  lea     ebx, NewSelectorJmp
  mov     [esp], ebx
  mov     ebx, 010h
  mov     [esp+4], ebx
  jmp     fword ptr [esp]

NewSelectorJmp:
  add     esp, 010h

  mov     ebx, 018h
  mov     ds, ebx
  mov     es, ebx
  mov     fs, ebx
  mov     gs, ebx
  mov     ss, ebx

  mov     esi, 08000h
  xor     ebp, ebp
  xor     edi, edi
  xor     ebx, ebx
  jmp     eax
  ret
AsmStartLinuxKernel   ENDP

  END

