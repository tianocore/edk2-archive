/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0.I2C0)
{
  Device(PMC0)
  {
    Name(_ADR, Zero)
    Name(_HID, "INT3472")
    Name(_CID, "INT3472")
    Name(_DDN, "INCL-CRDD")
    Name(_UID, "1")

    Method(_CRS, 0x0, Serialized)
    {
      Name(SBUF, ResourceTemplate()
      {
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
            "\\_SB.GPO0", 0x00, ResourceConsumer, ,)
        {   // Pin list
            0x35     // GPIO_65
        }
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
            "\\_SB.GPO0", 0x00, ResourceConsumer, ,)
        {   // Pin list
            0x3B     // GPIO_71
        }
      })
        Return (SBUF)
    }

    Method(_STA, 0, NotSerialized)    // _STA: Status
    {
      If (LEqual (UCAS, 1)) { // 1-OV2740
        Return (0x0F)
      } else {
        Return (0x0)
      }
    }
    Method(CLDB, 0, Serialized)
    {
      Name(PAR, Buffer(0x20)
      {
        0x00,     //Version
        0x01,     //Control logic Type 0:  UNKNOWN  1: DISCRETE 2: PMIC TPS68470  3: PMIC uP6641
        0x01,     //Control logic ID: Control Logic 1
        0x70,     //CRD board Type, 0: UNKNOWN  0x20: CRD-D  0x30: CRD-G  0x40: PPV 0x50:  CRD-G2  0x70: CRD_G_BXT
        0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
      })
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized)  // _DSM: Device-Specific Method
    {
      If (LEqual (Arg0, ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) {
        If (LEqual (Arg2, One)) {
          Return (0x02)           // number
        }
        If (LEqual (Arg2, 0x02)) {
          Return (0x01004100)     // RESET
        }
        If (LEqual (Arg2, 0x03)) {
          Return (0x01004701)    //  POWER DOWN
        }
      }
      Return (Zero)
    }
  }

  Device(CAM0)
  {
    Name(_ADR, Zero)
    Name(_HID, "INT3474")
    Name(_CID, "INT3474")
    Name(_SUB, "INTL0000")
    Name(_DDN, "OV2740")
    Name(_UID, One)

    Name (_DEP, Package ()  // _DEP: Dependencies
    {
      \_SB.PCI0.I2C0.PMC0
    })

    Name(PLDB, Package(1)
    {
      Buffer(0x14)
      {
        0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x61, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF
      }
    })

    Method(_PLD, 0, Serialized)
    {
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(CROT, RPOS)

      Return(PLDB)
    }

    Method(_STA, 0, NotSerialized)
    {
      If (LEqual (UCAS, 1)) { // 1-OV2740
        Return (0x0F)
      } else {
        Return (0x0)
      }
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer(0x6C)
      {
        0x00,                              //Version
        0x70,                              //SKU: CRD_G_BXT
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //GUID for CSI2 host controller
        0x00,                              //DevFunction
        0x00,                              //Bus
        0x00, 0x00, 0x00, 0x00,            //DphyLinkEnFuses
        0x00, 0x00, 0x00, 0x00,            //ClockDiv
        0x02,                              //LinkUsed
        0x02,                              //LaneUsed
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_TERMEN_CLANE
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_SETTLE_CLANE
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_TERMEN_DLANE0
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_SETTLE_DLANE0
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_TERMEN_DLANE1
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_SETTLE_DLANE1
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_TERMEN_DLANE2
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_SETTLE_DLANE2
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_TERMEN_DLANE3
        0x00, 0x00, 0x00, 0x00,            //CSI_RX_DLY_CNT_SETTLE_DLANE3
        0x00, 0x00, 0x00, 0x00,            //MaxLaneSpeed
        0x00,                              //SensorCalibrationFileIdx
        0x00, 0x00, 0x00,                  //SensorCalibrationFileIdxInMBZ
        0x00,                              //RomType: NONE
        0x00,                              //VcmType: none
        0x08,                              //Platform info  BXT
        0x00,                              //Platform sub info
        0x02,                              //Flash none
        0x00,                              //Privacy LED  not supported
        0x00,                              //0 degree
        0x01,                              //MIPI link/lane defined in ACPI
        0x00, 0xF8, 0x24, 0x01,            // MCLK: 19200000Hz
        0x01,                              //Control logic ID
        0x00, 0x00, 0x00,
        0x01,                              // M_CLK
        0x00, 0x00, 0x00,                  //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,      //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,      //Reserved
      })
      Return (PAR)
    }

    Method(_CRS, 0, Serialized)
    {
      Name(SBUF, ResourceTemplate()
      {
        I2CSerialBus(0x0036, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
        "\\_SB.PCI0.I2C0", 0x00, ResourceConsumer,,)
      })
      Return(SBUF)
    }

    Method(_DSM, 4, NotSerialized)
    {
      If (LEqual(Arg0, ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) {
        Return("4SF259T2")
      }
      If (LEqual(Arg0, ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) {
        If (LEqual(Arg2, One)) {
          Return(One)
        }
        If (LEqual(Arg2, 0x02)) {
          // I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function
          Return(0x00003600)  //  SENSOR
        }
      }
      Return(Zero)
    }
  }
}

