/** @file
  ACPI DSDT table

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02,       // DSDT revision.
              // A Revision field value greater than or equal to 2 signifies that integers
              // Declared within the Definition Block are to be evaluated as 64-bit values
  "INTEL",    // OEM ID (6 byte string)
  "BXT-SOC",  // OEM table ID  (8 byte string)
  0x0         // OEM version of DSDT table (4 byte Integer)
  )

{
  External(\_SB.PCI0.GFX0, DeviceObj)
// Miscellaneous services enabled in Project
  include ("token.asl")
  Include ("AMLUPD.asl")
  include ("GloblNvs.asl")
  include ("PciTree.asl")
  include ("Sc.asl")
  include ("LpcB.asl")
  include ("Bxt.asl")
  include ("CPU.asl")
  include ("Platform.asl")
  include ("THERMAL.ASL")
  include ("PCI_DRC.ASL")
  include ("Video.asl")
  Include ("PcieRpPxsxWrapper.asl")
  include ("Pep/Pep.asl")
  include ("Gpe.asl") //Need this for xHCI D3 wake flow.

  if (LEqual(ECR1,1)) {
    Scope(\_SB.PCI0) {
      //
      // PCI-specific method's GUID
      //
      Name(PCIG, ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))
      Method(PCID, 4, Serialized) {
        If (LEqual(Arg0, PCIG)) {         // PCIE capabilities UUID
          If (LGreaterEqual(Arg1,3)) {                                              // revision at least 3
            If (LEqual(Arg2,0)) { Return (Buffer(2){0x01,0x03}) }                   // function 0: list of supported functions
            If (LEqual(Arg2,8)) { Return (1) }                                      // function 8: Avoiding Power-On Reset Delay Duplication on Sx Resume
            If (LEqual(Arg2,9)) { Return (Package(5){50000,Ones,Ones,50000,Ones}) } // function 9: Specifying Device Readiness Durations
          }
        }
        return (Buffer(1){0})
      }
    } //scope
  } //if

  Scope(\_SB.PCI0) {
    //PciCheck, Arg0=UUID, returns true if support for 'PCI delays optimization ECR' is enabled and the UUID is correct
    Method(PCIC,1,Serialized) {
      If (LEqual(ECR1,1)) {
        If (LEqual(Arg0, PCIG)) {
          return (1)
        }
      }
      return (0)
    }
  }

// Sleep states supported by Chipset/Board.
//----------------------------------------------------------------------
// SSx - BIOS setup controlled enabled _Sx Sleep state status
// Values to be written to SLP_TYPE register are provided by SBACPI.SDL (South Bridge ACPI ModulePart)

  Name(\_S0, Package(4) {0x0,0x0,0,0}) // mandatory System state
  if (SS1) {Name(\_S1, Package(4) {0x1,0x0,0,0})}
  if (SS3) {Name(\_S3, Package(4) {0x5,0x0,0,0})}
  if (SS4) {Name(\_S4, Package(4) {0x6,0x0,0,0})}
  Name(\_S5, Package(4) {0x7,0x0,0,0}) // mandatory System state

  Method(PTS, 1) {        // METHOD CALLED FROM _PTS PRIOR TO ENTER ANY SLEEP STATE
    If (Arg0) {
    // entering any sleep state
    }
  }
  Method(WAK, 1) {        // METHOD CALLED FROM _WAK RIGHT AFTER WAKE UP
  }
}// End of ASL File
