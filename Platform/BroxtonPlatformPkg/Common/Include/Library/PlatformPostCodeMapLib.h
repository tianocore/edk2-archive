/** @file
  This library class provides Platform PostCode Map.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PLATFORM_POST_CODE_MAP_LIB__
#define __PLATFORM_POST_CODE_MAP_LIB__

/**
  Get PostCode from status code type and value.

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or
                               software entity. This includes information about the class and
                               subclass that is used to classify the entity as well as an operation.
                               For progress codes, the operation is the current activity.
                               For error codes, it is the exception.For debug codes,it is not defined at this time.

  @return     PostCode

**/
UINT32
EFIAPI
GetPostCodeFromStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  );

#endif

