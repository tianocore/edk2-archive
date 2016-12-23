;; @file
;  This is the ASM for Jump to a specific address.
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


JumpToVector    PROTO  C \
   EntryPoint:PTR DWORD, \
   Parameter:DWORD
;
; Routine Description:
;       This allows the caller to switch the stack and goes to the new entry point
;
; Arguments:
;       EntryPoint      - Pointer to the location to enter
;       Parameter       - Parameter to pass in
;
; Returns:
;
;       Nothing. Goes to the Entry Point passing in the new parameters
;
JumpToVector    PROC  C \
  EntryPoint:PTR DWORD, \
  Parameter:DWORD

  push  ebx

  mov   ebx, Parameter
  mov   ecx, EntryPoint
  push  ebx
  push  0
  jmp   ecx

  pop   ebx
  ret

JumpToVector    ENDP
  END

