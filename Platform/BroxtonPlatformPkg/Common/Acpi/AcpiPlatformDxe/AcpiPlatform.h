/** @file
  This is an implementation of the ACPI platform driver.  Requirements for
  this driver are defined in the Tiano ACPI External Product Specification,
  revision 0.3.6.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _ACPI_PLATFORM_H_
#define _ACPI_PLATFORM_H_

//
// Statements that include other header files
//
#include <FrameworkDxe.h>
#include <PiDxe.h>
#include <Protocol/PlatformCpu.h>
#include <Base.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PcdLib.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/MpService.h>
#include <Protocol/CpuIo.h>
#include <IndustryStandard/Acpi30.h>
#include <IndustryStandard/Acpi20.h>
#include <Library/HobLib.h>
#include "CMOSMap.h"

#include <Guid/SetupVariable.h>
#include <Protocol/GlobalNvsArea.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DpsdRsa1024AndSha256SignatureVerification.h>
#include <IndustryStandard/WatchdogActionTable.h>
#include <Protocol/AcpiTable.h>

//
// Global variables
//
extern EFI_GLOBAL_NVS_AREA_PROTOCOL  mGlobalNvsArea;

//
// Global for the CPU I/O Protocol that is consumed by this driver
//
extern EFI_CPU_IO2_PROTOCOL        *mCpuIo;

//
// ACPI table information used to initialize tables.
//
// SLP 2.0 equates
//
#define SLP20_MAGIC_NUMBER        0x00000001
#define SLP20_VERIFIED_INDICATOR  0x91827364
#define EFI_ACPI_OEM_REVISION     0x00000003
#define EFI_ACPI_CREATOR_ID       SIGNATURE_32 ('B', 'R', 'X', 'T')
#define EFI_ACPI_CREATOR_REVISION 0x0100000D

#define WPCN381U_CONFIG_INDEX     0x2E
#define WPCN381U_CONFIG_DATA      0x2F
#define WPCN381U_CHIP_ID          0xF4
#define WDCP376_CHIP_ID           0xF1

#define MOBILE_PLATFORM  1
#define DESKTOP_PLATFORM 2

#define DUAL_CORE_CONFIG 2

#define KBC_DATA_PORT             0x60
#define KBC_CMD_STS_PORT          0x64

//
// 8042 Command
//
#define READ_CMD_BYTE             0x20
#define WRITE_CMD_BYTE            0x60
#define DISABLE_AUX               0xa7
#define ENABLE_AUX                0xa8
#define DISABLE_KB                0xad
#define ENABLE_KB                 0xae
#define WRITE_AUX_DEV             0xd4
#define READ_ID_CMD               0xf2
#define ENABLE_CMD                0xf4
#define CMD_KB_STS                0x10
#define CMD_KB_DIS                0x10
#define CMD_AUX_STS               0x20
#define CMD_AUX_DIS               0x20

#define PS2_ACK                   0xfa
#define PS2_RESEND                0xfe

#define KBC_INPB                  0x02
#define KBC_OUTB                  0x01
#define KBC_AUXB                  0x20

#define TIMEOUT                   50000
#define BAT_TIMEOUT               500000

//
// +++++++++++++++++++++++++++++++++++++++++++++++++++
// ESS - GV3 Definitions
// +++++++++++++++++++++++++++++++++++++++++++++++++++
//
#pragma pack(1)

typedef struct {
  UINT32 PlatformId;
  UINT8  IoApicEnable;
  UINT8  NMEN;    // Node Manager Enable (aka NPTM)
  UINT8  TpmEnable      :1;
  UINT8  CStateEnable   :1;
  UINT8  OSC2Report     :1;
  UINT8  C6Enable       :1;
  UINT8  C7Enable       :1;
  UINT8  MonitorMwaitEnable :1;
  UINT8  PStateEnable   :1;
  UINT8  Rsvd           :1;
  UINT8  PSDT           :2;     // P-State Dependency Type
  UINT8  StandbyToS1    :1;
  UINT8  KBPresent      :1;
  UINT8  MousePresent   :1;
  UINT8  DualIohPresent :1;
  UINT8  DummyBits      :2;
  UINT32 CpuIdInfo; // Used in PciCrs.asl to determine Havendale/Lynnfield Processor
  UINT32 Tolm;      // Used in PciCrs.asl
  UINT16 OsSelect;              // O/S Type. This is for ASL only.
  UINT32 IgdOpRegionAddress;    // IGD OpRegion Starting Address
  UINT8  IgdBootType;           // IGD Boot Display Device
  UINT8  IgdPanelType;          // IGD Panel Type CMOs option
  UINT8  IgdTvFormat;           // IGD TV Format CMOS option
  UINT8  IgdTvMinor;            // IGD TV Minor Format CMOS option
  UINT8  IgdPanelScaling;       // IGD Panel Scaling
  UINT8  IgdBlcConfig;          // IGD BLC Configuration
  UINT8  IgdBiaConfig;          // IGD BIA Configuration
  UINT8  IgdSscConfig;          // IGD SSC Configuration
  UINT8  IgdDvmtMemSize;        // IGD DVMT Memory Size
  UINT8  IgdFunc1Enable;        // IGD Function 1 Enable
  UINT8  IgdHpllVco;            // HPLL VCO
  UINT8  IgdSciSmiMode;         // GMCH SMI/SCI mode (0=SCI)
  UINT8  IgdPAVP;               // IGD PAVP data
} BIOS_ACPI_PARAM;

typedef struct _AML_GV3_STATE_PACKAGE {
  UINT8   PkgOp;                // 0x12
  UINT8   PkgLeadByte;
  UINT8   NoData;
  UINT8   DWPrefix0;
  UINT32  CoreFreq;
  UINT8   DWPrefix1;
  UINT32  PowerConsumption;
  UINT8   DWPrefix2;
  UINT32  TransactionLatency;
  UINT8   DWPrefix3;
  UINT32  BusMasterLatency;
  UINT8   DWPrefix4;
  UINT32  ControlValue;
  UINT8   DWPrefix5;
  UINT32  StatusValue;
} AML_GV3_STATE_PACKAGE;

typedef struct _AML_PSS_PACKAGE {
  UINT8                 NameOp; // 0x08
  UINT8                 Sign[4];
  UINT8                 PkgOp;  // 0x12
  UINT8                 Res1[2];
  UINT8                 NoOfGV3Packages;
  AML_GV3_STATE_PACKAGE Pstates[16];
} AML_PSS_PACKAGE;

typedef struct _SMM_APICID_MAP {
  UINT32  ApicID;
  UINT16  PhysicalCpuNumber;
  UINT16  CpuNumber;
  UINT16  Version;
} SMM_APICID_MAP;

typedef struct _SMM_GV3_FVID {
  UINT16  VidValue : 8;
  UINT16  FreqRatio : 8;
  UINT16  SystemBusFreq : 16;
} SMM_GV3_FVID;

//
// APCI Watchdog Action Table (WDAT)
//
#define WATCHDOG_ACTION_INSTRUCTION_ENTRY_MAX_NUMBER 0x0E

typedef struct {
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE                                Table;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY    Entry[WATCHDOG_ACTION_INSTRUCTION_ENTRY_MAX_NUMBER];
} EFI_ACPI_WATCHDOG_ACTION_TABLE;

#pragma pack()

#define GV3_SSDT_OEM_TABLE_IDBASE 0x4000

//
// Private Driver Data
//
//
// Define Union of IO APIC & Local APIC structure;
//
typedef union {
  EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC_STRUCTURE AcpiLocalApic;
  EFI_ACPI_2_0_IO_APIC_STRUCTURE              AcpiIoApic;
  struct {
    UINT8 Type;
    UINT8 Length;
  } AcpiApicCommon;
} ACPI_APIC_STRUCTURE_PTR;

//
// Protocol private structure definition
//
/**
  Entry point of the ACPI platform driver.

  @param[in] ImageHandle           A handle for the image that is initializing this driver.
  @param[in] SystemTable           A pointer to the EFI system table.

  @retval EFI_SUCCESS              Driver initialized successfully.
  @retval EFI_LOAD_ERROR           Failed to Initialize or has been loaded.
  @retval EFI_OUT_OF_RESOURCES     Could not allocate needed resources.

**/
EFI_STATUS
InstallAcpiPlatform (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

/**
  Get Acpi Table Version.

**/
EFI_ACPI_TABLE_VERSION
GetAcpiTableVersion (
  VOID
  );

/**
  The function returns OEM specific information of Acpi Platform.

  @param[out] OemId           OemId returned.
  @param[out] OemTableId      OemTableId returned.
  @param[out] OemRevision     OemRevision returned.

  @retval     EFI_STATUS      Status of function execution.

**/
EFI_STATUS
AcpiPlatformGetOemFields (
  OUT UINT8   *OemId,
  OUT UINT64  *OemTableId,
  OUT UINT32  *OemRevision
  );

/**
  The function returns Acpi Setting.

  @retval        Acpi table version encoded as a UINT32.

**/
EFI_ACPI_TABLE_VERSION
AcpiPlatformGetAcpiSetting (
  VOID
  );

/**
  Entry point for Acpi platform driver.

  @param[in]  ImageHandle              A handle for the image that is initializing this driver.
  @param[in]  SystemTable              A pointer to the EFI system table.

  @retval     EFI_SUCCESS              Driver initialized successfully.
  @retval     EFI_LOAD_ERROR           Failed to Initialize or has been loaded.
  @retval     EFI_OUT_OF_RESOURCES     Could not allocate needed resources.

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
IrmtDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif

