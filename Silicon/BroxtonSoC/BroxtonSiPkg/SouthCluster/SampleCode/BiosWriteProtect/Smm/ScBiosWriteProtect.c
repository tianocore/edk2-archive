/** @file
  SC BIOS Write Protect Driver.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScBiosWriteProtect.h"

//
// Global variables
//
EFI_SMM_ICHN_DISPATCH_PROTOCOL  *mIchnDispatch;
EFI_SMM_SW_DISPATCH2_PROTOCOL   *mSwDispatch;
UINTN                           mPciSpiRegBase;

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used
  @param[in] DispatchContext      Not used

  @retval    None

**/
VOID
EFIAPI
ScBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{

  //
  // Disable BIOSWE bit to protect BIOS
  //
  MmioAnd8 ((UINTN) (mPciSpiRegBase + R_SPI_BCR), (UINT8) ~B_SPI_BCR_BIOSWE);

  //
  // Clear Sync SMI Status
  //
  MmioOr16 ((UINTN) (mPciSpiRegBase + R_SPI_BCR), (UINT16) B_SPI_BCR_SYNC_SS);
}


/**
  Register an IchnBiosWp callback function to handle TCO BIOSWR SMI
  SMM_BWP and BLE bits will be set here

  @param[in] DispatchHandle       Not used
  @param[in] DispatchContext      Not used

  @retval    None

**/
VOID
EFIAPI
ScBiosLockSwSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext
  )
{
  EFI_STATUS                        Status;
  EFI_SMM_ICHN_DISPATCH_CONTEXT     IchnContext;
  EFI_HANDLE                        IchnHandle;

  if (mIchnDispatch == NULL) {
    return;
  }

  IchnHandle = NULL;

  //
  // Set SMM_BWP bit before registering IchnBiosWp
  //
  MmioOr8 ((UINTN) (mPciSpiRegBase + R_SPI_BCR), (UINT8) B_SPI_BCR_SMM_BWP);

  //
  // Register an IchnBiosWp callback function to handle TCO BIOSWR SMI
  //
  IchnContext.Type = IchnBiosWp;
  Status = mIchnDispatch->Register (
                            mIchnDispatch,
                            ScBiosWpCallback,
                            &IchnContext,
                            &IchnHandle
                            );
  ASSERT_EFI_ERROR (Status);
}


/**
  Entry point for SC Bios Write Protect driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval    EFI_SUCCESS          Initialization complete.

**/
EFI_STATUS
EFIAPI
InstallScBiosWriteProtect (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
  SC_POLICY_HOB               *ScPolicy;
  EFI_HANDLE                  SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT SwContext;
  EFI_PEI_HOB_POINTERS        HobPtr;
  SC_LOCK_DOWN_CONFIG         *LockDownConfig;

  //
  // Get Sc Platform Policy Hob
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  ScPolicy = (SC_POLICY_HOB *) GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);

  if ((LockDownConfig->BiosLock == TRUE)) {
    mPciSpiRegBase = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_SC,
                       PCI_DEVICE_NUMBER_SPI,
                       PCI_FUNCTION_NUMBER_SPI
                       );
    //
    // Get the ICHn protocol
    //
    mIchnDispatch = NULL;
    Status        = gSmst->SmmLocateProtocol (&gEfiSmmIchnDispatchProtocolGuid, NULL, (VOID **) &mIchnDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Locate the SC SMM SW dispatch protocol
    //
    SwHandle  = NULL;
    Status    = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **) &mSwDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Register BIOS Lock SW SMI handler
    //
    SwContext.SwSmiInputValue = LockDownConfig->BiosLockSwSmiNumber;
    Status = mSwDispatch->Register (
                            mSwDispatch,
                            (EFI_SMM_HANDLER_ENTRY_POINT2) ScBiosLockSwSmiCallback,
                            &SwContext,
                            &SwHandle
                            );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

