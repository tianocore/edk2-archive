/** @file
  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.IETM)
{
  // PTTL (Participant Temperature Tolerance Level)
  // This object evaluates to an integer representing the temperature range within which any change
  // in participant temperature is considered acceptable and no policy actions will be taken by the
  // policy. The temperature tolerance level is expressed in the units of 10s of Kelvin.
  //
  Name (PTTL, 20) // ToleranceLevel
  Name (PSVT, Package()
  {
    2,
    Package(){\_SB.PCI0.TCPU, \_SB.SEN1, 2, 300, 3032,  9, 0x00010000, 12000, 500, 10, 20, 0},
    Package(){\_SB.PCI0.TCPU, \_SB.SEN1, 2, 300, 3082,  9, 0x00010000,  9000, 500, 10, 20, 0},
    Package(){\_SB.PCI0.TCPU, \_SB.SEN1, 2, 300, 3132,  9, 0x00010000,  6000, 500, 10, 20, 0},
    Package(){\_SB.PCI0.TCPU, \_SB.SEN1, 1, 300, 3232,  9, 0x00010000, "MIN", 500, 10, 20, 0},
    Package(){\_SB.PCI0.TCPU, \_SB.PCI0.TCPU,1,50,3532, 9, 0x00010000, "MIN", 500, 10, 20, 0}
  })

}// end Scope(\_SB.IETM)

