/** @file
  Definitions for MP and HT driver.

  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MP_SERVICE_LIB_
#define _MP_SERVICE_LIB_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/HobLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/CpuPolicy.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CpuPlatformLib.h>
#include <Private/Library/CpuCommonLib.h>
#include <CpuAccess.h>
#include <Private/PowerMgmtNvsStruct.h>

#define INTERRUPT_HANDLER_DIVIDE_ZERO           0x00
#define INTERRUPT_HANDLER_DEBUG                 0x01
#define INTERRUPT_HANDLER_NMI                   0x02
#define INTERRUPT_HANDLER_BREAKPOINT            0x03
#define INTERRUPT_HANDLER_OVERFLOW              0x04
#define INTERRUPT_HANDLER_BOUND                 0x05
#define INTERRUPT_HANDLER_INVALID_OPCODE        0x06
#define INTERRUPT_HANDLER_DEVICE_NOT_AVAILABLE  0x07
#define INTERRUPT_HANDLER_DOUBLE_FAULT          0x08
#define INTERRUPT_HANDLER_COPROCESSOR_OVERRUN   0x09
#define INTERRUPT_HANDLER_INVALID_TSS           0x0A
#define INTERRUPT_HANDLER_SEGMENT_NOT_PRESENT   0x0B
#define INTERRUPT_HANDLER_STACK_SEGMENT_FAULT   0x0C
#define INTERRUPT_HANDLER_GP_FAULT              0x0D
#define INTERRUPT_HANDLER_PAGE_FAULT            0x0E
#define INTERRUPT_HANDLER_RESERVED              0x0F
#define INTERRUPT_HANDLER_MATH_FAULT            0x10
#define INTERRUPT_HANDLER_ALIGNMENT_FAULT       0x11
#define INTERRUPT_HANDLER_MACHINE_CHECK         0x12
#define INTERRUPT_HANDLER_STREAMING_SIMD        0x13
#define INTERRUPT_VECTOR_NUMBER                 256
#define RENDEZVOUS_PROC_LENGTH                  0x1000
#define MAX_CPU_S3_MTRR_ENTRY                   0x0020
#define MAX_CPU_S3_TABLE_SIZE                   0x0400
#define VacantFlag                              0x00
#define NotVacantFlag                           0xff
#define MICROSECOND                             10
#define MAXIMUM_CPU_NUMBER                      0x40
#define STACK_SIZE_PER_PROC                     0x8000
#define MAXIMUM_CPU_S3_TABLE_SIZE               0x1000
#define MP_CPU_EXCHANGE_INFO_OFFSET             (0x1000 - 0x400)
#define PSIX_THRESHOLD_MASK                     0x3FFFFFFF  ///< Bits 61:32 - Mask value respect to Dword.High
#define EFI_MP_HEALTH_FLAGS_STATUS_HEALTHY                  0x0
#define EFI_MP_HEALTH_FLAGS_STATUS_PERFORMANCE_RESTRICTED   0x1
#define EFI_MP_HEALTH_FLAGS_STATUS_FUNCTIONALLY_RESTRICTED  0x2

///
///  The MP data structure follows.
///
#define CPU_SWITCH_STATE_IDLE   0
#define CPU_SWITCH_STATE_STORED 1
#define CPU_SWITCH_STATE_LOADED 2

extern UINTN FixedMtrrNumber;
extern UINTN MtrrDefTypeNumber;
extern SI_CPU_POLICY_PPI      *mSiCpuPolicyPpi;
extern CPU_CONFIG_PREMEM      *mCpuConfigPreMem;
extern CPU_CONFIG             *mCpuConfig;

#pragma pack(1)
typedef struct {
  UINT16  Offset15To0;
  UINT16  SegmentSelector;
  UINT16  Attributes;
  UINT16  Offset31To16;
  } INTERRUPT_GATE_DESCRIPTOR;

#define SIZE_OF_MCE_HANDLER 16

typedef struct {
  UINT16 LimitLow;
  UINT16 BaseLow;
  UINT8  BaseMiddle;
  UINT16 Attributes;
  UINT8  BaseHigh;
} SEGMENT_DESCRIPTOR;
#pragma pack()

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

typedef enum {
  CPU_STATE_IDLE,
  CPU_STATE_BLOCKED,
  CPU_STATE_READY,
  CPU_STATE_BUSY,
  CPU_STATE_FINISHED,
  CPU_STATE_DISABLED
} CPU_STATE;

typedef struct {
  UINT16 Index;
  UINT64 Value;
} EFI_MTRR_VALUES;

typedef struct {
  UINT32 ApicId;
  UINT32 MsrIndex;
  UINT64 MsrValue;
} MP_CPU_S3_SCRIPT_DATA;

typedef struct {
  UINT32 S3BootScriptTable;
  UINT32 S3BspMtrrTable;
  UINT32 VirtualWireMode;
} MP_CPU_S3_DATA_POINTER;

typedef struct {
  UINT32 Number;
  UINT32 BIST;
} BIST_INFO;

typedef union {
  struct {
    UINT32 Status                   : 2;
    UINT32 Tested                   : 1;
    UINT32 Reserved1                : 13;
    UINT32 VirtualMemoryUnavailable : 1;
    UINT32 Ia32ExecutionUnavailable : 1;
    UINT32 FloatingPointUnavailable : 1;
    UINT32 MiscFeaturesUnavailable  : 1;
    UINT32 Reserved2                : 12;
  } Bits;
  UINT32 Uint32;
} EFI_MP_HEALTH_FLAGS;

typedef struct {
  EFI_MP_HEALTH_FLAGS Flags;
  UINT32              TestStatus;
} EFI_MP_HEALTH;

typedef enum {
  EfiCpuAP                = 0,
  EfiCpuBSP,
  EfiCpuDesignationMaximum
} EFI_CPU_DESIGNATION;

typedef struct {
  UINT32              ApicID;
  BOOLEAN             Enabled;
  EFI_CPU_DESIGNATION Designation;
  EFI_MP_HEALTH       Health;
  UINTN               PackageNumber;
  UINTN               NumberOfCores;
  UINTN               NumberOfThreads;
  UINT64              ProcessorPALCompatibilityFlags;
  UINT64              ProcessorTestMask;
} EFI_MP_PROC_CONTEXT;

/**
  Functions of this type are used with the MP Services to execute a procedure on enabled APs.
  The context the AP should use during execution is specified by Buffer.

  @param[in]  Buffer   The pointer to the procedure's argument.

**/
typedef
VOID
(EFIAPI *EFI_AP_PROCEDURE) (
  IN  VOID  *Buffer
  );

///
/// Structure that describes the physical location of a logical CPU.
///
typedef struct {
  ///
  /// Zero-based physical package number that identifies the cartridge of the processor.
  ///
  UINT32 Package;
  ///
  /// Zero-based physical core number within package of the processor.
  ///
  UINT32 Core;
  ///
  /// Zero-based logical thread number within core of the processor.
  ///
  UINT32 Thread;
} EFI_CPU_PHYSICAL_LOCATION;

///
/// Define Individual Processor Data block.
///
typedef struct {
  UINT32                    ApicID;
  EFI_AP_PROCEDURE          Procedure;
  VOID                      *Parameter;
  UINT8                     StateLock;
  UINT8                     ProcedureLock;
  EFI_MP_HEALTH_FLAGS       Health;
  BOOLEAN                   SecondaryCpu;
  EFI_CPU_PHYSICAL_LOCATION PhysicalLocation;
  CPU_STATE                 State;
} CPU_DATA_BLOCK;

#define MP_SERVICES_DATA_SIGNATURE  SIGNATURE_32 ('c', 'p', 'u', 'm')

typedef struct {
  UINT8             Lock;         ///< offset 0
  UINT8             State;        ///< offset 1
  UINTN             StackPointer; ///< offset 4 / 8
  IA32_DESCRIPTOR   Gdtr;         ///< offset 8 / 16
  IA32_DESCRIPTOR   Idtr;         ///< offset 14 / 26
} CPU_EXCHANGE_ROLE_INFO;

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
  BOOLEAN                HyperThreadingEnable;
  UINT8                  ProcTraceMemSize;
  UINT8                  ProcTraceOutputScheme;
  BOOLEAN                ProcTraceEnable;
  UINTN                  NumberOfCpus;
  UINTN                  MaximumCpusForThisSystem;
  UINTN                  BSP;
  CPU_DATA_BLOCK         CpuData[MAXIMUM_CPU_NUMBER];
  CPU_STATE_CHANGE_CAUSE DisableCause[MAXIMUM_CPU_NUMBER];
  UINT8                  S3BootScriptLock;
  UINT32                 S3BootScriptCount;
  MP_CPU_S3_DATA_POINTER S3DataPointer;
  MP_CPU_S3_SCRIPT_DATA  S3BootScriptTable[MAX_CPU_S3_TABLE_SIZE];
  EFI_MTRR_VALUES        S3BspMtrrTable[MAX_CPU_S3_MTRR_ENTRY];
  UINT8                  FeatureLock;
  UINTN                  CommonFeatures;
  UINTN                  SetupFeatures;
  UINT8                  Lock;
  LEAST_FEATURE_PROC     LeastFeatureProcessor;
  CPU_EXCHANGE_ROLE_INFO BspInfo;
  CPU_EXCHANGE_ROLE_INFO ApInfo;
} MP_SYSTEM_DATA;

///
/// Data structure for MP framework
///
typedef struct {
  UINTN                  Signature;
  CONST EFI_PEI_SERVICES **PeiServices;
  EFI_PEI_PPI_DESCRIPTOR PpiDescriptor;
  ACPI_CPU_DATA          AcpiCpuData;
  MP_SYSTEM_DATA         MpSystemData;
  IA32_DESCRIPTOR        GdtrProfile;
  IA32_DESCRIPTOR        IdtrProfile;
  UINT64                 MtrrValues[MAX_CPU_S3_MTRR_ENTRY];
  CPU_CONFIG             CpuConfig;
  POWER_MGMT_CONFIG      PowerMgmtConfig;
  FVID_TABLE             FvidTable[FVID_MAX_STATES + 1];
} MP_CPU_RUNTIME_DATA;

typedef struct {
  UINT32                       Lock;
  UINT32                       StackStart;
  UINT32                       StackSize;
  UINT32                       ApFunction;
  IA32_DESCRIPTOR              GdtrProfile;
  IA32_DESCRIPTOR              IdtrProfile;
  UINT32                       BufferStart;
  UINT32                       PmodeOffset;
  UINT32                       AcpiCpuDataAddress;
  UINT32                       MtrrValuesAddress;
  UINT32                       FinishedCount;
  UINT32                       WakeupCount;
  UINT32                       SerializeLock;
  MP_CPU_S3_SCRIPT_DATA        *S3BootScriptTable;
  UINT32                       StartState;
  UINT32                       VirtualWireMode;
  VOID                         (*SemaphoreCheck) (UINT32 *SemaphoreAddress);
  UINT32                       McuLoadCount;
  IN CONST EFI_PEI_SERVICES    **PeiServices;
  UINT64                       CpuPerfCtrlValue;
  SI_CPU_POLICY_PPI            *SiCpuPolicyPpi;
  UINT32                       MpSystemDataAddress;
  UINT32                       MpServicePpiAddress;
  UINT32                       ApArgument;
  BIST_INFO                    BistBuffer[MAXIMUM_CPU_NUMBER];
} MP_CPU_EXCHANGE_INFO;

/**
  Get CPU platform features settings to fill MP data.

  @retval  MP_SYSTEM_DATA*    Return MpSystemData pointer

**/
MP_SYSTEM_DATA *
EFIAPI
GetMpSystemData (
  VOID
  );

/**
  Initialize multi-processor service.

  @param[in] SiCpuPolicyPpi The Cpu Policy PPI instance

  @retval  EFI_SUCCESS     Multi-processor initialization has been done successfully

**/
EFI_STATUS
InitializeMpServices (
  IN  SI_CPU_POLICY_PPI    *SiCpuPolicyPpi
  );

/**
  Initialize CPU Data Hob

  @retval  EFI_SUCCESS     The driver installs/initialized correctly.

**/
EFI_STATUS
InitializeCpuDataHob (
   VOID
   );

/**
  Check if this is non-core processor - HT AP thread

  @retval  TRUE       If this is HT AP thread
  @retval  FALSE      If this is core thread

**/
BOOLEAN
EFIAPI
IsSecondaryThread (
  VOID
  );

/**
  Get protected mode code offset

  @retval   Offset of protected mode code

**/
VOID *
AsmGetPmodeOffset (
  VOID
  );

/**
  Get code offset of SemaphoreCheck

  @retval Offset of SemaphoreCheck

**/
UINT32
AsmGetSemaphoreCheckOffset (
  VOID
  );

/**
  Read MTRR settings

  @param[in]   MtrrValues      Buffer to store MTRR settings

**/
VOID
ReadMtrrRegisters (
  UINT64              *MtrrValues
  );

/**
  Syncup MTRR settings between all processors

  @param[in]   MtrrValues      Buffer to store MTRR settings

**/
VOID
MpMtrrSynchUp (
  UINT64 *MtrrValues
  );

/**
  Set MTRR registers

  @param[in]  MtrrArray      Buffer with MTRR settings

**/
VOID
SetMtrrRegisters (
  IN EFI_MTRR_VALUES  *MtrrArray
  );

/**
  This will check if the microcode address is valid for this processor, and if so, it will
  load it to the processor.

  @param[in]  ExchangeInfo          Pointer to the exchange info buffer for output.
  @param[in]  MicrocodeAddress      The address of the microcode update binary (in memory).
  @param[out] FailedRevision        The microcode revision that fails to be loaded.

  @retval     EFI_SUCCESS           A new microcode update is loaded.
  @retval     Other                 Due to some reason, no new microcode update is loaded.

**/
EFI_STATUS
InitializeMicrocode (
  IN  MP_CPU_EXCHANGE_INFO *ExchangeInfo,
  IN  CPU_MICROCODE_HEADER *MicrocodeAddress,
  OUT UINT32               *FailedRevision
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
  IN UINT32           BroadcastMode,
  IN UINT32           ApicID,
  IN UINT32           VectorNumber,
  IN UINT32           DeliveryMode,
  IN UINT32           TriggerMode,
  IN BOOLEAN          Assert
  );

/**
  Lock APs

  @param[in]  Lock          Lock state

**/
VOID
AsmAcquireMPLock (
  IN UINT8 *Lock
  );

/**
  Release APs

  @param[in]  Lock          Lock state

**/
VOID
AsmReleaseMPLock (
  IN UINT8 *Lock
  );

/**
  Get address map of RendezvousFunnelProc.

  @retval  AddressMap       Output buffer for address map information

**/
VOID *
AsmGetAddressMap (
  VOID
  );

/**
  Patch the Rendezvous Code properly.

  @param[in]  RendezvousCodeBase    Rendezvous code base in memory

**/
VOID
AsmPatchRendezvousCode (
  VOID *RendezvousCodeBase
  );

/**
  Wake up all the application processors

  @param[in]  MtrrValues    Pointer to a buffer which stored MTRR settings
  @param[in]  Function      Pointer to AP Procedure Function

  @retval     EFI_SUCCESS   APs are successfully waked up

**/
EFI_STATUS
WakeUpAPs (
  UINT64               *MtrrValues,
  EFI_AP_PROCEDURE     Function
  );


/**
  Based on ResetType, perform warm or cold reset using PCH reset PPI.

  @param[in] ResetType         CPU_RESET_TYPE to indicate which reset should be performed.

  @retval    EFI_SUCCESS       Function successful (system should already reset).
  @retval    EFI_UNSUPPORTED   Reset type unsupported.

**/
EFI_STATUS
PerformWarmOrColdReset (
  IN CPU_RESET_TYPE ResetType
  );

/**
  Initialize performance and power management features before RESET_CPL at Post-memory phase.

  @param[in]  CpuPolicyPpi       The Cpu Policy PPI instance

**/
VOID
CpuInitPreResetCpl (
  IN SI_CPU_POLICY_PPI        *CpuPolicyPpi
  );

/**
  Get general MP information

  @param[in]  NumberOfCpus            Number of processors
  @param[in]  MaxiumNumberOfCpus      Max supported number of processors
  @param[in]  NumberOfEnabledCpus     Number of processors enabled
  @param[in]  RendezvousIntNumber     Number of Rendezvous procedure
  @param[in]  RendezvousProcLength    Length of Rendezvous procedure

  @retval     EFI_SUCCESS             Always return success
**/
EFI_STATUS
EFIAPI
GetGeneralMpInfo (
  OUT UINTN                   *NumberOfCpus,
  OUT UINTN                   *MaxiumNumberOfCpus,
  OUT UINTN                   *NumberOfEnabledCpus,
  OUT UINTN                   *RendezvousIntNumber,
  OUT UINTN                   *RendezvousProcLength
  );

/**
  Get processor context

  @param[in]  CpuNumber               The handle number of processor.
  @param[in]  BufferLength            Buffer length
  @param[in]  ProcessorContextBuffer  Pointer to the buffer that will be updated

  @retval     EFI_INVALID_PARAMETER   Buffer is NULL or CpuNumber our of range
  @retval     EFI_BUFFER_TOO_SMALL    Buffer too small
  @retval     EFI_SUCCESS             Got processor context successfully

**/
EFI_STATUS
EFIAPI
GetProcessorContext (
  IN UINTN                    CpuNumber,
  IN OUT UINTN                *BufferLength,
  IN OUT EFI_MP_PROC_CONTEXT  *ProcessorContextBuffer
  );

/**
  MP Service to get specified application processor (AP)
  to execute a caller-provided code stream.

  @param[in]  Procedure               The procedure to be assigned to AP.
  @param[in]  ProcessorNumber         The handle number of processor.
  @param[in]  WaitEvent               If timeout, the event to be triggered after this AP finishes.
  @param[in]  TimeoutInMicroSecs      The timeout value in microsecond. Zero means infinity.
  @param[in]  ProcArguments           Argument for Procedure.

  @retval     EFI_INVALID_PARAMETER   Procedure is NULL.
  @retval     EFI_INVALID_PARAMETER   Number of CPU out of range, or it belongs to BSP.
  @retval     EFI_INVALID_PARAMETER   Specified CPU is not idle.
  @retval     EFI_SUCCESS             The AP has finished.
  @retval     EFI_TIMEOUT             Time goes out before the AP has finished.

**/
EFI_STATUS
EFIAPI
StartupThisAP (
  IN EFI_AP_PROCEDURE           Procedure,
  IN UINTN                      ProcessorNumber,
  IN OUT VOID *ProcArguments    OPTIONAL
  );

/**
  MP Service to get all the available application processors (APs)
  to execute a caller-provided code stream.

  @param[in]  Function                The procedure to be assigned to APs.
  @param[in]  ProcArguments           Argument for Procedure.

  @retval     EFI_INVALID_PARAMETER   Procedure is NULL.
  @retval     EFI_SUCCESS             Only 1 logical processor exists.
  @retval     EFI_SUCCESS             All APs have finished.
  @retval     EFI_TIMEOUT             Time goes out before all APs have finished.

**/

EFI_STATUS
StartupAllAps (
  IN EFI_AP_PROCEDURE         Function,
  IN OUT VOID *ProcArguments  OPTIONAL
  );

/**
    MP Service to makes the current BSP into an AP and then switches the
    designated AP into the AP. This procedure is usually called after a CPU
    test that has found that BSP is not healthy to continue it's responsibilities.

    @param[in]  CpuNumber               The handle number of processor.
    @param[in]  EnableOldBsp            Whether to enable or disable the original BSP.

    @retval     EFI_INVALID_PARAMETER   Number for Specified AP out of range.
    @retval     EFI_INVALID_PARAMETER   Number of specified CPU belongs to BSP.
    @retval     EFI_NOT_READY           Specified AP is not idle.
    @retval     EFI_SUCCESS             BSP successfully switched.

**/
EFI_STATUS
EFIAPI
SwitchBsp (
  IN UINTN                    CpuNumber,
  IN BOOLEAN                  EnableOldBsp
  );

/**
  Implementation of EnableDisableAp() service of MP Services Protocol.

  This service lets the caller enable or disable an AP.
  This service may only be called from the BSP.

  @param[in]  CpuNumber                 The handle number of processor.
  @param[in]  NewAPState                Indicates whether the newstate of the AP is enabled or disabled.
  @param[in]  HealthState               Indicates new health state of the AP..

  @retval     EFI_SUCCESS               AP successfully enabled or disabled.
  @retval     EFI_DEVICE_ERROR          Caller processor is AP.
  @retval     EFI_NOT_FOUND             Processor with the handle specified by ProcessorNumber does not exist.
  @retval     EFI_INVALID_PARAMETERS    ProcessorNumber specifies the BSP.

**/
EFI_STATUS
EFIAPI
EnableDisableAp (
  IN UINTN                      CpuNumber,
  IN BOOLEAN                    NewApState,
  IN EFI_MP_HEALTH *HealthState OPTIONAL
  );

/**
  Implementation of WhoAmI() service of MP Services Protocol.

  This service lets the caller processor get its handle number.
  This service may be called from the BSP and APs.

  @param[in]  ProcessorNumber          Pointer to the handle number of AP.

  @retval     EFI_SUCCESS              Processor number successfully returned.
  @retval     EFI_INVALID_PARAMETER    ProcessorNumber is NULL

**/
EFI_STATUS
EFIAPI
WhoAmI (
 OUT UINTN                   *ProcessorNumber
  );

/**
  AP initialization

**/
VOID
InitialMpProcedure (
  VOID
  );

/**
  Write 64bits MSR with script

  @param[in]  Index       MSR index that will be written
  @param[in]  Value       Value written to MSR

**/
VOID
AsmWriteMsr64WithScript (
  IN UINT32 Index,
  IN UINT64 Value
  );

/**
  Write 64bits MSR to script

  @param[in]  Index      MSR index that will be written
  @param[in]  Value      Value written to MSR

**/
VOID
WriteMsr64ToScript (
  IN UINT32 Index,
  IN UINT64 Value
  );

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
  IN UINT32            BroadcastMode,
  IN UINT32            ApicID,
  IN UINT32            VectorNumber,
  IN UINT32            DeliveryMode,
  IN UINT32            TriggerMode,
  IN BOOLEAN           Assert
  );

/**
  Set APIC BSP bit

  @param[in]  Enable          Enable as BSP or not

  @retval     EFI_SUCCESS     Always return success

**/
EFI_STATUS
SetApicBspBit (
  IN BOOLEAN Enable
  );

/**
  Switch current BSP processor to AP

  @param[in]  MPSystemData    Pointer to the data structure containing MP related data

**/
VOID
EFIAPI
FutureBspProc (
  VOID
  );

/**
  Change CPU state

  @param[in]  CpuNumber       CPU number
  @param[in]  NewState        The new state that will be changed to
  @param[in]  Cause           Cause

  @retval     EFI_SUCCESS     Always return success

**/
EFI_STATUS
ChangeCpuState (
  IN UINTN                  CpuNumber,
  IN BOOLEAN                NewState,
  IN CPU_STATE_CHANGE_CAUSE Cause
  );

#ifdef EFI_DEBUG
/**
  Print MTRR settings in debug build BIOS

  @param[in]  MtrrArray         Buffer with MTRR settings

**/
VOID
ShowMtrrRegisters (
  IN EFI_MTRR_VALUES  *MtrrArray
  );
#endif
#endif

