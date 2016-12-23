/** @file
  Performance library instance used by SMM Core.

  This library provides the performance measurement interfaces and initializes performance
  logging for the SMM phase.
  It initializes SMM phase performance logging by publishing the SMM PerformanceEx Protocol,
  which is consumed by SmmPerformanceLib to logging performance data in SMM phase.

  This library is mainly used by SMM Core to start performance logging to ensure that
  SMM PerformanceEx Protocol are installed at the very beginning of SMM phase.

  This library also converts performance log to FPDT record, and report them to boot FPDT table.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiSmm.h>
#include <Guid/Performance.h>
#include <Guid/FirmwarePerformance.h>
#include <Protocol/SmmBase2.h>
#include <Library/DebugLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/SmmServicesTableLib.h>
#include <ExtendedFirmwarePerformanceData.h>

//
// Rmpt Table Data Array to record every module table.
//
typedef struct {
  VOID                              *Handle;
  UINTN                             MaxFunctionNumber;
  RUNTIME_MODULE_PERFORMANCE_TABLE  *RmptTable;
} RMPT_TABLE_DATA;

#define RMPT_TABLE_NUMBER                  40

RMPT_TABLE_DATA          *mRmptDataTable       = NULL;
UINTN                    mRmptTableNumber      = 0;
UINTN                    mMaxRmptTableNumber   = 0;
BOOLEAN                  mIsOutOfResource      = FALSE;
PERFORMANCE_EX_PROTOCOL  *mPerformanceEx       = NULL;

//
// The data structure to hold global performance data.
//
GAUGE_DATA_HEADER       *mGaugeData = NULL;

//
// The current maximum number of logging entries. If current number of
// entries exceeds this value, it will re-allocate a larger array and
// migration the old data to the larger array.
//
UINT32                  mMaxGaugeRecords = 0;
BOOLEAN                 mPerformanceMeasurementEnabled;
SPIN_LOCK               mSmmFpdtLock;

/**
  The function caches the pointers to PerformanceEx Protocol.
  The function locates PerformanceEx protocol from protocol database.

  @retval EFI_SUCCESS     PerformanceEx protocol is successfully located.
  @retval EFI_NOT_FOUND   PerformanceEx protocol is not located to log performance.

**/
EFI_STATUS
GetPerformanceExProtocol (
  VOID
  )
{
  EFI_STATUS                Status;
  PERFORMANCE_EX_PROTOCOL   *PerformanceEx;

  if (mPerformanceEx != NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gPerformanceExProtocolGuid, NULL, (VOID **) &PerformanceEx);
  if (!EFI_ERROR (Status)) {
    ASSERT (PerformanceEx != NULL);
    //
    // Cache PerformanceEx Protocol.
    //
    mPerformanceEx = PerformanceEx;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}


/**
  Convert performance token to FPDT record.

  @param[in]   IsStart                 TRUE if the performance log is start log.
  @param[in]   Token                   Pointer to environment specific context used
                                       to identify the component being measured.
  @param[out]  RecordType              Type of FPDT record
  @param[out]  Identifier              Identifier for FPDT records

  @retval      EFI_SUCCESS             The data was converted correctly.
  @retval      EFI_NOT_FOUND           No matched FPDT record is for the input Token.
  @retval      EFI_INVALID_PARAMETER   Input Pointer is NULL.

**/
EFI_STATUS
ConvertTokenToType (
  IN BOOLEAN                  IsStart,
  IN CONST CHAR8              *Token,
  OUT UINTN                   *FpdtRecType,
  OUT UINT32                  *Identifier
  )
{
  UINTN    RecordType;

  if (Token == NULL || FpdtRecType == NULL || Identifier == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RecordType  = STRING_EVENT_TYPE;

  //
  // Token to Type and Id.
  //
  if (AsciiStrCmp (Token, START_IMAGE_TOK) == 0) {
    if (IsStart) {
      *Identifier = MODULE_START_ID;
    } else {
      *Identifier = MODULE_END_ID;
    }
  } else if (AsciiStrCmp (Token, LOAD_IMAGE_TOK) == 0) {
    if (IsStart) {
      *Identifier = MODULE_LOADIMAGE_START_ID;
    } else {
      *Identifier = MODULE_LOADIMAGE_END_ID;
    }
  } else if (IsStart && AsciiStrCmp (Token, SMM_MODULE_TOK) == 0) {
    RecordType = RUNTIME_MODULE_TABLE_PTR_TYPE;
  } else if (!IsStart && AsciiStrCmp (Token, SMM_FUNCTION_TOK) == 0) {
    RecordType = RUNTIME_FUNCTION_TYPE;
  } else if (AsciiStrCmp (Token, EVENT_REC_TOK) == 0) {
    RecordType = STRING_EVENT_TYPE;
  } else {
    return EFI_NOT_FOUND;
  }

  *FpdtRecType = RecordType;

  return EFI_SUCCESS;
}


/**
  Allocate EfiReservedMemoryType below 4G memory address.
  This function allocates EfiReservedMemoryType below 4G memory address.

  @param[in]  Size    Size of memory to allocate.

  @return     Allocated address for output.

**/
VOID *
FpdtAllocateReservedMemoryBelow4G (
  IN UINTN       Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID                  *Buffer;

  Pages   = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  Pages,
                  &Address
                  );
  ASSERT_EFI_ERROR (Status);

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}


/**
  Add performance log to FPDT boot record table.

  @param[in]  IsStart                 TRUE if the performance log is start log.
  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  StartTicker             64-bit start ticker of start performance log.
  @param[in]  EndTicker               64-bit end ticker of end performance log.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             Add FPDT boot record.
  @retval     EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.
  @retval     EFI_ABORTED             No matched FPDT record.

**/
EFI_STATUS
InsertFpdtMeasurement (
  IN BOOLEAN      IsStart,
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       StartTicker,
  IN UINT64       EndTicker,
  IN UINT32       Identifier
  )
{
  UINTN                             FpdtRecType;
  EFI_STATUS                        Status;
  UINT64                            Ticker;
  UINT64                            Residency;
  UINTN                             Index;
  VOID                              *DataBuffer;
  UINTN                             RmptTableLength;
  FPDT_RECORD                       FpdtRecord;
  RUNTIME_MODULE_PERFORMANCE_TABLE  *RmptTable;
  RUNTIME_FUNCTION_PERF_RECORD      *RuntimeFunctionPerfRecord;

  //
  // Convert performance log to FPDT record.
  //
  Status = ConvertTokenToType (IsStart, Token, &FpdtRecType, &Identifier);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FpdtRecord.RecordHeader.Length = 0;
  if (IsStart) {
    Ticker = StartTicker;
  } else {
    Ticker = EndTicker;
  }

  switch (FpdtRecType) {
    case GUID_EVENT_TYPE:
    case STRING_EVENT_TYPE:
      //
      // Can't find PerformanceEx protocol
      //
      Status = GetPerformanceExProtocol ();
      if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
      }
      //
      // Log SMM Driver Load and Start Image point.
      //
      if (IsStart) {
        mPerformanceEx->StartGaugeEx (Handle, Token, NULL, Ticker, Identifier);
      } else {
        mPerformanceEx->EndGaugeEx (Handle, Token, NULL, Ticker, Identifier);
      }
      break;

    case RUNTIME_MODULE_TABLE_PTR_TYPE:
      //
      // Can't find PerformanceEx protocol
      //
      Status = GetPerformanceExProtocol ();
      if (EFI_ERROR (Status)) {
        return EFI_UNSUPPORTED;
      }
      if (Handle == NULL) {
        return EFI_UNSUPPORTED;
      }
      if (mIsOutOfResource) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Allocate Reserved buffer for SMM runtime table.
      //
      RmptTableLength = sizeof (RUNTIME_PERF_TABLE_HEADER) + sizeof (RUNTIME_MODULE_PERF_RECORD) +
                        sizeof (RUNTIME_FUNCTION_PERF_RECORD) * Identifier;
      RmptTable       = (RUNTIME_MODULE_PERFORMANCE_TABLE *) FpdtAllocateReservedMemoryBelow4G (RmptTableLength);
      ASSERT (RmptTable != NULL);
      FpdtRecord.RuntimeTablePtr.PerformanceTablePointer = (UINT64) (UINTN) RmptTable;

      //
      // Insert SMM runtime table record.
      //
      mPerformanceEx->StartGaugeEx (Handle, Token, NULL, (UINT64) (UINTN) RmptTable, Identifier);

      //
      // Fill RMPT table
      //
      RmptTable->Header.Header.Signature = RMPT_SIG;
      RmptTable->Header.Header.Length    = (UINT32) RmptTableLength;
      //
      // Module Guid has been saved in RmptTable record. Don't need to save it here again.
      //
      ZeroMem (&RmptTable->Header.Guid, sizeof (EFI_GUID));
      RmptTable->Module.Header.Type      = RUNTIME_MODULE_TYPE;
      RmptTable->Module.Header.Length    = sizeof (RUNTIME_MODULE_PERF_RECORD);
      RmptTable->Module.Header.Revision  = RECORD_REVISION_1;

      //
      // Add RMPT data into the cached RMPT table.
      //
      if (mRmptTableNumber >= mMaxRmptTableNumber) {
        DataBuffer = AllocatePool ((mRmptTableNumber + RMPT_TABLE_NUMBER) * sizeof (RMPT_TABLE_DATA));
        if (DataBuffer == NULL) {
          mIsOutOfResource = TRUE;
          return EFI_OUT_OF_RESOURCES;
        }
        if (mRmptDataTable != NULL) {
          CopyMem (DataBuffer, mRmptDataTable, mMaxRmptTableNumber * sizeof (RMPT_TABLE_DATA));
          FreePool (mRmptDataTable);
        }
        mRmptDataTable = DataBuffer;
        mMaxRmptTableNumber = mRmptTableNumber + RMPT_TABLE_NUMBER;
      }
      mRmptDataTable[mRmptTableNumber].Handle              = (VOID *) Handle;
      mRmptDataTable[mRmptTableNumber].RmptTable           = RmptTable;
      mRmptDataTable[mRmptTableNumber++].MaxFunctionNumber = Identifier;
      break;

    case RUNTIME_FUNCTION_TYPE:
      if (Handle == NULL) {
        return EFI_UNSUPPORTED;
      }
      //
      // Base on Handle to find the matched RmptTable.
      //
      for (Index = 0; Index < mRmptTableNumber; Index ++) {
        if (mRmptDataTable[mRmptTableNumber].Handle == Handle) {
          break;
        }
      }
      if (Index == mRmptTableNumber) {
        return EFI_NOT_FOUND;
      }

      //
      // Base on Identifier to find Function Record.
      //
      RmptTable = mRmptDataTable[Index].RmptTable;
      RuntimeFunctionPerfRecord = &RmptTable->Function[0];
      for (Index = 0; Index < mRmptDataTable[Index].MaxFunctionNumber; Index++) {
        if (RuntimeFunctionPerfRecord[Index].Header.Type == 0) {
          //
          // New Function Id
          //
          break;
        }
        if (RuntimeFunctionPerfRecord[Index].FunctionId == Identifier) {
          //
          // Function Id exists.
          //
          break;
        }
      }

      if (Index == mRmptDataTable[Index].MaxFunctionNumber) {
        return EFI_NOT_FOUND;
      }

      //
      // Update Module Record.
      //
      Residency = GetTimeInNanoSecond (EndTicker) - GetTimeInNanoSecond (StartTicker);
      RmptTable->Module.ModuleCallCount ++;
      RmptTable->Module.ModuleResidency += Residency;

      //
      // New function ID is found. Need to add it into FunctionRecord buffer.
      //
      if (RuntimeFunctionPerfRecord[Index].Header.Type == 0) {
        RuntimeFunctionPerfRecord[Index].Header.Type     = RUNTIME_FUNCTION_TYPE;
        RuntimeFunctionPerfRecord[Index].Header.Length   = sizeof (RUNTIME_FUNCTION_PERF_RECORD);
        RuntimeFunctionPerfRecord[Index].Header.Revision = RECORD_REVISION_1;
        RuntimeFunctionPerfRecord[Index].FunctionId      = Identifier;
      }
      RuntimeFunctionPerfRecord[Index].FunctionCallCount++;
      RuntimeFunctionPerfRecord[Index].FunctionResidency += Residency;
      break;

    default:
      //
      // Record is undefined, return EFI_ABORTED
      //
      return EFI_ABORTED;
      break;
    }

    //
    // Report record one by one after records have been reported together.
    //
    if (FpdtRecord.RecordHeader.Length != 0) {
      REPORT_STATUS_CODE_EX (
        EFI_PROGRESS_CODE,
        EFI_SOFTWARE_SMM_DRIVER,
        0,
        NULL,
        &gEfiFirmwarePerformanceGuid,
        &FpdtRecord,
        FpdtRecord.RecordHeader.Length
        );
    }

  return EFI_SUCCESS;
}


/**
  Searches in the gauge array with keyword Handle, Token, Module and Identfier.

  This internal function searches for the gauge entry in the gauge array.
  If there is an entry that exactly matches the given keywords
  and its end time stamp is zero, then the index of that gauge entry is returned;
  otherwise, the the number of gauge entries in the array is returned.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  Identifier              32-bit identifier.

  @retval     The index of gauge entry in the array.

**/
UINT32
SmmSearchForGaugeEntry (
  IN CONST VOID                 *Handle,     OPTIONAL
  IN CONST CHAR8                *Token,      OPTIONAL
  IN CONST CHAR8                *Module,     OPTIONAL
  IN CONST UINT32               Identifier   OPTIONAL
  )
{
  INT32                     Index;
  UINT32                    NumberOfEntries;
  GAUGE_DATA_ENTRY_EX       *GaugeEntryExArray;

  if (Token == NULL) {
    Token = "";
  }
  if (Module == NULL) {
    Module = "";
  }

  NumberOfEntries = mGaugeData->NumberOfEntries;
  GaugeEntryExArray = (GAUGE_DATA_ENTRY_EX *) (mGaugeData + 1);

  for (Index = NumberOfEntries - 1; Index >=0; Index--) {
    if (GaugeEntryExArray[Index].EndTimeStamp == 0 &&
         (GaugeEntryExArray[Index].Handle == (EFI_PHYSICAL_ADDRESS) (UINTN) Handle) &&
         AsciiStrnCmp (GaugeEntryExArray[Index].Token, Token, SMM_PERFORMANCE_STRING_LENGTH) == 0 &&
         AsciiStrnCmp (GaugeEntryExArray[Index].Module, Module, SMM_PERFORMANCE_STRING_LENGTH) == 0 &&
         (GaugeEntryExArray[Index].Identifier == Identifier)
         ) {
      break;
    }
  }

  return Index;
}


/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, Module and Identifier.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             The data was read correctly from the device.
  @retval     EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.

**/
EFI_STATUS
EFIAPI
StartGaugeEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  GAUGE_DATA_ENTRY_EX       *GaugeEntryExArray;
  UINTN                     GaugeDataSize;
  GAUGE_DATA_HEADER         *NewGaugeData;
  UINTN                     OldGaugeDataSize;
  GAUGE_DATA_HEADER         *OldGaugeData;
  UINT32                    Index;

  AcquireSpinLock (&mSmmFpdtLock);

  Index = mGaugeData->NumberOfEntries;
  if (Index >= mMaxGaugeRecords) {
    //
    // Try to enlarge the scale of gauge array.
    //
    OldGaugeData      = mGaugeData;
    OldGaugeDataSize  = sizeof (GAUGE_DATA_HEADER) + sizeof (GAUGE_DATA_ENTRY_EX) * mMaxGaugeRecords;

    GaugeDataSize     = OldGaugeDataSize + (sizeof (GAUGE_DATA_ENTRY_EX) * INIT_SMM_GAUGE_DATA_ENTRIES);

    NewGaugeData = AllocateZeroPool (GaugeDataSize);
    if (NewGaugeData == NULL) {
      ReleaseSpinLock (&mSmmFpdtLock);
      return EFI_OUT_OF_RESOURCES;
    }

    mGaugeData       = NewGaugeData;
    mMaxGaugeRecords = mMaxGaugeRecords + INIT_SMM_GAUGE_DATA_ENTRIES;

    //
    // Initialize new data array and migrate old data one.
    //
    mGaugeData = CopyMem (mGaugeData, OldGaugeData, OldGaugeDataSize);

    FreePool (OldGaugeData);
  }

  GaugeEntryExArray               = (GAUGE_DATA_ENTRY_EX *) (mGaugeData + 1);
  GaugeEntryExArray[Index].Handle = (EFI_PHYSICAL_ADDRESS) (UINTN) Handle;

  if (Token != NULL) {
    AsciiStrnCpyS (GaugeEntryExArray[Index].Token, SMM_PERFORMANCE_STRING_SIZE, Token, SMM_PERFORMANCE_STRING_LENGTH);
  }
  if (Module != NULL) {
    AsciiStrnCpyS (GaugeEntryExArray[Index].Module, SMM_PERFORMANCE_STRING_SIZE, Module, SMM_PERFORMANCE_STRING_LENGTH);
  }

  GaugeEntryExArray[Index].EndTimeStamp = 0;
  GaugeEntryExArray[Index].Identifier = Identifier;

  if (TimeStamp == 0) {
    TimeStamp = GetPerformanceCounter ();
  }
  GaugeEntryExArray[Index].StartTimeStamp = TimeStamp;

  mGaugeData->NumberOfEntries++;

  InsertFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, 0, Identifier);
  ReleaseSpinLock (&mSmmFpdtLock);

  return EFI_SUCCESS;
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, Module and Identifier and has an end time value of zero.
  If the record can not be found then return EFI_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the found record
                                      is same as the one found by EndGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             The end of  the measurement was recorded.
  @retval     EFI_NOT_FOUND           The specified measurement record could not be found.

**/
EFI_STATUS
EFIAPI
EndGaugeEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  GAUGE_DATA_ENTRY_EX   *GaugeEntryExArray;
  UINT32                Index;

  AcquireSpinLock (&mSmmFpdtLock);

  if (TimeStamp == 0) {
    TimeStamp = GetPerformanceCounter ();
  }

  Index = SmmSearchForGaugeEntry (Handle, Token, Module, Identifier);
  if (Index >= mGaugeData->NumberOfEntries) {
    ReleaseSpinLock (&mSmmFpdtLock);
    return EFI_NOT_FOUND;
  }
  GaugeEntryExArray = (GAUGE_DATA_ENTRY_EX *) (mGaugeData + 1);
  GaugeEntryExArray[Index].EndTimeStamp = TimeStamp;

  InsertFpdtMeasurement (FALSE, Handle, Token, Module, GaugeEntryExArray[Index].StartTimeStamp, TimeStamp, Identifier);
  ReleaseSpinLock (&mSmmFpdtLock);

  return EFI_SUCCESS;
}


/**
  Retrieves a previously logged performance measurement.
  It can also retrieve the log created by StartGauge and EndGauge of PERFORMANCE_PROTOCOL,
  and then assign the Identifier with 0.

  Retrieves the performance log entry from the performance log specified by LogEntryKey.
  If it stands for a valid entry, then EFI_SUCCESS is returned and
  GaugeDataEntryEx stores the pointer to that entry.

  @param[in]  LogEntryKey             The key for the previous performance measurement log entry.
                                      If 0, then the first performance measurement log entry is retrieved.
  @param[out] GaugeDataEntryEx        The indirect pointer to the extended gauge data entry specified by LogEntryKey
                                      if the retrieval is successful.

  @retval     EFI_SUCCESS             The GuageDataEntryEx is successfully found based on LogEntryKey.
  @retval     EFI_NOT_FOUND           The LogEntryKey is the last entry (equals to the total entry number).
  @retval     EFI_INVALIDE_PARAMETER  The LogEntryKey is not a valid entry (greater than the total entry number).
  @retval     EFI_INVALIDE_PARAMETER  GaugeDataEntryEx is NULL.

**/
EFI_STATUS
EFIAPI
GetGaugeEx (
  IN  UINTN                 LogEntryKey,
  OUT GAUGE_DATA_ENTRY_EX   **GaugeDataEntryEx
  )
{
  UINTN               NumberOfEntries;
  GAUGE_DATA_ENTRY_EX *GaugeEntryExArray;

  NumberOfEntries = (UINTN) (mGaugeData->NumberOfEntries);
  if (LogEntryKey > NumberOfEntries) {
    return EFI_INVALID_PARAMETER;
  }
  if (LogEntryKey == NumberOfEntries) {
    return EFI_NOT_FOUND;
  }

  GaugeEntryExArray = (GAUGE_DATA_ENTRY_EX *) (mGaugeData + 1);

  if (GaugeDataEntryEx == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *GaugeDataEntryEx = &GaugeEntryExArray[LogEntryKey];

  return EFI_SUCCESS;
}


//
// Interfaces for SMM PerformanceEx Protocol.
//
PERFORMANCE_EX_PROTOCOL mPerformanceExInterface = {
  StartGaugeEx,
  EndGaugeEx,
  GetGaugeEx
};


/**
  SmmBase2 protocol notify callback function, when SMST and SMM memory service get initialized
  this function is callbacked to initialize the Smm Performance Lib

  @param[in]  Event         The event of notify protocol.
  @param[in]  Context       Notify event context.

**/
VOID
EFIAPI
InitializeSmmCorePerformanceLib (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
  EFI_HANDLE      Handle;
  EFI_STATUS      Status;

  //
  // Initialize spin lock
  //
  InitializeSpinLock (&mSmmFpdtLock);

  mMaxGaugeRecords = INIT_SMM_GAUGE_DATA_ENTRIES;

  mGaugeData = AllocateZeroPool (sizeof (GAUGE_DATA_HEADER) + (sizeof (GAUGE_DATA_ENTRY_EX) * mMaxGaugeRecords));
  ASSERT (mGaugeData != NULL);

  //
  // Install the protocol interfaces for SMM performance library instance.
  //
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gSmmPerformanceExProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mPerformanceExInterface
                    );
  ASSERT_EFI_ERROR (Status);
}


/**
  The constructor function initializes the Performance Measurement Enable flag and
  registers SmmBase2 protocol notify callback.
  It will ASSERT() if one of these operations fails and it will always return EFI_SUCCESS.

  @param[in]  ImageHandle     The firmware allocated handle for the EFI image.
  @param[in]  SystemTable     A pointer to the EFI System Table.

  @retval     EFI_SUCCESS     The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmCoreFpdtPerformanceLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  VOID        *Registration;

  mPerformanceMeasurementEnabled =  (BOOLEAN) ((PcdGet8 (PcdPerformanceLibraryPropertyMask) & PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED) != 0);
  if (!mPerformanceMeasurementEnabled) {
    //
    // Do not initialize performance infrastructure if not required.
    //
    return EFI_SUCCESS;
  }

  //
  // Create the events to do the library init.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  InitializeSmmCorePerformanceLib,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSmmBase2ProtocolGuid,
                  Event,
                  &Registration
                  );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, Module and Identifier.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

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
  return (RETURN_STATUS) StartGaugeEx (Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, Module and Identifier and has an end time value of zero.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

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
  return (RETURN_STATUS) EndGaugeEx (Handle, Token, Module, TimeStamp, Identifier);
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

  @param[in]   LogEntryKey             On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance log entry.
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
  @param[out]  Identifier              Pointer to the 32-bit identifier that was recorded when the measurement
                                       was ended.

  @return      The key for the next performance log entry (in general case).

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
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, and Module.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

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
  return (RETURN_STATUS) StartGaugeEx (Handle, Token, Module, TimeStamp, 0);
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, and Module and has an end time value of zero.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

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
  return (RETURN_STATUS) EndGaugeEx (Handle, Token, Module, TimeStamp, 0);
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

  @param[in]   LogEntryKey             On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance log entry.
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

  @return      The key for the next performance log entry (in general case).

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
  return mPerformanceMeasurementEnabled;
}

