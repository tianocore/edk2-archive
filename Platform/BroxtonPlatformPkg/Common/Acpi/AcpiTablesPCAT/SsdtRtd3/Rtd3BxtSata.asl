/** @file
  ACPI RTD3 SSDT table for BXT SATA

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  Method(_DSW, 3, Serialized)
  {
  }

  /// _S0W method returns the lowest D-state can be supported in state S0.
  Method(_S0W, 0)
  {
    Return(4) // return 4 (D3cold)
  } // End _S0W

  Method(_PR0) {
    If (LAnd(LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012)), LEqual(PORT, 0))) {
      // Return ODPR for ZPODD
      Return(Package(){ODPR})
    } ElseIf (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
      If (CondRefOf(\_SB.MODS)) {
        Return(Package(){\_SB.MODS})
      }
    } Else {
      If (CondRefOf(SPPR)) {
        Return(Package(){SPPR})
      }
    }
    Return(Package() {})
  }
  Method(_PR3) {
    If (LAnd(LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012)), LEqual(PORT, 0))) {
      // Return ODPR for ZPODD
      Return(Package(){ODPR})
    } ElseIf (LAnd(LGreaterEqual(\OSYS, 2015), LEqual(\EMOD, 1))) {
      If (CondRefOf(\_SB.MODS)) {
        Return(Package(){\_SB.MODS})
      }
    } Else {
      If (CondRefOf(SPPR)) {
        Return(Package(){SPPR})
      }
    }
    Return(Package() {})
  }

  PowerResource(SPPR, 0, 0)
  {
    Method(_STA)
    {
      If (LEqual(\EMOD, 1)) {
        Return(0x01)
      } Else {
        Return(SPSA())
      }
    }

    Method(_ON) {
      If (LEqual(\EMOD, 1)) {
        // Do nothing
      } Else {
        SPON()    // _ON Method
        Sleep(16) // Delay for power ramp.
      }
    }

    Method(_OFF) {
      If (LEqual(\EMOD, 1)) {
        // Do nothing
      } Else {
        SPOF()
      }
    }
  } // end SPPR

  Name(OFTM, Zero) /// OFTM: Time returned by Timer() when the Power resource was turned OFF
  Method(SPSA) {

    // check power control enable
    If (LNotEqual(DeRefOf(Index(PWRG, 0)), 0)) {
      // read power pin status
      If (LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
        if (LEqual(\_SB.GGOV(DeRefOf(Index(PWRG, 2))), DeRefOf(Index(PWRG, 3)))) {
          Return(0x01)
        } Else {
          Return(0x00)
        }
      }
    }
    Return(0x00) // disabled
  } /// @defgroup sata_prt1_sta     SATA Port 1 PowerResource _STA Method

  Method(SPON, 0) {
    If (LNotEqual(^OFTM, Zero)) { /// if OFTM != 0 => Disk was turned OFF by asl
      Divide(Subtract(Timer(), ^OFTM), 10000, , Local0) ///- Store Elapsed time in ms
      Store(Zero, ^OFTM) ///- Reset OFTM to zero to indicate minimum 50ms requirement does not apply when _ON called next time
      If (LLess(Local0, 50)) ///- Do not sleep if already past the delay requirement
      {
        Sleep(Subtract(50, Local0)) ///- Sleep 50ms - time elapsed
      }
    }
    // drive power pin "ON"
    if (LNotEqual(DeRefOf(Index(PWRG, 0)),0)) {     // if power gating enabled
      if (LEqual(DeRefOf(Index(PWRG, 0)),1)) {      // GPIO mode
        \_SB.SGOV(DeRefOf(Index(PWRG, 2)),DeRefOf(Index(PWRG, 3)))
      }
    }
  }

  Method(SPOF, 0) {

    Add(\_SB.PCI0.SATA.MBR6, PBAR, Local0)
    /// if S0Ix enabled
    If (LEqual(S0ID, 1)) {
      OperationRegion(PSTS, SystemMemory, Local0, 0x18)
      Field(PSTS, DWordAcc, NoLock, Preserve)
      {
        Offset(0x0),
        CMST, 1,  //PxCMD.ST
        CSUD, 1,  //PxCMD.SUD
        , 2,
        CFRE, 1,  //PxCMD.FRE
        Offset(0x10),
        SDET, 4,  //PxSSTS.DET
        Offset(0x14),
        CDET, 4   //PxSCTL.DET
      }

      // Execute offline flow only if Device detected and Phy not offline
      If (LOr(LEqual(SDET, 1), LEqual(SDET, 3))) {

        ///- Clear ST (PxCMD.ST)
        Store(0, CMST)                   // PBAR[0]
        ///- Clear FRE
        Store(0, CFRE)                   // PBAR[4]
        ///- Clear SUD (PxCMD.SUD)
        Store(0, CSUD)                   // PBAR[1]
        ///- Set DET to 4 (PxSCTL.DET)
        Store(4, CDET)                   // PBAR+0x14[3:0]
        Sleep(16)
        ///- Wait until PxSSTS.DET == 4
        While(LNotEqual(SDET, 4)){
          Sleep(16)
        }
      }
    } // if S0Ix enabled
    // drive power pin "OFF"
    If (LNotEqual(DeRefOf(Index(PWRG, 0)),0)) { // if power gating enabled
      if (LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
        \_SB.SGOV(DeRefOf(Index(PWRG, 2)),Xor(DeRefOf(Index(PWRG, 3)),1))
      }
    }

    Store(Timer(), ^OFTM) /// Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
  } // end _OFF

  //
  // Power Resource for RTD3 ZPODD
  //
  PowerResource(ODPR, 0, 0)
  {
    Method(_STA)
    {
      Return(ODSA())
    }

    Method(_ON)
    {
      ODON()    // _ON Method
      Sleep(800) // Delay for power ramp.
    }

    Method(_OFF)
    {
      ODOF()    // _OFF Method
    }

  } // end PZPR

  Method(ODSA)
  {
    // check power control enable
    // read power pin status
    If (LEqual(\_SB.GGOV(N_GPIO_22), 0x01)) {
      Return(0x01)
    } Else {
      Return(0x00)
    }
  }

  Method(ODON, 0)
  {
    // drive power pin "ON"
    If (LAnd(LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012)), LEqual(PORT, 0))) {
      Store (1, \_SB.GPO0.ODPW)
    }
  }

  Method(ODOF, 0)
  {
    // drive power pin "OFF"
    If (LAnd(LAnd(LAnd(And(RCG0, 0x01), 0x01), LGreaterEqual(\OSYS, 2012)), LEqual(PORT, 0))) {
      Store (0, \_SB.GPO0.ODPW)
    }
  }

