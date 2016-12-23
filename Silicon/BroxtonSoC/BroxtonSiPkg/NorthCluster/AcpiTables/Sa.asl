/** @file
  Broxton SA configuration space definition.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\_SB.PCI0, DeviceObj)

External(PNSL)
External(\_SB.PEPD, DeviceObj)

Scope (\_SB.PCI0) {

  Device(GFX0) { // Mobile I.G.D
    Name(_ADR, 0x00020000)
#if (ENBDT_PF_ENABLE == 1)
    Name (_S0W, 3)

    Method (_DEP, 0, NotSerialized) {
      Return (Package() {\_SB.PEPD})
    }


    Name (CBUF, ResourceTemplate() {
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {37}  //  MIPI_DSI_RST_1_8V, GPIO_77 (NorthWest, 37)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {9}   //  PANEL1_VDDEN_1_8V_R, Used as GPIO_196 for MIPI (NorthWest, 6)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {10}  //  PANEL1_BKLTEN_1_8V_R, Used as GPIO_197 for MIPI (NorthWest, 7)
    })

    Name (DBUF, ResourceTemplate() {})//ed

    Method (_CRS, 0, Serialized)      // _CRS: Current Resource Settings
    {
      If (LAnd (LNotEqual (PNSL, 0), LNotEqual (PNSL, 4))) {
        Return (CBUF)
      } Else {
        Return (DBUF)
      }
    }
#else

    Name (_S0W, 3)

    Method (_DEP, 0, NotSerialized) {
      Return (Package() {\_SB.PCI0.PEPD})
    }

    Name (CBUF, ResourceTemplate() {
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO0", ) {27}  // N27: DISP0_RST_N, GPIO_27 (North, 27)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {6}   //  DISP0_VDDEN, Used as GPIO_193 for MIPI (South, 6)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {7}   //  DISP0_BKLTEN, Used as GPIO_194 for MIPI (South, 7)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {9}   //  DISP1_VDDEN, Used as GPIO_196 for MIPI (South, 9)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {10}  //  DISP1_BKLTEN, Used as GPIO_197 for MIPI (South, 10)
      // DISP0 I2C bus for Display backlight DC/DC LM3631 (0x29, 400k) on I2C BUS6 (1 based)
      I2CSerialBus(0x29,                   //SlaveAddress: bus address
                   ,                       //SlaveMode: default to ControllerInitiated
                   400000,                 //ConnectionSpeed: in Hz
                   ,                       //Addressing Mode: default to 7 bit
                   "\\_SB.PCI0.I2C6",      //ResourceSource: I2C bus controller name
                   ,                       //Descriptor Name: creates name for offset of resource descriptor
                  )                       //VendorData
      // DISP1 I2C bus for Display backlight DC/DC LM3631 (0x29, 400k) on I2C BUS4 (1 based) for FAB B/C
      I2CSerialBus(0x29,                 //SlaveAddress: bus address
                   ,                       //SlaveMode: default to ControllerInitiated
                   400000,                 //ConnectionSpeed: in Hz
                   ,                       //Addressing Mode: default to 7 bit
                   "\\_SB.PCI0.I2C4",           //ResourceSource: I2C bus controller name
                   ,                       //Descriptor Name: creates name for offset of resource descriptor
                  )
    })

    Name (DBUF, ResourceTemplate() {
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO0", ) {27}  // N27: DISP0_RST_N, GPIO_27 (North, 27)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {6}   //  DISP0_VDDEN, Used as GPIO_193 for MIPI (South, 6)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {7}   //  DISP0_BKLTEN, Used as GPIO_194 for MIPI (South, 7)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {9}   //  DISP1_VDDEN, Used as GPIO_196 for MIPI (South, 9)
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO4", ) {10}  //  DISP1_BKLTEN, Used as GPIO_197 for MIPI (South, 10)
      // DISP0 I2C bus for Display backlight DC/DC LM3631 (0x29, 400k) on I2C BUS6 (1 based)
      I2CSerialBus(0x29,                 //SlaveAddress: bus address
                   ,                       //SlaveMode: default to ControllerInitiated
                   400000,                 //ConnectionSpeed: in Hz
                   ,                       //Addressing Mode: default to 7 bit
                   "\\_SB.PCI0.I2C6",      //ResourceSource: I2C bus controller name
                   ,                       //Descriptor Name: creates name for offset of resource descriptor
                  )                       //VendorData
      // DISP1 I2C bus for Display backlight DC/DC LM3631 (0x29, 400k) on I2C BUS5 (1 based) for FAB D
      I2CSerialBus(0x29,                 //SlaveAddress: bus address
                   ,                       //SlaveMode: default to ControllerInitiated
                   400000,                 //ConnectionSpeed: in Hz
                   ,                       //Addressing Mode: default to 7 bit
                   "\\_SB.PCI0.I2C5",           //ResourceSource: I2C bus controller name
                   ,                       //Descriptor Name: creates name for offset of resource descriptor
                  )                       //VendorData
    })

    Method (_CRS, 0, NotSerialized)      // _CRS: Current Resource Settings
    {
      If (LAnd(LEqual(BDID, 0x00), LLess(BREV, 0x4)))
      {
        Return(CBUF)  // This resource buffer is only exposed for RVP A/B/C
      }
      Return (DBUF) //Not FAB D
    }
#endif
    include("Igfx.asl")
  } // end "IGD Device"

  include("Ipu.asl")
}

