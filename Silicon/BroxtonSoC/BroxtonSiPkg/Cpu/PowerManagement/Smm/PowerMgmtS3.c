/** @file
  This is the SMM driver for saving and restoring the powermanagement related MSRs.

  Copyright (c) 2011 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtSmm.h"

///
/// MSR table for S3 resume
///
STATIC EFI_MSR_VALUES mMsrValues[] = {
  { MSR_IA32_PERF_CTRL,                 0, B_IA32_PERF_CTRLP_STATE_TARGET, TRUE },
  { MSR_PMG_IO_CAPTURE_BASE,            0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PMG_CST_CONFIG,                 0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_MISC_PWR_MGMT,                  0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_IA32_MISC_ENABLE,               0, B_CPUID_POWER_MANAGEMENT_EAX_TURBO | B_MSR_IA32_MISC_DISABLE_TURBO | B_MSR_IA32_MISC_ENABLE_MONITOR | B_MSR_IA32_MISC_ENABLE_TME | B_MSR_IA32_MISC_ENABLE_EIST, TRUE },
  { MSR_POWER_CTL,                      0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PACKAGE_POWER_LIMIT,            0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_PL3_CONTROL,                    0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_IA32_ENERGY_PERFORMANCE_BIAS,   0, 0xFFFFFFFFFFFFFFFF, TRUE },
  { MSR_DDR_RAPL_LIMIT,                 0, 0xFFFFFFFFFFFFFFFF, FALSE },
  { MSR_TURBO_RATIO_LIMIT,              0, 0xFFFFFFFFFFFFFFFF, TRUE }
};

/**
  Save processor MSR runtime settings for S3.

  @retval  EFI_SUCCESS      Processor MSR setting is saved.

**/
EFI_STATUS
S3SaveMsr (
  VOID
  )
{
  UINT32              Index;
  EFI_CPUID_REGISTER  Cpuid06 = { 0, 0, 0, 0 };
  UINT64              MsrValue;

  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    DEBUG ((DEBUG_INFO, " MSR Number: %x\n", mMsrValues[Index].Index));
    if (mMsrValues[Index].Index == MSR_IA32_ENERGY_PERFORMANCE_BIAS) {
      //
      // MSR_IA32_ENERGY_PERFORMANCE_BIAS (1B0h) is accessible only if CPUID(6), ECX[3] = 1 to indicate feature availability.
      //
      AsmCpuid (CPUID_FUNCTION_6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);
      if (!(Cpuid06.RegEcx & B_CPUID_POWER_MANAGEMENT_ECX_ENERGY_EFFICIENT_POLICY_SUPPORT)) {
        mMsrValues[Index].RestoreFlag = FALSE;
        continue;
      }
    }
    //
    // Read Platform Info MSR
    //
    MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);

    //
    // Check PLATFORM_INFO MSR[34:33] > 0 before accessing the MSR_CONFIG_TDP_CONTROL
    //
    if ((mMsrValues[Index].Index == MSR_CONFIG_TDP_CONTROL) &&
        ((RShiftU64 (MsrValue, N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET) & 0x03))
        ) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }

    if (mMsrValues[Index].Index == MSR_TURBO_ACTIVATION_RATIO) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }
    if (mMsrValues[Index].Index == MSR_DDR_RAPL_LIMIT) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }

    //
    // PL3 is supported
    //
    if (mMsrValues[Index].Index == MSR_PL3_CONTROL) {
      mMsrValues[Index].RestoreFlag = TRUE;
    }

    if (mMsrValues[Index].RestoreFlag == TRUE) {
      mMsrValues[Index].Value = AsmReadMsr64 (mMsrValues[Index].Index);
      DEBUG ((DEBUG_INFO, " MSR Number %x read Done \n", mMsrValues[Index].Index));
    }
  }

  return EFI_SUCCESS;
}


/**
  Restore processor MSR runtime settings for S3.

  @param[in]  DispatchHandle     The handle of this callback, obtained when registering
  @param[in]  DispatchContex     Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

**/
VOID
S3RestoreMsr (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext
  )
{
  //
  // Restore MSR's on all logical processors.
  //
  RunOnAllLogicalProcessors (ApSafeRestoreMsr, NULL);
}


/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in, out] Procedure     The function to be run.
  @param[in, out] Buffer        Pointer to a parameter buffer.

  @retval         EFI_SUCCESS   Run the procedure on all logical processors

**/
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  )
{
  UINTN       Index;
  EFI_STATUS  Status;

  //
  // Run the procedure on all logical processors.
  //
  (*Procedure) (Buffer);
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    if (Index != gSmst->CurrentlyExecutingCpu) {
      Status = EFI_NOT_READY;
      while (Status != EFI_SUCCESS) {
        Status = gSmst->SmmStartupThisAp (Procedure, Index, Buffer);
        ASSERT(Status != EFI_INVALID_PARAMETER);
        if (Status != EFI_SUCCESS) {
          //
          // SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
          //
          MicroSecondDelay (PPM_WAIT_PERIOD);
        }
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  This function will restore MSR settings.

  This function must be MP safe.

  @param[in, out]  Buffer        Unused

**/
VOID
EFIAPI
ApSafeRestoreMsr (
  IN OUT VOID *Buffer
  )
{
  UINT32  Index;
  UINT64  MsrValue;

  for (Index = 0; Index < sizeof (mMsrValues) / sizeof (EFI_MSR_VALUES); Index++) {
    //
    // Check RestoreFlag and skip restoring the MSR if it is set to FALSE
    //
    if (mMsrValues[Index].RestoreFlag == FALSE) {
#ifdef EFI_DEBUG
    if (IsBsp ()) {
      DEBUG ((DEBUG_INFO, "Skipping MSR : %x as RestoreFalg is set to FALSE \n", mMsrValues[Index].Index));
    }
#endif
      continue;
    }
    //
    // Check for Lock bits before programming
    //
    MsrValue = AsmReadMsr64 (mMsrValues[Index].Index);
    if ((mMsrValues[Index].Index == MSR_CONFIG_TDP_CONTROL) && (MsrValue & CONFIG_TDP_CONTROL_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_TURBO_ACTIVATION_RATIO) && (MsrValue & MSR_TURBO_ACTIVATION_RATIO_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_PACKAGE_POWER_LIMIT) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_PL3_CONTROL) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    if ((mMsrValues[Index].Index == MSR_DDR_RAPL_LIMIT) && (MsrValue & B_POWER_LIMIT_LOCK)) {
      continue;
    }

    MsrValue = AsmReadMsr64 (mMsrValues[Index].Index);
    MsrValue &= ~mMsrValues[Index].BitMask;
    MsrValue |= (mMsrValues[Index].Value & mMsrValues[Index].BitMask);
    AsmWriteMsr64 (mMsrValues[Index].Index, MsrValue);
  }

  return;
}

