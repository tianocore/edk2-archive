/** @file
  ACPI GNVS

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

  //
  // Define a Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.
  //

  OperationRegion(GNVS, SystemMemory, 0xFFFF0000, 0xAA55)
  Field(GNVS, AnyAcc, Lock, Preserve)
  {
    Offset(0),      // Miscellaneous Dynamic Registers:
//  Name-----Size-------Offset----Description
    OSYS,    16,    //   (00) Operating System
    LIDS,    8,     //   (02) Lid State (Lid Open = 1)
    PWRS,    8,     //   (03) Power State (AC Mode = 1)
    ACTT,    8,     //   (04) Active Trip Point
    CRTT,    8,     //   (05) Critical Trip Point
    DTS1,    8,     //   (06) Digital Thermal Sensor 1 Reading
    DTS2,    8,     //   (07) Digital Thermal Sensor 2 Reading
    APIC,    8,     //   (08) APIC Enabled by SBIOS (APIC Enabled = 1)
    MPEN,    8,     //   (09) Number of Logical Processors if MP Enabled != 0
    CADL,    8,     //   (10) Current Attached Device List
    CSTE,    16,    //   (11) Current Display State
    NSTE,    16,    //   (13) Next Display State
    NDID,    8,     //   (15) Number of Valid Device IDs
    DID1,    32,    //   (16) Device ID 1
    DID2,    32,    //   (20) Device ID 2
    DID3,    32,    //   (24) Device ID 3
    DID4,    32,    //   (28) Device ID 4
    DID5,    32,    //   (32) Device ID 5
    BLCS,    8,     //   (36) Backlight Control Support
    BRTL,    8,     //   (37) Brightness Level Percentage
    ALSE,    8,     //   (38) ALS Enable
    MORD,    8,     //   (39) Memory Overwrite Request Data
    PPRP,    32,    //   (40) Physical Presence request operation response
    PPRQ,    8,     //   (44) Physical Presence request operation
    LPPR,    8,     //   (45) Last Physical Presence request operation
    BDID,    8,     //   (46) Platform board id
    ASLB,    32,    //   (47) IGD OpRegion base address
    IBTT,    8,     //   (51) IGD Boot Display Device
    IPAT,    8,     //   (52) IGD Panel Type CMOs option
    ITVF,    8,     //   (53) IGD TV Format CMOS option
    ITVM,    8,     //   (54) IGD TV Minor Format CMOS option
    IPSC,    8,     //   (55) IGD Panel Scaling
    IBLC,    8,     //   (56) IGD BLC Configuration
    IBIA,    8,     //   (57) IGD BIA Configuration
    ISSC,    8,     //   (58) IGD SSC Configuration
    I409,    8,     //   (59) IGD 0409 Modified Settings Flag
    I509,    8,     //   (60) IGD 0509 Modified Settings Flag
    I609,    8,     //   (61) IGD 0609 Modified Settings Flag
    I709,    8,     //   (62) IGD 0709 Modified Settings Flag
    IDMS,    8,     //   (63) IGD DVMT Memory Size
    IF1E,    8,     //   (64) IGD Function 1 Enable
    GSMI,    8,     //   (65) GMCH SMI/SCI mode (0=SCI)
    PAVP,    8,     //   (66) IGD PAVP data
    OSCC,    8,     //   (67) PCIE OSC Control
    NEXP,    8,     //   (68) Native PCIE Setup Value
    DSEN,    8,     //   (69) _DOS Display Support Flag.
    GPIC,    8,     //   (70) Global IOAPIC/8259 Interrupt Mode Flag.
    CTYP,    8,     //   (71) Global Cooling Type Flag.
    L01C,    8,     //   (72) Global L01 Counter.
    DID6,    32,    //   (73) Device ID 6
    DID7,    32,    //   (77) Device ID 7
    DID8,    32,    //   (81) Device ID 8
    DID9,    32,    //   (85) Device ID 9
    DIDA,    32,    //   (89) Device ID 10
    DIDB,    32,    //   (93) Device ID 11
    DIDC,    32,    //   (97) Device ID 12
    DIDD,    32,    //   (101) Device ID 13
    DIDE,    32,    //   (105) Device ID 14
    DIDF,    32,    //   (109) Device ID 15
    NHLA,    32,    //   (113) HD-Audio NHLT ACPI address
    NHLL,    32,    //   (117) HD-Audio NHLT ACPI length
    ADFM,    32,    //   (121) HD-Audio DSP Feature Mask
    PFLV,    8,     //   (125) Platform Flavor
    BREV,    8,     //   (126) Board Rev
    XHCI,    8,     //   (127) xHCI controller mode
    PMEN,    8,     //   (128) PMIC enable/disable
    IPUD,    8,     //   (129) IPU device Acpi type -- 0: Auto; 1: Acpi Igfx; 2: Acpi no Igfx
    U21A,    32,    //   (130) HSUART2 BAR1
    GP0A,    32,    //   (134) GPIO0 BAR
    GP0L,    32,    //   (138) GPIO0 BAR Length
    GP1A,    32,    //   (142) GPIO1 BAR
    GP1L,    32,    //   (146) GPIO1 BAR Length
    GP2A,    32,    //   (150) GPIO2 BAR
    GP2L,    32,    //   (154) GPIO2 BAR Length
    GP3A,    32,    //   (158) GPIO3 BAR
    GP3L,    32,    //   (162) GPIO3 BAR Length
    GP4A,    32,    //   (166) GPIO4 BAR
    GP4L,    32,    //   (170) GPIO4 BAR Length
    eM0A,    32,    //   (174) eMMC BAR0
    eM0L,    32,    //   (178) eMMC BAR0 Length
    eM1A,    32,    //   (182) eMMC BAR1
    eM1L,    32,    //   (186) eMMC BAR1 Length
    DPTE,    8,     //   (190) DPTF Enable
    S0DE,    8,     //   (191) EnableSen0Participant
    S0PT,    8,     //   (192) PassiveThermalTripPointSen0
    S0C3,    8,     //   (193) CriticalThermalTripPointSen0S3
    S0HT,    8,     //   (194) HotThermalTripPointSen0
    S0CT,    8,     //   (195) CriticalThermalTripPointSen0
    CHGE,    8,     //   (196) DptfChargerDevice
    DDSP,    8,     //   (197) DPTFDisplayDevice
    DFAN,    8,     //   (198) DPTF Fan device
    DPSR,    8,     //   (199) DPTF Processor device
    DPCT,    32,    //   (200) DPTF Processor participant critical temperature
    DPPT,    32,    //   (204) DPTF Processor participant passive temperature
    DGC0,    32,    //   (208) DPTF Generic sensor0 participant critical temperature
    DGP0,    32,    //   (212) DPTF Generic sensor0 participant passive temperature
    DGC1,    32,    //   (216) DPTF Generic sensor1 participant critical temperature
    DGP1,    32,    //   (220) DPTF Generic sensor1 participant passive temperature
    DGC2,    32,    //   (224) DPTF Generic sensor2 participant critical temperature
    DGP2,    32,    //   (228) DPTF Generic sensor2 participant passive temperature
    DGC3,    32,    //   (232) DPTF Generic sensor3 participant critical temperature
    DGP3,    32,    //   (236) DPTF Generic sensor3 participant passive temperature
    DGC4,    32,    //   (240) DPTF Generic sensor3 participant critical temperature
    DGP4,    32,    //   (244) DPTF Generic sensor3 participant passive temperature
    DLPM,    8,     //   (248) DPTF Current low power mode setting
    DSC0,    32,    //   (249) DPTF Critical threshold0 for SCU
    DSC1,    32,    //   (253) DPTF Critical threshold1 for SCU
    DSC2,    32,    //   (257) DPTF Critical threshold2 for SCU
    DSC3,    32,    //   (261) DPTF Critical threshold3 for SCU
    DSC4,    32,    //   (265) DPTF Critical threshold4 for SCU
        ,     8,    //   (269) Reserved
    LPOE,    32,    //   (270) DPTF LPO Enable
    LPPS,    32,    //   (274) P-State start index
    LPST,    32,    //   (278) Step size
    LPPC,    32,    //   (282) Power control setting
    LPPF,    32,    //   (286) Performance control setting
    DPME,    8,     //   (290) DPTF DPPM enable/disable
    BCSL,    8,     //   (291) Battery charging solution 0-CLV 1-ULPMC
    TPMA,    32,    //   (292) TPM Base Address
    TPML,    32,    //   (296) TPM Length
    PSSD,    8,     //   (300) PSS Device: 0 - None, 1 - Monzax 2K, 2 - Monzax 8K
    MDMS,    8,     //   (301) Modem selection: 0: Disabled, 1: 7260; 2: 7360;
    GPSM,    8,     //   (302) GNSS/GPS mode selection, 0: LPSS mode, 1: ISH mode
    ADPM,    32,    //   (303) Hd-Audio DSP Post-Processing Module Mask
    OSSL,    8,     //   (307) OS Selection 0: WOS 1:AOS: 2:Legacy OS
    WIFD,    8,     //   (308) Wi-Fi Device Select 0: Lightning Peak 1: Broadcom: only used by BXT-M
    DD1A,    32,    //   (309) IPC Bar0 Address
    DD1L,    32,    //   (313) IPC Bar0 Length
    DD3A,    32,    //   (317) IPC Bar1 Address
    DD3L,    32,    //   (321) IPC Bar1 Length
    BMDA,    32,    //   (325) IPC BIOS mail box data
    BMIA,    32,    //   (329) IPC BIOS mail box interface
    P2BA,    32,    //   (333) P2SB Base Address: only used by BXT-M
    EDPV,    8,     //   (337) Check for eDP display device: only used by BXT-M
    DIDX,    32,    //   (338) Device ID for eDP device: only used by BXT-M
    EPCS,    8,     //   (342) SGX Feature Status
    EMNA,    64,    //   (343) SGX Feature PRMRR Base address
    ELNG,    64,    //   (351) SGX Feature PRMRR Length (854-861)
    WCAS,    8,     //   (359) 0 - Disable, 1 - IMX214, 2 - IMX135
    UCAS,    8,     //   (360) 0 - Disable, 1 - OV2740
    ADOS,    8,     //   (361) 0 - Disable, 1 - WM8281, 2 - WM8998
//------------------Delta between BXTM and BXTP--------------------
    D11A,    32,    //   (362) DMA1 BAR1
    D11L,    32,    //   (366) DMA1 BAR1 Length  >> Not used in BXT
    ECDB,    8,     //   (370) [CSDebugLightEC] EC Debug Light (CAPS LOCK) for when in Low Power S0 Idle State
    ECLP,    8,     //   (371) EC Low Power Mode: 1 - Enabled, 0 - Disabled
    ECNO,    8,     //   (372) [CSNotifyEC] EC Notification of Low Power S0 Idle State
    EMOD,    8,     //   (373) Enable / Disable Modern Standby Mode
    I21A,    32,    //   (374) I2C2 BAR1
    I21L,    32,    //   (378) I2C2 BAR1 Length
    I31A,    32,    //   (382) I2C3 BAR1
    I31L,    32,    //   (386) I2C3 BAR1 Length
    I41A,    32,    //   (390) I2C4 BAR1
    I41L,    32,    //   (394) I2C4 BAR1 Length
    I51A,    32,    //   (398) I2C5 BAR1
    I51L,    32,    //   (402) I2C5 BAR1 Length
    I61A,    32,    //   (406) I2C6 BAR1
    I61L,    32,    //   (410) I2C6 BAR1 Length
    I71A,    32,    //   (414) I2C7 BAR1
    I71L,    32,    //   (418) I2C7 BAR1 Length
    OTG0,    32,    //   (422) USB OTG BAR0
    OTG1,    32,    //   (426) USB OTG BAR1
    P10A,    32,    //   (430) PWM1 BAR0
    P10L,    32,    //   (434) PWM1 BAR0 Length
    P11A,    32,    //   (438) PWM1 BAR1
    P11L,    32,    //   (442) PWM1 BAR1 Length
    P21A,    32,    //   (446) PWM2 BAR1
    P21L,    32,    //   (450) PWM2 BAR1 Length  >> Not used in BXT
    P80D,    32,    //   (454) Port 80 Debug Port Value
    PEP0,    8,     //   (458) [Rtd3P0dl] User selectable Delay for Device D0 transition.
    PEPC,    16,    //   (459) [LowPowerS0IdleConstraint] PEP Constraints
                    // Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
                    //    [2]   - En/Dis UART 0
                    //    [3]   -        UART 1
                    //    [4]   -        SDIO
                    //    [5]   -        I2C 0
                    //    [6]   -        I2C 1
                    //    [7]   -        XHCI
                    //    [8]   -        HD Audio (includes ADSP)
                    //    [9]   -        Gfx
                    //    [10]  -        EMMC
                    //    [11]  -        SDXC
                    //    [12]  -        CPU
    PEPY,    8,     //   (461) [PepList] RTD3 PEP support list(BIT0 - GFx , BIT1 - Sata, BIT2 - UART, BIT3 - SDHC, Bit4 - I2C0, BIT5 - I2C1, Bit6 - XHCI, Bit7 - Audio)
    PLCS,    8,     //   (462) [PL1LimitCS] set PL1 limit when entering CS
    PLVL,    16,    //   (463) [PL1LimitCSValue] PL1 limit value
    PSCP,    8,     //   (465) [PstateCapping] P-state Capping
    PSVT,    8,     //   (466) Passive Trip Point
    RCG0,    16,    //   (467) [RTD3Config0] RTD3 Config Setting
                               //(BIT0:ZPODD,BIT1:USB Camera Port4, BIT2/3:SATA Port3, Bit4/5:Sata Port1/2, Bit6:Card Reader, Bit7:WWAN, Bit8:WSB SIP FAB1 Card reader)
    RTD3,    8,     //   (469) Runtime D3 support.
    S0ID,    8,     //   (470) [LowPowerS0Idle] Low Power S0 Idle Enable
    S21A,    32,    //   (471) SPI2 BAR1
    S21L,    32,    //   (475) SPI2 BAR1 Length
    S31A,    32,    //   (479) SPI3 BAR1
    S31L,    32,    //   (483) SPI3 BAR1 Length
    SD1A,    32,    //   (487) SDCard BAR1
    SD1L,    32,    //   (491) SDCard BAR1 Length
    SI1A,    32,    //   (495) SDIO BAR1
    SI1L,    32,    //   (499) SDIO BAR1 Length
    SP1A,    32,    //   (503) SPI BAR1
    SP1L,    32,    //   (507) SPI BAR1 Length
    SPST,    8,     //   (511) [SataPortState] SATA port state, Bit0 - Port0, Bit1 - Port1, Bit2 - Port2, Bit3 - Port3
    U11A,    32,    //   (512) HSUART BAR1
    U11L,    32,    //   (516) HSUART BAR1 Length
    U21L,    32,    //   (520) HSUART2 BAR1 Length
    W381,    8,     //   (524) WPCN381U: only used by BXT-P
    ECON,    8,     //   (525) Embedded Controller Availability Flag.
    PB1E,    8,     //   (526) 10sec Power button support
                    //   Bit0: 10 sec P-button Enable/Disable
                    //   Bit1: Internal Flag
                    //   Bit2: Rotation Lock flag, 0:unlock, 1:lock
                    //   Bit3: Slate/Laptop Mode Flag, 0: Slate, 1: Laptop
                    //   Bit4: Undock / Dock Flag, 0: Undock, 1: Dock
                    //   Bit5: VBDL Flag. 0: VBDL is not called, 1: VBDL is called, Virtual Button Driver is loaded.
                    //   Bit6: Reserved for future use.
                    //   Bit7: EC 10sec PB Override state for S3/S4 wake up.
    DBGS,    8,     //   (527) Debug State
    IPUA,    32,    //   (528) IPU Base Address
    BNUM,    8,     //   (532) Number of batteries
    B0SC,    8,     //   (533) Battery 0 Stored Capacity
    ECR1,    8,     //   (534) PciDelayOptimizationEcr
    HVCO,    8,     //   (535) HPLL VCO
    UBCB,   32,     //   (536) USB Type C OpRegion base address
    SBTD,    8,     //   (540) SelectBtDevice
    //
    // DPPM Devices and trip points
    //
    DPAT,    32,    //   (541) DptfProcActiveTemperature
        ,    8,     //
        ,    8,     //
        ,    8,     //
        ,    8,     //
        ,    8,     //
    Offset(550),
    S1DE,    8,     //   (550) EnableSen1Participant
    S1AT,    8,     //   (551) ActiveThermalTripPointSen1
    S1PT,    8,     //   (552) PassiveThermalTripPointSen1
    S1CT,    8,     //   (553) CriticalThermalTripPointSen1
    SSP1,    8,     //   (554) SensorSamplingPeriodSen1
    //
    // DPPM Policies
    //
    DPAP,    8,     //   (555) EnableActivePolicy
    DPPP,    8,     //   (556) EnablePassivePolicy
    DPCP,    8,     //   (557) EnableCriticalPolicy
        ,    8,     //   (558) DPTF Reserved

    TC1V,    8,     //   (559) Passive Trip Point TC1 Value
    TC2V,    8,     //   (560) Passive Trip Point TC2 Value
    TSPV,    8,     //   (561) Passive Trip Point TSP Value
    DTSE,    8,     //   (562) Digital Thermal Sensor Enable
        ,    8,     //   (563) Dptf Reserved
    IGDS,    8,     //   (564) IGD State
    HPME,    8,     //   (565) Enable/Disable HighPerformance mode for Dptf
    WWEN,    8,     //   (566) WWAN Enable
    //
    // Offset (567) to (572) used by BXT-M DPTF
    //
    Offset(573),
    PASL,    8,     //   (573) Panel AOB  0 - Disable, 1 - TIANMA , 2 - TRULY Fab B TypeC, 3 - TRULY Fab B, 4 -TRULY Fab B Command Mode, 5 - TRULY Fab B Command Mode TypeC
    IRMC,    8,     //   (574) IRMT CONFIGURATION
    CPUS,    8,     //   (575) ThermalSamplingPeriodTCPU
    STEP,    8,     //   (576) BXT Stepping ID
    RVD1,    8,     //   (577) Reserved
    LTR1,    8,     //   (578) Latency Tolerance Reporting Enable
    LTR2,    8,     //   (579) Latency Tolerance Reporting Enable
    LTR3,    8,     //   (580) Latency Tolerance Reporting Enable
    LTR4,    8,     //   (581) Latency Tolerance Reporting Enable
    LTR5,    8,     //   (582) Latency Tolerance Reporting Enable
    LTR6,    8,     //   (583) Latency Tolerance Reporting Enable
    OBF1,    8,     //   (584) Optimized Buffer Flush and Fill
    OBF2,    8,     //   (585) Optimized Buffer Flush and Fill
    OBF3,    8,     //   (586) Optimized Buffer Flush and Fill
    OBF4,    8,     //   (587) Optimized Buffer Flush and Fill
    OBF5,    8,     //   (588) Optimized Buffer Flush and Fill
    OBF6,    8,     //   (589) Optimized Buffer Flush and Fill
    RPA1,    32,    //   (590) Root Port address 1
    RPA2,    32,    //   (594) Root Port address 2
    RPA3,    32,    //   (598) Root Port address 3
    RPA4,    32,    //   (602) Root Port address 4
    RPA5,    32,    //   (606) Root Port address 5
    RPA6,    32,    //   (610) Root Port address 6
    PML1,    16,    //   (614) PCIE LTR max snoop Latency 1
    PML2,    16,    //   (616) PCIE LTR max snoop Latency 2
    PML3,    16,    //   (618) PCIE LTR max snoop Latency 3
    PML4,    16,    //   (620) PCIE LTR max snoop Latency 4
    PML5,    16,    //   (622) PCIE LTR max snoop Latency 5
    PML6,    16,    //   (624) PCIE LTR max snoop Latency 6
    PNL1,    16,    //   (626) PCIE LTR max no snoop Latency 1
    PNL2,    16,    //   (628) PCIE LTR max no snoop Latency 2
    PNL3,    16,    //   (630) PCIE LTR max no snoop Latency 3
    PNL4,    16,    //   (632) PCIE LTR max no snoop Latency 4
    PNL5,    16,    //   (634) PCIE LTR max no snoop Latency 5
    PNL6,    16,    //   (636) PCIE LTR max no snoop Latency 6
    TRTV,    8,     //   (638) TrtRevision
    WWPS,    32,    //   (639) WWAN PSV
    PWRE,    8,     //   (643) EnablePowerParticipant
    PBPE,    8,     //   (644) EnablePowerBossPolicy
    HGEN,    8,     //   (645) HG Enabled (0=Disabled, 1=Enabled)
    XBAS,   32,     //   (646) Any Device's PCIe Config Space Base Address
    DLPW,   16,     //   (650) Delay after Power Enable
    DLHR,   16,     //   (652) Delay after Hold Reset
    HRCO,   32,     //   (654) dGPU HLD RST GPIO Community Offset
    HRPO,   16,     //   (658) dGPU HLD RST GPIO Pin Offset
    HRAI,    8,     //   (660) dGPU HLD RST GPIO Active Information
    PECO,   32,     //   (661) dGPU PWR Enable GPIO Community Offset
    PEPO,   16,     //   (665) dGPU PWR Enable GPIO Pin Offset
    PEAI,    8,     //   (667) dGPU PWR Enable GPIO Active Information
    SCBN,    8,     //   (668) PCIe Endpoint Bus Number
    EECP,    8,     //   (669) PCIe Endpoint PCIe Capability Structure Offset
    RPBA,   32,     //   (670) dGPU Root Port Base Address
    NVGA,   32,     //   (674) NVIG opregion address
    NVHA,   32,     //   (678) NVHM opregion address
    AMDA,   32,     //   (682) AMDA opregion address
    GN1E,    8,     //   (686) EnableGen1Participant
    GN2E,    8,     //   (687) EnableGen2Participant
    GN3E,    8,     //   (688) EnableGen3Participant
    GN4E,    8,     //   (689) EnableGen4Participant
    G1AT,    8,     //   (690) ActiveThermalTripPointGen1
    G1PT,    8,     //   (691) PassiveThermalTripPointGen1
    G1CT,    8,     //   (692) CriticalThermalTripPointGen1
    G1HT,    8,     //   (693) HotThermalTripPointGen1
    G1C3,    8,     //   (694) CriticalThermalTripPointGen1S3
    TSP1,    8,     //   (695) ThermistorSamplingPeriodGen1
    G2AT,    8,     //   (696) ActiveThermalTripPointGen2
    G2PT,    8,     //   (697) PassiveThermalTripPointGen2
    G2CT,    8,     //   (698) CriticalThermalTripPointGen2
    G2HT,    8,     //   (699) HotThermalTripPointGen2
    G2C3,    8,     //   (700) CriticalThermalTripPointGen2S3
    TSP2,    8,     //   (701) ThermistorSamplingPeriodGen2
    G3AT,    8,     //   (702) ActiveThermalTripPointGen3
    G3PT,    8,     //   (703) PassiveThermalTripPointGen3
    G3CT,    8,     //   (704) CriticalThermalTripPointGen3
    G3HT,    8,     //   (705) HotThermalTripPointGen3
    G3C3,    8,     //   (706) CriticalThermalTripPointGen3S3
    TSP3,    8,     //   (707) ThermistorSamplingPeriodGen3
    G4AT,    8,     //   (708) ActiveThermalTripPointGen4
    G4PT,    8,     //   (709) PassiveThermalTripPointGen4
    G4CT,    8,     //   (710) CriticalThermalTripPointGen4
    G4HT,    8,     //   (711) HotThermalTripPointGen4
    G4C3,    8,     //   (712) CriticalThermalTripPointGen4S3
    TSP4,    8,     //   (713) ThermistorSamplingPeriodGen4
    DPC3,    32,    //   (714) DptfProcCriticalTemperatureS3
    DPHT,    32,    //   (718) DptfProcHotThermalTripPoint
    S1S3,    8,     //   (722) CriticalThermalTripPointSen1S3
    S1HT,    8,     //   (723) HotThermalTripPointSen1
    PCSP,    8,     //   (724) PMIC Stepping
    IOBF,    8,     //   (725) ScHdAudioIoBufferOwnership
    XDCE,    8,     //   (726) Xdci Enabled
    RSVD,    16,    //   (727) Reserved[2]
    VTKB,    8,     //   (729) Virtual keyboard Function 0- Disable 1- Discrete Touch 2- Integrated Touch
    //
    //WiGig for BXTM B0
    //
    WGEN,    8,     //   (730) WiGig Enable switch, for BXTM B0
    WGPL,    16,    //   (731) WiGig SPLC Power Limit
    WGTW,    32,    //   (733) WiGig SPLC Time Window

    PSME,    8,     //   (737) WiGig Power sharing manager enabling
    PSD0,    8,     //   (738) WiGig PSM SPLC0 Domain Type
    PSP0,    16,    //   (739) WiGig PSM SPLC0 Power Limit
    PST0,    32,    //   (741) WiGig PSM SPLC0 Time Window
    PSD1,    8,     //   (745) WiGig PSM SPLC1 Domain Type
    PSP1,    16,    //   (746) WiGig PSM SPLC1 Power Limit
    PST1,    32,    //   (748) WiGig PSM SPLC1 Time Window

    PDD0,    8,     //   (752) WiGig PSM DPLC0 Domain Type
    PDP0,    8,     //   (753) WiGig PSM DPLC0 Domain Preference
    PDI0,    8,     //   (754) WiGig PSM DPLC0 Power Limit Index
    PDL0,    16,    //   (755) WiGig PSM DPLC0 Power Limit
    PDT0,    32,    //   (757) WiGig PSM DPLC0 Time Window

    PDD1,    8,     //   (761) WiGig PSM DPLC1 Domain Type
    PDP1,    8,     //   (762) WiGig PSM DPLC1 Domain Preference
    PDI1,    8,     //   (763) WiGig PSM DPLC1 Power Limit Index
    PDL1,    16,    //   (764) WiGig PSM DPLC1 Power Limit
    PDT1,    32,    //   (766) WiGig PSM DPLC1 Time Window
    IS3A,    8,     //   (770) I2S audio codec device - INT343A
    ISC1,    8,     //   (771) I2S audio codec device - INT34C1
    NFCN,    8,     //   (772) I2C NFC device - NXP1001
    PSSI,    8,     //   (773) I2S PSS device - IMPJ0003
        ,    8,     //   (774) (not used)
    GSBC,    8,     //   (775) UART GPS device - BCM4752

    AG1L,    64,    //   (776)
    AG1H,    64,    //   (784)
    AG2L,    64,    //   (792)
    AG2H,    64,    //   (800)
    AG3L,    64,    //   (808)
    AG3H,    64,    //   (816)

    PPPR,    16,    //   (824) PowerParticipantPollingRate
    DCFE,    16,    //   (826) EnableDCFG (DPTF Configuration)

    ODV0,    8,     //   (828) DPTF OemDesignVariable0
    ODV1,    8,     //   (829) DPTF OemDesignVariable1
    ODV2,    8,     //   (830) DPTF OemDesignVariable2
    ODV3,    8,     //   (831) DPTF OemDesignVariable3
    ODV4,    8,     //   (832) DPTF OemDesignVariable4
    ODV5,    8,     //   (833) DPTF OemDesignVariable5

    VSP1,    8,     //   (834) EnableVS1Participant
    V1AT,    8,     //   (835) ActiveThermalTripPointVS1
    V1PV,    8,     //   (836) PassiveThermalTripPointVS1
    V1CR,    8,     //   (837) CriticalThermalTripPointVS1
    V1C3,    8,     //   (838) CriticalThermalTripPointVS1S3
    V1HT,    8,     //   (839) HotThermalTripPointVS1
    VSP2,    8,     //   (840) EnableVS2Participant
    V2AT,    8,     //   (841) ActiveThermalTripPointVS2
    V2PV,    8,     //   (842) PassiveThermalTripPointVS2
    V2CR,    8,     //   (843) CriticalThermalTripPointVS2
    V2C3,    8,     //   (844) CriticalThermalTripPointVS2S3
    V2HT,    8,     //   (845) HotThermalTripPointVS2
    VSP3,    8,     //   (846) EnableVS3Participant
    V3AT,    8,     //   (847) ActiveThermalTripPointVS3
    V3PV,    8,     //   (848) PassiveThermalTripPointVS3
    V3CR,    8,     //   (849) CriticalThermalTripPointVS3
    V3C3,    8,     //   (850) CriticalThermalTripPointVS3S3
    V3HT,    8,     //   (851) HotThermalTripPointVS3

    VSPE,    8,     //   (852) EnableVsPolicy
    SDS1,    8,     //   (853) SPI1 Fingerprint device - FPC1020/FPC1021
    NFCS,    8,     //   (854) NFC device select: 0: disabled; 1: NFC (IPT)/secure NFC; 2: NFC;
        ,   16,     //   (855) reserved
        ,   32,     //   (857) reserved
    PNSL,    8,     //   (861) Panel Selection (0=eDP, >=1 for MIPI)
    EEPI,    8,     //   (862) EPIEnable
    ETYC,    8,     //   (863) TypeCEnable
    WWPT,    8,     //   (864) PassiveThermalTripPointWWAN
    WWC3,    8,     //   (865) CriticalThermalTripPointWWANS3
    WWHT,    8,     //   (866) HotThermalTripPointWWAN
    WWCT,    8,     //   (867) CriticalThermalTripPointWWAN
    DPHL,    8,     //   (868) DisplayHighLimit
    DPLL,    8,     //   (869) DisplayLowLimit
    ODBG,    8,     //   (871) OsDbgEnable
    M32B,   32,     //   (874) PCIE MMIO resource base
    M32L,   32,     //   (878) PCIE MMIO resource length
    CROT,    8,     //   (883) Camera Sensor Rotation Angle
    TCPL,    8,     //   (884) I2c Touch Panel
    TCPD,    8,     //   (885) I2c Touch pad
    IC0S,    32,    //   (886) I2C0 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC1S,    32,    //   (890) I2C1 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC2S,    32,    //   (894) I2C2 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC3S,    32,    //   (898) I2C3 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC4S,    32,    //   (902) I2C4 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC5S,    32,    //   (906) I2C5 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC6S,    32,    //   (910) I2C6 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    IC7S,    32,    //   (914) I2C7 Speed - Standard mode/Fast mode/FastPlus mode/HighSpeed mode
    SEN2,    8,     //   (918) EnableSen2Participant
    PTTP,    8,     //   (919) PassiveThermalTripPointSen2
    CTTP,    8,     //   (920) CriticalThermalTripPointSen2S3
    HTTP,    8,     //   (921) HotThermalTripPointSen2
    CRTP,    8,     //   (922) CriticalThermalTripPointSen2
    SUCE,    8,     //   (923) SueCreekEnable: 0: disabled; 1: enabled
    ISCT,    8,     //   (924) TI3100 Audio Codec: 0:Disable; 1:Enable
}

