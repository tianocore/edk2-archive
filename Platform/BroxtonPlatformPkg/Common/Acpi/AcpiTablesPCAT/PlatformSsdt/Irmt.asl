/** @file
  ACPI interface for Intel Ready Mode Technology(IRMT)

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "IrmtAcpi.aml",
  "SSDT",
  2,
  "Intel_",
  "IrmtTabl",
  0x1000
  )
{
  External(IRMC) // Irmt Configuration
  External(\_SB.PCI0.GFX0.TCHE)                 // Technology enabled indicator
  External(\_SB.PCI0.GFX0.STAT)                 // State Indicator

  Scope (\_SB) {
    Device (AOAA) {
      Name (_HID, "INT3398")

      Name (NSMD, 0x00)  // Used to store mode settings:
      /**
        Bits        Description
        - (1:0)     00: Leave Never Sleep Mode;
                    01: Enter Never Sleep Mode.
                    10: Power Reduction Mode
                    11: Enter Do Not Disturb mode.
        - (7:2)     Reserved: Set to 0.
      **/

      Method (_STA, 0)
      {
        //
        //The Intel RMT ACPI device is a hidden device and
        //listed under system devices on Device Manager.
        //
        Return (0x0B)
      }

      /**
        GNSC - Get Never Sleep Configuration
        Input:   None
        Return:
        Bits    Description
        - 0      State(Never Sleep Enabling Status in BIOS): 0 = Disabled, 1 = Enabled
        - 1      Notification(Hardware notification enabling status): 0 = Unsupported, 1 = Supported
        - (7:2)  Reserved: Set to 0.
      **/
      Method (GNSC, 0, NotSerialized, 0, IntObj) {
        Return (IRMC)
      }

      /**
        GNSM - Get Never Sleep Mode
        Input:   None
        Return:
        Bits        Description
        - (1:0)     00: Leave Never Sleep Mode;
                    01: Enter Never Sleep Mode.
                    10: Power Reduction Mode
                    11: Enter Do Not Disturb mode.
        - (7:2)     Reserved: Set to 0.
      **/
      Method (GNSM, 0, NotSerialized, 0, IntObj) {
        Return (NSMD)
      }

      /**
        SNSM - Set Never Sleep Mode
        Input:
        Bits        Description
        - (1:0)     00: Leave Never Sleep Mode;
                    01: Enter Never Sleep Mode.
                    10: Power Reduction Mode
                    11: Enter Do Not Disturb mode.
        - (7:2)     Reserved: Set to 0.
        Return:
      **/
      Method (SNSM, 1, NotSerialized, 0, UnknownObj, IntObj) {
        And(Arg0, 0x3, NSMD)
      }

    } // Device (AOAA)
  } // Scope (\_SB)
} // End SSDT


