/** @file
  Build time limits of SC resources.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_LIMITS_H_
#define _SC_LIMITS_H_

//
// PCIe limits
//
#define SC_MAX_PCIE_ROOT_PORTS       SC_BXTP_PCIE_MAX_ROOT_PORTS
#define SC_BXT_PCIE_MAX_ROOT_PORTS   2
#define SC_BXTX_PCIE_MAX_ROOT_PORTS  4
#define SC_BXTP_PCIE_MAX_ROOT_PORTS  6

#define SC_MAX_PCIE_CONTROLLERS      SC_BXTP_PCIE_MAX_CONTROLLERS
#define SC_PCIE_CONTROLLER_PORTS     4
#define SC_BXT_PCIE_MAX_CONTROLLERS  1
#define SC_BXTX_PCIE_MAX_CONTROLLERS 1
#define SC_BXTP_PCIE_MAX_CONTROLLERS 2

#define SC_PCIE_MAX_CLK_REQ          4

//
// PCIe clocks limits
//
#define SC_MAX_PCIE_CLOCKS                 6

//
// SATA limits
//
#define SC_MAX_SATA_PORTS                  2
#define SC_SATA_MAX_DEVICES_PER_PORT       1       ///< Max support device numner per port, Port Multiplier is not support.

//
// USB limits
//
#define HSIC_MAX_PORTS                  2
#define XHCI_MAX_USB3_PORTS             1
#define XHCI_MAX_HSIC_PORTS             1     ///< BXT has only 1 HSIC port
#define XHCI_MAX_SSIC_PORTS             2     ///< BXT has 2 SSIC port

#define SC_MAX_USB2_PORTS               SC_BXTP_MAX_USB2_PORTS
#define SC_BXT_MAX_USB2_PORTS           3
#define SC_BXTP_MAX_USB2_PORTS          8

#define SC_MAX_USB3_PORTS               SC_BXTP_MAX_USB3_PORTS
#define SC_BXT_MAX_USB3_PORTS           2
#define SC_BXTP_MAX_USB3_PORTS          6

//
// Flash Protection Range Register
//
#define SC_FLASH_PROTECTED_RANGES       5

#endif ///< _SC_LIMITS_H_

