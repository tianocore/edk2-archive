/** @file
  Platform PEI Debug Driver.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Ppi/DebugService.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/ReportStatusCodeLib.h>

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
VOID
EFIAPI
PeiDebugPrint (
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  IN  VA_LIST      VaListMarker
  )
{
  UINT64          Buffer[(MAX_DEBUG_MESSAGE_LENGTH / sizeof (UINT64)) + 1];
  EFI_DEBUG_INFO  *DebugInfo;
  UINTN           TotalSize;
  BASE_LIST       BaseListMarker;
  CHAR8           *FormatString;
  BOOLEAN         Long;

  //
  // Compute the total size of the record.
  // Note that the passing-in format string and variable parameters will be constructed to
  // the following layout:
  //
  //         Buffer->|------------------------|
  //                 |         Padding        | 4 bytes
  //      DebugInfo->|------------------------|
  //                 |      EFI_DEBUG_INFO    | sizeof(EFI_DEBUG_INFO)
  // BaseListMarker->|------------------------|
  //                 |           ...          |
  //                 |   variable arguments   | 12 * sizeof (UINT64)
  //                 |           ...          |
  //                 |------------------------|
  //                 |       Format String    |
  //                 |------------------------|<- (UINT8 *)Buffer + sizeof(Buffer)
  //
  TotalSize = 4 + sizeof (EFI_DEBUG_INFO) + 12 * sizeof (UINT64) + AsciiStrSize (Format);

  //
  // If the TotalSize is larger than the maximum record size, then return
  //
  if (TotalSize > sizeof (Buffer)) {
    return;
  }

  //
  // Fill in EFI_DEBUG_INFO
  //
  // Here we skip the first 4 bytes of Buffer, because we must ensure BaseListMarker is
  // 64-bit aligned, otherwise retrieving 64-bit parameter from BaseListMarker will cause
  // exception on IPF. Buffer starts at 64-bit aligned address, so skipping 4 types (sizeof(EFI_DEBUG_INFO))
  // just makes address of BaseListMarker, which follows DebugInfo, 64-bit aligned.
  //
  DebugInfo             = (EFI_DEBUG_INFO *) (Buffer) + 1;
  DebugInfo->ErrorLevel = (UINT32) ErrorLevel;
  BaseListMarker        = (BASE_LIST) (DebugInfo + 1);
  FormatString          = (CHAR8 *) ((UINT64 *) (DebugInfo + 1) + 12);

  //
  // Copy the Format string into the record
  //
  AsciiStrCpyS (FormatString, sizeof (Buffer) - 4 - (sizeof (EFI_DEBUG_INFO) + 12 * sizeof (UINT64)), Format);

  //
  // The first 12 * sizeof (UINT64) bytes following EFI_DEBUG_INFO are for variable arguments
  // of format in DEBUG string, which is followed by the DEBUG format string.
  // Here we will process the variable arguments and pack them in this area.
  //
  for (; *Format != '\0'; Format++) {
    //
    // Only format with prefix % is processed.
    //
    if (*Format != '%') {
      continue;
    }
    Long = FALSE;
    //
    // Parse Flags and Width
    //
    for (Format++; TRUE; Format++) {
      if (*Format == '.' || *Format == '-' || *Format == '+' || *Format == ' ') {
        //
        // These characters in format field are omitted.
        //
        continue;
      }
      if (*Format >= '0' && *Format <= '9') {
        //
        // These characters in format field are omitted.
        //
        continue;
      }
      if (*Format == 'L' || *Format == 'l') {
        //
        // 'L" or "l" in format field means the number being printed is a UINT64
        //
        Long = TRUE;
        continue;
      }
      if (*Format == '*') {
        //
        // '*' in format field means the precision of the field is specified by
        // a UINTN argument in the argument list.
        //
        BASE_ARG (BaseListMarker, UINTN) = VA_ARG (VaListMarker, UINTN);
        continue;
      }
      if (*Format == '\0') {
        //
        // Make no output if Format string terminates unexpectedly when
        // looking up for flag, width, precision and type.
        //
        Format--;
      }
      //
      // When valid argument type detected or format string terminates unexpectedly,
      // the inner loop is done.
      //
      break;
    }

    //
    // Pack variable arguments into the storage area following EFI_DEBUG_INFO.
    //
    if ((*Format == 'p') && (sizeof (VOID *) > 4)) {
      Long = TRUE;
    }
    if (*Format == 'p' || *Format == 'X' || *Format == 'x' || *Format == 'd') {
      if (Long) {
        BASE_ARG (BaseListMarker, INT64) = VA_ARG (VaListMarker, INT64);
      } else {
        BASE_ARG (BaseListMarker, int) = VA_ARG (VaListMarker, int);
      }
    } else if (*Format == 's' || *Format == 'S' || *Format == 'a' || *Format == 'g' || *Format == 't') {
      BASE_ARG (BaseListMarker, VOID *) = VA_ARG (VaListMarker, VOID *);
    } else if (*Format == 'c') {
      BASE_ARG (BaseListMarker, UINTN) = VA_ARG (VaListMarker, UINTN);
    } else if (*Format == 'r') {
      BASE_ARG (BaseListMarker, RETURN_STATUS) = VA_ARG (VaListMarker, RETURN_STATUS);
    }

    //
    // If the converted BASE_LIST is larger than the 12 * sizeof (UINT64) allocated bytes, then ASSERT()
    // This indicates that the DEBUG() macro is passing in more argument than can be handled by
    // the EFI_DEBUG_INFO record
    //
    ASSERT ((CHAR8 *) BaseListMarker <= FormatString);

    //
    // If the converted BASE_LIST is larger than the 12 * sizeof (UINT64) allocated bytes, then return
    //
    if ((CHAR8 *) BaseListMarker > FormatString) {
      VA_END (VaListMarker);
      return;
    }
  }

  //
  // Send the DebugInfo record
  //
  REPORT_STATUS_CODE_EX (
    EFI_DEBUG_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_DC_UNSPECIFIED),
    0,
    NULL,
    &gEfiStatusCodeDataTypeDebugGuid,
    DebugInfo,
    TotalSize
    );
}


/**
  Prints an assert message containing a file name, line number, and description.
  This may be followed by a breakpoint or a dead loop.

  Print a message of the form "ASSERT <FileName>(<LineNumber>): <Description>\n"
  to the debug output device.  If DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED bit of
  PcdDebugProperyMask is set then CpuBreakpoint() is called. Otherwise, if
  DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED bit of PcdDebugProperyMask is set then
  CpuDeadLoop() is called.  If neither of these bits are set, then this function
  returns immediately after the message is printed to the debug output device.
  DebugAssert() must actively prevent recursion.  If DebugAssert() is called while
  processing another DebugAssert(), then DebugAssert() must return immediately.

  If FileName is NULL, then a <FileName> string of "(NULL) File name" is printed.
  If Description is NULL, then a <Description> string of "(NULL) Description" is printed.

  @param[in]  FileName           Pointer to the name of the source file that generated the assert condition.
  @param[in]  LineNumber         The line number in the source file that generated the assert condition
  @param[in]  Description        Pointer to the description of the assert condition.
  @param[in]  DebugPropertyMask  PropertyMask to the description of the assert.

**/
VOID
EFIAPI
PeiDebugAssert (
  IN CONST CHAR8  *FileName,
  IN UINTN        LineNumber,
  IN CONST CHAR8  *Description,
  IN UINT8        DebugPropertyMask
  )
{
  UINT64                 Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE / sizeof(UINT64)];
  EFI_DEBUG_ASSERT_DATA  *AssertData;
  UINTN                  HeaderSize;
  UINTN                  TotalSize;
  CHAR8                  *Temp;
  UINTN                  FileNameSize;
  UINTN                  DescriptionSize;

  //
  // Get string size
  //
  HeaderSize       = sizeof (EFI_DEBUG_ASSERT_DATA);
  FileNameSize     = AsciiStrSize (FileName);
  DescriptionSize  = AsciiStrSize (Description);

  //
  // Make sure it will all fit in the passed in buffer.
  //
  if (HeaderSize + FileNameSize + DescriptionSize > sizeof (Buffer)) {
    //
    // FileName + Description is too long to be filled into buffer.
    //
    if (HeaderSize + FileNameSize < sizeof (Buffer)) {
      //
      // Description has enough buffer to be truncated.
      //
      DescriptionSize = sizeof (Buffer) - HeaderSize - FileNameSize;
    } else {
      //
      // FileName is too long to be filled into buffer.
      // FileName will be truncated. Reserved one byte for Description NULL terminator.
      //
      DescriptionSize = 1;
      FileNameSize    = sizeof (Buffer) - HeaderSize - DescriptionSize;
    }
  }

  //
  // Fill in EFI_DEBUG_ASSERT_DATA
  //
  AssertData = (EFI_DEBUG_ASSERT_DATA *) Buffer;
  AssertData->LineNumber = (UINT32) LineNumber;
  TotalSize  = sizeof (EFI_DEBUG_ASSERT_DATA);

  //
  // Copy Ascii FileName including NULL terminator.
  //
  Temp = CopyMem (AssertData + 1, FileName, FileNameSize);
  Temp[FileNameSize - 1] = 0;
  TotalSize += FileNameSize;

  //
  // Copy Ascii Description include NULL terminator.
  //
  Temp = CopyMem (Temp + FileNameSize, Description, DescriptionSize);
  Temp[DescriptionSize - 1] = 0;
  TotalSize += DescriptionSize;

  REPORT_STATUS_CODE_EX (
    (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_EC_ILLEGAL_SOFTWARE_STATE),
    0,
    NULL,
    NULL,
    AssertData,
    TotalSize
    );

  //
  // Generate a Breakpoint, DeadLoop, or NOP based on PCD settings
  //
  if ((DebugPropertyMask & DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED) != 0) {
    CpuBreakpoint ();
  } else if ((DebugPropertyMask & DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED) != 0) {
    CpuDeadLoop ();
  }
}

CHAR8                          mGdtTable[0x40];

DEBUG_SERVICE_PPI mDebugServicePpi = {
  PeiDebugPrint,
  PeiDebugAssert
};

EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gDebugServicePpiGuid,
    (VOID *) &mDebugServicePpi
  }
};


/**
  Main entry

  @param[in]  FileHandle           Handle of the file being invoked.
  @param[in]  PeiServices          Pointer to PEI Services table.

  @retval     EFI_SUCCESS          Install function successfully.

**/
EFI_STATUS
EFIAPI
PeiDebugInstanceEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_PEI_PPI_DESCRIPTOR      *PeiPpiDescriptor;
  EFI_STATUS                  Status= EFI_SUCCESS;
  BOOLEAN                     ImageInMemory = FALSE;

  Status = (*PeiServices)->RegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    ImageInMemory = TRUE;
  } else if (Status == EFI_NOT_FOUND) {
    ASSERT_EFI_ERROR (Status);
  }

  if (!ImageInMemory) {
    //
    // Register for a callback once memory has been initialized.
    //
    Status = PeiServicesInstallPpi (&mPpiList[0]);
    ASSERT_EFI_ERROR (Status);
  } else {

    //
    // locate the SEC platform information PPI
    //
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gDebugServicePpiGuid,
                               0,
                               &PeiPpiDescriptor,
                               NULL
                               );
    if (Status == EFI_SUCCESS) {
      //
      // Reinstall the StatusCode PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 PeiPpiDescriptor,
                                 &mPpiList[0]
                                 );
    }
  }

  return EFI_SUCCESS;
}

