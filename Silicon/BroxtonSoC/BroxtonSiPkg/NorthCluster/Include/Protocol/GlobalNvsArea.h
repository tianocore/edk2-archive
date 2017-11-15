/** @file
  Header file for Global NVS Area definition.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _GLOBAL_NVS_AREA_H_
#define _GLOBAL_NVS_AREA_H_

//
// Includes
//
#define GLOBAL_NVS_DEVICE_ENABLE 1
#define GLOBAL_NVS_DEVICE_DISABLE 0

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }
//
// Revision id - Added TPM related fields
//
#define GLOBAL_NVS_AREA_RIVISION_1       1
//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

//
// Global NVS Area definition
//
#pragma pack (1)
typedef struct {
/**
  Miscellaneous Dynamic Values, the definitions below need to be matched
  GNVS definitions in Platform.ASL

**/
  UINT16      OperatingSystem;                        ///<   (00) Operating System
  UINT8       LidState;                               ///<   (02) Lid State (Lid Open = 1)
  UINT8       PowerState;                             ///<   (03) Power State (AC Mode = 1)
  //
  // Thermal Policy Values
  //
  UINT8       ActiveTripPoint;                        ///<   (04) Active Trip Point
  UINT8       CriticalThermalTripPoint;               ///<   (05) Critical Trip Point
  UINT8       BspDigitalThermalSensorTemperature;     ///<   (06) Digital Thermal Sensor 1 Reading
  UINT8       ApDigitalThermalSensorTemperature;      ///<   (07) Digital Thermal Sensor 2 Reading
  //
  // Processor Configuration Values
  //
  UINT8       ApicEnable;                             ///<   (08) APIC Enabled by SBIOS (APIC Enabled = 1)
  UINT8       LogicalProcessorCount;                  ///<   (09) Number of Logical Processors if MP Enabled != 0
  UINT8       CurrentDeviceList;                      ///<   (10) Current Attached Device List
  UINT16      CurrentDisplayState;                    ///<   (11) Current Display State
  UINT16      NextDisplayState;                       ///<   (13) Next Display State
  UINT8       NumberOfValidDeviceId;                  ///<   (15) Number of Valid Device IDs
  UINT32      DeviceId1;                              ///<   (16) Device ID 1
  UINT32      DeviceId2;                              ///<   (20) Device ID 2
  UINT32      DeviceId3;                              ///<   (24) Device ID 3
  UINT32      DeviceId4;                              ///<   (28) Device ID 4
  UINT32      DeviceId5;                              ///<   (32) Device ID 5
  //
  // Backlight Control Values
  //
  UINT8       BacklightControlSupport;                ///<   (36) Backlight Control Support
  UINT8       BrightnessPercentage;                   ///<   (37) Brightness Level Percentage
  UINT8       AlsEnable;                              ///<   (38) ALS Enable
  UINT8       MorData;                                ///<   (39) Memory Overwrite Request Data
  UINT32      PPResponse;                             ///<   (40) Physical Presence request operation response
  UINT8       PPRequest;                              ///<   (44) Physical Presence request operation
  UINT8       LastPPRequest;                          ///<   (45) Last Physical Presence request operation
  UINT8       BoardId;                                ///<   (46) Platform board id
  UINT32      IgdOpRegionAddress;                     ///<   (47) IGD OpRegion base address
  UINT8       IgdBootType;                            ///<   (51) IGD Boot Display Device
  UINT8       IgdPanelType;                           ///<   (52) IGD Panel Type CMOs option
  UINT8       IgdTvFormat;                            ///<   (53) IGD TV Format CMOS option
  UINT8       IgdTvMinor;                             ///<   (54) IGD TV Minor Format CMOS option
  UINT8       IgdPanelScaling;                        ///<   (55) IGD Panel Scaling
  UINT8       IgdBlcConfig;                           ///<   (56) IGD BLC Configuration
  UINT8       IgdBiaConfig;                           ///<   (57) IGD BIA Configuration
  UINT8       IgdSscConfig;                           ///<   (58) IGD SSC Configuration
  UINT8       Igd409;                                 ///<   (59) IGD 0409 Modified Settings Flag
  UINT8       Igd509;                                 ///<   (60) IGD 0509 Modified Settings Flag
  UINT8       Igd609;                                 ///<   (61) IGD 0609 Modified Settings Flag
  UINT8       Igd709;                                 ///<   (62) IGD 0709 Modified Settings Flag
  UINT8       IgdDvmtMemSize;                         ///<   (63) IGD DVMT Memory Size
  UINT8       IgdFunc1Enable;                         ///<   (64) IGD Function 1 Enable
  UINT8       IgdSciSmiMode;                          ///<   (65) GMCH SMI/SCI mode (0=SCI)
  UINT8       IgdPAVP;                                ///<   (66) IGD PAVP data
  UINT8       PcieOSCControl;                         ///<   (67) PCIE OSC Control
  UINT8       NativePCIESupport;                      ///<   (68) Native PCIE Setup Value
  UINT8       DisplaySupport;                         ///<   (69) _DOS Display Support Flag.
  UINT8       GlobalIoapicInterruptMode;              ///<   (70) Global IOAPIC/8259 Interrupt Mode Flag.
  UINT8       GlobalCoolingType;                      ///<   (71) Global Cooling Type Flag.
  UINT8       GlobalL01Counter;                       ///<   (72) Global L01 Counter.
  UINT32      DeviceId6;                              ///<   (73) Device ID 6
  UINT32      DeviceId7;                              ///<   (77) Device ID 7
  UINT32      DeviceId8;                              ///<   (81) Device ID 8
  UINT32      DeviceId9;                              ///<   (85) Device ID 9
  UINT32      DeviceId10;                             ///<   (89) Device ID 10
  UINT32      DeviceId11;                             ///<   (93) Device ID 11
  UINT32      DeviceId12;                             ///<   (97) Device ID 12
  UINT32      DeviceId13;                             ///<   (101) Device ID 13
  UINT32      DeviceId14;                             ///<   (105) Device ID 14
  UINT32      DeviceId15;                             ///<   (109) Device ID 15
  UINT32      HdaNhltAcpiAddr;                        ///<   (113) HD-Audio NHLT ACPI address
  UINT32      HdaNhltAcpiLength;                      ///<   (117) HD-Audio NHLT ACPI length
  UINT32      HdaDspFeatureMask;                      ///<   (121) HD-Audio DSP Feature Mask
  UINT8       PlatformFlavor;                         ///<   (125) Platform Flavor
  UINT8       BoardRev;                               ///<   (126) Board Rev
  UINT8       XhciMode;                               ///<   (127) xHCI controller mode
  UINT8       PmicEnable;                             ///<   (128) PMIC enable/disable
  UINT8       IpuAcpiMode;                            ///<   (129) IPU device Acpi type -- 0: Auto; 1: Acpi Igfx; 2: Acpi no Igfx
  UINT32      UART21Addr;                             ///<   (130) HSUART2 BAR1
  UINT32      GPIO0Addr;                              ///<   (134) GPIO0 BAR
  UINT32      GPIO0Len;                               ///<   (138) GPIO0 BAR Length
  UINT32      GPIO1Addr;                              ///<   (142) GPIO1 BAR
  UINT32      GPIO1Len;                               ///<   (146) GPIO1 BAR Length
  UINT32      GPIO2Addr;                              ///<   (150) GPIO2 BAR
  UINT32      GPIO2Len;                               ///<   (154) GPIO2 BAR Length
  UINT32      GPIO3Addr;                              ///<   (158) GPIO3 BAR
  UINT32      GPIO3Len;                               ///<   (162) GPIO3 BAR Length
  UINT32      GPIO4Addr;                              ///<   (166) GPIO4 BAR
  UINT32      GPIO4Len;                               ///<   (170) GPIO4 BAR Length
  UINT32      eMMCAddr;                               ///<   (174) eMMC BAR0
  UINT32      eMMCLen;                                ///<   (178) eMMC BAR0 Length
  UINT32      eMMC1Addr;                              ///<   (182) eMMC BAR1
  UINT32      eMMC1Len;                               ///<   (186) eMMC BAR1 Length
  UINT8       DptfEnable;                             ///<   (190) DPTF Enable
  UINT8       EnableSen0Participant;                  ///<   (191) EnableSen0Participant
  UINT8       PassiveThermalTripPointSen0;            ///<   (192) PassiveThermalTripPointSen0
  UINT8       CriticalThermalTripPointSen0S3;         ///<   (193) CriticalThermalTripPointSen0S3
  UINT8       HotThermalTripPointSen0;                ///<   (194) HotThermalTripPointSen0
  UINT8       CriticalThermalTripPointSen0;           ///<   (195) CriticalThermalTripPointSen0
  UINT8       DptfChargerDevice;                      ///<   (196) DptfChargerDevice
  UINT8       DptfDisplayDevice;                      ///<   (197) DptfDisplayDevice
  UINT8       DptfFanDevice;                          ///<   (198) DptfFanDevice
  UINT8       DptfProcessor;                          ///<   (199) DptfProcessor
  UINT32      DptfProcCriticalTemperature;            ///<   (200) DPTF Processor participant critical temperature
  UINT32      DptfProcPassiveTemperature;             ///<   (204) DPTF Processor participant passive temperature
  UINT32      DptfGenericCriticalTemperature0;        ///<   (208) DPTF Generic sensor0 participant critical temperature
  UINT32      DptfGenericPassiveTemperature0;         ///<   (212) DPTF Generic sensor0 participant passive temperature
  UINT32      DptfGenericCriticalTemperature1;        ///<   (216) DPTF Generic sensor1 participant critical temperature
  UINT32      DptfGenericPassiveTemperature1;         ///<   (220) DPTF Generic sensor1 participant passive temperature
  UINT32      DptfGenericCriticalTemperature2;        ///<   (224) Reserved
  UINT32      DptfGenericPassiveTemperature2;         ///<   (228) Reserved
  UINT32      DptfGenericCriticalTemperature3;        ///<   (232) DPTF Generic sensor3 participant critical temperature
  UINT32      DptfGenericPassiveTemperature3;         ///<   (236) DPTF Generic sensor3 participant passive temperature
  UINT32      DptfGenericCriticalTemperature4;        ///<   (240) DPTF Generic sensor3 participant critical temperature
  UINT32      DptfGenericPassiveTemperature4;         ///<   (244) DPTF Generic sensor3 participant passive temperature
  UINT8       CLpmSetting;                            ///<   (248) DPTF Current low power mode setting
  UINT32      DptfCriticalThreshold0;                 ///<   (249) DPTF Critical threshold0 for SCU
  UINT32      DptfCriticalThreshold1;                 ///<   (253) DPTF Critical threshold1 for SCU
  UINT32      DptfCriticalThreshold2;                 ///<   (257) DPTF Critical threshold2 for SCU
  UINT32      DptfCriticalThreshold3;                 ///<   (261) DPTF Critical threshold3 for SCU
  UINT32      DptfCriticalThreshold4;                 ///<   (265) DPTF Critical threshold4 for SCU
  UINT8       Reserved6;                              ///<   (269) Reserved
  UINT32      LPOEnable;                              ///<   (270) DPTF LPO Enable
  UINT32      LPOStartPState;                         ///<   (274) P-State start index
  UINT32      LPOStepSize;                            ///<   (278) Step size
  UINT32      LPOPowerControlSetting;                 ///<   (282) Power control setting
  UINT32      LPOPerformanceControlSetting;           ///<   (286) Performance control setting
  UINT8       DppmEnabled;                            ///<   (290) DPTF DPPM enable/disable (Deprecated)
  UINT8       BatteryChargingSolution;                ///<   (291) Battery charging solution 0-CLV 1-ULPMC
  UINT32      TPMAddress;                             ///<   (292) TPM Base Address
  UINT32      TPMLength;                              ///<   (296) TPM Length
  UINT8       PssDeveice;                             ///<   (300) PSS Device: 0 - None, 1 - Monzax 2K, 2 - Monzax 8K
  UINT8       ModemSel;                               ///<   (301) Modem selection: 0: Disabled, 1: 7260; 2: 7360;
  UINT8       GpsModeSel;                             ///<   (302) GNSS/GPS mode selection, 0: LPSS mode, 1: ISH mode
  UINT32      HdaDspModMask;                          ///<   (303) Hd-Audio DSP Post-Processing Module Mask
  UINT8       Reservedo;                              ///<   (307) OS Selection.
  UINT8       WifiSel;                                ///<   (308) Wi-Fi Device Select 0: SDIO Lightning Peak 1: SDIO Broadcom 2. PCIe Lightning Peak
  UINT32      IPCBar0Address;                         ///<   (309) IPC Bar0 base address
  UINT32      IPCBar0Length;                          ///<   (313) IPC Bar0 Length
  UINT32      SSRAMBar0Address;                       ///<   (317) IPC Bar0 base address
  UINT32      SSRAMBar0Length;                        ///<   (321) IPC Bar0 Length
  UINT32      IPCBIOSMailBoxData;                     ///<   (325) IPC BIOS mail box data
  UINT32      IPCBIOSMailBoxInterface;                ///<   (329) IPC BIOS mail box interface
  UINT32      P2SBBaseAddress;                        ///<   (333) P2SB Base Address
  UINT8       EDPV;                                   ///<   (337) Check for eDP display device
  UINT32      DIDX;                                   ///<   (338) Device ID for eDP device
  UINT8       PrmrrStatus;                            ///<   (342) SGX Feature Status
  UINT64      PrmrrBaseAddress;                       ///<   (343) SGX Feature PRMRR Base address
  UINT64      PrmrrSize;                              ///<   (351) SGX Feature PRMRR Length (854-861)
  UINT8       WorldCameraSel;                         ///<   (359) 0 - Disable, 1 - IMX214, 2 - IMX135
  UINT8       UserCameraSel;                          ///<   (360) 0 - Disable, 1 - OV2740
  UINT8       AudioSel;                               ///<   (361) 0 - Disable, 1 - WM8281, 2 - WM8998
  UINT32      LDMA11Addr;                             ///<   (362) DMA1 BAR1
  UINT32      LDMA11Len;                              ///<   (366) DMA1 BAR1 Length
  UINT8       CSDebugLightEC;                         ///<   (370) EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
  UINT8       ECLowPowerMode;                         ///<   (371) EC Low Power Mode: 1 - Enabled, 0 - Disabled
  UINT8       CSNotifyEC;                             ///<   (372) EC Notification of Low Power S0 Idle State
  UINT8       EnableModernStandby;                    ///<   (373) Enable / Disable Modern Standby Mode
  UINT32      I2C21Addr;                              ///<   (374) I2C2 BAR1
  UINT32      I2C21Len;                               ///<   (378) I2C2 BAR1 Length
  UINT32      I2C31Addr;                              ///<   (382) I2C3 BAR1
  UINT32      I2C31Len;                               ///<   (386) I2C3 BAR1 Length
  UINT32      I2C41Addr;                              ///<   (390) I2C4 BAR1
  UINT32      I2C41Len;                               ///<   (394) I2C4 BAR1 Length
  UINT32      I2C51Addr;                              ///<   (398) I2C5 BAR1
  UINT32      I2C51Len;                               ///<   (402) I2C5 BAR1 Length
  UINT32      I2C61Addr;                              ///<   (406) I2C6 BAR1
  UINT32      I2C61Len;                               ///<   (410) I2C6 BAR1 Length
  UINT32      I2C71Addr;                              ///<   (414) I2C7 BAR1
  UINT32      I2C71Len;                               ///<   (418) I2C7 BAR1 Length
  UINT32      UsbOtgAddr;                             ///<   (422) USB OTG BAR0
  UINT32      UsbOtgAddr1;                            ///<   (426) USB OTG BAR1
  UINT32      PWM1Addr;                               ///<   (430) PWM1 BAR0
  UINT32      PWM1Len;                                ///<   (434) PWM1 BAR0 Length
  UINT32      PWM11Addr;                              ///<   (438) PWM1 BAR1
  UINT32      PWM11Len;                               ///<   (442) PWM1 BAR1 Length
  UINT32      PWM21Addr;                              ///<   (446) PWM2 BAR1
  UINT32      PWM21Len;                               ///<   (450) PWM2 BAR1 Length  >> Not used in BXT
  UINT32      Port80DebugValue;                       ///<   (454) Port 80 Debug Port Value
  UINT8       Rtd3P0dl;                               ///<   (458) User selectable Delay for Device D0 transition.
  UINT16      LowPowerS0IdleConstraint;               ///<   (459) PEP Constraints

/**
  Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
     [2]   - En/Dis UART 0
     [3]   -        UART 1
     [4]   -        SDIO
     [5]   -        I2C 0
     [6]   -        I2C 1
     [7]   -        XHCI
     [8]   -        HD Audio (includes ADSP)
     [9]   -        Gfx
     [10]  -        EMMC
     [11]  -        SDXC
     [12]  -        CPU
**/
  UINT8       PepList;                                ///<   (461) RTD3 PEP support list
  //
  //(BIT0 - GFx , BIT1 - Sata, BIT2 - UART, BIT3 - SDHC, Bit4 - I2C0, BIT5 - I2C1, Bit6 - XHCI, Bit7 - Audio)
  //
  UINT8       PL1LimitCS;                             ///<   (462) set PL1 limit when entering CS
  UINT16      PL1LimitCSValue;                        ///<   (463) PL1 limit value
  UINT8       PstateCapping;                          ///<   (465) P-state Capping
  UINT8       PassiveThermalTripPoint;                ///<   (466) Passive Trip Point
  UINT16      RTD3Config0;                            ///<   (467) RTD3 Config Setting(BIT0:ZPODD,BIT1:USB Camera Port4, BIT2/3:SATA Port3, Bit4/5:Sata Port1/2, Bit6:Card Reader, Bit7:WWAN, Bit8:WSB SIP FAB1 Card reader)
  UINT8       Rtd3Support;                            ///<   (469) Runtime D3 support.
  UINT8       LowPowerS0Idle;                         ///<   (470) Low Power S0 Idle Enable
  UINT32      SPI21Addr;                              ///<   (471) SPI2 BAR1
  UINT32      SPI21Len;                               ///<   (475) SPI2 BAR1 Length
  UINT32      SPI31Addr;                              ///<   (479) SPI3 BAR1
  UINT32      SPI31Len;                               ///<   (483) SPI3 BAR1 Length
  UINT32      SDIO1Addr;                              ///<   (487) SDCard BAR1
  UINT32      SDIO1Len;                               ///<   (491) SDCard BAR1 Length
  UINT32      SDCard1Addr;                            ///<   (495) SDIO BAR1
  UINT32      SDCard1Len;                             ///<   (499) SDIO BAR1 Length
  UINT32      SPI1Addr;                               ///<   (503) SPI BAR1
  UINT32      SPI1Len;                                ///<   (507) SPI BAR1 Length
  UINT8       SataPortState;                          ///<   (511) SATA port state
  //
  //Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
  //
  UINT32      UART11Addr;                             ///<   (512) HSUART BAR1
  UINT32      UART11Len;                              ///<   (516) HSUART BAR1 Length
  UINT32      UART21Len;                              ///<   (520) HSUART2 BAR1 Length
  UINT8       WPCN381U;                               ///<   (524) WPCN381U: only used by BXT-P
  UINT8       ECAvailability;                         ///<   (525) Embedded Controller Availability Flag.
  UINT8       PowerButtonSupport;                     ///<   (526) 10sec Power button support
/**
  Bit0: 10 sec P-button Enable/Disable
  Bit1: Internal Flag
  Bit2: Rotation Lock flag, 0:unlock, 1:lock
  Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
  Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
  Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
  Bit6: Reserved for future use.
  Bit7: EC 10sec PB Override state for S3/S4 wake up.

**/
  UINT8       DebugState;                             ///<   (527) Debug State
  UINT32      IpuAddr;                                ///<   (528) IPU Base Address
  UINT8       NumberOfBatteries;                      ///<   (532) Number of batteries
  UINT8       BatteryCapacity0;                       ///<   (533) Battery 0 Stored Capacity
  UINT8       PciDelayOptimizationEcr;                ///<   (534) PciDelayOptimizationEcr
  UINT8       IgdHpllVco;                             ///<   (535) HPLL VCO
  UINT32      UsbTypeCOpBaseAddr;                     ///<   (536) USB Type C OpRegion base address
  UINT8       SelectBtDevice;                         ///<   (540) Blue Tooth Device Selection
  UINT32      DptfProcActiveTemperature;              ///<   (541) DptfProcActiveTemperature
  UINT8       EnableMemoryDevice;                     ///<   (545) EnableMemoryDevice
  UINT8       ActiveThermalTripPointTMEM;             ///<   (546) ActiveThermalTripPointTMEM
  UINT8       PassiveThermalTripPointTMEM;            ///<   (547) PassiveThermalTripPointTMEM
  UINT8       CriticalThermalTripPointTMEM;           ///<   (548) CriticalThermalTripPointTMEM
  UINT8       ThermalSamplingPeriodTMEM;              ///<   (549) ThermalSamplingPeriodTMEM
  UINT8       EnableSen1Participant;                  ///<   (550) EnableSen1Participant
  UINT8       ActiveThermalTripPointSen1;             ///<   (551) ActiveThermalTripPointSen1
  UINT8       PassiveThermalTripPointSen1;            ///<   (552) PassiveThermalTripPointSen1
  UINT8       CriticalThermalTripPointSen1;           ///<   (553) CriticalThermalTripPointSen1
  UINT8       SensorSamplingPeriodSen1;               ///<   (554) SensorSamplingPeriodSen1
  UINT8       EnableActivePolicy;                     ///<   (555) EnableActivePolicy
  UINT8       EnablePassivePolicy;                    ///<   (556) EnablePassivePolicy
  UINT8       EnableCriticalPolicy;                   ///<   (557) EnableCriticalPolicy
  UINT8       EnableAPPolicy;                         ///<   (558) EnableAPPolicy
  UINT8       PassiveTc1Value;                        ///<   (559) Passive Trip Point TC1 Value
  UINT8       PassiveTc2Value;                        ///<   (560) Passive Trip Point TC2 Value
  UINT8       PassiveTspValue;                        ///<   (561) Passive Trip Point TSP Value
  UINT8       EnableDigitalThermalSensor;             ///<   (562) Digital Thermal Sensor Enable
  UINT8       Reserved0;                              ///<   (563) Reserved
  UINT8       IgdState;                               ///<   (564) IGD State
  UINT8       HighPerfMode;                           ///<   (565) Enable/Disable HighPerformance mode for Dptf
  UINT8       DptfWwanDevice;                         ///<   (566) DPTF WWAN
  UINT8       EnableSen3Participant;                  ///<   (567) EnableSen3Participant
  UINT8       PassiveThermalTripPointSen3;            ///<   (568) PassiveThermalTripPointSen3
  UINT8       CriticalThermalTripPointSen3S3;         ///<   (569) CriticalThermalTripPointSen3S3
  UINT8       HotThermalTripPointSen3;                ///<   (570) HotThermalTripPointSen3
  UINT8       CriticalThermalTripPointSen3;           ///<   (571) CriticalThermalTripPointSen3
  UINT8       Reserved1;                              ///<   Reserved
  UINT8       PanelSel;                               ///<   (573) Panel AOB  0 - Disable, 1 - TIANMA , 2 - TRULY Fab B TypeC, 3 - TRULY Fab B, 4 -TRULY Fab B Command Mode, 5 - TRULY Fab B Command Mode TypeC
  UINT8       IrmtCfg;                                ///<   (574) IRMT Configuration
  UINT8       ThermalSamplingPeriodTCPU;              ///<   (575) ThermalSamplingPeriodTCPU
  UINT8       BxtStepping;                            ///<   (576) BXT Stepping
  UINT8       Reserved2;                              ///<   (577) Reserved
  UINT8       LtrEnable[6];                           ///<   (578) Latency Tolerance Reporting Enable
                                                      ///<   (579) Latency Tolerance Reporting Enable
                                                      ///<   (580) Latency Tolerance Reporting Enable
                                                      ///<   (581) Latency Tolerance Reporting Enable
                                                      ///<   (582) Latency Tolerance Reporting Enable
                                                      ///<   (583) Latency Tolerance Reporting Enable
  UINT8       ObffEnable[6];                          ///<   (584) Optimized Buffer Flush and Fill
                                                      ///<   (585) Optimized Buffer Flush and Fill
                                                      ///<   (586) Optimized Buffer Flush and Fill
                                                      ///<   (587) Optimized Buffer Flush and Fill
                                                      ///<   (588) Optimized Buffer Flush and Fill
                                                      ///<   (589) Optimized Buffer Flush and Fill
  UINT32      RpAddress[6];                           ///<   (590) Root Port address 1
                                                      ///<   (594) Root Port address 2
                                                      ///<   (598) Root Port address 3
                                                      ///<   (602) Root Port address 4
                                                      ///<   (606) Root Port address 5
                                                      ///<   (610) Root Port address 6
  UINT16      PcieLtrMaxSnoopLatency[6];              ///<   (614) PCIE LTR max snoop Latency 1
                                                      ///<   (616) PCIE LTR max snoop Latency 2
                                                      ///<   (618) PCIE LTR max snoop Latency 3
                                                      ///<   (620) PCIE LTR max snoop Latency 4
                                                      ///<   (622) PCIE LTR max snoop Latency 5
                                                      ///<   (624) PCIE LTR max snoop Latency 6
  UINT16      PcieLtrMaxNoSnoopLatency[6];            ///<   (626) PCIE LTR max no snoop Latency 1
                                                      ///<   (628) PCIE LTR max no snoop Latency 2
                                                      ///<   (630) PCIE LTR max no snoop Latency 3
                                                      ///<   (632) PCIE LTR max no snoop Latency 4
                                                      ///<   (634) PCIE LTR max no snoop Latency 5
                                                      ///<   (636) PCIE LTR max no snoop Latency 6
  UINT8       TrtRevision;                            ///<   (638) TrtRevision
  UINT32      Reserved3;                              ///<
  UINT8       EnablePowerParticipant;                 ///<   (643) EnablePowerParticipant
  UINT8       EnablePowerBossPolicy;                  ///<   (644) EnablePowerBossPolicy
  //
  // Hybrid Graphics Support
  //
  UINT8       HgEnabled;                              ///<   (645) HG Enabled (0=Disabled, 1=Enabled)
  UINT32      XPcieCfgBaseAddress;                    ///<   (646) Any Device's PCIe Config Space Base Address
  UINT16      DelayAfterPwrEn;                        ///<   (650) Delay after Power Enable
  UINT16      DelayAfterHoldReset;                    ///<   (652) Delay after Hold Reset
  UINT32      HgHoldRstCommOffset;                    ///<   (654) dGPU HLD RST GPIO Community Offset
  UINT16      HgHoldRstPinOffset;                     ///<   (658) dGPU HLD RST GPIO Pin Offset
  UINT8       HgHoldRstActiveInfo;                    ///<   (660) dGPU HLD RST GPIO Active Information
  UINT32      HgPwrEnableCommOffset;                  ///<   (661) dGPU PWR Enable GPIO Community Offset
  UINT16      HgPwrEnablePinOffset;                   ///<   (665) dGPU PWR Enable GPIO Pin Offset
  UINT8       HgPwrEnableActiveInfo;                  ///<   (667) dGPU PWR Enable GPIO Active Information
  UINT8       PcieEpSecBusNum;                        ///<   (668) dGPU Root Port Base Address
  UINT8       PcieEpCapOffset;                        ///<   (669) dGPU Base Address
  UINT32      RootPortBaseAddress;                    ///<   (670) dGPU Root Port Base Address
  UINT32      NvIgOpRegionAddress;                    ///<   (674) NVIG support
  UINT32      NvHmOpRegionAddress;                    ///<   (678) NVHM support
  UINT32      ApXmOpRegionAddress;                    ///<   (682) AMDA support
  UINT8       EnableGen1Participant;                  ///<   (686) EnableGen1Participant
  UINT8       EnableGen2Participant;                  ///<   (687) EnableGen2Participant
  UINT8       EnableGen3Participant;                  ///<   (688) EnableGen3Participant
  UINT8       EnableGen4Participant;                  ///<   (689) EnableGen4Participant
  UINT8       ActiveThermalTripPointGen1;             ///<   (690) ActiveThermalTripPointGen1
  UINT8       PassiveThermalTripPointGen1;            ///<   (691) PassiveThermalTripPointGen1
  UINT8       CriticalThermalTripPointGen1;           ///<   (692) CriticalThermalTripPointGen1
  UINT8       HotThermalTripPointGen1;                ///<   (693) HotThermalTripPointGen1
  UINT8       CriticalThermalTripPointGen1S3;         ///<   (694) CriticalThermalTripPointGen1S3
  UINT8       ThermistorSamplingPeriodGen1;           ///<   (695) ThermistorSamplingPeriodGen1
  UINT8       ActiveThermalTripPointGen2;             ///<   (696) ActiveThermalTripPointGen2
  UINT8       PassiveThermalTripPointGen2;            ///<   (697) PassiveThermalTripPointGen2
  UINT8       CriticalThermalTripPointGen2;           ///<   (698) CriticalThermalTripPointGen2
  UINT8       HotThermalTripPointGen2;                ///<   (699) HotThermalTripPointGen2
  UINT8       CriticalThermalTripPointGen2S3;         ///<   (700) CriticalThermalTripPointGen2S3
  UINT8       ThermistorSamplingPeriodGen2;           ///<   (701) ThermistorSamplingPeriodGen2
  UINT8       ActiveThermalTripPointGen3;             ///<   (702) ActiveThermalTripPointGen3
  UINT8       PassiveThermalTripPointGen3;            ///<   (703) PassiveThermalTripPointGen3
  UINT8       CriticalThermalTripPointGen3;           ///<   (704) CriticalThermalTripPointGen3
  UINT8       HotThermalTripPointGen3;                ///<   (705) HotThermalTripPointGen3
  UINT8       CriticalThermalTripPointGen3S3;         ///<   (706) CriticalThermalTripPointGen3S3
  UINT8       ThermistorSamplingPeriodGen3;           ///<   (707) ThermistorSamplingPeriodGen3
  UINT8       ActiveThermalTripPointGen4;             ///<   (708) ActiveThermalTripPointGen4
  UINT8       PassiveThermalTripPointGen4;            ///<   (709) PassiveThermalTripPointGen4
  UINT8       CriticalThermalTripPointGen4;           ///<   (710) CriticalThermalTripPointGen4
  UINT8       HotThermalTripPointGen4;                ///<   (711) HotThermalTripPointGen4
  UINT8       CriticalThermalTripPointGen4S3;         ///<   (712) CriticalThermalTripPointGen4S3
  UINT8       ThermistorSamplingPeriodGen4;           ///<   (713) ThermistorSamplingPeriodGen4
  UINT32      DptfProcCriticalTemperatureS3;          ///<   (714) DptfProcCriticalTemperatureS3
  UINT32      DptfProcHotThermalTripPoint;            ///<   (718) DptfProcHotThermalTripPoint
  UINT8       CriticalThermalTripPointSen1S3;         ///<   (722) CriticalThermalTripPointSen1S3
  UINT8       HotThermalTripPointSen1;                ///<   (723) HotThermalTripPointSen1
  UINT8       PmicStepping;                           ///<   (724) PMIC Stepping
  UINT8       ScHdAudioIoBufferOwnership;             ///<   (725) ScHdAudioIoBufferOwnership
  UINT8       XdciEnable;                             ///<   (726) Xdci Enable
  UINT8       Reserved4;                              ///<   (727) Reserved
  UINT8       Reserved5;                              ///<   (728) Reserved
  UINT8       VirtualKeyboard;                        ///<   (729) Virtual keyboard Function 0- Disable 1- Discrete Touch 2- Integrated Touch
  UINT8       WiGigEnable;                            ///<   (730) WiGig Enable for BXTM B0
  UINT16      WiGigSPLCPwrLimit;                      ///<   (731) WiGig SPLC Power Limit
  UINT32      WiGigSPLCTimeWindow;                    ///<   (733) WiGig SPLC Time Window
  UINT8       PsmEnable;                              ///<   (737) WiGig Power sharing manager enabling
  UINT8       PsmSPLC0DomainType;                     ///<   (738) WiGig PSM SPLC0 Domain Type
  UINT16      PsmSPLC0PwrLimit;                       ///<   (739) WiGig PSM SPLC0 Power Limit
  UINT32      PsmSPLC0TimeWindow;                     ///<   (741) WiGig PSM SPLC0 Time Window
  UINT8       PsmSPLC1DomainType;                     ///<   (745) WiGig PSM SPLC1 Domain Type
  UINT16      PsmSPLC1PwrLimit;                       ///<   (746) WiGig PSM SPLC1 Power Limit
  UINT32      PsmSPLC1TimeWindow;                     ///<   (748) WiGig PSM SPLC1 Time Window
  UINT8       PsmDPLC0DomainType;                     ///<   (752) WiGig PSM DPLC0 Domain Type
  UINT8       PsmDPLC0DomainPerference;               ///<   (753) WiGig PSM DPLC0 Domain Preference
  UINT8       PsmDPLC0PowerLimitIndex;                ///<   (754) WiGig PSM DPLC0 Power Limit Index
  UINT16      PsmDPLC0PwrLimit;                       ///<   (755) WiGig PSM DPLC0 Power Limit
  UINT32      PsmDPLC0TimeWindow;                     ///<   (757) WiGig PSM DPLC0 Time Window
  UINT8       PsmDPLC1DomainType;                     ///<   (761) WiGig PSM DPLC1 Domain Type
  UINT8       PsmDPLC1DomainPerference;               ///<   (762) WiGig PSM DPLC1 Domain Preference
  UINT8       PsmDPLC1PowerLimitIndex;                ///<   (763) WiGig PSM DPLC1 Power Limit Index
  UINT16      PsmDPLC1PwrLimit;                       ///<   (764) WiGig PSM DPLC1 Power Limit
  UINT32      PsmDPLC1TimeWindow;                     ///<   (766) WiGig PSM DPLC1 Time Window
  UINT8       I2s343A;                                ///<   (770) I2S audio codec device - INT343A
  UINT8       I2s34C1;                                ///<   (771) I2S audio codec device - INT34C1
  UINT8       I2cNfc;                                 ///<   (772) I2C NFC device - NXP1001
  UINT8       I2cPss;                                 ///<   (773) I2S PSS device - IMPJ0003
  UINT8       UartBt;                                 ///<   (774) UART BT device - BCM2E40
  UINT8       UartGps;                                ///<   (775) UART GPS device - BCM4752
  UINT64      HdaDspPpModCustomGuid1Low;              ///<   (776)
  UINT64      HdaDspPpModCustomGuid1High;             ///<   (784)
  UINT64      HdaDspPpModCustomGuid2Low;              ///<   (792)
  UINT64      HdaDspPpModCustomGuid2High;             ///<   (800)
  UINT64      HdaDspPpModCustomGuid3Low;              ///<   (808)
  UINT64      HdaDspPpModCustomGuid3High;             ///<   (816)
  UINT16      PowerParticipantPollingRate;            ///<   (824) DPTF PowerParticipantPollingRate
  UINT16      EnableDCFG;                             ///<   (826) DPTF Enable DPTF Configuration
  UINT8       OemDesignVariable0;                     ///<   (828) DPTF Oem Design Variable
  UINT8       OemDesignVariable1;                     ///<   (829) DPTF Oem Design Variable
  UINT8       OemDesignVariable2;                     ///<   (830) DPTF Oem Design Variable
  UINT8       OemDesignVariable3;                     ///<   (831) DPTF Oem Design Variable
  UINT8       OemDesignVariable4;                     ///<   (832) DPTF Oem Design Variable
  UINT8       OemDesignVariable5;                     ///<   (833) DPTF Oem Design Variable
  UINT8       EnableVS1Participant;                   ///<   (834) EnableVS1Participant
  UINT8       ActiveThermalTripPointVS1;              ///<   (835) ActiveThermalTripPointVS1
  UINT8       PassiveThermalTripPointVS1;             //<    (836) PassiveThermalTripPointVS1
  UINT8       CriticalThermalTripPointVS1;            ///<   (837) CriticalThermalTripPointVS1
  UINT8       CriticalThermalTripPointVS1S3;          ///<   (838) CriticalThermalTripPointVS1S3
  UINT8       HotThermalTripPointVS1;                 ///<   (839) HotThermalTripPointVS1
  UINT8       EnableVS2Participant;                   ///<   (840) EnableVS2Participant
  UINT8       ActiveThermalTripPointVS2;              ///<   (841) ActiveThermalTripPointVS2
  UINT8       PassiveThermalTripPointVS2;             //<    (842) PassiveThermalTripPointVS2
  UINT8       CriticalThermalTripPointVS2;            ///<   (843) CriticalThermalTripPointVS2
  UINT8       CriticalThermalTripPointVS2S3;          ///<   (844) CriticalThermalTripPointVS2S3
  UINT8       HotThermalTripPointVS2;                 ///<   (845) HotThermalTripPointVS2
  UINT8       EnableVS3Participant;                   ///<   (846) EnableVS3Participant
  UINT8       ActiveThermalTripPointVS3;              ///<   (847) ActiveThermalTripPointVS3
  UINT8       PassiveThermalTripPointVS3;             ///<   (848) PassiveThermalTripPointVS3
  UINT8       CriticalThermalTripPointVS3;            ///<   (849) CriticalThermalTripPointVS3
  UINT8       CriticalThermalTripPointVS3S3;          ///<   (850) CriticalThermalTripPointVS3S3
  UINT8       HotThermalTripPointVS3;                 ///<   (851) HotThermalTripPointVS3
  UINT8       EnableVSPolicy;                         ///<   (852) EnableVsPolicy
  UINT8       Spi1SensorDevice;                       ///<   (853) SPI1 Fingerprint device - FPC1020/FPC1021
  UINT8       NfcSelect;                              ///<   (854) NFC device select: 0: disabled; 1: NFC (IPT)/secure NFC; 2: NFC;
  UINT16      RfemSPLCPwrLimit;                       ///<   (855) RFEM SPLC Power Limit
  UINT32      RfemSPLCTimeWindow;                     ///<   (857) RFEM SPLC Time Window
  UINT8       PanelSelect;                            ///<   (861) Panel Selection (0=eDP, >=1 for MIPI)
  UINT8       EPIEnable;                              ///<   (862) EPIEnable
  UINT8       TypeCEnable;                            ///<   (863) TypeCEnable
  UINT8       PassiveThermalTripPointWWAN;            ///<   (864) PassiveThermalTripPointWWAN
  UINT8       CriticalThermalTripPointWWANS3;         ///<   (865) CriticalThermalTripPointWWANS3
  UINT8       HotThermalTripPointWWAN;                ///<   (866) HotThermalTripPointWWAN
  UINT8       CriticalThermalTripPointWWAN;           ///<   (867) CriticalThermalTripPointWWAN
  UINT8       DisplayHighLimit;                       ///<   (868) DisplayHighLimit
  UINT8       DisplayLowLimit;                        ///<   (869) DisplayLowLimit
  UINT8       OsDbgEnable;                            ///<   (871) OsDbgEnable
  UINT32      Mmio32Base;                             ///<   (874) PCIE MMIO resource base
  UINT32      Mmio32Length;                           ///<   (878) PCIE MMIO resource length
  UINT8       CameraRotationAngle;                    ///<   (883) Camera Sensor Rotation Angle
  UINT8       I2cTouchPanel;                          ///<   (884) I2c Touch Panel
  UINT8       I2cTouchPad;                            ///<   (885) I2c Touch pad
  UINT32      I2C0Speed;                              ///<   (886) I2C0 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C1Speed;                              ///<   (890) I2C1 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C2Speed;                              ///<   (894) I2C2 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C3Speed;                              ///<   (898) I2C3 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C4Speed;                              ///<   (902) I2C4 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C5Speed;                              ///<   (906) I2C5 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C6Speed;                              ///<   (910) I2C6 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT32      I2C7Speed;                              ///<   (914) I2C7 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
  UINT8       EnableSen2Participant;                  ///<   (918) EnableSen2Participant
  UINT8       PassiveThermalTripPointSen2;            ///<   (919) PassiveThermalTripPointSen2
  UINT8       CriticalThermalTripPointSen2S3;         ///<   (920) CriticalThermalTripPointSen2S3
  UINT8       HotThermalTripPointSen2;                ///<   (921) HotThermalTripPointSen2
  UINT8       CriticalThermalTripPointSen2;           ///<   (922) CriticalThermalTripPointSen2
  UINT8       SueCreekEnable;                         ///<   (923) SueCreekEnable: 0: disabled; 1: enabled
  UINT8       Ti3100AudioCodecEnable;                 ///<   (924) TI3100 Audio Codec: 0:Disable; 1:Enable
} EFI_GLOBAL_NVS_AREA;
#pragma pack ()

//
// Global NVS Area Protocol
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  EFI_GLOBAL_NVS_AREA     *Area;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

#endif

