/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\PSSI, IntObj)

Scope(\_SB.PCI0.I2C7){
//------------------------
// PSS on I2C7
//------------------------
/*
PSS: Monza_X-2K_Dura
* GPIO: None
* I2C bus: I2C 7 (0 based)
* I2C slave address: 0x6E
* I2C frequency: 400K
* ACPI ID: IMPJ0003
*/
  Device (IMP3)
  {
    Name (_ADR, Zero)               // _ADR: Address
    Name (_HID, "IMPJ0003")         // _HID: Hardware ID
    Name (_CID, "IMPJ0003")         // _CID: Compatible ID
    Name (_UID, One)                // _UID: Unique ID

    Method(_STA, 0x0, NotSerialized) {
      If (LEqual (PSSI, 1)) {
        Return (0xF)
      }
      Return (0)
    }

    Method (_CRS, 0, Serialized) {
      Name (SBUF, ResourceTemplate () {
        I2cSerialBus (0x6E,ControllerInitiated,400000,AddressingMode7Bit,"\\_SB.PCI0.I2C7",0x00,ResourceConsumer,,)
      })
      Return (SBUF)
    }

  } // Device (IMP3)
}  //Scope(\_SB.PCI0.I2C7)

