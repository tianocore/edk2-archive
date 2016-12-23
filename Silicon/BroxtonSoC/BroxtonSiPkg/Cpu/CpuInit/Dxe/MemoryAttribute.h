/** @file
  Header file for CPU MTRR programming driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_MEMORY_ATTRIB_H
#define _EFI_MEMORY_ATTRIB_H

extern UINT32 mUsedMtrr;

///
/// This structure represents fixed MTRR
///
typedef struct {
  UINT32 Msr;         ///< Fixed MTRR MSR, as described in EDS.
  UINT32 BaseAddress; ///< Base address of the MTRR.
  UINT32 Length;      ///< Range of this MTRR.
} FIXED_MTRR;

///
/// This structure provides the information on whether a Fixed MTRR needs to be changed
///
typedef struct {
  UINT64  MsrValue; ///< Value read from the fixed MTRR MSR
  BOOLEAN Changed;  ///< Indicator of whether a range has been changed.
} MTRR_VALUE;

///
/// This structure represents variable MTRR
///
typedef struct {
  UINT64  BaseAddress; ///< Base address of the MTRR.
  UINT64  Length;      ///< Range of this MTRR.
  UINT64  Type;        ///< Cacheability type for this memory range.
  UINT32  Msr;         ///< Variable MTRR MSR
  BOOLEAN Valid;       ///< Indicator of whether the range is valid.
} VARIABLE_MTRR;

#if defined (__GNUC__)
#define IA32API                     _EFIAPI
#else
#define IA32API                     __cdecl
#endif

#define EFI_MEMORY_CACHETYPE_MASK (EFI_MEMORY_UC | EFI_MEMORY_WC | EFI_MEMORY_WT | EFI_MEMORY_WB | EFI_MEMORY_UCE)

/**
  Calculate fixed MTRR

  @param[in]  MemoryCacheType    Cache type for this memory range
  @param[in]  Base               Memory range base address
  @param[in]  Length             Memory range length

  @retval     EFI_UNSUPPORTED    Fixed MTRR number not enough or not present
  @retval     EFI_SUCCESS        Fixed MTRR settings calculated successfully

**/
EFI_STATUS
CalculateFixedMtrr (
  IN UINT64 MemoryCacheType,
  IN UINT64 *Base,
  IN UINT64 *Length
  );

/**
  Disable Cache MTRR

**/
VOID
PreMtrrChange (
  VOID
  );

/**
  Enable Cache MTRR

**/
VOID
PostMtrrChange (
  VOID
  );

/**
  Program fixed MTRR

**/
VOID
ProgramFixedMtrr (
  VOID
  );

/**
  Get all information about memory cache registers

  @retval  EFI_SUCCESS     Always success

**/
EFI_STATUS
GetMemoryAttribute (
  VOID
  );

/**
  Check if different memory attribute range overlapping with each other

  @param[in]  Start   Start of memory range that will be checking
  @param[in]  End     End of memory range address that will be checking

  @retval     TRUE    If overlapping found
  @retval     FALSE   If not found

**/
BOOLEAN
CheckMemoryAttributeOverlap (
  IN  EFI_PHYSICAL_ADDRESS    Start,
  IN  EFI_PHYSICAL_ADDRESS    End
  );

/**
  Combine current memory attribute range to existing memory attribute range

  @param[in] Attribute            Cache type
  @param[in] Base                 Base address of memory range that will be combined into existing one.
  @param[in] Length               Length of the memory range that will be combined into existing one.

  @retval    EFI_SUCCESS          Memory combined successfully
  @retval    EFI_ACCESS_DENIED    Memory type that is not allowed to overlap

**/
EFI_STATUS
CombineMemoryAttribute (
  IN UINT64 Attribute,
  IN UINT64 *Base,
  IN UINT64 *Length
  );

/**
  Given the input, check if the number of MTRR is lesser
  if positive or subtractive

  @param[in] Input       Length of Memory to program MTRR
  @param[in] MtrrNumber  Return needed Mtrr number
  @param[in] Direction   TRUE: do positive
                         FALSE: do subtractive

  @retval EFI_SUCCESS    Always return success

**/
EFI_STATUS
GetDirection (
  IN UINT64  Input,
  IN UINTN   *MtrrNumber,
  IN BOOLEAN *Direction
  );

/**
  Calculate max memory of power 2

  @param[in]  MemoryLength    Memory length that will be calculated

  @retval    Max memory

**/
UINT64
Power2MaxMemory (
  IN UINT64 MemoryLength
  );

/**
  Clear MTRR

  @param[in]  MtrrNumber    MTRR register that will be cleared
  @param[in]  Index         Index of MTRR register

  @retval     EFI_SUCCESS   Always return success

**/
EFI_STATUS
InvariableMtrr (
  IN UINTN MtrrNumber,
  IN UINTN Index
  );

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
  );

/**
  Get GCD Mem Space type from Mtrr Type

  @param[in]  MtrrAttribute      Mtrr type

  @retval    GCD Mem Space typed (64-bit)

**/
UINT64
GetMemorySpaceAttributeFromMtrrType (
  IN UINT8 MtrrAttribute
  );

/**
  Refresh the GCD Memory Space Attributes according to MTRRs

  @retval  EFI_STATUS    Status returned from each sub-routine

**/
EFI_STATUS
RefreshGcdMemoryAttributes (
  VOID
  );

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

  @retval    EFI_SUCCESS           Search successfully
  @retval    EFI_NOT_FOUND         The requested descriptors not exist

**/
EFI_STATUS
SearchGcdMemorySpaces (
  IN EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap,
  IN UINTN                           NumberOfDescriptors,
  IN EFI_PHYSICAL_ADDRESS            BaseAddress,
  IN UINT64                          Length,
  OUT UINTN                          *StartIndex,
  OUT UINTN                          *EndIndex
  );

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
  );
#endif

