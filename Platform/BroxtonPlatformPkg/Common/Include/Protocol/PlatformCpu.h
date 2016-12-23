/** @file
  Protocol used for specifying platform related CPU information and policy setting.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PLATFORM_CPU_PROTOCOL_H_
#define _PLATFORM_CPU_PROTOCOL_H_

#include <Guid/DataHubRecords.h>
#include <Protocol/MpService.h>
#include <Protocol/FrameworkMpService.h>

typedef struct _EFI_PLATFORM_CPU_PROTOCOL EFI_PLATFORM_CPU_PROTOCOL;

#define EFI_PLATFORM_CPU_PROTOCOL_GUID \
  { \
    0xbd26cdc9, 0xa092, 0x462a, 0x87, 0x7a, 0x5a, 0xb6, 0xad, 0xce, 0x48, 0x12 \
  }

//
// Prototypes for the Platform CPU Protocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_RETRIEVE_MICROCODE) (
  IN  EFI_PLATFORM_CPU_PROTOCOL             * This,
  OUT UINT8                                 **MicrocodeData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_GET_TM2_CONTROL_INFO) (
  IN  EFI_PLATFORM_CPU_PROTOCOL             * This,
  OUT UINT8                                 *Tm2Core2BusRatio,
  OUT UINT8                                 *Tm2Vid
  );

//
// The platform capabilities for enabling must be same as
// disabling.
//
typedef UINT32  EFI_CPU_STATE_CHANGE_CAPABILITIES;

//
// The platform supports a hardware mechanism to
// enable and disable the processor.
// A CPU that is disabled in HW becomes 'invisible' to
// the software and does not consume any system resources, such as
// RAM or ROM. This CPU does not respond to any bus
// cycles including Inter processor Interrupts
// The processor state is persistent. If the
// hardware underneath does not support persistency, the
// platform CPU driver will simulate that behavior.
//
#define EFI_CPU_HW_EN_DIS_SUPPORTED 0x0001

//
// Meaningful if EFI_CPU_HW_ EN_DIS_SUPPORTED is set.
// Set if the platform supports a harware mechanism to
// enable and disable the processor in this boot (without a reset)
// If this flag is clear, all state changes take effect on the
// next boot and the call to EnableDisableCpu ( )results in
// a system reset
//
#define EFI_CPU_HW_EN_DIS_THIS_BOOT 0x0002

//
// TBD, capabilities with regards to hot remove/addition of CPUs
//
//
// The minimum granularity of enable/disable operations
// EfiCpuControlThreadGranularity -Can enable/disable one thread at a // time. E.g. if a CPU core has two threads, one can be enabled
// while the other is disabled.
// EfiCpuControlCoreGranularity -Can enable/disable one core at a
// time. E.g. if a CPU package has two cores, one core can be
// enabled while the other is disabled.
// enabling or disabling CPUs.
// EfiCpuControlPackageGranularity -Can enable/disable one package at // a time
//
typedef enum {
  EfiCpuControlThreadGranularity,
  EfiCpuControlCoreGranularity,
  EfiCpuControlDieGranularity,
  EfiCpuControlPackageGranularity
} EFI_CPU_STATE_CHANGE_GRANULARITY;

//
// The state of the CPU
// EfiCpuNotKnown - Only used if the platform plays no role in
// enabling or disabling CPUs.
// EfiCpuAbsent - CPU is physically absent
// EfiCpuPresentDisabled - CPU is physically present, but has been
// disabled by the platform
//
typedef enum {
  EfiCpuStateNotKnown,
  EfiCpuAbsent,
  EfiCpuPresentDisabled,
  EfiCpuPresentEnabled
} EFI_CPU_STATE;

//
// The platform category, Server, Desktop and Mobile
// are defined.
//
typedef enum {
  EfiPlatformUnknown = 0,
  EfiPlatformDesktop,
  EfiPlatformMobile,
  EfiPlatformServer,
  EfiPlatformMax
} EFI_PLATFORM_CATEGORY;
//
// The reason for changing the state of the processor
// Only applies to Disabling processors.
// In future, we can add add/remove support
//
#define EFI_CPU_CAUSE_NOT_DISABLED      0x0000
#define EFI_CPU_CAUSE_INTERNAL_ERROR    0x0001
#define EFI_CPU_CAUSE_THERMAL_ERROR     0x0002
#define EFI_CPU_CAUSE_SELFTEST_FAILURE  0x0004
#define EFI_CPU_CAUSE_PREBOOT_TIMEOUT   0x0008
#define EFI_CPU_CAUSE_FAILED_TO_START   0x0010
#define EFI_CPU_CAUSE_CONFIG_ERROR      0x0020
#define EFI_CPU_CAUSE_USER_SELECTION    0x0080
#define EFI_CPU_CAUSE_BY_ASSOCIATION    0x0100
#define EFI_CPU_CAUSE_UNSPECIFIED       0x8000

typedef UINT32  EFI_CPU_STATE_CHANGE_CAUSE;

typedef struct {
  EFI_CPU_PHYSICAL_LOCATION   Location;
  EFI_CPU_STATE               State;
  EFI_CPU_STATE_CHANGE_CAUSE  Cause;
} EFI_CPU_STATE_CHANGE_OP_LIST;

typedef struct {
  EFI_MP_PROC_CONTEXT                   *Context;
  EFI_EXP_BASE10_DATA                   CoreFreq;
  EFI_EXP_BASE10_DATA                   BusFreq;
  EFI_EXP_BASE2_DATA                    CacheSize[EFI_CACHE_LMAX];
  EFI_PROCESSOR_ID_DATA                 CpuId;
  EFI_PROCESSOR_MICROCODE_REVISION_DATA MuData;
  EFI_PROCESSOR_STATUS_DATA             Status;
} EFI_DETAILED_CPU_INFO;

typedef struct {
  UINT64                                  ApicID;
  STRING_REF                              ReferenceString;
  EFI_PROCESSOR_SOCKET_TYPE_DATA          SocketType;
  EFI_PROCESSOR_SOCKET_NAME_DATA          SocketName;
  EFI_PROCESSOR_MAX_CORE_FREQUENCY_DATA   MaxCoreFrequency;
  EFI_PROCESSOR_MAX_FSB_FREQUENCY_DATA    MaxFsbFrequency;
  EFI_PROCESSOR_CORE_FREQUENCY_LIST_DATA  PlatformCoreFrequencyList;
  EFI_PROCESSOR_FSB_FREQUENCY_LIST_DATA   PlatformFsbFrequencyList;
  STRING_REF                              AssetTag;
  EFI_HII_HANDLE                          StringHandle;
} EFI_PLATFORM_CPU_INFORMATION;

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_GET_MAX_COUNT) (
  IN  EFI_PLATFORM_CPU_PROTOCOL               * This,
  OUT UINT32                                  *MaxThreadsPerCore,
  OUT UINT32                                  *MaxCoresPerDie,
  OUT UINT32                                  *MaxDiesPerPackage,
  OUT UINT32                                  *MaxPackages
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_GET_CPU_INFO) (
  IN      EFI_PLATFORM_CPU_PROTOCOL           * This,
  IN      EFI_CPU_PHYSICAL_LOCATION           * Location,
  IN OUT  EFI_PLATFORM_CPU_INFORMATION        * PlatformCpuInfo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_STATE_CHANGE_CAP) (
  IN  EFI_PLATFORM_CPU_PROTOCOL               * This,
  OUT EFI_CPU_STATE_CHANGE_GRANULARITY        * Granularity,
  OUT EFI_CPU_STATE_CHANGE_CAPABILITIES       * Capabilities
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_GET_CPU_STATE) (
  IN   EFI_PLATFORM_CPU_PROTOCOL              * This,
  IN   EFI_CPU_PHYSICAL_LOCATION              * Location,
  OUT  EFI_CPU_STATE                          * State,
  OUT  EFI_CPU_STATE_CHANGE_CAUSE             * Cause
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_CHANGE_CPU_STATE) (
  IN  EFI_PLATFORM_CPU_PROTOCOL               * This,
  IN  BOOLEAN                                 EnableAll,
  IN  UINT32                                  Count,
  IN  EFI_CPU_STATE_CHANGE_OP_LIST            * List
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_OVERRIDE_CPU_POLICY) (
  IN  EFI_PLATFORM_CPU_PROTOCOL               * This,
  IN  UINT32                                  CpuCount,
  IN  EFI_DETAILED_CPU_INFO                   * CpuInfoList,
  IN  OUT EFI_CPU_STATE_CHANGE_CAUSE          * CpuDisableList,
  IN  OUT UINT32                              *Bsp
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_CPU_STALL) (
  IN  EFI_PLATFORM_CPU_PROTOCOL  *This,
  IN  UINTN                       Microseconds
  );

//
// Interface structure for the Platform CPU Protocol
//
typedef struct _EFI_PLATFORM_CPU_PROTOCOL {
  BOOLEAN                               HtState;
  EFI_PLATFORM_CPU_STALL                Stall;
  EFI_PLATFORM_CPU_RETRIEVE_MICROCODE   RetrieveMicrocode;
  EFI_PLATFORM_CPU_GET_TM2_CONTROL_INFO GetTm2ControlInfo;
  BOOLEAN                               EnableL3Cache;
  BOOLEAN                               LimitCpuidMaximumValue;
  EFI_PLATFORM_CPU_GET_MAX_COUNT        GetMaxCount;
  EFI_PLATFORM_CPU_GET_CPU_INFO         GetCpuInfo;
  EFI_PLATFORM_CPU_GET_CPU_STATE        GetCpuState;
  EFI_PLATFORM_CPU_STATE_CHANGE_CAP     StateChangeCap;
  EFI_PLATFORM_CPU_CHANGE_CPU_STATE     ChangeCpuState;
  EFI_PLATFORM_CPU_OVERRIDE_CPU_POLICY  OverridePolicy;
  UINT8                                 BspSelection;
  UINT8                                 UpBootSelection;
  UINT8                                 ProcessorBistEnable;
  UINT8                                 ProcessorHyperThreadingDisable;
  UINT8                                 ProcessorVmxEnable;
  UINT8                                 EnableCoresInSbsp;
  UINT8                                 EnableCoresInNbsp;
  UINT32                                DcaPrefetchDelayValue;
  UINT32                                VirtualWireMode;
  BOOLEAN                               ProcessorMsrLockControl;
  BOOLEAN                               Processor3StrikeControl;
  BOOLEAN                               DcaState;
  BOOLEAN                               CcxEnable;
  BOOLEAN                               C1AutoDemotion;
  BOOLEAN                               C3AutoDemotion;
  UINT8                                 PackageCState;
  BOOLEAN                               C1eEnable;
  BOOLEAN                               Gv3State;
  BOOLEAN                               PsdState;
  BOOLEAN                               CmpState;
  BOOLEAN                               PECIEnable;
  BOOLEAN                               LtEnable;
  BOOLEAN                               L2Enable;
  BOOLEAN                               L2EccEnable;
  BOOLEAN                               FastString;
  BOOLEAN                               MachineCheckEnable;
  BOOLEAN                               MLCSpatialPrefetcherEnable;
  BOOLEAN                               MLCStreamerPrefetcherEnable;
  BOOLEAN                               DCUStreamerPrefetcherEnable;
  BOOLEAN                               DCUIPPrefetcherEnable;
  BOOLEAN                               EchoTprDisable;
  BOOLEAN                               MonitorMwaitEnable;
  BOOLEAN                               TurboModeEnable;
  BOOLEAN                               ExtremeEnable;
  BOOLEAN                               XapicEnable;
  BOOLEAN                               Vr11Enable;
  BOOLEAN                               TdcLimitOverride;
  UINT16                                TdcLimit;
  BOOLEAN                               TdpLimitOverride;
  UINT16                                TdpLimit;
  UINT8                                 RatioLimit1C;
  UINT8                                 RatioLimit2C;
  UINT8                                 RatioLimit3C;
  UINT8                                 RatioLimit4C;
  UINT64                                CpuCommonFeature;
  BOOLEAN                               DCUModeSelection;
  BOOLEAN                               BiDirectionalProchot;
  EFI_PLATFORM_CATEGORY                 PlatformCategory;
  UINT8                                 ActiveProcessorCores;
  UINT8                                 DisableCore1;
  UINT8                                 DisableCore2;
  UINT8                                 DisableCore3;
} EFI_PLATFORM_CPU_PROTOCOL;

extern EFI_GUID gEfiPlatformCpuProtocolGuid;

#endif

