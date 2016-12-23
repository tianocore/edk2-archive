/** @file
  Debug Service PPI.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DEBUG_SERVICE_PPI_H_
#define _DEBUG_SERVICE_PPI_H_
#define MAX_DEBUG_MESSAGE_LENGTH  0x200


/**
  Prints a debug message to the debug output device if the specified error level is enabled.

  If any bit in ErrorLevel is also set in DebugPrintErrorLevelLib function
  GetDebugPrintErrorLevel (), then print the message specified by Format and the
  associated variable argument list to the debug output device.

  If Format is NULL, then ASSERT().

  If the length of the message string specificed by Format is larger than the maximum allowable
  record length, then directly return and not print it.

  @param[in]  ErrorLevel  The error level of the debug message.
  @param[in]  Format      Format string for the debug message to print.
  @param[in]  Marker      Variable argument list whose contents are accessed
                          based on the format string specified by Format.

**/
typedef
VOID
(EFIAPI *DEBUG_PRINT)(
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  IN  VA_LIST       Marker
  );

/**
  Prints an assert message containing a filename, line number, and description.
  This may be followed by a breakpoint or a dead loop.

  Print a message of the form "ASSERT <FileName>(<LineNumber>): <Description>\n"
  to the debug output device.  If DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED bit of
  PcdDebugProperyMask is set then CpuBreakpoint() is called. Otherwise, if
  DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED bit of PcdDebugProperyMask is set then
  CpuDeadLoop() is called.  If neither of these bits are set, then this function
  returns immediately after the message is printed to the debug output device.
  DebugAssert() must actively prevent recursion.  If DebugAssert() is called while
  processing another DebugAssert(), then DebugAssert() must return immediately.

  If FileName is NULL, then a <FileName> string of "(NULL) Filename" is printed.
  If Description is NULL, then a <Description> string of "(NULL) Description" is printed.

  @param[in]  FileName           Pointer to the name of the source file that generated the assert condition.
  @param[in]  LineNumber         The line number in the source file that generated the assert condition
  @param[in]  Description        Pointer to the description of the assert condition.
  @param[in]  DebugPropertyMask  PropertyMask to the description of the assert.

**/
typedef
VOID
(EFIAPI *DEBUG_ASSERT)(
  IN CONST CHAR8  *FileName,
  IN UINTN        LineNumber,
  IN CONST CHAR8  *Description,
  IN UINT8        DebugPropertyMask
  );

typedef struct {
  DEBUG_PRINT            DebugPrint;
  DEBUG_ASSERT           DebugAssert;
} DEBUG_SERVICE_PPI;

extern EFI_GUID gDebugServicePpiGuid;

#endif

