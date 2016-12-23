/** @file
  Header file for MiscSubclass Driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MISC_SUBCLASS_DRIVER_H
#define _MISC_SUBCLASS_DRIVER_H


#include "CommonHeader.h"

extern UINT8  MiscSubclassStrings[];


#define T14_FVI_STRING          "Driver/firmware version"
#define EFI_SMBIOS_TYPE_FIRMWARE_VERSION_INFO 0x90
#define EFI_SMBIOS_TYPE_MISC_VERSION_INFO 0x94
#define EFI_SMBIOS_TYPE_SEC_INFO 0x83
#define IntelIdentifer 0x6F725076
//
// Data table entry update function.
//
typedef EFI_STATUS (EFIAPI EFI_MISC_SMBIOS_DATA_FUNCTION) (
  IN  VOID                 *RecordData,
  IN  EFI_SMBIOS_PROTOCOL  *Smbios
  );

//
// Data table entry definition.
//
typedef struct {
  //
  // intermediat input data for SMBIOS record
  //
  VOID                              *RecordData;
  EFI_MISC_SMBIOS_DATA_FUNCTION     *Function;
} EFI_MISC_SMBIOS_DATA_TABLE;

//
// Data Table extern definitions.
//
#define MISC_SMBIOS_TABLE_EXTERNS(NAME1, NAME2, NAME3) \
extern NAME1 NAME2 ## Data; \
extern EFI_MISC_SMBIOS_DATA_FUNCTION NAME3 ## Function


//
// Data Table entries
//
#define MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(NAME1, NAME2) \
{ \
  & NAME1 ## Data, \
  & NAME2 ## Function \
}

//
// Global definition macros.
//
#define MISC_SMBIOS_TABLE_DATA(NAME1, NAME2) \
  NAME1 NAME2 ## Data

#define MISC_SMBIOS_TABLE_FUNCTION(NAME2) \
  EFI_STATUS EFIAPI NAME2 ## Function( \
  IN  VOID                  *RecordData, \
  IN  EFI_SMBIOS_PROTOCOL   *Smbios \
  )

#pragma pack(1)
//
// This is definition for SMBIOS Oem data type 0x83
//
typedef struct {
  UINT32                            SecEnable                :1;
  UINT32                            Reserved0                :12;
  UINT32                            ATSupport                :1;
  UINT32                            Reserved1                :18;
  UINT32                            SecFwMinorVersion        :16;
  UINT32                            SecFwMajorVersion        :16;
  UINT32                            SecFwBuildVersion        :16;
  UINT32                            SecFwHotfixVersion       :16;
} SEC_CAPABILITYS;

typedef struct {
  UINT32                            Reserved0                :5;
  UINT32                            ATConfigured             :1;
  UINT32                            Reserved1                :26;
} SEC_CFG_STATE;

typedef struct {
  UINT32                            Reserved0                :4;
  UINT32                            SecBiosSetup             :1;
  UINT32                            Reserved1                :1;
  UINT32                            ATPBA                    :1;
  UINT32                            ATWWAN                   :1;
  UINT32                            Reserved2                :24;
} BIOS_SEC_CAPABILITYS;

typedef struct {
  UINT32                            Identifer;
} STRUCTURE_IDENTIFIER;

typedef struct {
  UINT32                            SecFwMinorVersion        :16;
  UINT32                            SecFwMajorVersion        :16;
  UINT32                            SecFwBuildVersion        :16;
  UINT32                            SecFwHotfixVersion       :16;
} EFI_MISC_OEM_TYPE_0x83;

//
// This is definition for SMBIOS Oem data type 0x83
//
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT32                    Reserved0;
  UINT32                    Reserved1;
  UINT32                    Reserved2;
  UINT32                    Reserved3;
  UINT32                    Reserved4;
  SEC_CAPABILITYS           SeCCapabilities;
  SEC_CFG_STATE             SeCCfgState;
  UINT32                    Reserved5;
  UINT32                    Reserved6;
  UINT32                    Reserved7;
  BIOS_SEC_CAPABILITYS      BiosSeCCapabilities;
  STRUCTURE_IDENTIFIER      StructureIdentifer;
  UINT32                    Reserverd8;
} SMBIOS_TABLE_TYPE83;


//
// This is definition for SMBIOS Oem data type 0x90
//
typedef struct {
  STRING_REF                         SECVersion;
  STRING_REF                         uCodeVersion;
  STRING_REF                         GOPVersion;
  STRING_REF                         CpuStepping;
} EFI_MISC_OEM_TYPE_0x90;

//
// This is definition for SMBIOS Oem data type 0x90
//
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       SECVersion;
  SMBIOS_TABLE_STRING       uCodeVersion;
  SMBIOS_TABLE_STRING       GOPVersion;
  SMBIOS_TABLE_STRING       CpuStepping;
} SMBIOS_TABLE_TYPE90;

typedef struct {
  STRING_REF                FviPlatform;
  UINT8                     FviVersion;
  STRING_REF                IFWIVersionStr;
  STRING_REF                IFWIVersion;
  STRING_REF                BiosVersionStr;
  STRING_REF                BiosVersion;
  STRING_REF                CseVersionStr;
  STRING_REF                CseVersion;
  STRING_REF                GopVersionStr;
  STRING_REF                GopVersion;
  STRING_REF                MRCVersionStr;
  STRING_REF                MRCVersion;
  STRING_REF                UCodeVersionStr;
  STRING_REF                UCodeVersion;
  STRING_REF                PunitVersionStr;
  STRING_REF                PunitVersion;
  STRING_REF                PMCVersionStr;
  STRING_REF                PMCVersion;
  STRING_REF                ISHVersionStr;
  STRING_REF                ISHVersion;
  STRING_REF                SoCVersionStr;
  STRING_REF                SoCVersion;
  STRING_REF                BoardVersionStr;
  STRING_REF                BoardVersion;
  STRING_REF                FabVersionStr;
  STRING_REF                FabVersion;
  STRING_REF                CPUFlavorStr;
  STRING_REF                CPUFlavor;
  STRING_REF                PmicVersionStr;
  STRING_REF                PmicVersion;
  STRING_REF                SecureBootStr;
  STRING_REF                SecureBoot;
  STRING_REF                BootModeStr;
  STRING_REF                BootMode;
  STRING_REF                SpeedStepModeStr;
  STRING_REF                SpeedStepMode;
  STRING_REF                CPUTurboModeStr;
  STRING_REF                CPUTurboMode;
  STRING_REF                CStateStr;
  STRING_REF                CState;
  STRING_REF                EnhancedCStateStr;
  STRING_REF                EnhancedCState;
  STRING_REF                MaxPkgCStateStr;
  STRING_REF                MaxPkgCState;
  STRING_REF                MaxCoreCStateStr;
  STRING_REF                MaxCoreCState;
  STRING_REF                GfxTurboStr;
  STRING_REF                GfxTurbo;
  STRING_REF                S0ixStr;
  STRING_REF                S0ix;
  STRING_REF                RC6Str;
  STRING_REF                RC6;
} EFI_MISC_OEM_TYPE_0x94;


typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       FviPlatform;
  UINT8                     FviVersion;
  SMBIOS_TABLE_STRING       IFWIVersionStr;
  SMBIOS_TABLE_STRING       IFWIVersion;
  SMBIOS_TABLE_STRING       BiosVersionStr;
  SMBIOS_TABLE_STRING       BiosVersion;
  SMBIOS_TABLE_STRING       CseVersionStr;
  SMBIOS_TABLE_STRING       CseVersion;
  SMBIOS_TABLE_STRING       GopVersionStr;
  SMBIOS_TABLE_STRING       GopVersion;
  SMBIOS_TABLE_STRING       MRCVersionStr;
  SMBIOS_TABLE_STRING       MRCVersion;
  SMBIOS_TABLE_STRING       UCodeVersionStr;
  SMBIOS_TABLE_STRING       UCodeVersion;
  SMBIOS_TABLE_STRING       PunitVersionStr;
  SMBIOS_TABLE_STRING       PunitVersion;
  SMBIOS_TABLE_STRING       PMCVersionStr;
  SMBIOS_TABLE_STRING       PMCVersion;
  SMBIOS_TABLE_STRING       ISHVersionStr;
  SMBIOS_TABLE_STRING       ISHVersion;
  SMBIOS_TABLE_STRING       SoCVersionStr;
  SMBIOS_TABLE_STRING       SoCVersion;
  SMBIOS_TABLE_STRING       BoardVersionStr;
  SMBIOS_TABLE_STRING       BoardVersion;
  SMBIOS_TABLE_STRING       FabVersionStr;
  SMBIOS_TABLE_STRING       FabVersion;
  SMBIOS_TABLE_STRING       CPUFlavorStr;
  SMBIOS_TABLE_STRING       CPUFlavor;
  SMBIOS_TABLE_STRING       PmicVersionStr;
  SMBIOS_TABLE_STRING       PmicVersion;
  SMBIOS_TABLE_STRING       SecureBootStr;
  SMBIOS_TABLE_STRING       SecureBoot;
  SMBIOS_TABLE_STRING       BootModeStr;
  SMBIOS_TABLE_STRING       BootMode;
  SMBIOS_TABLE_STRING       SpeedStepModeStr;
  SMBIOS_TABLE_STRING       SpeedStepMode;
  SMBIOS_TABLE_STRING       CPUTurboModeStr;
  SMBIOS_TABLE_STRING       CPUTurboMode;
  SMBIOS_TABLE_STRING       CStateStr;
  SMBIOS_TABLE_STRING       CState;
  SMBIOS_TABLE_STRING       EnhancedCStateStr;
  SMBIOS_TABLE_STRING       EnhancedCState;
  SMBIOS_TABLE_STRING       MaxPkgCStateStr;
  SMBIOS_TABLE_STRING       MaxPkgCState;
  SMBIOS_TABLE_STRING       MaxCoreCStateStr;
  SMBIOS_TABLE_STRING       MaxCoreCState;
  SMBIOS_TABLE_STRING       GfxTurboStr;
  SMBIOS_TABLE_STRING       GfxTurbo;
  SMBIOS_TABLE_STRING       S0ixStr;
  SMBIOS_TABLE_STRING       S0ix;
  SMBIOS_TABLE_STRING       RC6Str;
  SMBIOS_TABLE_STRING       RC6;
} SMBIOS_TABLE_TYPE94;

#pragma pack()
//
// Data Table Array
//
extern EFI_MISC_SMBIOS_DATA_TABLE mMiscSubclassDataTable[];
//
// For BXTM B0
//
extern EFI_MISC_SMBIOS_DATA_TABLE mMiscSubclassDataTableB0[];

//
// Data Table Array Entries
//
extern UINTN                        mMiscSubclassDataTableEntries;
//
// For BXTM B0
//
extern UINTN                        mMiscSubclassDataTableEntriesB0;
extern EFI_HII_HANDLE               mHiiHandle;

//
// Prototypes
//
EFI_STATUS
EFIAPI
MiscSubclassDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

#endif

