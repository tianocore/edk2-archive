/** @file
  ACPI RTD3 SSDT table for APLK

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "Rtd3.aml",
  "SSDT",
  2,
  "AcpiRef",
  "RVPRtd3",
  0x1000
  )
{
External(RTD3, IntObj)
External(EMOD, IntObj)
External(RCG0, IntObj)
External(RPA1, IntObj)
External(RPA2, IntObj)
External(RPA3, IntObj)
External(RPA4, IntObj)
External(RPA5, IntObj)
External(RPA6, IntObj)
External(\_SB.GPO0.AVBL, IntObj)
External(\_SB.GPO0.ODPW, IntObj)
External(\_SB.GGOV, MethodObj)
External(\_SB.SGOV, MethodObj)
External(OSYS)
External(HGEN)
External(S0ID)

Include ("BxtPGpioDefine.asl")
Include ("Rtd3Common.asl")

//
// PCIe root ports - START
//
  ///
  /// PCIe RTD3 - SLOT#1
  ///
  Scope(\_SB.PCI0.RP01)
  {
    // reset pin = N_GPIO_13
    // power pin = N_GPIO_17
    // wake pin  = SW_GPIO_207
    // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC2 (MSKCRQSRC2):
    Name(SLOT, 1)   // port #1

    Name(RSTG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
      N_GPIO_13, // GPIO pad #/IOEX pin #
              1  // reset pin de-assert polarity
    })
    Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
      N_GPIO_17, // GPIO pad #/IOEX pin #
              1  // power on polarity
    })
    Name(WAKG, Package() {
                0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                0, // IOEX #
      SW_GPIO_207, // MSKCRQSRC2
                0  // wake en polarity
    })
    Name(SCLK, Package() {
               0, // 0-disable, 1-enable
      W_GPIO_211, // MSKCRQSRC2
               0  // assert polarity, ICC should be LOW-activated
    })

  Include("Rtd3BxtPcie.asl")
  }

  ///
  /// PCIe RTD3 - SLOT#2
  ///
  Scope(\_SB.PCI0.RP02)
  {
    Name (PCA2, 0)
    // reset pin = N_GPIO_15
    // power pin = N_GPIO_17
    // wake pin  = SW_GPIO_208
    // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC1 (MSKCRQSRC1):
    Name(SLOT, 2)   // port #2

    Name(RSTG, Package() {
              1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
      N_GPIO_15, // GPIO pad #/IOEX pin #
              1  // reset pin de-assert polarity
    })
    Name(PWRG, Package() {
              0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
              0, // IOEX #
      N_GPIO_17, // GPIO pad #/IOEX pin #
              1  // power on polarity
    })
    Name(WAKG, Package() {
                0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                0, // IOEX #
      SW_GPIO_208, // GPIO pad #/IOEX pin #
                1  // wake en polarity
    })
    Name(SCLK, Package() {
               1,   // 0-disable, 1-enable
      W_GPIO_212,   // MSKCRQSRC1
               0    // assert polarity, ICC should be LOW-activated
    })

    Include("Rtd3BxtPcie.asl")
  }

  ///
  /// PCIe RTD3 - SLOT#3
  ///
  If (LEqual (HGEN, 0)) {
    Scope(\_SB.PCI0.RP03)
    {
      Name (PCA3, 0)
      // reset pin = W_GPIO_152
      // power pin = N_GPIO_19
      // wake pin  = SW_GPIO_205
      // CLK_REQ   = W_GPIO_209
      Name(SLOT, 3)   // port #3

      Name(RSTG, Package() {
                 1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                 0, // IOEX #
        W_GPIO_152, // GPIO pad #/IOEX pin #
                 1  // reset pin de-assert polarity
      })
      Name(PWRG, Package() {
                1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                0, // IOEX #
        N_GPIO_19, // GPIO pad #/IOEX pin #
                1  // power on polarity
      })
      Name(WAKG, Package() {
                  1, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                  0, // IOEX #
        SW_GPIO_205, // GPIO pad #/IOEX pin #
                  1  // wake en polarity
      })
      Name(SCLK, Package() {
                 1, // 0-disable, 1-enable
        W_GPIO_209, // MSKCRQSRCx
                 0  // assert polarity, ICC should be LOW-activated
      })
      Include("Rtd3BxtPcie.asl")
    }
  }

  ///
  /// PCIe RTD3 - SLOT#4
  ///
  Scope(\_SB.PCI0.RP04)
  {
    Name (PCA4, 0)
    // reset pin = none
    // power pin = none
    // wake pin  = none
    // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC5 (MSKCRQSRC5)
    Name(SLOT, 4)   ///- Slot #4

    Name(RSTG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // reset pin de-assert polarity
        })
    Name(PWRG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // power on polarity
        })
    Name(WAKG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // wake en polarity
        })
    Name(SCLK, Package() {
          0,    // 0-disable, 1-enable
          0, // MSKCRQSRC5
          0     // assert polarity, ICC should be LOW-activated
        })

    Include("Rtd3BxtPcie.asl")
  }

  ///
  /// PCIe RTD3 - SLOT#5
  ///
  Scope(\_SB.PCI0.RP05)
  {
    Name (PCA5, 0)
    // reset pin = none
    // power pin = none
    // wake pin  = none
    // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC4 (MSKCRQSRC4)
    Name(SLOT, 5)   ///- port #5

    Name(RSTG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // reset pin de-assert polarity
        })
    Name(PWRG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // power on polarity
        })
    Name(WAKG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // wake en polarity
        })
    Name(SCLK, Package() {
          0,    // 0-disable, 1-enable
          0, // MSKCRQSRC4
          0     // assert polarity, ICC should be LOW-activated
        })

    Include("Rtd3BxtPcie.asl")
  }

  ///
  /// PCIe RTD3 - SLOT#6
  ///
  Scope(\_SB.PCI0.RP06)
  {
    Name (PCA6, 0)
    // reset pin = none
    // power pin = none
    // wake pin  = none
    // CLK_REQ   = Mask Enable of Mapped CLKREQ# for CLKOUT_SRC4 (MSKCRQSRC4)
    Name(SLOT, 6)   ///- port #6

    Name(RSTG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // reset pin de-assert polarity
        })
    Name(PWRG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // power on polarity
        })
    Name(WAKG, Package() {
            0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
            0, // IOEX #
            0, // GPIO pad #/IOEX pin #
            0  // wake en polarity
        })
    Name(SCLK, Package() {
          0,    // 0-disable, 1-enable
          0, // MSKCRQSRC4
          0     // assert polarity, ICC should be LOW-activated
        })

    Include("Rtd3BxtPcie.asl")
  }
//
// PCIe root ports - END
//

//
// SATA - START
//

  Scope(\_SB.PCI0.SATA) {

    OperationRegion (PMCS, PCI_Config, 0x74, 0x4)
    Field (PMCS, WordAcc, NoLock, Preserve) {
      PMSR, 32,   // 0x74, PMCSR - Power Management Control and Status
    }

    /// _PS0 Method for SATA HBA
    Method(_PS0,0,Serialized)
    {

    }

    /// _PS3 Method for SATA HBA
    Method(_PS3,0,Serialized)
    {
    }

    /// Define SATA PCI Config OperationRegion
    OperationRegion(SMIO,PCI_Config,0x24,4)
    Field(SMIO,AnyAcc, NoLock, Preserve) {
      Offset(0x00), ///- SATA MABR6
      MBR6, 32,     ///- SATA ABAR
    }

    Scope(PRT0) {
      // Define _PR0, _PR3 PowerResource Package
      // P0 command port = ABAR + 0x118
      // power pin = N_GPIO_22
      Name(PORT, 0)
      Name(PBAR, 0x118)       // port0 command port address
      Name(PWRG, Package() {
                 1,         // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                 0,         // GPIO group #/IOEX #
                 N_GPIO_22, // GPIO pad #/IOEX pin #
                 1          // power on polarity
               })
        Include("Rtd3BxtSata.asl")
    } // end device(PRT0)

    Scope(PRT1) {
      // Define _PR0, _PR3 PowerResource Package
      // P1 command port = ABAR + 0x198
      // power pin = none
      Name(PORT, 1)
      Name(PBAR, 0x198)       // port1 command port address
      Name(PWRG, Package() {
                 0, // 0-disable, 1-enable GPIO, 2-enable IOEX(IO Expander)
                 0, // GPIO group #/IOEX #
                 0, // GPIO pad #/IOEX pin #
                 0  // power on polarity
               })
      Include("Rtd3BxtSata.asl")
    } // end device(PRT1)

    ///
    /// _DSM Device Specific Method supporting AHCI DEVSLP
    ///
    /// Arg0: UUID Unique function identifier \n
    /// Arg1: Integer Revision Level \n
    /// Arg2: Integer Function Index \n
    /// Arg3: Package Parameters \n
    ///
    /// Variables:
    Name(DRV, 0)  /// Storage for _DSM Arg3 parameter 0
    Name(PWR, 0)  /// Storage for _DSM Arg3 parameter 1

    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {

    /// Check UUID of _DSM defined by Microsoft
      If (LEqual(Arg0, ToUUID ("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
        ///
        /// Switch by function index
        ///
        Switch (ToInteger(Arg2)) {
          Case (0) {  /// case 0:
            ///- Standard query - A bitmask of functions supported
            ///- Supports function 0-3
            Return(0x0f)
          }
          Case (1) {  /// case 1:
            ///- Query Device IDs (Addresses) of children where drive power and/or DevSleep are supported.
            ///- SATA HBA provides autonomous link (DevSleep) support, return a package of 0 elements
            Return( Package(){})     ///- SATA HBA provides native DevSleep
          }
          Case (2) { /// Case 2: Control power to device.
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Store(ToInteger(DerefOf(Index(Arg3, One))), PWR)

            Switch(ToInteger(DRV)){
              Case (0x0000FFFF){
                If (LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012))) {
                  ///- Turn on drive and link power to all drives. Note that even though
                  If(PWR){ // Applying Power
                    \_SB.PCI0.SATA.PRT0.ODPR._ON() ///- apply power to port 0
                  }
                } Else {
                  If(PWR){ // Applying Power
                    \_SB.PCI0.SATA.PRT0.SPPR._ON() ///- apply power to port 0
                  }
                }
              }
            } //Switch(DRV)
            Return (0)
          } //Case (2)
          Case (3){ /// Case 3: Current status of Device/Link of Port
            Store(ToInteger(DerefOf(Index(Arg3, Zero))), DRV)
            Switch(ToInteger(DRV)){  /// Check for SATA port
              Case (0x0000FFFF) {  ///- SATA Port 0
                ///- Bit0 => Device power state
                If (LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012))) {
                  If(LEqual(\_SB.PCI0.SATA.PRT0.ODPR._STA(), 0)){
                    Store(0, Local0)
                  }Else{
                    Store(1, Local0)
                  }
                } Else {
                  ///- Bit0 => Device power state
                  If (LEqual(\_SB.PCI0.SATA.PRT0.SPPR._STA(), 0)){
                    Store(0, Local0)
                  } Else {
                    Store(1, Local0)
                  }
                }
                Return (Local0)
              }
              Case (0x0001FFFF){  ///- SATA Port 1
                ///- Bit0 => Device power state
                If (LEqual(\_SB.PCI0.SATA.PRT1.SPPR._STA(), 0)) {
                  Store(0, Local0)
                } Else {
                  Store(1, Local0)
                }
                Return (Local0)
              }
              Default { ///- Invalid SATA Port - error
                Return (Ones)
              }
            }
          }
          Default {
            Return (0)
          }
        }
      } Else { // UUID does not match
        Return (0)
      }
    } /// @defgroup sata_dsm SATA _DSM Method

  } //Scope(\_SB.PCI0.SATA)
//
// SATA - END
//

  Scope(\_SB.PCI0.XHC.RHUB) { //USB XHCI RHUB
    //
    // No specific power control (GPIO) to USB connectors (p2/p4/p5),
    // declare power resource with null functions and specify its wake ability
    // Please refer to ACPI 5.0 spec CH7.2
    //
    Scope (HS02) {
      Name(PORT, 2)
      Include ("Rtd3BxtUsb.asl")
    }
    Scope (SSP2) {
      Name(PORT, 10)
      Include ("Rtd3BxtUsb.asl")
    }

    Scope (HS04) {
      Name(PORT, 4)
      Include ("Rtd3BxtUsb.asl")
    }
    Scope (SSP4) {
      Name(PORT, 12)
      Include ("Rtd3BxtUsb.asl")
    }

    Scope (HS05) {
      Name(PORT, 5)
      Include ("Rtd3BxtUsb.asl")
    }
    Scope (SSP5) {
      Name(PORT, 13)
      Include ("Rtd3BxtUsb.asl")
    }

    ///
    /// WWAN RTD3 support, associate _PR0, PR2, PR3 for USB High speed Port 3
    Scope (HS03) {
      Name(PORT, 3)
      Include ("Rtd3BxtUsbWwan.asl")
    }

  } // RHUB

  Scope(\_SB){
    PowerResource(SDPR, 0, 0) {
      Name(_STA, One)
      Method(_ON, 0, Serialized) {

      }

      Method(_OFF, 0, Serialized) {

      }

    } //End of PowerResource(SDPR, 0, 0)
  } //End of Scope(\_SB)

   Scope(\_SB.PCI0.SDHA)
   {

     Name(_PR0, Package(){SDPR})   // TBD
     Name(_PR3, Package(){SDPR})   // TBD
   }  //Scope(\_SB.PCI0.SDHA)

   Scope(\_SB.PCI0.SDIO)
   {

     Name(_PR0, Package(){SDPR})   // TBD
     Name(_PR3, Package(){SDPR})   // TBD
   }  //Scope(\_SB.PCI0.SDIO)

    Scope(\_SB)
    {
      //
      // Dummy power resource for USB D3 cold support
      //
      PowerResource(USBC, 0, 0)
      {
        Method(_STA) { Return (0x1) }
        Method(_ON) {}
        Method(_OFF) {}
      }
    }

    Scope(\_SB.PCI0.XDCI)
    {
      Method (_RMV, 0, NotSerialized)  // _RMV: Removal Status
      {
        Return (Zero)
      }

      Method (_PR3, 0, NotSerialized)  // _PR3: Power Resources for D3hot
      {
        Return (Package (0x01)
        {
          USBC // return dummy package
        })
      }

      OperationRegion (PMCS, PCI_Config, 0x74, 0x4)
      Field (PMCS, WordAcc, NoLock, Preserve) {
        PMSR, 32,   // 0x74, PMCSR - Power Management Control and Status
      }

      Method (_PS0, 0, NotSerialized) { // _PS0: Power State 0
      }

      Method (_PS3, 0, NotSerialized) { // _PS3: Power State 3
        //
        // dummy read PMCSR
        //

        Store (PMSR, Local0)
        And (Local0, 1, Local0) // Dummy operation on Local0
      }
    } // Scope(\_SB.PCI0.XDCI)

//
// XDCI - end
//

//Power Resource for Audio Codec
    Scope(\_SB.PCI0)
    {
      PowerResource(PAUD, 0, 0) {

        /// Namespace variable used:
        Name(PSTA, One) /// PSTA: Physical Power Status of Codec 0 - OFF; 1-ON
        Name(ONTM, Zero) /// ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Name(_STA, One) /// _STA: PowerResource Logical Status 0 - OFF; 1-ON

        ///@defgroup pr_paud Power Resource for onboard Audio CODEC

        Method(_ON, 0){     /// _ON method \n

          Store(One, _STA)        ///- Set Logocal power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
                    ///@addtogroup pr_paud
        } // End _ON

        Method(_OFF, 0){    /// _OFF method \n

          Store(Zero, _STA)    ///- Set the current power state
          PUAM() ///- Call PUAM() to tansition Physical state to match current logical state
        ///@addtogroup pr_paud
        } // End _OFF

        ///  PUAM - Power Resource User Absent Mode for onboard Audio CODEC
        ///  Arguments:
        ///
        ///  Uses:
        ///      _STA - Variable updated by Power Resource _ON/_OFF methods \n
        ///      \\UAMS - Variable updated by GUAM method to show User absent present \n
        ///      ONTM - Local variable to store ON time during Speculative ON \n
        /// ______________________________
        // |  Inputs      |   Outputs    |
        // ______________________________
        // | _STA | \UAMS | GPIO | ONTM |
        // ______________________________
        // |   1  |   0   | ON   |   !0 |
        // |   1  |   !0  | ON   |   !0 |
        // |   0  |   0   | ON   |   !0 |
        // |   0  |   !0  | OFF  |   0  |
        // ______________________________
                    /**
                    <table>
                    <tr> <th colspan="2"> Inputs <th colspan="2"> Output
                    <tr> <th>_STA <th> \\UAMS <th> GPIO <th>ONTM
                    <tr> <td>1 <td>0 <td>ON <td>!0
                    <tr> <td>1 <td>!0<td>ON <td>!0
                    <tr> <td>0 <td>0 <td>ON <td>!0
                    <tr> <td>0 <td>!0<td>OFF<td> 0
                    </table>
                    **/
        ///@addtogroup pr_paud_puam
        Method(PUAM, 0, Serialized)
        {
         If (LEqual(^_STA, Zero)) {
          } Else { /// New state = ON (_STA=1) or (_STA=0 and \UAMS=0)
          }
        } //PUAM
      } //PAUD
    } //Scope(\_SB.PCI0)
//Power Resource for Audio Codec End

// I2C1 - TouchPanel Power control

//Serial IO End

  // Modern Standby N:1 Power Resource definition. Place Holder.
  If (LEqual(\EMOD, 1)) {
    Scope(\_SB){
      Name(GBPS, 0) // Power state flag for Modern Standby. Initial value = 1 (On).
      PowerResource(MODS, 0, 0){                  /// Modern Standby Power Resource

        Method(_STA) {  /// _STA method
          If (LGreaterEqual(OSYS, 2015)) {
            Return(GBPS)
          }
          Return(0)
        }

        Method(_ON, 0) {                            /// _ON Method
          If (LGreaterEqual(OSYS, 2015)) {
            \_SB.PCI0.SATA.PRT0.SPON()
            Store(1, GBPS) // Indicate devices are ON
          }
        }

        Method(_OFF, 0) {                           /// _OFF Method
          If (LGreaterEqual(OSYS, 2015)) {
            \_SB.PCI0.SATA.PRT0.SPOF()
            Store(0, GBPS)
          }
        }
      } // End MODS
    } // \_SB
  } // End If ((LEqual(EMOD, 1))

  Scope(\_SB.PCI0) {
    Method(LPD3, 0, Serialized) {
      //
      // Memory Region to access to the PCI Configuration Space
      // and dummy read PMCSR
      //
      OperationRegion (PMCS, PCI_Config, 0x84, 0x4)
      Field (PMCS, WordAcc, NoLock, Preserve) {
        PMSR, 32,   // 0x84, PMCSR - Power Management Control and Status
      }
      //
      // Dummy PMCSR read
      //
      Store (PMSR, Local0)
      And (Local0, 1, Local0) // Dummy operation on Local0
    }

    PowerResource (LSPR, 0, 0) {
      //
      // LPSS Power Resource
      //
      Name (_STA, One)
      Method (_ON, 0, Serialized) {

      }

      Method (_OFF, 0, Serialized) {

      }
    } //End of PowerResource(SDPR, 0, 0)
  }

  Scope(\_SB.PCI0.PWM)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.PWM)

  Scope(\_SB.PCI0.I2C0)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C0)

  Scope(\_SB.PCI0.I2C1)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C1)

  Scope(\_SB.PCI0.I2C2)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C2)

  Scope(\_SB.PCI0.I2C3)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C3)

  Scope(\_SB.PCI0.I2C4)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C4)

  Scope(\_SB.PCI0.I2C5)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C5)

  Scope(\_SB.PCI0.I2C6)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C6)

  Scope(\_SB.PCI0.I2C7)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.I2C7)

  Scope(\_SB.PCI0.SPI1)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.SPI1)

  Scope(\_SB.PCI0.SPI2)
  {
    //
    // Dummy Power Resource declaration
    //
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.SPI2)

  Scope(\_SB.PCI0.SPI3)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.SPI3)

  Scope(\_SB.PCI0.URT1)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.URT1)

  Scope(\_SB.PCI0.URT2)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) { LPD3 () }
  }  //Scope(\_SB.PCI0.URT2)

  Scope(\_SB.PCI0.URT3)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.URT3)

  Scope(\_SB.PCI0.URT4)
  {
    Name(_PR0, Package(){LSPR})  // TBD
    Name(_PR3, Package(){LSPR})  // TBD
    Method (_PS0) { }
    Method (_PS3) {LPD3 ()}
  }  //Scope(\_SB.PCI0.URT4)

} // End SSDT

