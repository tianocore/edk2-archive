/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0.I2C2)
{
  Device (PMC2) {
    Name (_ADR, Zero)
    Name (_HID, "INT3472")
    Name (_CID, "INT3472")
    Name (_DDN, "INCL-CRDD")
    Name (_UID, "0")

    Method (_CRS, 0x0, Serialized) {
      Name (SBUF, ResourceTemplate() {
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
                "\\_SB.GPO0", 0x00, ResourceConsumer, ,) {
          0x37     // GPIO_67
        }

        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
                "\\_SB.GPO0", 0x00, ResourceConsumer, ,) {
          0x3C     // GPIO_72
        }
      })
      Return (SBUF)
    }

    Method (_STA, 0, NotSerialized) {
      If (LEqual (WCAS, 1)) {  // 1-IMX214
        Return (0x0F)
      }
      Return (0x0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer(0x20) {
        0x00,     //Version
        0x01,     //Control logic Type 0:  UNKNOWN  1: DISCRETE 2: PMIC TPS68470  3: PMIC uP6641
        0x00,     //Control logic ID: Control Logic 0
        0x70,     //CRD board Type, 0: UNKNOWN  0x20: CRD-D  0x30: CRD-G  0x40: PPV 0x50:  CRD-G2  0x70: CRD_G_BXT
        0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
      })
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) {
      If (LEqual (Arg0, ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) {
        If (LEqual (Arg2, One)) {
          Return (0x02)           // number
        }
        If (LEqual (Arg2, 0x02)) {
          Return (0x01004300)     // RESET
        }
        If (LEqual (Arg2, 0x03)) {
          Return (0x01004801)    //  POWER DOWN
        }
      }
      Return (Zero)
    }
  }

  //
  // Rear Camera IMX214A
  //
  Device (CAM2) {
    Name (_ADR, Zero)
    Name (_HID, "SONY214A")
    Name (_CID, "SONY214A")
    Name (_SUB, "INTL0000")
    Name (_DDN, "SONY IMX214")
    Name (_UID, One)

    Name (_DEP, Package () {
      \_SB.PCI0.I2C2.PMC2
    })

    Name (PLDB, Package(1) {
      Buffer(0x14)
      {
        0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x69, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF
      }
    })
    Method (_PLD, 0, Serialized) {
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(CROT, RPOS)

      Return(PLDB)
    }

    Method (_STA, 0, NotSerialized) {
      If (LEqual (WCAS, 1)) {  // 1-IMX214
        Return (0x0F)
      }
      Return (0x0)
    }

    Method (SSDB, 0, Serialized) {
      Name (PAR, Buffer(0x6C) {
        0x00,                              //Version
        0x70,                              //SKU: CRD_G_BXT
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //GUID for CSI2 host controller
        0x00,                              //DevFunction
        0x00,                              //Bus
        0x00, 0x00, 0x00, 0x00,            //DphyLinkEnFuses
        0x00, 0x00, 0x00, 0x00,            //ClockDiv
        0x06,                              //LinkUsed
        0x04,                              //LaneUsed
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
        0x02,                              //VcmType: DW9714A
        0x08,                              //Platform info  BXT
        0x00,                              //Platform sub info
        0x03,                              //Flash ENABLED
        0x00,                              //Privacy LED  not supported
        0x00,                              //0 degree
        0x01,                              //MIPI link/lane defined in ACPI
        0x00, 0xF8, 0x24, 0x01,            // MCLK: 19200000Hz
        0x00,                              //Control logic ID
        0x00, 0x00, 0x00,
        0x02,                              // M_CLK Port
        0x00, 0x00, 0x00,                  //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,      //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,      //Reserved
      })
      Return (PAR)
    }

    Method (_CRS, 0, Serialized) {
      Name (SBUF, ResourceTemplate() {
        I2CSerialBus(0x001A, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PCI0.I2C2", 0x00, ResourceConsumer,,)
        I2CSerialBus(0x000C, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PCI0.I2C2", 0x00, ResourceConsumer,,)
      })
      Return(SBUF)
    }

    Method (_DSM, 4, NotSerialized)
    {
      If (LEqual (Arg0, ToUUID ("822ACE8F-2814-4174-A56B-5F029FE079EE"))) {
        Return("P13N05BA")
      }

      If (LEqual (Arg0, ToUUID ("26257549-9271-4CA4-BB43-C4899D5A4881"))) {
        If (LEqual (Arg2, One)) {
          Return (0x02)
        }
        If (LEqual (Arg2, 0x02)) {
          // I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function
          Return (0x02001A00)  //  SENSOR
        }
        If (LEqual (Arg2, 0x03)) {
          // I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function
          Return (0x02000C01)   // VCM
        }
      }
      Return(Zero)
    }
  }
} //  Scope(\_SB.PCI0.I2C2)

