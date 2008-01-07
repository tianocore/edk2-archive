/** @file

Copyright (c) 2004 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  InternalBdsLib.h

Abstract:

  BDS library definition, include the file and data structure


**/

#ifndef _INTERNAL_BDS_LIB_H_
#define _INTERNAL_BDS_LIB_H_

#include <PiDxe.h>

#include <IndustryStandard/Pci22.h>

#include <Protocol/BlockIo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/Cpu.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DebugPort.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PciIo.h>
#include <Protocol/AcpiS3Save.h>

#include <Guid/MemoryTypeInformation.h>
#include <Guid/FileInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/PcAnsi.h>
#include <Guid/ShellFile.h>
#include <Guid/HobList.h>


#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>


#include <Library/UefiIfrSupportLib.h>
#include <Library/GenericBdsLib.h>

#include "BmMachine.h"

#include "R8Lib.h"

#endif // _BDS_LIB_H_
