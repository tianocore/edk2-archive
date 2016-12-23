/** @file
  Implementation file for SeC Policy functionality.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/SeCPolicyLib.h>
#include <Protocol/Heci.h>
#include <Library/PeiServicesLib.h>
#include <SeCAccess.h>

/**
  Check if SeC is enabled.

  @param[in] VOID                 Parameter is VOID

  @retval    EFI_SUCCESS          Command succeeded

**/
EFI_STATUS
SeCPolicyLibInit (
  VOID
  )
{
  return EFI_SUCCESS;
}


/**
  Check if HECI Communication is enabled in setup options.

  @param[in] VOID                    Parameter is VOID

  @retval    FALSE                   HECI is disabled.
  @retval    TRUE                    HECI is enabled.

**/
BOOLEAN
SeCHECIEnabled (
  VOID
  )
{
  return TRUE;
}


/**
  Check if End of Post Message is enabled in setup options.

  @param[in] VOID                    Parameter is VOID

  @retval    FALSE                   EndOfPost is disabled.
  @retval    TRUE                    EndOfPost is enabled.

**/
BOOLEAN
SeCEndOfPostEnabled (
  VOID
  )
{
  return TRUE;
}


/**
  Check if Thermal Reporting Message is enabled in setup options.

  @param[in] VOID                    Parameter is VOID

  @retval    FALSE                   Thermal Reporting is disabled.
  @retval    TRUE                    Thermal Reporting is enabled.

**/
BOOLEAN
SeCTrEnabled (
  VOID
  )
{
  return TRUE;
}


/**
  Show SeC Error message.

  @param[in] MsgId           SeC error message ID.

  @retval    None.
**/
VOID
SeCReportError (
  SEC_ERROR_MSG_ID MsgId
  )
{
  return;
}


/**
  SeC platform Hook function.

  @param[in] VOID              Parameter is VOID

  @retval    None.

**/
VOID
SeCPlatformHook (
  VOID
  )
{
  return;
}


/**
  Check if SeCFwDowngrade is enabled in setup options.

  @param[in] None

  @retval    FALSE                SeCFwDowngrade is disabled.
  @retval    TRUE                 SeCFwDowngrade is enabled.

**/
BOOLEAN
SeCFwDowngradeSupported (
  VOID
  )
{
  return TRUE;
}

