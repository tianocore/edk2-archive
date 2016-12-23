/** @file
  Initilize Cpu DXE Platform Policy.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SiPolicyProtocol.h>

DXE_SI_POLICY_PROTOCOL mSiPolicyData = { 0 };

/**
  Initilize Intel Cpu DXE Platform Policy.

  @param[in]  ImageHandle           Image handle of this driver.

  @retval     EFI_SUCCESS           Initialization complete.
  @retval     EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval     EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval     EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InitSiPolicy(
  )
{
  EFI_STATUS      Status;
  EFI_HANDLE      ImageHandle;

  ImageHandle = NULL;
  mSiPolicyData.Revision = DXE_SI_POLICY_PROTOCOL_REVISION_1;

  //
  // Install the DXE_SI_POLICY_PROTOCOL interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gDxeSiPolicyProtocolGuid,
                  &mSiPolicyData,
                  NULL
                  );

  ASSERT_EFI_ERROR (Status);

  return Status;
}

