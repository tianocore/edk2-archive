/** @file
  FmpSample header file

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _FMP_SAMPLE_UPDATE_H_
#define _FMP_SAMPLE_UPDATE_H_

#include <PiDxe.h>

#include <Guid/SystemResourceTable.h>
#include <Guid/FmpSampleImageInfoGuid.h>

#include <Protocol/FirmwareManagement.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DevicePathLib.h>
#include <Library/HobLib.h>

//
// Current Firmwarre version
//
#define CURRENT_FIRMWARE_VERSION            0x00000001
//
// The lowesest Supported Firmware Version
//
#define LOWEST_SUPPORTED_FIRMWARE_VERSION   0x00000001


//
// Image Version number
//

#define FMP_SAMPLE_PRIVATE_DATA_SIGNATURE  SIGNATURE_32('F', 'M', 'P', 'S')

#pragma pack(1)
typedef struct{
  UINT32 Version;
  UINT32 Data;
}FMP_SAMPLE_UDPATE_DATA;
#pragma pack()

//
// LAN MAC private data structure.
//
struct _FMP_SAMPLE_PRIVATE_DATA{
  UINT32                            Signature;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  Fmp;
  EFI_HANDLE                        Handle;
  UINT8                             DescriptorCount;
  EFI_SYSTEM_RESOURCE_ENTRY         EsrtInfo;
  FMP_SAMPLE_UDPATE_DATA            UpdateData;
};

typedef struct _FMP_SAMPLE_PRIVATE_DATA  FMP_SAMPLE_PRIVATE_DATA;


/**
  Returns a pointer to the FMP_SAMPLE_PRIVATE_DATA structure from the input a as Fmp.
  
  If the signatures matches, then a pointer to the data structure that contains 
  a specified field of that data structure is returned.
   
  @param  a              Pointer to the field specified by ServiceBinding within 
                         a data structure of type FMP_SAMPLE_PRIVATE_DATA.
 
**/
#define FMP_SAMPLE_PRIVATE_DATA_FROM_FMP(a) \
  CR ( \
  (a), \
  FMP_SAMPLE_PRIVATE_DATA, \
  Fmp, \
  FMP_SAMPLE_PRIVATE_DATA_SIGNATURE \
  )


EFI_STATUS
EFIAPI FmpGetImageInfo(
  IN EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN OUT    UINTN                           *ImageInfoSize,
  IN OUT    EFI_FIRMWARE_IMAGE_DESCRIPTOR   *ImageInfo,
  OUT       UINT32                          *DescriptorVersion,
  OUT       UINT8                           *DescriptorCount,
  OUT       UINTN                           *DescriptorSize,
  OUT       UINT32                          *PackageVersion,
  OUT       CHAR16                          **PackageVersionName
  );

EFI_STATUS
EFIAPI FmpGetImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  OUT  VOID                         *Image,
  IN  OUT  UINTN                        *ImageSize
  );

EFI_STATUS
EFIAPI FmpSetImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL                 *This,
  IN  UINT8                                            ImageIndex,
  IN  CONST VOID                                       *Image,
  IN  UINTN                                            ImageSize,
  IN  CONST VOID                                       *VendorCode,
  IN  EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS    Progress,
  OUT CHAR16                                           **AbortReason
  );

EFI_STATUS
EFIAPI FmpCheckImage(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN  UINT8                             ImageIndex,
  IN  CONST VOID                        *Image,
  IN  UINTN                             ImageSize,
  OUT UINT32                            *ImageUpdatable
  );

EFI_STATUS
EFIAPI FmpGetPackageInfo(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL *This,
  OUT UINT32                           *PackageVersion,
  OUT CHAR16                           **PackageVersionName,
  OUT UINT32                           *PackageVersionNameMaxLen,
  OUT UINT64                           *AttributesSupported,
  OUT UINT64                           *AttributesSetting
  );

EFI_STATUS
EFIAPI FmpSetPackageInfo(
  IN  EFI_FIRMWARE_MANAGEMENT_PROTOCOL   *This,
  IN  CONST VOID                         *Image,
  IN  UINTN                              ImageSize,
  IN  CONST VOID                         *VendorCode,
  IN  UINT32                             PackageVersion,
  IN  CONST CHAR16                       *PackageVersionName
  );

/*++
  
  Routine Description:
  
     Initialize FmpSampleDriver private data structure.
  
  Arguments:
  

     FmpSamplePrivate     - private data structure to be initialized.
  
  Returns:

  --*/
EFI_STATUS
InitializePrivateData(
  IN FMP_SAMPLE_PRIVATE_DATA *FmpSamplePrivate
  );

extern EFI_FIRMWARE_MANAGEMENT_PROTOCOL mFirmwareManagementProtocol;

#endif

