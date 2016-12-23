/** @file
  Power resource and wake capability for USB ports

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  //
  // Define _PR0, _PR3 PowerResource Package
  //
  Name (_PR0, Package(){UPPR})
  Name (_PR3, Package(){UPPR})

  PowerResource(UPPR, 0, 0)
  {
    Method(_STA)
    {
      Return(0x01)
    }

    Method(_ON)
    {
    }

    Method(_OFF)
    {
    }
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

