/** @file
  Platform Early Stage header file.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_PLATFORM_INIT_H_
#define _EFI_PLATFORM_INIT_H_

#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x

#include <CpuRegs.h>
#include <CpuType.h>
#include <FrameworkPei.h>
#include <PeiSaPolicyUpdate.h>
#include <Core/Pei/PeiMain.h>

#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>

#include <Guid/Capsule.h>
#include <Guid/EfiVpdData.h>
#include <Guid/FirmwareFileSystem.h>
#include <Guid/GlobalVariable.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/PlatformInfo.h>

#include <Guid/RecoveryDevice.h>
#include <Guid/SetupVariable.h>
#include <Guid/PlatformCpuInfo.h>

#include <Ppi/AtaController.h>
#include <Ppi/BlockIo.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/Capsule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/Reset.h>
#include <Ppi/Smbus.h>
#include <Ppi/Stall.h>
#include <Ppi/CpuPolicy.h>
#include <Ppi/FirmwareVolume.h>
#include <Ppi/BoardInitSignalling.h>
#include <Library/PeiPolicyInitLib.h>
#include <Library/PeiCpuPolicyUpdateLib.h>
#include <Library/SideBandLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/GpioLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PmicLib.h>

#include "CMOSMap.h"
#include "Platform.h"
#include <PlatformBaseAddresses.h>
#include <PlatformBootMode.h>
#include <SaAccess.h>
#include <ScAccess.h>
#include <SetupMode.h>
#include <CpuAccess.h>

#define SMC_LAN_ON      0x46
#define SMC_LAN_OFF     0x47
#define SMC_DEEP_S3_STS 0xB2

//
// Wake Event Types
//
#define SMBIOS_WAKEUP_TYPE_RESERVED           0x00
#define SMBIOS_WAKEUP_TYPE_OTHERS             0x01
#define SMBIOS_WAKEUP_TYPE_UNKNOWN            0x02
#define SMBIOS_WAKEUP_TYPE_APM_TIMER          0x03
#define SMBIOS_WAKEUP_TYPE_MODEM_RING         0x04
#define SMBIOS_WAKEUP_TYPE_LAN_REMOTE         0x05
#define SMBIOS_WAKEUP_TYPE_POWER_SWITCH       0x06
#define SMBIOS_WAKEUP_TYPE_PCI_PME            0x07
#define SMBIOS_WAKEUP_TYPE_AC_POWER_RESTORED  0x08

//
// Defines for stall PPI
//
#define PEI_STALL_RESOLUTION  1

//
// Define for Vibra
//
#define  PWM_DUTY_CYCLE            0x55  // zero duty cycle is 0x32/0xff
#define  PWM_BASE_UNIT             0x3   // DRV2603 work on 10k-250k while PWM base is 19.2M, 0x3/0xFF*19.2MHz ~ 200KHz
#define  PWM_TIMEOUT_MAX           100
#define  PWM_DELAY                 1000000

extern EFI_GUID gVbtInfoGuid;

typedef struct {
  EFI_PHYSICAL_ADDRESS    VbtAddress;
  UINT32                  VbtSize;
} VBT_INFO;

#pragma pack(push, 1)
typedef union {
  UINT32 Raw;
  struct {
    UINT32 Pwm_On_Time_Divisor    :8;  ///< [7:0]
    UINT32 Pwm_Base_Unit          :16; ///< [23:8]
    UINT32 Pwm_Rsvd               :6;  ///< [29:24]
    UINT32 Pwm_Sw_Update          :1;  ///< [30]
    UINT32 Pwm_Enable             :1;  ///< [31]
  } BITS; // Bits
} PWMCTRL;
#pragma pack(pop)

//
// Function Prototypes
//
EFI_STATUS
PlatformScInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration,
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINT16                      PlatformType
  );

EFI_STATUS
EFIAPI
IchReset (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

BOOLEAN
GetSleepTypeAfterWakeup (
  IN  CONST EFI_PEI_SERVICES    **PeiServices,
  OUT UINT16                    *SleepType
  );

EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

VOID
CheckPowerOffNow (
  VOID
  );

VOID
SetPlatformBootMode (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB *PlatformInfoHob
  );

EFI_STATUS
EFIAPI
CpuOnlyReset (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  );

EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN CONST EFI_PEI_STALL_PPI    *This,
  IN UINTN                      Microseconds
  );

EFI_STATUS
MultiPlatformInfoInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB *PlatformInfoHob
  );

BOOLEAN
IsRecoveryJumper (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB *PlatformInfoHob
  );

EFI_STATUS
PlatformInfoUpdate (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB *PlatformInfoHob,
  IN SYSTEM_CONFIGURATION      *SystemConfiguration
  );

EFI_STATUS
InitializePlatform (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PLATFORM_INFO_HOB        *PlatformInfoHob,
  IN SYSTEM_CONFIGURATION         *SystemConfiguration
  );

EFI_STATUS
GeneralPowerFailureHandler (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

BOOLEAN
IsRtcUipAlwaysSet (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
RtcPowerFailureHandler (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
PublishMemoryTypeInfo (
  VOID
  );

EFI_STATUS
EFIAPI
PeiGetSectionFromFv (
  IN CONST  EFI_GUID        NameGuid,
  OUT VOID                  **Address,
  OUT UINT32               *Size
  );

EFI_STATUS
EFIAPI
ConfigClockTrunk (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

EFI_STATUS
EFIAPI
PlatformInitFinalConfig (
  IN CONST EFI_PEI_SERVICES           **PeiServices
  );

EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  );

EFI_STATUS
UpdateBootMode (
  IN CONST EFI_PEI_SERVICES                       **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB                    *PlatformInfoHob
  );

UINT8
ConfigurePlatformPmic (
  VOID
  );

VOID
ScUsb2PhyOverride (
  VOID
  );
#endif // _EFI_PLATFORM_INIT_H_

