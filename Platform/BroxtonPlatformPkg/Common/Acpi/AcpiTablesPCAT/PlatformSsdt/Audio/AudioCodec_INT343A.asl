/** @file
  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\IOBF, IntObj) // ScHdAudioIoBufferOwnership
External(\IS3A, IntObj)

  Scope(\_SB.PCI0.I2C7) {
  //-----------------------------------
  //  HD Audio I2S Codec device
  //  I2C7
  //-----------------------------------
  Device (HDAC)
  {
    Name (_HID, "INT343A")
    Name (_CID, "INT343A")
    Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
    Name (_UID, 1)
    Name (CADR, 0) // Codec I2C address

    Method(_INI) {
    }

    Method (_CRS, 0, Serialized) {
      Name (SBFB, ResourceTemplate () {
        I2cSerialBus (0x1A, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PCI0.I2C7",0x00, ResourceConsumer,,)
      })

      Return (SBFB)
    }

    Method (_STA, 0, NotSerialized)
    {
      If (LAnd (LEqual (IOBF, 3), LEqual(IS3A, 1))) {
        Return (0xF)  // I2S Codec Enabled
      }
      Return (0)
    }
  }  // Device (HDAC)
}

