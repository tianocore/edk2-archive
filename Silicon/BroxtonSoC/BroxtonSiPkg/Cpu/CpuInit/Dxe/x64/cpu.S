## @file
#  Assembly code for the x64 resources.
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

//=============================================================================
// Debug Macros
//=============================================================================

//ENABLE_DEADLOOP = Deadloop on exception handler entry, exit

// #define ENABLE_DEADLOOP 1
#ifdef ENABLE_DEADLOOP
  .macro DEADLOOP
      jmp .
  .endm
#else
  .macro DEADLOOP
  .endm
#endif

  .text
  ASM_FUNCTION_REMOVE_IF_UNREFERENCED

//.globl _CpuFlushTlb
.globl __outp
.globl ASM_PFX(mErrorCodeFlag)

//
// Float control word initial value:
// all exceptions masked, double-extended-precision, round-to-nearest
//
mFpuControlWord:
.word 0x37F

//
// Multimedia-extensions control word:
// all exceptions masked, round-to-nearest, flush to zero for masked underflow
//
mMmxControlWord:
.long 0x1F80

//
//
//
//------------------------------------------------------------------------------
//  Generic IDT Vector Handlers for the Host. They are all the same so they
//  will compress really well.
//
//  By knowing the return address for Vector 00 you can can calculate the
//  vector number by looking at the call CommonInterruptEntry return address.
//  (return address - (AsmIdtVector00 + 5))/8 == IDT index
//
//------------------------------------------------------------------------------

  .p2align   3

.globl ASM_PFX(AsmIdtVector00)
ASM_PFX(AsmIdtVector00):
  callq ASM_PFX(CommonInterruptEntry)
  .set vector00, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector00 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector01, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector01 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector02, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector02 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector03, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector03 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector04, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector04 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector05, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector05 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector06, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector06 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector07, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector07 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector08, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector08 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector09, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector09 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0a / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0b / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0c / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0d / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0e / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector0f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector0f / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector10, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector10 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector11, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector11 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector12, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector12 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector13, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector13 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector14, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector14 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector15, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector15 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector16, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector16 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector17, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector17 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector18, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector18 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector19, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector19 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1a / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1b / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1c / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1d / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1e / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector1f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector1f / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector20, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector20 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector21, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector21 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector22, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector22 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector23, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector23 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector24, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector24 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector25, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector25 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector26, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector26 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector27, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector27 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector28, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector28 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector29, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector29 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2a / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2b / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2c / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2d / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2e / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector2f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector2f / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector30, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector30 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector31, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector31 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector32, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector32 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector33, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector33 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector34, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector34 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector35, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector35 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector36, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector36 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector37, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector37 / 8
  nop

  callq ASM_PFX(CommonInterruptEntry)
  .set vector38, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector38 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector39, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector39 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector3f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector3f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector40, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector40 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector41, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector41 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector42, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector42 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector43, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector43 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector44, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector44 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector45, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector45 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector46, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector46 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector47, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector47 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector48, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector48 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector49, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector49 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector4f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector4f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector50, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector50 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector51, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector51 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector52, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector52 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector53, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector53 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector54, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector54 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector55, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector55 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector56, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector56 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector57, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector57 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector58, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector58 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector59, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector59 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector5f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector5f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector60, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector60 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector61, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector61 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector62, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector62 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector63, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector63 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector64, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector64 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector65, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector65 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector66, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector66 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector67, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector67 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector68, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector68 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector69, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector69 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector6f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector6f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector70, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector70 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector71, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector71 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector72, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector72 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector73, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector73 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector74, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector74 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector75, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector75 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector76, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector76 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector77, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector77 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector78, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector78 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector79, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector79 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector7f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector7f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector80, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector80 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector81, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector81 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector82, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector82 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector83, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector83 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector84, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector84 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector85, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector85 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector86, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector86 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector87, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector87 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector88, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector88 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector89, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector89 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector8f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector8f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector90, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector90 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector91, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector91 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector92, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector92 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector93, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector93 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector94, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector94 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector95, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector95 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector96, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector96 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector97, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector97 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector98, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector98 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector99, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector99 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9a, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9a / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9b, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9b / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9c, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9c / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9d, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9d / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9e, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9e / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vector9f, . - ASM_PFX(AsmIdtVector00) - 5
  .short vector9f / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectora9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectora9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectoraa, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectoraa / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorab, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorab / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorac, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorac / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorad, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorad / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorae, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorae / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectoraf, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectoraf / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorb9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorb9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorba, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorba / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorbb, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorbb / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorbc, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorbc / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorbd, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorbd / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorbe, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorbe / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorbf, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorbf / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorc9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorc9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorca, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorca / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorcb, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorcb / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorcc, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorcc / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorcd, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorcd / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorce, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorce / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorcf, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorcf / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectord9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectord9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorda, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorda / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectordb, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectordb / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectordc, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectordc / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectordd, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectordd / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorde, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorde / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectordf, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectordf / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectore9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectore9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorea, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorea / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectoreb, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectoreb / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorec, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorec / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectored, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectored / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectoree, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectoree / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectoref, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectoref / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf0, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf0 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf1, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf1 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf2, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf2 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf3, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf3 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf4, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf4 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf5, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf5 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf6, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf6 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf7, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf7 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf8, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf8 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorf9, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorf9 / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorfa, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorfa / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorfb, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorfb / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorfc, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorfc / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorfd, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorfd / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorfe, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorfe / 8
  nop

  callq  ASM_PFX(CommonInterruptEntry)
  .set vectorff, . - ASM_PFX(AsmIdtVector00) - 5
  .short vectorff / 8
  nop

//---------------------------------------//
// CommonInterruptEntry                  //
//---------------------------------------//
// The follow algorithm is used for the common interrupt routine.

//    +---------------------+ <-- 16-byte aligned ensured by processor
// 40 +    Old SS           +
//    +---------------------+
// 32 +    Old RSP          +
//    +---------------------+
// 24 +    RFlags           +
//    +---------------------+
// 16 +    CS               +
//    +---------------------+
// 8  +    RIP              +
//    +---------------------+
// 0  +    RBP              + <-- RBP, 16-byte aligned
//    +---------------------+
// -8 + RCX / Vector Number +
//    +---------------------+
//-16 +    Error Code       +
//    +---------------------+
//

.globl ASM_PFX(CommonInterruptEntry)
ASM_PFX(CommonInterruptEntry):

  DEADLOOP

  cli
  //
  // All interrupt handlers are invoked through interrupt gates, so
  // IF flag automatically cleared at the entry point
  //
  //
  // Calculate vector number
  //
  xchg    (%rsp), %rcx    // get the return address of call
  movzwl  (%rcx), %ecx    // actually, it is the address of vector number.
  cmp     $32, %ecx       // Intel reserved vector for exceptions?
  jae     L_NoErrorCode
  bt      %ecx, ASM_PFX(mErrorCodeFlag)(%rip)
  jc      L_ErrorCode

L_NoErrorCode:
  pushq   (%rsp)          // Copy RCX/Vector to next stack slot
  mov     %rbp, 8(%rsp)   // Save RBP
  push    $0              // Push dummy error code
  jmp     L_SaveState

L_ErrorCode:
  xchg    8(%rsp), %rbp   // Put RBP into Error Code slot
  push    %rbp            // Push Error Code to stack

  //
  // Since here the stack pointer is 16-byte aligned, so
  // EFI_FX_SAVE_STATE_X64 of EFI_SYSTEM_CONTEXT_x64
  // is 16-byte aligned
  //

//// UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax//
//// UINT64  R8, R9, R10, R11, R12, R13, R14, R15//
L_SaveState:
  mov  %rsp, %rbp
  add  $16, %rbp          // Fixup RBP after RCX/Vector, Error Code swizzle
  push %r15
  push %r14
  push %r13
  push %r12
  push %r11
  push %r10
  push %r9
  push %r8
  push %rax
  push -8(%rbp)  // RCX
  push %rdx
  push %rbx
  pushq 32(%rbp) // RSP
  pushq 0(%rbp)  // RBP
  push %rsi
  push %rdi

//// UINT64  Gs, Fs, Es, Ds, Cs, Ss//  insure high 16 bits of each is zero
  movzwq  40(%rbp), %rax
  push    %rax                      // for ss
  movzwq  16(%rbp), %rax
  push    %rax                      // for cs
  mov     %ds, %eax
  push    %rax
  mov     %es, %eax
  push    %rax
  mov     %fs, %eax
  push    %rax
  mov     %gs, %eax
  push    %rax

  mov     %rcx, -8(%rbp)            // save vector number

//// UINT64  Rip
  pushq   8(%rbp)

//// UINT64  Gdtr[2], Idtr[2]//
  sub     $16, %rsp           // IDT base / limit
  sidt    6(%rsp)
  movzwq  6(%rsp), %rax
  mov     %rax, (%rsp)

  sub     $16, %rsp           // GDT base / limit
  sgdt    6(%rsp)
  movzwq  6(%rsp), %rax
  mov     %rax, (%rsp)

//// UINT64  Ldtr, Tr//
  xor     %rax, %rax
  str     %ax
  push    %rax
  sldt    %ax
  push    %rax

//// UINT64  RFlags//
  pushq   24(%rbp)

//// UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8//
  mov     %cr8, %rax
  push    %rax
  mov     %cr4, %rax
  or      $0x208, %rax
  mov     %rax, %cr4
  push    %rax
  mov     %cr3, %rax
  push    %rax
  mov     %cr2, %rax
  push    %rax
  xor     %rax, %rax
  push    %rax
  mov     %cr0, %rax
  push    %rax

//// UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7//
  mov     %db7, %rax
  push    %rax
  mov     %db6, %rax
  push    %rax
  mov     %db3, %rax
  push    %rax
  mov     %db2, %rax
  push    %rax
  mov     %db1, %rax
  push    %rax
  mov     %db0, %rax
  push    %rax

//// FX_SAVE_STATE_X64 FxSaveState//

  sub     $512, %rsp
  mov     %rsp, %rdi
  .byte   0x0f, 0xae, 0x07 //fxsave [rdi]

//// UINT32  ExceptionData//
  pushq   -16(%rbp)

//// call into exception handler
  mov     -8(%rbp), %rcx
  mov     ExternalVectorTablePtr(%rip), %rax  // get the interrupt vectors base
  mov     (%rax,%rcx,8),%rax
  or      %rax, %rax                        // NULL?

  je    L_nonNullValue

//// Prepare parameter and call
  mov     %rsp, %rdx

  //
  // Per X64 calling convention, allocate maximum parameter stack space
  // and make sure RSP is 16-byte aligned
  //
  DEADLOOP

  sub     $0x28, %rsp
  callq   *%rax
  add     $0x28, %rsp

  DEADLOOP

L_nonNullValue:
  cli
//// UINT64  ExceptionData
  pop     %rax
  mov     %rax, -16(%rbp)

//// FX_SAVE_STATE_X64 FxSaveState//

  mov     %rsp, %rsi
  .byte   0x0f, 0xae, 0x0e // fxrstor [rsi]
  add     $512, %rsp

//// UINT64  Dr0, Dr1, Dr2, Dr3, Dr6, Dr7//
  add     $48, %rsp
  xor     %rax, %rax  //// always clear dr6
  mov     %rax, %dr6

//// UINT64  Cr0, Cr1, Cr2, Cr3, Cr4, Cr8//
  pop     %rax
  mov     %rax, %cr0
  add     $8, %rsp   // not for Cr1
  pop     %rax
  mov     %rax, %cr2
  pop     %rax
  mov     %rax, %cr3
  pop     %rax
  mov     %rax, %cr4
  pop     %rax
  mov     %rax, %cr8

//// UINT64  RFlags
  popq    24(%rbp)

//// UINT64  Ldtr, Tr//
//// UINT64  Gdtr[2], Idtr[2]//
//// Best not let anyone mess with these particular registers...
  add     $48, %rsp

//// UINT64  Rip
  popq    8(%rbp)

//// UINT64  Gs, Fs, Es, Ds, Cs, Ss//
  pop     %rax
  // mov     gs, rax // not for gs
  pop     %rax
  // mov     fs, rax // not for fs
  // (X64 will not use fs and gs, so we do not restore it)
  pop     %rax
  mov     %eax, %es
  pop     %rax
  mov     %eax, %ds
  popq    16(%rbp) // for cs
  popq    40(%rbp) // for ss

//// UINT64  Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax//
//// UINT64  R8, R9, R10, R11, R12, R13, R14, R15//
  pop     %rdi
  pop     %rsi
  add     $8, %rsp // not for rbp
  popq    32(%rbp) // for rsp
  pop     %rbx
  pop     %rdx
  pop     %rcx
  pop     %rax
  pop     %r8
  pop     %r9
  pop     %r10
  pop     %r11
  pop     %r12
  pop     %r13
  pop     %r14
  pop     %r15

  mov     %rbp, %rsp
  pop     %rbp

  DEADLOOP

  iretq

//
//Initializes floating point units for requirement of UEFI specification.
//
// This function initializes floating-point control word to 0x037F (all exceptions
// masked,double-extended-precision, round-to-nearest) and multimedia-extensions control word
// (if supported) to 0x1F80 (all exceptions masked, round-to-nearest, flush to zero
// for masked underflow).
//
.globl ASM_PFX(CpuInitFloatPointUnit)
ASM_PFX(CpuInitFloatPointUnit):

    //
    // Initialize floating point units
    //
    // The following opcodes stand for instruction 'finit'
    // to be supported by some 64-bit assemblers
    //

    //finit
    .byte  0x9B, 0xDB, 0xE3
    fldcw    mFpuControlWord(%rip)

    //
    // Set OSFXSR bit 9 in CR4
    //
    mov    %cr4,%rax
    or     $0x200,%rax
    mov    %rax,%cr4

    ldmxcsr  mMmxControlWord(%rip)
    retq

.globl ASM_PFX(InitializeExternalVectorTablePtr)
ASM_PFX(InitializeExternalVectorTablePtr):
  mov  %rcx, ExternalVectorTablePtr(%rip)
  retq

ExternalVectorTablePtr:
.quad 0 // point to the external interrupt vector table

__outp:
    mov    %edx,%eax
    movzwl %cx,%edx
    out    %al,(%dx)
    retq
