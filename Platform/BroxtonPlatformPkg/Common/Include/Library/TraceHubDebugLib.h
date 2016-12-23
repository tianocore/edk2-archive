/** @file
  Definitions for the TraceHub Debug Lib.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _TRACE_HUB_DEBUG_LIB_H_
#define _TRACE_HUB_DEBUG_LIB_H_

UINTN
EFIAPI
TraceHubDebugPostCode (
  IN UINT16     PostCodeValue
  );

RETURN_STATUS
EFIAPI
TraceHubDebugWrite (
  IN UINTN    ErrorLevel,
  IN UINT8    *Buffer,
  IN UINTN    NumberOfBytes
  );

#endif

