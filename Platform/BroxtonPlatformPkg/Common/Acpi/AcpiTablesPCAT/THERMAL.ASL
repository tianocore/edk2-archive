/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_TZ)
{
  //
  // Memory window to MCHBAR+7000h.
  //
  OperationRegion (MBAR, SystemMemory, Add(ShiftLeft(\_SB.PCI0.VLVC.MHBR,15),0x7000), 0x100)
  Field (MBAR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x1C), // P_CR_PKG_THERM_STATUS_0_0_0_MCHBAR (MCHBAR+0x701C)
      ,      1,       // THRESHOLD1_STATUS[0]
      ,      1,       // THRESHOLD1_LOG[1]
      ,      1,       // THRESHOLD2_STATUS[2]
      ,      1,       // THRESHOLD2_LOG[3]
      ,      1,       // CRITICAL_THERMAL_EVENT_STATUS[4]
      ,      1,       // CRITICAL_THERMAL_EVENT_LOG[5]
      ,     10,       // RESERVED[15:6]
      PKGT,  8,       // TEMPERATURE[23:16]
  }

  // Notes:
  //  1) WIN2K strictly uses interrupt driven thermal events.
  //  2) Temperature values are stored in tenths of Kelvin to eliminate the decimal place.
  //  3) Kelvin = Celsius + 273.2.
  //  4) All temperature must be >= 289K.

  // Flag to indicate Legacy thermal control.
  // 1 = Legacy thermal control
  // 0 = other thermal control, like DPTF
  Name(LEGA, 1) // for Active Policy
  Name(LEGP, 1) // for Passive Policy
  Name(LEGC, 1) // for Critical Policy

  // Fan 0 = Package Processor Fan.

  PowerResource(FN00,0,0)
  {
    // Return Virtual Fan 0 status.
    //
    Method(_STA,0,Serialized)
    {
      Return(0x0F)
    }

    Name(FANS,0) // Fan State: 1=ON, 0=OFF

    Method(FCTL,1,Serialized)
    {

    }

    Method(_ON,0,Serialized)
    {
      // If EC access is enabled.
      If (\_TZ.LEGA) {
        Store(1,FANS) // save fan state as on
        FCTL(100)
      }
    }

    Method(_OFF,0,Serialized)
    {
      // If EC access is enabled.
      If (\_TZ.LEGA) {
        Store(0,FANS) // save fan state as off
        FCTL(0)
      }
    }
  }

  // Associate Virtual Fan 0 Power Resource with the FAN0 Device.
  //
  Device(FAN0)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID,0)
    Name(_PR0, Package(1){FN00})
  }

  // Thermal Zone 1 = DTS Thermal Zone.
  //
  ThermalZone(TZ01)
  {
    // Return the temperature at which the OS initiates Active Cooling.
    //
    Method(_AC0,0,Serialized)
    {
      If (LEqual(\_TZ.LEGA,0)) { Return(Add(2732,Multiply(210,10)))} // 210 degree C
      Return(Add(2732,Multiply(ACTT,10)))
    }

    // Returns the Device to turn on when _ACx is exceeded.
    //
    Name(_AL0, Package(1){FAN0})

    // Return the temperature at which the OS must perform a Critcal Shutdown.
    //
    Method(_CRT,0,Serialized)
    {
      If (LEqual(\_TZ.LEGC,0)) { Return(Add(2732,Multiply(210,10)))} // 210 degree C
      Return(Add(2732,Multiply(\CRTT,10)))
    }

    // Notifies ASL Code the current cooling mode.
    //  0 - Active cooling
    //  1 - Passive cooling
    //
    Method(_SCP,1,Serialized)
    {
      Store(Arg0,\CTYP)
    }

    // _TMP (Temperature)
    //
    // Return the highest of the CPU temperatures to the OS.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP,0,Serialized)
    {
      //
      // DTS Enabled Case
      //
      If (DTSE) {
        //
        // If DTS support is enabled, simply return the
        // higher of the two DTS Temperatures.
        If (LGreaterEqual(DTS1, DTS2)) {
          Return(Add(2732, Multiply(DTS1, 10)))
        }
        Return(Add(2732, Multiply(DTS2, 10)))
      }
      //
      // Max Platform temperature returned by EC
      //
      Else{
        Return (Add(2732,Multiply(PKGT,10))) // this depends on EC sending a SCI every second to simulate polling.
      }
    } // End of _TMP

    // Return the Processor(s) used for Passive Cooling.
    //
    Method(_PSL,0,Serialized)
    {
      If (LEqual(MPEN, 4)) {
        //  CMP - Throttling controls all four logical CPUs.
        Return(Package(){\_PR.CPU0,\_PR.CPU1,\_PR.CPU2,\_PR.CPU3})
      }
      If (MPEN) {
        //  CMP - Throttling controls both CPUs.
        Return(Package(){\_PR.CPU0,\_PR.CPU1})
      }
      Return(Package(){\_PR.CPU0})
    }

    // Returns the temperature at which the OS initiates CPU throttling.
    //
    Method(_PSV,0,Serialized)
    {
      If (LEqual(\_TZ.LEGP,0)){ Return(Add(2732,Multiply(110,10)))} // 110 degree C
      Return(Add(2732,Multiply(\PSVT,10)))
    }

    // Returns TC1 value used in the passive cooling formula.
    //
    Method(_TC1,0,Serialized)
    {
      Return(\TC1V)
    }

    // Returns TC2 value used in the passive cooling formula.
    //
    Method(_TC2,0,Serialized)
    {
      Return(\TC2V)
    }

    // Returns the sampling period used in the passive cooling formula.
    //
    Method(_TSP,0,Serialized)
    {
      Return(\TSPV)
    }

  }// end ThermalZone(TZ01)
} // end Scope(\_TZ)
