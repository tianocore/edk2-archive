/** @file
  This header file contains the processor power management definitions.

  Acronyms:
    PPM   Processor Power Management
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POWER_MGMT_COMMON_H_
#define _POWER_MGMT_COMMON_H_

#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/MpService.h>
#include <Protocol/FirmwareVolume2.h>
#include <ScAccess.h>
#include <SaAccess.h>
#include <CpuAccess.h>
#include <Protocol/CpuGlobalNvsArea.h>
#include <Private/Protocol/PowerMgmtInitDone.h>
#include <Library/CpuPlatformLib.h>
#include <Private/PowerMgmtNvsStruct.h>
#include <Library/AslUpdateLib.h>
#include <Library/MmPciLib.h>
#include <Private/CpuInitDataHob.h>
#include <Library/TimerLib.h>

#define PM_CST_LVL2                     0x14

#define FADT_C3_LATENCY                 57
#define FADT_C3_LATENCY_DISABLED        1001

#define NATIVE_PSTATE_LATENCY           10
#define PSTATE_BM_LATENCY               10
#define MP_TIMEOUT_FOR_STARTUP_ALL_APS  0 ///< Set 0 for BSP always wait for APs

///
/// Limit the number of P-states to 16. Up to Windows 7, the OS allocates 1KB buffer for the PSS package.
/// So the maximum number of P-state OS can handle is 19. This is not an OS issue. Having too many P-states
/// is not good for the system performance.
///
#define FVID_MAX_STATES                 20
#define FVID_MIN_STEP_SIZE              1

///
/// Cpu Brandstring length
///
#define MAXIMUM_CPU_BRAND_STRING_LENGTH       48
#define EFI_FIELD_OFFSET(TYPE,Field) ((UINTN)(&(((TYPE *) 0)->Field)))
#define EFI_IDIV_ROUND(r, s)  ((r) / (s) + (((2 * ((r) % (s))) < (s)) ? 0 : 1))

///
/// Global variables
///
extern EFI_GUID gCpuAcpiTableStorageGuid;

///
/// SiCpuPolicy Revision
///
extern UINT8 mCpuPolicyRevision;

extern POWER_MGMT_CONFIG      *mPowerMgmtConfig; ///< Power Managment policy configurations
extern EFI_CPUID_REGISTER     mCpuid01;          // CPUID 01 values
///
/// Values for FVID table calculate.
///
extern UINT16 mTurboBusRatio;
extern UINT16 mMaxBusRatio;
extern UINT16 mMinBusRatio;
extern UINT16 mProcessorFlavor;
extern UINT16 mBspBootRatio;
extern UINT16 mPackageTdp;
extern UINT16 mPackageTdpWatt;
extern UINT16 mCpuConfigTdpBootRatio;
extern UINT16 mCustomPowerUnit;
extern UINT16 mCpuCacheSize;
///
/// Fractional part of Processor Power Unit in Watts. (i.e. Unit is 1/mProcessorPowerUnit)
///
extern UINT8 mProcessorPowerUnit;
///
/// Fractional part of Processor Time Unit in seconds. (i.e Unit is 1/mProcessorTimeUnit)
///
extern UINT8 mProcessorTimeUnit;
///
/// Maximum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
extern UINT16 mPackageMaxPower;
///
/// Minimum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
extern UINT16 mPackageMinPower;
extern UINT8  mControllableTdpEnable;                        ///< Controllable TDP Enable/Disable
extern UINT8  mRatioLimitProgrammble;                        ///< Programmable Ratio Limit
extern UINT8  mTdpLimitProgrammble;                          ///< Programmable TDP Limit
extern CPU_GLOBAL_NVS_AREA_PROTOCOL *gCpuGlobalNvsAreaProtocol; ///< CPU GlobalNvs Protocol
extern EFI_MP_SERVICES_PROTOCOL *mMpService;                 ///< EFI MP SERVICES PROTOCOL

///
/// Globals to support updating ACPI Tables
///
extern EFI_ACPI_TABLE_PROTOCOL     *mAcpiTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0IstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApIstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0CstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApCstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpuSsdtTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0TstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApTstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCtdpTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0HwpTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApHwpTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mHwpLvtTable;

///
/// ASL SSDT structure layout
///
#pragma pack(1)
typedef struct {
  UINT8  NameOp;          ///< First opcode is a NameOp.
  UINT32 NameString;      ///< 'TDSS'   ; Name of object.
  UINT8  PackageOp;       ///< db 12h   ; Sixth OpCode is PackageOp.
  UINT16 PackageLen;      ///< dw 0h    ; Seventh/Eighth OpCode is PackageLength.
  UINT8  PackageEntryNum; ///< db 0Ch   ; Ninth OpCode is number of package entries.
  UINT8  StringPrefix1;   ///< 0Dh
  UINT64 Cpu0IstStr;      ///< 00h
  UINT8  StringNull1;     ///< 00h
  UINT8  DwordPrefix1a;   ///< 0Ch
  UINT32 Cpu0IstAddr;     ///< 00h
  UINT8  DwordPrefix1b;   ///< 0Ch
  UINT32 Cpu0IstLen;      ///< 00h
  UINT8  StringPrefix2;   ///< 0Dh
  UINT64 Cpu1IstStr;      ///< 00h
  UINT8  StringNull2;     ///< 00h
  UINT8  DwordPrefix2a;   ///< 0Ch
  UINT32 ApIstAddr;       ///< 00h
  UINT8  DwordPrefix2b;   ///< 0Ch
  UINT32 ApIstLen;        ///< 00h
  UINT8  StringPrefix3;   ///< 0Dh
  UINT64 Cpu0CstStr;      ///< 00h
  UINT8  StringNull3;     ///< 00h
  UINT8  DwordPrefix3a;   ///< 0Ch
  UINT32 Cpu0CstAddr;     ///< 00h
  UINT8  DwordPrefix3b;   ///< 0Ch
  UINT32 Cpu0CstLen;      ///< 00h
  UINT8  StringPrefix4;   ///< 0Dh
  UINT64 ApCstStr;        ///< 00h
  UINT8  StringNull4;     ///< 00h
  UINT8  DwordPrefix4a;   ///< 0Ch
  UINT32 ApCstAddr;       ///< 00h
  UINT8  DwordPrefix4b;   ///< 0Ch
  UINT32 ApCstLen;        ///< 00h
  UINT8  StringPrefix5;   ///< 0Dh
  UINT64 Cpu0HwpStr;      ///< 00h
  UINT8  StringNull5;     ///< 00h
  UINT8  DwordPrefix5a;   ///< 0Ch
  UINT32 Cpu0HwpAddr;     ///< 00h
  UINT8  DwordPrefix5b;   ///< 0Ch
  UINT32 Cpu0HwpLen;      ///< 00h
  UINT8  StringPrefix6;   ///< 0Dh
  UINT64 ApHwpStr;        ///< 00h
  UINT8  StringNull6;     ///< 00h
  UINT8  DwordPrefix6a;   ///< 0Ch
  UINT32 ApHwpAddr;       ///< 00h
  UINT8  DwordPrefix6b;   ///< 0Ch
  UINT32 ApHwpLen;        ///< 00h
  UINT8  StringPrefix7;   ///< 0Dh
  UINT64 HwpLvtStr;       ///< 00h
  UINT8  StringNull7;     ///< 00h
  UINT8  DwordPrefix7a;   ///< 0Ch
  UINT32 HwpLvtAddr;      ///< 00h
  UINT8  DwordPrefix7b;   ///< 0Ch
  UINT32 HwpLvtLen;       ///< 00h
} SSDT_LAYOUT;
#pragma pack()

//
// FVID Table Information
// Default FVID table
// One header field plus states
//
extern UINT16     mNumberOfStates;
extern FVID_TABLE *mFvidPointer;

//
// Function prototypes
//

/**
  Patch the native _PSS package with the EIST values
  Uses ratio/VID values from the FVID table to fix up the control values in the _PSS.

  (1) Find _PSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the _PSS AML name object.
  (2) Resize the _PSS package.
  (3) Fix up the _PSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.

  @retval EFI_SUCCESS     On success
  @retval EFI_NOT_FOUND   If _PR_.CPU0 scope is not foud in the ACPI tables

**/
EFI_STATUS
AcpiPatchPss (
  VOID
  );

/**
  Configure the FACP for C state support

**/
VOID
ConfigureFadtCStates (
  VOID
  );

/**
  Locate the CPU ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @retval EFI_SUCCESS - On success
  @retval EFI_NOT_FOUND - Required firmware volume not found
  @retval EFI_OUT_OF_RESOURCES - No enough resoruces (such as out of memory).
  @retval Appropiate failure code on error

**/
EFI_STATUS
InitCpuAcpiTable (
  VOID
  );

#endif

