/** @file
  CPU Common Lib implementation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>
#include <Library/PostCodeLib.h>
#include <CpuAccess.h>
#include <Private/Library/CpuCommonLib.h>

#define INTERRUPT_VECTOR_NUMBER 256
#define END_OF_TABLE            0xFF

///
/// Table to convert PL1 / Pl2 / RATL Seconds into equivalent MSR values
/// This table is used for TDP Time Window programming
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSecondsToMsrValueMapTable[][2] = {
  ///
  ///  Seconds,    MSR Value
  ///
  {        1,         0x0A     },
  {        2,         0x0B     },
  {        3,         0x4B     },
  {        4,         0x0C     },
  {        5,         0x2C     },
  {        6,         0x4C     },
  {        7,         0x6C     },
  {        8,         0x0D     },
  {       10,         0x2D     },
  {       12,         0x4D     },
  {       14,         0x6D     },
  {       16,         0x0E     },
  {       20,         0x2E     },
  {       24,         0x4E     },
  {       28,         0x6E     },
  {       32,         0x0F     },
  {       40,         0x2F     },
  {       48,         0x4F     },
  {       56,         0x6F     },
  {       64,         0x10     },
  {       80,         0x30     },
  {       96,         0x50     },
  {      112,         0x70     },
  {      128,         0x11     },
  { END_OF_TABLE, END_OF_TABLE }
};

///
/// Table to convert PL3 Milli Seconds into equivalent MSR values
/// This table is used for TDP Time Window programming
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mMilliSecondsToMsrValueMapTable[][2] = {
  ///
  ///  MilliSeconds,  MSR Value
  ///
  {        3,         0x41     },
  {        4,         0x02     },
  {        5,         0x22     },
  {        6,         0x42     },
  {        7,         0x62     },
  {        8,         0x03     },
  {       10,         0x23     },
  {       12,         0x43     },
  {       14,         0x63     },
  {       16,         0x04     },
  {       20,         0x24     },
  {       24,         0x44     },
  {       28,         0x64     },
  {       32,         0x05     },
  {       40,         0x25     },
  {       48,         0x45     },
  {       56,         0x65     },
  {       64,         0x06     },
  { END_OF_TABLE, END_OF_TABLE }
};

/**
  Initialize prefetcher settings.

  @param[in] MlcStreamerprefecterEnabled   Enable/Disable MLC streamer prefetcher
  @param[in] MlcSpatialPrefetcherEnabled   Enable/Disable MLC spatial prefetcher

**/
VOID
ProcessorsPrefetcherInitialization (
  IN UINTN MlcStreamerprefecterEnabled,
  IN UINTN MlcSpatialPrefetcherEnabled
  )
{
  UINT64  MsrValue;

  MsrValue = AsmReadMsr64 (MISC_FEATURE_CONTROL);

  if (MlcStreamerprefecterEnabled == 0) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_STRP;
  }

  if (MlcSpatialPrefetcherEnabled == 0) {
    MsrValue |= B_MISC_FEATURE_CONTROL_MLC_SPAP;
  }

  if ((MsrValue & (B_MISC_FEATURE_CONTROL_MLC_STRP | B_MISC_FEATURE_CONTROL_MLC_SPAP)) != 0) {
    AsmWriteMsr64 (MISC_FEATURE_CONTROL, MsrValue);
  }

  return;
}


/**
  Set up flags in CR4 for XMM instruction enabling.

**/
VOID
EFIAPI
XmmInit (
  VOID
  )
{
  EFI_CPUID_REGISTER  Cpuid;
  UINTN               Cr0;
  UINTN               Cr4;

  //
  // Read the CPUID information
  //
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);

  //
  // Check whether SSE2 is supported
  //
  if (Cpuid.RegEdx & BIT26) {

    //
    // Enable XMM
    //
    Cr0 = AsmReadCr0 ();
    Cr0 |= BIT1;
    AsmWriteCr0 (Cr0);

    Cr4 = AsmReadCr4 ();
    Cr4 |= (BIT9 | BIT10);
    AsmWriteCr4 (Cr4);
  }
}


/**
  Enable "Machine Check Enable" bit in Cr4.

**/
VOID
EFIAPI
EnableMce (
  VOID
  )
{
  UINTN  Cr4;

  //
  // Enable MCE
  //
  Cr4 = AsmReadCr4 ();
  Cr4 |= BIT6;
  AsmWriteCr4 (Cr4);
}


/**
  Mtrr Synch Up Entry.

**/
UINTN
EFIAPI
MpMtrrSynchUpEntry (
  VOID
  )
{
  EFI_CPUID_REGISTER  Cpuid;
  UINT64              MsrData;
  UINTN               Cr0;
  UINTN               Cr4;

  //
  // Read the CPUID and MSR 1Bh information
  //
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  MsrData = AsmReadMsr64 (MSR_IA32_APIC_BASE);

  //
  // Set CD(Bit30) bit and clear NW(Bit29) bit of CR0 followed by a WBINVD.
  //
  if (!(Cpuid.RegEdx & BIT24) || (MsrData & BIT8)) {
    AsmDisableCache ();
  } else {
    //
    // We could bypass the wbinvd by
    // checking MSR 1Bh(MSR_IA32_APIC_BASE) Bit8 (1 = BSP, 0 = AP) to see if we're the BSP?
    // and checking CPUID if the processor support self-snooping.
    //
    Cr0 = AsmReadCr0 ();
    Cr0 &= ~BIT29;
    Cr0 |= BIT30;
    AsmWriteCr0 (Cr0);
  }

  //
  // Clear PGE flag Bit 7
  //
  Cr4 = AsmReadCr4 ();
  Cr4 &= ~BIT7;
  AsmWriteCr4 (Cr4);

  //
  // Flush all TLBs
  //
  CpuFlushTlb ();

  return Cr4;
}


/**
  Mtrr Synch Up Exit

**/
VOID
EFIAPI
MpMtrrSynchUpExit (
  UINTN Cr4
  )
{
  UINTN   Cr0;

  //
  // Flush all TLBs the second time
  //
  CpuFlushTlb ();

  //
  // Clear both the CD and NW bits of CR0.
  //
  Cr0 = AsmReadCr0 ();
  Cr0 &= ~(BIT29 | BIT30);
  AsmWriteCr0 (Cr0);

  //
  // Set PGE Flag in CR4 if set
  //
  AsmWriteCr4 (Cr4);
}


/**
  This procedure sends an IPI to the designated processor in
  the requested delivery mode with the requested vector.

  @param[in] ApicID                  APIC ID of processor.
  @param[in] VectorNumber            Vector number.
  @param[in] DeliveryMode            I/O APIC Interrupt Deliver Modes

  @retval    EFI_INVALID_PARAMETER   Input paramters were not correct.
  @retval    EFI_NOT_READY           There was a pending interrupt
  @retval    EFI_SUCCESS             Interrupt sent successfully

**/
EFI_STATUS
EFIAPI
CpuSendIpi (
  IN UINT32            ApicID,
  IN UINTN             VectorNumber,
  IN UINTN             DeliveryMode
  )
{
  UINT64                ApicBaseReg;
  UINT64                MsrValue;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  UINT32                IcrLow;
  UINT32                IcrHigh;
  BOOLEAN               XapicEnabled;
  UINT32                TriggerMode;

  //
  // Check for valid input parameters.
  //
  if (VectorNumber >= INTERRUPT_VECTOR_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  if (DeliveryMode >= DELIVERY_MODE_MAX) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Fix the vector number for special interrupts like SMI and INIT.
  //
  if (DeliveryMode == DELIVERY_MODE_SMI || DeliveryMode == DELIVERY_MODE_INIT) {
    VectorNumber = 0x0;
  }

  //
  // Initialze ICR high dword, since P6 family processor needs
  // the destination field to be 0x0F when it is a broadcast
  //
  IcrHigh = 0x0f000000;
  IcrLow  = (UINT32) (VectorNumber | (LShiftU64 (DeliveryMode, 8)));

  TriggerMode = TRIGGER_MODE_EDGE;

  //
  // Interrupt trigger mode
  //
  if (TriggerMode == TRIGGER_MODE_LEVEL) {
    IcrLow |= 0x8000;
  }

  //
  // Interrupt pin polarity
  //
  IcrLow |= 0x4000;

  //
  // xAPIC Enabled
  //
  MsrValue = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  if (MsrValue & B_MSR_IA32_APIC_BASE_G_XAPIC) {
    if (MsrValue & B_MSR_IA32_APIC_BASE_M_XAPIC) {
      XapicEnabled = TRUE;
    } else {
      XapicEnabled = FALSE;
    }
  } else {
    XapicEnabled = FALSE;
  }

  if (XapicEnabled) {
    IcrHigh = (UINT32) ApicID;
  } else {
    IcrHigh = (UINT32) LShiftU64 (ApicID, 24);
  }

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000;

  /* If Extended XAPIC Mode is enabled,
     legacy xAPIC is no longer working.
     So, previous MMIO offset must be transferred to MSR offset R/W.
     ----------------------------------------------------------------
     MMIO Offset     MSR Offset     Register Name
     ----------------------------------------------------------------
     300h-310h        830h         Interrupt Command Register [63:0]
                      831h         [Reserved]
     ----------------------------------------------------------------
  */
  //
  // To write APIC register by MSR or MMIO
  //
  if (XapicEnabled) {
    AsmWriteMsr64 (EXT_XAPIC_ICR, (((UINT64) LShiftU64 (IcrHigh, 32)) | (UINT64) IcrLow));
  } else {
    *(volatile UINT32 *) (UINTN) (ApicBase  + APIC_REGISTER_ICR_HIGH_OFFSET) = (UINT32) IcrHigh;
    *(volatile UINT32 *) (UINTN) (ApicBase  + APIC_REGISTER_ICR_LOW_OFFSET) = (UINT32) IcrLow;
  }

  MicroSecondDelay (10);

  //
  // To get APIC register from MSR or MMIO
  //
  if (XapicEnabled) {
    IcrLow = (UINT32) AsmReadMsr64 (EXT_XAPIC_ICR);
  } else {
    IcrLow = (UINT32) *(volatile UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_ICR_LOW_OFFSET);
  }

  if (IcrLow & BIT12) {
    return EFI_NOT_READY;
  }

  MicroSecondDelay (100);

  return EFI_SUCCESS;
}


/**
  Private helper function to convert various Turbo Power Limit Time from Seconds to CPU units

  @param[in] TimeInSeconds       Time in seconds
  @param[in] TimeWindowConvType  Time Window Convert Type

  @retval    UINT8               Converted time in CPU units

**/
UINT8
GetConvertedTime (
  IN UINT32            TimeInSeconds,
  IN TIME_WINDOW_CONV  TimeWindowConvType
  )
{
  UINT8  ConvertedPowerLimitTime;
  UINT8  Index;

  //
  // Convert seconds to MSR value. Since not all values are programmable, we'll select
  // the entry from mapping table which is either equal to the user selected value. OR to a value in the mapping table
  // which is closest (but less than) to the user-selected value.
  //
  ConvertedPowerLimitTime = 0;
  switch (TimeWindowConvType) {
    case PL1TimeWindowConvert:
    case TccOffsetTimeWindowConvert:
      ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[0][1];
      for (Index = 0; mSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
        if (TimeInSeconds == mSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
          break;
        }
        if (TimeInSeconds > mSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
        } else {
          break;
        }
      }
      break;
    case PL3TimeWindowConvert:
      ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[0][1];
      for (Index = 0; mMilliSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
        if (TimeInSeconds == mMilliSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[Index][1];
          break;
        }
        if (TimeInSeconds > mMilliSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[Index][1];
        } else {
          break;
        }
      }
      break;
    default:
      break;
    }

  return ConvertedPowerLimitTime;
}


/**
  Get APIC ID of processor.

  @retval    APIC ID of processor.

**/
UINT32
GetCpuApicId (
  VOID
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  return (UINT32) (CpuidRegisters.RegEbx >> 24);
}


/**
  Programs XAPIC registers.

  @param[in]   Bsp              Is this BSP?

**/
VOID
ProgramXApic (
  BOOLEAN Bsp
  )
{
  UINT64                ApicBaseReg;
  EFI_PHYSICAL_ADDRESS  ApicBase;
  volatile UINT32       *EntryAddress;
  UINT32                EntryValue;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);
  ApicBase    = ApicBaseReg & 0xffffff000ULL;

  //
  // Program the spurious vector entry
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_SPURIOUS_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFFFD0F;
  EntryValue |= 0x10F;
  *EntryAddress = EntryValue;

  //
  // Program the LINT1 vector entry as extINT
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT0_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;

  if (Bsp) {
    EntryValue &= 0xFFFE00FF;
    EntryValue |= 0x700;
  } else {
    EntryValue |= 0x10000;
  }

  *EntryAddress = EntryValue;

  //
  // Program the LINT1 vector entry as NMI
  //
  EntryAddress  = (UINT32 *) (UINTN) (ApicBase + APIC_REGISTER_LINT1_VECTOR_OFFSET);
  EntryValue    = *EntryAddress;
  EntryValue &= 0xFFFE00FF;
  if (Bsp) {
    EntryValue |= 0x400;
  } else {
    EntryValue |= 0x10400;
  }

  *EntryAddress = EntryValue;
}


/**
  This function returns the maximum number of core supported in this physical processor package.

  @retval Maximum number of cores in the package.

**/
UINT8
GetMaxSupportedCoreCount (
  VOID
  )
{
  EFI_CPUID_REGISTER  Cpuid;

  AsmCpuidEx (
    4,
    0,
    &Cpuid.RegEax,
    NULL,
    NULL,
    NULL
    );

  return (UINT8) (RShiftU64 (Cpuid.RegEax, 26) & 0x3f) + 1;
}


/**
  This function returns the actual factory-configured number of threads per core,
  and actual factory-configured number of cores in this physical processor package.

  @param[out] NumberOfEnabledThreadsPerCore    Variable that will store Maximum enabled threads per core.
  @param[out] NumberOfEnabledCoresPerDie       Variable that will store Maximum enabled cores per die.

**/
VOID
GetSupportedCount (
  OUT UINT16 *ThreadsPerCore,  OPTIONAL
  OUT UINT16 *NumberOfCores    OPTIONAL
  )
{
  EFI_CPUID_REGISTER    CpuidRegs;
  UINT16                Threads;

  AsmCpuidEx (CPUID_CORE_TOPOLOGY, 0, NULL, &CpuidRegs.RegEbx, NULL, NULL);
  Threads = (UINT16) CpuidRegs.RegEbx;

  if (ThreadsPerCore != NULL) {
    *ThreadsPerCore = Threads;
  }

  if (NumberOfCores != NULL) {
    AsmCpuidEx (CPUID_CORE_TOPOLOGY, 1, NULL, &CpuidRegs.RegEbx, NULL, NULL);
    *NumberOfCores = (UINT16) (CpuidRegs.RegEbx / Threads);
  }
}


/**
  This function returns the maximum enabled Core per die, maximum enabled threads per core,
  maximum number of dies and packages.

  @param[out] NumberOfEnabledThreadsPerCore    Variable that will store Maximum enabled threads per core.
  @param[out] NumberOfEnabledCoresPerDie       Variable that will store Maximum enabled cores per die.
  @param[out] NumberOfDiesPerPackage           Variable that will store Maximum dies per package.
  @param[out] NumberOfPackages                 Variable that will store Maximum Packages.

**/
VOID
GetEnabledCount (
  OUT UINT16 *NumberOfEnabledThreadsPerCore,  OPTIONAL
  OUT UINT16 *NumberOfEnabledCoresPerDie,     OPTIONAL
  OUT UINT16 *NumberOfDiesPerPackage,         OPTIONAL
  OUT UINT16 *NumberOfPackages                OPTIONAL
  )
{
  UINT64  MsrValue;
  UINT16  NumCores;

  PostCode (0xC81);

  //
  // Read MSR_CORE_THREAD_COUNT (0x35)
  //
  MsrValue = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);

  NumCores = (UINT16) RShiftU64 (MsrValue, N_CORE_COUNT_OFFSET);

  //
  // Get enabled core count in the package (BITS[31:16])
  //
  if (NumberOfEnabledCoresPerDie != NULL) {
    *NumberOfEnabledCoresPerDie = NumCores;
  }

  //
  // Get enabled thread count in the package (BITS[15:0])
  //
  if (NumberOfEnabledThreadsPerCore != NULL) {
    *NumberOfEnabledThreadsPerCore  = (UINT16)DivU64x32((UINT64)(MsrValue & B_THREAD_COUNT_MASK), (UINT32) NumCores);
  }

  //
  // For client, the number of dies and packages will be one
  //
  if (NumberOfDiesPerPackage != NULL) {
    *NumberOfDiesPerPackage  = 1;
  }

  if (NumberOfPackages != NULL) {
    *NumberOfPackages        = 1;
  }
  PostCode (0xC84);
}


/**
  Check to see if the executing thread is BSP.

  @retval  TRUE     Executing thread is BSP
  @retval  FALSE    Executing thread is AP

**/
BOOLEAN
IsBsp (
  VOID
)
{
  BOOLEAN  BspIndicator;

  BspIndicator  = (AsmReadMsr64 (MSR_IA32_APIC_BASE) & B_MSR_IA32_APIC_BASE_BSP) ? TRUE : FALSE;

  return BspIndicator;
}

