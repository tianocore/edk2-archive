/** @file
  Null instance of Sec Platform Hook Lib.

  Copyright (c) 2007 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// The package level header files this module uses
//
#include "PlatformSecLib.h"
#include <Library/DebugLib.h>


/**
  Perform those platform specific operations that are requried to be executed as early as possibile.

  @return TRUE always return true.

**/
EFI_STATUS
EFIAPI
PlatformSecLibConstructor (
  )
{
  BOOLEAN                       DefaultsRestored;

  //
  // Init Apic Timer for Performance collection.
  // Use EXCEPT_IA32_BOUND as interrupte type.
  //
  PERF_CODE (
    InitializeApicTimer (0, (UINT32) - 1, TRUE, 5);
  );

  DefaultsRestored = FALSE;

  return EFI_SUCCESS;
}


/**
  A developer supplied function to perform platform specific operations.

  It's a developer supplied function to perform any operations appropriate to a
  given platform. It's invoked just before passing control to PEI core by SEC
  core. Platform developer may modify the SecCoreData and PPI list that is
  passed to PEI Core.

  @param[in, out]  SecCoreData           The same parameter as passing to PEI core. It
                                         could be overridden by this function.
  @param[in]       PpiList               The default PPI list passed from generic SEC part.

  @return          The final PPI list that platform wishes to passed to PEI core.

**/
EFI_PEI_PPI_DESCRIPTOR *
EFIAPI
SecPlatformMain (
  IN OUT   EFI_SEC_PEI_HAND_OFF        *SecCoreData,
  IN       EFI_PEI_PPI_DESCRIPTOR      *PpiList
  )
{
  return NULL;
}


/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param[in]      PeiServices               Pointer to the PEI Services Table.
  @param[in, out] StructureSize             Pointer to the variable describing size of the input buffer.
  @param[out]     PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval         EFI_SUCCESS               The data was successfully returned.
  @retval         EFI_BUFFER_TOO_SMALL      The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN OUT   UINT64                                *StructureSize,
  OUT      EFI_SEC_PLATFORM_INFORMATION_RECORD   *PlatformInformationRecord
  )
{

  UINT32  *Bist;
  UINT32  TopOfCar;

  DEBUG ((EFI_D_INFO, "SecPlatformInformation entry \n"));

  //
  // The entries of BIST information, together with the number of them,
  // reside in the bottom of stack, left untouched by normal stack operation.
  // This routine copies the BIST information to the buffer pointed by
  // PlatformInformationRecord for output.
  //

  TopOfCar = CAR_BASE_ADDR + CAR_SIZE;
  //
  // At this stage we only have information about the BSP.
  //
  Bist = (UINT32 *) (UINTN) (TopOfCar - sizeof (UINT32) - sizeof (UINT32));

  CopyMem (PlatformInformationRecord, Bist, (UINTN) *StructureSize);
  DEBUG ((EFI_D_INFO, "Bist is = %x \n", Bist ));
  return EFI_SUCCESS;
}

