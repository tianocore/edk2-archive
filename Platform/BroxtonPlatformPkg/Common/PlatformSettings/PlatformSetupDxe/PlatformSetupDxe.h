/** @file
  The defintions are required both by Source code and Vfr file.
  The PLAT_OVER_MNGR_DATA structure, form guid and Ifr question ID are defined.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PLAT_OVER_MNGR_H_
#define _PLAT_OVER_MNGR_H_

#include <FrameworkDxe.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PciIo.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Protocol/DataHub.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/VariableFormat.h>
#include <Guid/DataHubRecords.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/BiosIdLib.h>
#include <Library/HobLib.h>
#include <Guid/PlatformInfo.h>
#include <IndustryStandard/Pci22.h>
#include "Guid/SetupVariable.h"
#include <Library/HeciMsgLib.h>

#include <CpuType.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/FrameworkFormBrowser.h>

extern EFI_HII_HANDLE   mHiiHandle;

UINT32
ConvertBase10ToRaw (
  IN  EFI_EXP_BASE10_DATA            *Data
  );

UINT32
ConvertBase2ToRaw (
  IN  EFI_EXP_BASE2_DATA             *Data
  );

EFI_STATUS
GetStringFromToken (
  IN      EFI_GUID                  *ProducerGuid,
  IN      STRING_REF                Token,
  OUT     CHAR16                    **String
  );

VOID
SwapEntries (
  IN  CHAR8 *Data
  );

VOID
EFIAPI
SetupInfo (
  );

extern    EFI_HANDLE            mImageHandle;

#endif

