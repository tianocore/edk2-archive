/** @file
  Header file for SeC Policy functionality.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_POLICY_LIB_H_
#define _SEC_POLICY_LIB_H_

#include <Protocol/SeCPlatformPolicy.h>

/**
  Check if SeC is enabled

  @param[in]  None

  @retval     None

**/
EFI_STATUS
SeCPolicyLibInit (
  VOID
  );

/**
  Check if End of Post Message is enabled in setup options.

  @param[in]  None

  @retval     FALSE            EndOfPost is disabled.
  @retval     TRUE             EndOfPost is enabled.

**/
BOOLEAN
SeCHECIEnabled (
  VOID
  );

/**
  Check if End of Post Message is enabled in setup options.

  @param[in]  None

  @retval     FALSE            EndOfPost is disabled.
  @retval     TRUE             EndOfPost is enabled.

**/
BOOLEAN
SeCEndOfPostEnabled (
  VOID
  );

/**
  Check if Thermal Reporting Message is enabled in setup options.

  @param[in]  None

  @retval     FALSE                   Thermal Reporting is disabled.
  @retval     TRUE                    Thermal Reporting is enabled.

**/
BOOLEAN
SeCTrEnabled (
  VOID
  );

/**
  Show SeC Error message.

  @param[in]  MsgId   SeC error message ID.

  @retval     None

**/
VOID
SeCReportError (
  IN SEC_ERROR_MSG_ID MsgId
  );

/**
  Check if SeCFwDowngrade is enabled in setup options.

  @param[in]  None

  @retval     FALSE                   SeCFwDowngrade is disabled.
  @retval     TRUE                    SeCFwDowngrade is enabled.

**/
BOOLEAN
SeCFwDowngradeSupported (
  VOID
  );

/**
  Check if integarted touch is enabled in setup options.

  @param[in] VOID                    Parameter is VOID

  @retval    FALSE                   itouch is disabled.
  @retval    TRUE                    itouch is enabled.

**/
BOOLEAN
SeCITouchEnabled (
  VOID
  );

#endif

