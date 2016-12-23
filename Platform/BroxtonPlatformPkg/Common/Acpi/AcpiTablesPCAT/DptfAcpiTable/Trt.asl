/** @file
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
  // _TRT (Thermal Relationship Table)
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   A variable-length Package containing a list of Thermal Relationship Packages as described below.
  //
  // Return Value Information
  //   Package {
  //   ThermalRelationship[0] // Package
  //    ...
  //   ThermalRelationship[n] // Package
  //   }
  //
  Name(_TRT, Package()
  {
    // Source                       Target             Influence  Period    Reserved
    Package(){\_SB.PCI0.TCPU,       \_SB.SEN1,                10,    100,   0, 0, 0, 0}
  })

  // TRTR (Thermal Relationship Table Revision)
  //
  // This object evaluates to an integer value that defines the revision of the _TRT object.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  // 0: Traditional TRT as defined by the ACPI Specification.
  // 1: Priority based TRT
  //
  Method(TRTR)
  {
    Return(TRTV)
  }

} // End Scope(\_SB.IETM)

