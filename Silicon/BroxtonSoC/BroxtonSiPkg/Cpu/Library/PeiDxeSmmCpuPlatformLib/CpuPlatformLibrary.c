/** @file
  CPU Platform Lib implementation.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CpuPlatformLibrary.h"
#include <Library/MmPciLib.h>
#include <SaRegs.h>

#define SKIP_MICROCODE_CHECKSUM_CHECK 1

/**
  Return CPU Family ID

  @retval  CPU_FAMILY              CPU Family ID

**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  )
{
  EFI_CPUID_REGISTER  Cpuid;

  //
  // Read the CPUID information
  //
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_FAMILY) (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL));
}


/**
  Return Cpu stepping type

  @retval  UINT8                  Cpu stepping type

**/
CPU_STEPPING
EFIAPI
GetCpuStepping (
  VOID
  )
{
  EFI_CPUID_REGISTER  Cpuid;

  //
  // Read the CPUID information
  //
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_STEPPING) (Cpuid.RegEax & CPUID_FULL_STEPPING));
}


/**
  Returns the processor microcode revision of the processor installed in the system.

  @retval Processor Microcode Revision

**/
UINT32
GetCpuUcodeRevision (
  VOID
  )
{
  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);

  return (UINT32) RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32);
}


/**
  Verify the DWORD type checksum

  @param[in] ChecksumAddr         The start address to be checkumed
  @param[in] ChecksumLen          The length of data to be checksumed

  @retval    EFI_SUCCESS          Checksum correct
  @retval    EFI_CRC_ERROR        Checksum incorrect

**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
#if SKIP_MICROCODE_CHECKSUM_CHECK
  return EFI_SUCCESS;
#else
  UINT32  Checksum;
  UINT32  Index;

  Checksum = 0;

  for (Index = 0; Index < ChecksumLen; Index++) {
    Checksum += ChecksumAddr[Index];
  }

  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
#endif
}


/**
  This function checks the MCU revision to decide if BIOS needs to load
  microcode.

  @param[in] MicrocodePointer  Microcode in memory
  @param[in] Revision          Current CPU microcode revision

  @retval    EFI_SUCCESS       BIOS needs to load microcode
  @retval    EFI_ABORTED       Don't need to update microcode

**/
EFI_STATUS
CheckMcuRevision (
  IN CPU_MICROCODE_HEADER *MicrocodePointer,
  IN UINT32               Revision
  )
{
  EFI_STATUS    Status;

  Status = EFI_ABORTED;
  if ((MicrocodePointer->UpdateRevision & 0x80000000) ||
     (MicrocodePointer->UpdateRevision > Revision) ||
     (Revision == 0)){
     Status = EFI_SUCCESS;
  }

  return Status;
}


/**
  Check if this microcode is correct one for processor

  @param[in]  Cpuid                 Processor CPUID
  @param[in]  MicrocodeEntryPoint   Entry point of microcode
  @param[in]  Revision              Revision of microcode

  @retval     CorrectMicrocode      If this microcode is correct

**/
BOOLEAN
CheckMicrocode (
  IN UINT32               Cpuid,
  IN CPU_MICROCODE_HEADER *MicrocodeEntryPoint,
  IN UINT32               *Revision
  )
{
  EFI_STATUS                           Status;
  UINT8                                ExtendedIndex;
  UINT8                                MsrPlatform;
  UINT32                               ExtendedTableLength;
  UINT32                               ExtendedTableCount;
  BOOLEAN                              CorrectMicrocode;
  CPU_MICROCODE_EXTENDED_TABLE         *ExtendedTable;
  CPU_MICROCODE_EXTENDED_TABLE_HEADER  *ExtendedTableHeader;

  Status              = EFI_NOT_FOUND;
  ExtendedTableLength = 0;
  CorrectMicrocode    = FALSE;

  if (MicrocodeEntryPoint == NULL) {
    return FALSE;
  }

  //
  // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  //
  MsrPlatform = (UINT8) (RShiftU64 (AsmReadMsr64 (MSR_IA32_PLATFORM_ID), N_PLATFORM_ID_SHIFT) & B_PLATFORM_ID_MASK);

  //
  // Check if the microcode is for the Cpu and the version is newer
  // and the update can be processed on the platform
  //
  if ((MicrocodeEntryPoint->HeaderVersion == 0x00000001) &&
      !EFI_ERROR (CheckMcuRevision (MicrocodeEntryPoint, *Revision))) {
    if ((MicrocodeEntryPoint->ProcessorId == Cpuid) && (MicrocodeEntryPoint->ProcessorFlags & (1 << MsrPlatform))) {
      if (MicrocodeEntryPoint->DataSize == 0) {
        Status = Checksum32Verify ((UINT32 *) MicrocodeEntryPoint, 2048 / sizeof (UINT32));
      } else {
        Status = Checksum32Verify (
                   (UINT32 *) MicrocodeEntryPoint,
                   (MicrocodeEntryPoint->DataSize + sizeof (CPU_MICROCODE_HEADER)) / sizeof (UINT32)
                   );
      }

      if (!EFI_ERROR (Status)) {
        CorrectMicrocode = TRUE;
      }
    } else if ((MicrocodeEntryPoint->DataSize != 0)) {
      //
      // Check the  Extended Signature if the entended signature exist
      // Only the data size != 0 the extended signature may exist
      //
      ExtendedTableLength = MicrocodeEntryPoint->TotalSize - (MicrocodeEntryPoint->DataSize + sizeof (CPU_MICROCODE_HEADER));
      if (ExtendedTableLength != 0) {
        //
        // Extended Table exist, check if the CPU in support list
        //
        ExtendedTableHeader = (CPU_MICROCODE_EXTENDED_TABLE_HEADER *) ((UINT8 *) (MicrocodeEntryPoint) + MicrocodeEntryPoint->DataSize + 48);
        //
        // Calulate Extended Checksum
        //
        if ((ExtendedTableLength % 4) == 0) {
          Status = Checksum32Verify ((UINT32 *) ExtendedTableHeader, ExtendedTableLength / sizeof (UINT32));
          if (!EFI_ERROR (Status)) {
            //
            // Checksum correct
            //
            ExtendedTableCount  = ExtendedTableHeader->ExtendedSignatureCount;
            ExtendedTable       = (CPU_MICROCODE_EXTENDED_TABLE *) (ExtendedTableHeader + 1);
            for (ExtendedIndex = 0; ExtendedIndex < ExtendedTableCount; ExtendedIndex++) {
              //
              // Verify Header
              //
              if ((ExtendedTable->ProcessorSignature == Cpuid) && (ExtendedTable->ProcessorFlag & (1 << MsrPlatform))) {
                Status = Checksum32Verify (
                           (UINT32 *) ExtendedTable,
                            sizeof (CPU_MICROCODE_EXTENDED_TABLE) / sizeof (UINT32)
                            );
                if (!EFI_ERROR (Status)) {
                  //
                  // Find one
                  //
                  CorrectMicrocode = TRUE;
                  break;
                }
              }

              ExtendedTable++;
            }
          }
        }
      }
    }
  }

  return CorrectMicrocode;
}


