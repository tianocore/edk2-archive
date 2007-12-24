/** @file
  HII Library implementation that uses DXE protocols and services.

  Copyright (c) 2006, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <PiDxe.h>

#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_HII_DATABASE_PROTOCOL   *mHiiDatabaseProt;
EFI_HII_STRING_PROTOCOL     *mHiiStringProt;


EFI_STATUS
EFIAPI
UefiHiiLibConstructor (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS Status;
  
  Status = gBS->LocateProtocol (
      &gEfiHiiDatabaseProtocolGuid,
      NULL,
      (VOID **) &mHiiDatabaseProt
    );
  ASSERT_EFI_ERROR (Status);
  ASSERT (mHiiDatabaseProt != NULL);

  Status = gBS->LocateProtocol (
      &gEfiHiiStringProtocolGuid,
      NULL,
      (VOID **) &mHiiStringProt
    );
  ASSERT_EFI_ERROR (Status);
  ASSERT (mHiiStringProt != NULL);

  return EFI_SUCCESS;
}

EFI_HII_PACKAGE_LIST_HEADER *
HiiLibInternalPreparePackages (
  IN UINTN           NumberOfPackages,
  IN CONST EFI_GUID  *GuidId, OPTIONAL
  VA_LIST            Marker
  )
{
  EFI_HII_PACKAGE_LIST_HEADER *PackageListHeader;
  UINT8                       *PackageListData;
  UINT32                      PackageListLength;
  UINT32                      PackageLength;
  EFI_HII_PACKAGE_HEADER      PackageHeader;
  UINT8                       *PackageArray;
  UINTN                       Index;

  PackageListLength = sizeof (EFI_HII_PACKAGE_LIST_HEADER);

  for (Index = 0; Index < NumberOfPackages; Index++) {
    CopyMem (&PackageLength, VA_ARG (Marker, VOID *), sizeof (UINT32));
    PackageListLength += (PackageLength - sizeof (UINT32));
  }

  //
  // Include the lenght of EFI_HII_PACKAGE_END
  //
  PackageListLength += sizeof (EFI_HII_PACKAGE_HEADER);
  PackageListHeader = AllocateZeroPool (PackageListLength);
  ASSERT (PackageListHeader != NULL);
  CopyMem (&PackageListHeader->PackageListGuid, GuidId, sizeof (EFI_GUID));
  PackageListHeader->PackageLength = PackageListLength;

  PackageListData = ((UINT8 *) PackageListHeader) + sizeof (EFI_HII_PACKAGE_LIST_HEADER);

  for (Index = 0; Index < NumberOfPackages; Index++) {
    PackageArray = (UINT8 *) VA_ARG (Marker, VOID *);
    CopyMem (&PackageLength, PackageArray, sizeof (UINT32));
    PackageLength  -= sizeof (UINT32);
    PackageArray += sizeof (UINT32);
    CopyMem (PackageListData, PackageArray, PackageLength);
    PackageListData += PackageLength;
  }

  //
  // Append EFI_HII_PACKAGE_END
  //
  PackageHeader.Type = EFI_HII_PACKAGE_END;
  PackageHeader.Length = sizeof (EFI_HII_PACKAGE_HEADER);
  CopyMem (PackageListData, &PackageHeader, PackageHeader.Length);

  return PackageListHeader;
}

EFI_HII_PACKAGE_LIST_HEADER *
EFIAPI
PreparePackageList (
  IN UINTN                    NumberOfPackages,
  IN EFI_GUID                 *GuidId,
  ...
  )
/*++

Routine Description:
  Assemble EFI_HII_PACKAGE_LIST according to the passed in packages.

Arguments:
  NumberOfPackages  -  Number of packages.
  GuidId            -  Package GUID.

Returns:
  Pointer of EFI_HII_PACKAGE_LIST_HEADER.

--*/
{
  EFI_HII_PACKAGE_LIST_HEADER *PackageListHeader;
  VA_LIST                     Marker;

  VA_START (Marker, GuidId);
  PackageListHeader = HiiLibInternalPreparePackages (NumberOfPackages, GuidId, Marker);
  VA_END (Marker);

  return PackageListHeader;
}


/**
  This function allocates pool for an EFI_HII_PACKAGE_LIST structure
  with additional space that is big enough to host all packages described by the variable 
  argument list of package pointers.  The allocated structure is initialized using NumberOfPackages, 
  GuidId,  and the variable length argument list of package pointers.

  Then, EFI_HII_PACKAGE_LIST will be register to the default System HII Database. The
  Handle to the newly registered Package List is returned throught HiiHandle.

  @param  NumberOfPackages  The number of HII packages to register.
  @param  GuidId                    Package List GUID ID.
  @param  HiiHandle                The ID used to retrieve the Package List later.
  @param  ...                          The variable argument list describing all HII Package.

  @return
  The allocated and initialized packages.

**/

EFI_STATUS
EFIAPI
HiiLibAddPackagesToHiiDatabase (
  IN       UINTN               NumberOfPackages,
  IN CONST EFI_GUID            *GuidId,
  OUT      EFI_HII_HANDLE      *HiiHandle, OPTIONAL
  ...
  )
{
  VA_LIST                   Args;
  EFI_HII_PACKAGE_LIST_HEADER *PackageListHeader;
  VA_LIST                     Marker;
  EFI_STATUS                Status;


  VA_START (Args, GuidId);
  PackageListHeader = InternalPreparePackages (NumberOfPackages, GuidId, Args);

  Status      = mHiiDatabaseProt->NewPackageList (mHiiDatabaseProt, PackageListHeader, gImageHandle, HiiHandle);
  if (HiiHandle != NULL) {
    if (EFI_ERROR (Status)) {
      *HiiHandle = NULL;
    }
  }

  FreePool (PackageListHeader);
  
  return Status;
}

EFI_STATUS
EFIAPI
HiiLibRemovePackagesFromHiiDatabase (
  IN      EFI_HII_HANDLE      HiiHandle
  )
{
  return mHiiDatabaseProt->RemovePackageList (mHiiDatabaseProt, HiiHandle);
}

