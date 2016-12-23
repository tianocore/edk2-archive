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

ASM_GLOBAL ASM_PFX(EnableMCE)
ASM_PFX(EnableMCE):
  push    %ebp               # C prolog
  movl    %esp, %ebp
  
  movl    %cr4, %eax
  orl     $0x40, %eax
  movl    %eax, %cr4

  pop     %ebp
  ret
#EnableMCE  endp
