## @file
#  This is the ASM for starting a linux kernel.
#
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

ASM_GLOBAL ASM_PFX(AsmStartLinuxKernel)
ASM_PFX(AsmStartLinuxKernel):
  movl     4(%esp), %eax
  subl     $0x10, %esp
  leal     NewSelectorJmp, %ebx
  movl     %ebx, (%esp)
  movl     $0x10, %ebx
  movl     %ebx, 4(%esp)
  jmpl     (%esp)

NewSelectorJmp:
  addl     $0x10, %esp

  movl     $0x18, %ebx
  movl     %ebx, %ds
  movl     %ebx, %es
  movl     %ebx, %fs
  movl     %ebx, %gs
  movl     %ebx, %ss

  movl     $0x8000, %esi
  xorl     %ebp, %ebp
  xorl     %edi, %edi
  xorl     %ebx, %ebx
  jmpl     %eax
  ret
