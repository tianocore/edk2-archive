/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0) {
  //
  // Integrated Sensor Hub - Device 17, Function 0
  //
  Device(ISH0) {
    Name(_ADR, 0x00110000)
    Method (_STA, 0, NotSerialized) {
      Return (0x0F)
    }

    //--------------------
    //  Intel Proprietary Wake up Event solution
    //--------------------
    Method(_DSM, 0x4, Serialized)
    {
      If (LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37"))) {
        // Function 0 : Query Function
        If (LEqual(Arg2, Zero)) {
          // Revision 0
          If (LEqual(Arg1, Zero)) {
            Return(Buffer(One) { 0x03 }) // There are 1 function defined other than Query.
          } Else {
            Return(0) // Revision mismatch
          }
        }
        // Function 1 :
        If (LEqual(Arg2, One)) {
          Store(DerefOf(Index(Arg3, Zero)), Local0)
          If (LEqual(Local0, Zero)) {

          } Else {
            //Handle sleep, dock, un-dock events here
          }
          Return(0)
        } Else {
          Return(0) // Function number mismatch but normal return.
        }
      } Else {
        Return(Buffer(One) { 0x00 }) // Guid mismatch
      }
    }

    Method (_PS0, 0, NotSerialized) { // _PS0: Power State 0
    }

    Method (_PS3, 0, NotSerialized) { // _PS3: Power State 3
    }
  }

}//end scope

