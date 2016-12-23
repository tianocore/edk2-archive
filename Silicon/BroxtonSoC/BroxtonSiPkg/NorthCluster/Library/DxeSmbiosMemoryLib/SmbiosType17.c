/** @file
  This library will determine memory configuration information from the chipset
  and memory and create SMBIOS memory structures appropriately.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmbiosMemory.h"

//
// Memory Device (Type 17) data
//
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE17 SmbiosTableType17Data = {
  { EFI_SMBIOS_TYPE_MEMORY_DEVICE, sizeof (SMBIOS_TABLE_TYPE17), 0 },
  TO_BE_FILLED, ///< MemoryArrayHandle
  0xFFFE,       ///< MemoryErrorInformationHandle
  TO_BE_FILLED, ///< TotalWidth
  TO_BE_FILLED, ///< DataWidth
  TO_BE_FILLED, ///< Size
  TO_BE_FILLED, ///< FormFactor
  0,            ///< DeviceSet
  STRING_1,     ///< DeviceLocator
  STRING_2,     ///< BankLocator
  TO_BE_FILLED, ///< MemoryType
  {             ///< TypeDetail
    0,            ///< Reserved     :1;
    0,            ///< Other        :1;
    0,            ///< Unknown      :1;
    0,            ///< FastPaged    :1;
    0,            ///< StaticColumn :1;
    0,            ///< PseudoStatic :1;
    TO_BE_FILLED, ///< Rambus       :1;
    TO_BE_FILLED, ///< Synchronous  :1;
    0,            ///< Cmos         :1;
    0,            ///< Edo          :1;
    0,            ///< WindowDram   :1;
    0,            ///< CacheDram    :1;
    0,            ///< Nonvolatile  :1;
    0,            ///< Registered   :1;
    0,            ///< Unbuffered   :1;
    0,            ///< Reserved1    :1;
  },
  TO_BE_FILLED, ///< Speed
  TO_BE_FILLED, ///< Manufacturer
  TO_BE_FILLED, ///< SerialNumber
  TO_BE_FILLED, ///< AssetTag
  TO_BE_FILLED, ///< PartNumber
  TO_BE_FILLED, ///< Attributes
  TO_BE_FILLED, ///< ExtendedSize
  TO_BE_FILLED, ///< ConfiguredMemoryClockSpeed
  TO_BE_FILLED, ///< MinimumVoltage
  TO_BE_FILLED, ///< MaximumVoltage
  TO_BE_FILLED, ///< ConfiguredVoltage
};
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TYPE17_STRING_ARRAY SmbiosTableType17Strings = {
  TO_BE_FILLED_STRING,  ///< DeviceLocator
  TO_BE_FILLED_STRING,  ///< BankLocator
  TO_BE_FILLED_STRING,  ///< Manufacturer
  TO_BE_FILLED_STRING,  ///< SerialNumber
#ifdef MEMORY_ASSET_TAG ///< AssetTag
  MEMORY_ASSET_TAG,
#else
  "0123456789",
#endif
  TO_BE_FILLED_STRING   ///< PartNumber
};


//
// Even SPD Addresses only as we read Words
//
const UINT8
  SpdAddress[] = { 2, 8, 116, 118, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144 };


/**
  This function installs SMBIOS Table Type 17 (Memory Device).
  This function installs one table per memory device slot, whether populated or not.

  @param[in] SmbiosProtocol          Instance of Smbios Protocol.

  @retval    EFI_SUCCESS             If the data is successfully reported.
  @retval    EFI_OUT_OF_RESOURCES    If not able to get resources.
  @retval    EFI_INVALID_PARAMETER   If a required parameter in a subfunction is NULL.

**/
EFI_STATUS
InstallSmbiosType17 (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT16                          Index;
  UINT32                          DimmMemorySizeInMB;
  UINT8                           Dimm;
  UINT8                           *SmbusBuffer;
  UINTN                           SmbusBufferSize;
  UINTN                           SmbusOffset;
  CHAR8                           *StringBuffer;
  CHAR8                           *StringBufferStart;
  UINTN                           StringBufferSize;
  CHAR8                           StringBuffer2[4];
  UINT8                           IndexCounter;
  UINTN                           IdListIndex;
  BOOLEAN                         SlotPresent;
  UINT16                          MemoryTotalWidth;
  UINT16                          MemoryDataWidth;
  UINT8                           i;
  BOOLEAN                         FoundManufacturer;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  UINTN                           StrBufferLen;

  Status = EFI_SUCCESS;

  //
  // StringBuffer should only use around 50 to 60 characters max.
  // Therefore, allocate around double that, as a saftey margin
  //
  StringBufferSize  = (sizeof (CHAR8)) * 100;
  StringBufferStart = AllocateZeroPool (StringBufferSize);
  if (StringBufferStart == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmbusBuffer     = NULL;
  SmbusBufferSize = 0x100; //< SPD data section is exactly 256 bytes.
  SmbusBuffer     = AllocatePool (SmbusBufferSize);
  if (SmbusBuffer == NULL) {
    FreePool (StringBufferStart);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get Memory size parameters for each rank from the chipset registers
  //
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &mMemInfoHob);

  //
  // Each instance of table type 17 has the same MemoryArrayHandle
  //
  SmbiosTableType17Data.MemoryArrayHandle = mSmbiosType16Handle;

  for (Dimm = 0; Dimm < SA_MC_MAX_SOCKETS; Dimm++) {
    SlotPresent = FALSE;
    if (mMemInfoHob->MemInfoData.DimmPresent[Dimm]) {
      SlotPresent = TRUE;
    }
    //
    // Don't create Type 17 tables for sockets that don't exist
    //
    if (!SlotPresent) {
      continue;
    }
    //
    // Generate Memory Device info (Type 17)
    //
    ZeroMem (SmbusBuffer, SmbusBufferSize);

    //
    // Only read the SPD data if the DIMM is populated in the slot.
    //
    if (SlotPresent) {
      for (i = 0; i < sizeof SpdAddress; i++) {
        SmbusOffset = SpdAddress[i];
        *(UINT16 *) (SmbusBuffer + SmbusOffset) = *(UINT16 *) (mMemInfoHob->MemInfoData.DimmsSpdData[Dimm] + SmbusOffset);
      }
    }

    //
    // Use SPD data to generate Device Type info
    //
    SmbiosTableType17Strings.DeviceLocator = DimmToDevLocator[Dimm];
    SmbiosTableType17Strings.BankLocator = DimmToBankLocator[Dimm];

    if (SlotPresent) {
      //
      // Reset StringBuffer
      //
      StringBuffer = StringBufferStart;

      //
      // Show name for known manufacturer or ID for unknown manufacturer
      //
      FoundManufacturer = FALSE;

      //
      // Calculate index counter
      // Clearing Bit7 as it is the Parity Bit for Byte 117
      //
      IndexCounter = SmbusBuffer[117] & (~0x80);

      //
      // Convert memory manufacturer ID to string
      //
      for (IdListIndex = 0; MemoryModuleManufactureList[IdListIndex].Index != 0xff; IdListIndex++) {
        if (MemoryModuleManufactureList[IdListIndex].Index == IndexCounter &&
            MemoryModuleManufactureList[IdListIndex].ManufactureId == SmbusBuffer[118]
            ) {
          SmbiosTableType17Strings.Manufacturer = MemoryModuleManufactureList[IdListIndex].ManufactureName;
          FoundManufacturer = TRUE;
          break;
        }
      }
      //
      // Use original data if no conversion information in conversion table
      //
      StrBufferLen = StringBufferSize / sizeof (CHAR8);

      if (!(FoundManufacturer)) {
        AsciiStrCpyS (StringBuffer, StrBufferLen, "");
        for (Index = 117; Index < 119; Index++) {
          AsciiValueToString (StringBuffer2, PREFIX_ZERO, SmbusBuffer[Index], 2);
          AsciiStrCatS (StringBuffer, StrBufferLen, StringBuffer2);
        }
        SmbiosTableType17Strings.Manufacturer = StringBuffer;
        StringBuffer += AsciiStrSize (StringBuffer);
      }

      AsciiStrCpyS (StringBuffer, StrBufferLen, "");
      for (Index = 122; Index < 126; Index++) {
        AsciiValueToString (StringBuffer2, PREFIX_ZERO, SmbusBuffer[Index], 2);
        AsciiStrCatS (StringBuffer, StrBufferLen, StringBuffer2);
      }
      SmbiosTableType17Strings.SerialNumber = StringBuffer;
      StringBuffer += AsciiStrSize (StringBuffer);

      AsciiStrCpyS (StringBuffer, StrBufferLen, "");
      for (Index = 128; Index < 146; Index++) {
        AsciiSPrint (StringBuffer2, 4, "%c", SmbusBuffer[Index]);
        AsciiStrCatS (StringBuffer, StrBufferLen, StringBuffer2);
      }
      SmbiosTableType17Strings.PartNumber = StringBuffer;

      ASSERT ((StringBuffer + AsciiStrSize (StringBuffer)) < (StringBufferStart + StringBufferSize));

      SmbiosTableType17Data.Manufacturer = STRING_3;
      SmbiosTableType17Data.SerialNumber = STRING_4;
      SmbiosTableType17Data.AssetTag     = STRING_5;
      SmbiosTableType17Data.PartNumber   = STRING_6;

      MemoryDataWidth = 8 * (1 << mMemInfoHob->MemInfoData.BusWidth);
      MemoryTotalWidth = MemoryDataWidth;
      SmbiosTableType17Data.TotalWidth = MemoryTotalWidth;
      SmbiosTableType17Data.DataWidth = MemoryDataWidth;

      DimmMemorySizeInMB = mMemInfoHob->MemInfoData.dimmSize[Dimm];

      if (DimmMemorySizeInMB < SMBIOS_TYPE17_USE_EXTENDED_SIZE) {
        SmbiosTableType17Data.Size = (UINT16) DimmMemorySizeInMB;
        SmbiosTableType17Data.ExtendedSize = 0;
      } else {
        SmbiosTableType17Data.Size = SMBIOS_TYPE17_USE_EXTENDED_SIZE;
        SmbiosTableType17Data.ExtendedSize = DimmMemorySizeInMB;
      }

      switch (SmbusBuffer[DDR_MTYPE_SPD_OFFSET] & DDR_MTYPE_SPD_MASK) {
        case DDR_MTYPE_SODIMM:
          SmbiosTableType17Data.FormFactor = MemoryFormFactorSodimm;
          break;

        case DDR_MTYPE_RDIMM:
        case DDR_MTYPE_MINI_RDIMM:
          SmbiosTableType17Data.FormFactor = MemoryFormFactorRimm;
          break;

        case DDR_MTYPE_UDIMM:
        case DDR_MTYPE_MICRO_DIMM:
        case DDR_MTYPE_MINI_UDIMM:
        default:
          SmbiosTableType17Data.FormFactor = MemoryFormFactorDimm;
      }

      //
      // Memory Type
      //
      switch  (mMemInfoHob->MemInfoData.ddrType) {
        case DDRType_DDR3:
        case DDRType_DDR3L:
        case DDRType_DDR3U:
        case DDRType_LPDDR3:
          SmbiosTableType17Data.MemoryType = MemoryTypeDdr3;
          break;
        default:
          SmbiosTableType17Data.MemoryType = 0x1E;
          break;
      }

      if (SmbiosTableType17Data.FormFactor == MemoryFormFactorRimm) {
        SmbiosTableType17Data.TypeDetail.Rambus = 1;
      } else {
        SmbiosTableType17Data.TypeDetail.Rambus = 0;
      }
      SmbiosTableType17Data.TypeDetail.Synchronous  = 1;

      //
      // Memory Freq
      //
      switch (mMemInfoHob->MemInfoData.ddrFreq){
        case FREQ_800:
          SmbiosTableType17Data.Speed = 800;
          break;
        case FREQ_1066:
          SmbiosTableType17Data.Speed = 1066;
          break;
        case FREQ_1333:
          SmbiosTableType17Data.Speed = 1333;
          break;
        case FREQ_1600:
          SmbiosTableType17Data.Speed = 1600;
          break;
        case FREQ_1866:
          SmbiosTableType17Data.Speed = 1866;
          break;
        case FREQ_2133:
          SmbiosTableType17Data.Speed = 2133;
          break;
        case FREQ_2666:
          SmbiosTableType17Data.Speed = 2666;
          break;
        case FREQ_3200:
          SmbiosTableType17Data.Speed = 3200;
          break;
        default:
          SmbiosTableType17Data.Speed = 0;
          break;
      }
      SmbiosTableType17Data.ConfiguredMemoryClockSpeed = SmbiosTableType17Data.Speed;
    } else {
      //
      // Memory is not Populated in this slot.
      //
      SmbiosTableType17Strings.DeviceLocator       = DimmToDevLocator[Dimm];
      SmbiosTableType17Strings.BankLocator         = DimmToBankLocator[Dimm];

      SmbiosTableType17Strings.Manufacturer        = NULL;
      SmbiosTableType17Strings.SerialNumber        = NULL;
      SmbiosTableType17Strings.PartNumber          = NULL;

      SmbiosTableType17Data.Manufacturer           = NO_STRING_AVAILABLE;
      SmbiosTableType17Data.SerialNumber           = NO_STRING_AVAILABLE;
      SmbiosTableType17Data.AssetTag               = STRING_3;
      SmbiosTableType17Data.PartNumber             = NO_STRING_AVAILABLE;

      SmbiosTableType17Data.TotalWidth             = 0;
      SmbiosTableType17Data.DataWidth              = 0;
      SmbiosTableType17Data.Size                   = 0;
      SmbiosTableType17Data.FormFactor             = MemoryFormFactorDimm;
      SmbiosTableType17Data.MemoryType             = MemoryTypeUnknown;
      SmbiosTableType17Data.TypeDetail.Rambus      = 0;
      SmbiosTableType17Data.TypeDetail.Synchronous = 0;
      SmbiosTableType17Data.Speed                  = 0;
      SmbiosTableType17Data.Attributes             = 0;
      SmbiosTableType17Data.ExtendedSize           = 0;
    }

    //
    // Generate Memory Device info (Type 17)
    //
    Status = AddSmbiosEntry (
               (EFI_SMBIOS_TABLE_HEADER *) &SmbiosTableType17Data,
               (CHAR8 **) &SmbiosTableType17Strings,
               SMBIOS_TYPE17_NUMBER_OF_STRINGS,
               &SmbiosHandle);
    if (EFI_ERROR (Status)) {
      goto CleanAndExit;
    }
  }
CleanAndExit:
  FreePool (SmbusBuffer);
  FreePool (StringBufferStart);
  DEBUG ((DEBUG_INFO, "\nInstall SMBIOS Table Type 17"));

  return Status;
}

