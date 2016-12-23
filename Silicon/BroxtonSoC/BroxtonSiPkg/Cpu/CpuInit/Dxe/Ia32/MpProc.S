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

ASM_GLOBAL ASM_PFX(MpMtrrSynchUpEntry)
ASM_PFX(MpMtrrSynchUpEntry):
    #
    # Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
    #
    movl %cr0, %eax
    andl $0x0DFFFFFFF, %eax
    orl  $0x040000000, %eax
    movl %eax, %cr0
    #
    # Flush cache
    #
    wbinvd
    #
    # Clear PGE flag Bit 7
    #
    movl %cr4, %eax
    movl %eax, %edx
    andl $0x0FFFFFF7F, %eax
    movl %eax, %cr4
    #
    # Flush all TLBs
    #
    movl %cr3, %eax
    movl %eax, %cr3
    
    movl %edx, %eax
    ret
#MpMtrrSynchUpEntry  ENDP

ASM_GLOBAL ASM_PFX(MpMtrrSynchUpExit)
ASM_PFX(MpMtrrSynchUpExit):
    push    %ebp             # C prolog
    movl    %esp, %ebp
    #
    # Flush all TLBs the second time
    #
    movl %cr3, %eax
    movl %eax, %cr3
    #
    # Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    #
    movl %cr0, %eax
    andl $0x09FFFFFFF, %eax
    movl %eax, %cr0
    #
    # Set PGE Flag in CR4 if set
    #
    movl 8(%ebp), %eax
    movl %eax, %cr4
    
    pop %ebp
    ret
#MpMtrrSynchUpExit  ENDP
