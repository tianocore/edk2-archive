/** @file
  This file defines the extended FPDT record types from iFPDT specification 1.0.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EXTENDED_FIRMWARE_PERFORMANCE_DATA_H__
#define __EXTENDED_FIRMWARE_PERFORMANCE_DATA_H__

#include <IndustryStandard/Acpi50.h>

//
// Known performance tokens
//
#define SEC_TOK                         "SEC"             ///< SEC Phase
#define DXE_TOK                         "DXE"             ///< DXE Phase
#define SHELL_TOK                       "SHELL"           ///< Shell Phase
#define PEI_TOK                         "PEI"             ///< PEI Phase
#define BDS_TOK                         "BDS"             ///< BDS Phase
#define DRIVERBINDING_START_TOK         "DB:Start:"       ///< Driver Binding Start() function call
#define DRIVERBINDING_SUPPORT_TOK       "DB:Support:"     ///< Driver Binding Support() function call
#define LOAD_IMAGE_TOK                  "LoadImage:"      ///< Load a dispatched module
#define START_IMAGE_TOK                 "StartImage:"     ///< Dispatched Modules Entry Point execution
#define PEIM_TOK                        "PEIM"            ///< PEIM Modules Entry Point execution
#define PREMEM_TOK                      "PreMem"          ///< PEI PreMem Phase
#define POSTMEM_TOK                     "PostMem"         ///< PEI PostMem Phase
#define DISMEM_TOK                      "DisMem"          ///< PEI Phase dispatch any notifies for Memory Discovered.
#define HARDWARE_BOOT_TOK               "HardwareBoot"    ///< PreOS Boot
#define EVENT_REC_TOK                   "EventRec"        ///< GUID or String Event
#define BDS_ATTEMPT_TOK                 "BdsAttempt"      ///< Attempt BDS Boot
#define DXE_CORE_DISP_INIT_TOK          "CoreInitializeDispatcher" ///< DXE Core Initialize Dispatcher
#define COREDISPATCHER_TOK              "CoreDispatcher"  ///< Dxe Core Dispatcher
#define SMM_MODULE_TOK                  "SmmModule"       ///< SMM Module Entry Point
#define SMM_FUNCTION_TOK                "SmmFunction"     ///< SMM Function execution.
#define BOOT_MODULE_TOK                 "BootModule"      ///< Boot Module Table record
#define PLATFORM_BOOT_TOK               "PlatformBoot"    ///< Platform Boot Table record
#define SCRIPT_EXEC_TOK                 "ScriptExec"      ///< Boot script execution

//
// FPDT Record types
//
#define PLATFORM_BOOT_TABLE_PTR_TYPE   0x1001
#define BOOT_MODULE_TABLE_PTR_TYPE     0x1002
#define RUNTIME_MODULE_TABLE_PTR_TYPE  0x1003
#define TIMESTAMP_DELTA_TYPE           0x1004
#define CUMULATIVE_PREOS_FIRMWARE_TYPE 0x100F
#define GUID_EVENT_TYPE                0x1010
#define STRING_EVENT_TYPE              0x1011
#define BDS_ATTEMPT_EVENT_TYPE         0x1012
#define RUNTIME_MODULE_TYPE            0x1020
#define RUNTIME_FUNCTION_TYPE          0x1021
#define PLATFORM_BOOT_TABLE_TYPE       0x1022

//
//  Progress Identifiers for Event Records
//
#define MODULE_START_ID                 0x01
#define MODULE_END_ID                   0x02
#define MODULE_LOADIMAGE_START_ID       0x03
#define MODULE_LOADIMAGE_END_ID         0x04
#define MODULE_DRIVERBINDING_START_ID   0x05
#define MODULE_DRIVERBINDING_END_ID     0x06
#define PREMEM_START_ID                 0x20
#define PREMEM_END_ID                   0x21
#define DISMEM_START_ID                 0x30
#define DISMEM_END_ID                   0x31
#define POSTMEM_START_ID                0x40
#define POSTMEM_END_ID                  0x41
#define SCRIPT_EXEC_START_ID            0x50
#define SCRIPT_EXEC_END_ID              0x51
#define DXE_START_ID                    0x7000
#define DXE_END_ID                      0x7001
#define DXE_CORE_DISP_START_ID          0x7010
#define DXE_CORE_DISP_END_ID            0x7011
#define COREDISPATCHER_START_ID         0x7020
#define COREDISPATCHER_END_ID           0x7021
#define BDS_START_ID                    0xA0A0
#define BDS_END_ID                      0xA0A1

//
// Record information
//
#define STRING_EVENT_RECORD_NAME_LENGTH 24
#define RMPT_SIG                        SIGNATURE_32 ('R', 'M', 'P', 'T')

//
// Record version
//
#define RECORD_REVISION_1               0x01

//
// Fpdt record table structures
//
#pragma pack(1)
///
/// FPDT Boot Performance String Event Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  ///
  /// Progress Identifier
  ///
  UINT16                                      ProgressID;
  ///
  /// APIC ID for the processor in the system used as a timestamp clock source.
  ///
  UINT32                                      ApicID;
  ///
  /// 64-bit value describing elapsed time since the most recent deassertion of processor reset.
  ///
  UINT64                                      Timestamp;
  ///
  /// If ProgressID < 0x10, GUID of the referenced module; otherwise, GUID of the module logging the event.
  ///
  EFI_GUID                                    Guid;
  ///
  /// ASCII string describing the module.
  ///
  UINT8                                       NameString[STRING_EVENT_RECORD_NAME_LENGTH];
} STRING_EVENT_RECORD;

///
/// FPDT Boot Performance Guid Event Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  ///
  /// Progress Identifier
  ///
  UINT16                                      ProgressID;
  ///
  /// APIC ID for the processor in the system used as a timestamp clock source.
  ///
  UINT32                                      ApicID;
  ///
  /// 64-bit value describing elapsed time since the most recent deassertion of processor reset.
  ///
  UINT64                                      Timestamp;
  ///
  /// If ProgressID < 0x10, GUID of the referenced module; otherwise, GUID of the module logging the event.
  ///
  EFI_GUID                                    Guid;
} GUID_EVENT_RECORD;

///
/// FPDT BDS Attempt Event Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  ///
  /// APIC ID for the processor in the system used as a timestamp clock source.
  ///
  UINT32                                      ApicID;
  ///
  /// Numbered BDS attempt, starting at 1.
  ///
  UINT16                                      BdsAttemptNo;
  ///
  /// Timestamp of the BDS attempt event.
  ///
  UINT64                                      Timestamp;
  ///
  /// Boot variable in the form "BOOT<nnnn>"
  ///
  UINT64                                      UEFIBootVar;
  ///
  /// Device path of this BDS attempt as an ASCII string.
  /// Pad 2 byte to make end byte at the 4 byte alignment.
  ///
  CHAR8                                       DevicePathString[2];
} BDS_ATTEMPT_RECORD;

///
/// FPDT Firmware Boot Performance Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// Total elapsed time by platform from the start of firmware execution until handover to OS.
  ///
  UINT64                                      Timestamp;
} CUMULATIVE_PREOS_FIRMWARE_RECORD;

typedef struct {
  ///
  /// Platform-specific identifier for each unique processor in the system using a separate timestamp clock source.
  ///
  UINT16 ProcessorID;
  ///
  /// Timestamp delay between ProcessorID[0] start and this ProcessorID. The entry for ProcessorID[0] must be zero.
  ///
  UINT16 TimestampDelta[3];
} TIMESTAMP_DELTA;

///
/// FPDT BDS Attempt Event Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  ///
  /// One or more delta for multiple processors.
  ///
  TIMESTAMP_DELTA TimestampDelta[1];
} TIMESTAMP_DELTA_RECORD;

///
/// FPDT Platform Boot Performance Table Pointer Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// 64-bit processor-relative physical address of Platform Boot Performance Table.
  ///
  UINT64                                      PerformanceTablePointer;
} PLATFORM_BOOT_PERFORMANCE_TABLE_POINTER_RECORD;

///
/// FPDT Boot Module Performance Table Pointer Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// GUID of the module logging the data.
  ///
  EFI_GUID                                    Guid;
  ///
  /// 64-bit processor-relative physical address of a Boot SubRecord Performance Table.
  ///
  UINT64                                      PerformanceTablePointer;
} BOOT_MODULE_PERFORMANCE_TABLE_POINTER_RECORD;

///
/// FPDT Runtime Module Performance Table Pointer Record Structure
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// GUID of the runtime module.
  ///
  EFI_GUID                                    Guid;
  ///
  /// 64-bit processor-relative physical address of the runtime module performance record.
  ///
  UINT64                                      PerformanceTablePointer;
} RUNTIME_MODULE_PERFORMANCE_TABLE_POINTER_RECORD;

///
/// Runtime Module Performance Table Header
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER  Header;
  ///
  /// GUID of the runtime module.
  ///
  EFI_GUID                                    Guid;
} RUNTIME_PERF_TABLE_HEADER;

///
/// Runtime Module Performance Record
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  ///
  /// A count of the number of runtime calls into this module.
  ///
  UINT32                                      ModuleCallCount;
  ///
  /// Accumulated amount of time spent in this runtime module.
  ///
  UINT64                                      ModuleResidency;
} RUNTIME_MODULE_PERF_RECORD;

///
/// Runtime Function Performance Record
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// Module-specific and -unique value identifying the specific callback function within the module.
  ///
  UINT32                                      FunctionId;
  ///
  /// A count of the number of calls into this runtime function.
  ///
  UINT32                                      FunctionCallCount;
  ///
  /// Accumulated amount of time spent in the Function
  ///
  UINT64                                      FunctionResidency;
} RUNTIME_FUNCTION_PERF_RECORD;

///
/// Runtime Module Performance table
///
typedef struct {
  ///
  /// Runtime Module Performance Table Header
  ///
  RUNTIME_PERF_TABLE_HEADER         Header;
  ///
  /// Runtime Module Performance Record
  ///
  RUNTIME_MODULE_PERF_RECORD        Module;
  ///
  /// One or more Runtime Function Performance Records
  ///
  RUNTIME_FUNCTION_PERF_RECORD      Function[1];
} RUNTIME_MODULE_PERFORMANCE_TABLE;

///
/// Platform Boot Performance Table Record
///
typedef struct {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER Header;
  UINT32                                      Reserved;
  ///
  /// Amount of time spent between pre-firmware phase 1 and wake signal to platform.
  ///
  UINT64                                      TimestampDelta1;
  ///
  /// Amount of time spent between pre-firmware phase 2 and wake signal to platform.
  ///
  UINT64                                      TimestampDelta2;
  ///
  /// Amount of time spent between pre-firmware phase 3 and wake signal to platform.
  ///
  UINT64                                      TimestampDelta3;
} PLATFORM_BOOT_TABLE_RECORD;

///
/// Platform boot Performance Table
///
typedef struct {
  ///
  /// Platform boot Performance Table Header
  ///
  EFI_ACPI_5_0_FPDT_PERFORMANCE_TABLE_HEADER  Header;
  ///
  /// Platform boot Performance Table Record
  ///
  PLATFORM_BOOT_TABLE_RECORD                  PlatformBoot;
} PLATFORM_BOOT_PERFORMANCE_TABLE;
#pragma pack()

///
/// Union of all FPDT record and pointers to FPDT record.
///
typedef union {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER        RecordHeader;
  PLATFORM_BOOT_PERFORMANCE_TABLE_POINTER_RECORD     PlatformBootTablePtr;
  BOOT_MODULE_PERFORMANCE_TABLE_POINTER_RECORD       BootTablePtr;
  RUNTIME_MODULE_PERFORMANCE_TABLE_POINTER_RECORD    RuntimeTablePtr;
  TIMESTAMP_DELTA_RECORD                             TimeStampDelta;
  CUMULATIVE_PREOS_FIRMWARE_RECORD                   CumulativePreOsFirmware;
  GUID_EVENT_RECORD                                  GuidEvent;
  STRING_EVENT_RECORD                                StringEvent;
  BDS_ATTEMPT_RECORD                                 BdsAttempt;
} FPDT_RECORD;

typedef union {
  EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER        *RecordHeader;
  PLATFORM_BOOT_PERFORMANCE_TABLE_POINTER_RECORD     *PlatformBootTablePtr;
  BOOT_MODULE_PERFORMANCE_TABLE_POINTER_RECORD       *BootTablePtr;
  RUNTIME_MODULE_PERFORMANCE_TABLE_POINTER_RECORD    *RuntimeTablePtr;
  TIMESTAMP_DELTA_RECORD                             *TimeStampDelta;
  CUMULATIVE_PREOS_FIRMWARE_RECORD                   *CumulativePreOsFirmware;
  GUID_EVENT_RECORD                                  *GuidEvent;
  STRING_EVENT_RECORD                                *StringEvent;
  BDS_ATTEMPT_RECORD                                 *BdsAttempt;
} FPDT_RECORD_PTR;

#endif

