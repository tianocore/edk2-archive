/** @file
  This driver is responsible for the registration of child drivers
  and the abstraction of the SC SMI sources.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SMM_HELPERS_H_
#define _SC_SMM_HELPERS_H_

#include "ScSmm.h"
#include <Library/ScPlatformLib.h>
#include <Library/SteppingLib.h>

/**
  Initialize bits that aren't necessarily related to an SMI source.

  @retval EFI_SUCCESS             SMI source initialization completed.
  @retval Asserts                 Global Smi Bit is not enabled successfully.

**/
EFI_STATUS
ScSmmInitHardware (
  VOID
  );

/**
  Enables the SC to generate SMIs. Note that no SMIs will be generated
  if no SMI sources are enabled. Conversely, no enabled SMI source will
  generate SMIs if SMIs are not globally enabled. This is the main
  switchbox for SMI generation.

  @retval EFI_SUCCESS             Enable Global Smi Bit completed

**/
EFI_STATUS
ScSmmEnableGlobalSmiBit (
  VOID
  );

/**
  Clears the SMI after all SMI source have been processed.
  Note that this function will not work correctly (as it is
  written) unless all SMI sources have been processed.
  A revision of this function could manually clear all SMI
  status bits to guarantee success.

  @retval EFI_SUCCESS             Clears the SMIs completed
  @retval Asserts                 EOS was not set to a 1

**/
EFI_STATUS
ScSmmClearSmi (
  VOID
  );

/**
  Set the SMI EOS bit after all SMI source have been processed.

  @retval FALSE                   EOS was not set to a 1; this is an error
  @retval TRUE                    EOS was correctly set to a 1

**/
BOOLEAN
ScSmmSetAndCheckEos (
  VOID
  );

/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present

**/
BOOLEAN
ScSmmGetSciEn (
  VOID
  );

/**
  Read a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number

  @retval    TRUE                 The bit is enabled
  @retval    FALSE                The bit is disabled

**/
BOOLEAN
ReadBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc
  );

/**
  Write a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be wrote
  @param[in] WriteClear           If the rest bits of the register is write clear

**/
VOID
WriteBitDesc (
  IN CONST SC_SMM_BIT_DESC        *BitDesc,
  IN CONST BOOLEAN                ValueToWrite,
  IN CONST BOOLEAN                WriteClear
  );

/**
  Specific programming done before exiting SMI

**/
VOID
ScBeforeExitSmi (
  );

#endif

