/**@file
  Common header file shared by all source files.

  This file includes package header files, library classes and protocol, PPI & GUID definitions.

  Copyright (c) 2007 - 2007, Intel Corporation
  All rights reserved. This program and the accompanying materials are licensed and made available under the terms and conditions of the BSD License which accompanies this distribution. The full text of the license may be found at http://opensource.org/licenses/bsd-license.php THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef __COMMON_HEADER_H_
#define __COMMON_HEADER_H_


//
// The package level header files this module uses
//
#include <DxeCore.h>
#include <EdkDxeCore.h>
//
// The protocols, PPI and GUID defintions for this module
//
#include <Protocol/LoadedImage.h>
#include <Protocol/SectionExtraction.h>
#include <Guid/DebugImageInfoTable.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadFile.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/PlatformDriverOverride.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/HobList.h>
#include <Guid/FileInfo.h>
#include <Guid/Apriori.h>
#include <Protocol/FirmwareVolume.h>
#include <Guid/DxeServices.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/MemoryAllocationHob.h>
#include <Protocol/CustomizedDecompress.h>
#include <Protocol/Decompress.h>
#include <Protocol/LoadPe32Image.h>
#include <Protocol/FirmwareVolumeDispatch.h>
#include <Protocol/Ebc.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EdkDecompress.h>
#include <Guid/EventGroup.h>
#include <Guid/FirmwareFileSystem.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/Capsule.h>
#include <Protocol/BusSpecificDriverOverride.h>
//
// The Library classes this module consumes
//
#include <Library/DxeCoreEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/PerformanceLib.h>
#include <Library/UefiDecompressLib.h>
#include <Library/TianoDecompressLib.h>
#include <Library/CustomDecompressLib.h>
#include <Library/EdkPeCoffLoaderLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/BaseMemoryLib.h>

#endif
