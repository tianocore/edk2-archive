/** @file
  MP Support functions.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <MpService.h>

GLOBAL_REMOVE_IF_UNREFERENCED ACPI_CPU_DATA  *mAcpiCpuData;
GLOBAL_REMOVE_IF_UNREFERENCED MP_SYSTEM_DATA *mMPSystemData;

//
// Function declarations
//
/**
  Initializes MP support in the system.

  @retval EFI_SUCCESS            Multiple processors are initialized successfully.
  @retval EFI_OUT_OF_RESOURCES   No enough resoruces (such as out of memory).

**/
EFI_STATUS
InitializeMpSupport (
  VOID
  )
{
  EFI_STATUS            Status;
  MP_CPU_RESERVED_DATA  *MpCpuReservedData;

  MpCpuReservedData = NULL;

  Status = AllocateReservedMemoryBelow4G (
             sizeof (MP_CPU_RESERVED_DATA),
             (VOID **) &MpCpuReservedData
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (MpCpuReservedData != NULL) {
    ZeroMem (MpCpuReservedData, sizeof (MP_CPU_RESERVED_DATA));
  } else {
    ASSERT (MpCpuReservedData != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  mMPSystemData = &(MpCpuReservedData->MPSystemData);
  mAcpiCpuData  = &(MpCpuReservedData->AcpiCpuData);
  mAcpiCpuData->CpuPrivateData          = (EFI_PHYSICAL_ADDRESS) (UINTN) (&(mMPSystemData->S3DataPointer));
  mAcpiCpuData->S3BootPath              = FALSE;
  mAcpiCpuData->IdtrProfile             = (EFI_PHYSICAL_ADDRESS) (UINTN) (&(MpCpuReservedData->IdtrProfile));

  InitializeMpServices ();

  return EFI_SUCCESS;
}

