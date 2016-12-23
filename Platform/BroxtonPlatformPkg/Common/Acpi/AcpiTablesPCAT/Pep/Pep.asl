/** @file
  ACPI uPEP Support

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\_PR.NLPC, IntObj)

//@todo MSFT has NOT implemented uPEP for Non-CS configuration on Win10. Need to uncomment once MSFT fully implements uPEP
If (LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))) {
  //Comment out the GFX0, since the _DEP is added in NorthCluster/AcpiTables/Sa.asl
  Scope(\_SB.PCI0.SATA) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C3) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C4) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C5) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C6) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.I2C7) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SPI1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SPI2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SPI3) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.URT1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.URT2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.URT4) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.XHC)  { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_PR.CPU0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_PR.CPU1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_PR.CPU2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_PR.CPU3) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SDIO) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SDHA) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.SDC) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PCI0.RP02.PXSX) { Name(_DEP, Package(){\_SB.PEPD}) }
}

Scope(\_SB)
{
  Device (PEPD)
  {
    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)

    Name(PEPP, Zero)
    Name (DEVS, Package()
    {
      1,
      Package() {"\\_SB.PCI0.GFX0"},
      //Package() {"\\_SB.PCI0.SAT0.PRT1"}
    })

    Name(DEVY, Package() // uPEP Device List
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name-string
      // 2: Enabled/Disabled Field
      //      0 = This device is disabled and applies no constraints
      //     1+ = This device is enabled and applies constraints
      // 3: Constraint Package: entry per LPI state in LPIT
      //     a. Associated LPI State UID
      //         ID == 0xFF: same constraints apply to all states in LPIT
      //     b: minimum Dx state as pre-condition
      //     c: (optional) OEM specific OEM may provide an additional encoding
      //         which further defines the D-state Constraint
      //            0x0-0x7F - Reserved
      //            0x80-0xFF - OEM defined
      //
      Package() {"\\_PR.CPU0",                    0x1, Package() {0, Package() {0xFF, 0}}},
      Package() {"\\_PR.CPU1",                    0x1, Package() {0, Package() {0xFF, 0}}},
      Package() {"\\_PR.CPU2",                    0x1, Package() {0, Package() {0xFF, 0}}},
      Package() {"\\_PR.CPU3",                    0x1, Package() {0, Package() {0xFF, 0}}},
      Package() {"\\_SB.PCI0.GFX0",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SATA",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},
      Package() {"\\_SB.PCI0.URT1",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.URT2",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.URT3",               0x0, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.URT4",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SPI1",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SPI2",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SPI3",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C0",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C1",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C2",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C3",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C4",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C5",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C6",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.I2C7",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.PWM",                0x0, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SDIO",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SDHA",               0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.SDC",                0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"\\_SB.PCI0.XHC",                0x1, Package() {0, Package() {0xFF, 3}}},
      Package() {"INTELAUDIO\\FUNC_01&VEN_8086&DEV_280*", 0x1, Package(){0, Package (){0xFF, 3}}},
      Package() {"\\_SB.PCI0.RP02.PXSX",          0x0, Package() {0, Package() {0xFF, 0}}},
    })

    Name(BCCD, Package() // Bugcheck Critical Device(s)
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name string
      // 2: Package of packages: 1 or more specific commands to power up critical device
      //  2a: Package: GAS-structure describing location of PEP accessible power control
      //    Refer to ACPI 5.0 spec section 5.2.3.1 for details
      //    a: Address Space ID (0 = System Memory)
      //       NOTE: A GAS Address Space of 0x7F (FFH) indicates remaining package
      //             elements are Intel defined
      //    b: Register bit width (32 = DWORD)
      //    c: Register bit offset
      //    d: Access size (3 = DWORD Access)
      //    e: Address (for System Memory = 64-bit physical address)
      //  2b: Package containing:
      //    a: AND mask !V not applicable for all Trigger Types
      //    b: Value (bits required to power up the critical device)
      //    c: Trigger Type:
      //         0 = Read
      //         1 = Write
      //         2 = Write followed by Read
      //         3 = Read Modify Write
      //         4 = Read Modify Write followed by Read
      //  2c: Power up delay: Time delay before next operation in uSec
      //
      Package() {"\\_SB.PCI0.SATA", Package() {
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SATA.PRT0", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PCI0.SATA.PRT1", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
    })

    Method(_STA, 0x0, NotSerialized)
    {
      If (LOr(LGreaterEqual(OSYS,2015), LAnd(LGreaterEqual(OSYS,2012),LEqual(S0ID, 1)))) {
        Return(0xf)
      }
      Return(0)
    }

    Method(_DSM, 0x4, Serialized)
    {

      If (LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66"))) {
        // Number of Functions (including this one)
        If (LEqual(Arg2, Zero)) {
          Return(Buffer(One){0x07})
        }

        If (LEqual(\_PR.NLPC, 1)) {
          Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - CPU-0
          Store (0x00, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - CPU-1
          Store (0x00, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - CPU-2
          Store (0x00, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - CPU-3
        }

        If (LEqual(\_PR.NLPC, 2)) {
          Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - CPU-0
          Store (0x00, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - CPU-1
          Store (0x01, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - CPU-2
          Store (0x00, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - CPU-3
        }

        If (LEqual(\_PR.NLPC, 3)) {
          Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - CPU-0
          Store (0x01, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - CPU-1
          Store (0x01, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - CPU-2
          Store (0x00, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - CPU-3
        }

        If (LEqual(\_PR.NLPC, 4)) {
          Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - CPU-0
          Store (0x01, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - CPU-1
          Store (0x01, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - CPU-2
          Store (0x01, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - CPU-3
        }

        If (LEqual(ODBG, 0)) {
          Store (0x01, Index (DeRefOf(Index (DEVY, 8)), 1))  // URT-3
        }

        If (LEqual(ODBG, 1)) {
          Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1))  // URT-3
        }

        If (\_SB.PCI0.RP02.PXSX.WIST()) {
          Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 27)), 2)), 1)), 1)) // 27 - RP02
          Store (0x01, Index (DeRefOf(Index (DEVY, 27)), 1)) // 27 - RP02
        }

        // Device Constraints Enumeration
        If (LEqual(Arg2, One)) {
          If (LEqual(S0ID, 1)) {
            Return(DEVY)
          }
          Return(Package() {0})
        }
        // BCCD
        If (LEqual(Arg2, 2)) {
          Return(BCCD)
        }
        // Screen off notification
        If (LEqual(Arg2, 0x3)) {

        }

        // Screen on notification
        If (LEqual(Arg2, 0x4)) {

        }

        // resiliency phase entry (deep standby entry)
        If (LEqual(Arg2, 0x5)) {
          If (LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM(1) // 0x01 - Power State Standby (CS Entry)
          }
        }

        // resiliency phase exit (deep standby exit)
        If (LEqual(Arg2, 0x6)) {
          If (LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM(0) // 0x00 - Power State On (CS Exit)
          }
        }
      }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      Return(One)
    } // Method(_DSM)
  } //device (PEPD)
} // End Scope(\_SB)

