/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB)
{
  Device(TPWR)  // Power participant
  {
    Name(_HID, EISAID("INT3407")) //Intel DPTF platform power device
    Name(_UID, "TPWR")
    Name(_STR, Unicode ("Platform Power"))
    Name(PTYP, 0x11)

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(\PWRE,1)) {
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _BST (Battery Status)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package containing the battery status in the format below:
    //   Package {
    //      Battery State
    //      Battery Present Rate
    //      Battery Remaining Capacity
    //      Battery Present Voltage
    //   }
    //
    Method(_BST,,,,PkgObj)
    {
      Return(Package(){0,0,0,0})
    }

    // _BIX (Battery Information Extended)
    //
    // The _BIX object returns the static portion of the Control Method Battery information.
    // This information remains constant until the battery is changed.
    // The _BIX object returns all information available via the _BIF object plus additional battery information.
    // The _BIF object is deprecated in lieu of _BIX in ACPI 4.0.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package containing the battery information as described below
    //   Package {
    //      Revision
    //      Power Unit
    //      Design Capacity
    //      Last Full Charge Capacity
    //      Battery Technology
    //      Design Voltage
    //      Design Capacity of Warning
    //      Design Capacity of Low
    //      Cycle Count
    //      Measurement Accuracy
    //      Max Sampling Time
    //      Min Sampling Time
    //      Max Averaging Interval
    //      Min Averaging Interval
    //      Battery Capacity Granularity 1
    //      Battery Capacity Granularity 2
    //      Model Number
    //      Serial Number
    //      Battery Type
    //      OEM Information
    //      Battery Swapping Capability
    //   }
    //
    Method(_BIX,,,,PkgObj)
    {
      Return(Package(){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"0","0","0","0",0})
    }

    // PSOC (Platform State of Charge)
    //
    // This object evaluates to the remaining battery state of charge in %.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   remaining battery charge in %
    //
    Method(PSOC)
    {

        Return (0) // EC not available


    }

    // PMAX (Platform MAXimum)
    //
    // The PMAX object provides maximum power that can be supported by the battery in mW.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   maximum power in mW
    //
    Method(PMAX,0,Serialized)
    {

       Return(0)

    }

    // NPWR (N PoWeR)
    //
    // True Platform Power: Could be AC supplied+Battery Pack supplied power or AC supplied.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Power required to charge battery in mW.
    //
    Method(NPWR)
    {
      Return(20000) // 20 watts
    }

    // PSRC (Power SouRCe)
    //
    // The PSRC object provides power source type.
    //
    // Arguments: (0)
    //   None
    // Return Value: (enumeration which DPTF driver expects)
    //   0 = DC
    //   1 = AC
    //   2 = USB
    //   3 = WC
    //
    // Notes: (Bitmap from EC)
    //   Bit0=1 if AC
    //   Bit1=1 if USB-PD
    //   Bit2=1 if Wireless Charger
    //
    Method(PSRC, 0, Serialized)
    {


        Return(0) // Default return is DC

    }

    // ARTG (Adapter RaTinG)
    //
    // The ARTG object provides AC adapter rating in mW.
    // ARTG should return 0 if PSRC is DC (0).
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   AC adapter rating in mW
    //
    Method(ARTG)
   {
     If (LEqual(PSRC(),1)) {
       // AC check
       Return(90000) // 90 watts
     } Else {
       Return(0)
     }
   }

    // CTYP (Charger TYPe)
    //
    // The CTYP object provides charger type.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //  0x01 = Traditional
    //  0x02 = Hybrid
    //  0x03 = NVDC
    //
    Method(CTYP)
   {
      Return(3)
   }

    // PROP (Platform Rest Of worst case Power)
    //
    // This object provides maximum worst case platform rest of power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(PROP)
   {
      Return(25000) // 25 watts
   }

    // APKP (Ac PeaK Power)
    //
    // This object provides maximum adapter power output.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(APKP)
   {
      Return(90000) // 90 watts
   }

    // APKT (Ac PeaK Time)
    //
    // This object provides the maximum time the adapter can maintain peak power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   time in milliseconds
    //
    Method(APKT)
   {
      Return(10) // 10 milliseconds
   }

    // PBSS (Power Battery Steady State)
    //
    // Returns max sustained power for battery.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(PBSS)
    {
    }

    // DPSP (DPTF Power Sampling Period)
    //
    // Sets the polling interval in 10ths of seconds. A value of 0 tells the driver to use event notification for PMAX and PBSS..
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the polling rate in tenths of seconds.
    //   A value of 0 will specify using interrupts through the ACPI notifications.
    //
    //   The granularity of the sampling period is 0.1 seconds. For example, if the sampling period is 30.0
    //   seconds, then report 300; if the sampling period is 0.5 seconds, then will report 5.
    //
    Method(DPSP,0,Serialized)
    {
      Return(\PPPR)
    }

  } // End TPWR Device
}// end Scope(\_SB)

