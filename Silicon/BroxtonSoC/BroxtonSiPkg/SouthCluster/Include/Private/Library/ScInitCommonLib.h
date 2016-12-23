/** @file
  Header file for SC Init Common Lib.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_INIT_COMMON_LIB_H_
#define _SC_INIT_COMMON_LIB_H_

#include <ScPolicyCommon.h>
#include <ScAccess.h>

/**
  This function returns PID according to Root Port Number.

  @param[in] RpPort             Root Port Number

  @retval    SC_SBI_PID         Returns PID for SBI Access

**/
SC_SBI_PID
GetRpSbiPid (
  IN  UINTN  RpPort
  );

/**
  Calculate root port device number based on physical port index.

  @param[in]  RpIndex              Root port index (0-based).

  @retval     Root port device number.

**/
UINT32
PchGetPcieRpDevice (
  IN  UINT32   RpIndex
  );

/**
  This function reads Pci Config register via SBI Access.

  @param[in]  RpDevice            Root Port Device Number
  @param[in]  RpPort              Root Port Number
  @param[in]  Offset              Offset of Config register
  @param[out] Data32              Value of Config register

  @retval     EFI_SUCCESS         SBI Read successful

**/
EFI_STATUS
PchSbiRpPciRead32 (
  IN    UINTN   RpDevice,
  IN    UINTN   RpPort,
  IN    UINTN   Offset,
  OUT   UINT32  *Data32
  );

/**
  This function And then Or Pci Config register via SBI Access.

  @param[in]  RpDevice            Root Port Device Number
  @param[in]  RpPort              Root Port Number
  @param[in]  Offset              Offset of Config register
  @param[in]  Data32And           Value of Config register to be And-ed
  @param[in]  Data32AOr           Value of Config register to be Or-ed

  @retval     EFI_SUCCESS         SBI Read and Write successful

**/
EFI_STATUS
PchSbiRpPciAndThenOr32 (
  IN  UINTN   RpDevice,
  IN  UINTN   RpPort,
  IN  UINTN   Offset,
  IN  UINT32  Data32And,
  IN  UINT32  Data32Or
  );

/**
  Print registers value

  @param[in] PrintMmioBase       Mmio base address
  @param[in] PrintSize           Number of registers
  @param[in] OffsetFromBase      Offset from mmio base address

  @retval    None

**/
VOID
PrintRegisters (
  IN  UINTN        PrintMmioBase,
  IN  UINT32       PrintSize,
  IN  UINT32       OffsetFromBase
  );

/**
  PrintPchPciConfigSpace

  @param[in]  None

  @retval     None

**/
VOID
PrintPchPciConfigSpace (
  VOID
  );

#endif

