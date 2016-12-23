/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0.URT1)
{
  Device(BTH0) {
    Method(_HID) {
      // Return HID based on BIOS Setup
      Return("BCM2EA1")
    }

    Method(_INI) {
    }

    Method(_CRS, 0x0, Serialized) {
      Name(SBFG, ResourceTemplate (){
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PCI0.URT1" )
        GpioIo(Exclusive, PullDown, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {27}  // North-west(27):GPIO_214 NGFF_BT_DEV_WAKE_N
        GpioIo(Exclusive, PullDown, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO2", ) {24}  // West(24):GPIO_154, BT_DISABLE2_1P8_N
        GpioInt (Level, ActiveLow, Exclusive, PullDefault, 0x0000, "\\_SB.GPO1", 0x00, ResourceConsumer, ) {35}
      })

      Name(SBFI, ResourceTemplate () {
        UARTSerialBus(115200,,,0xc0,,,FlowControlHardware,32,32,"\\_SB.PCI0.URT1" )
        GpioIo(Exclusive, PullDown, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {27}  // North-west(27):GPIO_214 NGFF_BT_DEV_WAKE_N
        GpioIo(Exclusive, PullDown, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO2", ) {24}  // West(24):GPIO_154, BT_DISABLE2_1P8_N
        Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, ) {0x34}
      })

      If(LLess(OSYS,2012)) { return (SBFI) } // For Windows 7 only report Interrupt; it doesn't support ACPI5.0 and wouldn't understand GpioInt nor I2cBus
      Return (SBFG)
    }

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (SBTD, 2)) {
        Return (0xF)
      }
      Return (0)
    }

    Name (_S0W, 2)                            // required to put the device to D2 during S0 idle
  } // Device BTH0

}  //  end Scope(\_SB.PCI0.URT1)

