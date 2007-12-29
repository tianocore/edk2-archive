/** @file

Copyright (c) 2004 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Bds.h

Abstract:

  Head file for BDS Architectural Protocol implementation

Revision History


**/

#ifndef _BDS_H
#define _BDS_H

#include <PiDxe.h>

//
// WQBugBug: These Macro will be handled properly later.
//
#define PI_SPECIFICATION_VERSION 0x00010000
#define PERF_ENABLE(handle, table, ticker)
#define PERF_START(handle, token, host, ticker)
#define PERF_END(handle, token, host, ticker)
#define PERF_UPDATE(handle, token, host, handlenew, tokennew, hostnew)
#define PERF_CODE(code)


#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/PciIo.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadFile.h>
#include <Protocol/CpuIo.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DriverConfiguration.h>
#include <Protocol/DataHub.h>
#include <Protocol/Bds.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/BlockIo.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/GenericMemoryTest.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/SerialIo.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/SimpleTextInEx.h>

#include <Guid/GlobalVariable.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/BootState.h>
#include <Guid/Bmp.h>
#include <Guid/HobList.h>
#include <Guid/FileInfo.h>
#include <Guid/GenericPlatformVariable.h>
#include <Guid/DataHubRecords.h>

#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GraphicsLib.h>
#include <Library/UefiEfiIfrSupportLib.h>

#include <Common/MaxBbsEntries.h>

#include <MdeModuleHii.h>

//
// WQBugBug: This Macro Definition will be removed when UefiStringGather Tool is ready.
//
#include "BdsStrDefs.h"

//
// Bds AP Context data
//
#define EFI_BDS_ARCH_PROTOCOL_INSTANCE_SIGNATURE  EFI_SIGNATURE_32 ('B', 'd', 's', 'A')
typedef struct {
  UINTN                     Signature;

  EFI_HANDLE                Handle;

  EFI_BDS_ARCH_PROTOCOL     Bds;

  //
  // Save the current boot mode
  //
  EFI_BOOT_MODE             BootMode;

  //
  // Set true if boot with default settings
  //
  BOOLEAN                   DefaultBoot;

  //
  // The system default timeout for choose the boot option
  //
  UINT16                    TimeoutDefault;

  //
  // Memory Test Level
  //
  EXTENDMEM_COVERAGE_LEVEL  MemoryTestLevel;

} EFI_BDS_ARCH_PROTOCOL_INSTANCE;

#define EFI_BDS_ARCH_PROTOCOL_INSTANCE_FROM_THIS(_this) \
  CR (_this, \
      EFI_BDS_ARCH_PROTOCOL_INSTANCE, \
      Bds, \
      EFI_BDS_ARCH_PROTOCOL_INSTANCE_SIGNATURE \
      )

//
// Prototypes
//
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                     ImageHandle,
  IN EFI_SYSTEM_TABLE               *SystemTable
  );

VOID
EFIAPI
BdsEntry (
  IN  EFI_BDS_ARCH_PROTOCOL *This
  );

#endif
