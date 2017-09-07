/** @file
  Header file for Platform Initialization Driver.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PLATFORM_DRIVER_H
#define _PLATFORM_DRIVER_H

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Framework/FrameworkInternalFormRepresentation.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/FrameworkFormCallback.h>
#include <Protocol/CpuIo.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/DataHub.h>
#include <Protocol/PciIo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/BoardFeatures.h>
#include <Guid/SetupVariable.h>
#include <Guid/PlatformInfo.h>
#include <IndustryStandard/Pci22.h>
#include <ScAccess.h>
#include <SaAccess.h>
#include <CMOSMap.h>
#include <PlatformBaseAddresses.h>
#include <SetupMode.h>
#include <PlatformBootMode.h>

//
// Internal (Locoal) Header files
//
#include "Configuration.h"

#define _EFI_H_    //skip efi.h

#define PCAT_RTC_ADDRESS_REGISTER   0x74
#define PCAT_RTC_DATA_REGISTER      0x75

#define RTC_ADDRESS_REGISTER_A      0x0A
#define RTC_ADDRESS_REGISTER_B      0x0B
#define RTC_ADDRESS_REGISTER_C      0x0C
#define RTC_ADDRESS_REGISTER_D      0x0D

#define B_RTC_ALARM_INT_ENABLE      0x20
#define B_RTC_ALARM_INT_STATUS      0x20

//
// Platform driver GUID
//
#define EFI_PLATFORM_DRIVER_GUID \
  { 0x056E7324, 0xA718, 0x465b, {0x9A, 0x84, 0x22, 0x8F, 0x06, 0x64, 0x2B, 0x4F }}

#define EFI_SECTION_STRING                  0x1C
#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
#define PREFIX_BLANK                        0x04

#pragma pack(1)

typedef UINT64  EFI_BOARD_FEATURES;
typedef UINT16  STRING_REF;
typedef UINT16  EFI_FORM_LABEL;

typedef struct {
  CHAR8 AaNumber[7];
  UINT8 BoardId;
  EFI_BOARD_FEATURES Features;
  UINT16 SubsystemDeviceId;
  UINT16 AudioSubsystemDeviceId;
  UINT64 AcpiOemTableId;
} BOARD_ID_DECODE;

typedef
EFI_STATUS
(EFIAPI *EFI_FORM_ROUTINE) (
  SYSTEM_CONFIGURATION *SetupBuffer
  );

typedef struct {
  CHAR16      *OptionString;  ///< Passed in string to generate a token for in a truly dynamic form creation
  STRING_REF  StringToken;    ///< This is used when creating a single op-code without generating a StringToken (have one already)
  UINT16      Value;
  UINT8       Flags;
  UINT16      Key;
} IFR_OPTION;

typedef struct {
  UINT8   Number;
  UINT32  HorizontalResolution;
  UINT32  VerticalResolution;
} PANEL_RESOLUTION;

#pragma pack()

//
// Prototypes
//
/**
  Saves UART2 Base Address To Restore on S3 Resume Flows

  @param[in] Event     A pointer to the Event that triggered the callback.
  @param[in] Context   A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ConfigureUart2OnReadyToBoot (
  EFI_EVENT  Event,
  VOID       *Context
  );

//
// Prototypes defined in Platform.c start
//
VOID
EFIAPI
ReadyToBootFunction (
  EFI_EVENT  Event,
  VOID       *Context
  );

UINT8
ReadCmosBank1Byte (
  IN  EFI_CPU_IO2_PROTOCOL            *CpuIo,
  IN  UINT8                           Index
  );

VOID
WriteCmosBank1Byte (
  IN  EFI_CPU_IO2_PROTOCOL            *CpuIo,
  IN  UINT8                           Index,
  IN  UINT8                           Data
  );

//
// Prototypes defined in BoardId.c
//
VOID
InitializeBoardId (
  );

//
// Prototypes defined in PciDevice.c
//
EFI_STATUS
PciBusDriverHook (
  );

VOID
EFIAPI
PciBusEvent (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  );

//
// Prototypes defined in IdePlatformPolicy.c
//
VOID
InitPlatformIdePolicy (
  VOID
  );

//
// Prototypes defined in ClockControl.c
//
VOID
InitializeClockRouting (
  VOID
  );

//
// Prototypes defined in SlotConfig.c
//
VOID
InitializeSlotInfo (
  VOID
  );

//
// Prototypes defined in SiPolicy.c
//
VOID
InitSiPolicy (
  VOID
  );

//
// Prototypes defined in SensorVar.c
//
#if defined(SENSOR_INFO_VAR_SUPPORT) && SENSOR_INFO_VAR_SUPPORT != 0
VOID
InitializeSensorInfoVariable (
  );
#endif

//
// Prototypes defined in IchTcoReset.c
//
VOID
InitTcoReset (
  VOID
  );

//
// Prototypes defined in SioPlatformPolicy.c
//
VOID
InitPlatformSaPolicy (
  SYSTEM_CONFIGURATION* SystemConfiguration
  );

VOID
InitSioPlatformPolicy (
  VOID
  );

VOID
EFIAPI
AdjustRtcCentury (
  VOID
  );

//
// Global externs
//
extern SYSTEM_CONFIGURATION mSystemConfiguration;

extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *mPciRootBridgeIo;
extern UINT32 mSubsystemVidDid;

extern UINT8 mBoardIdIndex;
extern BOOLEAN mFoundAANum;
extern EFI_BOARD_FEATURES mBoardFeatures;
extern UINT16 mSubsystemDeviceId;
extern UINT16 mSubsystemAudioDeviceId;
extern CHAR8 BoardAaNumber[];

extern BOARD_ID_DECODE mBoardIdDecodeTable[];
extern UINTN mBoardIdDecodeTableSize;

extern BOOLEAN mMfgMode;
extern UINT32 mPlatformBootMode;

extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

#endif

