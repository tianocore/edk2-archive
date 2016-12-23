/** @file
  CPU MTRR programming driver.

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
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "MpService.h"
#include "MemoryAttribute.h"
#include "CpuInitDxe.h"

GLOBAL_REMOVE_IF_UNREFERENCED FIXED_MTRR mFixedMtrrTable[V_FIXED_MTRR_NUMBER];

GLOBAL_REMOVE_IF_UNREFERENCED FIXED_MTRR mFixedMtrrTable[] = {
  { IA32_MTRR_FIX64K_00000, 0,       0x10000 },
  { IA32_MTRR_FIX16K_80000, 0x80000, 0x4000  },
  { IA32_MTRR_FIX16K_A0000, 0xA0000, 0x4000  },
  { IA32_MTRR_FIX4K_C0000,  0xC0000, 0x1000  },
  { IA32_MTRR_FIX4K_C8000,  0xC8000, 0x1000  },
  { IA32_MTRR_FIX4K_D0000,  0xD0000, 0x1000  },
  { IA32_MTRR_FIX4K_D8000,  0xD8000, 0x1000  },
  { IA32_MTRR_FIX4K_E0000,  0xE0000, 0x1000  },
  { IA32_MTRR_FIX4K_E8000,  0xE8000, 0x1000  },
  { IA32_MTRR_FIX4K_F0000,  0xF0000, 0x1000  },
  { IA32_MTRR_FIX4K_F8000,  0xF8000, 0x1000  },
};

GLOBAL_REMOVE_IF_UNREFERENCED MTRR_VALUE mFixedMtrrValueTable[] = {
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
  { 0, FALSE },
};

GLOBAL_REMOVE_IF_UNREFERENCED VARIABLE_MTRR  mVariableMtrr[V_MAXIMUM_VARIABLE_MTRR_NUMBER];
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mUsedMtrr;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32         mVariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8          mDefaultMemoryType = EFI_MEMORY_UC;

extern UINT64  mValidMtrrAddressMask;
extern UINT64  mValidMtrrBitsMask;
extern BOOLEAN mVariableMtrrChanged;
extern BOOLEAN mFixedMtrrChanged;


/**
  Disable Cache MTRR

**/
VOID
PreMtrrChange (
  VOID
  )
{
  UINT64  TempQword;

  AsmDisableCache ();

  //
  // Disable Cache MTRR
  //
  TempQword = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  TempQword = TempQword &~B_CACHE_MTRR_VALID &~B_CACHE_FIXED_MTRR_VALID;
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword);

  return;
}


/**
  Enable Cache MTRR

**/
VOID
PostMtrrChange (
  VOID
  )
{
  UINT64  TempQword;

  TempQword = 0;

  //
  // Enable Cache MTRR
  //
  TempQword = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword | B_CACHE_MTRR_VALID | B_CACHE_FIXED_MTRR_VALID);

  AsmEnableCache ();
  return;
}


/**
  Calculate fixed MTRR

  @param[in]  MemoryCacheType   Cache type for this memory range
  @param[in]  Base              Memory range base address
  @param[in]  Length            Memory range length

  @retval     EFI_UNSUPPORTED   Fixed MTRR number not enough or not present
  @retval     EFI_SUCCESS       Fixed MTRR settings calculated successfully

**/
EFI_STATUS
CalculateFixedMtrr (
  IN UINT64 MemoryCacheType,
  IN UINT64 *Base,
  IN UINT64 *Length
  )
{
  UINT32  MsrNum;
  UINT32  ByteShift;
  UINT64  TempQword;
  UINT64  OrMask;
  UINT64  ClearMask;
  UINT64  MtrrValue;

  TempQword = 0;
  OrMask    = 0;
  ClearMask = 0;

  //
  // Locate the Fixed MTRR MSR that contains the BaseAddress.
  //
  for (MsrNum = 0; MsrNum < V_FIXED_MTRR_NUMBER; MsrNum++) {
    if ((*Base >= mFixedMtrrTable[MsrNum].BaseAddress) &&
        (*Base < (mFixedMtrrTable[MsrNum].BaseAddress + 8 * mFixedMtrrTable[MsrNum].Length))
        ) {
      break;
    }
  }

  if (MsrNum == V_FIXED_MTRR_NUMBER) {
    return EFI_UNSUPPORTED;
  }

  //
  // We found the fixed MTRR to be programmed
  //
  for (ByteShift = 0; ByteShift < 8; ByteShift++) {
    if (*Base == (mFixedMtrrTable[MsrNum].BaseAddress + ByteShift * mFixedMtrrTable[MsrNum].Length)) {
      break;
    }
  }

  if (ByteShift == 8) {
    return EFI_UNSUPPORTED;
  }

  //
  // Create memory attribute and/or masks for the found fixed MTRR.
  //
  for (; ((ByteShift < 8) && (*Length >= mFixedMtrrTable[MsrNum].Length)); ByteShift++) {
    OrMask |= LShiftU64 ((UINT64) MemoryCacheType, (UINT32) (ByteShift * 8));
    ClearMask |= LShiftU64 ((UINT64) 0xFF, (UINT32) (ByteShift * 8));
    *Length -= mFixedMtrrTable[MsrNum].Length;
    *Base += mFixedMtrrTable[MsrNum].Length;
  }

  if (ByteShift < 8 && (*Length != 0)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check the fixed MTRR changed or not.
  //
  MtrrValue = AsmReadMsr64 (mFixedMtrrTable[MsrNum].Msr);
  TempQword = (MtrrValue & ~ClearMask) | OrMask;
  if (MtrrValue != TempQword) {
    mFixedMtrrChanged = TRUE;
    mFixedMtrrValueTable[MsrNum].MsrValue = TempQword;
    mFixedMtrrValueTable[MsrNum].Changed  = TRUE;
  }
  return EFI_SUCCESS;
}


/**
  Program fixed MTRR

**/
VOID
ProgramFixedMtrr (
  VOID
  )
{
  UINT32  MsrNum;

  PreMtrrChange ();
  for (MsrNum = 0; MsrNum < V_FIXED_MTRR_NUMBER; MsrNum++) {
    if (mFixedMtrrValueTable[MsrNum].Changed) {
      AsmWriteMsr64 (mFixedMtrrTable[MsrNum].Msr, mFixedMtrrValueTable[MsrNum].MsrValue);
      mFixedMtrrValueTable[MsrNum].Changed = FALSE;
    }
  }

  PostMtrrChange ();
}


/**
  Get all information about memory cache registers

  @retval  EFI_SUCCESS    always success

**/
EFI_STATUS
GetMemoryAttribute (
  VOID
  )
{
  UINTN   Index;
  UINT32  MsrNum;
  UINT64  MsrValue;

  mVariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  DEBUG ((DEBUG_INFO, "Variable-range MTRR count = %d \n", mVariableMtrrLimit));

  if (mVariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    mVariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
    DEBUG ((DEBUG_INFO, "Capping Variable-range MTRR count to %d \n", mVariableMtrrLimit));
  }

  //
  // Get Default Mtrr Type
  //
  MsrValue            = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  mDefaultMemoryType  = (UINT8) MsrValue;

  //
  // Get Variable Mtrr
  //
  ZeroMem (mVariableMtrr, (sizeof (VARIABLE_MTRR)) * mVariableMtrrLimit);
  mUsedMtrr = 0;

  for (MsrNum = CACHE_VARIABLE_MTRR_BASE, Index = 0;
       ((MsrNum < (CACHE_VARIABLE_MTRR_BASE + mVariableMtrrLimit * 2 - 1)) && (Index < mVariableMtrrLimit));
       MsrNum += 2
       ) {
    if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) != 0) {
      mVariableMtrr[Index].Msr          = MsrNum;
      mVariableMtrr[Index].BaseAddress  = (AsmReadMsr64 (MsrNum) & mValidMtrrAddressMask);
      mVariableMtrr[Index].Length       = ((~((AsmReadMsr64 (MsrNum + 1) & mValidMtrrAddressMask))) & mValidMtrrBitsMask) + 1;
      mVariableMtrr[Index].Type         = (AsmReadMsr64 (MsrNum) & 0x0ff);
      mVariableMtrr[Index].Valid        = TRUE;
      mUsedMtrr++;
      Index++;
    }
  }

  return EFI_SUCCESS;
}


/**
  Check if different memory attribute range overlapping with each other

  @param[in]  Start   Start of memory range that will be checking
  @param[in]  End     End of memory range address that will be checking

  @retval     TRUE    If overlapping found
  @retval     FALSE   If not found

**/
BOOLEAN
CheckMemoryAttributeOverlap (
  IN EFI_PHYSICAL_ADDRESS Start,
  IN EFI_PHYSICAL_ADDRESS End
  )
{
  UINT32  Index;

  for (Index = 0; Index < mVariableMtrrLimit; Index++) {
    if (mVariableMtrr[Index].Valid && !(Start > (mVariableMtrr[Index].BaseAddress + mVariableMtrr[Index].Length - 1) || (End < mVariableMtrr[Index].BaseAddress))) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Combine current memory attribute range to existing memory attribute range

  @param[in]  Attributes          Cache type
  @param[in]  Base                Base address of memory range that will be combined into existing one.
  @param[in]  Length              Length of the memory range that will be combined into existing one.

  @retval     EFI_SUCCESS         Memory combined successfully
  @retval     EFI_ACCESS_DENIED   Memory type that is not allowed to overlap

**/
EFI_STATUS
CombineMemoryAttribute (
  IN UINT64 Attributes,
  IN UINT64 *Base,
  IN UINT64 *Length
  )
{
  UINT32  Index;
  UINT64  CombineStart;
  UINT64  CombineEnd;
  UINT64  MtrrEnd;
  UINT64  EndAddress;
  BOOLEAN InvalidMTRRs[V_MAXIMUM_VARIABLE_MTRR_NUMBER];

  EndAddress = *Base + *Length - 1;

  for (Index = 0; Index < V_MAXIMUM_VARIABLE_MTRR_NUMBER; Index++) {
    InvalidMTRRs[Index] = FALSE;
  }

  Index = 0;
  while (Index < mVariableMtrrLimit) {
    MtrrEnd = mVariableMtrr[Index].BaseAddress + mVariableMtrr[Index].Length - 1;

    //
    // The MTRR is marked invalid or the ranges are not intersected.
    //
    if (InvalidMTRRs[Index] ||
        !mVariableMtrr[Index].Valid ||
        (*Base > (MtrrEnd) || (EndAddress < mVariableMtrr[Index].BaseAddress))
        ) {
      Index++;
      continue;
    }
    //
    // if the requested range contains MTRR range, invalidate this MTRR
    //
    if (mVariableMtrr[Index].BaseAddress >= *Base && MtrrEnd <= EndAddress) {
      InvalidMTRRs[Index] = TRUE;
      Index++;
      continue;
    }

    if (Attributes == mVariableMtrr[Index].Type) {
      //
      // if the Mtrr range contain the request range, return EFI_SUCCESS
      //
      if (mVariableMtrr[Index].BaseAddress <= *Base && MtrrEnd >= EndAddress) {
        *Length = 0;
        return EFI_SUCCESS;
      }
      //
      // invalid this MTRR, and program the combine range
      //
      CombineStart  = (*Base) < mVariableMtrr[Index].BaseAddress ? (*Base) : mVariableMtrr[Index].BaseAddress;
      CombineEnd    = EndAddress > MtrrEnd ? EndAddress : MtrrEnd;

      //
      // Record this MTRR as invalid
      //
      InvalidMTRRs[Index] = TRUE;

      //
      // The range is modified, retry from the first MTRR
      //
      if (*Base != CombineStart || *Length != CombineEnd - CombineStart + 1) {
        Index = 0;
      } else {
        Index++;
      }

      *Base       = CombineStart;
      *Length     = CombineEnd - CombineStart + 1;
      EndAddress  = CombineEnd;
      continue;
    }

    if ((Attributes == CACHE_UNCACHEABLE) ||
        (Attributes == CACHE_WRITETHROUGH && mVariableMtrr[Index].Type == CACHE_WRITEBACK) ||
        (Attributes == CACHE_WRITEBACK && mVariableMtrr[Index].Type == CACHE_WRITETHROUGH) ||
        (Attributes == CACHE_WRITETHROUGH && mVariableMtrr[Index].Type == CACHE_UNCACHEABLE) ||
        (Attributes == CACHE_WRITEBACK && mVariableMtrr[Index].Type == CACHE_UNCACHEABLE)
        ) {
      Index++;
      continue;
    }
    //
    // Other type memory overlap is invalid
    //
    return EFI_ACCESS_DENIED;
  }
  //
  // Finally invalidate recorded MTRRs
  //
  for (Index = 0; Index < mVariableMtrrLimit; Index++) {
    if (InvalidMTRRs[Index]) {
      InvariableMtrr (mVariableMtrr[Index].Msr, Index);
    }
  }

  return EFI_SUCCESS;
}


/**
  Given the input, check if the number of MTRR is lesser
  if positive or subtractive

  @param[in]  Input          Length of Memory to program MTRR
  @param[in]  MtrrNumber     Return needed Mtrr number
  @param[in]  Direction      TRUE: do positive
                             FALSE: do subtractive

  @retval     EFI_SUCCESS    Always return success

**/
EFI_STATUS
GetDirection (
  IN UINT64  Input,
  IN UINTN   *MtrrNumber,
  IN BOOLEAN *Direction
  )
{
  UINT64  TempQword;
  UINT32  Positive;
  UINT32  Subtractive;

  TempQword   = Input;
  Positive    = 0;
  Subtractive = 0;

  do {
    TempQword -= Power2MaxMemory (TempQword);
    Positive++;

  } while (TempQword != 0);

  TempQword = Power2MaxMemory (LShiftU64 (Input, 1)) - Input;
  Subtractive++;
  do {
    TempQword -= Power2MaxMemory (TempQword);
    Subtractive++;

  } while (TempQword != 0);

  if (Positive <= Subtractive) {
    *Direction  = TRUE;
    *MtrrNumber = Positive;
  } else {
    *Direction  = FALSE;
    *MtrrNumber = Subtractive;
  }

  return EFI_SUCCESS;
}


/**
  Calculate max memory of power 2

  @param[in]  MemoryLength    Memory length that will be calculated

  @retval     Max memory

**/
UINT64
Power2MaxMemory (
  IN UINT64 MemoryLength
  )
{
  UINT64  Result;
  UINT32  *ResultPointer;
  UINT32  *MemoryLengthPointer;

  MemoryLengthPointer = (UINT32 *) &MemoryLength;
  ResultPointer       = (UINT32 *) &Result;
  Result              = 0;
  if (MemoryLengthPointer[1] != 0) {
    ResultPointer[1] = GetPowerOfTwo32 (MemoryLengthPointer[1]);
  } else {
    ResultPointer[0] = GetPowerOfTwo32 (MemoryLengthPointer[0]);
  }

  return Result;
}


/**
  Clear MTRR

  @param[in] MtrrNumber    MTRR register that will be cleared
  @param[in] Index         Index of MTRR register

  @retval    EFI_SUCCESS   Always return success

**/
EFI_STATUS
InvariableMtrr (
  IN UINTN MtrrNumber,
  IN UINTN Index
  )
{
  PreMtrrChange ();

  mVariableMtrr[Index].Valid = FALSE;
  AsmWriteMsr64 ((UINT32) MtrrNumber, 0);
  AsmWriteMsr64 ((UINT32) (MtrrNumber + 1), 0);
  mUsedMtrr--;

  PostMtrrChange ();

  return EFI_SUCCESS;
}


/**
  Programm VARIABLE MTRR

  @param[in]  MtrrNumber        Variable MTRR register
  @param[in]  BaseAddress       Memory base address
  @param[in]  Length            Memory length
  @param[in]  MemoryCacheType   Cache type

  @retval     EFI_SUCCESS       Always return success

**/
EFI_STATUS
ProgramVariableMtrr (
  IN UINTN                MtrrNumber,
  IN EFI_PHYSICAL_ADDRESS BaseAddress,
  IN UINT64               Length,
  IN UINT64               MemoryCacheType
  )
{
  UINT64  TempQword;

  PreMtrrChange ();

  //
  // MTRR Physical Base
  //
  TempQword = (BaseAddress & mValidMtrrAddressMask) | MemoryCacheType;
  AsmWriteMsr64 ((UINT32) MtrrNumber, TempQword);

  //
  // MTRR Physical Mask
  //
  TempQword = ~(Length - 1);
  AsmWriteMsr64 ((UINT32) (MtrrNumber + 1), (TempQword & mValidMtrrAddressMask) | B_CACHE_MTRR_VALID);

  //
  // Variable MTRR is updated
  //
  mVariableMtrrChanged = TRUE;

  PostMtrrChange ();

  return EFI_SUCCESS;
}


/**
  Get GCD Mem Space type from Mtrr Type

  @param[in]  MtrrAttributes - Mtrr type

  @retval     GCD Mem Space typed (64-bit)
  @retval     EFI_MEMORY_UC    Input MTRR type is Uncacheable
  @retval     EFI_MEMORY_WC    Input MTRR type is Write Combining
  @retval     EFI_MEMORY_WT    Input MTRR type is Write-through
  @retval     EFI_MEMORY_WP    Input MTRR type is Write-protected
  @retval     EFI_MEMORY_WB    Input MTRR type is Write Back

**/
UINT64
GetMemorySpaceAttributeFromMtrrType (
  IN UINT8 MtrrAttributes
  )
{
  switch (MtrrAttributes) {
    case CACHE_UNCACHEABLE:
      return EFI_MEMORY_UC;

    case CACHE_WRITECOMBINING:
      return EFI_MEMORY_WC;

    case CACHE_WRITETHROUGH:
      return EFI_MEMORY_WT;

    case CACHE_WRITEPROTECTED:
      return EFI_MEMORY_WP;

    case CACHE_WRITEBACK:
      return EFI_MEMORY_WB;

    default:
      return 0;
  }
}


/**
  Refresh the GCD Memory Space Attributes according to MTRRs

  @retval  EFI_STATUS   Status returned from each sub-routine

**/
EFI_STATUS
RefreshGcdMemoryAttributes (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            Index;
  UINTN                            SubIndex;
  UINT64                           RegValue;
  EFI_PHYSICAL_ADDRESS             BaseAddress;
  UINT64                           Length;
  UINT64                           Attributes;
  UINT64                           CurrentAttributes;
  UINT8                            MtrrType;
  UINTN                            NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  *MemorySpaceMap;
  UINT64                           DefaultAttributes;

  MemorySpaceMap  = NULL;

  Status = GetMemoryAttribute ();
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = gDS->GetMemorySpaceMap (
                  &NumberOfDescriptors,
                  &MemorySpaceMap
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  DefaultAttributes = GetMemorySpaceAttributeFromMtrrType (mDefaultMemoryType);

  //
  // Set default attributes to all spaces.
  //
  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeNonExistent) {
      continue;
    }

    gDS->SetMemorySpaceAttributes (
           MemorySpaceMap[Index].BaseAddress,
           MemorySpaceMap[Index].Length,
           (MemorySpaceMap[Index].Attributes &~EFI_MEMORY_CACHETYPE_MASK) |
           (MemorySpaceMap[Index].Capabilities & DefaultAttributes)
           );
  }
  //
  // Go for variable MTRRs, WB first, Other types second
  //
  for (Index = 0; Index < mVariableMtrrLimit; Index++) {
    if (mVariableMtrr[Index].Valid && mVariableMtrr[Index].Type == CACHE_WRITEBACK) {
      SetGcdMemorySpaceAttributes (
        MemorySpaceMap,
        NumberOfDescriptors,
        mVariableMtrr[Index].BaseAddress,
        mVariableMtrr[Index].Length,
        EFI_MEMORY_WB
        );
    }
  }

  for (Index = 0; Index < mVariableMtrrLimit; Index++) {
    if (mVariableMtrr[Index].Valid && mVariableMtrr[Index].Type != CACHE_WRITEBACK) {
      Attributes = GetMemorySpaceAttributeFromMtrrType ((UINT8) mVariableMtrr[Index].Type);
      SetGcdMemorySpaceAttributes (
        MemorySpaceMap,
        NumberOfDescriptors,
        mVariableMtrr[Index].BaseAddress,
        mVariableMtrr[Index].Length,
        Attributes
        );
    }
  }
  //
  // Go for fixed MTRRs
  //
  Attributes  = 0;
  BaseAddress = 0;
  Length      = 0;
  for (Index = 0; Index < V_FIXED_MTRR_NUMBER; Index++) {
    RegValue = AsmReadMsr64 (mFixedMtrrTable[Index].Msr);
    for (SubIndex = 0; SubIndex < 8; SubIndex++) {
      MtrrType = (UINT8) RShiftU64 (RegValue, SubIndex * 8);
      CurrentAttributes = GetMemorySpaceAttributeFromMtrrType (MtrrType);
      if (Length == 0) {
        Attributes = CurrentAttributes;
      } else {
        if (CurrentAttributes != Attributes) {
          SetGcdMemorySpaceAttributes (
            MemorySpaceMap,
            NumberOfDescriptors,
            BaseAddress,
            Length,
            Attributes
            );
          BaseAddress = mFixedMtrrTable[Index].BaseAddress + mFixedMtrrTable[Index].Length * SubIndex;
          Length      = 0;
          Attributes  = CurrentAttributes;
        }
      }

      Length += mFixedMtrrTable[Index].Length;
    }
  }
  //
  // handle the last region
  //
  SetGcdMemorySpaceAttributes (
    MemorySpaceMap,
    NumberOfDescriptors,
    BaseAddress,
    Length,
    Attributes
    );

Done:
  FreePool (MemorySpaceMap);

  return Status;
}


/**
  Search into the Gcd Memory Space for descriptors (from StartIndex
  to EndIndex) that contains the memory range specified by BaseAddress
  and Length.

  @param[in]  MemorySpaceMap       Gcd Memory Space Map as array
  @param[in]  NumberOfDescriptors  Number of descriptors in map
  @param[in]  BaseAddress          BaseAddress for the requested range
  @param[in]  Length               Length for the requested range
  @param[out] StartIndex           Start index into the Gcd Memory Space Map
  @param[out] EndIndex             End index into the Gcd Memory Space Map

  @retval     EFI_SUCCESS          Search successfully
  @retval     EFI_NOT_FOUND        The requested descriptors not exist

**/
EFI_STATUS
SearchGcdMemorySpaces (
  IN EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap,
  IN UINTN                           NumberOfDescriptors,
  IN EFI_PHYSICAL_ADDRESS            BaseAddress,
  IN UINT64                          Length,
  OUT UINTN                          *StartIndex,
  OUT UINTN                          *EndIndex
  )
{
  UINTN  Index;

  *StartIndex = 0;
  *EndIndex   = 0;

  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if (BaseAddress >= MemorySpaceMap[Index].BaseAddress &&
        BaseAddress < MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length
        ) {
      *StartIndex = Index;
    }

    if (BaseAddress + Length - 1 >= MemorySpaceMap[Index].BaseAddress &&
        BaseAddress + Length - 1 < MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length
        ) {
      *EndIndex = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Set the attributes for a specified range in Gcd Memory Space Map.

  @param[in] MemorySpaceMap       Gcd Memory Space Map as array
  @param[in] NumberOfDescriptors  Number of descriptors in map
  @param[in] BaseAddress          BaseAddress for the range
  @param[in] Length               Length for the range
  @param[in] Attributes           Attributes to set

  @retval    EFI_SUCCESS          Set successfully
  @retval    EFI_NOT_FOUND        The specified range does not exist in Gcd Memory Space

**/
EFI_STATUS
SetGcdMemorySpaceAttributes (
  IN EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap,
  IN UINTN                           NumberOfDescriptors,
  IN EFI_PHYSICAL_ADDRESS            BaseAddress,
  IN UINT64                          Length,
  IN UINT64                          Attributes
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  UINTN                 StartIndex;
  UINTN                 EndIndex;
  EFI_PHYSICAL_ADDRESS  RegionStart;
  UINT64                RegionLength;

  Status = SearchGcdMemorySpaces (
             MemorySpaceMap,
             NumberOfDescriptors,
             BaseAddress,
             Length,
             &StartIndex,
             &EndIndex
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = StartIndex; Index <= EndIndex; Index++) {
    if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeNonExistent) {
      continue;
    }

    if (BaseAddress >= MemorySpaceMap[Index].BaseAddress) {
      RegionStart = BaseAddress;
    } else {
      RegionStart = MemorySpaceMap[Index].BaseAddress;
    }

    if (BaseAddress + Length - 1 < MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length) {
      RegionLength = BaseAddress + Length - RegionStart;
    } else {
      RegionLength = MemorySpaceMap[Index].BaseAddress + MemorySpaceMap[Index].Length - RegionStart;
    }

    gDS->SetMemorySpaceAttributes (
           RegionStart,
           RegionLength,
           (MemorySpaceMap[Index].Attributes &~EFI_MEMORY_CACHETYPE_MASK) | (MemorySpaceMap[Index].Capabilities & Attributes)
           );
  }

  return EFI_SUCCESS;
}

