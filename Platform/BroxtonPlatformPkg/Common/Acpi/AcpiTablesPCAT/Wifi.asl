/** @file
  Copyright (c)  2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  OperationRegion(RPXX, PCI_Config, 0x00, 0x10)
  Field(RPXX, AnyAcc, NoLock, Preserve)
  {
    Offset(0),     // Vendor-Device ID
    VDID, 32,
  }

  // WIST (WiFi Device Presence Status)
  //
  // Check if a WiFi Device is present on the RootPort.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //    0 - if a device is not present.
  //    1 - if a device is present.
  //
  Method(WIST,0,Serialized)
  {
    // check Vendor-Device ID for supported devices
    If (CondRefOf(VDID)) {
      Switch (ToInteger(VDID)) {
        // Wifi devices
        Case(0x095A8086){Return(1)} // StonePeak
        Case(0x095B8086){Return(1)} // StonePeak
        Case(0x31658086){Return(1)} // StonePeak 1x1
        Case(0x31668086){Return(1)} // StonePeak 1x1
        Case(0x08B18086){Return(1)} // WilkinsPeak
        Case(0x08B28086){Return(1)} // WilkinsPeak
        Case(0x08B38086){Return(1)} // WilkinsPeak
        Case(0x08B48086){Return(1)} // WilkinsPeak
        Case(0x24F38086){Return(1)} // SnowfieldPeak
        Case(0x24F48086){Return(1)} // SnowfieldPeak
        Case(0x24F58086){Return(1)} // SnF/LnP/DgP SKUs
        Case(0x24F68086){Return(1)} // SnF/LnP/DgP SKUs
        Case(0x24FD8086){Return(1)} // Windstorm Peak
        Case(0x24FB8086){Return(1)} // Sandy Peak
        Default{Return(0)}          // no supported device
        }
    } Else {
      Return(0)
    }
  }

