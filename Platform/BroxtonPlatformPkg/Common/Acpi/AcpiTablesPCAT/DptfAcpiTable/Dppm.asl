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
  //
  // DPPM Passive Policy 2.0
  //
  Name (DP2P, Package ()
  {
    ToUUID("9E04115A-AE87-4D1C-9500-0F3E340BFE75")
  })

  //
  // DPPM Passive Policy 1.0
  //
  Name (DPSP, Package ()
  {
    ToUUID("42A441D6-AE6A-462B-A84B-4A8CE79027D3")
  })

  // DPPM Active Policy
  //
  Name (DASP, Package ()
  {
    ToUUID("3A95C389-E4B8-4629-A526-C52C88626BAE")
  })

  //
  // DPPM Crtical Policy
  //
  Name (DCSP, Package()
  {
    ToUUID("97C68AE7-15FA-499c-B8C9-5DA81D606E0A")
  })

  //
  // Power Boss Policy
  //
  Name (POBP, Package ()
  {
    ToUUID("F5A35014-C209-46A4-993A-EB56DE7530A1")
  })

  //
  // Virtual Sensor Policy
  //
  Name (DVSP, Package ()
  {
    ToUUID("6ED722A7-9240-48a5-B479-31EEF723D7CF")
  })
}

