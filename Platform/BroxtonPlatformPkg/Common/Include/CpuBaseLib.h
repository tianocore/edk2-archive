/** @file
  The Lib of CPU Base.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_BASE_LIB_H
#define _CPU_BASE_LIB_H

#include "CpuRegs.h"
#include "CpuDataStruct.h"
#include "CpuType.h"
#include "Protocol/MpService.h"

//
// Combine f(FamilyId), m(Model), s(SteppingId) to a single 32 bit number
//
#define EfiMakeCpuVersion(f, m, s)         \
  (((UINT32) (f) << 16) | ((UINT32) (m) << 8) | ((UINT32) (s)))

#ifdef __GNUC__
  #define IA32API
#else
  #define IA32API __cdecl
#endif


/**
  Halt the Cpu.

  @param[in]    None
  @retval       None

**/
VOID
IA32API
EfiHalt (
  VOID
  );

/**
  Write back and invalidate the Cpu cache.

  @param[in]    None
  @retval       None

**/
IA32API
EfiWbinvd (
  VOID
  );

/**
  Invalidate the Cpu cache

  @param[in]    None
  @retval       None

**/
VOID
IA32API
EfiInvd (
  VOID
  );

/**
  Get the Cpu info by excute the CPUID instruction.

  @param[in]  RegisterInEax    The input value to put into register EAX
  @param[in]  Regs             The Output value

  @retval     None

**/
VOID
IA32API
EfiCpuid (
  IN  UINT32                 RegisterInEax,
  OUT EFI_CPUID_REGISTER     *Regs
  );

/**
  When RegisterInEax != 4, the functionality is the same as EfiCpuid.
  When RegisterInEax == 4, the function return the deterministic cache
  parameters by excuting the CPUID instruction

  @param[in] RegisterInEax   The input value to put into register EAX
  @param[in] CacheLevel      The deterministic cache level
  @param[in] Regs            The Output value

  @retval     None

**/
VOID
IA32API
EfiCpuidExt (
  IN  UINT32                 RegisterInEax,
  IN  UINT32                 CacheLevel,
  OUT EFI_CPUID_REGISTER     *Regs
  );

/**
  Read Cpu MSR.

  @param[in]  Index    The index value to select the register

  @retval      Return the read data

**/
UINT64
IA32API
EfiReadMsr (
  IN UINT32     Index
  );

/**
  Write Cpu MSR.

  @param[in] Index   The index value to select the register
  @param[in] Value   The value to write to the selected register

  @retval  None

**/
VOID
IA32API
EfiWriteMsr (
  IN UINT32     Index,
  IN UINT64     Value
  );

/**
  Read Time stamp.

  @param[in]  None

  @retval  Return the read data

**/
UINT64
IA32API
EfiReadTsc (
  VOID
  );

/**
  Writing back and invalidate the cache,then diable it.

  @param[in]  None

  @retval  None

**/
VOID
IA32API
EfiDisableCache (
  VOID
  );

/**
  Invalidate the cache,then Enable it.

  @param[in]  None

  @retval  None

**/
VOID
IA32API
EfiEnableCache (
  VOID
  );

/**
  Get Eflags

  @param[in]  None
  @retval     Return the Eflags value

**/
UINT32
IA32API
EfiGetEflags (
  VOID
  );

/**
  Disable Interrupts

  @param[in]  None

  @retval  None

**/
VOID
IA32API
EfiDisableInterrupts (
  VOID
  );

/**
  Enable Interrupts

  @param[in]  None

  @retval  None

**/
VOID
IA32API
EfiEnableInterrupts (
  VOID
  );

/**
  Extract CPU detail version infomation

  @param[in] FamilyId     FamilyId, including ExtendedFamilyId
  @param[in] Model        Model, including ExtendedModel
  @param[in] SteppingId   SteppingId
  @param[in] Processor    Processor

**/
VOID
IA32API
EfiCpuVersion (
  IN   UINT16  *FamilyId,    OPTIONAL
  IN   UINT8   *Model,       OPTIONAL
  IN   UINT8   *SteppingId,  OPTIONAL
  IN   UINT8   *Processor    OPTIONAL
  );

#endif

