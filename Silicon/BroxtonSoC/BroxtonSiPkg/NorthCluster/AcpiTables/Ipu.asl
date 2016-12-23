/** @file
  This file contains the device definition of the System Agent
  ACPI reference code.
  Currently defines the device objects for the
  System Agent Camera IPU device.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\_SB.OSSL, IntObj)
External(\_SB.IPUD, IntObj)

//
// Device IPU is the Bxt IPU device
//

//
// Device IPU0 is the AVStream virtual device and it appears under _SB
//

Scope (\_SB) {
  Device(IPU0)
  {
    Name (_HID, "INT34D7") // This is an the ACPI ID of  AVStream device driver
    /*
      Allows OS to not yellow bang if Intel do not
      provide driver for that OS version, otherwise
      if we have driver for every OS, then no need
      for _CID at all.
    */
    Name(_CID, EISAID("PNP0C02"))
    /*
      The following is a technique that may be used (per OEM needs) to prevent
      the load of the camera device in one of the following cases:
      - Camera device and the CIO2 device are fused out
      - If the platform setup requires that in a secured boot the camera device
      should not be enabled
    */
    Method (_STA, 0, NotSerialized) {
      If (LAnd(LEqual(IPUD,2),And(OSSL,0))) { // ACPI need report IPU0 as available device for  WOS
        Return (0xF)
      } Else {             // ACPI should NOT report IPU0 as available device
        Return (0x0)
      }
    }
  }
}

//
// Device IPU0 is the  AVStream virtual device and it appears under GFX0
//
Scope (\_SB.PCI0.GFX0)
{
  Device(IPU0) //  AVStream virtual device name
  {
    /*
      The identifier for this device (Same as in
      _DOD above). This is required so GFX driver can
      associate a matching device ID for the AVStream
      driver and provide it to PnP (this device ID
      should appear in the INF file of the AVStream
      driver).
    */
    Name(_ADR, 0x000034D7)
    /*
      The following is a technique that may be used (per OEM needs) to prevent
      the load of the camera device in one of the following cases:
      - Camera device and the CIO2 device are fused out
      - If the platform setup requires that in a secured boot the camera device
      should not be enabled
    */
    Method (_STA, 0, NotSerialized) {
      If(LAnd(LEqual(IPUD,1),And(OSSL,0))) { // IGFX need report IPU0 as GFX0 child for WOS
        Return (0xF)
      } Else { // IGFX should NOT report IPU0 as GFX0 child
        Return (0x0)
      }
    }
  } // End IPU0
} // end I.G.D

