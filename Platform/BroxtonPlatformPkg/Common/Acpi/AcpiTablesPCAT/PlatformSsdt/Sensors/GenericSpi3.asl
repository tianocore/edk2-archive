/** @file

Copyright (c) 2017 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.SPI3) {
  Device (TP0) {
    Name (_HID, "SPT0001")
    Name (_DDN, "Sensor - SPI3, CS0")
    Name (_CRS, ResourceTemplate () {
      SpiSerialBus (
        0,                      // Chip select (0, 1, 2)
        PolarityLow,            // Chip select is active low
        FourWireMode,           // Full duplex
        8,                      // Bits per word is 8 (byte)
        ControllerInitiated,    // Don't care
        1000000,                // 1 MHz
        ClockPolarityLow,       // SPI mode 0
        ClockPhaseFirst,        // SPI mode 0
        "\\_SB.PCI0.SPI3",      // SPI host controller
        0                       // Must be 0
      )
    })
    Method (_STA, 0x0, NotSerialized) {
        Return (0xF)
    }
  }
}

