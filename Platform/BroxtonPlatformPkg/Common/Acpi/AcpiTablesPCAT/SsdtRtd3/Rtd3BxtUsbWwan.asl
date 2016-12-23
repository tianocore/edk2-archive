/** @file
  Power resource and wake capability for USB ports hosting WWAN module

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  // USB Port 3 power resource

  PowerResource(PX03, 0, 0) {
    Name(WOFF, 0)  // Last OFF Time stamp (WOFF): The time stamp of the last power resource _OFF method evaluation

    Method(_STA)
    {
      If (LEqual(\_SB.GGOV(NW_GPIO_78), 1)) {
        Return(0x01)
      } Else {
        Return(0x00)
      }
    }

    Method(_ON, 0)                                        /// _ON Method
    {
      If (LNotEqual(^WOFF, Zero)) {
        Divide(Subtract(Timer(), ^WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
        If (LLess(Local0,100)) {                           // If Elapsed time is less than 100ms
          Sleep(Subtract(100,Local0))                     // Sleep for the remaining time
        }
      }
      \_SB.SGOV(NW_GPIO_78,1)                             // set power pin to high
      \_SB.SGOV(NW_GPIO_117,1)                            // set reset pin to high
    }

    Method(_OFF, 0)                                       /// _OFF Method
    {
      \_SB.SGOV(NW_GPIO_117,0)                            // set reset pin to low
      \_SB.SGOV(NW_GPIO_78,0)                             // set power pin to low

      Store(Timer(), ^WOFF)                               // Start OFF timer here.
    }
  } // End PX03

  Name(_PR0,Package(){PX03})                 // Power Resource required to support D0
  Name(_PR2,Package(){PX03})                 // Power Resource required to support D2
  Name(_PR3,Package(){PX03})                 // Power Resource required to support D3

  //
  // WWAN Modem device with the same power resource as its composite parent device
  //
  Device (MODM) {
    Name(_ADR, 0x3)
    Name(_PR0,Package(){PX03})               // Power Resource required to support D0
    Name(_PR2,Package(){PX03})               // Power Resource required to support D2
    Name(_PR3,Package(){PX03})               // Power Resource required to support D3
  }

  //
  // _SxW, in Sx, the lowest power state supported to wake up the system
  // _SxD, in Sx, the highest power state supported by the device
  // If OSPM supports _PR3 (_OSC, Arg3[2]), 3 represents D3hot; 4 represents D3cold, otherwise 3 represents D3.
  //
  Method(_S0W, 0, Serialized)
  {
    Return(0x3) // return 3 (D3hot)
  }
  Method (_S3D, 0, Serialized)
  {
    Return (0x2)
  }
  Method (_S3W, 0, Serialized)
  {
    Return (0x3)
  }
  Method (_S4D, 0, Serialized)
  {
    Return (0x2)
  }
  Method (_S4W, 0, Serialized)
  {
    Return (0x3)
  }

