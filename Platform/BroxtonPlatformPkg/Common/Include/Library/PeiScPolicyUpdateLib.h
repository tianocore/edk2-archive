/** @file
  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_SC_POLICY_UPDATE_LIB_H_
#define _PEI_SC_POLICY_UPDATE_LIB_H_

#if 0
/**
  Install PCIE Device Table.

  @param[in]  DeviceTable            The pointer to the PCH_PCIE_DEVICE_OVERRIDE

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
InstallPcieDeviceTable (
  IN PCH_PCIE_DEVICE_OVERRIDE         *DeviceTable
  );
#endif

/**
  This function performs SC PEI Policy initialization.

  @param[in, out] ScPolicy       The SC Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
UpdatePeiScPolicy (
  IN OUT  SC_POLICY_PPI         *ScPolicyPpi
  );

#endif

