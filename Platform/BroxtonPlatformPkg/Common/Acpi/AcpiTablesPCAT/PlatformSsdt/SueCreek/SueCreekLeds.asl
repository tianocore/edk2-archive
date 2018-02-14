/** @file

Copyright (c) 2017 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0.I2C5) {
  Device (LED0) {
    Name (_HID, "PCA9956")
    Name (_UID, 0)
    Name (_DDN, "SueCreekLed, CS0")
    Name (SBUF, ResourceTemplate () {
      I2cSerialBus (
        0x0065,
        ControllerInitiated,
        400000,
        AddressingMode7Bit,
        "\\_SB.PCI0.I2C5",
        0x00,
        ResourceConsumer,
        ,
        )
    })
    Method (_CRS, 0, NotSerialized)
    {
      Return (SBUF)
    }
    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (SUCE, 0)) {
        Return (0x0)
      } else {
        Return (0xF)
      }
    }
  }
  Device (LED1) {
    Name (_HID, "PCA9956")
    Name (_UID, 1)
    Name (_DDN, "SueCreekLed, CS0")
    Name (SBUF, ResourceTemplate () {
      I2cSerialBus (
        0x0069,
        ControllerInitiated,
        400000,
        AddressingMode7Bit,
        "\\_SB.PCI0.I2C5",
        0x00,
        ResourceConsumer,
        ,
        )
    })
    Method (_CRS, 0, NotSerialized)
    {
      Return (SBUF)
    }
    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (SUCE, 0)) {
        Return (0x0)
      } else {
        Return (0xF)
      }
    }
  }
}
