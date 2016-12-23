/** @file
  Header file for SMM Access Driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMM_ACCESS_DRIVER_H_
#define _SMM_ACCESS_DRIVER_H_

#include <SaRegs.h>
#include <SaAccess.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/PciRootBridgeIo.h>

#define SMM_ACCESS_PRIVATE_DATA_SIGNATURE   SIGNATURE_32('4','5','s','a')

//
// Chipset specific constants
//

#define MAX_SMRAM_RANGES 2

// PCI "address" of chipset
#define PCI_BUS 0
#define PCI_DEV 0
#define PCI_FNC 0

// Chipset identifying registers
#define CHIPSET_VID       0x8086
#define CHIPSET_DID       0xA000

//
// Chipset register(s)
//

// SMM configuration register
typedef UINT8 SMRAM;        // System Management RAM Control
#define SMRAM_OFFSET        HUNIT_HSMMCTL

//
// Private data
//
typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_SMM_ACCESS2_PROTOCOL        SmmAccess;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  //
  // Local Data for SMM Access interface goes here
  //
  UINTN                           NumberRegions;
  EFI_SMRAM_DESCRIPTOR            SmramDesc[MAX_SMRAM_RANGES];
} SMM_ACCESS_PRIVATE_DATA;

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR(a, SMM_ACCESS_PRIVATE_DATA, SmmAccess, SMM_ACCESS_PRIVATE_DATA_SIGNATURE)

//
// Prototypes
// Driver model protocol interface
//

/**
  This is the standard EFI driver point that detects
  whether there is an GMCH815 chipset in the system
  and if so, installs an SMM Access Protocol

  @param[in]  ImageHandle       Handle for the image of this driver
  @param[in]  SystemTable       Pointer to the EFI System Table

  @retval     EFI_SUCCESS       Protocol successfully started and installed
  @retval     EFI_UNSUPPORTED   Protocol can't be started

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

/**
  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all boot-service
  and SMM agents.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully opened
  @retval    EFI_DEVICE_ERROR        The region could not be opened because locked by chipset
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
  This routine accepts a request to "close" a region of SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "close" means that the memory is only visible from SMM agents,
  not from BS or RT code.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully closed
  @retval    EFI_DEVICE_ERROR        The region could not be closed because locked by chipset
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to BS state.

  @param[in] This                    Pointer to the SMM Access Interface

  @retval    EFI_SUCCESS             The region was successfully locked.
  @retval    EFI_DEVICE_ERROR        The region could not be locked because at least one range is still open.
  @retval    EFI_INVALID_PARAMETER   The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the
  memory controller capabilities.

  @param[in]      This                    Pointer to the SMRAM Access Interface.
  @param[in, out] SmramMapSize            Pointer to the variable containing size of the buffer to contain the description information
  @param[in, out] SmramMap                Buffer containing the data describing the Smram region descriptors

  @retval         EFI_BUFFER_TOO_SMALL    The user did not provide a sufficient buffer.
  @retval         EFI_SUCCESS             The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL    *This,
  IN OUT UINTN                         *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR          *SmramMap
  );

#endif

