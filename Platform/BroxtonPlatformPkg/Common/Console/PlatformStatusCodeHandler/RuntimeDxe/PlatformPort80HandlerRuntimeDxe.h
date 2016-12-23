/** @file
  Header file for platform port 80 implementation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PORT_80_HANDLER_RUNTIME_DXE_H__
#define __PORT_80_HANDLER_RUNTIME_DXE_H__

#include <Protocol/ReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PostCodeLib.h>
#include <Library/PlatformPostCodeMapLib.h>
#include <Library/PcdLib.h>
#include <Guid/EventGroup.h>
#include <Library/HobLib.h>

/**
  Convert status code value and write data to port code.

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or software entity.
                               This included information about the class and subclass that is used to
                               classify the entity as well as an operation.
  @param[in]  Instance         The enumeration of a hardware or software entity within
                               the system. Valid instance numbers start with 1.
  @param[in]  CallerId         This optional parameter may be used to identify the caller.
                               This parameter allows the status code driver to apply different rules to
                               different callers.
  @param[in]  Data             This optional parameter may be used to pass additional data.

  @retval     EFI_SUCCESS      Status code reported to port code successfully.

**/
EFI_STATUS
EFIAPI
PostCodeStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  );

#endif

