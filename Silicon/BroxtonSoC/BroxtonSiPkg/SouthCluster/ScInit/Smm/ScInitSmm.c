/** @file
  SC Init Smm module for SC specific SMI handlers.

  Copyright (c) 2013 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInitSmm.h"
#include <Library/MemoryAllocationLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SW_DISPATCH2_PROTOCOL *mSwDispatch;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SX_DISPATCH2_PROTOCOL *mSxDispatch;
GLOBAL_REMOVE_IF_UNREFERENCED SC_NVS_AREA                   *mScNvsArea;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                        mAcpiBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED SC_POLICY_HOB                 *mScPolicy;
GLOBAL_REMOVE_IF_UNREFERENCED SC_PCIE_CONFIG                *mPcieRpConfig;

//
// The reserved MMIO range to be used in Sx handler
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                      mResvMmioBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                     mResvMmioSize;


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
PchSxHandler (
  IN EFI_HANDLE                    Handle,
  IN CONST VOID                    *Context OPTIONAL,
  IN OUT VOID                      *CommBuffer OPTIONAL,
  IN OUT UINTN                     *CommBufferSize OPTIONAL
  )
{
  return EFI_SUCCESS;
}


/**
  Initialize PCH Sx entry SMI handler.

  @param[in]  ImageHandle   Handle for the image of this driver

  @retval     None

**/
VOID
InitializeSxHandler (
  IN EFI_HANDLE                            ImageHandle
  )
{
  EFI_SMM_SX_REGISTER_CONTEXT               SxDispatchContext;
  EFI_HANDLE                                SxDispatchHandle;
  EFI_SLEEP_TYPE                            SxType;
  EFI_STATUS                                Status;

  DEBUG ((DEBUG_INFO, "InitializeSxHandler() Start\n"));

  //
  // Register the callback for S3/S4/S5 entry
  //
  SxDispatchContext.Phase = SxEntry;
  for (SxType = SxS3; SxType <= SxS5; SxType++) {
    SxDispatchContext.Type = SxType;
    Status = mSxDispatch->Register (
                            mSxDispatch,
                            PchSxHandler,
                            &SxDispatchContext,
                            &SxDispatchHandle
                            );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "InitializeSxHandler() End\n"));
}


/**
  Initializes the PCH SMM handler for for PCIE hot plug support
  <b>PchInit SMM Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInitSmm module is a SMM driver that initializes the Intel Platform Controller Hub
      SMM requirements and services. It consumes the PCH_POLICY_PROTOCOL for expected
      configurations per policy.

  - <b>Details</b>\n
    This module provides SMI handlers to services PCIE HotPlug SMI, LinkActive SMI, and LinkEq SMI.
    And also provides port 0x61 emulation support, GPIO Sx isolation requirement, and register BIOS WP
    handler to process BIOSWP status.

  - @pre
    - @link _EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL @endlink
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.
    - EFI_SMM_BASE2_PROTOCOL
    - @link _EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL @endlink
    - @link _EFI_SMM_SX_DISPATCH2_PROTOCOL EFI_SMM_SX_DISPATCH2_PROTOCOL @endlink
    - @link _EFI_SMM_CPU_PROTOCOL EFI_SMM_CPU_PROTOCOL @endlink
    - @link _PCH_SMI_DISPATCH_PROTOCOL PCH_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_PCIE_SMI_DISPATCH_PROTOCOL PCH_PCIE_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_TCO_SMI_DISPATCH_PROTOCOL PCH_TCO_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_ESPI_SMI_DISPATCH_PROTOCOL PCH_ESPI_SMI_DISPATCH_PROTOCOL @endlink
    - SC_NVS_AREA_PROTOCOL
    - @link PeiDxeSmmPchPlatformLib.h PeiDxeSmmPchPlatformLib @endlink library
    - @link PeiDxeSmmPchPciExpressHelpersLib.h PchPciExpressHelpersLib @endlink library

  - <b>References</b>\n
    @link _PCH_POLICY PCH_POLICY_PROTOCOL @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS

  @param[in] ImageHandle      Handle for the image of this driver
  @param[in] SystemTable      Pointer to the EFI System Table

  @retval    EFI_SUCCESS      PCH SMM handler was installed

**/
EFI_STATUS
EFIAPI
ScInitSmmEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_PEI_HOB_POINTERS    HobPtr;
  SC_PCIE_CONFIG          *PcieRpConfig;

  DEBUG ((DEBUG_INFO, "ScInitSmmEntryPoint()\n"));

  //
  // Locate the SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &mSwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID**) &mSxDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Sc Policy Hob
  //
  HobPtr.Guid  = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mScPolicy = (SC_POLICY_HOB *) GET_GUID_HOB_DATA (HobPtr.Guid);

  Status = GetConfigBlock ((VOID *) mScPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);

  mPcieRpConfig = AllocatePool(sizeof (SC_PCIE_CONFIG));
  if (mPcieRpConfig != NULL) {
    CopyMem (mPcieRpConfig, PcieRpConfig, sizeof (SC_PCIE_CONFIG));
    }

  InitializeSxHandler (ImageHandle);

  Status = InitializeScPcieSmm (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

