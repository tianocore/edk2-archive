/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

// LPC Bridge - Device 31, Function 0
// Define the needed LPC registers used by ASL.

scope(\_SB) {
  OperationRegion(ILBR, SystemMemory, \IBAS, 0x8C)
  Field(ILBR, AnyAcc, NoLock, Preserve) {
    Offset(0x08), // 0x08
    PARC,   8,
    PBRC,   8,
    PCRC,   8,
    PDRC,   8,
    PERC,   8,
    PFRC,   8,
    PGRC,   8,
    PHRC,   8,
    Offset(0x88), // 0x88
    ,       4,
    UI4E,   1
  }

  Include ("98_LINK.ASL")
}
// LPC Bridge - Device 31, Function 0
scope (\_SB.PCI0.LPCB) {
  OperationRegion(LPC0, PCI_Config, 0x40, 0xC0)
  Field(LPC0, AnyAcc, NoLock, Preserve)
  {
    Offset(0x040), // 0x80
    C1EN,   1, // COM1 Enable
    ,      31
  }

  Include ("LPC_DEV.ASL")

} //end of SCOPE

