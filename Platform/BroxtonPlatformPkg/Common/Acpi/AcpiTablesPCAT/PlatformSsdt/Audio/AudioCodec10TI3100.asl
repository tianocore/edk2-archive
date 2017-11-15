/** @file
  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\ISCT, IntObj)

Scope(\_SB.PCI0.I2C5)
{
    Device (ACDT)  // Audio Codec driver I2C
    {
      Name (_ADR, 0x18)
      Name (_HID, "10TI3100")
      Name (_CID, "10TI3100")
      Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec" )
      Name (_UID, 1)

      Method(_CRS, 0x0, Serialized)
      {
        Name(SBUF,ResourceTemplate ()
        {
          I2CSerialBus (0x18, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PCI0.I2C5",0x00, ResourceConsumer,,)
        })
        Return (SBUF)
      }

      Method (_STA, 0x0, NotSerialized)
      {
        If (LEqual (ISCT, 1)) {
          Return (0xF)  // I2S Codec Enabled
        }
        Return (0)
      }
    } // Device (ACDT)
}

