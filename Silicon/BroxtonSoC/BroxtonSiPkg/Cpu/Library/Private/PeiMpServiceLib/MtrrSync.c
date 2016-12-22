/** @file
  Synchronization of MTRRs on S3 boot path.

  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "CpuAccess.h"
#include <Private/Library/MpServiceLib.h>

UINTN
MpMtrrSynchUpEntry (
  VOID
  );

VOID
MpMtrrSynchUpExit (
  UINTN Cr4
  );

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mFixedMtrrIndex[] = {
  IA32_MTRR_FIX64K_00000,
  IA32_MTRR_FIX16K_80000,
  IA32_MTRR_FIX16K_A0000,
  IA32_MTRR_FIX4K_C0000,
  IA32_MTRR_FIX4K_C8000,
  IA32_MTRR_FIX4K_D0000,
  IA32_MTRR_FIX4K_D8000,
  IA32_MTRR_FIX4K_E0000,
  IA32_MTRR_FIX4K_E8000,
  IA32_MTRR_FIX4K_F0000,
  IA32_MTRR_FIX4K_F8000,
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mMtrrDefType[] = { CACHE_IA32_MTRR_DEF_TYPE };

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 mVariableMtrrIndex[] = {
  CACHE_VARIABLE_MTRR_BASE,
  CACHE_VARIABLE_MTRR_BASE + 1,
  CACHE_VARIABLE_MTRR_BASE + 2,
  CACHE_VARIABLE_MTRR_BASE + 3,
  CACHE_VARIABLE_MTRR_BASE + 4,
  CACHE_VARIABLE_MTRR_BASE + 5,
  CACHE_VARIABLE_MTRR_BASE + 6,
  CACHE_VARIABLE_MTRR_BASE + 7,
  CACHE_VARIABLE_MTRR_BASE + 8,
  CACHE_VARIABLE_MTRR_BASE + 9,
  CACHE_VARIABLE_MTRR_BASE + 10,
  CACHE_VARIABLE_MTRR_BASE + 11,
  CACHE_VARIABLE_MTRR_BASE + 12,
  CACHE_VARIABLE_MTRR_BASE + 13,
  CACHE_VARIABLE_MTRR_BASE + 14,
  CACHE_VARIABLE_MTRR_BASE + 15,
  CACHE_VARIABLE_MTRR_BASE + 16,
  CACHE_VARIABLE_MTRR_BASE + 17,
  CACHE_VARIABLE_MTRR_BASE + 18,
  CACHE_VARIABLE_MTRR_BASE + 19,
  ///
  /// CACHE_VARIABLE_MTRR_END,
  ///
};

GLOBAL_REMOVE_IF_UNREFERENCED UINTN FixedMtrrNumber   = sizeof (mFixedMtrrIndex) / sizeof (UINT16);
GLOBAL_REMOVE_IF_UNREFERENCED UINTN MtrrDefTypeNumber = sizeof (mMtrrDefType) / sizeof (UINT16);

/**
  Save the MTRR registers to global variables

  @param[in]  MtrrValues        Pointer to the buffer which stores MTRR settings

**/
VOID
ReadMtrrRegisters (
  IN UINT64           *MtrrValues
  )
{
  UINT32  Index;
  UINT32  VariableMtrrNumber;

  //
  // Read all Mtrrs
  //
  for (Index = 0; Index < FixedMtrrNumber; Index++) {
    *MtrrValues = AsmReadMsr64 (mFixedMtrrIndex[Index]);
    MtrrValues++;
  }

  for (Index = 0; Index < MtrrDefTypeNumber; Index++) {
    *MtrrValues = AsmReadMsr64 (mMtrrDefType[Index]);
    MtrrValues++;
  }

  VariableMtrrNumber = ((UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT));
  if (VariableMtrrNumber > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrNumber = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }
  for (Index = 0; Index < VariableMtrrNumber * 2; Index++) {
    *MtrrValues = AsmReadMsr64 (mVariableMtrrIndex[Index]);
    MtrrValues++;
  }

  return;
}


/**
  Synch up the MTRR values for all processors

  @param[in]  MtrrValues       Pointer to the buffer which stores MTRR settings

**/
VOID
MpMtrrSynchUp (
  UINT64 *MtrrValues
  )
{
  UINT32              Index;
  UINT32              VariableMtrrNumber;
  UINTN               Cr4;
  UINT64              *FixedMtrr;
  UINT64              *MtrrDefType;
  UINT64              *VariableMtrr;
  UINT64              ValidMtrrAddressMask;
  EFI_CPUID_REGISTER  FeatureInfo;
  EFI_CPUID_REGISTER  FunctionInfo;
  UINT8               PhysicalAddressBits;

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

  ValidMtrrAddressMask  = (LShiftU64 (1, PhysicalAddressBits) - 1) & 0xfffffffffffff000ULL;

  FixedMtrr             = MtrrValues;
  MtrrDefType           = MtrrValues + FixedMtrrNumber;
  VariableMtrr          = MtrrValues + FixedMtrrNumber + MtrrDefTypeNumber;

  //
  // ASM code to setup processor register before synching up the MTRRs
  //
  Cr4 = MpMtrrSynchUpEntry ();

  //
  // Disable Fixed Mtrrs
  //
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, MtrrDefType[0] & 0xFFFFF7FF);

  //
  // Update Fixed Mtrrs
  //
  for (Index = 0; Index < FixedMtrrNumber; Index++) {
    AsmWriteMsr64 (mFixedMtrrIndex[Index], FixedMtrr[Index]);
  }
  //
  // Synchup Base Variable Mtrr
  //
  VariableMtrrNumber = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  if (VariableMtrrNumber > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrNumber = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  for (Index = 0; Index < VariableMtrrNumber * 2; Index++) {
    AsmWriteMsr64 (
      mVariableMtrrIndex[Index],
      (VariableMtrr[Index] & 0x0FFF) | (VariableMtrr[Index] & ValidMtrrAddressMask)
      );
  }
  //
  // Synchup def type Fixed Mtrrs
  //
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, MtrrDefType[0]);

  //
  // ASM code to setup processor register after synching up the MTRRs
  //
  MpMtrrSynchUpExit (Cr4);

  return;
}


/**
  Set MTRR registers

  @param[in]  MtrrArray      Buffer with MTRR settings

**/
VOID
SetMtrrRegisters (
  IN EFI_MTRR_VALUES  *MtrrArray
  )
{
  UINT32  Index;
  UINTN   Cr4;

  //
  // ASM code to setup processor register before synching up the MTRRs
  //
  Cr4 = MpMtrrSynchUpEntry ();

  Index = 0;
  while ((MtrrArray[Index].Index != 0) && (MtrrArray[Index].Index >= CACHE_VARIABLE_MTRR_BASE)) {
    AsmWriteMsr64 (MtrrArray[Index].Index, MtrrArray[Index].Value);
    Index++;
  }
  //
  // ASM code to setup processor register after synching up the MTRRs
  //
  MpMtrrSynchUpExit (Cr4);
}


#ifdef EFI_DEBUG
/**
  Print MTRR settings in debug build BIOS

  @param[in]  MtrrArray             Buffer with MTRR settings
**/
VOID
ShowMtrrRegisters (
  IN EFI_MTRR_VALUES  *MtrrArray
  )
{
  UINT32  Index;

  Index = 0;
  while ((MtrrArray[Index].Index != 0) && (MtrrArray[Index].Index >= CACHE_VARIABLE_MTRR_BASE)) {
    DEBUG ((DEBUG_INFO, "MTRR: MtrrArray Index = %x\n", Index));
    DEBUG (
      (DEBUG_INFO,
      "MTRR: MtrrArray[%x].Index = %x    MtrrArray[%x].Value = %x\n",
      Index,
      MtrrArray[Index].Index,
      Index,
      MtrrArray[Index].Value)
      );
    Index++;
  }

  DEBUG ((DEBUG_INFO, "MTRR: Total Index = %x\n", Index));
}
#endif

