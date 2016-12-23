/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\TCPL, IntObj)

Scope(\_SB.PCI0.I2C3) {
//------------------------
// Touch Panels on I2C3
// Note: instead of adding more touch panels, parametrize this one with appropriate _HID value and GPIO numbers
// GPIO_21:TCH_PNL_INTR_LS_N     North Community, IRQ number 0x32.
//------------------------
  Device (TPL1) {
    Name (HID2, Zero)
    Name (_HID, "ELAN221D")  // _HID: Hardware ID
    Name (_CID, "PNP0C50")  // _CID: Compatible ID
    Name (_S0W, 0x04)  // _S0W: S0 Device Wake State
    Name (SBFB, ResourceTemplate () {
      I2cSerialBus (
        0x0010,
        ControllerInitiated,
        400000,
        AddressingMode7Bit,
        "\\_SB.PCI0.I2C3",
        0x00,
        ResourceConsumer,
        ,
        )
    })
    Name (SBFG, ResourceTemplate () {
      GpioInt (Level, ActiveLow, Exclusive, PullDefault, 0x0000,
        "\\_SB.GPO0", 0x00, ResourceConsumer, ,
        )
        {   // Pin list
            0x0015
        }
    })
    Name (SBFI, ResourceTemplate () {
      Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, ,, )
      {
        0x32,
      }
    })

    Method (_INI, 0, NotSerialized)  // _INI: Initialize
    {
    }

    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
      If (LEqual (TCPL, 1)) {
        Return (0x0F)
      }
      Return (0x00)
    }

    Method (_CRS, 0, NotSerialized) {
      If (LLess (OSYS,2012)) {
        // For Windows 7 only report Interrupt; it doesn't support ACPI5.0 and wouldn't understand GpioInt nor I2cBus
        return (SBFI)
      }
      Return (ConcatenateResTemplate(SBFB, SBFI))
    }

    Method(_DSM, 0x4, Serialized){
      Store ("Method _DSM begin", Debug)
      If (LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))) {
        // DSM Function
        switch(ToInteger(Arg2))
          {
          // Function 0: Query function, return based on revision
          case(0)
          {
            // DSM Revision
            switch(ToInteger(Arg1))
            {
              // Revision 1: Function 1 supported
              case(1)
              {
                Store ("Method _DSM Function Query", Debug)
                Return(Buffer(One) { 0x03 })
              }

              default
              {
                // Revision 2+: no functions supported
                Return(Buffer(One) { 0x00 })
              }
            }
          }

          // Function 1 : HID Function
          case(1)
          {
            Store ("Method _DSM Function HID", Debug)
            // HID Descriptor Address
            Return(0x0001)
          }

          default
          {
            // Functions 2+: not supported
            Return(0x0000)
          }
        }
      }
      Elseif(LEqual(Arg0, ToUUID("EF87EB82-F951-46DA-84EC-14871AC6F84B")))
      { // Windows 7 Resources DSM (Intel specific)

        // Function 0 : Query Function
        If (LEqual(Arg2, Zero)) {
          // Revision 1
          If (LEqual(Arg1, One)) {
            Return(Buffer(One) { 0x03 })
          }
        }
        // Function 1 : I2CSerialBus(...) and GpioInt(...) resources
        If (LEqual(Arg2, One)) {
          Return (ConcatenateResTemplate(SBFB, SBFG))
        }

        Return(Buffer(One) { 0x00 })
      }
      else
      {
        // No other GUIDs supported
        Return(Buffer(One) { 0x00 })
      }
    }

  } // Device (TPL0)
}  //Scope(\_SB.PCI0.I2C3)

