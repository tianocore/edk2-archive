/** @file
  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "PlatformSsdt.aml",
  "SSDT",
  1,
  "Intel_",
  "PlatformTable",
  0x1000
  )
{
  External(\_SB.PCI0.I2C0, DeviceObj)
  External(\_SB.PCI0.I2C1, DeviceObj)
  External(\_SB.PCI0.I2C2, DeviceObj)
  External(\_SB.PCI0.I2C3, DeviceObj)
  External(\_SB.PCI0.I2C4, DeviceObj)
  External(\_SB.PCI0.I2C5, DeviceObj)
  External(\_SB.PCI0.I2C6, DeviceObj)
  External(\_SB.PCI0.I2C7, DeviceObj)
  External(\_SB.PCI0.URT1, DeviceObj)
  External(\_SB.PCI0.URT2, DeviceObj)
  External(\_SB.PCI0.SDIO, DeviceObj)
  External(\_SB.PCI0.SPI1, DeviceObj)
  External(\_SB.PCI0.SPI3, DeviceObj)
  External(\_SB.GPO0.CWLE, IntObj)
  External(\_SB.GPO0.AVBL, IntObj)
  External(\_SB.PCI0.SDIO.PSTS, IntObj)
  External(\SUCE, IntObj)
  External(HIDG, MethodObj)
  External(OSYS, IntObj)
  External(SBTD, IntObj)
  External(WCAS, IntObj)
  External(UCAS, IntObj)
  External(CROT, IntObj)

  External(TP7G)
  External(IPUD)

  include ("Audio/AudioCodec_INT34C1.asl")
  include ("Audio/AudioCodec_INT343A.asl")
  include ("Audio/AudioCodec10TI3100.asl")

  If (LEqual(IPUD, 1)) {
    include ("Camera/Camera_INT3471.asl")
    include ("Camera/Camera_INT3474.asl")
    include ("Camera/Camera_Sony214A.asl")
    include ("Camera/Camera_Sony219A.asl")
    include ("Camera/Camera_Sony219B.asl")
    include ("Camera/Flash_TPS61311.asl")
  }

  include ("Touch/TouchPanel_I2C3.asl")
  include ("Touch/TouchPads_I2C4.asl")

  include ("PSS/MonzaX2K_IMPJ0003.asl")

  include ("Bluetooth/LBEE5KL1DX.asl")
  include ("Wifi/LBEE5KL1DX.asl")

  include ("Gps/Gps.asl")

  include ("Nfc/Nfc.asl")

  include ("Fingerprint/Fingerprint_FPC.asl")
  include ("SueCreek/SueCreek.asl")
  include ("SueCreek/SueCreekLeds.asl")
  include ("Sensors/GenericSpi3.asl")
}

