/** @file
  Header file for CpuPlatform Lib.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_PLATFORM_LIB_H_
#define _CPU_PLATFORM_LIB_H_

#include <CpuRegs.h>
#include <CpuDataStruct.h>

/**
  Check CPU Type of the platform

  @retval  CPU_FAMILY              CPU type

**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  );

/**
  Return Cpu stepping type

  @retval  CPU_STEPPING                   Cpu stepping type

**/
CPU_STEPPING
EFIAPI
GetCpuStepping (
  VOID
  );

/**
  Returns the processor microcode revision of the processor installed in the system.

  @retval  Processor Microcode Revision

**/
UINT32
GetCpuUcodeRevision (
  VOID
  );

/**
  Check if this microcode is correct one for processor

  @param[in] Cpuid                 Processor CPUID
  @param[in] MicrocodeEntryPoint   Entry point of microcode
  @param[in] Revision              Revision of microcode

  @retval    CorrectMicrocode      If this microcode is correct

**/
BOOLEAN
CheckMicrocode (
  IN UINT32               Cpuid,
  IN CPU_MICROCODE_HEADER *MicrocodeEntryPoint,
  IN UINT32               *Revision
  );

/**
  This function will set and lock PRMRR which is required to be locked before enabling normal mode
  for memory.

  @param[in]  PrmrrBase      Base address of PRMRR range.  Must be naturally algined
  @param[in]  PrmrrSize      Size of the PRMRR range in Bytes

**/
VOID
SetUncorePrmrr (
  IN UINT32 PrmrrBase,
  IN UINT32 PrmrrSize
  );

#endif

