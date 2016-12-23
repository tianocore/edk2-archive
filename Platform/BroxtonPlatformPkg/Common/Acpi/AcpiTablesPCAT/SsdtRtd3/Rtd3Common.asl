/** @file
  ACPI RTD3 SSDT table

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\_SB.OSCO)
External(\_SB.PCI0, DeviceObj)

External(\_SB.PCI0.RP01, DeviceObj)
External(\_SB.PCI0.RP02, DeviceObj)
External(\_SB.PCI0.RP03, DeviceObj)
External(\_SB.PCI0.RP04, DeviceObj)
External(\_SB.PCI0.RP05, DeviceObj)
External(\_SB.PCI0.RP06, DeviceObj)
External(\_SB.PCI0.RP01.VDID)
External(\_SB.PCI0.RP02.VDID)
External(\_SB.PCI0.RP03.VDID)
External(\_SB.PCI0.RP04.VDID)
External(\_SB.PCI0.RP05.VDID)
External(\_SB.PCI0.RP06.VDID)
External(\_SB.PCI0.SATA, DeviceObj)
External(\_SB.PCI0.SATA.PRT0, DeviceObj)
External(\_SB.PCI0.SATA.PRT1, DeviceObj)

External(\_SB.PCI0.XHC, DeviceObj)
External(\_SB.PCI0.XHC.RHUB, DeviceObj)

External(\_SB.PCI0.XHC.RHUB.HS01, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS02, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS03, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS04, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS05, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS06, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS07, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.HS08, DeviceObj)

External(\_SB.PCI0.XHC.RHUB.SSP1, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP2, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP3, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP4, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP5, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP6, DeviceObj)
External(\_SB.PCI0.XHC.RHUB.SSP7, DeviceObj)

External(\_SB.PCI0.XDCI, DeviceObj)

External(\_SB.PCI0.SDHA, DeviceObj)
External(\_SB.PCI0.SDIO, DeviceObj)

External(\_SB.PCI0.PWM,  DeviceObj)
External(\_SB.PCI0.I2C0, DeviceObj)
External(\_SB.PCI0.I2C1, DeviceObj)
External(\_SB.PCI0.I2C2, DeviceObj)
External(\_SB.PCI0.I2C3, DeviceObj)
External(\_SB.PCI0.I2C4, DeviceObj)
External(\_SB.PCI0.I2C5, DeviceObj)
External(\_SB.PCI0.I2C6, DeviceObj)
External(\_SB.PCI0.I2C7, DeviceObj)
External(\_SB.PCI0.SPI1, DeviceObj)
External(\_SB.PCI0.SPI2, DeviceObj)
External(\_SB.PCI0.SPI3, DeviceObj)
External(\_SB.PCI0.URT1, DeviceObj)
External(\_SB.PCI0.URT2, DeviceObj)
External(\_SB.PCI0.URT3, DeviceObj)
External(\_SB.PCI0.URT4, DeviceObj)

External(\GPRW, MethodObj)
External(P8XH, MethodObj)
External(XDST, IntObj)

//
// Externs common to ULT0RTD3.asl and FFRDRTD3.asl and exclude for BRRTD3.asl
//

// GPIO methods
External(\_SB.GPC0, MethodObj)
External(\_SB.SPC0, MethodObj)

// IO expander methods
// RTD3 devices and variables

