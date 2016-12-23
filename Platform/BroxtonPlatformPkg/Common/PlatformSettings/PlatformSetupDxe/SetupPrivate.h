/** @file
  Header file for Setup Private data.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SetupPrivate__H__
#define __SetupPrivate__H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef VFRCOMPILE
#include <Framework/FrameworkInternalFormRepresentation.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/PciIo.h>
#include <Protocol/CpuIo.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/MemInfo.h>
#include <Protocol/LegacyBios.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PciLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/EventGroup.h>
#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Acpi.h>

void InitString (EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrRef, CHAR16 *sFormat, ...);

EFI_STATUS
HiiLibGetString (
  IN  EFI_HII_HANDLE                  PackageList,
  IN  EFI_STRING_ID                   StringId,
  OUT EFI_STRING                      String,
  IN  OUT UINTN                       *StringSize
  );
EFI_STRING
EFIAPI
HiiConstructRequestString (
  IN EFI_STRING      RequestString, OPTIONAL
  IN UINTN           Offset,
  IN UINTN           Width
  );

typedef struct {
  EFI_GUID                        *pGuid;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *pFormCallback;
  UINT16                          Class, SubClass;
  EFI_HII_HANDLE                  HiiHandle;
} CALLBACK_INFO;

extern EFI_GUID mSetupGuid;
extern EFI_GUID mSetupNvramUpdateGuid;
extern EFI_GUID mSetupEnterGuid;
extern EFI_GUID mSystemAccessGuid;
#define INVALID_HII_HANDLE NULL

#endif

#define PLATFORM_SETUP_VARSTORE_NAME PLATFORM_SETUP_VARIABLE_NAME

#define MAIN_FORM_SET_GUID     { 0x985eee91, 0xbcac, 0x4238, 0x87, 0x78, 0x57, 0xef, 0xdc, 0x93, 0xf2, 0x4e }
#define ADVANCED_FORM_SET_GUID { 0xe14f04fa, 0x8706, 0x4353, 0x92, 0xf2, 0x9c, 0x24, 0x24, 0x74, 0x6f, 0x9f }
#define CHIPSET_FORM_SET_GUID  { 0xadfe34c8, 0x9ae1, 0x4f8f, 0xbe, 0x13, 0xcf, 0x96, 0xa2, 0xcb, 0x2c, 0x5b }
#define BOOT_FORM_SET_GUID     { 0x8b33ffe0, 0xd71c, 0x4f82, 0x9c, 0xeb, 0xc9, 0x70, 0x58, 0xc1, 0x3f, 0x8e }
#define SECURITY_FORM_SET_GUID { 0x981ceaee, 0x931c, 0x4a17, 0xb9, 0xc8, 0x66, 0xc7, 0xbc, 0xfd, 0x77, 0xe1 }
#define EXIT_FORM_SET_GUID     { 0xa43b03dc, 0xc18a, 0x41b1, 0x91, 0xc8, 0x3f, 0xf9, 0xaa, 0xa2, 0x57, 0x13 }
#define RESET_FORM_SET_GUID    { 0x46975fe8, 0x64f6, 0x4692, { 0x8d, 0x64, 0xd5, 0x6a, 0xa7, 0x52, 0xea, 0xa9 } }

#define MAIN_FORM_SET_CLASS             0x01
#define ADVANCED_FORM_SET_CLASS         0x02
#define CHIPSET_FORM_SET_CLASS          0x04
#define BOOT_FORM_SET_CLASS             0x08
#define SECURITY_FORM_SET_CLASS         0x10
#define EXIT_FORM_SET_CLASS             0x20
#define RESET_FORM_SET_CLASS            0x40

#define DEFAULT_BOOT_TIME_OUT           5
#define LABEL_CPU_RATIO                 1

typedef struct {
  UINT8  SerialDebug;
  UINT8  SerialDebugBaudRate;
  UINT8  RamDebugInterface;
  UINT8  UartDebugInterface;
  UINT8  Usb2DebugInterface;
  UINT8  Usb3DebugInterface;
  UINT8  SerialIoDebugInterface;
  UINT8  TraceHubDebugInterface;
} DEBUG_CONFIG_DATA;

//
// DO NOT WRITE BELOW THIS LINE
//
#ifdef __cplusplus
}
#endif

#endif

