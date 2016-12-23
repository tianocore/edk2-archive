/** @file
  Beep status code implementation.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformStatusCodeHandlerRuntimeDxe.h"

/**
  Convert status code value to the times of beep.

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or
                               software entity. This includes information about the class and
                               subclass that is used to classify the entity as well as an operation.
                               For progress codes, the operation is the current activity.
                               For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param[in]  Instance         The enumeration of a hardware or software entity within
                               the system. A system may contain multiple entities that match a class/subclass
                               pairing. The instance differentiates between them. An instance of 0 indicates
                               that instance information is unavailable, not meaningful, or not relevant.
                               Valid instance numbers start with 1.
  @param[in]  CallerId         This optional parameter may be used to identify the caller.
                               This parameter allows the status code driver to apply different rules to
                               different callers.
  @param[in]  Data             This optional parameter may be used to pass additional data.

  @retval     EFI_SUCCESS      Status code reported to beep successfully.

**/
EFI_STATUS
EFIAPI
BeepStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  )
{
  return EFI_SUCCESS;
}

