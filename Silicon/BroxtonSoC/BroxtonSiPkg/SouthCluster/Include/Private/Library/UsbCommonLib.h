/** @file
  Header file for USB Common Lib.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _USB_COMMON_H_
#define _USB_COMMON_H_

#include <Library/ScPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/SideBandLib.h>
#include <Ppi/ScPolicy.h>
#include <Library/S3BootScriptLib.h>
#include <Pi/PiBootMode.h>
#include <Library/TimerLib.h>
#include <SiPolicyHob.h>
#include <Private/Guid/ScPolicyHobGuid.h>

typedef struct {
  UINT8   Device;
  UINT8   Function;
} USB_CONTROLLER;

#define INIT_COMMON_SCRIPT_IO_WRITE(TableName, Width, Address, Count, Buffer)
#define INIT_COMMON_SCRIPT_IO_READ_WRITE(TableName, Width, Address, Data, DataMask)
#define INIT_COMMON_SCRIPT_MEM_WRITE(TableName, Width, Address, Count, Buffer)
#define INIT_COMMON_SCRIPT_MEM_READ_WRITE(TableName, Width, Address, Data, DataMask)
#define INIT_COMMON_SCRIPT_PCI_CFG_WRITE(TableName, Width, Address, Count, Buffer)
#define INIT_COMMON_SCRIPT_PCI_CFG_READ_WRITE(TableName, Width, Address, Data, DataMask)
#define INIT_COMMON_SCRIPT_STALL(TableName, Duration)


/**
  Configures SC USB controller.

  @param[in] UsbConfig               The SC Policy for USB configuration
  @param[in] XhciMmioBase            Memory base address of XHCI Controller
  @param[in] BusNumber               PCI Bus Number of the SC device
  @param[in] FuncDisableReg          Function Disable Register
  @param[in] BootMode                current boot mode

  @retval    EFI_INVALID_PARAMETER   The parameter of ScPolicy is invalid
  @retval    EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
EFIAPI
CommonUsbInit (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       BusNumber,
  IN OUT UINT32                   *FuncDisableReg,
  IN  EFI_BOOT_MODE               BootMode
  );

/**
  Performs basic configuration of SC USB3 (xHCI) controller.

  @param[in] UsbConfig            The SC Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
CommonXhciHcInit (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase,
  IN  EFI_BOOT_MODE               BootMode
  );

/**
  Setup XHCI Over-Current Mapping.

  @param[in] UsbConfig            The SC Platform Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciOverCurrentMapping (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Program and enable XHCI Memory Space.

  @param[in] UsbConfig            The SC Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciMemorySpaceOpen (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Clear and disable XHCI Memory Space.

  @param[in] UsbConfig            The SC Platform Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciMemorySpaceClose (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Lock USB registers before boot.

  @param[in] ScPolicy    The SC Platform Policy

  @retval    None

**/
VOID
UsbInitBeforeBoot (
  IN  SI_POLICY_HOB              *SiPolicy,
  IN  SC_POLICY_HOB              *ScPolicy
  );

/**
  Initialization USB Clock Gating registers.

  @retval     None

**/
VOID
ConfigureUsbClockGating (
  VOID
  );

#endif

