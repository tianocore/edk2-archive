/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*
 GPIO_10 for Wi-Fi direct IRQ 0x6D.
 GPIO_15 for Wi-Fi reset
 PMIC_STDBY for Wi-Fi disable, NW index 30
*/


Scope(\_SB.PCI0.SDIO)
{
  Device (BRCM)
  {
    Name (_ADR, One)  // _ADR: Address
    Name (_DEP, Package() {\_SB.GPO0})
    Name (_S4W, 2)
    Name (_S0W, 2)

    Method (_STA, 0, NotSerialized)
    {
      Return (0xF)
    }

    Method (_RMV, 0, NotSerialized)
    {
      Return (Zero)
    }

    Name (_PRW, Package (0x02)
    {
      Zero,
      Zero
    })

    Method (_CRS, 0, Serialized)
    {
      Name (RBUF, ResourceTemplate ()
      {
        Interrupt (ResourceConsumer, Edge, ActiveHigh, ExclusiveAndWake, , , ) {0x6D}         // GPIO_10 to IOAPIC IRQ 0x6D
      })
      Return (RBUF)
    }

    Method (_PS3, 0, NotSerialized)
    {
      Store( 0x00, \_SB.GPO0.CWLE )        // Put WiFi chip in Reset
      Sleep(150)
    }

    Method (_PS0, 0, NotSerialized)
    {
      Store( 0x01, \_SB.GPO0.CWLE )      // Take WiFi chip out in Reset
      Sleep(150)
    }
  }
}

