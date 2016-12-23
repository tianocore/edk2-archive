/** @file
  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// Define SC NVS Area operatino region.
//

#ifndef _SC_NVS_AREA_H_
#define _SC_NVS_AREA_H_

#pragma pack (push,1)
typedef struct {
  UINT32   RcRevision;                              ///< Offset 0       RC Revision
  UINT16   PchSeries;                               ///< Offset 4       PCH Series
  UINT16   PchGeneration;                           ///< Offset 6       PCH Generation
  UINT32   RpAddress[20];                           ///< Offset 8       Root Port address 1
                                                    ///< Offset 12      Root Port address 2
                                                    ///< Offset 16      Root Port address 3
                                                    ///< Offset 20      Root Port address 4
                                                    ///< Offset 24      Root Port address 5
                                                    ///< Offset 28      Root Port address 6
                                                    ///< Offset 32      Root Port address 7
                                                    ///< Offset 36      Root Port address 8
                                                    ///< Offset 40      Root Port address 9
                                                    ///< Offset 44      Root Port address 10
                                                    ///< Offset 48      Root Port address 11
                                                    ///< Offset 52      Root Port address 12
                                                    ///< Offset 56      Root Port address 13
                                                    ///< Offset 60      Root Port address 14
                                                    ///< Offset 64      Root Port address 15
                                                    ///< Offset 68      Root Port address 16
                                                    ///< Offset 72      Root Port address 17
                                                    ///< Offset 76      Root Port address 18
                                                    ///< Offset 80      Root Port address 19
                                                    ///< Offset 84      Root Port address 20
  UINT32   NHLA;                                    ///< Offset 88      HD-Audio NHLT ACPI address
  UINT32   NHLL;                                    ///< Offset 92      HD-Audio NHLT ACPI length
  UINT32   ADFM;                                    ///< Offset 96      HD-Audio DSP Feature Mask
  UINT32   SBRG;                                    ///< Offset 100     SBREG_BAR
  UINT32   GPEM;                                    ///< Offset 104     GPP_X to GPE_DWX mapping
  UINT16   PcieLtrMaxSnoopLatency[20];              ///< Offset 108     PCIE LTR max snoop Latency 1
                                                    ///< Offset 110     PCIE LTR max snoop Latency 2
                                                    ///< Offset 112     PCIE LTR max snoop Latency 3
                                                    ///< Offset 114     PCIE LTR max snoop Latency 4
                                                    ///< Offset 116     PCIE LTR max snoop Latency 5
                                                    ///< Offset 118     PCIE LTR max snoop Latency 6
                                                    ///< Offset 120     PCIE LTR max snoop Latency 7
                                                    ///< Offset 122     PCIE LTR max snoop Latency 8
                                                    ///< Offset 124     PCIE LTR max snoop Latency 9
                                                    ///< Offset 126     PCIE LTR max snoop Latency 10
                                                    ///< Offset 128     PCIE LTR max snoop Latency 11
                                                    ///< Offset 130     PCIE LTR max snoop Latency 12
                                                    ///< Offset 132     PCIE LTR max snoop Latency 13
                                                    ///< Offset 134     PCIE LTR max snoop Latency 14
                                                    ///< Offset 136     PCIE LTR max snoop Latency 15
                                                    ///< Offset 138     PCIE LTR max snoop Latency 16
                                                    ///< Offset 140     PCIE LTR max snoop Latency 17
                                                    ///< Offset 142     PCIE LTR max snoop Latency 18
                                                    ///< Offset 144     PCIE LTR max snoop Latency 19
                                                    ///< Offset 146     PCIE LTR max snoop Latency 20
  UINT16   PcieLtrMaxNoSnoopLatency[20];            ///< Offset 148     PCIE LTR max no snoop Latency 1
                                                    ///< Offset 150     PCIE LTR max no snoop Latency 2
                                                    ///< Offset 152     PCIE LTR max no snoop Latency 3
                                                    ///< Offset 154     PCIE LTR max no snoop Latency 4
                                                    ///< Offset 156     PCIE LTR max no snoop Latency 5
                                                    ///< Offset 158     PCIE LTR max no snoop Latency 6
                                                    ///< Offset 160     PCIE LTR max no snoop Latency 7
                                                    ///< Offset 162     PCIE LTR max no snoop Latency 8
                                                    ///< Offset 164     PCIE LTR max no snoop Latency 9
                                                    ///< Offset 166     PCIE LTR max no snoop Latency 10
                                                    ///< Offset 168     PCIE LTR max no snoop Latency 11
                                                    ///< Offset 170     PCIE LTR max no snoop Latency 12
                                                    ///< Offset 172     PCIE LTR max no snoop Latency 13
                                                    ///< Offset 174     PCIE LTR max no snoop Latency 14
                                                    ///< Offset 176     PCIE LTR max no snoop Latency 15
                                                    ///< Offset 178     PCIE LTR max no snoop Latency 16
                                                    ///< Offset 180     PCIE LTR max no snoop Latency 17
                                                    ///< Offset 182     PCIE LTR max no snoop Latency 18
                                                    ///< Offset 184     PCIE LTR max no snoop Latency 19
                                                    ///< Offset 186     PCIE LTR max no snoop Latency 20
  UINT32   SerialIoDebugUart0Bar0;                  ///< Offset 188     SerialIo Hidden UART0 BAR 0
  UINT32   SerialIoDebugUart1Bar0;                  ///< Offset 192     SerialIo Hidden UART1 BAR 0
  UINT32   ADPM;                                    ///< Offset 196     HD-Audio DSP Post-Processing Module Mask
  UINT8    XHPC;                                    ///< Offset 200     Number of HighSpeed ports implemented in XHCI controller
  UINT8    XRPC;                                    ///< Offset 201     Number of USBR ports implemented in XHCI controller
  UINT8    XSPC;                                    ///< Offset 202     Number of SuperSpeed ports implemented in XHCI controller
  UINT8    XSPA;                                    ///< Offset 203     Address of 1st SuperSpeed port
  UINT32   HPTB;                                    ///< Offset 204     HPET base address
  UINT8    HPTE;                                    ///< Offset 208     HPET enable
  //
  // 110-bytes large SerialIo block
  //
  UINT8    SMD[11];                                 ///< Offset 209     SerialIo controller 0 (sdma) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 210     SerialIo controller 1 (i2c0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 211     SerialIo controller 2 (i2c1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 212     SerialIo controller 3 (spi0) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 213     SerialIo controller 4 (spi1) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 214     SerialIo controller 5 (ua00) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 215     SerialIo controller 6 (ua01) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 216     SerialIo controller 7 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 217     SerialIo controller 8 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 218     SerialIo controller 9 (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
                                                    ///< Offset 219     SerialIo controller A (shdc) mode (0: disabled, 1: pci, 2: acpi, 3: debug port)
  UINT8    SIR[11];                                 ///< Offset 220     SerialIo controller 0 (sdma) irq number
                                                    ///< Offset 221     SerialIo controller 1 (i2c0) irq number
                                                    ///< Offset 222     SerialIo controller 2 (i2c1) irq number
                                                    ///< Offset 223     SerialIo controller 3 (spi0) irq number
                                                    ///< Offset 224     SerialIo controller 4 (spi1) irq number
                                                    ///< Offset 225     SerialIo controller 5 (ua00) irq number
                                                    ///< Offset 226     SerialIo controller 6 (ua01) irq number
                                                    ///< Offset 227     SerialIo controller 7 (shdc) irq number
                                                    ///< Offset 228     SerialIo controller 8 (shdc) irq number
                                                    ///< Offset 229     SerialIo controller 9 (shdc) irq number
                                                    ///< Offset 230     SerialIo controller A (shdc) irq number
  UINT32   SB0[11];                                 ///< Offset 231     SerialIo controller 0 (sdma) BAR0
                                                    ///< Offset 235     SerialIo controller 1 (i2c0) BAR0
                                                    ///< Offset 239     SerialIo controller 2 (i2c1) BAR0
                                                    ///< Offset 243     SerialIo controller 3 (spi0) BAR0
                                                    ///< Offset 247     SerialIo controller 4 (spi1) BAR0
                                                    ///< Offset 251     SerialIo controller 5 (ua00) BAR0
                                                    ///< Offset 255     SerialIo controller 6 (ua01) BAR0
                                                    ///< Offset 259     SerialIo controller 7 (shdc) BAR0
                                                    ///< Offset 263     SerialIo controller 8 (shdc) BAR0
                                                    ///< Offset 267     SerialIo controller 9 (shdc) BAR0
                                                    ///< Offset 271     SerialIo controller A (shdc) BAR0
  UINT32   SB1[11];                                 ///< Offset 275     SerialIo controller 0 (sdma) BAR1
                                                    ///< Offset 279     SerialIo controller 1 (i2c0) BAR1
                                                    ///< Offset 283     SerialIo controller 2 (i2c1) BAR1
                                                    ///< Offset 287     SerialIo controller 3 (spi0) BAR1
                                                    ///< Offset 291     SerialIo controller 4 (spi1) BAR1
                                                    ///< Offset 295     SerialIo controller 5 (ua00) BAR1
                                                    ///< Offset 299     SerialIo controller 6 (ua01) BAR1
                                                    ///< Offset 303     SerialIo controller 7 (shdc) BAR1
                                                    ///< Offset 307     SerialIo controller 8 (shdc) BAR1
                                                    ///< Offset 311     SerialIo controller 9 (shdc) BAR1
                                                    ///< Offset 315     SerialIo controller A (shdc) BAR1
  //
  // end of SerialIo block
  //
  UINT8    GPEN;                                    ///< Offset 319     GPIO enabled
  UINT8    SGIR;                                    ///< Offset 320     GPIO IRQ
  UINT8    RstPcieStorageInterfaceType[3];          ///< Offset 321     RST PCIe Storage Cycle Router#1 Interface Type
                                                    ///< Offset 322     RST PCIe Storage Cycle Router#2 Interface Type
                                                    ///< Offset 323     RST PCIe Storage Cycle Router#3 Interface Type
  UINT8    RstPcieStoragePmCapPtr[3];               ///< Offset 324     RST PCIe Storage Cycle Router#1 Power Management Capability Pointer
                                                    ///< Offset 325     RST PCIe Storage Cycle Router#2 Power Management Capability Pointer
                                                    ///< Offset 326     RST PCIe Storage Cycle Router#3 Power Management Capability Pointer
  UINT8    RstPcieStoragePcieCapPtr[3];             ///< Offset 327     RST PCIe Storage Cycle Router#1 PCIe Capabilities Pointer
                                                    ///< Offset 328     RST PCIe Storage Cycle Router#2 PCIe Capabilities Pointer
                                                    ///< Offset 329     RST PCIe Storage Cycle Router#3 PCIe Capabilities Pointer
  UINT16   RstPcieStorageL1ssCapPtr[3];             ///< Offset 330     RST PCIe Storage Cycle Router#1 L1SS Capability Pointer
                                                    ///< Offset 332     RST PCIe Storage Cycle Router#2 L1SS Capability Pointer
                                                    ///< Offset 334     RST PCIe Storage Cycle Router#3 L1SS Capability Pointer
  UINT8    RstPcieStorageEpL1ssControl2[3];         ///< Offset 336     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data2
                                                    ///< Offset 337     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data2
                                                    ///< Offset 338     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data2
  UINT32   RstPcieStorageEpL1ssControl1[3];         ///< Offset 339     RST PCIe Storage Cycle Router#1 Endpoint L1SS Control Data1
                                                    ///< Offset 343     RST PCIe Storage Cycle Router#2 Endpoint L1SS Control Data1
                                                    ///< Offset 347     RST PCIe Storage Cycle Router#3 Endpoint L1SS Control Data1
  UINT16   RstPcieStorageLtrCapPtr[3];              ///< Offset 351     RST PCIe Storage Cycle Router#1 LTR Capability Pointer
                                                    ///< Offset 353     RST PCIe Storage Cycle Router#2 LTR Capability Pointer
                                                    ///< Offset 355     RST PCIe Storage Cycle Router#3 LTR Capability Pointer
  UINT32   RstPcieStorageEpLtrData[3];              ///< Offset 357     RST PCIe Storage Cycle Router#1 Endpoint LTR Data
                                                    ///< Offset 361     RST PCIe Storage Cycle Router#2 Endpoint LTR Data
                                                    ///< Offset 365     RST PCIe Storage Cycle Router#3 Endpoint LTR Data
  UINT16   RstPcieStorageEpLctlData16[3];           ///< Offset 369     RST PCIe Storage Cycle Router#1 Endpoint LCTL Data
                                                    ///< Offset 371     RST PCIe Storage Cycle Router#2 Endpoint LCTL Data
                                                    ///< Offset 373     RST PCIe Storage Cycle Router#3 Endpoint LCTL Data
  UINT16   RstPcieStorageEpDctlData16[3];           ///< Offset 375     RST PCIe Storage Cycle Router#1 Endpoint DCTL Data
                                                    ///< Offset 377     RST PCIe Storage Cycle Router#2 Endpoint DCTL Data
                                                    ///< Offset 379     RST PCIe Storage Cycle Router#3 Endpoint DCTL Data
  UINT16   RstPcieStorageEpDctl2Data16[3];          ///< Offset 381     RST PCIe Storage Cycle Router#1 Endpoint DCTL2 Data
                                                    ///< Offset 383     RST PCIe Storage Cycle Router#2 Endpoint DCTL2 Data
                                                    ///< Offset 385     RST PCIe Storage Cycle Router#3 Endpoint DCTL2 Data
  UINT16   RstPcieStorageRpDctl2Data16[3];          ///< Offset 387     RST PCIe Storage Cycle Router#1 RootPort DCTL2 Data
                                                    ///< Offset 389     RST PCIe Storage Cycle Router#2 RootPort DCTL2 Data
                                                    ///< Offset 391     RST PCIe Storage Cycle Router#3 RootPort DCTL2 Data
  UINT32   RstPcieStorageUniqueTableBar[3];         ///< Offset 393     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 397     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR
                                                    ///< Offset 401     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR
  UINT32   RstPcieStorageUniqueTableBarValue[3];    ///< Offset 405     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 409     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X Table BAR value
                                                    ///< Offset 413     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X Table BAR value
  UINT32   RstPcieStorageUniquePbaBar[3];           ///< Offset 417     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 421     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR
                                                    ///< Offset 425     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR
  UINT32   RstPcieStorageUniquePbaBarValue[3];      ///< Offset 429     RST PCIe Storage Cycle Router#1 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 433     RST PCIe Storage Cycle Router#2 Endpoint unique MSI-X PBA BAR value
                                                    ///< Offset 437     RST PCIe Storage Cycle Router#3 Endpoint unique MSI-X PBA BAR value
  UINT8    SDME;                                    ///< Offset 441     SCS SDIO Controller Mode (0: disabled, 1: pci, 2: acpi)
  UINT8    SDIR;                                    ///< Offset 442     SCS SDIO Controller interrupt number
  UINT32   SDB0;                                    ///< Offset 443     SCS SDIO controller BAR0
  UINT32   SDB1;                                    ///< Offset 447     SCS SDIO controller BAR1
  UINT8    ExitBootServicesFlag;                    ///< Offset 451     Flag indicating Exit Boot Service, to inform SMM
  UINT32   SxMemBase;                               ///< Offset 452     Sx handler reserved MMIO base
  UINT32   SxMemSize;                               ///< Offset 456     Sx handler reserved MMIO size
} SC_NVS_AREA;

#pragma pack(pop)
#endif

