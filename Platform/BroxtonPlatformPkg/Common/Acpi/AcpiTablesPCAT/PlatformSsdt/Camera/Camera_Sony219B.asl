/** @file
  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


Scope(\_SB.PCI0.I2C6)
{
    Device (CAM4)
    {
        //
        // Front Camera IMX219
        //
        Name (_ADR, Zero)  // _ADR: Address
        Name (_HID, "SONY219A")  // _HID: Hardware ID
        Name (_CID, "SONY219A")  // _CID: Compatible ID
        Name (_SUB, "INTL0000")  // _SUB: Subsystem ID
        Name (_DDN, "SONY IMX219")  // _DDN: DOS Device Name
        Name (_UID, One)  // _UID: Unique ID
        /*  need to be update after power on
        Name (_DEP, Package ()  // _DEP: Dependencies
        {
           //PMIC device
        })

        Name (_PR0, Package (0x03)  // _PR0: Power Resources for D0
        {
            // Power and Clock
        })
        */
        Name (PLDB, Package (0x01)
        {
            Buffer (0x14)
            {
                /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                /* 0008 */   0x61, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
                /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
            }
        })
        Method (_PLD, 0, Serialized)  // _PLD: Physical Location of Device
        {
            Return (PLDB)
        }

        Method (_STA, 0, NotSerialized)  // _STA: Status
        {
            If(LEqual(UCAS,2)) {  // 2-IMX219
              Return (0x0F)
            }
            Return (0x0)
        }
        /*
        Method (_PS3, 0, Serialized)  // _PS3: Power State 3
        {
        }

        Method (_PS0, 0, Serialized)  // _PS0: Power State 0
        {
        }
        */
        Name (SBUF, ResourceTemplate ()
        {
            GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
                "\\_SB.GPO0", 0x00, ResourceConsumer, ,
                )
                {
                    0x13  // GPIO_19
                }
             //I2C for CMOS
            I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
                AddressingMode7Bit, "\\_SB.PCI0.I2C6",
                0x00, ResourceConsumer, ,
                )
        })
        Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
        {
            Return (SBUF)
        }

        Method (_DSM, 4, NotSerialized)  // _DSM: Device-Specific Method
        {
            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x6A, 0xA7, 0x7B, 0x37, 0x90, 0xF3, 0xFF, 0x4A,
                        /* 0008 */   0xAB, 0x38, 0x9B, 0x1B, 0xF3, 0x3A, 0x30, 0x15
                    }))
            {
                Return ("SONY219A")     //DSDT_CAM_HWID
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0xAA, 0xAA, 0x62, 0x3C, 0xE0, 0xD8, 0x1A, 0x40,
                        /* 0008 */   0x84, 0xC3, 0xFC, 0x05, 0x65, 0x6F, 0xA2, 0x8C
                    }))
            {
                Return ("IMX219")       //DSDT_CAM_CMOS
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x8F, 0xCE, 0x2A, 0x82, 0x14, 0x28, 0x74, 0x41,
                        /* 0008 */   0xA5, 0x6B, 0x5F, 0x02, 0x9F, 0xE0, 0x79, 0xEE
                    }))
            {
                Return ("RasperryPi")    //DSDT_CAM_MODULE
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x2A, 0x51, 0x59, 0x29, 0x8C, 0x02, 0x46, 0x46,
                        /* 0008 */   0xB7, 0x3D, 0x4D, 0x1B, 0x56, 0x72, 0xFA, 0xD8
                    }))
            {
                Return ("Intel_RVP")    //DSDT_CAM_CUSTOM
            }


            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0xD8, 0x7B, 0x3B, 0xEA, 0x9B, 0xE0, 0x39, 0x42,
                        /* 0008 */   0xAD, 0x6E, 0xED, 0x52, 0x5F, 0x3F, 0x26, 0xAB
                    }))
            {
                Return (0x1022)         //DSDT_CAM_MIPIPORT
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x92, 0xC4, 0x5A, 0xB6, 0x30, 0x9E, 0x60, 0x4D,
                        /* 0008 */   0xB5, 0xB2, 0xF4, 0x97, 0xC7, 0x90, 0xD9, 0xCF
                    }))
            {
                Return (Zero)           //DSDT_CAM_FUNC
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x0F, 0xAB, 0x70, 0xE7, 0x44, 0x26, 0xAB, 0x4B,
                        /* 0008 */   0x86, 0x28, 0xD6, 0x2F, 0x16, 0x83, 0xFB, 0x9D
                    }))
            {
                Return (Zero)           //DSDT_CAM_ROM
            }
            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0xB2, 0x4A, 0xA5, 0x1E, 0x84, 0xCD, 0xCC, 0x48,
                        /* 0008 */   0x9D, 0xD4, 0x7F, 0x59, 0x4E, 0xC3, 0xB0, 0x15
                    }))
            {
                Return (Zero)           //DSDT_CAM_INTF_VER
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x39, 0xA6, 0xC9, 0x75, 0x8A, 0x5C, 0x00, 0x4A,
                        /* 0008 */   0x9F, 0x48, 0xA9, 0xC3, 0xB5, 0xDA, 0x78, 0x9F
                    }))
            {
                Return (Zero)           //DSDT_CAM_VCM
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x51, 0x26, 0xBE, 0x8D, 0xC1, 0x70, 0x6F, 0x4C,
                        /* 0008 */   0xAC, 0x87, 0xA3, 0x7C, 0xB4, 0x6E, 0x4A, 0xF6
                        }))
            {
                  Return (0x0)           //MCLK0 --> OSC_CLKOUT_R_0
            }
            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x49, 0x75, 0x25, 0x26, 0x71, 0x92, 0xA4, 0x4C,
                        /* 0008 */   0xBB, 0x43, 0xC4, 0x89, 0x9D, 0x5A, 0x48, 0x81
                    }))

            {
                If (LEqual (Arg2, One))     //DSDT_CAM_I2C
                {
                    Return (One)
                }

                If (LEqual (Arg2, 0x02))
                {
                    Return (0x03003600)
                }
            }

            If (LEqual (Arg0, Buffer (0x10)
                    {
                        /* 0000 */   0x40, 0x46, 0x23, 0x79, 0x10, 0x9E, 0xEA, 0x4F,
                        /* 0008 */   0xA5, 0xC1, 0xB5, 0xAA, 0x8B, 0x19, 0x75, 0x6F
                    }))
            {
                If (LEqual (Arg2, One))     //DSDT_CAM_GPIO
                {
                    Return (0x01)
                }

                If (LEqual (Arg2, 0x02))
                {
                    Return (0x01004300)
                }
            }

            Return (Zero)
        }
    }

} //  Scope(\_SB.PCI0.I2C4)
