/** @file
  CPU microcode update library.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Private/Library/MpServiceLib.h>
#include <CpuRegs.h>

/**
  Check if this is non-core processor - HT AP thread

  @retval  TRUE              If this is HT AP thread
  @retval  FALSE             If this is core thread

**/
BOOLEAN
IsSecondaryThread (
  VOID
  )
{
  UINT32              ApicID;
  EFI_CPUID_REGISTER  CpuidRegisters;
  UINT8               CpuCount;
  UINT8               CoreCount;
  UINT8               CpuPerCore;
  UINT32              Mask;

  ApicID = GetCpuApicId ();

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  if ((CpuidRegisters.RegEdx & 0x10000000) == 0) {
    return FALSE;
  }

  CpuCount = (UINT8) ((CpuidRegisters.RegEbx >> 16) & 0xff);
  if (CpuCount == 1) {
    return FALSE;
  }

  AsmCpuid (
    CPUID_SIGNATURE,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  if (CpuidRegisters.RegEax > 3) {

    CoreCount = GetMaxSupportedCoreCount ();
  } else {
    CoreCount = 1;
  }
  //
  // Assumes there is symmetry across core boundary, i.e. each core within a package has the same number of logical processors
  //
  if (CpuCount == CoreCount) {
    return FALSE;
  }

  CpuPerCore = CpuCount / CoreCount;

  //
  // Assume 1 Core  has no more than 8 threads
  //
  if (CpuPerCore == 2) {
    Mask = 0x1;
  } else if (CpuPerCore <= 4) {
    Mask = 0x3;
  } else {
    Mask = 0x7;
  }

  if ((ApicID & Mask) == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}


/**
  Wait until all primary threads are done with the microcode load.

  @param[in]  ExchangeInfo       Pointer to the exchange info buffer for output.

**/
VOID
WaitForPrimaryThreadMcuUpdate (
  IN MP_CPU_EXCHANGE_INFO *ExchangeInfo
  )
{
  UINTN  CoreNumber;

  CoreNumber = (UINTN) ((RShiftU64 (AsmReadMsr64 (MSR_CORE_THREAD_COUNT), 16)) & 0xffff);
  if (IsSecondaryThread ()) {
    while (ExchangeInfo->McuLoadCount < CoreNumber) {
      CpuPause ();
    }
  }
}


/**
  This will load the microcode to the processors.

  @param[in]      MicrocodeEntryPoint   The microcode update pointer
  @param[in, out] Revision              The current (before load this microcode update) microcode revision
                                        as output parameter, the microcode revision after microcode update is loaded

  @retval         EFI_SUCCESS           Microcode loaded
  @retval         EFI_LOAD_ERROR        Microcode not loaded

**/
EFI_STATUS
LoadMicrocode (
  IN CPU_MICROCODE_HEADER *MicrocodeEntryPoint,
  IN OUT UINT32           *Revision
  )
{
  EFI_STATUS  Status;
  UINT32      NewRevision;

  Status = EFI_SUCCESS;

#ifdef EFI_DEBUG
  if (IsBsp()) {
    DEBUG ((DEBUG_INFO, "LoadMicrocode: Before load, revision = 0x%x\n", *Revision));
  }
#endif

  //
  // Load the Processor Microcode
  //
  AsmWriteMsr64 (
    MSR_IA32_BIOS_UPDT_TRIG,
    (UINT64) ((UINTN) MicrocodeEntryPoint + sizeof (CPU_MICROCODE_HEADER))
    );

  NewRevision = GetCpuUcodeRevision ();

#ifdef EFI_DEBUG
  if (IsBsp ()) {
    DEBUG ((DEBUG_INFO, "LoadMicrocode: After load, revision = 0x%x\n", NewRevision));
  }
#endif

  //
  // Verify that the microcode has been loaded
  //
  if (NewRevision == *Revision) {
    return EFI_LOAD_ERROR;
  }
  *Revision = MicrocodeEntryPoint->UpdateRevision;

  return Status;
}


/**
  This will check if the microcode address is valid for this processor, and if so, it will
  load it to the processor.

  @param[in]  ExchangeInfo       Pointer to the exchange info buffer for output.
  @param[in]  MicrocodeAddress   The address of the microcode update binary (in memory).
  @param[out] FailedRevision     The microcode revision that fails to be loaded.

  @retval     EFI_SUCCESS        A new microcode update is loaded.
  @retval     Other              Due to some reason, no new microcode update is loaded.

**/
EFI_STATUS
InitializeMicrocode (
  IN  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  IN  CPU_MICROCODE_HEADER *MicrocodeAddress,
  OUT UINT32               *FailedRevision
  )
{
  EFI_STATUS          Status;
  EFI_CPUID_REGISTER  Cpuid;
  UINT32              UcodeRevision;

  Status = EFI_NOT_FOUND;


  AsmCpuid (
    CPUID_VERSION_INFO,
    &Cpuid.RegEax,
    &Cpuid.RegEbx,
    &Cpuid.RegEcx,
    &Cpuid.RegEdx
    );

  WaitForPrimaryThreadMcuUpdate (ExchangeInfo);
  UcodeRevision = GetCpuUcodeRevision ();

  if (CheckMicrocode (Cpuid.RegEax, MicrocodeAddress, &UcodeRevision)) {
    Status = LoadMicrocode (MicrocodeAddress, &UcodeRevision);
    *FailedRevision = UcodeRevision;
  }
  InterlockedIncrement (&(ExchangeInfo->McuLoadCount));

  return Status;
}

