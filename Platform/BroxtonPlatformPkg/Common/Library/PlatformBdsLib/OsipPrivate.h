/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DevicePathLib.h>
#include "BdsPlatform.h"

#pragma pack(1)
//
// Forward declarations
//
typedef struct _OSIP_HEADER_V0100   OSIP_HEADER_V0100;
typedef struct _OSII_ENTRY_V0100    OSII_ENTRY_V0100 ;

//
// Supported values for OSIP version 1.0 (PNW A0)
//
#define OSIP_HEADER_MAJOR_REVISION    1
#define OSIP_HEADER_MINOR_REVISION    0
#define OSIP_HEADER_SIGNATURE         0x24534F24
#define OSIP_SIZE_IN_BLOCKS           1
#define OSIP_HEADER                   OSIP_HEADER_V0100
#define OSII_ENTRY                    OSII_ENTRY_V0100

//
// Module global definitions
//
#define OSP_BLOCK_SIZE            512
#define OSP_BLOCKS_TO_PAGES(x)    (EFI_SIZE_TO_PAGES(x * OSP_BLOCK_SIZE))
#define MAX_OSII_ENTRIES          ((OSIP_SIZE_IN_BLOCKS * OSP_BLOCK_SIZE - sizeof (OSIP_HEADER)) / sizeof (OSII_ENTRY))
#define VRL_SIZE_LOCATION         0x20

//
// Definitions for PNW A0...
//
typedef struct _SMIP_FHOB_DWORD0 {
  UINT8 MsicModuleId            : 2;
  UINT8 Qualifier               : 1;
  UINT8 OSRCR                   : 1;
  UINT8 EOSR                    : 1;
  UINT8 SafeModeJumper          : 1;
  UINT8 MM                      : 1;
  UINT8 NOREn                   : 1;  ///< 8
  UINT8 OSVEn                   : 1;
  UINT8 OSDnX                   : 1;
  UINT8 Reserved11to10          : 2;
  UINT8 Reserved19to12          : 8;  ///< 20
  UINT8 Reserved25to20          : 6;
  UINT8 MemoryChannels          : 1;
  UINT8 MemoryBurstLength       : 1;  ///< 28
  UINT8 MemoryDeviceDensity     : 2;
  UINT8 MemoryRanks             : 1;
  UINT8 MemoryFrequencyRatio    : 1;  ///< 32
} SMIP_FHOB_DWORD0 ;

struct _OSII_ENTRY_V0100 {
  UINT16    MinorRevision;
  UINT16    MajorRevision;
  UINT32    FirstBlock;
  UINT32    LoadAddress;
  UINT32    EntryPoint;
  UINT32    BlockCount;
  UINT8     Attributes;
  UINT8     Reserved1[3];
} ;

struct _OSIP_HEADER_V0100 {
  UINT32      Signature;
  UINT8       Reserved1;
  UINT8       MinorRevision;
  UINT8       MajorRevision;
  UINT8       Checksum;
  UINT8       NumberOfPointers;
  UINT8       NumberOfImages;
  UINT16      HeaderSize;
  UINT8       Reserved2[0x14];
  OSII_ENTRY  Osii[1];
} ;

#pragma pack()

//
// Global Variables
//
#include <Protocol/BlockIo.h>

extern OSIP_HEADER             *mOsip;
extern OSIP_HEADER             *mOsip;
extern BOOLEAN                  mOsipFound;
extern EFI_BLOCK_IO_PROTOCOL   *mEmmcBlockIo;
extern EFI_BLOCK_IO_PROTOCOL   *mSdBlockIo;
extern EFI_BLOCK_IO_PROTOCOL   *mBootableBlockIo;

//
// Functions oustide of the main module
//

//
// OspUtil.c
//
EFI_STATUS
ValidateFvHeader (
  IN    VOID                *Buffer,
  IN    UINTN               Size
  );

EFI_STATUS
ValidateOsip (
  IN  OSIP_HEADER           *OsipHdr,
  IN  UINTN                 MaxBlocks
  );

EFI_STATUS
ConnectSecondStageFvDevice (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect,
  OUT EFI_HANDLE                *MatchingHandle
  );

VOID
RebaseImage (
  IN  EFI_PHYSICAL_ADDRESS  DstBuffer,
  IN  EFI_PHYSICAL_ADDRESS  SrcBuffer,
  IN  UINTN                 NumberOfBytes
  );

UINTN
GetImageSizeByNumber (
  IN  UINTN  ImageNumber
  );

EFI_STATUS
LoadImageByNumber (
  IN  UINTN                 ImageNumber,
  OUT EFI_PHYSICAL_ADDRESS  *ImageBase,
  OUT UINTN                 *NumberOfPages
  );

//
// OsrIpc.c
//
EFI_STATUS
OsrIpcGetDnx ();

EFI_STATUS
OsrIpcGetOsip (
  IN EFI_PHYSICAL_ADDRESS   osipBase
  );

EFI_STATUS
OsrIpcGetOsImage (
  IN  OSII_ENTRY            *osiiEntry,
  OUT EFI_PHYSICAL_ADDRESS  osBuffer
  );

EFI_STATUS
OsrIpcEndOfUpdate ();

