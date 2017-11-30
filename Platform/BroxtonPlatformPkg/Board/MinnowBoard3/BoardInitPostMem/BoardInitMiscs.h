/** @file
  Multiplatform initialization header file.
  This file includes package header files, library classes.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MINNOW_MULTIPLATFORM_LIB_H_
#define _MINNOW_MULTIPLATFORM_LIB_H_

#define LEN_64M       0x4000000
//
// Default PCI32 resource size
//
#define RES_MEM32_MIN_LEN   0x38000000

//
// Bluetooth device identifier.
//
#define SELECT_BLUE_TOOTH_DISABLE 0x00
#define SELECT_BLUE_TOOTH_BCM2E40 0x01
#define SELECT_BLUE_TOOTH_BCM2EA1 0x02

#define RES_IO_BASE   0x0D00
#define RES_IO_LIMIT  0xFFFF

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <FrameworkPei.h>
#include "PlatformBaseAddresses.h"
#include "ScAccess.h"
#include "SetupMode.h"
#include "PlatformBootMode.h"
#include "CpuRegs.h"
#include "Platform.h"
#include "CMOSMap.h"
#include <Ppi/Stall.h>
#include <Guid/SetupVariable.h>
#include <Ppi/AtaController.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Capsule.h>
#include <Guid/EfiVpdData.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/SteppingLib.h>
#include <IndustryStandard/Pci22.h>
#include <Guid/FirmwareFileSystem.h>
#include <Guid/MemoryTypeInformation.h>
#include <Ppi/Reset.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MemoryDiscovered.h>
#include <Guid/GlobalVariable.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Guid/Capsule.h>
#include <Guid/RecoveryDevice.h>
#include <Ppi/MasterBootMode.h>
#include <Guid/PlatformInfo_Aplk.h>

#define EFI_ACPI_OEM_ID_DEFAULT    SIGNATURE_64('I', 'N', 'T', 'E', 'L', ' ', ' ', ' ')     // max 6 chars
#define EFI_ACPI_OEM_ID1           SIGNATURE_64('I', 'N', 'T', 'E', 'L', '1', ' ', ' ')     // max 6 chars
#define EFI_ACPI_OEM_ID2           SIGNATURE_64('I', 'N', 'T', 'E', 'L', '2', ' ', ' ')     // max 6 chars

#define EFI_ACPI_OEM_TABLE_ID_DEFAULT   SIGNATURE_64('E', 'D', 'K', '2', ' ', ' ', ' ', ' ')
#define EFI_ACPI_OEM_TABLE_ID1          SIGNATURE_64('E', 'D', 'K', '2', '_', '1', ' ', ' ')
#define EFI_ACPI_OEM_TABLE_ID2          SIGNATURE_64('E', 'D', 'K', '2', '_', '2', ' ', ' ')

//
// Default Vendor ID and Subsystem ID
//
#define SUBSYSTEM_VENDOR_ID1   0x8086
#define SUBSYSTEM_DEVICE_ID1   0x1999
#define SUBSYSTEM_SVID_SSID1   (SUBSYSTEM_VENDOR_ID1 + (SUBSYSTEM_DEVICE_ID1 << 16))

#define SUBSYSTEM_VENDOR_ID2   0x8086
#define SUBSYSTEM_DEVICE_ID2   0x1888
#define SUBSYSTEM_SVID_SSID2   (SUBSYSTEM_VENDOR_ID2 + (SUBSYSTEM_DEVICE_ID2 << 16))

#define SUBSYSTEM_VENDOR_ID   0x8086
#define SUBSYSTEM_DEVICE_ID   0x1234
#define SUBSYSTEM_SVID_SSID   (SUBSYSTEM_VENDOR_ID + (SUBSYSTEM_DEVICE_ID << 16))

//
// eMMCHostMaxSpeed identifier.
//
#define EMMC_HS400_MODE   0x00
#define EMMC_HS200_MODE   0x01
#define EMMC_DDR50_MODE   0x02

EFI_STATUS
Minnow3GetPlatformInfoHob (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO_HOB     **PlatformInfoHob
  );

EFI_STATUS
Minnow3MultiPlatformGpioTableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  );

EFI_STATUS
Minnow3MultiPlatformGpioProgram (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  );

EFI_STATUS
Minnow3MultiPlatformInfoInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB  *PlatformInfoHob
  );

EFI_STATUS
Minnow3InitializeBoardOemId (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PLATFORM_INFO_HOB        *PlatformInfoHob
  );

EFI_STATUS
Minnow3InitializeBoardSsidSvid (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PLATFORM_INFO_HOB        *PlatformInfoHob
  );

#endif

