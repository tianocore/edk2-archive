/** @file
  Code to support MTRR synch operations.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/CpuPlatformLib.h>
#include <Private/Library/CpuCommonLib.h>
#include <MpService.h>

extern MP_SYSTEM_DATA *mMPSystemData;

EFI_MTRR_VALUES mFixedMtrrValues[] = {
  { IA32_MTRR_FIX64K_00000, 0},
  { IA32_MTRR_FIX16K_80000, 0},
  { IA32_MTRR_FIX16K_A0000, 0},
  { IA32_MTRR_FIX4K_C0000,  0},
  { IA32_MTRR_FIX4K_C8000,  0},
  { IA32_MTRR_FIX4K_D0000,  0},
  { IA32_MTRR_FIX4K_D8000,  0},
  { IA32_MTRR_FIX4K_E0000,  0},
  { IA32_MTRR_FIX4K_E8000,  0},
  { IA32_MTRR_FIX4K_F0000,  0},
  { IA32_MTRR_FIX4K_F8000,  0}
};

EFI_MTRR_VALUES mMtrrDefType[] = { { CACHE_IA32_MTRR_DEF_TYPE, 0 } };

///
/// Pre-defined Variable MTRR number to 20
///
EFI_MTRR_VALUES mVariableMtrrValues[] = {
  { CACHE_VARIABLE_MTRR_BASE,      0},
  { CACHE_VARIABLE_MTRR_BASE + 1,  0},
  { CACHE_VARIABLE_MTRR_BASE + 2,  0},
  { CACHE_VARIABLE_MTRR_BASE + 3,  0},
  { CACHE_VARIABLE_MTRR_BASE + 4,  0},
  { CACHE_VARIABLE_MTRR_BASE + 5,  0},
  { CACHE_VARIABLE_MTRR_BASE + 6,  0},
  { CACHE_VARIABLE_MTRR_BASE + 7,  0},
  { CACHE_VARIABLE_MTRR_BASE + 8,  0},
  { CACHE_VARIABLE_MTRR_BASE + 9,  0},
  { CACHE_VARIABLE_MTRR_BASE + 10, 0},
  { CACHE_VARIABLE_MTRR_BASE + 11, 0},
  { CACHE_VARIABLE_MTRR_BASE + 12, 0},
  { CACHE_VARIABLE_MTRR_BASE + 13, 0},
  { CACHE_VARIABLE_MTRR_BASE + 14, 0},
  { CACHE_VARIABLE_MTRR_BASE + 15, 0},
  { CACHE_VARIABLE_MTRR_BASE + 16, 0},
  { CACHE_VARIABLE_MTRR_BASE + 17, 0},
  { CACHE_VARIABLE_MTRR_BASE + 18, 0},
  { CACHE_VARIABLE_MTRR_BASE + 19, 0}
};

/**
  Save the MTRR registers to global variables

**/
VOID
ReadMtrrRegisters (
  VOID
  )
{
  UINT32  Index;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  ///
  /// Only support MAXIMUM_VARIABLE_MTRR_NUMBER variable MTRR
  ///
  ASSERT (VariableMtrrLimit <= V_MAXIMUM_VARIABLE_MTRR_NUMBER);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }
  ///
  /// Read Fixed Mtrrs
  ///
  for (Index = 0; Index < sizeof (mFixedMtrrValues) / sizeof (EFI_MTRR_VALUES); Index++) {
    mFixedMtrrValues[Index].Value = AsmReadMsr64 (mFixedMtrrValues[Index].Index);
  }
  ///
  /// Read def type Fixed Mtrrs
  ///
  mMtrrDefType[0].Value = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);

  ///
  /// Read Variable Mtrr
  ///
  for (Index = 0; Index < VariableMtrrLimit * 2; Index++) {
    mVariableMtrrValues[Index].Value = AsmReadMsr64 (mVariableMtrrValues[Index].Index);
  }

  return;
}


/**
  Synch up the MTRR values for all processors

  @param[in]  Buffer      Not used.

**/
VOID
EFIAPI
MpMtrrSynchUp (
  IN VOID *Buffer
  )
{
  UINT32              Index;
  UINTN               Cr4;
  UINT64              MsrValue;
  UINT64              ValidMtrrAddressMask;
  EFI_CPUID_REGISTER  FeatureInfo;
  EFI_CPUID_REGISTER  FunctionInfo;
  UINT8               PhysicalAddressBits;
  UINT32              VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  //
  // Only support MAXIMUM_VARIABLE_MTRR_NUMBER variable MTRR
  //
  ASSERT (VariableMtrrLimit <= V_MAXIMUM_VARIABLE_MTRR_NUMBER);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  //
  // ASM code to setup processor register before synching up the MTRRs
  //
  Cr4 = MpMtrrSynchUpEntry ();

  //
  // Get physical CPU MTRR width in case of difference from BSP
  //
  AsmCpuid (
    CPUID_EXTENDED_FUNCTION,
    &FunctionInfo.RegEax,
    &FunctionInfo.RegEbx,
    &FunctionInfo.RegEcx,
    &FunctionInfo.RegEdx
    );
  PhysicalAddressBits = 36;
  if (FunctionInfo.RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (
      CPUID_VIR_PHY_ADDRESS_SIZE,
      &FeatureInfo.RegEax,
      &FeatureInfo.RegEbx,
      &FeatureInfo.RegEcx,
      &FeatureInfo.RegEdx
      );
    PhysicalAddressBits = (UINT8) FeatureInfo.RegEax;
  }

  ValidMtrrAddressMask = (LShiftU64 (1, PhysicalAddressBits) - 1) & 0xfffffffffffff000;

  //
  // Disable Fixed Mtrrs
  //
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, mMtrrDefType[0].Value & 0xFFFFF7FF);

  //
  // Update Fixed Mtrrs
  //
  for (Index = 0; Index < sizeof (mFixedMtrrValues) / sizeof (EFI_MTRR_VALUES); Index++) {
    AsmWriteMsr64 (mFixedMtrrValues[Index].Index, mFixedMtrrValues[Index].Value);
  }

  //
  // Synchup def type Fixed Mtrrs
  //
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, mMtrrDefType[0].Value);

  //
  // Synchup Base Variable Mtrr
  //
  for (Index = 0; Index < VariableMtrrLimit * 2 - 1; Index += 2) {
    MsrValue = (mVariableMtrrValues[Index].Value & 0x0FFF) | (mVariableMtrrValues[Index].Value & ValidMtrrAddressMask);
    AsmWriteMsr64 (mVariableMtrrValues[Index].Index, MsrValue);
  }

  //
  // Synchup Mask Variable Mtrr including valid bit
  //
  for (Index = 1; Index < VariableMtrrLimit * 2; Index += 2) {
    MsrValue = (mVariableMtrrValues[Index].Value & 0x0FFF) | (mVariableMtrrValues[Index].Value & ValidMtrrAddressMask);
    AsmWriteMsr64 (mVariableMtrrValues[Index].Index, MsrValue);
  }

  //
  // ASM code to setup processor register after synching up the MTRRs
  //
  MpMtrrSynchUpExit (Cr4);

  return;
}


/**
  Copy Global MTRR data to S3

**/
VOID
SaveBspMtrrForS3 (
  VOID
  )
{
  UINTN   Index;
  UINTN   TableIndex;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  //
  // Only support MAXIMUM_VARIABLE_MTRR_NUMBER variable MTRR
  //
  ASSERT (VariableMtrrLimit <= V_MAXIMUM_VARIABLE_MTRR_NUMBER);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  TableIndex = 0;
  for (Index = 0; Index < sizeof (mFixedMtrrValues) / sizeof (EFI_MTRR_VALUES); Index++) {
    mMPSystemData->S3BspMtrrTable[TableIndex].Index = mFixedMtrrValues[Index].Index;
    mMPSystemData->S3BspMtrrTable[TableIndex].Value = mFixedMtrrValues[Index].Value;
    TableIndex++;
  }

  for (Index = 0; Index < VariableMtrrLimit * 2; Index++) {
    mMPSystemData->S3BspMtrrTable[TableIndex].Index = mVariableMtrrValues[Index].Index;
    mMPSystemData->S3BspMtrrTable[TableIndex].Value = mVariableMtrrValues[Index].Value;
    TableIndex++;
  }

  mMPSystemData->S3BspMtrrTable[TableIndex].Index = CACHE_IA32_MTRR_DEF_TYPE;
  mMPSystemData->S3BspMtrrTable[TableIndex].Value = mMtrrDefType[0].Value;

  ASSERT (TableIndex < MAX_CPU_S3_MTRR_ENTRY);

  return;
}

