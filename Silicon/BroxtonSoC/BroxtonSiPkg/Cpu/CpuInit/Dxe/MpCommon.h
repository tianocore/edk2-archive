/** @file
  Some definitions for MP and HT driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MP_COMMON_
#define _MP_COMMON_

#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include "CpuInitDxe.h"
#include "Exception.h"
#include "ProcessorDef.h"
#include "ScAccess.h"
#include <Private/CpuInitDataHob.h>

#include <Protocol/MpService.h>

#define VacantFlag          0x00
#define NotVacantFlag       0xff
#define MICROSECOND         10
#define MAXIMUM_CPU_NUMBER  0x40
#define STACK_SIZE_PER_PROC 0x8000

///
/// Data structure used in MP/HT driver
///
#define MP_CPU_EXCHANGE_INFO_OFFSET     (0x1000 - 0x400)
#define MP_CPU_LEGACY_RESET_INFO_OFFSET (0x100 - 0x20)

#define SMM_FROM_CPU_DRIVER_SAVE_INFO   0x81

#pragma pack(1)
#define SIZE_OF_MCE_HANDLER 16

typedef struct {
  UINT16 LimitLow;
  UINT16 BaseLow;
  UINT8  BaseMiddle;
  UINT16 Attributes;
  UINT8  BaseHigh;
} SEGMENT_DESCRIPTOR;

#pragma pack()

#define BREAK_TO_RUN_AP_SIGNAL  0x6E755200
#define MONITOR_FILTER_SIZE     0x40

typedef enum {
  WakeUpApCounterInit   = 0,
  WakeUpApPerHltLoop    = 1,
  WakeUpApPerMwaitLoop  = 2,
  WakeUpApPerRunLoop    = 3,
  WakeUpApPerMwaitLoop32= 4,
  WakeUpApPerRunLoop32  = 5
} WAKEUP_AP_MANNER;

typedef struct {
  UINTN BreakToRunApSignal;
  UINTN HltLoopBreakCounter;
  UINTN MwaitLoopBreakCounter;
  UINTN RunLoopBreakCounter;
  UINTN MwaitLoopBreakCounter32;
  UINTN RunLoopBreakCounter32;
  UINTN WakeUpApVectorChangeFlag;
  UINTN MwaitTargetCstate;
} MONITOR_MWAIT_DATA;

typedef struct {
  UINT32 Number;
  UINT32 BIST;
} BIST_INFO;

typedef struct {
  UINTN             Lock;
  VOID              *StackStart;
  UINTN             StackSize;
  VOID              *ApFunction;
  IA32_DESCRIPTOR   GdtrProfile;
  IA32_DESCRIPTOR   IdtrProfile;
  UINT32            BufferStart;
  UINT32            Cr3;
  UINT32            InitFlag;
  WAKEUP_AP_MANNER  WakeUpApManner;
  BIST_INFO         BistBuffer[MAXIMUM_CPU_NUMBER];
} MP_CPU_EXCHANGE_INFO;

extern ACPI_CPU_DATA *mAcpiCpuData;

//
// Protocol interface functions
//
/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  This service retrieves the number of logical processor in the platform
  and the number of those logical processors that are enabled on this boot.
  This service may only be called from the BSP.

  @param[in]  This                         A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[out] NumberOfProcessors           Pointer to the total number of logical processors in the system,
                                           including the BSP and disabled APs.
  @param[out] NumberOfEnabledProcessors    Pointer to the number of enabled logical processors that exist
                                           in system, including the BSP.

  @retval     EFI_SUCCESS                  Number of logical processors and enabled logical processors retrieved..
  @retval     EFI_DEVICE_ERROR             Caller processor is AP.
  @retval     EFI_INVALID_PARAMETER        NumberOfProcessors is NULL.
  @retval     EFI_INVALID_PARAMETER        NumberOfEnabledProcessors is NULL.

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
  @param[in]  ProcessorNumber         Cpu number
  @param[in]  WaitEvent               If timeout, the event to be triggered after this AP finishes.
  @param[in]  TimeoutInMicroSecs      The timeout value in microsecond. Zero means infinity.
  @param[out] ProcArguments           Argument for Procedure.

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

  @param[in]  This                   Pointer to MP Service Protocol
  @param[in]  Procedure              The procedure to be assigned to APs.
  @param[in]  SingleThread           If true, all APs execute in block mode.
                                     Otherwise, all APs exceute in non-block mode.
  @param[in]  WaitEvent              If timeout, the event to be triggered after all APs finish.
  @param[in]  TimeoutInMicroSecs     The timeout value in microsecond. Zero means infinity.
  @param[in]  ProcArguments          Argument for Procedure.
  @param[out] FailedCPUList          If not NULL, all APs that fail to start will be recorded in the list.

  @retval     EFI_INVALID_PARAMETER  Procudure is NULL.
  @retval     EFI_SUCCESS            Only 1 logical processor exists.
  @retval     EFI_SUCCESS            All APs have finished.
  @retval     EFI_TIMEOUT            Time goes out before all APs have finished.

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
  @param[in] OldBSPState             Whether to enable or disable the original BSP.

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
  IN BOOLEAN                  OldBSPState
  );

/**
  This procedure enables or disables APs.

  @param[in] This                    Pointer to MP Service Protocol.
  @param[in] ProcessorNumber         The handle number of processor.
  @param[in] NewAPState              Indicate new desired AP state
  @param[in] HealthState             If not NULL, it points to the value that specifies the new health status of the AP.
                                     If it is NULL, this parameter is ignored.

  @retval    EFI_INVALID_PARAMETER   Input paramters were not correct.
  @retval    EFI_SUCCESS             Function completed successfully

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
  Implementation of WhoAmI() service of MP Services Protocol.

  This service lets the caller processor get its handle number.
  This service may be called from the BSP and APs.

  @param[in]  This                     A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[out] ProcessorNumber          Pointer to the handle number of AP.

  @retval     EFI_SUCCESS              Processor number successfully returned.
  @retval     EFI_INVALID_PARAMETER    ProcessorNumber is NULL

**/
EFI_STATUS
EFIAPI
WhoAmI (
  IN  EFI_MP_SERVICES_PROTOCOL *This,
  OUT UINTN                   *ProcessorNumber
  );

///
/// Functions shared in MP/HT drivers
///
/**
  Send interrupt to CPU

  @param[in]  BroadcastMode            Interrupt broadcast mode
  @param[in]  ApicID                   APIC ID for sending interrupt
  @param[in]  VectorNumber             Vector number
  @param[in]  DeliveryMode             Interrupt delivery mode
  @param[in]  TriggerMode              Interrupt trigger mode
  @param[in]  Assert                   Interrupt pin polarity

  @retval     EFI_INVALID_PARAMETER    Input parameter not correct
  @retval     EFI_NOT_READY            There was a pending interrupt
  @retval     EFI_SUCCESS              Interrupt sent successfully

**/
EFI_STATUS
SendInterrupt (
  IN UINT32  BroadcastMode,
  IN UINT32  ApicID,
  IN UINT32  VectorNumber,
  IN UINT32  DeliveryMode,
  IN UINT32  TriggerMode,
  IN BOOLEAN Assert
  );

/**
  Programs XAPIC registers.

  @param[in]  Bsp             Is this BSP

**/
VOID
ProgramCpuXApic (
  IN BOOLEAN Bsp
  );

/**
  Allocate a temporary memory under 1MB for MP Init to perform INIT-SIPI.
  This buffer also provides memory for stack/data for MP running.

  @param[out]  WakeUpBuffer      Return buffer location

  @retval      EFI_SUCCESS       If ok to get a memory under 1MB for MP running.

**/
EFI_STATUS
AllocateWakeUpBuffer (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer
  );

///
/// Assembly functions implemented in MP/HT drivers
///
/**
  Lock APs

  @param[in]  Lock     Lock state

**/
VOID
AsmAcquireMPLock (
  IN UINT8 *Lock
  );

/**
  Release APs

  @param[in]  Lock     Lock state

**/
VOID
AsmReleaseMPLock (
  IN UINT8 *Lock
  );

/**
  Prepare GDTR and IDTR for AP

  @param[out]  GDTR          The GDTR profile
  @param[out]  IDTR          The IDTR profile

  @retval      EFI_STATUS    Status returned by each sub-routine
  @retval      EFI_SUCCESS   GDTR and IDTR has been prepared for AP

**/
EFI_STATUS
PrepareGdtIdtForAP (
  OUT IA32_DESCRIPTOR *GDTR,
  OUT IA32_DESCRIPTOR *IDTR
  );

/**
  Allocate Reserved Memory

  @param[in]  Size           Memory Size
  @param[in]  Alignment      Alignment size
  @param[out] Pointer        Return memory location

  @retval     EFI_SUCCESS    Allocate a reserved memory successfully

**/
EFI_STATUS
AllocateAlignedReservedMemory (
  IN UINTN Size,
  IN UINTN Alignment,
  OUT VOID **Pointer
  );

/**
  This function is invoked when LegacyBios protocol is installed, we must
  allocate reserved memory under 1M for AP.

  @param[in]  Event     The triggered event.
  @param[in]  Context   Context for this event.

**/
VOID
EFIAPI
ReAllocateEbdaMemoryForAP (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  This function is invoked when LegacyBios protocol is installed, we must
  allocate reserved memory under 1M for AP.

  @param[in]  Event     The triggered event.
  @param[in]  Context   Context for this event.

**/
VOID
EFIAPI
ReAllocateMemoryForAP (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  This function is invoked by EFI_EVENT_SIGNAL_LEGACY_BOOT.
  Before booting to legacy OS, reset it with memory allocated
  by ReAllocateMemoryForAp() and set local APIC correctly.

  @param[in]  Event     The triggered event.
  @param[in]  Context   Context for this event.

**/
VOID
EFIAPI
ResetAps (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  Prepare Wakeup Buffer and stack for APs.

  @param[out] WakeUpBuffer               Pointer to the address of wakeup buffer for output.
  @param[out] StackAddressStart          Pointer to the stack address of APs for output.
  @param[in]  MaximumCPUsForThisSystem   Maximum CPUs in this system.

  @retval     EFI_SUCCESS                Memory successfully prepared for APs.
  @retval     Other                      Error occurred while allocating memory.

**/
EFI_STATUS
PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS *WakeUpBuffer,
  OUT VOID                 **StackAddressStart,
  IN UINTN                 MaximumCPUsForThisSystem
  );

/**
  Prepare exchange information for APs.

  @param[out] ExchangeInfo        Pointer to the exchange info buffer for output.
  @param[in]  StackAddressStart   Start address of APs' stacks.
  @param[in]  ApFunction          Address of function assigned to AP.
  @param[in]  WakeUpBuffer        Pointer to the address of wakeup buffer.

  @retval     EFI_SUCCESS         Exchange Info successfully prepared for APs.
  @retval     Other               Error occurred while allocating memory.

**/
EFI_STATUS
PrepareExchangeInfo (
  OUT MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  IN VOID                  *StackAddressStart,
  IN VOID                  *ApFunction,
  IN EFI_PHYSICAL_ADDRESS  WakeUpBuffer
  );

/**
  Check whether any AP is running for assigned task.

  @retval  TRUE        Some APs are running.
  @retval  FALSE       No AP is running.

**/
BOOLEAN
ApRunning (
  VOID
  );

/**
  Wrapper function for all procedures assigned to AP via MP service protocol.
  It controls states of AP and invokes assigned precedure.

**/
VOID
ApProcWrapper (
  VOID
  );

/**
  Allocate EfiReservedMemoryType below 4G memory address.

  @param[in]  Size          Size of memory to allocate.
  @param[out] Buffer        Allocated address for output.

  @retval     EFI_SUCCESS   Memory successfully allocated.
  @retval     Other         Other errors occur.

**/
EFI_STATUS
AllocateReservedMemoryBelow4G (
  IN UINTN Size,
  OUT VOID **Buffer
  );

/**
  Dynamically write the far jump destination in APs' wakeup buffer,
  in order to refresh APs' CS registers for mode switching.

**/
VOID
RedirectFarJump (
  VOID
  );

#endif

