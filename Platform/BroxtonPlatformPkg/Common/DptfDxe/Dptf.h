/** @file
  Header file for the Dptf driver.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DPTF_H
#define _DPTF_H

#include <Library/UefiLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSupport.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/GlobalNvsArea.h>
#include <Guid/SetupVariable.h>
#include <ScAccess.h>
#include <SaRegs.h>
#include <Library/ScPlatformLib.h>
#include <Library/MmPciLib.h>
#include <Library/SteppingLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PcdLib.h>

#if (ENBDT_PF_ENABLE == 0)
#include <Library/PmicLib.h>
#endif

#define DPTF_ACPI_CPU_TABLE_SIGNATURE SIGNATURE_64('C','p','u','D','p','t','f',0x0)
#define DPTF_ACPI_SOC_TABLE_SIGNATURE SIGNATURE_64('S','o','C','D','p','t','f',0x0)
#define DPTF_ACPI_LPM_TABLE_SIGNATURE SIGNATURE_64('L','o','w','P','w','r','M',0x0)
#define DPTF_ACPI_GEN_TABLE_SIGNATURE SIGNATURE_64('D','p','t','f','T','a','b',0x0)

#define DPTF_SHUTDOWN_DELAY (5) // 5 Seconds

#define A0_SYS_THERM0_BASE_ADDRESS  (0xFFFF9E00)
#define A0_SYS_THERM1_BASE_ADDRESS  (0xFFFF9E08)
#define A0_SYS_THERM2_BASE_ADDRESS  (0xFFFF9E10)
#define A0_SYS_THERM3_BASE_ADDRESS  (0xFFFF9E18)

#if (ENBDT_PF_ENABLE == 0)

#define PMIC_VENDOR_ROHM   0x1F
#define PMIC_VENDOR_DIALOG 0x0

#define PMIC_I2C_CONTROLLER_ID  6
#define PMIC_I2C_SLAVE_ADDR     0x6E


#define I2C_ACPI_ID_LEN 16   //I2C0X\\SFFFF\\400K
#define I2C_ADDRESS_MAX 0x3FE
#define I2C_TIMEOUT_DEFAULT 1000
#define I2C_READ_MAX (1024*1024*10+1)   //Defined by the tool itself for maximum read length.

#define DID_ACPI_ID_PREFIX   "I2C0"
#define DID_ACPI_ID_SUFFIX   "\\SFFFF"
#define DID_ACPI_ID_SUFFIX_400K "\\400K"


#define PMIC_ID0 0
#define PMIC_SYS0_THRMCRIT  0x98
#define PMIC_SYS1_THRMCRIT  0x9D
#define PMIC_SYS2_THRMCRIT  0xA2
#define PMIC_TS_CRIT_ENABLE 0x91
#endif

#define CHV_DTS_TJMAX      90

#pragma pack(1)

typedef struct _THERMISTOR_REG_DEF {
  UINT16 STR:10;  //Sensor Thermal Readout value, Raw ADC values of thermistor
  UINT16 RES1:5;
  UINT16 TSE:1;   // Thermal Sensor Enable
  UINT16 ATH0:10; // Aux0 Threshold. It is raw ADC value that is compared against STR to generate interrupts
  UINT16 ATD0:3;  // AUX0 Threshold hysterisis data.
  UINT16 ATE0:1;  // AUX0 Trip Enable, when set indicates SCU to generate virtual MSI when temperature in STR crosses programmed value in A0T field
  UINT16 ATS0:1;  // Aux0 Trip Status, When set, indicates that the temperature has crossed the AUX0 threshold set in Bits 9:0.
  UINT16 APE0:1;  // AUX0 Policy Enable, When set, instructs the SCU to trigger PROCHOT# behavior on SoC as long
                  //  as the temperature is above the A0T value in this register.
                  //    The Host (BIOS) typically sets this bit when the platform implements static fail safe behavior based on skin sensor thermal excursions.
  UINT16 ATH1:10; // AUX1 Threshold (ATH). RW to Host, RO to SCU. When set to a value, indicates the raw ADC value to be compared against the STR register to generate interrupts.
  UINT16 ATD1:3;  // AUX1 Threshold Hysteresis Delta (ATHD).
  UINT16 ATE1:1;  // AUX1 Trip Enable (ATE). RW to host, RO to SCU. When set, instructs the SCU to generate a virtual MSI when the temperature in STR register crosses the value programmed in A0T field of this register.
  UINT16 ATS1:1;  // AUX1 Trip Status (ATS). RW to host, RW to SCU. When set, indicates that the temperature has crossed the AUX0 threshold set in Bits 9:0.
  UINT16 RES2:1;
  UINT16 CTH:10;  //Critical Threshold (CTH). RW to Host, RO to SCU. When set to a value, indicates the raw ADC value to be compared against the STR register to perform fail safe critical shutdown.
  UINT16 RES3:4;
  UINT16 CTS:1;   //Critical Trip Status (CTS). RW to host, RW to SCU. When set, indicates that the temperature has crossed the Critical threshold set in Bits 9:0.
  UINT16 CPEN:1;  //Critical Policy Enable (CPEN).
} THERISTOR_REG_DEF;

typedef struct _DPTF_DRIVER_DATA {
  BOOLEAN           bDptfEnabled;
  UINT32            CpuParticipantCriticalTemperature;
  INT16             CpuParticipantPassiveTemperature;
  INT16             GenParticipant0CriticalTemperature;
  INT16             GenParticipant0PassiveTemperature;
  INT16             GenParticipant1CriticalTemperature;
  INT16             GenParticipant1PassiveTemperature;
  INT16             GenParticipant2CriticalTemperature;
  INT16             GenParticipant2PassiveTemperature;
  INT16             GenParticipant3CriticalTemperature;
  INT16             GenParticipant3PassiveTemperature;
  INT16             GenParticipant4CriticalTemperature;
  INT16             GenParticipant4PassiveTemperature;
  INT16             CriticalThreshold0ForScu;
  INT16             CriticalThreshold1ForScu;
  INT16             CriticalThreshold2ForScu;
  INT16             CriticalThreshold3ForScu;
  INT16             CriticalThreshold4ForScu;
  UINT32            LPOEnable;                      // DPTF: Instructs the policy to use Active Cores if they are available. If this option is set to 0, then policy does not use any active core controls even if they are available
  UINT32            LPOStartPState;                 // DPTF: Instructs the policy when to initiate Active Core control if enabled. Returns P state index.
  UINT32            LPOStepSize;                    // DPTF: Instructs the policy to take away logical processors in the specified percentage steps
  UINT32            LPOPowerControlSetting;         // DPTF: Instructs the policy whether to use Core offliing or SMT offlining if Active core control is enabled to be used in P0 or when power control is applied. 1- SMT Off lining 2- Core Off lining
  UINT32            LPOPerformanceControlSetting;   // DPTF: Instructs the policy whether to use Core offliing or SMT offlining if Active core control is enabled to be used in P1 or when performance control is applied.1- SMT Off lining 2- Core Off lining
  BOOLEAN           bDppmEnabled;
  UINT8             BidirectionalProchotEnable;
  UINT8             ThermalMonitoring;
  UINT8             ThermalMonitoringHot;
  UINT8             ThermalMonitoringSystherm0Hot;
  UINT8             ThermalMonitoringSystherm1Hot;
  UINT8             ThermalMonitoringSystherm2Hot;
  THERISTOR_REG_DEF *pSysTherm0BaseAddress;
  THERISTOR_REG_DEF *pSysTherm1BaseAddress;
  THERISTOR_REG_DEF *pSysTherm2BaseAddress;
  THERISTOR_REG_DEF *pSysTherm3BaseAddress;
} DPTF_DRIVER_DATA;

#pragma pack()

#define PCT_DEFAULT       90
#define PPT_DEFAULT       85
#define GCT0_DEFAULT      70
#define GPT0_DEFAULT      60
#define GCT1_DEFAULT      75
#define GPT1_DEFAULT      55
#define GCT2_DEFAULT      70
#define GPT2_DEFAULT      60
#define GCT3_DEFAULT      75
#define GPT3_DEFAULT      50
#define GCT4_DEFAULT      75
#define GPT4_DEFAULT      50
#define SYSTHERM0_DEFAULT 110
#define SYSTHERM1_DEFAULT 110
#define SYSTHERM2_DEFAULT 110

//
// Current Logical processor offlining (CLPO) Default parameters
//
#define CLPO_DEFAULT_ENABLE             (0x1)
#define CLPO_DEFAULT_START_PSTATE       (0x1)
#define CLPO_DEFAULT_STEP_SIZE          (25)
#define CLPO_DEFAULT_PWR_CTRL_SETTING   (0x1) // 1-SMT or 2-Core
#define CLPO_DEFAULT_PERF_CTRL_SETTING  (0x1) // 1-SMT or 2-Core
#define DPPM_ENABLE_DEFAULT             (0x1)
#define BI_DIRECTIONAL_PROCHOT_ENABLE   (0x0)
#define THERMAL_MOINTER                 (0x0)
#define TJTARGET_THRESHOLD              (0x3)

#endif //_DPTF_H

