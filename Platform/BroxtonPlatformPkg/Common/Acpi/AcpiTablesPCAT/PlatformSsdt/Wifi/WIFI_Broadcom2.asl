/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0.SDIO)
{
  Device (BRC2)
  {
    Name (_ADR, 0x02)

    Method (_STA, 0, NotSerialized)
    {
      Return (0xF)
    }

    Method (_RMV, 0, NotSerialized)
    {
      Return (Zero)
    }

    Method (_CRS, 0, Serialized)
    {
      Name (NAM, Buffer ()
      { // 20
        "\\_SB.PCI0.SDIO.BRCM"
      })
      Name (SPB, Buffer ()
      {
        /* 0000 */   0x8E, 0x1D, 0x00, 0x01, 0x00, 0xC0, 0x02, 0x00,
        /* 0008 */   0x00, 0x01, 0x00, 0x00
      })
      Name(END, Buffer() {0x79, 0x00})

      Concatenate (SPB, NAM, Local0)
      Concatenate (Local0, END, Local1)
      Return (Local1)
    }
  }
}

