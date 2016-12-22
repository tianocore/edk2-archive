/** @file
  Header file of CPU feature control module.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _FEATURES_H_
#define _FEATURES_H_

#include <Private/Library/MpServiceLib.h>

///
/// Processor feature definitions.
///
#define TXT_SUPPORT        1
#define VMX_SUPPORT        (1 << 1)
#define XD_SUPPORT         (1 << 2)
#define DCA_SUPPORT        (1 << 3)
#define X2APIC_SUPPORT     (1 << 4)
#define AES_SUPPORT        (1 << 5)
#define HT_SUPPORT         (1 << 6)
#define DEBUG_SUPPORT      (1 << 7)
#define DEBUG_LOCK_SUPPORT (1 << 8)
#define PROC_TRACE_SUPPORT (1 << 9)

#define OPTION_FEATURE_RESERVED_MASK        0xFFFB00F8  ///< bits 30:16, 18, 7:3
#define OPTION_FEATURE_CONFIG_RESERVED_MASK 0xFFFFFFFC  ///< bits 2:31

#define MAX_TOPA_ENTRY_COUNT 2

typedef struct {
  UINT64   TopaEntry[MAX_TOPA_ENTRY_COUNT];
} PROC_TRACE_TOPA_TABLE;

/**
  Create feature control structure which will be used to program each feature on each core.

**/
VOID
InitializeFeaturePerSetup (
  VOID
  );

/**
  Program all processor features basing on desired settings

**/
VOID
EFIAPI
ProgramProcessorFeature (
  VOID
  );

/**
  Program CPUID Limit before booting to OS

**/
VOID
EFIAPI
ProgramCpuidLimit (
   VOID
  );

/**
  Initialize prefetcher settings

  @param[in] MlcStreamerprefecterEnabled       Enable/Disable MLC streamer prefetcher
  @param[in] MlcSpatialPrefetcherEnabled       Enable/Disable MLC spatial prefetcher

**/
VOID
InitializeProcessorsPrefetcher (
  IN UINTN MlcStreamerprefecterEnabled,
  IN UINTN MlcSpatialPrefetcherEnabled
  );

/**
  Detect each processor feature and log all supported features

**/
VOID
EFIAPI
CollectProcessorFeature (
  VOID
  );

/**
  Lock VMX/TXT feature bits on the processor.
  Set "CFG Lock" (MSR 0E2h Bit[15]

**/
VOID
LockFeatureBit (
  VOID
  );


/**
  Provide access to the CPU misc enables MSR

  @param[in] Enable     Enable or Disable Misc Features
  @param[in] BitMask    The register bit offset of MSR MSR_IA32_MISC_ENABLE

**/
VOID
CpuMiscEnable (
  BOOLEAN Enable,
  UINT64  BitMask
  );
#endif

