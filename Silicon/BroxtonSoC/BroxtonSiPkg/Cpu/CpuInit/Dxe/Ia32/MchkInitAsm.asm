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

_EnableMCE  proc near public
  push    ebp               ; C prolog
  mov     ebp, esp

  mov     eax, cr4
  or      eax, 40h
  mov     cr4, eax

  pop     ebp
  ret
_EnableMCE  endp


  end

