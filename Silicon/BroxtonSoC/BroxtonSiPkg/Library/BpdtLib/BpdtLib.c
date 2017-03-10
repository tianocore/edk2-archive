/** @file
  This file contains the implementation of BpdtLib library.

  The library exposes an abstract interface for accessing boot data
  stored in the BPDT format on the Logical Boot Partition of the boot device.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <SeCAccess.h>
#include <PiPei.h>
#include <Ppi/BlockIoPei.h>
#include <Ppi/Spi.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/ScPlatformLib.h>
#include <Library/BpdtLib.h>
#include <Library/PeiSpiInitLib.h>
#include <Library/BaseCryptLib.h>

#define DIR_HDR_SIGNATURE     SIGNATURE_32('$', 'C', 'P', 'D')
#define DIR_ENTRY_NAME_SIZE   12   //This is spec defined to be bytes
#define SPLIT_ENTRY_PART_1    BIT0
#define SPLIT_ENTRY_PART_2    BIT1
#define SPLIT_ENTRY_FLAGS     (BIT1 | BIT0)
#define BPDT_HOB_DATA_SIZE    (sizeof(BPDT_PAYLOAD_DATA) * BpdtMaxType)
#define SpiMmioBoundary       0xFFFC0000

/**
  Calculate the Address in SPI of the selected Boot Partition

  @param[in]     BpSel                 Which BP to locate on the SPI
  @param[out]    BpFlashLinearAddress  The linear flash address of selected BP
  @param[out]    LbpPointer            A pointer to the selected Boot partition on this media

  @retval        EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
EFIAPI
InternalGetBpSpi (
  IN  BOOT_PARITION_SELECT  BpSel,
  OUT UINT32                *BpFlashLinearAddress,
  OUT VOID                  **LbpPointer
  )
{
  UINT32  BiosAddr;

  //
  // Compute BP FlashLinearAddress.
  //
  switch (BpSel) {
    case BootPart1:
      *BpFlashLinearAddress = 0x1000;
      break;
    case BootPart2:
      if (HeciPciRead16(R_SEC_DevID_VID) != 0xFFFF) {
        //
        // BP2 linear address is the midpoint between BIOS base and expansion data base.
        //
        *BpFlashLinearAddress = (GetSpiFlashRegionBase(BIOS) + GetSpiFlashRegionBase(DeviceExpansion1)) / 2;
      } else {

        *BpFlashLinearAddress = 0x00800000>>1; 
      }
      break;
    default:
      return EFI_NOT_FOUND;
      break;
  }
  DEBUG ((DEBUG_INFO, "BpFlashLinearAddress = %X\n", *BpFlashLinearAddress));
  
  //
  // Calculate Boot partition physical address.
  // FLA[26:0] <= (Flash_Region_Limit) - (FFFF_FFFCh - bios_address)
  //
  BiosAddr = GetSpiFlashRegionLimit(BIOS) + 0xFFC - *BpFlashLinearAddress;
  *LbpPointer = (VOID *)((UINTN)0xFFFFFFFC - BiosAddr);
  DEBUG ((DEBUG_INFO, "Flash address of BP%d = %X\n", BpSel+1, *LbpPointer));

  return EFI_SUCCESS;
}

/**
  SPI CMD read w/a. Because the upper 256k of address space is mapped to CSE SRAM,
  it cannot be directly accessed via MMIO from host.

  @param[in]         BpSel                 Which BP to locate on the SPI
  @param[in out]     CombinedSpiObb        Combined OBB data.       
  
  @retval            EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
EFIAPI
InternalCheckAndJoinUpperSpi (
  IN      BOOT_PARITION_SELECT  BpSel,
  IN OUT  BPDT_PAYLOAD_DATA     *CombinedSpiObb
  )
{
  VOID                      *UpperSpiObb;
  VOID                      *LbpPointer;
  UINT32                    Bp2LinearAddr;
  UINT32                    SpiPart1Size;
  UINT32                    SpiPart2Size;
  EFI_STATUS                Status;
  SC_SPI_PROTOCOL           *SpiPpi;
  CONST EFI_PEI_SERVICES    **PeiServices;

  //
  // Check if Obb part is in upper spi address space.
  // If the entry is from emmc this will be false, it can only be true for SPI.
  //
  if ((UINT32)CombinedSpiObb->DataPtr + CombinedSpiObb->Size >= SpiMmioBoundary) {

    //Get Linear Addr of BpSel (on BXT this will be BP2, since it is)
    InternalGetBpSpi (BpSel, &Bp2LinearAddr, &LbpPointer);

    SpiPart1Size = SpiMmioBoundary - (UINT32)CombinedSpiObb->DataPtr;
    SpiPart2Size = CombinedSpiObb->Size - SpiPart1Size;

    UpperSpiObb = AllocatePages (EFI_SIZE_TO_PAGES (CombinedSpiObb->Size));

    CopyMem (UpperSpiObb, CombinedSpiObb->DataPtr, SpiPart1Size);  //Copy MemMapped part of Spi.

    //
    // Install ScSpi Ppi.
    //
    InstallScSpi ();
    PeiServices = GetPeiServicesTablePointer ();
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gScSpiPpiGuid,
                               0,
                               NULL,
                               (VOID **) &SpiPpi
                               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Cannot Locate gScSpiPpiGuid: %r. Halting system.\n", Status));
      CpuDeadLoop ();
    }

    //
    // Use SPI cmd to read entry into memory.
    //
    Status = SpiPpi->FlashRead (
                       SpiPpi,
                       FlashRegionBios,
                       Bp2LinearAddr + SpiPart1Size,
                       SpiPart2Size,
                       (UINT8 *)((UINTN)UpperSpiObb + SpiPart1Size)
                       );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Read SPI fail: %r. Halting system.\n", Status));
      CpuDeadLoop ();
    }

    CombinedSpiObb->DataPtr = UpperSpiObb;
  }
  return EFI_SUCCESS;
}


/**
  This function will handle locating and combining both parts of Split Obb,
  regardless of which BootPartition that part1 or part2 is located on.

  @param[in]     BpSel            Which BP to locate on the SPI
  @param[out]    ObbPayloadPtr    Pointer to other part of Obb (hopefully on Bp1).
                                  It does not matter if this ptr is actual memory or mmio,
                                  since it needs to be combined anyway.
                                  
  @retval        EFI_SUCCESS      The operation completed successfully.
  @retval        other            The selected BP could not be found.
**/
EFI_STATUS
EFIAPI
InternalLocateSplitObb (
  IN   BOOT_PARITION_SELECT  BpSel,
  OUT  BPDT_PAYLOAD_DATA     *ObbPayloadPtr
  )
{
  UINT8               index;
  VOID                *LbpPointer;
  VOID                *LbpOffset;
  BPDT_HEADER         *BpdtHeaderPtr;
  BPDT_HEADER         *sBpdtHeader;
  BPDT_ENTRY          *BpdtEntryPtr;
  EFI_STATUS          Status;

  Status = GetBootPartitionPointer (BpSel, &LbpPointer);
  ASSERT_EFI_ERROR (Status);

  BpdtHeaderPtr = (BPDT_HEADER *)LbpPointer;
  Status = EFI_NOT_FOUND;
  do {
    //
    //Reset secondary header each time to ensure proper exit.
    //
    sBpdtHeader = NULL;
    BpdtEntryPtr = (BPDT_ENTRY *)((UINTN)BpdtHeaderPtr + sizeof(BPDT_HEADER));

    if (BpdtHeaderPtr->Signature != BPDT_SIGN_GREEN) {
      DEBUG ((EFI_D_INFO, "Signature of BPDT Header is invalid - Stop Parsing BP%d.\n", BpSel));
      break;
    }
    for (index=0; index < BpdtHeaderPtr->DscCount; index++, BpdtEntryPtr++) {
      if (BpdtEntryPtr->LbpOffset == 0) {
        DEBUG ((EFI_D_INFO, "Skipping Stub-entry for Bpdt type: 0x%x \n", BpdtEntryPtr->Type));
        continue;
      }
      LbpOffset = (VOID *)((UINTN)LbpPointer + BpdtEntryPtr->LbpOffset);

      if (BpdtEntryPtr->Type == BpdtSbpdt) {
        sBpdtHeader = (BPDT_HEADER *)LbpOffset;
        continue;
      }
      if (BpdtEntryPtr->Type == BpdtObb) {
        ObbPayloadPtr->DataPtr = LbpOffset;
        ObbPayloadPtr->Size    = BpdtEntryPtr->Size;
        Status = EFI_SUCCESS;
        break;
      }
    }
    if (sBpdtHeader != NULL) {
      BpdtHeaderPtr = sBpdtHeader;
    }
  } while (sBpdtHeader != NULL);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to locate part of Split Obb: %r. Halting system.\n", Status));
    CpuDeadLoop ();
  }
  return Status;
}


/**
  Locate and return a pointer to the selected Logical Boot Partition.

  For block devices, the address will be the location in memory that the BP was copied to.
  For memory mapped device (eg SPI), the address will be the location on the device.

  @param[in]     LbpSel         Which BP to locate on the boot device
  @param[out]    Address        The address of the selected BP on the boot device

  @retval        EFI_SUCCESS    The operation completed successfully.
  @retval        other          The selected BP could not be found.
**/
EFI_STATUS
EFIAPI
GetBootPartitionPointer (
  IN  BOOT_PARITION_SELECT  LbpSel,
  OUT VOID                  **LbpPointer
  )
{
  UINT32                    BpLinearAddr;
  EFI_STATUS                Status;

  DEBUG ((EFI_D_INFO, "CSE Boot Device is SPI.\n"));
  Status = InternalGetBpSpi (LbpSel, &BpLinearAddr, LbpPointer);

  return Status;
}

/**
  Parse the Boot Partition Descriptor Table of the provided Logical Boot Partition.
  Note: "payload" is the Sub-Partition pointed to by the BDPT Entry of the given type.

  @param[in]    LbpPointer      Pointer to the start of the BootPartition to be parsed
  @param[in]    EntryType       The entry type of the subparition(payload) to look for
  @param[out]   BpdtPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval       EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBpdtPayloadDataRaw (
  IN  VOID                *LbpPointer,
  IN  BPDT_ENTRY_TYPES    EntryType,
  OUT BPDT_PAYLOAD_DATA   *BpdtPayloadPtr
  )
{
  UINT8                   Index;
  VOID                    *LbpOffset;
  BPDT_HEADER             *BpdtHeaderPtr;
  BPDT_HEADER             *sBpdtHeader;
  BPDT_ENTRY              *BpdtEntryPtr;


  BpdtHeaderPtr = (BPDT_HEADER *)LbpPointer;
  do {
  	//
    //Reset secondary header each time to ensure proper exit.
    //
    sBpdtHeader = NULL;
    BpdtEntryPtr = (BPDT_ENTRY *)((UINTN)BpdtHeaderPtr + sizeof(BPDT_HEADER));

    DEBUG ((DEBUG_INFO, "BpdtHeaderPtr->Signature = %x\n", BpdtHeaderPtr->Signature));
    if ((BpdtHeaderPtr->Signature != BPDT_SIGN_GREEN) && (BpdtHeaderPtr->Signature != BPDT_SIGN_YELLOW)) {
      DEBUG ((EFI_D_ERROR, "Signature of BPDT Header is invalid - Stop Parsing RawBP.\n"));
      return EFI_NOT_FOUND;
    }
    for (Index=0; Index < BpdtHeaderPtr->DscCount; Index++, BpdtEntryPtr++) {
      if (BpdtEntryPtr->LbpOffset == 0) {
        DEBUG ((EFI_D_INFO, "Skipping Stub-entry for Bpdt type: %d\n", BpdtEntryPtr->Type));
        continue;
      }
      LbpOffset = (VOID *)((UINTN)LbpPointer + BpdtEntryPtr->LbpOffset);

      if (BpdtEntryPtr->Type == BpdtSbpdt) {
        sBpdtHeader = (BPDT_HEADER *)LbpOffset;
      }
      if (BpdtEntryPtr->Type == EntryType) {
        DEBUG ((EFI_D_INFO, "Found BPDT Entry. Type: %d, Addr: 0x%x\n", BpdtEntryPtr->Type, LbpOffset));
        BpdtPayloadPtr->DataPtr = LbpOffset;
        BpdtPayloadPtr->Size = BpdtEntryPtr->Size;
        return EFI_SUCCESS;
      }
    }
    if (sBpdtHeader != NULL) {
      BpdtHeaderPtr = sBpdtHeader;
    }
  } while (sBpdtHeader != NULL);

  BpdtPayloadPtr->DataPtr = NULL;
  BpdtPayloadPtr->Size = 0;
  return EFI_NOT_FOUND;
}

/**
  Parse the Boot Partition Descriptor Table on the selected Logical Boot Partition.
  Takes the BP as input since some BPDT entry types can exist in both locations.
  Note: "payload" is the Sub-Partition pointed to by the BDPT Entry of the given type.

  @param[in]     LbpSel         Which BP to locate on the boot device
  @param[in]    EntryType       The entry type of the subparition(payload) to look for
  @param[out]   BpdtPayloadPtr  Pointer to a Struct (in a HOB) containing the Size and Absolute address
                                in memory(or spi) of the BPDT payload(subpart) of EntryType.
                                If the caller shadows the Payload for performance, it should update this value

  @retval       EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBpdtPayloadData (
  IN  BOOT_PARITION_SELECT  BpSel,
  IN  BPDT_ENTRY_TYPES      EntryType,
  OUT BPDT_PAYLOAD_DATA     **BpdtPayloadPtr
  )
{
  UINT8                 Index;
  VOID                  *LbpOffset;
  VOID                  *LbpBuffer = NULL;
  VOID                  *CombinedObb;
  BPDT_PAYLOAD_DATA     *BpdtPayloadList = NULL;
  BPDT_PAYLOAD_DATA     *AdditionalPayloadList = NULL;
  BPDT_PAYLOAD_DATA     AdditionalObbInfo;
  BPDT_PAYLOAD_DATA     *ObbInfoPart1Ptr;
  BPDT_PAYLOAD_DATA     *ObbInfoPart2Ptr;
  BPDT_HEADER           *BpdtHeaderPtr;
  BPDT_HEADER           *sBpdtHeader;
  BPDT_ENTRY            *BpdtEntryPtr;
  BPDT_ENTRY            *ObbEntryPtr = NULL;
  EFI_HOB_GUID_TYPE     *GuidHobPtr;
  EFI_STATUS            Status;
  EFI_GUID              CurrentBpdtDataGuid;
  EFI_GUID              AdditionalDataGuid;
  BOOT_PARITION_SELECT  AdditionalObbBp;

  //
  // Set the data guid's to the correct HOB's based on the selected BP.
  // If Current contains Split OBB, then Additional needs to be the BP with the other SplitObb.
  //
  switch (BpSel) {
    case BootPart1:
      CurrentBpdtDataGuid = gEfiBpdtLibBp1DataGuid;
      AdditionalDataGuid  = gEfiBpdtLibBp2DataGuid;
      AdditionalObbBp     = BootPart2;
      break;
    case BootPart2:
      CurrentBpdtDataGuid = gEfiBpdtLibBp2DataGuid;
      AdditionalDataGuid  = gEfiBpdtLibBp1DataGuid;
      AdditionalObbBp     = BootPart1;
      break;
    default:
      DEBUG ((EFI_D_ERROR, "Invalid BootPartion parameter: %d\n", BpSel));
      return EFI_NOT_FOUND;
      break;
  }

  *BpdtPayloadPtr = NULL;
  GuidHobPtr = GetFirstGuidHob (&CurrentBpdtDataGuid);
  if (GuidHobPtr != NULL) {
    DEBUG ((EFI_D_INFO, "BPDT %d already parsed. Getting Payload info from HOB\n", BpSel+1));
    BpdtPayloadList = GET_GUID_HOB_DATA (GuidHobPtr);
    *BpdtPayloadPtr = &BpdtPayloadList[EntryType];
    if (*BpdtPayloadPtr == NULL) {
      DEBUG ((EFI_D_WARN, "Requested Payload info in NULL: %d\n", EntryType));
      return EFI_NOT_FOUND;
    } else {
      return EFI_SUCCESS;
    }
  }

  //
  //Must use ZeroPool here to ensure correct error handling of missing entries.
  //
  BpdtPayloadList = AllocateZeroPool (BPDT_HOB_DATA_SIZE);
  if (BpdtPayloadList == NULL) {
    ASSERT (BpdtPayloadList != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetBootPartitionPointer (BpSel, &LbpBuffer);
  ASSERT_EFI_ERROR (Status);

  //
  // Parse the BPDT and store the Entry offsets into the array.
  //
  BpdtHeaderPtr = (BPDT_HEADER *)LbpBuffer;
  do {
  	//
    //reset secondary header each time to ensure proper exit.
    //
    sBpdtHeader = NULL;
    BpdtEntryPtr = (BPDT_ENTRY *)((UINTN)BpdtHeaderPtr + sizeof(BPDT_HEADER));

    if (BpdtHeaderPtr->Signature != BPDT_SIGN_GREEN) {
      DEBUG ((EFI_D_ERROR, "Signature of BPDT Header is invalid - Stop Parsing BP%d.\n", BpSel+1));
      return EFI_NOT_FOUND;
    }
    for (Index=0; Index < BpdtHeaderPtr->DscCount; Index++, BpdtEntryPtr++) {
      if (BpdtEntryPtr->LbpOffset == 0) {
        DEBUG ((EFI_D_INFO, "Skipping Stub-entry for Bpdt type: %d\n", BpdtEntryPtr->Type));
        continue;
      }
      LbpOffset = (VOID *)((UINTN)LbpBuffer + BpdtEntryPtr->LbpOffset);

      if (BpdtEntryPtr->Type == BpdtSbpdt) {
        sBpdtHeader = (BPDT_HEADER *)LbpOffset;
      }
      if (BpdtEntryPtr->Type < BpdtMaxType) {
        DEBUG ((EFI_D_INFO, "Saving Info of BPDT Entry. Type: %d, Addr: 0x%x\n", BpdtEntryPtr->Type, LbpOffset));
        BpdtPayloadList[BpdtEntryPtr->Type].DataPtr = LbpOffset;
        BpdtPayloadList[BpdtEntryPtr->Type].Size = BpdtEntryPtr->Size;
      }
      if (BpdtEntryPtr->Type == BpdtObb) {
        ObbEntryPtr = BpdtEntryPtr;
      }
    }
    if (sBpdtHeader != NULL) {
      BpdtHeaderPtr = sBpdtHeader;
    }
  } while (sBpdtHeader != NULL);


  //
  // If the additional data HOB contains the address of Obb, then the special cases for
  // Obb were already handled by a previous call to this function (no need to recombine =).
  //
  GuidHobPtr = GetFirstGuidHob (&AdditionalDataGuid);
  if (GuidHobPtr != NULL) {
    AdditionalPayloadList = GET_GUID_HOB_DATA (GuidHobPtr);
  }
  if (AdditionalPayloadList != NULL && AdditionalPayloadList[BpdtObb].DataPtr != NULL) {
    //
    // using copymem will auto scale if the struct defintion changes.
    //
    CopyMem (&BpdtPayloadList[BpdtObb], &AdditionalPayloadList[BpdtObb], sizeof (BPDT_PAYLOAD_DATA));
  } else {
    //
    // Check for Payload overlap with non-MMIO SPI region.
    //
    InternalCheckAndJoinUpperSpi (BpSel, &BpdtPayloadList[BpdtObb]);

    //
    // look for and handle the Split OBB case.
    //
    if (ObbEntryPtr != NULL && (ObbEntryPtr->Flags & SPLIT_ENTRY_FLAGS)) {

      DEBUG ((EFI_D_INFO, "Attempting to resolve Split Obb..\n"));
      InternalLocateSplitObb (AdditionalObbBp, &AdditionalObbInfo);
      
      //
      // Since each Obb part could be on either BP, then we also need to check
      // it for overlap into the non-MMIO accessible SPI region.
      //
      InternalCheckAndJoinUpperSpi (AdditionalObbBp, &AdditionalObbInfo);

      if (ObbEntryPtr->Flags & SPLIT_ENTRY_PART_1) {
        ObbInfoPart1Ptr = &BpdtPayloadList[BpdtObb];
        ObbInfoPart2Ptr = &AdditionalObbInfo;
      } else {
        ObbInfoPart1Ptr = &AdditionalObbInfo;
        ObbInfoPart2Ptr = &BpdtPayloadList[BpdtObb];
      }
      
      //
      // Combine the final parts together and set the output address to the result.
      //
      CombinedObb = AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)ObbInfoPart1Ptr->Size + ObbInfoPart2Ptr->Size));
      CopyMem (CombinedObb, ObbInfoPart1Ptr->DataPtr, ObbInfoPart1Ptr->Size);
      CopyMem ((VOID *)((UINTN)CombinedObb+ObbInfoPart1Ptr->Size), ObbInfoPart2Ptr->DataPtr, ObbInfoPart2Ptr->Size);
      BpdtPayloadList[BpdtObb].DataPtr = CombinedObb;
      BpdtPayloadList[BpdtObb].Size = ObbInfoPart1Ptr->Size + ObbInfoPart2Ptr->Size;
    }
  }

  //
  // Build HOB for BPDT data.
  // Need to use a HOB (or local PPI) instead of a module global since the library will be called in PreMem,
  // and PreMem globals will not work if executing in place (ie SPI boot with no code in CAR).
  //
  GuidHobPtr = BuildGuidDataHob (
    &gEfiBpdtLibBp2DataGuid,
    BpdtPayloadList,
    BPDT_HOB_DATA_SIZE
  );
  *BpdtPayloadPtr = &BpdtPayloadList[EntryType];
  FreePool(BpdtPayloadList);

  if (GuidHobPtr == NULL || *BpdtPayloadPtr == NULL) {
   DEBUG ((EFI_D_ERROR, "Unable to create BPDT HOB.\n"));
   return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}


/**
  Search the Subpartition pointed to by BpdtPayloadPtr for a Directory entry with "EntryName",
  then fill in the Struct pointed to by SubPartPayloadPtr with the Addres and Size of the Dir Entry.
  Note: "payload" is the Data pointed to by the Directory Entry with the given name.

  @param[in]   BpdtPayloadPtr     Location and Size of the Payload to search
  @param[in]   EntryName          the String name of the Directory entry to look for
  @param[out]  SubPartPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval      EFI_SUCCESS        The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetSubPartitionPayloadData (
  IN  BPDT_PAYLOAD_DATA     *BpdtPayloadPtr,
  IN  CONST CHAR8           *EntryName,
  OUT BPDT_PAYLOAD_DATA     *SubPartPayloadPtr
  )
{
  UINT8                   Index;
  CHAR8                   SearchName[DIR_ENTRY_NAME_SIZE];
  UINTN                   SearchNameSize;
  SUBPART_DIR_HEADER      *DirectoryHeader;
  SUBPART_DIR_ENTRY       *DirectoryEntry;

  //
  // Pad Name with zeros (per spec) to ensure correct match.
  //
  ZeroMem(SearchName, DIR_ENTRY_NAME_SIZE);
  
  //
  // Safely handle the input string to ensure we dont copy garbage when EntryName is less than 12.
  //
  SearchNameSize = AsciiStrnLenS (EntryName, DIR_ENTRY_NAME_SIZE);
  CopyMem(SearchName, EntryName, SearchNameSize);

  DEBUG ((EFI_D_INFO, "Looking for Directory Entry with Name: %a\n", SearchName));
  DirectoryHeader = (SUBPART_DIR_HEADER *)BpdtPayloadPtr->DataPtr;
  DirectoryEntry = (SUBPART_DIR_ENTRY *)((UINTN)DirectoryHeader + sizeof(SUBPART_DIR_HEADER));

  if (DirectoryHeader->HeaderMarker != DIR_HDR_SIGNATURE) {
    DEBUG ((EFI_D_ERROR, "Directory Header is invalid - return Not_Found.\n"));
    return EFI_NOT_FOUND;
  }
  
  for (Index = 0; Index < DirectoryHeader->NumOfEntries; Index++, DirectoryEntry++) {
    DEBUG ((EFI_D_INFO, " %a Entry is located at %x\n",
      DirectoryEntry->EntryName, (UINTN)DirectoryHeader+DirectoryEntry->EntryOffset));
    if (!CompareMem(DirectoryEntry->EntryName, SearchName, DIR_ENTRY_NAME_SIZE)) {
      SubPartPayloadPtr->DataPtr = (VOID *)((UINTN)DirectoryHeader + DirectoryEntry->EntryOffset);
      SubPartPayloadPtr->Size = DirectoryEntry->EntrySize;
      return EFI_SUCCESS;
    }
  }

  DEBUG ((EFI_D_WARN, "Entry not found.\n"));
  return EFI_NOT_FOUND;
}


/**
  This is a wrapper function for using VerifyHashBpm that will only
  check the hash for the payloads used during normal boot.

  If you want to check other payloads (ie during recovery flow), then the
  calling code should locate and pass in the payload directly to VerifyHashBpm().

  @param[in]  PayloadSel   Which hash in BPM to compare with.

  @retval     EFI_SUCCESS  The function completed successfully and the Hash matched.
  @retval     other        An error occured when locating and computing the Hash.
**/
EFI_STATUS
EFIAPI
LocateAndVerifyHashBpm (
  IN  BPM_HASH_SELECT    PayloadSel
  )
{

  UINT8                  Index;
  EFI_STATUS             Status;
  VOID                   *HashDataPtr;
  UINT32                 HashDataSize;
  UINT32                 FvTailAddr;
  BPM_DATA_FILE          *BpmPtr;
  BPDT_PAYLOAD_DATA      *IbbPayloadPtr;
  BPDT_PAYLOAD_DATA      *ObbPayloadPtr;
  BPDT_PAYLOAD_DATA      SubPartPayload;
  SUBPART_DIR_HEADER     *DirectoryHeader;
  SUBPART_DIR_ENTRY      *DirectoryEntry;

  DEBUG ((EFI_D_INFO, "Searching IBB for BPM..\n" ));
  GetBpdtPayloadData (BootPart1, BpdtIbb, &IbbPayloadPtr);
  if (IbbPayloadPtr == NULL) {
    return EFI_NOT_FOUND;
  }
  Status = GetSubPartitionPayloadData (IbbPayloadPtr, "BPM.met", &SubPartPayload);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BpmPtr = (BPM_DATA_FILE *)SubPartPayload.DataPtr;

  switch (PayloadSel) {
    case HashIbbl:
      Status = GetSubPartitionPayloadData (IbbPayloadPtr, "IBBL", &SubPartPayload);
      HashDataPtr  = SubPartPayload.DataPtr;
      HashDataSize = SubPartPayload.Size;
      break;

    case HashIbbm:
      Status = GetSubPartitionPayloadData (IbbPayloadPtr, "IBB", &SubPartPayload);
      HashDataPtr  = SubPartPayload.DataPtr;
      HashDataSize = SubPartPayload.Size;
      break;

    case HashObb:
      Status = GetBpdtPayloadData (BootPart2, BpdtObb, &ObbPayloadPtr);
      if (ObbPayloadPtr == NULL) {
        return EFI_NOT_FOUND;
      }
      DirectoryHeader = (SUBPART_DIR_HEADER *)ObbPayloadPtr->DataPtr;
      DirectoryEntry = (SUBPART_DIR_ENTRY *)((UINTN)DirectoryHeader + sizeof(SUBPART_DIR_HEADER));
      HashDataPtr = (VOID *)((UINTN)DirectoryHeader + DirectoryEntry->EntryOffset);

      for (Index = 1; Index < DirectoryHeader->NumOfEntries; Index++) {
        DirectoryEntry++;
      }

      FvTailAddr = (UINT32)DirectoryHeader + DirectoryEntry->EntryOffset + DirectoryEntry->EntrySize;
      HashDataSize = FvTailAddr - (UINT32)HashDataPtr;
      break;

    default:

      return EFI_INVALID_PARAMETER;
      break;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = VerifyHashBpm(PayloadSel, BpmPtr, HashDataPtr, HashDataSize);
  return Status;
}



/**
  Computes the Hash of a given data block and compares to the one in the Boot Policy Metadata.

  @param[in]  PayloadSel   Which hash in BPM to compare with.
  @param[in]  PayloadPtr   Pointer to the begining of the data to be hashed.
  @param[in]  DataSize     Size of the data to be hashed.
  @param[in]  BpmPtr       Pointer to the BPM structure.

  @retval EFI_SUCCESS             If the function completed successfully.
  @retval EFI_ABORTED             If the attempt to compute the hash fails.
  @retval EFI_INVALID_PARAMETER   If the hash type or size in BPM is unsupported.
  @retval EFI_SECURITY_VIOLATION  If the has does not match the one in BPM.
**/
EFI_STATUS
EFIAPI
VerifyHashBpm (
  IN BPM_HASH_SELECT   PayloadSel,
  IN BPM_DATA_FILE     *BpmPtr,
  IN VOID              *PayloadPtr,
  IN UINT32            DataSize
  )
{
  UINT8                Index;
  UINT32               BpmHashAlgo;
  UINT32               BpmHashSize;
  UINT8                *BpmHash;
  UINTN                ContextSize;
  VOID                 *HashContext;
  BOOLEAN              HashResult;
  UINT8                Digest[SHA256_DIGEST_SIZE];


  ZeroMem (Digest, SHA256_DIGEST_SIZE);
  ContextSize = Sha256GetContextSize ();
  HashContext = AllocatePool (ContextSize);

  DEBUG((EFI_D_INFO, "VerifyFV: PayloadPtr = %x, DataSize = %x\n", PayloadPtr, DataSize));

  switch (PayloadSel) {
    case HashIbbl:
      BpmHashAlgo = BpmPtr->IbblHashAlgo;
      BpmHashSize = BpmPtr->IbblHashSize;
      BpmHash     = BpmPtr->IbblHash;
      break;

    case HashIbbm:
      BpmHashAlgo = BpmPtr->IbbmHashAlgo;
      BpmHashSize = BpmPtr->IbbmHashSize;
      BpmHash     = BpmPtr->IbbmHash;
      break;

    case HashObb:
    default:
      BpmHashAlgo = BpmPtr->ObbHashAlgo;
      BpmHashSize = BpmPtr->ObbHashSize;
      BpmHash     = BpmPtr->ObbHash;
      break;
  }
  
  if (BpmHashAlgo != 2 || BpmHashSize != SHA256_DIGEST_SIZE) {
    DEBUG((EFI_D_ERROR, "Hash length NOT correct for SHA256.\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  //
  //If hash entry in BPM is empty (all 0s), skip the OBB verification.
  //
  if (!CompareMem(BpmHash, Digest, SHA256_DIGEST_SIZE)) {
    return EFI_SUCCESS;
  }

  DEBUG((EFI_D_INFO, "BPM Info:\n" ));
  DEBUG((EFI_D_INFO, " Payload = %d, HashAlgo = %d, HashSize = %d\n", PayloadSel, BpmHashAlgo, BpmHashSize));
  DEBUG((EFI_D_INFO, " Hash ="));
  for (Index = 0; Index < BpmHashSize; Index++) {
    DEBUG((EFI_D_INFO, " %02x", BpmHash[Index]));
  }
  DEBUG((EFI_D_INFO, "\n"));

  HashResult = Sha256Init (HashContext);
  if (!HashResult) {
    DEBUG((EFI_D_ERROR, "Sha256Init Failed!\n"));
    return EFI_ABORTED;
  }
  HashResult = Sha256Update (HashContext, PayloadPtr, DataSize);
  if (!HashResult) {
    DEBUG((EFI_D_ERROR, "Sha256Update Failed!\n"));
    return EFI_ABORTED;
  }
  HashResult = Sha256Final (HashContext, Digest);
  if (!HashResult) {
    DEBUG((EFI_D_ERROR, "Sha256Final Failed!\n"));
    return EFI_ABORTED;
  }

  DEBUG((EFI_D_INFO, " Hash ="));
  for (Index = 0; Index < BpmHashSize; Index ++) {
    DEBUG((EFI_D_INFO, " %02x", Digest[Index]));
  }
  DEBUG((EFI_D_INFO, "\n"));

  if (CompareMem(BpmHash, Digest, BpmHashSize)) {
    DEBUG((EFI_D_ERROR, "\nHash Mis-Match. Return Security Violation\n"));
    return EFI_SECURITY_VIOLATION;
  }

  return EFI_SUCCESS;
}
