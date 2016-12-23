/** @file
  ACPI DSDT table for USB Type C.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "UsbC.aml",
  "SSDT",
  2,
  "Intel_",
  "UsbCTabl",
  0x1000
  )
{

  External(UBCB)
  External(P8XH, MethodObj)
  External(BID, IntObj)  // BoardId
  External(\_SB.PCI0.XHC.RHUB, DeviceObj)
  External(\_SB.PCI0.XHC.RHUB.TPLD, MethodObj)
  External(\_SB.PCI0.XHC.RHUB.TUPC, MethodObj)
  External(ETYC, IntObj)

  Scope (\_SB)
  {
    Device(UBTC)  // USB type C device
    {
      Name (_HID, EISAID("USBC000"))
      Name (_CID, EISAID("PNP0CA0"))
      Name (_UID, 0)
      Name (_DDN, "USB Type C")
      Name (_ADR, 0x0)


      Name (CRS, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0x00000000, 0x1000, USBR)
      })

      Device (CR01) // USB Type C Connector
      {
        Name (_ADR, 0x0)
        Method (_PLD)  {  Return ( \_SB.PCI0.XHC.RHUB.TPLD(1,1) )  }
      }

      Method(_CRS,0, Serialized)
      {
        CreateDWordField (CRS, \_SB.UBTC.USBR._BAS, CBAS)
        Store (UBCB, CBAS)
        Return(CRS)
      }

      Method(_STA,0)
      {
        If (LEqual (ETYC, 1)) {
          Return(0xF)
        }
        Return(0x0)
      }

      OperationRegion (USBC, SystemMemory, UBCB, 0x38)  // 56 bytes Opm Buffer
      Field(USBC,ByteAcc,Lock,Preserve)
      {
        VER1, 8,
        VER2, 8,
        RSV1, 8,
        RSV2, 8,
        Offset(4),
        CCI0, 8,  //  PPM->OPM CCI indicator
        CCI1, 8,
        CCI2, 8,
        CCI3, 8,

        CTL0, 8,  //  OPM->PPM Control message
        CTL1, 8,
        CTL2, 8,
        CTL3, 8,
        CTL4, 8,
        CTL5, 8,
        CTL6, 8,
        CTL7, 8,

        // USB Type C Mailbox Interface
        MGI0, 8,  //  PPM->OPM Message In
        MGI1, 8,
        MGI2, 8,
        MGI3, 8,
        MGI4, 8,
        MGI5, 8,
        MGI6, 8,
        MGI7, 8,
        MGI8, 8,
        MGI9, 8,
        MGIA, 8,
        MGIB, 8,
        MGIC, 8,
        MGID, 8,
        MGIE, 8,
        MGIF, 8,

        MGO0, 8,  //  OPM->PPM Message Out
        MGO1, 8,
        MGO2, 8,
        MGO3, 8,
        MGO4, 8,
        MGO5, 8,
        MGO6, 8,
        MGO7, 8,
        MGO8, 8,
        MGO9, 8,
        MGOA, 8,
        MGOB, 8,
        MGOC, 8,
        MGOD, 8,
        MGOE, 8,
        MGOF, 8,

      }  // end of Field

      Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj} )
      {
        // Compare passed in UUID with supported UUID.
        If (LEqual(Arg0, ToUUID ("6f8398c2-7ca4-11e4-ad36-631042b5008f"))) {
          // UUID for USB type C
          Switch (ToInteger(Arg2))  // Arg2:  0 for query, 1 for write and 2 for read
          {
            Case (0)
            {
              Return (Buffer() {0x07}) // 2 functions defined other than Query.
            }

          }  // End switch
        }   // End UUID check
        Return (0)
      }  // End _DSM Method
    }  // end of Device
  } // end \_SB scope
} // end SSDT

