/** @file
  Header file for Initialization Driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_INITIALIZATION_DRIVER_H_
#define _SC_INITIALIZATION_DRIVER_H_

#include <Library/UefiLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/EventGroup.h>
#include <Library/ConfigBlockLib.h>
//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/ScS3Support.h>
#include <Library/ScAslUpdateLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/ScInfo.h>
#include <ScAccess.h>
#include <Library/ScPlatformLib.h>
#include <Library/SideBandLib.h>
#include <PlatformBaseAddresses.h>
#include <Private/Library/UsbCommonLib.h>
#include <Private/Guid/ScPolicyHobGuid.h>
#include <Library/MmPciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/SteppingLib.h>

#define SC_PM_ENABLE

typedef struct {
  EFI_SC_INFO_PROTOCOL ScInfo;
} SC_INSTANCE_PRIVATE_DATA;

//
// Data definitions
//
extern EFI_HANDLE mImageHandle;

///
///  IRQ routing init table entry
///
typedef struct {
  UINTN BusNumber;
  UINTN DeviceNumber;
  UINTN FunctionNumber;
  UINTN Irq;
} SC_IRQ_INIT_ENTRY;

/**
  Initialize the SC device according to the SC Platform Policy

  @param[in, out] ScInstance                 SC instance private data. May get updated by this function
  @param[in]      ScPolicy                   The SC Policy instance
  @param[in]      PmcBase                    PMC base address of this SC device
  @param[in]      AcpiBaseAddr               ACPI IO base address of this SC device

  @retval         EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
InitializeScDevice (
  IN OUT SC_INSTANCE_PRIVATE_DATA       *ScInstance,
  IN     SC_POLICY_HOB                  *ScPolicy,
  IN     UINT32                         PmcBase,
  IN     UINT16                         AcpiBaseAddr
  );

/**
  Program Sc devices dedicated IRQ#.

  @param[in] ScPolicy                   The SC Policy instance

  @retval    EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
ConfigureIrqAtBoot (
  IN      SC_POLICY_HOB                 *ScPolicy
  );

/**
  SC initialization triggered by ExitBootServices events
  Useful for operations which must happen later than at the end of post

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScExitBootServicesEvent (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

/**
  SC initialization triggered by ExitBootServices for LPSS events
  This should be at a higher TPL than the callback that sets Untrusted Mode.

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScLpssExitBootServicesEvent (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

/**
  SC initialization before Boot Script Table is closed

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScOnEndOfDxe (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  );

/**
  Hide PCI config space of HD-Audio device and do any final initialization.

  @param[in] ScPolicy                   The SC Policy instance

  @retval    EFI_SUCCESS                The function completed successfully
  @retval    EFI_UNSUPPORTED            Device non-exist or Dsp is disabled

**/
EFI_STATUS
ConfigureHdaAtBoot (
  IN SC_POLICY_HOB                      *ScPolicy
  );

/**
  Perform the remaining configuration on SC SATA to perform device detection
  at end of Dxe, then set the SATA SPD and PxE corresponding, and set the Register Lock

  @param[in] ScPolicy                      The SC Policy instance

  @retval    EFI_SUCCESS                   The function completed successfully

**/
EFI_STATUS
ConfigureSataDxe (
  IN SC_POLICY_HOB                      *ScPolicy
  );

/**
  Hide PCI config space of Otg device and do any final initialization.

  @param[in] ScPolicy                      The SC Policy instance

  @retval    EFI_SUCCESS                   The function completed successfully
  @retval    EFI_NOT_FOUND                 GNVS Protocol interface not found

**/
EFI_STATUS
ConfigureOtgAtBoot (
  IN SC_POLICY_HOB                         *ScPolicy
  );

/**
  Hide PCI config space of Lpss devices and do any final initialization.

  @param[in] ScPolicy                      The SC Policy instance

  @retval    EFI_SUCCESS                   The function completed successfully
  @retval    EFI_NOT_FOUND                 GNVS Protocol interface not found

**/
EFI_STATUS
ConfigureLpssAtBoot (
  IN SC_POLICY_HOB                         *ScPolicy
  );

/**
  Stop Lpss devices[I2C and SPI] if any is running.

  @retval EFI_SUCCESS                   The function completed successfully

**/
EFI_STATUS
StopLpssAtBoot (
  VOID
  );

/**
  Register notification function for PM configuration after PCI enumeration

  @param[in]  ScPolicy                  The SC Policy instance

  @retval     EFI_SUCCESS               The function completed successfully

**/
EFI_STATUS
ConfigurePmcAfterPciEnum (
  IN SC_POLICY_HOB                      *ScPolicy
  );

/**
  Dump whole SC_POLICY_HOB and serial out.

  @param[in] ScPolicy                   The SC Policy instance

**/
VOID
ScDumpPolicy (
  IN  SC_POLICY_HOB                     *ScPolicy
  );

/**
  Configures ports of the SC USB3 (xHCI) controller before entering OS.     the SC USB3 (xHCI) controller

  @param[in] ScPolicy                   The SC Policy instance

**/
VOID
ConfigureXhciAtBoot (
  IN SC_POLICY_HOB                      *ScPolicy
  );

/**
  Update ASL object before Boot

  @param[in] ScPolicy                   The SC Policy instance

  @retval    EFI_STATUS
  @retval    EFI_NOT_READY              The Acpi protocols are not ready.

**/
EFI_STATUS
UpdateNvsArea (
  IN     SC_POLICY_HOB                  *ScPolicy
  );

#endif

