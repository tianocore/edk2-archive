/** @file
  Some definitions for MP services Protocol.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MP_SERVICE_H_
#define _MP_SERVICE_H_

#include <Protocol/MpService.h>

#include "MpCommon.h"

//
// Constant definitions
//
#define FOURGB                            0x100000000
#define ONEPAGE                           0x1000

#define RENDEZVOUS_PROC_LENGTH            0x1000
#define STACK_SIZE_PER_PROC               0x8000
#define MAX_CPU_S3_MTRR_ENTRY             0x0020
#define MAX_CPU_S3_TABLE_SIZE             0x0400

#define AP_HALT_CODE_SIZE                 10

#define CPU_CHECK_AP_INTERVAL             10     // multiply to microseconds for gBS->SetTimer in 100nsec.
#define CPU_WAIT_FOR_TASK_TO_BE_COMPLETED 100000 // microseconds
///
///  The MP data structure follows.
///
#define CPU_SWITCH_STATE_IDLE   0
#define CPU_SWITCH_STATE_STORED 1
#define CPU_SWITCH_STATE_LOADED 2

#define MSR_L3_CACHE_DISABLE    0x40

typedef struct {
  UINT8             Lock;         ///< offset 0
  UINT8             State;        ///< offset 1
  UINTN             StackPointer; ///< offset 4 / 8
  IA32_DESCRIPTOR   Gdtr;         ///< offset 8 / 16
  IA32_DESCRIPTOR   Idtr;         ///< offset 14 / 26
} CPU_EXCHANGE_ROLE_INFO;

//
// MTRR table definitions
//
typedef struct {
  UINT16 Index;
  UINT64 Value;
} EFI_MTRR_VALUES;

typedef enum {
  CPU_STATE_IDLE,
  CPU_STATE_BLOCKED,
  CPU_STATE_READY,
  CPU_STATE_BUSY,
  CPU_STATE_FINISHED,
  CPU_STATE_DISABLED
} CPU_STATE;

//
// Define CPU feature information
//
#define MAX_FEATURE_NUM 6
typedef struct {
  UINTN  Index;
  UINT32 ApicId;
  UINT32 Version;
  UINT32 FeatureDelta;
  UINT32 Features[MAX_FEATURE_NUM];
} LEAST_FEATURE_PROC;

///
/// Define Individual Processor Data block.
///
typedef struct {
  UINT32                    ApicID;
  EFI_AP_PROCEDURE          Procedure;
  VOID                      *Parameter;
  UINT8                     StateLock;
  UINT8                     ProcedureLock;
  UINT32                    Health;
  BOOLEAN                   SecondaryCpu;
  EFI_CPU_PHYSICAL_LOCATION PhysicalLocation;
  CPU_STATE                 State;
  //
  // for PI MP Services Protocol
  //
  BOOLEAN                   *Finished;
  UINT64                    ExpectedTime;
  EFI_EVENT                 WaitEvent;
  EFI_EVENT                 CheckThisAPEvent;
} CPU_DATA_BLOCK;

typedef struct {
  UINT32 ApicId;
  UINT32 MsrIndex;
  UINT64 MsrValue;
} MP_CPU_S3_SCRIPT_DATA;

typedef struct {
  UINT32 S3BootScriptTable;
  UINT32 S3BspMtrrTable;
} MP_CPU_S3_DATA_POINTER;

///
/// Define MP data block which consumes individual processor block.
///
typedef struct {
  UINT8                  ApSerializeLock;
  BOOLEAN                VmxEnable;
  BOOLEAN                TxtEnable;
  BOOLEAN                ThreeStrikeCounterDisable;
  BOOLEAN                MonitorMwaitEnable;
  BOOLEAN                MachineCheckEnable;
  BOOLEAN                AesEnable;
  BOOLEAN                DebugInterfaceEnable;
  BOOLEAN                DebugInterfaceLockEnable;
  BOOLEAN                EnableSecondaryCpu;
  UINTN                  NumberOfCpus;
  UINTN                  MaximumCpusForThisSystem;
  CPU_EXCHANGE_ROLE_INFO BSPInfo;
  CPU_EXCHANGE_ROLE_INFO APInfo;
  EFI_CPU_ARCH_PROTOCOL  *CpuArch;
  EFI_EVENT              CheckAllAPsEvent;
  EFI_EVENT              WaitEvent;
  UINTN                  BSP;
  BIST_HOB_DATA          *BistHobData;
  UINTN                  BistHobSize;
  UINTN                  FinishCount;
  UINTN                  StartCount;
  EFI_AP_PROCEDURE       Procedure;
  VOID                   *ProcArguments;
  BOOLEAN                SingleThread;
  UINTN                  StartedCpuNumber;
  CPU_DATA_BLOCK         CpuData[MAXIMUM_CPU_NUMBER];
  CPU_STATE_CHANGE_CAUSE DisableCause[MAXIMUM_CPU_NUMBER];
  UINT8                  S3BootScriptLock;
  UINT32                 S3BootScriptCount;
  MP_CPU_S3_DATA_POINTER S3DataPointer;
  MP_CPU_S3_SCRIPT_DATA  S3BootScriptTable[MAX_CPU_S3_TABLE_SIZE];
  EFI_MTRR_VALUES        S3BspMtrrTable[MAX_CPU_S3_MTRR_ENTRY];

  ///
  /// for PI MP Services Protocol
  ///
  BOOLEAN                CpuList[MAXIMUM_CPU_NUMBER];
  UINTN                  **FailedCpuList;
  UINT64                 ExpectedTime;
  EFI_EVENT              CheckAPsEvent;
} MP_SYSTEM_DATA;

typedef struct {
  ACPI_CPU_DATA        AcpiCpuData;
  MP_SYSTEM_DATA       MPSystemData;
  IA32_DESCRIPTOR      GdtrProfile;
  IA32_DESCRIPTOR      IdtrProfile;
} MP_CPU_RESERVED_DATA;

extern MP_SYSTEM_DATA              *mMPSystemData;
extern ACPI_CPU_DATA               *mAcpiCpuData;
extern EFI_MP_SERVICES_PROTOCOL    mMpService; // equals to EFI_MP_SERVICES_PROTOCOL

///
/// Prototypes.
///

/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  This service retrieves the number of logical processor in the platform
  and the number of those logical processors that are enabled on this boot.
  This service may only be called from the BSP.

  @param[in]  This                        A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[out] NumberOfProcessors          Pointer to the total number of logical processors in the system,
                                          including the BSP and disabled APs.
  @param[out] NumberOfEnabledProcessors   Pointer to the number of enabled logical processors that exist
                                          in system, including the BSP.

  @retval     EFI_SUCCESS                 Number of logical processors and enabled logical processors retrieved..
  @retval     EFI_DEVICE_ERROR            Caller processor is AP.
  @retval     EFI_INVALID_PARAMETER       NumberOfProcessors is NULL.
  @retval     EFI_INVALID_PARAMETER       NumberOfEnabledProcessors is NULL.

**/
EFI_STATUS
EFIAPI
GetNumberOfProcessors (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  OUT UINTN                   *NumberOfProcessors,
  OUT UINTN                   *NumberOfEnabledProcessors
  );

/**
  Implementation of GetProcessorInfo() service of MP Services Protocol.

  Gets detailed MP-related information on the requested processor at the
  instant this call is made. This service may only be called from the BSP.

  @param[in]  This                    A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in]  ProcessorNumber         The handle number of processor.
  @param[out] ProcessorInfoBuffer     A pointer to the buffer where information for the requested processor is deposited.

  @retval     EFI_SUCCESS             Processor information successfully returned.
  @retval     EFI_DEVICE_ERROR        Caller processor is AP.
  @retval     EFI_INVALID_PARAMETER   ProcessorInfoBuffer is NULL
  @retval     EFI_NOT_FOUND           Processor with the handle specified by ProcessorNumber does not exist.

**/
EFI_STATUS
EFIAPI
GetProcessorInfo (
  IN EFI_MP_SERVICES_PROTOCOL    *This,
  IN UINTN                       ProcessorNumber,
  OUT EFI_PROCESSOR_INFORMATION  *ProcessorInfoBuffer
  );

/**
  MP Service to get specified application processor (AP)
  to execute a caller-provided code stream.

  @param[in]  This                    Pointer to MP Service Protocol
  @param[in]  Procedure               The procedure to be assigned to AP.
  @param[in]  ProcessorNumber         The handle number of processor.
  @param[in]  WaitEvent               If timeout, the event to be triggered after this AP finishes.
  @param[in]  TimeoutInMicroSecs      The timeout value in microsecond. Zero means infinity.
  @param[in]  ProcArguments           Argument for Procedure.
  @param[out] Finished                Pointer to the mode of AP running.

  @retval     EFI_INVALID_PARAMETER   Procudure is NULL.
  @retval     EFI_INVALID_PARAMETER   Number of CPU out of range, or it belongs to BSP.
  @retval     EFI_INVALID_PARAMETER   Specified CPU is not idle.
  @retval     EFI_SUCCESS             The AP has finished.
  @retval     EFI_TIMEOUT             Time goes out before the AP has finished.

**/
EFI_STATUS
EFIAPI
StartupThisAP (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN UINTN                    ProcessorNumber,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN TimeoutInMicroSecs OPTIONAL,
  IN VOID *ProcArguments      OPTIONAL,
  OUT BOOLEAN *Finished       OPTIONAL
  );

/**
  MP Service to get all the available application processors (APs)
  to execute a caller-provided code stream.

  @param[in]  This                    Pointer to MP Service Protocol
  @param[in]  Procedure               The procedure to be assigned to APs.
  @param[in]  SingleThread            If true, all APs execute in block mode.
                                      Otherwise, all APs exceute in non-block mode.
  @param[in]  WaitEvent               If timeout, the event to be triggered after all APs finish.
  @param[in]  TimeoutInMicroSecs      The timeout value in microsecond. Zero means infinity.
  @param[in]  ProcArguments           Argument for Procedure.
  @param[out] FailedCPUList           If not NULL, all APs that fail to start will be recorded in the list.

  @retval     EFI_INVALID_PARAMETER   Procudure is NULL.
  @retval     EFI_SUCCESS             Only 1 logical processor exists.
  @retval     EFI_SUCCESS             All APs have finished.
  @retval     EFI_TIMEOUT             Time goes out before all APs have finished.

**/
EFI_STATUS
EFIAPI
StartupAllAPs (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN BOOLEAN                  SingleThread,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN                    TimeoutInMicroSecs,
  IN VOID *ProcArguments      OPTIONAL,
  OUT UINTN **FailedCPUList   OPTIONAL
  );

/**
  MP Service to makes the current BSP into an AP and then switches the
  designated AP into the AP. This procedure is usually called after a CPU
  test that has found that BSP is not healthy to continue it's responsbilities.

  @param[in] This                    Pointer to MP Service Protocol.
  @param[in] ProcessorNumber         The handle number of processor.
  @param[in] EnableOldBSPState       Whether to enable or disable the original BSP.

  @retval    EFI_INVALID_PARAMETER   Number for Specified AP out of range.
  @retval    EFI_INVALID_PARAMETER   Number of specified CPU belongs to BSP.
  @retval    EFI_NOT_READY           Specified AP is not idle.
  @retval    EFI_SUCCESS             BSP successfully switched.

**/
EFI_STATUS
EFIAPI
SwitchBSP (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                    ProcessorNumber,
  IN BOOLEAN                  EnableOldBSPState
  );

/**
  Implementation of EnableDisableAP() service of MP Services Protocol.

  This service lets the caller enable or disable an AP.
  This service may only be called from the BSP.

  @param[in] This                    A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber         The handle number of processor.
  @param[in] NewAPState              Indicates whether the newstate of the AP is enabled or disabled.
  @param[in] HealthState             Indicates new health state of the AP..

  @retval    EFI_SUCCESS             AP successfully enabled or disabled.
  @retval    EFI_DEVICE_ERROR        Caller processor is AP.
  @retval    EFI_NOT_FOUND           Processor with the handle specified by ProcessorNumber does not exist.
  @retval    EFI_INVALID_PARAMETERS  ProcessorNumber specifies the BSP.

**/
EFI_STATUS
EFIAPI
EnableDisableAP (
  IN EFI_MP_SERVICES_PROTOCOL *This,
  IN UINTN                      ProcessorNumber,
  IN BOOLEAN                    NewAPState,
  IN UINT32 *HealthState OPTIONAL
  );

/**
  Initialize multiple processors and collect MP related data

  @retval  EFI_SUCCESS           Multiple processors get initialized and data collected successfully
  @retval  Other                 The operation failed due to some reason

**/
EFI_STATUS
InitializeMpSystemData (
  VOID
  );

/**
  Wake up all the application processors

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval     EFI_SUCCESS    APs are successfully waked up

**/
EFI_STATUS
WakeUpAps (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

/**
  Exchange 2 processors (BSP to AP or AP to BSP)

  @param[in]  MyInfo        CPU info for current processor
  @param[in]  OthersInfo    CPU info that will be exchanged with

**/
VOID
AsmExchangeRole (
  IN CPU_EXCHANGE_ROLE_INFO *MyInfo,
  IN CPU_EXCHANGE_ROLE_INFO *OthersInfo
  );

/**
  Switch current BSP processor to AP

  @param[in]  MPSystemData     Pointer to the data structure containing MP related data

**/
VOID
EFIAPI
FutureBspProc (
  IN MP_SYSTEM_DATA *MPSystemData
  );

/**
  Searches the HOB list provided by the core to find
  if a MP guided HOB list exists or not. If it does, it copies it to the driver
  data area, else returns 0

  @param[in]  MPSystemData     Pointer to an MP_SYSTEM_DATA structure

  @retval     EFI_SUCCESS      Success
  @retval     EFI_NOT_FOUND    HOB not found or else

**/
EFI_STATUS
GetMpBistStatus (
  IN MP_SYSTEM_DATA *MPSystemData
  );

/**
  Initialize the state information for the MP DXE Protocol.

**/
VOID
EFIAPI
InitializeMpServices (
  VOID
  );

/**
  Copy Global MTRR data to S3

**/
VOID
SaveBspMtrrForS3 (
  VOID
  );

/**
  This function is called by all processors (both BSP and AP) once and collects MP related data

  @param[in]  MPSystemData   Pointer to the data structure containing MP related data
  @param[in]  BSP            TRUE if the CPU is BSP
  @param[in]  BistParam      BIST (build-in self test) data for the processor. This data
                             is only valid for processors that are waked up for the 1st
                             time in this CPU DXE driver.

  @retval    EFI_SUCCESS     Data for the processor collected and filled in

**/
EFI_STATUS
FillInProcessorInformation (
  IN MP_SYSTEM_DATA *MPSystemData,
  IN BOOLEAN        BSP,
  IN UINT32         BistParam
  );

/**
  Set APIC BSP bit

  @param[in] Enable          Enable as BSP or not

  @retval    EFI_SUCCESS     Always return success

**/
EFI_STATUS
SetApicBspBit (
  IN BOOLEAN Enable
  );

/**
  Change CPU state

  @param[in]  CpuNumber     CPU number
  @param[in]  NewState      The new state that will be changed to
  @param[in]  Cause         Cause

  @retval     EFI_SUCCESS   Always return success

**/
EFI_STATUS
ChangeCpuState (
  IN UINTN                  CpuNumber,
  IN BOOLEAN                NewState,
  IN CPU_STATE_CHANGE_CAUSE Cause
  );

/**
  Check if this is non-core processor - HT AP thread

  @retval  TRUE       If this is HT AP thread
  @retval  FALSE      If this is core thread

**/
BOOLEAN
IsSecondaryThread (
  VOID
  );

/**
  If timeout occurs in StartupAllAps(), a timer is set, which invokes this
  procedure periodically to check whether all APs have finished.

  @param[in]  Event         Event triggered.
  @param[in]  Context       Parameter passed with the event.

**/
VOID
EFIAPI
CheckAllApsStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  Check if this AP has finished task

  @param[in]  Event        Event triggered.
  @param[in]  Context      Parameter passed with the event.

**/
VOID
EFIAPI
CheckThisApStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  Convert the timeout value to TSC value

  @param[in]  TimeoutInMicroSecs       How many microseconds the timeout is

  @retval     expected TSC value for timeout

**/
UINT64
CalculateTimeout (
  IN UINTN TimeoutInMicroSecs
  );

/**
  Check if timeout happened

  @param[in]  ExpectedTsc       The TSC value for timeout

  @retval     TRUE              If timeout happened
  @retval     FALSE             If timeout not yet happened

**/
BOOLEAN
CheckTimeout (
  IN UINT64 ExpectedTsc
  );

/**
  Get the next blocked processor

  @param[out]  NextCpuNumber   That will be updated for next blocked CPU number

  @retval      EFI_SUCCESS     The next blocked CPU found
  @retval      EFI_NOT_FOUND   Cannot find blocked CPU

**/
EFI_STATUS
GetNextBlockedCpuNumber (
  OUT UINTN *NextCpuNumber
  );

/**
  Procedure for detailed initialization of APs. It will be assigned to all APs while
  they are waken up for the second time.

**/
VOID
DetailedMpInitialization (
  VOID
  );

/**
  Function to wake up a specified AP and assign procedure to it.

  @param[in]  CpuData         CPU data block for the specified AP.
  @param[in]  Procedure       Procedure to assign.
  @param[in]  Parameter       Argument for Procedure.

**/
VOID
WakeUpAp (
  IN CPU_DATA_BLOCK   *CpuData,
  IN EFI_AP_PROCEDURE Procedure,
  IN VOID             *Parameter
  );

/**
  Checks APs' status periodically.

  This function is triggerred by timer perodically to check the
  state of APs for StartupAllAPs() and StartupThisAP() executed
  in non-blocking mode.

  @param[in]  Event     Event triggered.
  @param[in]  Context   Parameter passed with the event.

**/
VOID
EFIAPI
CheckApsStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  Checks status of all APs.

  This function checks whether all APs have finished task assigned by StartupAllAPs(),
  and whether timeout expires.

  @retval  EFI_SUCCESS      All APs have finished task assigned by StartupAllAPs().
  @retval  EFI_TIMEOUT      The timeout expires.
  @retval  EFI_NOT_READY    APs have not finished task and timeout has not expired.

**/
EFI_STATUS
CheckAllAps (
  VOID
  );

/**
  Checks status of specified AP.

  This function checks whether specified AP has finished task assigned by StartupThisAP(),
  and whether timeout expires.

  @param[in] ProcessorNumber   The handle number of processor.

  @retval    EFI_SUCCESS       Specified AP has finished task assigned by StartupThisAPs().
  @retval    EFI_TIMEOUT       The timeout expires.
  @retval    EFI_NOT_READY     Specified AP has not finished task and timeout has not expired.

**/
EFI_STATUS
CheckThisAp (
  IN UINTN ProcessorNumber
  );

/**
  Searches for the next waiting AP.

  Search for the next AP that is put in waiting state by single-threaded StartupAllAPs().

  @param[out]  NextProcessorNumber    Pointer to the processor number of the next waiting AP.

  @retval      EFI_SUCCESS            The next waiting AP has been found.
  @retval      EFI_NOT_FOUND          No waiting AP exists.

**/
EFI_STATUS
GetNextWaitingProcessorNumber (
  OUT UINTN *NextProcessorNumber
  );

/**
  Abort any task on the AP and reset the AP to be in idle state.

  @param[in]  ProcessorNumber     Processor index of an AP.

**/
VOID
ResetProcessorToIdleState (
  IN UINTN ProcessorNumber
  );
#endif

