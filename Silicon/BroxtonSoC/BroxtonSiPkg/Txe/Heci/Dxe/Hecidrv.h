/** @file
  Header file for HECI driver.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_DRV_H_
#define _HECI_DRV_H_

#include <SeCAccess.h>
#include <HeciRegs.h>
#include <IndustryStandard/Pci22.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/SeCLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SeCPlatformPolicy.h>

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/Heci.h>
#include <Protocol/SeCRcInfo.h>
#include <SeCAccess.h>
#include <HeciRegs.h>

#define HECI_PRIVATE_DATA_SIGNATURE         SIGNATURE_32 ('H', 'e', 'c', 'i')
#define HECI_ROUND_UP_BUFFER_LENGTH(Length) ((UINT32) ((((Length) + 3) / 4) * 4))

//
// HECI private data structure
//
typedef struct {
  UINTN                     Signature;
  EFI_HANDLE                Handle;
  UINT32                    HeciMBAR;
  UINT32                    HeciMBAR0;
  UINT16                    DeviceInfo;
  UINT32                    RevisionInfo;
  EFI_HECI_PROTOCOL         HeciCtlr;
  volatile UINT32           *HpetTimer;
  EFI_SEC_RC_INFO_PROTOCOL  SeCRcInfo;
} HECI_INSTANCE;
#endif

