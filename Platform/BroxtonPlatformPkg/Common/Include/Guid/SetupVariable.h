/** @file
  Header file for Setup Variable.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SETUP_VARIABLE_H
#define _SETUP_VARIABLE_H

#include <ScLimits.h>

//
// {EC87D643-EBA4-4bb5-A1E5-3F3E36B20DA9}
//
#define SYSTEM_CONFIGURATION_GUID  { 0xec87d643, 0xeba4, 0x4bb5, {0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0xa9 }}

#define ROOT_SECURITY_GUID  { 0xd387d688, 0xeba4, 0x45b5, {0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0x37} }

//
// {6936B3BD-4350-46d9-8940-1FA20961AEB1}
//
#define SYSTEM_ROOT_MAIN_GUID  { 0x6936b3bd, 0x4350, 0x46d9, {0x89, 0x40, 0x1f, 0xa2, 0x9, 0x61, 0xae, 0xb1} }

//
// {21FEE8DB-0D29-477e-B5A9-96EB343BA99C}
//
#define ADDITIONAL_SYSTEM_INFO_GUID  {  0x21fee8db, 0xd29, 0x477e, {0xb5, 0xa9, 0x96, 0xeb, 0x34, 0x3b, 0xa9, 0x9c }}

//
// {EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9}
//
#define SETUP_GUID  { 0xEC87D643, 0xEBA4, 0x4BB5, {0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }}

//
// {1B838190-4625-4ead-ABC9-CD5E6AF18FE0}
//
#define EFI_HII_EXPORT_DATABASE_GUID { 0x1b838190, 0x4625, 0x4ead, {0xab, 0xc9, 0xcd, 0x5e, 0x6a, 0xf1, 0x8f, 0xe0} }

#define PASSWORD_MAX_SIZE            20
#define MAX_CUSTOM_VID_TABLE_STATES  6
#define OVERCLOCK_SOURCE_BIOS        0
#define OVERCLOCK_SOURCE_OS          1
#define  PCIE_MAX_ROOT_PORT          6
#define  LPSS_I2C_DEVICE_NUM         8
#define  LPSS_HSUART_DEVICE_NUM      4
#define  LPSS_SPI_DEVICE_NUM         3
#define HDAUDIO_FEATURES             10
#define HDAUDIO_PP_MODULES           32
#define GUID_CHARS_NUMBER            37 // 36 GUID chars + null termination

#define TPM2_SUPPORTED_BANK_NULL            0
#define TPM2_SUPPORTED_BANK_SHA1            1
#define TPM2_SUPPORTED_BANK_SHA2            2
#define TPM2_SUPPORTED_BANK_BOTH            3

#define PS2_CONSOLE                        0x00
#define RECONNECT_LAST_GOOD_INPUT_CONSOLE  0x01
#define WINDOWS_CONSOLE                    0x02

//
// #defines for Drive Presence
//
#define EFI_HDD_PRESENT       0x01
#define EFI_HDD_NOT_PRESENT   0x00
#define EFI_CD_PRESENT        0x02
#define EFI_CD_NOT_PRESENT    0x00

#define EFI_HDD_WARNING_ON    0x01
#define EFI_CD_WARNING_ON     0x02
#define EFI_SMART_WARNING_ON  0x04
#define EFI_HDD_WARNING_OFF   0x00
#define EFI_CD_WARNING_OFF    0x00
#define EFI_SMART_WARNING_OFF 0x00

#ifndef VFRCOMPILE
  extern EFI_GUID gEfiSetupVariableGuid;
#endif

#ifndef PLATFORM_SETUP_VARIABLE_NAME
  #define PLATFORM_SETUP_VARIABLE_NAME             L"Setup"
#endif
#define SETUP_DATA SYSTEM_CONFIGURATION


#pragma pack(push,1)

typedef struct {

  //
  // Floppy
  //
  UINT8         Floppy;
  UINT8         FloppyWriteProtect;

  //
  // System ports
  //
  UINT8         UsbLegacy;

  //
  // Keyboard
  //
  UINT8         Numlock;

  //
  // Power State
  //
  UINT8         PowerState;
  UINT8         PciDelayOptimizationEcr;

  //
  // Wake on RTC variables
  //
  UINT8         WakeOnRtcS5;
  UINT8         RTCWakeupDate;
  UINT8         RTCWakeupTimeHour;
  UINT8         RTCWakeupTimeMinute;
  UINT8         RTCWakeupTimeSecond;

  //
  // Wake On Lan
  //
  UINT8         WakeOnLanS5;

  //
  // Boot Order
  //
  UINT8         BootOrder[8];

  //
  // Hard Drive Boot Order
  //
  UINT8         HardDriveBootOrder[8];

  //
  // CD Drive Boot Order
  //
  UINT8         CdDriveBootOrder[4];

  //
  // FDD Drive Boot Order
  //
  UINT8         FddDriveBootOrder[4];

  //
  // Drive Boot Order
  //
  UINT8         DriveBootOrder[16];

  //
  // Boot Menu Type
  //
  UINT8         BootMenuType;

  //
  // Boot from Network
  //
  UINT8         BootNetwork;

  //
  // Boot USB
  //
  UINT8         BootUsb;

  //
  // Dummy place holder to prevent VFR compiler problem.
  //
  UINT16        DummyDataForVfrBug;  // Don't change or use.

  //
  // Fixed Disk Boot Sector (Fdbs)
  //
  UINT8         Fdbs;

  //
  // Event Logging
  //
  UINT8         EventLogging;

  //
  // Video Adaptor
  //
  UINT8         PrimaryVideoAdaptor;

  //
  // Hybrid Graphics
  //
  UINT8         HgCardSelect;
  UINT16        DelayAfterPwrEn;
  UINT16        DelayAfterHoldReset;

  //
  // Maximum FSB Automatic/Disable
  //
  UINT8         MaxFsb;

  //
  // Hard Disk Pre-delay
  //
  UINT8         HddPredelay;

  //
  // S.M.A.R.T. Mode
  //
  UINT8         SmartMode;

  //
  // ACPI Suspend State
  //
  UINT8         AcpiSuspendState;

  //
  // PCI Latency Timer
  //
  UINT8         PciLatency;

  //
  // Processor (CPU)
  //

  //
  // EIST or GV3 setup option
  //
  UINT8         ProcessorEistEnable;

  //
  // C1E Enable
  //
  UINT8         ProcessorC1eEnable;

  //
  // Enabling CPU C-States of processor
  //
  UINT8         ProcessorCcxEnable;

  //
  // Package C-State Limit
  //
  UINT8         PackageCState;

  //
  // Enable/Disable NHM C3(ACPI C2) report to OS
  //
  UINT8         OSC2Report;

  //
  // Enable/Disable NHM C6(ACPI C3) report to OS
  //
  UINT8         C6Enable;

  //
  // Enable/Disable NHM C7(ACPI C3) report to OS
  //
  UINT8         C7Enable;

  //
  // EIST/PSD Function select option
  //
  UINT8         ProcessorEistPsdFunc;

  //
  // CPU Active Cores and SMT
  //
  UINT8         ActiveProcessorCores;
  //
  // Core0 is always Enable.
  //
  UINT8         Core0;
  //
  // Enable/Disable Individual core1
  //
  UINT8         Core1;
  //
  // Enable/Disable Individual core2
  //
  UINT8         Core2;
  //
  // Enable/Disable Individual core3
  //
  UINT8         Core3;
  //
  // Hyper Threading
  //
  UINT8         ProcessorHyperThreadingDisable;

  //
  // Enabling VMX
  //
  UINT8         ProcessorVmxEnable;

  //
  // Enabling VTD
  //
  UINT8         VTdEnable;

  //
  // Enabling BIST
  //
  UINT8         ProcessorBistEnable;

  //
  // Disabling XTPR
  //
  UINT8         ProcessorxTPRDisable;

  //
  // Enabling XE
  //
  UINT8         ProcessorXEEnable;

  //
  // Fast String
  //
  UINT8         FastStringEnable;

  //
  // Monitor/Mwait
  //
  UINT8         MonitorMwaitEnable;

  //
  // Machine Check
  //
  UINT8         MachineCheckEnable;

  //
  // Turbo mode
  //
  UINT8         TurboModeEnable;

  //
  // Enable Processor XAPIC
  //
  UINT8         ProcessorXapic;

  //
  // Select BSP
  //
  UINT8         BspSelection;

  //
  // Turbo-XE Mode Processor TDC Limit Override Enable
  //
  UINT8         ProcessorTDCLimitOverrideEnable;

  //
  // Turbo-XE Mode Processor TDC Limit
  //
  UINT16        ProcessorTDCLimit;

  //
  // Turbo-XE Mode Processor TDP Limit Override Enable
  //
  UINT8         ProcessorTDPLimitOverrideEnable;

  //
  // Turbo-XE Mode Processor TDP Limit
  //
  UINT16        ProcessorTDPLimit;

  //
  // Virtual wire A or B
  //
  UINT8         ProcessorVirtualWireMode;

  //
  // FSB Frequency Override in MHz
  //
  UINT16        FsbFrequency;

  //
  // Ecc  0/1 Disable/Enable if supported
  //
  UINT8         EccEnable;

  //
  // Memory
  //
  UINT16        ChannelHashMask;
  UINT16        SliceHashMask;
  UINT8         InterleavedMode;
  UINT8         ChannelsSlicesEnabled;
  UINT8         MinRefRate2xEnabled;
  UINT8         DualRankSupportEnabled;
  UINT8         DualRankSupportEnableLockHide;

  //
  // Port 80 decode 0/1 - PCI/LPC
  //
  UINT8         Port80Route;

  //
  // ECC Event Logging
  //
  UINT8         EccEventLogging;

  //
  // VT-d Option
  //
  UINT8         InterruptRemap;
  UINT8         Isoc;
  UINT8         ATS;

  //
  // IGD option
  //
  UINT8         GraphicsDriverMemorySize;

  //
  // IGD Aperture Size question
  //
  UINT8         IgdApertureSize;

  //
  // Boot Display Device
  //
  UINT8         BootDisplayDevice;

  //
  // Integrated Graphics Device
  //
  UINT8         Igd;

  //
  // Flash update sleep delay
  //
  UINT8         FlashSleepDelay;

  //
  // FSC system Variable
  //
  UINT8         IgdFlatPanel;
  UINT8         EnableC10;
  UINT8         FastBoot;
  UINT8         EfiNetworkSupport;
  UINT8         PxeRom;

  UINT8         Knob001;
  UINT8         EnableGv;
  UINT8         EnableCx;
  UINT8         EnableCxe;
  UINT8         EnableTm;
  UINT8         EnableProcHot;
  UINT8         BootPState;
  UINT8         FlexRatio;
  UINT8         FlexVid;
  UINT8         QuietBoot;

  //
  // Thermal Policy Values
  //
  UINT8         AutoThermalReporting;
  UINT8         EnableDigitalThermalSensor;
  UINT8         PassiveThermalTripPoint;
  UINT8         PassiveTc1Value;
  UINT8         PassiveTc2Value;
  UINT8         PassiveTspValue;
  UINT8         CriticalThermalTripPoint;
  UINT8         ActiveTripPoint;
  UINT8         DeepStandby;
  UINT8         AlsEnable;
  UINT8         IgdLcdIBia;
  UINT8         LogBootTime;

  //
  // EM-1 related
  //
  UINT16        IaAppsRun;
  UINT8         IaAppsCap;
  UINT8         CapOrVoltFlag;
  UINT8         BootOnInvalidBatt;
  UINT8         ScramblerSupport;
  UINT8         SecureBoot;
  UINT8         SecureBootCustomMode;
  UINT8         MaxPkgCState;
  UINT8         PanelScaling;
  UINT8         IgdLcdIGmchBlc;
  UINT8         SecEnable;
  UINT8         TPM;
  UINT8         TPMSupportedBanks;
  UINT8         TpmDetection;
  UINT8         PttSuppressCommandSend;  // For PTT Debug
  UINT8         SecFlashUpdate;
  UINT8         SecFirmwareUpdate;
  //
  //Image Processing Unit PCI Device Configuration
  //
  UINT8         IpuEn;
  UINT8         IpuAcpiMode;
  //
  // Passwords
  //
  UINT8         Recovery;
  UINT8         Suspend;

  //
  // South Cluster Area - START
  //
  //
  // Security options
  //
  UINT8         ScBiosLock;
  //
  // Miscellaneous options
  //
  UINT8         Hpet;
  UINT8         Cg8254;
  UINT8         EnableClockSpreadSpec;
  UINT8         StateAfterG3;
  UINT8         UartInterface;
  UINT8         Wol;

  //
  // SCC Configuration
  //
  UINT8         SccSdcardEnabled;
  UINT8         ScceMMCEnabled;
  UINT8         ScceMMCHostMaxSpeed;
  UINT8         SccSdioEnabled;
  UINT8         GPPLock;
  UINT8         GppLockOptionHide;
  //
  // LPSS Configuration
  //
  UINT8         LpssI2C0Enabled;
  UINT8         LpssI2C1Enabled;
  UINT8         LpssI2C2Enabled;
  UINT8         LpssI2C3Enabled;
  UINT8         LpssI2C4Enabled;
  UINT8         LpssI2C5Enabled;
  UINT8         LpssI2C6Enabled;
  UINT8         LpssI2C7Enabled;

  UINT8         LpssHsuart0Enabled;
  UINT8         LpssHsuart1Enabled;
  UINT8         LpssHsuart2Enabled;
  UINT8         LpssHsuart3Enabled;

  UINT8         LpssSpi0Enabled;
  UINT8         LpssSpi1Enabled;
  UINT8         LpssSpi2Enabled;

  UINT8         LpssNFCSelect;

  UINT8         LpssI2cClkGateCfg[LPSS_I2C_DEVICE_NUM];
  UINT8         LpssHsuartClkGateCfg[LPSS_HSUART_DEVICE_NUM];
  UINT8         LpssSpiClkGateCfg[LPSS_SPI_DEVICE_NUM];
  UINT8         S0ixEnable;

  //
  // Usb Config
  //
  UINT8         UsbXhciSupport;
  UINT8         ScUsb30Mode;
  UINT8         ScUsb30Streams;
  UINT8         ScUsb20;
  UINT8         ScUsbPortDisable;
  UINT8         PortUsb20[SC_MAX_USB2_PORTS];
  UINT8         ScUsbOtg;
  UINT8         ScUsbVbusOn;       //OTG VBUS control
  UINT8         DisableComplianceMode;
  UINT8         IddigComp;
  //
  //Hsic and SSic Config
  //
  UINT8         Hsic1Support;
  UINT8         Ssic1Support;
  UINT8         Ssic2Support;
  UINT8         Ssic1Rate;
  UINT8         Ssic2Rate;
  UINT8         SsicDlanePg;
  //
  // Lan Config
  //
  UINT8         Lan;
  UINT8         SlpLanLowDc;

  //
  // HD-Audio Config
  //
  UINT8         ScHdAudio;
  UINT8         ScHdAudioDsp;
  UINT8         ScHdAudioCtlPwrGate;  // Deprecated option
  UINT8         ScHdAudioDspPwrGate;  // Deprecated option
  UINT8         ScHdAudioMmt;
  UINT8         ScHdAudioHmt;
  UINT8         ScHdAudioIoBufferOwnership;
  UINT8         ScHdAudioBiosCfgLockDown;
  UINT8         ScHdAudioPwrGate;
  UINT8         ScHdAudioClkGate;
  UINT8         ScHdAudioPme;
  UINT8         ScHdAudioFeature[HDAUDIO_FEATURES];
  UINT8         ScHdAudioPostProcessingMod[HDAUDIO_PP_MODULES];
  CHAR16        ScHdAudioPostProcessingModCustomGuid1[GUID_CHARS_NUMBER];
  CHAR16        ScHdAudioPostProcessingModCustomGuid2[GUID_CHARS_NUMBER];
  CHAR16        ScHdAudioPostProcessingModCustomGuid3[GUID_CHARS_NUMBER];
  UINT8         ScHdAudioNhltEndpointDmic;
  UINT8         ScHdAudioNhltEndpointBt;
  UINT8         ScHdAudioNhltEndpointI2sSKP;
  UINT8         ScHdAudioNhltEndpointI2sHP;
  UINT8         ScHdAduioRsvd1;
  UINT8         SvHdaVcType;
  UINT8         HdAudioLinkFrequency;
  UINT8         IDispLinkFrequency;
  UINT8         IDispLinkTmode;

  //
  // GMM Config
  //
  UINT8         Gmm;
  UINT8         GmmCgPGCBEnabled;
  UINT8         GmmCgSBDEnabled;
  UINT8         GmmCgSBTEnabled;
  UINT8         GmmCgSBPEnabled;
  UINT8         GmmCgCoreEnabled;
  UINT8         GmmCgDmaEnabled;
  UINT8         GmmCgRAEnabled;
  UINT8         GmmCgHostEnabled;
  UINT8         GmmCgPEnabled;
  UINT8         GmmCgTEnabled;
  UINT8         GmmPgHwAutoEnabled;
  UINT8         GmmPgD3HotEnabled;
  UINT8         GmmPgI3Enabled;
  UINT8         GmmPgPMCREnabled;

  //
  // ISH Config
  //
  UINT8         ScIshEnabled;
  UINT8         IshI2c0PullUp;
  UINT8         IshI2c1PullUp;

  //
  // SATA_CONFIG
  //
  UINT8         Sata;
  UINT8         SataTestMode;
  UINT8         SataInterfaceMode;
  UINT8         SataPort[SC_MAX_SATA_PORTS];
  UINT8         SataHotPlug[SC_MAX_SATA_PORTS];
  UINT8         SataMechanicalSw[SC_MAX_SATA_PORTS];
  UINT8         SataSpinUp[SC_MAX_SATA_PORTS];
  UINT8         SataExternal[SC_MAX_SATA_PORTS];
  UINT8         SataType[SC_MAX_SATA_PORTS];
  UINT8         SataSalp;
  UINT8         PxDevSlp[SC_MAX_SATA_PORTS];
  UINT8         EnableDitoConfig[SC_MAX_SATA_PORTS];
  UINT16        DitoVal[SC_MAX_SATA_PORTS];
  UINT8         DmVal[SC_MAX_SATA_PORTS];

  //
  // PCI_EXPRESS_CONFIG, 6 ROOT PORTS
  //
  UINT8   PcieClockGatingDisabled;
  UINT8   PcieRootPort8xhDecode;
  UINT8   Pcie8xhDecodePortIndex;
  UINT8   PcieRootPortPeerMemoryWriteEnable;
  UINT8   PcieComplianceMode;
  UINT8   PcieRootPortEn[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortAspm[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortURE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortFEE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortNFE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortCEE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortCTD[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortPIE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortSFE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortSNE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortSCE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortPMCE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortHPE[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortSpeed[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortTHS[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortL1SubStates[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieRootPortACS[SC_MAX_PCIE_ROOT_PORTS];
  //
  // PCIe LTR Configuration
  //
  UINT8   PchPcieLtrEnable[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PchPcieLtrConfigLock[SC_MAX_PCIE_ROOT_PORTS];
  UINT16  PcieLtrMaxSnoopLatency[SC_MAX_PCIE_ROOT_PORTS];
  UINT16  PcieLtrMaxNoSnoopLatency[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PchPcieSnoopLatencyOverrideMode[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PchPcieSnoopLatencyOverrideMultiplier[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PchPcieNonSnoopLatencyOverrideMode[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PchPcieNonSnoopLatencyOverrideMultiplier[SC_MAX_PCIE_ROOT_PORTS];
  UINT16  PchPcieSnoopLatencyOverrideValue[SC_MAX_PCIE_ROOT_PORTS];
  UINT16  PchPcieNonSnoopLatencyOverrideValue[SC_MAX_PCIE_ROOT_PORTS];

  //
  // PCI Bridge Resources
  //
  UINT8   PcieExtraBusRsvd[SC_MAX_PCIE_ROOT_PORTS];
  UINT16  PcieMemRsvd[SC_MAX_PCIE_ROOT_PORTS];
  UINT8   PcieIoRsvd[SC_MAX_PCIE_ROOT_PORTS];

  //
  // South Cluster Area - END
  //

  UINT8         GTTSize;
  UINT8         PeiGraphicsPeimInit;
  //
  // DVMT5.0 Graphic memory setting
  //
  UINT8         IgdDvmt50PreAlloc;
  UINT8         IgdDvmt50TotalAlloc;
  UINT8         GOPEnable;
  UINT8         GOPBrightnessLevel;                     //Gop Brightness level
  UINT8         VbtSelect;
  //
  // Gt Config
  //
  UINT8         PmSupport;
  UINT8         EnableRenderStandby;
  UINT8         PavpEnable;
  UINT8         CdClock;

  UINT8         SeCOpEnable;
  UINT8         SeCModeEnable;
  UINT8         SeCEOPEnable;
  UINT8         SeCEOPDone;
  UINT8         SeCLockDir;

  //
  // DPTF
  //
  UINT8         EnableDptf;                              // Option to enable/disable DPTF
  UINT16        EnableDCFG;                              // Option to enable/disable DPTF Configuration
  UINT8         DptfProcessor;                           // Enable processor participant
  UINT16        DptfProcCriticalTemperature;             // Processor critical temperature
  UINT16        DptfProcPassiveTemperature;              // Processor passive temperature
  UINT16        DptfProcActiveTemperature;               // Processor active temperature
  UINT16        DptfProcCriticalTemperatureS3;           // Processor warm temperature
  UINT16        DptfProcHotThermalTripPoint;             // Processor hot temperature
  UINT16        GenericCriticalTemp0;                    // Critical temperature value for generic sensor0 participant
  UINT16        GenericPassiveTemp0;                     // Passive temperature value for generic sensor0 participant
  UINT16        GenericCriticalTemp1;                    // Critical temperature value for generic sensor1 participant
  UINT16        GenericPassiveTemp1;                     // Passive temperature value for generic sensor1 participant
  UINT16        GenericCriticalTemp2;                    // Critical temperature value for generic sensor2 participant
  UINT16        GenericPassiveTemp2;                     // Passive temperature value for generic sensor2 participant
  UINT16        GenericCriticalTemp3;                    // Critical temperature value for generic sensor3 participant
  UINT16        GenericPassiveTemp3;                     // Passive temperature value for generic sensor3 participant
  UINT16        GenericCriticalTemp4;                    // Critical temperature value for generic sensor3 participant
  UINT16        GenericPassiveTemp4;                     // Passive temperature value for generic sensor3 participant
  UINT8         ThermalSamplingPeriodTCPU;               //
  UINT8         Clpm;                                    // Current low power mode
  UINT32        LPOEnable;                               // DPTF: Instructs the policy to use Active Cores if they are available. If this option is set to 0, then policy does not use any active core controls ?even if they are available
  UINT32        LPOStartPState;                          // DPTF: Instructs the policy when to initiate Active Core control if enabled. Returns P state index.
  UINT32        LPOStepSize;                             // DPTF: Instructs the policy to take away logical processors in the specified percentage steps
  UINT32        LPOPowerControlSetting;                  // DPTF: Instructs the policy whether to use Core offliing or SMT offlining if Active core control is enabled to be used in P0 or when power control is applied. 1 ?SMT Off lining 2- Core Off lining
  UINT32        LPOPerformanceControlSetting;            // DPTF: Instructs the policy whether to use Core offliing or SMT offlining if Active core control is enabled to be used in P1 or when performance control is applied.1 ?SMT Off lining 2- Core Off lining
  UINT8         EnableDppm;                              // DPTF: Controls DPPM Policies (enabled/disabled)

  UINT8         EnableSen0Participant;
  UINT8         PassiveThermalTripPointSen0;
  UINT8         CriticalThermalTripPointSen0;
  UINT8         CriticalThermalTripPointSen0S3;
  UINT8         HotThermalTripPointSen0;

  UINT8         EnableSen3Participant;
  UINT8         PassiveThermalTripPointSen3;
  UINT8         CriticalThermalTripPointSen3;
  UINT8         CriticalThermalTripPointSen3S3;
  UINT8         HotThermalTripPointSen3;

  UINT8         DptfChargerDevice;
  UINT8         DptfDisplayDevice;
  UINT8         DisplayHighLimit;
  UINT8         DisplayLowLimit;
  UINT8         EnablePowerParticipant;
  UINT16        PowerParticipantPollingRate;
  UINT8         DptfFanDevice;
  UINT8         DptfWwanDevice;
  UINT8         PassiveThermalTripPointWWAN;
  UINT8         CriticalThermalTripPointWWANS3;
  UINT8         HotThermalTripPointWWAN;
  UINT8         CriticalThermalTripPointWWAN;
  UINT8         HighPerfMode;
  UINT8         PmicEnable;
  UINT8         APEIBERT;
  UINT8         ACPIMemDbg;
  UINT8         BatteryChargingSolution;                 //0-non ULPMC 1-ULPMC
  UINT8         EnableActivePolicy;
  UINT8         EnablePassivePolicy;
  UINT8         TrtRevision;
  UINT8         EnableCriticalPolicy;
  UINT8         EnableAPPolicy;
  UINT8         EnablePowerBossPolicy;
  UINT8         EnableVSPolicy;

  UINT8         EnableMemoryDevice;
  UINT8         ActiveThermalTripPointTMEM;
  UINT8         PassiveThermalTripPointTMEM;
  UINT8         CriticalThermalTripPointTMEM;
  UINT8         ThermalSamplingPeriodTMEM;

  UINT8         EnableSen1Participant;
  UINT8         ActiveThermalTripPointSen1;
  UINT8         PassiveThermalTripPointSen1;
  UINT8         CriticalThermalTripPointSen1;
  UINT8         CriticalThermalTripPointSen1S3;
  UINT8         HotThermalTripPointSen1;
  UINT8         SensorSamplingPeriodSen1;

  UINT8         EnableGen1Participant;
  UINT8         ActiveThermalTripPointGen1;
  UINT8         PassiveThermalTripPointGen1;
  UINT8         CriticalThermalTripPointGen1;
  UINT8         CriticalThermalTripPointGen1S3;
  UINT8         HotThermalTripPointGen1;
  UINT8         ThermistorSamplingPeriodGen1;

  UINT8         EnableGen2Participant;
  UINT8         ActiveThermalTripPointGen2;
  UINT8         PassiveThermalTripPointGen2;
  UINT8         CriticalThermalTripPointGen2;
  UINT8         CriticalThermalTripPointGen2S3;
  UINT8         HotThermalTripPointGen2;
  UINT8         ThermistorSamplingPeriodGen2;

  UINT8         EnableGen3Participant;
  UINT8         ActiveThermalTripPointGen3;
  UINT8         PassiveThermalTripPointGen3;
  UINT8         CriticalThermalTripPointGen3;
  UINT8         CriticalThermalTripPointGen3S3;
  UINT8         HotThermalTripPointGen3;
  UINT8         ThermistorSamplingPeriodGen3;

  UINT8         EnableGen4Participant;
  UINT8         ActiveThermalTripPointGen4;
  UINT8         PassiveThermalTripPointGen4;
  UINT8         CriticalThermalTripPointGen4;
  UINT8         CriticalThermalTripPointGen4S3;
  UINT8         HotThermalTripPointGen4;
  UINT8         ThermistorSamplingPeriodGen4;

  UINT8         OemDesignVariable0;
  UINT8         OemDesignVariable1;
  UINT8         OemDesignVariable2;
  UINT8         OemDesignVariable3;
  UINT8         OemDesignVariable4;
  UINT8         OemDesignVariable5;

  UINT8         EnableVS1Participant;
  UINT8         ActiveThermalTripPointVS1;
  UINT8         PassiveThermalTripPointVS1;
  UINT8         CriticalThermalTripPointVS1;
  UINT8         CriticalThermalTripPointVS1S3;
  UINT8         HotThermalTripPointVS1;

  UINT8         EnableVS2Participant;
  UINT8         ActiveThermalTripPointVS2;
  UINT8         PassiveThermalTripPointVS2;
  UINT8         CriticalThermalTripPointVS2;
  UINT8         CriticalThermalTripPointVS2S3;
  UINT8         HotThermalTripPointVS2;

  UINT8         EnableVS3Participant;
  UINT8         ActiveThermalTripPointVS3;
  UINT8         PassiveThermalTripPointVS3;
  UINT8         CriticalThermalTripPointVS3;
  UINT8         CriticalThermalTripPointVS3S3;
  UINT8         HotThermalTripPointVS3;

  UINT8         PnpSettings;
  UINT8         EhciDebug;
  UINT8         IrmtConfiguration;
  UINT8         CRIDSettings;
  //
  //Dnx/Fastboot enable for PO
  //
  UINT8         OSDnX;

#if _SVBIOS_
  //
  // SV-SSA
  //
#if  _SVSSABIOS_
  UINT16        MemorySize;
#endif
  //
  // SV-MRC
  //
#if  _SVMRCBIOS_
  UINT8         MrcDebugMsgLevel;
#endif

  //
  // For FW Debug Menu Options
  //
#if _SVSECBIOS_
  UINT8         EndOfPostEnabled;
  UINT8         HeciCommunication;
  UINT8         TdtAssertStolen;
#endif
#endif //end _SVBIOS_

  //
  //OEM1 table
  //
  UINT8         CStateAutoDemotion;
  UINT8         CStateUnDemotion;
  UINT8         PkgCStateDemotion;
  UINT8         PkgCStateUnDemotion;

  UINT8         ProcTraceMemSize;
  UINT8         ProcTraceEnable;
  UINT8         ProcTraceOutputScheme;

  // TDO
  UINT8         TDO;

  //
  // Kernel Debugger (WinDBG)
  //
  UINT8         OsDbgEnable;

#ifdef PRAM_SUPPORT
  UINT8         Pram;
#endif

  //
  // GNSS/GPS mode selection
  //
  UINT8         GpsModeSel;
  UINT8         PreOsSelection;
  //
  //Trunk clock enable
  //
  UINT8         TrunkClockEnable;
  UINT8         MaxCoreCState;
  //
  //Multi-BOM
  //
  UINT8         BomSelection;
  UINT8         PanelSel;
  UINT8         WorldCameraSel;
  UINT8         UserCameraSel;
  UINT8         AudioSel;
  UINT8         ModemSel; // Modem selection: 0: Disabled, 1: 7260; 2: 7360;
  UINT8         TouchSel; // Touch selection: 0: Disabled, 1: Atmel mXT1066T2; 2: Atmel mXT1668T2
  UINT8         WifiSel;  // Wi-Fi Device Select 0: SDIO Lightning Peak 1: SDIO Broadcom 2. PCIe Lightning Peak

  UINT8         FirmwareConfiguration;
  //
  // ACPI
  //
  UINT8         PciExpNative;
  UINT8         NativeAspmEnable;
  UINT8         LowPowerS0Idle;
  UINT8         TenSecondPowerButtonEnable;
  UINT8         EcLowPowerMode;
  UINT8         CSNotifyEC;
  UINT8         CSDebugLightEC;
  //
  // RTD3
  //
  UINT8         Rtd3Support;
  UINT8         Rtd3P0dl;
  UINT8         Rtd3P3dl;
  UINT8         RTD3UsbPt1;
  UINT8         RTD3UsbPt2;
  UINT8         RTD3I2C0SensorHub;
  UINT16        RTD3AudioDelay;
  UINT16        RTD3TouchPadDelay;
  UINT16        RTD3TouchPanelDelay;
  UINT16        RTD3SensorHub;
  UINT16        VRStaggeringDelay;
  UINT16        VRRampUpDelay;
  UINT8         PstateCapping;
  UINT8         RTD3ZPODD;
  UINT8         RTD3Camera;
  UINT8         RTD3SataPort1;
  UINT8         RTD3SataPort2;
  UINT8         RTD3SataPort3;
  UINT8         Rtd3WaGpio;
  UINT16        RTD3I2C0ControllerPS0Delay;
  UINT16        RTD3I2C1ControllerPS0Delay;
  UINT8         ConsolidatedPR;

  UINT8         DciEn;
  UINT8         DciAutoDetect;
  //
  // Secure NFC
  //
  UINT8         NfcSelect;
  UINT8         SelectBtDevice;

  UINT8         ResetSelect;
  UINT8         FprrEnable;
  //
  // WIGIG enable switch
  //
  UINT8         WiGigEnable;
  UINT16        WiGigSPLCPwrLimit;
  UINT32        WiGigSPLCTimeWindow;
  UINT16        RfemSPLCPwrLimit;
  UINT32        RfemSPLCTimeWindow;
  //
  //PSM
  //
  UINT8         PsmEnable;
  UINT8         PsmSPLC0DomainType;
  UINT16        PsmSPLC0PwrLimit;
  UINT32        PsmSPLC0TimeWindow;
  UINT8         PsmSPLC1DomainType;
  UINT16        PsmSPLC1PwrLimit;
  UINT32        PsmSPLC1TimeWindow;

  UINT8         PsmDPLC0DomainType;
  UINT8         PsmDPLC0DomainPerference;
  UINT8         PsmDPLC0PowerLimitIndex;
  UINT16        PsmDPLC0PwrLimit;
  UINT32        PsmDPLC0TimeWindow;

  UINT8         PsmDPLC1DomainType;
  UINT8         PsmDPLC1DomainPerference;
  UINT8         PsmDPLC1PowerLimitIndex;
  UINT16        PsmDPLC1PwrLimit;
  UINT32        PsmDPLC1TimeWindow;

  UINT8         VirtualKbEnable;

  UINT8         EPIEnable;
  UINT8         TypeCEnable;

  //
  //DDR and High Speed Serial IO SSC
  //
  UINT8         DDRSSCEnable;
  UINT8         DDRSSCSelection;
  UINT8         DDRCLKBending;
  UINT8         HSSIOSSCEnable;
  UINT8         HSSIOSSCSelection;
  //
  // LPSS devices
  //
  UINT8         I2s343A;
  UINT8         I2s34C1;
  UINT8         I2cNfc;
  UINT8         I2cPss;
  UINT8         UartBt;
  UINT8         UartGps;
  UINT8         Spi1SensorDevice;

  UINT32        Uart2KernelDebugBaseAddress;

  //
  // SMRR for SMM source level debug
  //
  UINT8         SMRREnable;

  //
  // Windows Offline Crash Dump
  //
#ifdef CRASHDUMP_SUPPORT
  UINT8         CrashDump;
#endif

  //
  // S3 Setupvairable
  //
  UINT64        AcpiVariableSetCompatibility;
  UINT32        BootPerformanceTablePointer;
  UINT32        S3PerformanceTablePointer;
  UINT8         CseBootDevice;                  // 0 - eMMC, 1- UFS, 2 - SPI

  //
  // Platform Default
  //
  UINT8         BoardId;
  UINT8         PlatformSettingEn;
  UINT8         Max2G;
  UINT8         PmicSetupDefault;
  UINT8         RTEn;               //Real Time

  //
  // CPU Power Limit
  //
  UINT8         PowerLimit1Enable;
  UINT8         PowerLimit1Clamp;
  UINT8         PowerLimit1Time;
  UINT8         PowerLimit1;

  //
  // IPC1 (PMI) device
  //
  UINT8         RtcLock;
  UINT8         UseProductKey;
  UINT8         PowerButterDebounceMode;
  UINT8         PcieRootPortSelectableDeemphasis[SC_MAX_PCIE_ROOT_PORTS];
  UINT8         ConInBehavior;
  //
  // Number Of Processors
  //
  UINT8         NumOfProcessors;
  UINT8         GpioLock;

  UINT8         CameraRotationAngle;
  UINT8         PortUsb30[SC_MAX_USB3_PORTS];

  //
  // Touch Panel and Pad setting
  //
  UINT8         I2cTouchPanel;
  UINT8         I2cTouchPad;

  //
  //I2C Speed
  //
  UINT8         I2C0Speed;
  UINT8         I2C1Speed;
  UINT8         I2C2Speed;
  UINT8         I2C3Speed;
  UINT8         I2C4Speed;
  UINT8         I2C5Speed;
  UINT8         I2C6Speed;
  UINT8         I2C7Speed;
  UINT8         TcoLock;

  UINT8         EnableSen2Participant;
  UINT8         PassiveThermalTripPointSen2;
  UINT8         CriticalThermalTripPointSen2;
  UINT8         CriticalThermalTripPointSen2S3;
  UINT8         HotThermalTripPointSen2;

} SYSTEM_CONFIGURATION;
#pragma pack(pop)

#endif // #ifndef _SETUP_VARIABLE

