/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\TCPD, IntObj)

Scope(\_SB.PCI0.I2C4) {
//------------------------
// Touch Pads on I2C4
// Note: instead of adding more touch devices, parametrize this one with appropriate _HID value and GPIO numbers
// GPIO_18:TCHPAD_INT_N     North West Community, IRQ number 0x75.
//------------------------

  Device(TPD0)
  {
    Name(_ADR, One)
    Name(_HID, "ALPS0001")
    Name(_CID, "PNP0C50")
    Name(_UID, One)
    Name(_S0W, 4) // required to put the device to D3 Cold during S0 idle
    Name (SBFB, ResourceTemplate () {
      I2cSerialBus (
        0x2C,
        ControllerInitiated,
        400000,
        AddressingMode7Bit,
        "\\_SB.PCI0.I2C4",
        0x00,
        ResourceConsumer,
        ,
        )
    })
    Name (SBFG, ResourceTemplate () {
      GpioInt (Level, ActiveLow, Exclusive, PullDefault, 0x0000, "\\_SB.GPO0", 0x00, ResourceConsumer, ,)
        {   // Pin list
          0x0012
        }
    })
    Name (SBFI, ResourceTemplate () {
      Interrupt(ResourceConsumer, Level, ActiveLow, ExclusiveAndWake,,,)
      {
        0x75,
      }
    })
    Method (_INI, 0, NotSerialized)
    {
    }
    Method (_STA, 0, NotSerialized)
    {
      If (LEqual (TCPD, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }
    Method (_CRS, 0, NotSerialized)
    {
      If (LLess (OSYS, 2012)) {
        Return (SBFI)
      }
      Return (ConcatenateResTemplate (SBFB, SBFI))
    }
    Method(_DSM, 0x4, NotSerialized)
    {
      // DSM UUID for HIDI2C. Do Not change.
      If (LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        // Function 0 : Query Function
        If (LEqual(Arg2, Zero)) {
          // Revision 1
          If (LEqual(Arg1, One)) {
            Return (Buffer (One) {0x03})
          }
          Else
          {
            Return (Buffer (One) {0x00})
          }
        } ElseIf (LEqual(Arg2, One)) {  // Function 1 : HID Function
          // HID Descriptor Address (IHV Specific)
          Return(0x0020)
        } Else {
          Return (Buffer (One) {0x00})
        }
      }
      Else
      {
        If (LEqual(Arg0, ToUUID("EF87EB82-F951-46DA-84EC-14871AC6F84B"))) {
          If (LEqual (Arg2, Zero)) {
            If (LEqual (Arg1, One)) {
              Return (Buffer (One) {0x03})
            }
          }

          If (LEqual (Arg2, One)) {
            Return (ConcatenateResTemplate (SBFB, SBFG))
          }

          Return (Buffer (One) {0x00})
        }
        Else
        {
          Return (Buffer (One) {0x00})
        }
      }
    }
  }
}  //Scope(\_SB.PCI0.I2C4)

