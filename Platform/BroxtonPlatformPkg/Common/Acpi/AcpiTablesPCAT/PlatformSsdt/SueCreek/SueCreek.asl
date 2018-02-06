/** @file

Copyright (c) 2017 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.SPI1) {
  Device (TP0) {
    Name (_HID, "SUE1000")
    Name (_DDN, "SueCreek - SPI0, CS0")
    Name (_CRS, ResourceTemplate () {
      SpiSerialBus (
        0,                      // Chip select (0, 1, 2)
        PolarityLow,            // Chip select is active low
        FourWireMode,           // Full duplex
        8,                      // Bits per word is 8 (byte)
        ControllerInitiated,    // Don't care
        9600000,                // 9.6 MHz
        ClockPolarityHigh,      // SPI mode 3
        ClockPhaseSecond,       // SPI mode 3
        "\\_SB.PCI0.SPI1",      // SPI host controller
        0                       // Must be 0
      )
    })
    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (SUCE, 0)) {
        Return (0x0)
      } else {
        Return (0xF)
      }
    }
  }
}

