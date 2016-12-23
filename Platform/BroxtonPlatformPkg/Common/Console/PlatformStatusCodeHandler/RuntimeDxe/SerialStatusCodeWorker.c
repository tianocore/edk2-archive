/** @file
  Serial I/O status code reporting worker.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformStatusCodeHandlerRuntimeDxe.h"
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/BaseDebugPrintErrorLevelLib.h>


/**
  Convert status code value and extended data to readable ASCII string, send string to serial I/O device.

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

  @retval     EFI_SUCCESS      Status code reported to serial I/O successfully.
  @retval     EFI_DEVICE_ERROR EFI serial device cannot work after ExitBootService() is called.
  @retval     EFI_DEVICE_ERROR EFI serial device cannot work with TPL higher than TPL_CALLBACK.

**/
EFI_STATUS
EFIAPI
SerialStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINTN           CharCount;
  BASE_LIST       Marker;
  UINT32          DebugPrintErrorLevel;

  Buffer[0] = '\0';

  DebugPrintErrorLevel = GetDebugPrintErrorLevel ();

  if (Data != NULL &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
    if ((DEBUG_ERROR & DebugPrintErrorLevel) == 0) {
      return EFI_SUCCESS;
    }
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rDXE_ASSERT!: %a (%d): %a\n\r",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if (Data != NULL &&
             ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
    if ((ErrorLevel & DebugPrintErrorLevel) == 0) {
      return EFI_SUCCESS;
    }
    //
    // Print DEBUG() information into output buffer.
    //
    CharCount = AsciiBSPrint (
                  Buffer,
                  sizeof (Buffer),
                  Format,
                  Marker
                  );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    if ((DEBUG_ERROR& DebugPrintErrorLevel) == 0) {
      return EFI_SUCCESS;
    }
    //
    // Print ERROR information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "ERROR: C%08x:V%08x I%x",
                  CodeType,
                  Value,
                  Instance
                  );
    ASSERT (CharCount > 0);

    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }

    CharCount += AsciiSPrint (
                   &Buffer[CharCount],
                   (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                   "\n\r"
                   );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    //
    // Print PROGRESS information into output buffer.
    //
    // Print in PlatformPort80HandlerRuntimeDxe.efi for synchronizing all output channel
    //
    Buffer[0] = '\0';
    CharCount = 0;
  } else if (Data != NULL &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             ((EFI_STATUS_CODE_STRING_DATA *) Data)->StringType == EfiStringAscii) {
    if ((DEBUG_INFO & DebugPrintErrorLevel) == 0) {
      return EFI_SUCCESS;
    }
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a\n\r",
                  ((EFI_STATUS_CODE_STRING_DATA *) Data)->String.Ascii
                  );
  } else {
    if ((DEBUG_INFO & DebugPrintErrorLevel) == 0) {
      return EFI_SUCCESS;
    }
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "Undefined: C%x:V%x I%x\n\r",
                  CodeType,
                  Value,
                  Instance
                  );
  }

  //
  // Call SerialPort Lib function to do print.
  //
  SerialPortWrite ((UINT8 *) Buffer, CharCount);

  return EFI_SUCCESS;
}

