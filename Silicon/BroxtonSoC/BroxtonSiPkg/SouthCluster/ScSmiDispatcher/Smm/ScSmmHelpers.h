/** @file
  Helper functions for SC SMM

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef SC_SMM_HELPERS_H
#define SC_SMM_HELPERS_H

#include "ScSmm.h"
#include "ScxSmmHelpers.h"

//
// SUPPORT / HELPER FUNCTIONS (SC version-independent)
//
/**
  Publish SMI Dispatch protocols.

**/
VOID
ScSmmPublishDispatchProtocols (
  VOID
  );

/**
  Compare 2 SMM source descriptors' enable settings.

  @param[in] Src1                 Pointer to the SC SMI source description table 1
  @param[in] Src2                 Pointer to the SC SMI source description table 2

  @retval    TRUE                 The enable settings of the 2 SMM source descriptors are identical.
  @retval    FALSE                The enable settings of the 2 SMM source descriptors are not identical.

**/
BOOLEAN
CompareEnables (
  CONST IN SC_SMM_SOURCE_DESC    *Src1,
  CONST IN SC_SMM_SOURCE_DESC    *Src2
  );

/**
  Compare 2 SMM source descriptors' statuses.

  @param[in] Src1                 Pointer to the SC SMI source description table 1
  @param[in] Src2                 Pointer to the SC SMI source description table 2

  @retval    TRUE                 The statuses of the 2 SMM source descriptors are identical.
  @retval    FALSE                The statuses of the 2 SMM source descriptors are not identical.

**/
BOOLEAN
CompareStatuses (
  IN CONST SC_SMM_SOURCE_DESC     *Src1,
  IN CONST SC_SMM_SOURCE_DESC     *Src2
  );

/**
  Compare 2 SMM source descriptors, based on Enable settings and Status settings of them.

  @param[in] Src1                    Pointer to the SC SMI source description table 1
  @param[in] Src2                    Pointer to the SC SMI source description table 2

  @retval    TRUE                    The 2 SMM source descriptors are identical.
  @retval    FALSE                   The 2 SMM source descriptors are not identical.

**/
BOOLEAN
CompareSources (
  IN CONST SC_SMM_SOURCE_DESC *Src1,
  IN CONST SC_SMM_SOURCE_DESC *Src2
  );

/**
  Check if an SMM source is active.

  @param[in] Src                     Pointer to the SC SMI source description table

  @retval    TRUE                    It is active.
  @retval    FALSE                   It is inactive.

**/
BOOLEAN
SourceIsActive (
  IN CONST  SC_SMM_SOURCE_DESC *Src
  );

/**
  Enable the SMI source event by set the SMI enable bit, this function would also clear SMI
  status bit to make initial state is correct

  @param[in] SrcDesc              Pointer to the SC SMI source description table

**/
VOID
ScSmmEnableSource (
  IN CONST SC_SMM_SOURCE_DESC *SrcDesc
  );

/**
  Disable the SMI source event by clear the SMI enable bit

  @param[in] SrcDesc              Pointer to the SC SMI source description table

**/
VOID
ScSmmDisableSource (
  IN CONST SC_SMM_SOURCE_DESC     *SrcDesc
  );

/**
  Clear the SMI status bit by set the source bit of SMI status register

  @param[in] SrcDesc              Pointer to the SC SMI source description table

**/
VOID
ScSmmClearSource (
  IN CONST SC_SMM_SOURCE_DESC *SrcDesc
  );

/**
  Sets the source to a 1 and then waits for it to clear.
  Be very careful when calling this function -- it will not
  ASSERT.  An acceptable case to call the function is when
  waiting for the NEWCENTURY_STS bit to clear (which takes
  3 RTCCLKs).

  @param[in] SrcDesc              Pointer to the SC SMI source description table

**/
VOID
ScSmmClearSourceAndBlock (
  IN CONST SC_SMM_SOURCE_DESC    *SrcDesc
  );

#endif

