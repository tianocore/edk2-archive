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

#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "OsipPrivate.h"
#include "BdsPlatform.h"

#define EFI_MAX_E820_ENTRY  100

typedef enum {
  EfiAcpiAddressRangeMemory   = 1,
  EfiAcpiAddressRangeReserved = 2,
  EfiAcpiAddressRangeACPI     = 3,
  EfiAcpiAddressRangeNVS      = 4
} EFI_ACPI_MEMORY_TYPE;

#pragma pack(1)

typedef struct {
  UINT32                BaseAddr;
  UINT32                Length;
  UINT32                Type;
} EFI_E820_ENTRY;

#pragma pack()

EFI_STATUS
ValidateFvHeader (
  IN    VOID                *Buffer,
  IN    UINTN               Size
  )
{
  EFI_FIRMWARE_VOLUME_HEADER   *FvHdr;

  FvHdr = (EFI_FIRMWARE_VOLUME_HEADER *) Buffer;

  if (FvHdr->Signature != EFI_FVH_SIGNATURE) {
    DEBUG ((EFI_D_ERROR, "Error: Invalid FV signature\n"));
    return EFI_VOLUME_CORRUPTED;
  }

  if (FvHdr->FvLength > Size) {
    DEBUG ((EFI_D_ERROR,
      "Error: FV length (0x%x) is larger than data read (0x%x)\n",
      FvHdr->FvLength,
      Size));
    return EFI_VOLUME_CORRUPTED;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ValidateOsip (
  IN  OSIP_HEADER   *Osip,
  IN  UINTN         MaxBlocks
  )
{
  UINTN           idx;
  UINTN           nextBlock = OSIP_SIZE_IN_BLOCKS;

  DEBUG ((EFI_D_INFO, "Parsing OSIP Header...\n")) ;
  DEBUG ((EFI_D_INFO, " Signature         : %x\n", Osip->Signature));
  DEBUG ((EFI_D_INFO, " MajorRevision     : %x\n", Osip->MajorRevision));
  DEBUG ((EFI_D_INFO, " MinorRevision     : %x\n", Osip->MinorRevision));
  DEBUG ((EFI_D_INFO, " NumberOfPointers  : %x\n", Osip->NumberOfPointers));
  DEBUG ((EFI_D_INFO, " NumberOfImages    : %x\n", Osip->NumberOfImages));
  DEBUG ((EFI_D_INFO, " Checksum          : %x\n", Osip->Checksum));

  if (Osip->Signature != OSIP_HEADER_SIGNATURE)             return EFI_INVALID_PARAMETER;
  if (Osip->MajorRevision != OSIP_HEADER_MAJOR_REVISION)    return EFI_INVALID_PARAMETER;
  if (Osip->MinorRevision != OSIP_HEADER_MINOR_REVISION)    return EFI_INVALID_PARAMETER;
  if (Osip->NumberOfPointers == 0)                          return EFI_INVALID_PARAMETER;
  if (Osip->NumberOfPointers > MAX_OSII_ENTRIES)            return EFI_INVALID_PARAMETER;
  if (Osip->NumberOfImages == 0)                            return EFI_INVALID_PARAMETER;
  if (Osip->NumberOfImages > Osip->NumberOfPointers)        return EFI_INVALID_PARAMETER;

  //
  // TODO: Validate checksum, not sure what good that does though...
  //
  DEBUG ((EFI_D_INFO, "Parsing OSII Entries...\n"));

  for (idx = 0; idx < Osip->NumberOfPointers; idx++) {
    DEBUG((EFI_D_INFO, "Image %d\n", idx + 1));
    DEBUG((EFI_D_INFO, " MajorRevision     : %x\n", Osip->Osii[idx].MajorRevision));
    DEBUG((EFI_D_INFO, " MinorRevision     : %x\n", Osip->Osii[idx].MinorRevision));
    DEBUG((EFI_D_INFO, " FirstBlock        : %x\n", Osip->Osii[idx].FirstBlock));
    DEBUG((EFI_D_INFO, " LoadAddress       : %x\n", Osip->Osii[idx].LoadAddress));
    DEBUG((EFI_D_INFO, " EntryPoint        : %x\n", Osip->Osii[idx].EntryPoint));
    DEBUG((EFI_D_INFO, " BlockCount        : %x\n", Osip->Osii[idx].BlockCount));
    DEBUG((EFI_D_INFO, " Attributes        : %x\n", Osip->Osii[idx].Attributes));

    //
    // Enforce ordering, do not permit empty entries or holes
    //
    if (Osip->Osii[idx].FirstBlock != nextBlock)       return EFI_INVALID_PARAMETER;
    if (Osip->Osii[idx].BlockCount == 0)               return EFI_INVALID_PARAMETER;
    nextBlock += Osip->Osii[idx].BlockCount;

    //
    // TODO: More intensive OSII validation
    //
  }

  //
  // Make sure numBlocks is not pointing past the end of the device
  //
  if (nextBlock > MaxBlocks) return EFI_INVALID_PARAMETER;

  return EFI_SUCCESS;
}


VOID
RebaseImage (
  IN  EFI_PHYSICAL_ADDRESS  DstBuffer,
  IN  EFI_PHYSICAL_ADDRESS  SrcBuffer,
  IN  UINTN                 NumberOfBytes
  )
{
  UINT8   *sptr;
  UINT8   *dptr;
  UINTN   idx;

  sptr      = (UINT8 *) (UINTN) SrcBuffer;
  dptr      = (UINT8 *) (UINTN) DstBuffer;

  for (idx = 0; idx < NumberOfBytes; ++idx, ++dptr, ++sptr) {
    *dptr = *sptr;
  }

  return;
}


UINTN
GetImageSizeByNumber (
  IN  UINTN  ImageNumber
  )
{
  return mOsip->Osii[ImageNumber].BlockCount * OSP_BLOCK_SIZE;
}


EFI_STATUS
LoadImageByNumber (
  IN  UINTN                 ImageNumber,
  OUT EFI_PHYSICAL_ADDRESS  *ImageBase,
  OUT UINTN                 *NumberOfPages
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  physAddr;
  UINTN                 firstBlock;
  UINTN                 numBlocks;
  UINTN                 numPages;

  //
  // Parse OSII entry and find OS Image size in bytes
  //
  firstBlock  = mOsip->Osii[ImageNumber].FirstBlock;
  numBlocks   = mOsip->Osii[ImageNumber].BlockCount;
  numPages    = OSP_BLOCKS_TO_PAGES (numBlocks);

  //
  // Allocate image buffer
  //
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiBootServicesData,
                  numPages,
                  &physAddr);

  if (EFI_ERROR (Status)) ASSERT_EFI_ERROR (Status);

  //
  // Copy OSII[ImageNumber] to buffer
  //
  Status = mBootableBlockIo->ReadBlocks (
                               mBootableBlockIo,
                               mBootableBlockIo->Media->MediaId,
                               firstBlock,
                               numBlocks * OSP_BLOCK_SIZE,
                               (VOID *) (UINTN) physAddr
                               );

  if (EFI_ERROR (Status)) {
    gBS->FreePages (physAddr, numPages);
    return EFI_DEVICE_ERROR;
  }

  *ImageBase      = physAddr;
  *NumberOfPages  = numPages;

  return EFI_SUCCESS;
}


/**
  Convert EFI Memory Type to E820 Memory Type.

  @param[in]  Type      EFI Memory Type

  @retval               ACPI Memory Type for EFI Memory Type

**/
EFI_ACPI_MEMORY_TYPE
EfiMemoryTypeToE820Type (
  IN  UINT32    Type
  )
{
  switch (Type) {
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiConventionalMemory:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
      return EfiAcpiAddressRangeMemory;

    case EfiACPIReclaimMemory:
      return EfiAcpiAddressRangeACPI;

    case EfiACPIMemoryNVS:
      return EfiAcpiAddressRangeNVS;

    default:
      return EfiAcpiAddressRangeReserved;
  }
}

