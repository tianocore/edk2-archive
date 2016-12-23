/** @file
  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\SDS1, IntObj)

//-----------------------------
//  Serial IO SPI1 Controller
//-----------------------------
Scope(\_SB.PCI0.SPI1)
{
  Device(FPNT) {
    Method(_HID) {
      //
      // Return FPS HID based on BIOS Setup
      //
      if (LEqual(SDS1, 1)) {Return ("FPC1020")}
      if (LEqual(SDS1, 2)) {Return ("FPC1021")}
      Return ("FPNT_DIS")
    }

    Method(_STA) {
      //
      // Is SerialIo SPI1 FPS enabled in BIOS Setup?
      //
      If (LNotEqual(SDS1, 0)) {
        Return (0x0F)
      }
      Return (0x00)
    }
    Method(_CRS, 0x0, Serialized) {
      Name(BBUF,ResourceTemplate () {
        SPISerialBus(0x00,PolarityLow,FourWireMode,8,ControllerInitiated,3000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PCI0.SPI1",,,SPIR)
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1",,,GSLP) {67} //North-west(67):GPIO_112 FGR_RESET_N
      GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.GPO0", 0x00, ResourceConsumer,GINT ) {14} //North(14):GPIO_14 : FGR_INT
      })
      Return (BBUF)
    }
  } // Device (FPNT)
}  //  end Scope(\_SB.PCI0.SPI1)

