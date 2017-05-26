/** @file
  Header file for SC Init SMM Handler

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_INIT_SMM_H_
#define _SC_INIT_SMM_H_

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>
#include <Library/S3BootScriptLib.h>
#include <Library/HobLib.h>
#include <Protocol/SmmCpu.h>
#include <IndustryStandard/Pci30.h>
#include <ScAccess.h>
#include <Library/ScPlatformLib.h>
#include <Library/MmPciLib.h>
#include <Private/Library/PeiDxeSmmScPciExpressHelpersLib.h>
#include <Private/Guid/ScPolicyHobGuid.h>
#include <Ppi/ScPcieDeviceTable.h>
#include <Protocol/ScPcieSmiDispatch.h>
#include <Protocol/ScSmmIoTrapControl.h>
#include <Private/Protocol/ScNvs.h>
#include <Private/Protocol/PcieIoTrap.h>
#include <Include/SiPolicyHob.h>
#include <Library/ConfigBlockLib.h>

extern EFI_SMM_SW_DISPATCH2_PROTOCOL *mSwDispatch;
extern EFI_SMM_SX_DISPATCH2_PROTOCOL *mSxDispatch;
extern SC_NVS_AREA                   *mScNvsArea;
extern UINT16                        mAcpiBaseAddr;
extern SC_POLICY_HOB                 *mScPolicy;
extern SC_PCIE_CONFIG                *mPcieRpConfig;
extern EFI_PHYSICAL_ADDRESS          mResvMmioBaseAddr;
extern UINTN                         mResvMmioSize;

#define EFI_PCI_CAPABILITY_ID_PCIPM 0x01

#define DeviceD0 0x00
#define DeviceD3 0x03

typedef enum {
  PciCfg,
  PciMmr
} SC_PCI_ACCESS_TYPE;

typedef enum {
  Acpi,
  Rcrb,
  Tco
} SC_ACCESS_TYPE;

typedef struct {
  SC_ACCESS_TYPE  AccessType;
  UINT32          Address;
  UINT32          Data;
  UINT32          Mask;
  UINT8           Width;
} SC_SAVE_RESTORE_REG;

typedef struct {
  SC_SAVE_RESTORE_REG*  ScSaveRestoreReg;
  UINT8                 size;
} SC_SAVE_RESTORE_REG_WRAP;

struct _SC_SAVE_RESTORE_PCI;

typedef struct _SC_SAVE_RESTORE_PCI{
  SC_PCI_ACCESS_TYPE AccessType;
  UINT8              Device;
  UINT8              Function;
  UINT8              BarOffset;
  UINT16             Offset;
  UINT32             Data;
  UINT32             Mask;
  UINT8              Width;
  VOID              (*RestoreFunction) (struct _SC_SAVE_RESTORE_PCI *ScSaveRestorePci);
} SC_SAVE_RESTORE_PCI;

typedef struct {
  SC_SAVE_RESTORE_PCI*  ScSaveRestorePci;
  UINT8                 size;
} Sc_SAVE_RESTORE_PCI_WRAP;

typedef struct {
  UINT8           Device;
  UINT8           Function;
  UINT8           PowerState;
} DEVICE_POWER_STATE;

/**
  Register PCIE Hotplug SMI dispatch function to handle Hotplug enabling

  @param[in] ImageHandle             The image handle of this module
  @param[in] SystemTable             The EFI System Table

  @retval    EFI_SUCCESS             The function completes successfully

**/
EFI_STATUS
EFIAPI
InitializeScPcieSmm (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  );

/**
  Program Common Clock and ASPM of Downstream Devices

  @param[in] PortIndex                     Pcie Root Port Number
  @param[in] RpDevice                      Pcie Root Pci Device Number
  @param[in] RpFunction                    Pcie Root Pci Function Number

  @retval    EFI_SUCCESS                   Root port complete successfully
  @retval    EFI_UNSUPPORTED               PMC has invalid vendor ID

**/
EFI_STATUS
ScPcieSmi (
  IN  UINT8                             PortIndex,
  IN  UINT8                             RpDevice,
  IN  UINT8                             RpFunction
  );

/**
  PCIE Link Active State Change Hotplug SMI call back function for all Root ports

  @param[in] DispatchHandle             Handle of this dispatch function
  @param[in] RpContext                  Rootport context, which contains RootPort Index,
                                        and RootPort PCI BDF.

  @retval    None

**/
VOID
EFIAPI
ScPcieLinkActiveStateChange (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  SC_PCIE_SMI_RP_CONTEXT            *RpContext
  );

/**
  PCIE Link Equalization Request SMI call back function for all Root ports

  @param[in] DispatchHandle             Handle of this dispatch function
  @param[in] RpContext                  Rootport context, which contains RootPort Index,
                                        and RootPort PCI BDF.

  @retval    None

**/
VOID
EFIAPI
ScPcieLinkEqHandlerFunction (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  SC_PCIE_SMI_RP_CONTEXT            *RpContext
  );

/**
  The SW SMI callback to config PCIE power management settings

  @param[in]      DispatchHandle      The handle of this callback, obtained when registering
  @param[in]      CallbackContext     Pointer to the EFI_SMM_SW_REGISTER_CONTEXT
  @param[in, out] CommBuffer          Point to the CommBuffer structure
  @param[in, out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
ScPciePmSwSmiCallback (
  IN     EFI_HANDLE                   DispatchHandle,
  IN     EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT VOID                         *CommBuffer,
  IN OUT UINTN                        *CommBufferSize
  );

/**
  Initializes the PCH SMM handler for PCH save and restore

  @param[in] ImageHandle     Handle for the image of this driver
  @param[in] SystemTable     Pointer to the EFI System Table

  @retval    EFI_SUCCESS     PCH SMM handler was installed

**/
EFI_STATUS
EFIAPI
ScInitLateSmm (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  );

/**
  Locate required protocol and register the 61h IO trap

  @param[in] ImageHandle       Handle for the image of this driver
  @param[in] SystemTable       Pointer to the EFI System Table

  @retval    EFI_SUCCESS       PCH SMM handler was installed

**/
EFI_STATUS
EFIAPI
InstallIoTrapPort61h (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

/**
  Initialize PCH Sx entry SMI handler.

  @param[in] ImageHandle       Handle for the image of this driver

  @retval    None

**/
VOID
InitializeSxHandler (
  IN EFI_HANDLE        ImageHandle
  );

/**
  PCH Sx entry SMI handler.

  @param[in]      Handle          Handle of the callback
  @param[in]      Context         The dispatch context
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval         EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ScSxHandler (
  IN  EFI_HANDLE                   Handle,
  IN CONST VOID                    *Context OPTIONAL,
  IN OUT VOID                      *CommBuffer OPTIONAL,
  IN OUT UINTN                     *CommBufferSize OPTIONAL
  );

/**
  GbE Sx entry handler.

  @param[in]  None

  @retval     None

**/
VOID
EFIAPI
ScLanSxCallback (
  VOID
  );

/**
  This function performs GPIO Sx Isolation steps.

  @param[in]  None

  @retval     None

**/
VOID
EFIAPI
ScGpioSxIsolationCallBack (
  VOID
  );

/**
  Register dispatch function to handle GPIO pads Sx isolation

  @param[in]  None

  @retval     None

**/
VOID
InitializeGpioSxIsolationSmm (
  VOID
  );

/**
  Entry point for Sc Bios Write Protect driver.

  @param[in] ImageHandle             Image handle of this driver.
  @param[in] SystemTable             Global system service table.

  @retval    EFI_SUCCESS             Initialization complete.

**/
EFI_STATUS
EFIAPI
InstallScBiosWriteProtect (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  );

#endif

