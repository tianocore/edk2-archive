/** @file
  Some definitions for MP services Ppi.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MP_SERVICE_H_
#define _MP_SERVICE_H_

#include <Library/ReportStatusCodeLib.h>
#include <Private/Library/MpServiceLib.h>

///
/// Combine f(FamilyId), m(Model), s(SteppingId) to a single 32 bit number
///
#define EfiMakeCpuVersion(f, m, s)  (((UINT32) (f) << 16) | ((UINT32) (m) << 8) | ((UINT32) (s)))

/**
  Get CPU platform features settings to fill MP data.

  @param[in] WakeUpBuffer               The address of wakeup buffer.
  @param[in] StackAddressStart          The start address of APs's stacks.
  @param[in] MaximumCPUsForThisSystem   Maximum CPUs in this system.

  @retval    EFI_SUCCESS                Function successfully executed.
  @retval    Other                      Error occurred while allocating memory.

**/
EFI_STATUS
EFIAPI
FillMpData (
  IN UINTN        WakeUpBuffer,
  IN VOID         *StackAddressStart,
  IN UINTN        MaximumCPUsForThisSystem
  );

/**
  This function is called by all processors (both BSP and AP) once and collects MP related data

  @param[in] BSP             TRUE if the CPU is BSP
  @param[in] BistParam       BIST (build-in self test) data for the processor. This data
                             is only valid for processors that are waked up for the 1st
                             time in this CPU DXE driver.

  @retval    EFI_SUCCESS     Data for the processor collected and filled in

**/
EFI_STATUS
FillInProcessorInformation (
  IN BOOLEAN        BSP,
  IN UINT32         BistParam
  );

/**
  Notification function that gets called once permanent memory installed to take care
  of MP CPU related activities in PEI phase

  @param[in] PeiServices     Indirect reference to the PEI Services Table
  @param[in] NotifyDesc      Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi             Pointer to the PPI data associated with this function.

  @retval    EFI_SUCCESS     Multiple processors are intialized successfully

**/
EFI_STATUS
InitializeMpSupport (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

/**
  Re-load microcode patch.

  @retval   EFI_SUCCESS         Multiple processors re-load microcode patch

**/
EFI_STATUS
ReloadMicrocode (
  IN MP_CPU_EXCHANGE_INFO     *ExchangeInfo
  );

/**
  Get processor feature

  @param[in]  Features        Pointer to a buffer which stores feature information

**/
VOID
EFIAPI
GetProcessorFeatures (
  IN UINT32 *Features
  );

/**
  Switch BSP to the processor which has least features

  @retval  EFI_STATUS       Status code returned from each sub-routines

**/
EFI_STATUS
EFIAPI
SwitchToLowestFeatureProcess (
  VOID
  );

/**
  Find out the common features supported by all core/threads

**/
VOID
EFIAPI
GetProcessorCommonFeature (
  VOID
  );

/**
  Get the processor data with least features

**/
VOID
EFIAPI
GetProcessorWithLeastFeature (
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
EFIAPI
EfiCpuVersion (
  IN  OUT UINT16  *FamilyId,    OPTIONAL
  IN  OUT UINT8   *Model,       OPTIONAL
  IN  OUT UINT8   *SteppingId,  OPTIONAL
  IN  OUT UINT8   *Processor    OPTIONAL
  );

/**
  Update some processor info into LEAST_FEATURE_PROC data structure.

  @param[in] Index                    Indicate which processor calling this routine
  @param[in] LeastFeatureProcessor    The data structure that will be updated

**/
VOID
EFIAPI
UpdateProcessorInfo (
  IN UINTN              Index,
  IN LEAST_FEATURE_PROC *LeastFeatureProcessor
  );

/**
  Get processor feature delta

  @param[in] FeaturesInput     Supported features for input processor
  @param[in] CommonFeatures    Supported features for processor (subset of FeaturesInput)

  @retval    The least of processor features

**/
UINT32
EFIAPI
GetProcessorFeatureDelta (
  IN UINT32 *FeaturesInput,
  IN UINT32 *CommonFeatures
  );

/**
  Calculate how many bits are one from given number

  @param[in]  Value    Number that will be calculated bits

  @retval     Number of bits

**/
UINT32
EFIAPI
GetBitsNumberOfOne (
  IN UINT32 Value
  );

/**
  Exchange 2 processors (BSP to AP or AP to BSP)

  @param[in]  MyInfo           CPU info for current processor
  @param[in]  OthersInfo       CPU info that will be exchanged with

**/
VOID
AsmExchangeRole (
  IN CPU_EXCHANGE_ROLE_INFO *MyInfo,
  IN CPU_EXCHANGE_ROLE_INFO *OthersInfo
  );
#endif

