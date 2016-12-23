/** @file
  Performance library instance used in PEI phase.

  This file implements Performance Library class in MdePkg. It converts performance data
  to PEI GUID event, and creates performance logging GUIDed HOB on the first performance
  logging and then logs the performance data to the GUIDed HOB. Due to the limitation
  of temporary RAM, the maximum number of performance logging entry is specified by PcdMaxPeiPerformanceLogEntries.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <PeiFirmwarePerformance.h>
#include <Library/PerformanceLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/LocalApicLib.h>
#include <ExtendedFirmwarePerformanceData.h>


/**
  Convert PEI performance log to FPDT GUID boot record.

  @param[in]  IsStart                    TRUE if the performance log is start log.
  @param[in]  Handle                     Pointer to environment specific context used
                                         to identify the component being measured.
  @param[in]  Token                      Pointer to a Null-terminated ASCII string
                                         that identifies the component being measured.
  @param[in]  Module                     Pointer to a Null-terminated ASCII string
                                         that identifies the module being measured.
  @param[in]  Ticker                     64-bit time stamp.
  @param[in]  Identifier                 32-bit identifier. If the value is 0, the created record
                                         is same as the one created by StartGauge of PERFORMANCE_PROTOCOL.

  @retval     RETURN_SUCCESS             Add FPDT boot record.
  @retval     RETURN_OUT_OF_RESOURCES    There are not enough resources to record the measurement.
  @retval     RETURN_UNSUPPORTED         No matched FPDT record.

**/
RETURN_STATUS
InsertPeiFpdtMeasurement (
  IN BOOLEAN      IsStart,
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       Ticker,
  IN UINT32       Identifier
  )
{
  EFI_HOB_GUID_TYPE               *GuidHob;
  UINTN                           PeiPerformanceSize;
  PEI_FIRMWARE_PERFORMANCE_HOB    *PeiFirmwarePerformance;
  PEI_GUID_EVENT_RECORD           *PeiGuidEvent;
  UINT16                          ProgressId;

  //
  // Create GUID HOB Data.
  //
  GuidHob = GetFirstGuidHob (&gPeiFirmwarePerformanceGuid);
  if (GuidHob != NULL) {
    //
    // PEI Performance HOB was found, then return the existing one.
    //
    PeiFirmwarePerformance = GET_GUID_HOB_DATA (GuidHob);
  } else {
    //
    // PEI Performance HOB was not found, then build one.
    //
    PeiPerformanceSize      = sizeof (PEI_FIRMWARE_PERFORMANCE_HOB) +
                              sizeof (PEI_GUID_EVENT_RECORD) * (PcdGet8 (PcdMaxPeiPerformanceLogEntries) - 1);
    PeiFirmwarePerformance = BuildGuidHob (&gPeiFirmwarePerformanceGuid, PeiPerformanceSize);
    ASSERT (PeiFirmwarePerformance != NULL);
    ZeroMem (PeiFirmwarePerformance, PeiPerformanceSize);
  }

  //
  // Check whether GUID Data is enough to store new PEI GUID Event Perf Data.
  //
  if (PeiFirmwarePerformance->NumberOfEntries >= PcdGet8 (PcdMaxPeiPerformanceLogEntries)) {
    return RETURN_OUT_OF_RESOURCES;
  }
  PeiGuidEvent = &PeiFirmwarePerformance->GuidEventRecord[PeiFirmwarePerformance->NumberOfEntries];

  //
  // Covert Performance token to ProgressId.
  //
  if (Identifier != 0) {
    ProgressId = (UINT16) Identifier;
  } else if (Token == NULL) {
    return RETURN_UNSUPPORTED;
  } else if (AsciiStrCmp (Token, PEIM_TOK) == 0) {
    if (IsStart) {
      ProgressId = MODULE_START_ID;
    } else {
      ProgressId = MODULE_END_ID;
    }
  } else if (AsciiStrCmp (Token, PREMEM_TOK) == 0) {
    if (IsStart) {
      ProgressId = PREMEM_START_ID;
    } else {
      ProgressId = PREMEM_END_ID;
    }
  } else if (AsciiStrCmp (Token, POSTMEM_TOK) == 0) {
    if (IsStart) {
      ProgressId = POSTMEM_START_ID;
    } else {
      ProgressId = POSTMEM_END_ID;
    }
  } else if (AsciiStrCmp (Token, DISMEM_TOK) == 0) {
    if (IsStart) {
      ProgressId = DISMEM_START_ID;
    } else {
      ProgressId = DISMEM_END_ID;
    }
  } else if (AsciiStrCmp (Token, SCRIPT_EXEC_TOK) == 0) {
    if (IsStart) {
      ProgressId = SCRIPT_EXEC_START_ID;
    } else {
      ProgressId = SCRIPT_EXEC_END_ID;
    }
  } else {
    return RETURN_UNSUPPORTED;
  }

  //
  // Fill PEI GUID Event Data.
  //
  if (Handle != NULL) {
    CopyGuid (&PeiGuidEvent->Guid, Handle);
  } else {
    CopyGuid (&PeiGuidEvent->Guid, &gEfiCallerIdGuid);
  }
  PeiGuidEvent->ProgressID = ProgressId;
  PeiGuidEvent->ApicID     = GetApicId ();
  //
  // Get ticker value.
  //
  if (Ticker == 0) {
    Ticker = GetPerformanceCounter ();
  }
  PeiGuidEvent->Timestamp  = GetTimeInNanoSecond (Ticker);
  PeiFirmwarePerformance->NumberOfEntries ++;

  return RETURN_SUCCESS;
}


/**
  Creates a record for the beginning of a performance measurement.

  Creates a record that contains the Handle, Token, Module and Identifier.
  If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
  If TimeStamp is zero, then this function reads the current time stamp
  and adds that time stamp value to the record as the start time.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartPerformanceMeasurement.

  @retval     RETURN_SUCCESS          The start of the measurement was recorded.
  @retval     RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return InsertPeiFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Fills in the end time of a performance measurement.

  Looks up the record that matches Handle, Token, Module and Identifier.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then TimeStamp is added to the record as the end time.
  If the record is found and TimeStamp is zero, then this function reads
  the current time stamp and adds that time stamp value to the record as the end time.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the found record
                                      is same as the one found by EndPerformanceMeasurement.

  @retval     RETURN_SUCCESS          The end of  the measurement was recorded.
  @retval     RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return InsertPeiFpdtMeasurement (FALSE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurement and EndPerformanceMeasurement,
  and then assign the Identifier with 0.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, EndTimeStamp and Identifier.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().
  If Identifier is NULL, then ASSERT().

  @param[in]  LogEntryKey              On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance of entry entry.
  @param[out]  Handle                  Pointer to environment specific context used to identify the component
                                       being measured.
  @param[out]  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                       being measured.
  @param[out]  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                       being measured.
  @param[out]  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was started.
  @param[out]  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was ended.
  @param[out]  Identifier              Pointer to the 32-bit identifier that was recorded.

  @retval      The key for the next performance log entry (in general case).

**/
UINTN
EFIAPI
GetPerformanceMeasurementEx (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp,
  OUT UINT32      *Identifier
  )
{
  return 0;
}


/**
  Creates a record for the beginning of a performance measurement.

  Creates a record that contains the Handle, Token, and Module.
  If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
  If TimeStamp is zero, then this function reads the current time stamp
  and adds that time stamp value to the record as the start time.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.

  @retval     RETURN_SUCCESS          The start of the measurement was recorded.
  @retval     RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return InsertPeiFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, 0);
}


/**
  Fills in the end time of a performance measurement.

  Looks up the record that matches Handle, Token, and Module.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then TimeStamp is added to the record as the end time.
  If the record is found and TimeStamp is zero, then this function reads
  the current time stamp and adds that time stamp value to the record as the end time.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.

  @retval     RETURN_SUCCESS          The end of  the measurement was recorded.
  @retval     RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return InsertPeiFpdtMeasurement (FALSE, Handle, Token, Module, TimeStamp, 0);
}


/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurementEx and EndPerformanceMeasurementEx,
  and then eliminate the Identifier.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, and EndTimeStamp.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().

  @param[in]  LogEntryKey              On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance of entry entry.
  @param[out]  Handle                  Pointer to environment specific context used to identify the component
                                       being measured.
  @param[out]  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                       being measured.
  @param[out]  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                       being measured.
  @param[out]  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was started.
  @param[out]  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was ended.

  @retval                              The key for the next performance log entry (in general case).

**/
UINTN
EFIAPI
GetPerformanceMeasurement (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp
  )
{
  return 0;
}


/**
  Returns TRUE if the performance measurement macros are enabled.

  This function returns TRUE if the PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
  PcdPerformanceLibraryPropertyMask is set.  Otherwise FALSE is returned.

  @retval TRUE                    The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is set.
  @retval FALSE                   The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is clear.

**/
BOOLEAN
EFIAPI
PerformanceMeasurementEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8 (PcdPerformanceLibraryPropertyMask) & PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED) != 0);
}

