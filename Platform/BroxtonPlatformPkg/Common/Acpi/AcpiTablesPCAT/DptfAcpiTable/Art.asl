/** @file
  ACPI Active Cooling Relationship Table

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.IETM)
{
  // _ART (Active Cooling Relationship Table)
  //
  // Arguments:
  //   None
  // Return Value:
  //   A variable-length Package containing a Revision ID and a list of Active Relationship Packages as described below:
  //
  // Return Value Information
  //   Package {
  //   Revision, // Integer - Current revision is: 0
  //   ActiveRelationship[0] // Package
  //   ...
  //   ActiveRelationship[n] // Package
  //   }
  //
  Name(_ART, Package()
  {
    0,    // Revision
    //        Source     Target        Weight, AC0MaxLevel, AC1MaxLevel,  AC2MaxLevel,  AC3MaxLevel,  AC4MaxLevel,  AC5MaxLevel,  AC6MaxLevel,  AC7MaxLevel,  AC8MaxLevel,  AC9MaxLevel
    Package(){\_SB.TFN1, \_SB.PCI0.TCPU,  100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    Package(){\_SB.TFN1, \_SB.SEN1,       100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    Package(){\_SB.TFN1, \_SB.GEN1,       100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    Package(){\_SB.TFN1, \_SB.GEN2,       100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    Package(){\_SB.TFN1, \_SB.GEN3,       100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    Package(){\_SB.TFN1, \_SB.GEN4,       100,         100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF}
  })

} // End of Scope \_SB.IETM

