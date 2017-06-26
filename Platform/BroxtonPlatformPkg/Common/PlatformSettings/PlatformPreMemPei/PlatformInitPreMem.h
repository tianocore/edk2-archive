/** @file
  The header file of Platform PEIM.

  Copyright (c) 2010 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PLATFORM_INIT_PREMEM_H__
#define __PLATFORM_INIT_PREMEM_H__

typedef struct  {
  UINT32  Sign           ; ///< Signiture#
  UINT32  CarBase        ; ///< Cache As Ram Base Address
  UINT32  CarSize        ; ///< Cache As Ram Size
  UINT32  IBBSource      ; ///< IBB Address in SRAM
  UINT32  IBBBase        ; ///< IBB Base in CAR.
  UINT32  IBBSize        ; ///< IBB Size
  UINT32  IBBLSource     ; ///< IBBL Address in SRAM
  UINT32  IBBLBase       ; ///< IBBL Base in CAR.
  UINT32  IBBLSize       ; ///< IBBL Size
  UINT32  FITBase        ; ///< FIT Base Address
  UINT32  StackHeapBase  ; ///< STACK&HEAP Base.
  UINT32  StackHeapSize  ; ///< STACK&HEAP Size
  UINT32  HostToCse      ;
  UINT32  CseToHost      ;
  UINT32  ChunkIndex     ;
  UINT32  NumberOfChunks ;
  UINT32  IbbSizeLeft    ;
  UINT32  Chunksize      ;
  UINT64  IbblPerfRecord0; ///< The QWORD Performance record0 of IBBL
  UINT64  IbblPerfRecord1; ///< The QWORD Performance record1 of IBBL
  UINT64  IbblPerfRecord2; ///< The QWORD Performance record2 of IBBL
  UINT64  IbblPerfRecord3; ///< The QWORD Performance record3 of IBBL
  UINT64  IbblPerfRecord4; ///< The QWORD Performance record4 of IBBL
  UINT64  IbblPerfRecord5; ///< The QWORD Performance record5 of IBBL
} CarMapStruc;

//
//Function Prototypes Only - please do not add #includes here
//
/**
  This is the callback function notified by FvFileLoader PPI, it depends on FvFileLoader PPI to load
  the PEIM into memory.

  @param[in]   PeiServices General purpose services available to every PEIM.
  @param[in]   NotifyDescriptor The context of notification.
  @param[in]   Ppi The notify PPI.

  @retval      EFI_SUCCESS if it completed successfully.

**/
EFI_STATUS
EFIAPI
FvFileLoaderPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
SeCUmaEntry (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  );

EFI_STATUS
EFIAPI
GetBiosReservedMemoryPolicy (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_BIOS_RESERVED_MEMORY_POLICY_PPI  *This,
  IN OUT BIOS_RESERVED_MEMORY_CONFIG      *BiosReservedMemoryPolicy
  );

EFI_STATUS
EFIAPI
MfgMemoryTest (
  IN  CONST EFI_PEI_SERVICES        **PeiServices,
  IN  PEI_MFG_MEMORY_TEST_PPI       *This,
  IN  UINT32                        BeginAddress,
  IN  UINT32                        MemoryLength
  );

BOOLEAN
IsRtcUipAlwaysSet (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

VOID
RtcPowerFailureHandler (
  VOID
  );

VOID
ScBaseInit (
  VOID
  );

EFI_STATUS
UpdateBootMode (
  IN CONST EFI_PEI_SERVICES                       **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB                    *PlatformInfoHob
  );

/**
  This function performs Silicon Policy initialization.

  @param[in]  FirmwareConfiguration  It uses to skip specific policy init that depends
                                     on the 'FirmwareConfiguration' variable.
  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
PeiSiPolicyInit (
  VOID
  );

EFI_STATUS
ReadBxtIPlatformIds (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB    *PlatformInfoHob
  );

EFI_STATUS
EFIAPI
InstallMonoStatusCode (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

EFI_STATUS 
PlatformCreateDefaultVariableHob (
IN UINT16  DefaultId
);
#endif

