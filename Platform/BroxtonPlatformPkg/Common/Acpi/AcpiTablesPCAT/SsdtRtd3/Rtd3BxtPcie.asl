/** @file
  ACPI RTD3 SSDT table for BXT PCIe

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

    Name(WKEN, 0)

    Method(_S0W, 0)
    {
    /// This method returns the lowest D-state supported by PCIe root port during S0 state

     ///- PMEs can be generated from D3Cold for ULT
        Return(4)

    /** @defgroup pcie_s0W PCIE _S0W **/
    } // End _S0W


    Method(_DSW, 3)
    {
    /// This method is used to enable/disable wake from PCIe (WKEN)
      If (Arg1) { /// If entering Sx, need to disable WAKE# from generating runtime PME
        Store(0, WKEN)
      } Else {  /// If Staying in S0
        If (LAnd(Arg0, Arg2)) {
          ///- Check if Exiting D0 and arming for wake
          ///- Set PME
          Store(1, WKEN)
        } Else { ///- Disable runtime PME, either because staying in D0 or disabling wake
          Store(0, WKEN)
        }
      }

    /** @defgroup pcie_dsw PCIE _DSW **/
    } // End _DSW


    PowerResource(PXP, 0, 0)
    {
    /// Define the PowerResource for PCIe slot
    /// Method: _STA(), _ON(), _OFF()
    /** @defgroup pcie_pxp PCIE Power Resource **/

      Method(_STA, 0)
      {
      /// Returns the status of PCIe slot core power

        // detect power pin status
        if (LNotEqual(DeRefOf(Index(PWRG, 0)),0)) {
          if (LEqual(DeRefOf(Index(PWRG, 0)),1)) { // GPIO mode
            if (LEqual(\_SB.GGOV(DeRefOf(Index(PWRG, 2))),DeRefOf(Index(PWRG, 3)))) {
              Return (1)
            } Else {
              Return (0)
            }
          } // GPIO mode
        }
        // detect reset pin status
        if (LNotEqual(DeRefOf(Index(RSTG, 0)),0)) {
          if (LEqual(DeRefOf(Index(RSTG, 0)),1)) { // GPIO mode
            if (LEqual(\_SB.GGOV(DeRefOf(Index(RSTG, 2))),DeRefOf(Index(RSTG, 3)))) {
              Return (1)
            } Else {
              Return (0)
            }
          } // GPIO mode
        }
        Return (0)
      }  /** @defgroup pcie_sta PCIE _STA method **/

      Method(_ON,0,Serialized) /// Turn on core power to PCIe Slot
      {
        Name(PCIA, 0)
        //
        // Windows will call _ON for all devices,regardless
        // of the device enable state.
        // We need to exit when the device is not present
        // to prevent driving power to the device.
        //
        if (LEqual(SLOT, 1)) {
          ShiftRight(RPA1, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        } elseif(LEqual(SLOT, 2)) {
          ShiftRight(RPA2, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        } elseif(LEqual(SLOT, 3)) {
          ShiftRight(RPA3, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        } elseif(LEqual(SLOT, 4)) {
          ShiftRight(RPA4, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        } elseif(LEqual(SLOT, 5)) {
          ShiftRight(RPA5, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        } elseif(LEqual(SLOT, 6)) {
          ShiftRight(RPA6, 1, PCIA)
          Or(0xE0000000, PCIA, PCIA)
          PON(PCIA)
        }
      }

      Method(PON, 1, Serialized) {
        OperationRegion(PX02,SystemMemory, Arg0,0x380)
        Field(PX02,AnyAcc, NoLock, Preserve)
        {
          Offset(0),
          VD02, 32,
          Offset(0x50), // LCTL - Link Control Register
          L0SE, 1,      // 0, L0s Entry Enabled
          , 3,
          LDIS, 1,
          Offset(0x52), // LSTS - Link Status Register
          , 13,
          LASX, 1,      // 0, Link Active Status
          Offset(0xE2), // RPPGEN - Root Port Power Gating Enable
          , 2,
          L23E, 1,      // 2,   L23_Rdy Entry Request (L23ER)
          L23R, 1,      // 3,   L23_Rdy to Detect Transition (L23R2DT)
          Offset(0xF4), // BLKPLLEN
          , 10,
          BPLL, 1,
          Offset(0x324),
          , 3,
          LEDM, 1,       // PCIEDBG.DMIL1EDM
          Offset(0x338),
          , 26,
          BDQA, 1        // BLKDQDA
        }
        /// Turn ON Power for PCIe Slot
        if (LNotEqual(DeRefOf(Index(WAKG, 0)),0)) { // if power gating enabled
          if (LEqual(DeRefOf(Index(WAKG, 0)),1)) { // GPIO mode
            \_SB.SGOV(DeRefOf(Index(WAKG, 2)),DeRefOf(Index(WAKG, 3)))
            //  \_SB.SHPO(DeRefOf(Index(WAKG, 2)), 0) // set gpio ownership to ACPI(0=ACPI mode, 1=GPIO mode)
            Store(\_SB.GPC0(DeRefOf(Index(WAKG, 2))), Local1)
            Or(Local1, 0x400000, Local1)
            And(Local1, 0xFFFFFBFF, Local1)
            \_SB.SPC0(DeRefOf(Index(WAKG, 2)), Local1)
          }
        }

        If (LEqual(DeRefOf(Index(SCLK,0)), 1)) {
          Store(\_SB.GPC0(DeRefOf(Index(SCLK,1))), Local1)
          And(Local1, 0xFFBFFFFF, Local1)
          \_SB.SPC0(DeRefOf(Index(SCLK,1)), Local1)
        }

        \_SB.SGOV(DeRefOf(Index(RSTG, 2)),DeRefOf(Index(RSTG, 3)))

        Store(0, BDQA)  // Set BLKDQDA to 0
        Store(0, BPLL)  // Set BLKPLLEN to 0

        /// Set L23_Rdy to Detect Transition  (L23R2DT)
        Store(1, L23R)
        Sleep(16)
        Store(0, Local0)
        /// Wait up to 12 ms for transition to Detect
        While(L23R) {
          If(Lgreater(Local0, 4))    // Debug - Wait for 5 ms
          {
            Break
          }
          Sleep(16)
          Increment(Local0)
        }
      }

      Method(_OFF,0,Serialized) /// Turn off core power to PCIe Slot
      {
        Name(PCIA, 0)

        Switch(ToInteger(SLOT)) {
          Case(1) {
            ShiftRight(RPA1, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Case(2) {
            ShiftRight(RPA2, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Case(3) {
            ShiftRight(RPA3, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Case(4) {
            ShiftRight(RPA4, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Case(5) {
            ShiftRight(RPA5, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Case(6) {
            ShiftRight(RPA6, 1, PCIA)
            Or(0xE0000000, PCIA, PCIA)
          }
          Default {
          }
        }

        // Dynamic Opregion needed to access registers when the controller is in D3 cold
        OperationRegion(PX02,SystemMemory,PCIA,0x380)
        Field(PX02,AnyAcc, NoLock, Preserve)
        {
          Offset(0x50), // LCTL - Link Control Register
          L0SE, 1,      // 0, L0s Entry Enabled
          , 3,
          LDIS, 1,
          Offset(0xE2), // RPPGEN - Root Port Power Gating Enable
          , 2,
          L23E, 1,      // 2,   L23_Rdy Entry Request (L23ER)
          L23R, 1,       // 3,   L23_Rdy to Detect Transition (L23R2DT)
          Offset(0xF4),  // BLKPLLEN
          , 10,
          BPLL, 1,
          Offset(0x324),
          , 3,
          LEDM, 1,        // PCIEDBG.DMIL1EDM
          Offset(0x338),
          , 26,
          BDQA, 1         // BLKDQDA
        }
        /// Set L23_Rdy Entry Request (L23ER)
        Store(1, L23E)
        Sleep(16)
        Store(0, Local0)
        While(L23E) {
          If (Lgreater(Local0, 4)) {
            /// Debug - Wait for 5 ms
            Break
          }
          Sleep(16)
          Increment(Local0)
        }

        Store(1, BDQA)  // Set BLKDQDA to 1
        Store(1, BPLL)  // Set BLKPLLEN to 1

        if (LNotEqual(DeRefOf(Index(WAKG, 0)),0)) {
          // if power gating enabled
          if (LEqual(DeRefOf(Index(WAKG, 0)),1)) {
            // GPIO mode
            Store(\_SB.GPC0(DeRefOf(Index(WAKG, 2))), Local1)
            Or(Local1, 0x400, Local1)
            And(Local1, 0xFFBFFFFF, Local1)
            \_SB.SPC0(DeRefOf(Index(WAKG, 2)), Local1)
          }
        }

        /// Assert Reset Pin
        if (LNotEqual(DeRefOf(Index(RSTG, 0)),0)) {
          // if reset pin enabled
          if (LEqual(DeRefOf(Index(RSTG, 0)),1)) {
            // GPIO mode
            \_SB.SGOV(DeRefOf(Index(RSTG, 2)),Xor(DeRefOf(Index(RSTG, 3)),1))
          }
        }


        /// assert CLK_REQ MSK
        if (LEqual(DeRefOf(Index(SCLK, 0)),1)) {
          // if power gating enabled
          Store(\_SB.GPC0(DeRefOf(Index(SCLK, 1))), Local1)
          Or(Local1, 0x400000, Local1)
          \_SB.SPC0(DeRefOf(Index(SCLK, 1)), Local1)
        }

        /** @defgroup pcie_off PCIE _OFF method **/
      } // End of Method_OFF
    } // End PXP

  Name(_PR0, Package(){PXP})
  Name(_PR3, Package(){PXP})

  //
  // _SxW, in Sx, the lowest power state supported to wake up the system
  // _SxD, in Sx, the highest power state supported by the device
  // If OSPM supports _PR3 (_OSC, Arg3[2]), 3 represents D3hot; 4 represents D3cold, otherwise 3 represents D3.
  //

  Method (_S3D, 0, Serialized)
  {
    Return (0x2)
  }
  Method (_S3W, 0, Serialized)
  {
    Return (0x3)
  }
  Method (_S4D, 0, Serialized)
  {
    Return (0x2)
  }
  Method (_S4W, 0, Serialized)
  {
    Return (0x3)
  }


