/** @file
  Prototype of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VT_D_PEI_H_
#define _VT_D_PEI_H_

#include <PlatformBaseAddresses.h>

#define VTD_ECAP_REG  0x10
#define IR            BIT3
#define R_SA_MCHBAR   0x48

//
// VT-d Engine base address.
//
#define R_SA_MCHBAR_VTD1_OFFSET  0x6C88  ///< DMA Remapping HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET  0x6C80  ///< DMA Remapping HW UNIT2 for all other - PEG, USB, SATA etc

#define SA_VTD_ENGINE_NUMBER     2

/**
  Configure VT-d Base and capabilities.

  @param[in]  ScPolicyPpi           The SC Policy PPI instance

  @retval     EFI_SUCCESS           VT-d initialization complete
  @retval     EFI_UNSUPPORTED       VT-d is disabled by policy or not supported

**/
EFI_STATUS
VtdInit (
  IN SC_POLICY_PPI     *ScPolicyPpi
  );
#endif

