/** @file
  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\GSBC, IntObj)

Scope(\_SB.PCI0.URT2)
{
  Device(GPS1)//GPS for Windows OS.
  {
    Name(_HID, "BCM4752")      //Vendor: test GPS device for BYT
    Name(_HRV, 0x0001)

    Method(_STA, 0x0, NotSerialized)
    {
      If (LEqual (GSBC, 1)) {
        Return (0xF)
      }
      Return (0)
    }

    Method(_CRS, 0x0, Serialized)
    {
      // UARTSerial Bus Connection Descriptor
      Name(BBUF, ResourceTemplate () {
        UARTSerialBus(
        115200,   // InitialBaudRate: in bits ber second
        ,             // BitsPerByte: default to 8 bits
        ,             // StopBits: Defaults to one bit
        0xfc,         // LinesInUse: 8 1-bit flags to declare line enabled
        ,             // IsBigEndian: default to LittleEndian
        ,             // Parity: Defaults to no parity
        FlowControlHardware,             // FlowControl: Defaults to no flow control
        32,             // ReceiveBufferSize
        32,             // TransmitBufferSize
        "\\_SB.PCI0.URT2",       // ResourceSource: UART bus controller name
        ,)            // DescriptorName: creates name for offset of resource descriptor
        //
        //Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, , , ) {62}                // GPS_HOSTREQ IRQ - 62
        GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO1", ) {36}       // GNSS_UART_WAKE_N
        })
       Return (BBUF)
    }

  } // Device GPS1
}

