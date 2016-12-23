/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.I2C2)
{
  Device (STRB)
  {
    Name (_ADR, Zero)  // _ADR: Address
    Name (_HID, "INT3481")  // _HID: Hardware ID
    Name (_CID, "INT3481")  // _CID: Compatible ID
    Name (_SUB, "INTL0000")  // _SUB: Subsystem ID
    Name (_DDN, "Flash TPS61311")  // _DDN: DOS Device Name
    Name (_UID, Zero)  // _UID: Unique ID
    Name (PLDB, Package (0x01)
    {
      Buffer (0x14)
      {
        /* 0000 */  0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /* ........ */
        /* 0008 */  0x69, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,  /* i....... */
        /* 0010 */  0xFF, 0xFF, 0xFF, 0xFF                           /* .... */
      }
    })

    Method (_PLD, 0, Serialized)  // _PLD: Physical Location of Device
    {
      Return (PLDB) /* \_SB_.PCI0.I2C2.STRB.PLDB */
    }

    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
      Return (0x0F)
    }

    Method (_CRS, 0, Serialized)  // _CRS: Current Resource Settings
    {
      Name (SBUF, ResourceTemplate ()
      {
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
        "\\_SB.GPO0", 0x00, ResourceConsumer, ,
        )
        {   // Pin list
        0x0036
        }
        I2cSerialBus (0x0033, ControllerInitiated, 0x00061A80,
          AddressingMode7Bit, "\\_SB.PCI0.I2C2",
          0x00, ResourceConsumer, ,
        )
      })
      Return (SBUF) /* \_SB_.PCI0.I2C2.STRB._CRS.SBUF */
    }

    Method (_DSM, 4, NotSerialized)  // _DSM: Device-Specific Method
    {
      If (LEqual(Arg0,ToUUID ("377ba76a-f390-4aff-ab38-9b1bf33a3015"))) {
        Return ("INT3481")
      }
      If (LEqual(Arg0,ToUUID ("3c62aaaa-d8e0-401a-84c3-fc05656fa28c"))) {
        Return ("TPS61311")
      }
      If (LEqual(Arg0,ToUUID ("2959512a-028c-4646-b73d-4d1b5672fad8"))) {
        Return ("BXT")
      }
      If (LEqual(Arg0,ToUUID ("26257549-9271-4ca4-bb43-c4899d5a4881"))) {
        If (LEqual(Arg2,One)) {
          Return (One)
        }
        If (LEqual(Arg2,0x02)) {
          Return (0x02003300)
        }
      }
      If (LEqual(Arg0,ToUUID ("79234640-9e10-4fea-a5c1-b5aa8b19756f"))) {
        If (LEqual(Arg2,One)) {
          Return (One)
        }
        If (LEqual(Arg2,0x02)) {
          Return (0x01003604)
        }
      }
      Return (Zero)
    }
  }
}

