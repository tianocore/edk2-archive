/** @file
  Header file for CpuRng Lib.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_RNG_LIB_H_
#define _CPU_RNG_LIB_H_

/**
  Gets a random number from the CPU's 16 bits random number.

  @param[out]  UINT16       Random value

**/
UINT16
EFIAPI
GetRandomNumber16 (
  VOID
  );

/**
  Gets a random number from the CPU's 32 bits random number.

  @param[out]  UINT32      Random value

**/
UINT32
EFIAPI
GetRandomNumber32 (
  VOID
  );

/**
  Gets a random number from the CPU's 64 bits random number and only in 64 bits environment.

  @param[out]  UINT64      Random value

**/
UINT64
EFIAPI
GetRandomNumber64 (
  VOID
  );
#endif

