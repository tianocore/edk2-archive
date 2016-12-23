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
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Heci.h>
#include <SeCAccess.h>

//
// Global variables
//
DXE_SEC_POLICY_PROTOCOL  *mDxePlatformSeCPolicy;

/**
  Check if SeC is enabled.

  @param[in]  None

  @retval     None

**/
EFI_STATUS
SeCPolicyLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformSeCPolicyGuid, NULL, (VOID **) &mDxePlatformSeCPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No SEC Platform Policy Protocol available"));
  }

  ASSERT_EFI_ERROR (Status);

  return Status;
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
  BOOLEAN Supported;

  if (mDxePlatformSeCPolicy->SeCConfig.HeciCommunication != 1) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }
  return Supported;
}


/**
  Check if End of Post Message is enabled in setup options.

  @param[in] VOID                 Parameter is VOID

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.

**/
BOOLEAN
SeCEndOfPostEnabled (
  VOID
  )
{
  BOOLEAN Supported;

  if (mDxePlatformSeCPolicy->SeCConfig.EndOfPostEnabled != 1) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }

  return Supported;
}


/**
  Check if Thermal Reporting Message is enabled in setup options.

  @param[in] VOID                    Parameter is VOID.

  @retval    FALSE                   Thermal Reporting is disabled.
  @retval    TRUE                    Thermal Reporting is enabled.

**/
BOOLEAN
SeCTrEnabled (
  VOID
  )
{
  if (mDxePlatformSeCPolicy->SeCConfig.TrConfig->TrEnabled == 1) {
    return TRUE;
  }

  return FALSE;
}


/**
  Show SeC Error message.

  @param[in] MsgId           SeC error message ID.

  @retval    None.

**/
VOID
SeCReportError (
 IN SEC_ERROR_MSG_ID MsgId
  )
{
  if (mDxePlatformSeCPolicy->Revision >= DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_3) {
    mDxePlatformSeCPolicy->SeCReportError (MsgId);
  }

  return ;
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
  if (mDxePlatformSeCPolicy->Revision >= DXE_PLATFORM_SEC_POLICY_PROTOCOL_REVISION_7) {
    if (mDxePlatformSeCPolicy->SeCConfig.SeCFwDownGrade == 1) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Check if integarted touch is enabled in setup options.

  @param[in] VOID                    Parameter is VOID.

  @retval    FALSE                   itouch is disabled.
  @retval    TRUE                    itouch is enabled.

**/
BOOLEAN
SeCITouchEnabled (
  VOID
  )
{
  return mDxePlatformSeCPolicy->SeCConfig.ITouchEnabled;
}

