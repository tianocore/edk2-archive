/** @file
  To retrieve various platform info data for Setup menu.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformSetupDxe.h"
#include <Protocol/LegacyBios.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/MpService.h>
#include <Protocol/SeCOperation.h>
#include <Protocol/CpuInfo.h>
#include <Library/IoLib.h>
#include <Library/PlatformSecureDefaultsLib.h>
#include <Library/I2CLib.h>
#include <Library/PmcIpcLib.h>
#include <CpuAccess.h>
#include <Library/SteppingLib.h>
#include <Guid/MemoryConfigData.h>
#include <Guid/HobList.h>
#include <Library/HeciMsgLib.h>
#include <Library/SeCLib.h>
#include <Library/PciLib.h>
#include <Protocol/Tcg2Protocol.h>
#undef SET

#include <Include/KscLib.h>
#include <Guid/PlatformInfo.h>
#include <BoardFunctionsDxe.h>

#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include <MmrcData.h>

#include "ScAccess.h"
#include "SetupMode.h"


#define LEFT_JUSTIFY  0x01
#define PREFIX_SIGN   0x02
#define PREFIX_BLANK  0x04
#define COMMA_TYPE    0x08
#define LONG_TYPE     0x10
#define PREFIX_ZERO   0x20

BOOLEAN                         mSetupInfoDone = FALSE;
EFI_EXP_BASE10_DATA             mProcessorFrequency;
EFI_EXP_BASE10_DATA             mProcessorFsbFrequency;

EFI_GUID                        mProcessorProducerGuid;
EFI_HII_HANDLE                  mHiiHandle;
SYSTEM_CONFIGURATION            mSystemConfiguration;
EFI_PLATFORM_INFO_HOB           *mPlatformInfo;

#define memset SetMem

UINT16                mMemorySpeed              = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelASlot0  = 0;
UINT16                mMemorySpeedChannelASlot0 = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelASlot1  = 0;
UINT16                mMemorySpeedChannelASlot1 = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelBSlot0  = 0;
UINT16                mMemorySpeedChannelBSlot0 = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelBSlot1  = 0;
UINT16                mMemorySpeedChannelBSlot1 = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelCSlot0  = 0;
UINT16                mMemorySpeedChannelCSlot0 = 0xffff;
EFI_PHYSICAL_ADDRESS  mMemorySizeChannelCSlot1  = 0;
UINT16                mMemorySpeedChannelCSlot1 = 0xffff;
UINTN                 mMemoryMode               = 0xff;

#define CHARACTER_NUMBER_FOR_VALUE  30
typedef struct {
  EFI_STRING_TOKEN            MemoryDeviceLocator;
  EFI_STRING_TOKEN            MemoryBankLocator;
  EFI_STRING_TOKEN            MemoryManufacturer;
  EFI_STRING_TOKEN            MemorySerialNumber;
  EFI_STRING_TOKEN            MemoryAssetTag;
  EFI_STRING_TOKEN            MemoryPartNumber;
  EFI_INTER_LINK_DATA         MemoryArrayLink;
  EFI_INTER_LINK_DATA         MemorySubArrayLink;
  UINT16                      MemoryTotalWidth;
  UINT16                      MemoryDataWidth;
  UINT64                      MemoryDeviceSize;
  EFI_MEMORY_FORM_FACTOR      MemoryFormFactor;
  UINT8                       MemoryDeviceSet;
  EFI_MEMORY_ARRAY_TYPE       MemoryType;
  EFI_MEMORY_TYPE_DETAIL      MemoryTypeDetail;
  UINT16                      MemorySpeed;
  EFI_MEMORY_STATE            MemoryState;
} EFI_MEMORY_ARRAY_LINK;


typedef struct {
  EFI_PHYSICAL_ADDRESS        MemoryArrayStartAddress;
  EFI_PHYSICAL_ADDRESS        MemoryArrayEndAddress;
  EFI_INTER_LINK_DATA         PhysicalMemoryArrayLink;
  UINT16                      MemoryArrayPartitionWidth;
} EFI_MEMORY_ARRAY_START_ADDRESS;


/**
  VSPrint worker function that prints a Value as a decimal number in Buffer

  @param[in] Buffer       Location to place ASCII decimal number string of Value.
  @param[in] Value        Decimal value to convert to a string in Buffer.
  @param[in] Flags        Flags to use in printing decimal string, see file header for details.
  @param[in] Width        Width of hex value.

  @return                 Number of characters printed.

**/
UINTN
EfiValueToString (
  IN  OUT CHAR16  *Buffer,
  IN  INT64       Value,
  IN  UINTN       Flags,
  IN  UINTN       Width
  )
{
  CHAR16    TempBuffer[CHARACTER_NUMBER_FOR_VALUE];
  CHAR16    *TempStr;
  CHAR16    *BufferPtr;
  UINTN     Count;
  UINTN     ValueCharNum;
  UINTN     Remainder;
  CHAR16    Prefix;
  UINTN     Index;
  BOOLEAN   ValueIsNegative;
  UINT64    TempValue;

  TempStr         = TempBuffer;
  BufferPtr       = Buffer;
  Count           = 0;
  ValueCharNum    = 0;
  ValueIsNegative = FALSE;

  if (Width > CHARACTER_NUMBER_FOR_VALUE - 1) {
    Width = CHARACTER_NUMBER_FOR_VALUE - 1;
  }

  if (Value < 0) {
    Value           = -Value;
    ValueIsNegative = TRUE;
  }

  do {
    TempValue = Value;
    Value = (INT64) DivU64x32 ((UINT64) Value, 10);
    Remainder = (UINTN) ((UINT64) TempValue - 10 * Value);
    *(TempStr++) = (CHAR16) (Remainder + '0');
    ValueCharNum++;
    Count++;
    if ((Flags & COMMA_TYPE) == COMMA_TYPE) {
      if (ValueCharNum % 3 == 0 && Value != 0) {
        *(TempStr++) = ',';
        Count++;
      }
    }
  } while (Value != 0);

  if (ValueIsNegative) {
    *(TempStr++) = '-';
    Count++;
  }

  if ((Flags & PREFIX_ZERO) && !ValueIsNegative) {
    Prefix = '0';
  } else {
    Prefix = ' ';
  }

  Index = Count;
  if (!(Flags & LEFT_JUSTIFY)) {
    for (; Index < Width; Index++) {
      *(TempStr++) = Prefix;
    }
  }

  //
  // Reverse temp string into Buffer.
  //
  if (Width > 0 && (UINTN) (TempStr - TempBuffer) > Width) {
    TempStr = TempBuffer + Width;
  }
  Index = 0;
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
    Index++;
  }

  *BufferPtr = 0;
  return Index;
}

static CHAR16 mHexStr[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
                            L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };

/**
  VSPrint worker function that prints a Value as a hex number in Buffer

  @param[in] Buffer        Location to place ASCII hex string of Value.
  @param[in] Value         Hex value to convert to a string in Buffer.
  @param[in] Flags         Flags to use in printing Hex string, see file header for details.
  @param[in] Width         Width of hex value.

  @return                  Number of characters printed.

**/
UINTN
EfiValueToHexStr (
  IN  OUT CHAR16  *Buffer,
  IN  UINT64      Value,
  IN  UINTN       Flags,
  IN  UINTN       Width
  )
{
  CHAR16  TempBuffer[CHARACTER_NUMBER_FOR_VALUE];
  CHAR16  *TempStr;
  CHAR16  Prefix;
  CHAR16  *BufferPtr;
  UINTN   Count;
  UINTN   Index;

  TempStr   = TempBuffer;
  BufferPtr = Buffer;

  //
  // Count starts at one since we will null terminate. Each iteration of the
  // loop picks off one nibble. Oh yea TempStr ends up backwards
  //
  Count = 0;

  if (Width > CHARACTER_NUMBER_FOR_VALUE - 1) {
    Width = CHARACTER_NUMBER_FOR_VALUE - 1;
  }

  do {
    Index = ((UINTN)Value & 0xf);
    *(TempStr++) = mHexStr[Index];
    Value = RShiftU64 (Value, 4);
    Count++;
  } while (Value != 0);

  if (Flags & PREFIX_ZERO) {
    Prefix = '0';
  } else {
    Prefix = ' ';
  }

  Index = Count;
  if (!(Flags & LEFT_JUSTIFY)) {
    for (; Index < Width; Index++) {
      *(TempStr++) = Prefix;
    }
  }

  //
  // Reverse temp string into Buffer.
  //
  if (Width > 0 && (UINTN) (TempStr - TempBuffer) > Width) {
    TempStr = TempBuffer + Width;
  }
  Index = 0;
  while (TempStr != TempBuffer) {
    *(BufferPtr++) = *(--TempStr);
    Index++;
  }

  *BufferPtr = 0;
  return Index;
}


/**
  Converts MAC address to Unicode string.
  The value is 64-bit and the resulting string will be 12
  digit hex number in pairs of digits separated by dashes.

  @param[in] String         String that will contain the value
  @param[in] MacAddr        MAC Address
  @param[in] AddrSize       The size of MAC Address.

  @return                   The Unicode String.

**/
CHAR16 *
StrMacToString (
  OUT CHAR16              *String,
  IN  EFI_MAC_ADDRESS     *MacAddr,
  IN  UINT32              AddrSize
  )
{
  UINT32  i;

  for (i = 0; i < AddrSize; i++) {
    EfiValueToHexStr (
      &String[2 * i],
      MacAddr->Addr[i] & 0xFF,
      PREFIX_ZERO,
      2
      );
  }

  //
  // Terminate the string.
  //
  String[2 * AddrSize] = L'\0';

  return String;
}


VOID
UpdateLatestBootTime (
  )
{
  UINTN                        VarSize;
  EFI_STATUS                   Status;
  UINT64                       TimeValue;
  CHAR16                       Buffer[40];

  if (mSystemConfiguration.LogBootTime != 1) {
    return;
  }

  VarSize = sizeof (TimeValue);
  Status = gRT->GetVariable (
                  BOOT_TIME_NAME,
                  &gEfiNormalSetupGuid,
                  NULL,
                  &VarSize,
                  &TimeValue
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  UnicodeSPrint (Buffer, sizeof (Buffer), L"%d ms", (UINT32) TimeValue);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_LOG_BOOT_TIME_VALUE), Buffer, NULL);
}


/**
  Get Cache Type for the specified Cache. This function is invoked when there is data records
  available in the Data Hub.

  @param[in] Instance          The instance number of the subclass with the same ProducerName..
  @param[in] SubInstance       The instance number of the RecordType for the same Instance.
  @param[in] CacheType         Cache type, see definition of EFI_CACHE_TYPE_DATA.

  @retval    EFI_STATUS

**/
EFI_STATUS
GetCacheType (
  IN  UINT16                  Instance,
  IN  UINT16                  SubInstance,
  IN  EFI_CACHE_TYPE_DATA*    CacheType
  )
{
  EFI_STATUS                  Status;
  EFI_DATA_HUB_PROTOCOL       *DataHub;
  EFI_DATA_RECORD_HEADER      *Record;
  UINT64                      MonotonicCount;
  EFI_CACHE_VARIABLE_RECORD*  CacheVariableRecord;
  EFI_SUBCLASS_TYPE1_HEADER   *DataHeader;

  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **) &DataHub);
  ASSERT_EFI_ERROR (Status);

  //
  // Get all available data records from data hub
  //
  MonotonicCount = 0;
  Record = NULL;

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (!EFI_ERROR (Status)) {
      if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
        DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
        if (CompareGuid(&Record->DataRecordGuid, &gEfiCacheSubClassGuid) &&
          (DataHeader->RecordType == CacheTypeRecordType) &&
          (DataHeader->Instance == Instance) &&
          (DataHeader->SubInstance == SubInstance)) {
          CacheVariableRecord     = (EFI_CACHE_VARIABLE_RECORD  *) (DataHeader + 1);
          if(CacheType){
            *CacheType = CacheVariableRecord->CacheType;
            return EFI_SUCCESS;
          }
        }
      }
    }
  } while (!EFI_ERROR (Status) && (MonotonicCount != 0));

  return EFI_NOT_FOUND;
}


VOID
PrepareSetupInformation (
  )
{
  EFI_STATUS                      Status;
  EFI_DATA_HUB_PROTOCOL           *DataHub;
  EFI_DATA_RECORD_HEADER          *Record;
  UINT8                           *SrcData;
  UINT32                          SrcDataSize;
  EFI_SUBCLASS_TYPE1_HEADER       *DataHeader;
  CHAR16                          *NewString;
  CHAR16                          *NewString2;
  STRING_REF                      TokenToUpdate;
  UINTN                           Index;
  UINT16                          EeState;
  UINTN                           DataOutput;
  CPU_INFO                        *GetCpuInfo;
  CPU_INFO_PROTOCOL               *DxeCpuInfo;
  MSR_REGISTER                    MicroCodeVersion;
  EFI_MEMORY_ARRAY_START_ADDRESS  *MemoryArray;
  EFI_MEMORY_ARRAY_LINK           *MemoryArrayLink;
  UINT64                          MonotonicCount;
  CHAR16                          Version[100];         //Assuming that strings are < 100 UCHAR
  CHAR16                          ReleaseDate[100];     //Assuming that strings are < 100 UCHAR
  CHAR16                          ReleaseTime[100];     //Assuming that strings are < 100 UCHAR
  VOID                            *HobList;
  VOID                            *HobData;
  UINTN                           DataSize;
  MRC_PARAMS_SAVE_RESTORE         *MemInfoHob;
  UINT32                          MrcVersion;
  CHAR16                          Buffer[0x10];
  UINT16                          MemFreqTable[11] = {800, 1066, 1333, 1600, 1866, 2133, 2400, 2666, 3200, 0000, 3777};
  HobData                         = NULL;
  MemInfoHob                      = NULL;
  UINT32                          Zid;
  UINT16                          ZidUnicode[11];

  NewString = AllocateZeroPool (0x100);
  if (NewString == NULL)
    return;

  NewString2 = AllocateZeroPool (0x100);
  if (NewString2 == NULL)
    return;

  SetMem (Version, sizeof (Version), 0);
  SetMem (ReleaseDate, sizeof (ReleaseDate), 0);
  SetMem (ReleaseTime, sizeof (ReleaseTime), 0);

  //
  // Initialize EE state for not to show EE related setup options
  //
  EeState = 0;

  //
  // Get the Data Hub Protocol. Assume only one instance
  //
  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **) &DataHub);
  ASSERT_EFI_ERROR(Status);

  //
  // Get all available data records from data hub
  //
  MonotonicCount = 0;
  Record = NULL;

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (!EFI_ERROR (Status)) {
      if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
        DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
        SrcData     = (UINT8 *) (DataHeader + 1);
        SrcDataSize = Record->RecordSize - Record->HeaderSize - sizeof (EFI_SUBCLASS_TYPE1_HEADER);

        //
        // Memory
        //
        if (CompareGuid (&Record->DataRecordGuid, &gEfiMemorySubClassGuid)) {
          switch (DataHeader->RecordType) {
            case EFI_MEMORY_ARRAY_LINK_RECORD_NUMBER:
              MemoryArrayLink = (EFI_MEMORY_ARRAY_LINK *) SrcData;
              if (MemoryArrayLink->MemorySpeed > 0) {
                //
                // Save the lowest speed memory module
                //
                if (MemoryArrayLink->MemorySpeed < mMemorySpeed) {
                  mMemorySpeed = MemoryArrayLink->MemorySpeed;
                }
                switch (DataHeader->SubInstance) {
                  case 1:
                    mMemorySpeedChannelASlot0 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelASlot0 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  case 2:
                    mMemorySpeedChannelASlot1 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelASlot1 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  case 3:
                    mMemorySpeedChannelBSlot0 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelBSlot0 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  case 4:
                    mMemorySpeedChannelBSlot1 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelBSlot1 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  case 5:
                    mMemorySpeedChannelCSlot0 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelCSlot0 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  case 6:
                    mMemorySpeedChannelCSlot1 = MemoryArrayLink->MemorySpeed;
                    mMemorySizeChannelCSlot1 = MemoryArrayLink->MemoryDeviceSize;
                    break;

                  default:
                    break;
                  }
              }
              break;

            case EFI_MEMORY_ARRAY_START_ADDRESS_RECORD_NUMBER:
              MemoryArray = (EFI_MEMORY_ARRAY_START_ADDRESS *) SrcData;
              if (MemoryArray->MemoryArrayEndAddress - MemoryArray->MemoryArrayStartAddress) {
                DataOutput = (UINTN) RShiftU64 ((MemoryArray->MemoryArrayEndAddress - MemoryArray->MemoryArrayStartAddress + 1), 20);
                EfiValueToString (NewString, DataOutput / 1024, PREFIX_ZERO, 0);
                if (DataOutput % 1024) {
                  StrCatS (NewString, 0x100 / sizeof (CHAR16), L".");
                  DataOutput = ((DataOutput % 1024) * 1000) / 1024;
                  while (!(DataOutput % 10))
                    DataOutput = DataOutput / 10;
                  EfiValueToString (NewString2, DataOutput, PREFIX_ZERO, 0);
                  StrCatS (NewString, 0x100 / sizeof (CHAR16), NewString2);
                }
                StrCatS (NewString, 0x100 / sizeof (CHAR16), L" GB");
                TokenToUpdate = (STRING_REF) STR_TOTAL_MEMORY_SIZE_VALUE;
                HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
              }
              break;

            default:
              break;
          }
        }
      }
    }
  } while (!EFI_ERROR (Status) && (MonotonicCount != 0));

  //
  // Update Cache data
  //
  //
  // Locate DxeCpuInfo protocol instance and gather CPU information
  //
  Status = gBS->LocateProtocol (&gCpuInfoProtocolGuid, NULL, (VOID **) &DxeCpuInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,"Failed to locate DxeCpuInfo Protocol\n"));
    return;
  }

  GetCpuInfo               = DxeCpuInfo->CpuInfo;
  GetCpuInfo->BrandString  = DxeCpuInfo->CpuInfo->BrandString;
  GetCpuInfo->CacheInfo    = DxeCpuInfo->CpuInfo->CacheInfo;

  for (Index = 0; Index <= GetCpuInfo->MaxCacheSupported; ++ Index){
    switch (GetCpuInfo->CacheInfo[Index].Level) {
      case 1:
        if (GetCpuInfo->CacheInfo[Index].Type == 1) {
          EfiValueToString (NewString, GetCpuInfo->CacheInfo[Index].Size, PREFIX_ZERO, 0);
          StrCatS (NewString, 0x100 / sizeof (CHAR16) , L" KB");
          TokenToUpdate = (STRING_REF) STR_PROCESSOR_L1_DATA_CACHE_VALUE;
          HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
        } else if (GetCpuInfo->CacheInfo[Index].Type == 2) {
          EfiValueToString (NewString, GetCpuInfo->CacheInfo[Index].Size, PREFIX_ZERO, 0);
          StrCatS (NewString, 0x100 / sizeof (CHAR16), L" KB");
          TokenToUpdate = (STRING_REF) STR_PROCESSOR_L1_INSTR_CACHE_VALUE;
          HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
       }
        break;

      case 2:
        EfiValueToString (NewString, GetCpuInfo->CacheInfo[Index].Size, PREFIX_ZERO, 0);
        StrCatS (NewString, 0x100 / sizeof (CHAR16), L" KB");
        TokenToUpdate = (STRING_REF) STR_PROCESSOR_L2_CACHE_VALUE;
        HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
        break;

      case 3:
        break;

      case 4:
        break;
    }
  }

  //
  // Update processor information
  //
  EfiValueToString (NewString, GetCpuInfo->IntendedFreq, PREFIX_ZERO, 0);
  StrCatS (NewString, 0x100 / sizeof (CHAR16), L" MHz");
  TokenToUpdate = (STRING_REF) STR_PROCESSOR_SPEED_VALUE;
  HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);

  AsciiStrToUnicodeStr (GetCpuInfo->BrandString, NewString);
  TokenToUpdate = (STRING_REF) STR_PROCESSOR_VERSION_VALUE;
  HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);

  //
  // Get PCD ZID and update String
  //
  Zid = PcdGet32 (PcdIfwiZid);
  TokenToUpdate = (STRING_REF) STR_ZID_VERSION_VALUE;
  UnicodeValueToString (ZidUnicode, PREFIX_ZERO | RADIX_HEX, Zid, 8);
  HiiSetString (mHiiHandle, TokenToUpdate, ZidUnicode, NULL);

  //
  // Update the Microcode Revision
  //
  MicroCodeVersion.Qword = AsmReadMsr64 (0x8b);
  EfiValueToHexStr (NewString, MicroCodeVersion.Dwords.High, PREFIX_ZERO, 8);
  TokenToUpdate = (STRING_REF) STR_PROCESSOR_MICROCODE_VALUE;
  HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);

  Status = GetBiosVersionDateTime (Version, ReleaseDate, ReleaseTime);

  DEBUG ((EFI_D_INFO, "GetBiosVersionDateTime :%s %s %s \n", Version, ReleaseDate, ReleaseTime));
  if (!EFI_ERROR (Status)) {
    UINTN         Length = 0;
    CHAR16        *BuildDateTime;

    Length = StrLen (ReleaseDate) + StrLen (ReleaseTime);

    BuildDateTime = AllocateZeroPool ((Length + 2) * sizeof (CHAR16));
    if (BuildDateTime == NULL)
      return;

    StrCpyS (BuildDateTime, (Length + 2), ReleaseDate);
    StrCatS (BuildDateTime, (Length + 2), L" ");
    StrCatS (BuildDateTime, (Length + 2), ReleaseTime);

    TokenToUpdate = (STRING_REF) STR_BIOS_VERSION_VALUE;
    DEBUG ((EFI_D_INFO, "update STR_BIOS_VERSION_VALUE\n"));
    HiiSetString (mHiiHandle, TokenToUpdate, Version, NULL);

    TokenToUpdate = (STRING_REF) STR_BIOS_BUILD_TIME_VALUE;
    DEBUG ((EFI_D_INFO, "update STR_BIOS_BUILD_TIME_VALUE\n"));
    HiiSetString (mHiiHandle, TokenToUpdate, BuildDateTime, NULL);
  }

  //
  // Update UEFI Core Version in Main Page
  //
  UnicodeSPrint (Buffer, sizeof (Buffer), L"UEFI v%d.%02d", (gST->Hdr.Revision & 0xFFFF0000) >> 16, (gST->Hdr.Revision & 0x0000FFFF));
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_CORE_VERSION_VALUE), Buffer, NULL);

  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
#ifndef FSP_WRAPPER_FLAG
  if ((HobList = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobList)) != NULL) {
    HobData = GET_GUID_HOB_DATA (HobList);
    DataSize = GET_GUID_HOB_DATA_SIZE (HobList);
    MemInfoHob = (MRC_PARAMS_SAVE_RESTORE *) HobData;
  }
#else
  if ((HobList = GetNextGuidHob (&gFspNonVolatileStorageHobGuid, HobList)) != NULL) {
    HobData = GET_GUID_HOB_DATA (HobList);
    DataSize = GET_GUID_HOB_DATA_SIZE (HobList);
    MemInfoHob = (MRC_PARAMS_SAVE_RESTORE *) HobData;
  }
#endif

  //
  // Calculate and update memory speed display in Main Page
  //
  if (!EFI_ERROR (Status) && NULL != HobData ) {
    EfiValueToString (NewString, MemFreqTable[(UINT8) (MemInfoHob->CurrentFrequency)], PREFIX_ZERO, 0);
    StrCatS (NewString, 0x100 / sizeof (CHAR16), L" MHz");
    TokenToUpdate = (STRING_REF) STR_SYSTEM_MEMORY_SPEED_VALUE;
    HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
    DEBUG ((EFI_D_INFO, "update memory speed:MRC_ID = %d, %dMHz\n", MemInfoHob->CurrentFrequency, MemFreqTable[(UINT8)(MemInfoHob->CurrentFrequency)]));
  }

  //
  // update memory size display in Main Page
  //
  if (!EFI_ERROR (Status) && NULL != HobData ) {
    EfiValueToString (NewString, (MemInfoHob->SystemMemorySize / 1024), PREFIX_ZERO, 0);
    StrCatS (NewString, 0x100 / sizeof (CHAR16), L" GB");
    TokenToUpdate = (STRING_REF) STR_TOTAL_MEMORY_SIZE_VALUE;
    HiiSetString (mHiiHandle, TokenToUpdate, NewString, NULL);
    DEBUG ((EFI_D_INFO, "update total memory size:%dMB, %dGB\n", MemInfoHob->SystemMemorySize, MemInfoHob->SystemMemorySize/1024));
  }

  //
  //Update MRC Version
  //
  if (!EFI_ERROR (Status) && NULL != HobData ) {
    MrcVersion = MemInfoHob->MrcVer >> 16;
    MrcVersion &= 0xffff;
    Index = EfiValueToString (Buffer, MrcVersion/256, PREFIX_ZERO, 0);
    StrCatS (Buffer, sizeof (Buffer) / sizeof (CHAR16), L".");
    EfiValueToString (Buffer + Index + 1, MrcVersion%256, PREFIX_ZERO, 0);
    HiiSetString (mHiiHandle,STRING_TOKEN (STR_MRC_VERSION_VALUE), Buffer, NULL);
    DEBUG ((EFI_D_INFO, "update MRC version:%x, %s\n", MemInfoHob->MrcVer, Buffer));
  }

  gBS->FreePool(NewString);
  gBS->FreePool(NewString2);

  return;
}


EFI_STATUS
UpdateAdditionalInformation (
  )
{
  EFI_STATUS                      Status;
  UINT64                          MonotonicCount;
  EFI_DATA_HUB_PROTOCOL           *DataHub;
  EFI_DATA_RECORD_HEADER          *Record;
  EFI_SUBCLASS_TYPE1_HEADER       *DataHeader;
  EFI_MISC_SYSTEM_MANUFACTURER    *SystemManufacturer;
  UINTN                           Size;

  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **) &DataHub);

  ASSERT_EFI_ERROR (Status);

  Size = 3;

  MonotonicCount  = 0;
  Record = NULL;
  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);

      if (CompareGuid(&Record->DataRecordGuid, &gEfiMiscSubClassGuid) &&
          (DataHeader->RecordType == EFI_MISC_SYSTEM_MANUFACTURER_RECORD_NUMBER)) {
        //
        // System Information
        //
        SystemManufacturer = (EFI_MISC_SYSTEM_MANUFACTURER *) (DataHeader + 1);

      }
    }
  } while (!EFI_ERROR (Status) && (MonotonicCount != 0));

  UpdateLatestBootTime ();

  return EFI_SUCCESS;
}


VOID
UpdateCPUInformation (
  )
{
  CHAR16                                Buffer[40];
  UINT16                                FamilyId;
  UINT8                                 Model;
  UINT8                                 SteppingId;
  UINT8                                 ProcessorType;
  EFI_STATUS                            Status;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  UINTN                                 MaximumNumberOfCPUs;
  UINTN                                 NumberOfEnabledCPUs;
  EFI_CPUID_REGISTER                    CpuidRegs;

  AsmCpuid (1, &(CpuidRegs.RegEax), &(CpuidRegs.RegEbx), &(CpuidRegs.RegEcx), &(CpuidRegs.RegEdx));
  FamilyId = ((CpuidRegs.RegEax >> 8 )& 0xF);
  Model = ((CpuidRegs.RegEax >> 4) & 0xF);
  SteppingId =  (CpuidRegs.RegEax & 0xF);
  ProcessorType = ((CpuidRegs.RegEax>> 12) & 0x3);
  //
  // we need raw Model data
  //
  Model = Model & 0xf;
  //
  // Family/Model/Step
  //
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%d/%d/%d", FamilyId,  Model, SteppingId);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_PROCESSOR_ID_VALUE), Buffer, NULL);

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Determine the number of processors
    //
    MpService->GetNumberOfProcessors (
                 MpService,
                 &MaximumNumberOfCPUs,
                 &NumberOfEnabledCPUs
                 );

    UnicodeSPrint (Buffer, sizeof (Buffer), L"%d", MaximumNumberOfCPUs);
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_PROCESSOR_CORE_VALUE), Buffer, NULL);
  }
}


EFI_STATUS
SearchChildHandle (
  EFI_HANDLE Father,
  EFI_HANDLE *Child
  )
{
  EFI_STATUS                           Status;
  UINTN                                HandleIndex;
  EFI_GUID                             **ProtocolGuidArray = NULL;
  UINTN                                ArrayCount;
  UINTN                                ProtocolIndex;
  UINTN                                OpenInfoCount;
  UINTN                                OpenInfoIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfo = NULL;
  UINTN                                HandleCount;
  EFI_HANDLE                           *HandleBuffer= NULL;

  //
  // Retrieve the list of all handles from the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    //
    // Retrieve the list of all the protocols on each handle
    //
    Status = gBS->ProtocolsPerHandle (
                    HandleBuffer[HandleIndex],
                    &ProtocolGuidArray,
                    &ArrayCount
                    );

    if (!EFI_ERROR (Status)) {
      for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
        Status = gBS->OpenProtocolInformation (
                        HandleBuffer[HandleIndex],
                        ProtocolGuidArray[ProtocolIndex],
                        &OpenInfo,
                        &OpenInfoCount
                        );

        if (!EFI_ERROR (Status)) {
          for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
            if (OpenInfo[OpenInfoIndex].AgentHandle == Father) {
              if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
                *Child = HandleBuffer[HandleIndex];
                Status = EFI_SUCCESS;
                goto TryReturn;
              }
            }
          }
          Status = EFI_NOT_FOUND;
        }
      }
      if (OpenInfo != NULL) {
        FreePool(OpenInfo);
        OpenInfo = NULL;
      }
    }
    FreePool (ProtocolGuidArray);
    ProtocolGuidArray = NULL;
  }

TryReturn:
  if (OpenInfo != NULL) {
    FreePool (OpenInfo);
    OpenInfo = NULL;
  }
  if (ProtocolGuidArray != NULL) {
    FreePool (ProtocolGuidArray);
    ProtocolGuidArray = NULL;
  }
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }
  return Status;
}


EFI_STATUS
JudgeHandleIsPCIDevice(
  EFI_HANDLE    Handle,
  UINT8         Device,
  UINT8         Funs
  )
{
  EFI_STATUS        Status;
  EFI_DEVICE_PATH   *DPath;
  EFI_DEVICE_PATH   *DevicePath;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DPath
                  );

  if (!EFI_ERROR (Status)) {
    DevicePath = DPath;
    while (!IsDevicePathEnd (DPath)) {
      if ((DPath->Type == HARDWARE_DEVICE_PATH) && (DPath->SubType == HW_PCI_DP)) {
        PCI_DEVICE_PATH   *PCIPath;

        PCIPath = (PCI_DEVICE_PATH *) DPath;
        DPath = NextDevicePathNode (DPath);
        if (IsDevicePathEnd (DPath) && (PCIPath->Device == Device) && (PCIPath->Function == Funs)) {
          return EFI_SUCCESS;
        }
      } else {
        DPath = NextDevicePathNode (DPath);
      }
    }
  }

  return EFI_UNSUPPORTED;
}


EFI_STATUS
GetDriverName (
  EFI_HANDLE   Handle,
  CHAR16       *Name
  )
{
  EFI_DRIVER_BINDING_PROTOCOL        *BindHandle = NULL;
  EFI_STATUS                         Status;
  UINT32                             Version;
  UINT32                             *Ptr;

  Status = gBS->OpenProtocol(
                  Handle,
                  &gEfiDriverBindingProtocolGuid,
                  (VOID**) &BindHandle,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Version = BindHandle->Version;
  Ptr = (UINT32 *) &Version;
  UnicodeSPrint (Name, 40, L"%d.%d.%04d", (*(Ptr) & 0xff000000) >> 24, (*(Ptr) & 0x00ff0000) >> 16, (*Ptr) & 0x0000ffff);

  return EFI_SUCCESS;
}


EFI_STATUS
GetGOPDriverName (
  CHAR16 *Name
  )
{
  UINTN          HandleCount;
  EFI_HANDLE     *Handles= NULL;
  UINTN          Index;
  EFI_STATUS     Status;
  EFI_HANDLE     Child = 0;

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiDriverBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );

  for (Index = 0; Index < HandleCount ; Index++) {
    Status = SearchChildHandle (Handles[Index], &Child);
    if (!EFI_ERROR (Status)) {
      Status = JudgeHandleIsPCIDevice (Child, 0x02, 0x00);
      if (!EFI_ERROR (Status)) {
        return GetDriverName (Handles[Index], Name);
      }
    }
  }

  return EFI_UNSUPPORTED;
}


EFI_STATUS
GetImageFwVersion (
  VOID
  )
{
  EFI_STATUS                  Status;
  FW_VERSION_CMD_RESP_DATA    MsgGetFwVersionRespData;
  UINT8                       Index;
  CHAR16                      Buffer[40];
  CHAR8                       *CseManName;
  CHAR8                       *IshManName;

  DEBUG ((EFI_D_INFO, "Executing GetImageFwVersion().\n"));
  SetMem (&MsgGetFwVersionRespData, sizeof (FW_VERSION_CMD_RESP_DATA), 0x0);
  SetMem (Buffer, sizeof (Buffer), 0x0);
  CseManName = "FTPR.man";  //CSE manifest file name
  IshManName = "ISHC.man";  //ISH manifest file name

  Status = HeciGetImageFwVerMsg (&MsgGetFwVersionRespData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < MAX_IMAGE_MODULE_NUMBER; Index++) {
    if (!CompareMem (MsgGetFwVersionRespData.ModuleEntries[Index].EntryName, CseManName, AsciiStrLen (CseManName))) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%d.%d.%d.%d",
        MsgGetFwVersionRespData.ModuleEntries[Index].Major,
        MsgGetFwVersionRespData.ModuleEntries[Index].Minor,
        MsgGetFwVersionRespData.ModuleEntries[Index].Hotfix,
        MsgGetFwVersionRespData.ModuleEntries[Index].Build);
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_TXE_FW_VALUE), Buffer, NULL);
    }

    if (!CompareMem (MsgGetFwVersionRespData.ModuleEntries[Index].EntryName, IshManName, AsciiStrLen (IshManName))) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%d.%d.%d.%d",
        MsgGetFwVersionRespData.ModuleEntries[Index].Major,
        MsgGetFwVersionRespData.ModuleEntries[Index].Minor,
        MsgGetFwVersionRespData.ModuleEntries[Index].Hotfix,
        MsgGetFwVersionRespData.ModuleEntries[Index].Build);
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_ISH_FW_VALUE), Buffer, NULL);
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
UpdatePlatformInformation (
  )
{
  UINT32                   MicroCodeVersion;
  CHAR16                   Buffer[40];
  UINT8                    IgdVbiosRevH;
  UINT8                    IgdVbiosRevL;
  UINT16                   Edx;
  EFI_IA32_REGISTER_SET    RegSet;
  EFI_LEGACY_BIOS_PROTOCOL *LegacyBios = NULL;
  EFI_STATUS               Status;
  UINT8                    CpuFlavor = 0;
  EFI_PEI_HOB_POINTERS     GuidHob;
  EFI_PLATFORM_INFO_HOB    *PlatformInfo = NULL;
  UINT8                    Data8;
  CHAR16                   Name[40];
  UINT32                   SocVersion;
  UINT8                    PMCVersion[8];
  EFI_CPUID_REGISTER       CpuidRegs;
  GET_BOARD_NAME           GetBoardNameFunc;

  Data8 = 0;
  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      PlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  //
  // Update VBIOS version in Setup
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (!EFI_ERROR (Status)) {
    RegSet.X.AX = 0x5f01;
    Status = LegacyBios->Int86 (LegacyBios, 0x10, &RegSet);
    ASSERT_EFI_ERROR(Status);

    //
    // Simulate int15 (AX = 5f01) handler
    //
    Edx = (UINT16) ((RegSet.E.EBX >> 16) & 0xffff);
    IgdVbiosRevH = (UINT8) (((Edx & 0x0F00) >> 4) | (Edx & 0x000F));
    IgdVbiosRevL = (UINT8) (((RegSet.X.BX & 0x0F00) >> 4) | (RegSet.X.BX & 0x000F));

    if ((IgdVbiosRevH == 0) && (IgdVbiosRevL == 0)) {
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_CHIP_IGD_VBIOS_REV_VALUE), L"N/A", NULL);
    } else {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%02X%02X", IgdVbiosRevH, IgdVbiosRevL);
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_CHIP_IGD_VBIOS_REV_VALUE), Buffer, NULL);
    }
  }

  Status = GetGOPDriverName (Name);

  if (!EFI_ERROR (Status)) {
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_GOP_VALUE), Name, NULL);
  }

  AsmCpuid (1, &(CpuidRegs.RegEax), &(CpuidRegs.RegEbx), &(CpuidRegs.RegEcx), &(CpuidRegs.RegEdx));
  CpuFlavor = (UINT8) ((CpuidRegs.RegEax) & 0x8) >> 3;

  switch (CpuFlavor) {
    case 0x0:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s (%01x)", L"BXT IVI", CpuFlavor);
      break;

    case 0x1:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s (%01x)", L"BXT Notebook/Desktop", CpuFlavor);
      break;

    default:
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s (%01x)", L"Unknown CPU", CpuFlavor);
      break;
  }
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_CPU_FLAVOR_VALUE), Buffer, NULL);

  if (PlatformInfo != NULL) {
    //
    // BoardId
    //
    DEBUG ((EFI_D_INFO,  "UpdatePlatformInformation(): calling GetBoardName()\n"));
    GetBoardNameFunc = (GET_BOARD_NAME) (UINTN) PcdGet64 (PcdGetBoardNameFunc);
    UnicodeSPrint (Buffer, sizeof (Buffer), GetBoardNameFunc (PlatformInfo->BoardId));
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_BOARD_ID_VALUE), Buffer, NULL);

    //
    // FAB ID is read from EC, 0x00 means FAB1
    //
    if (mPlatformInfo->BoardRev == 0x00) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"FAB A");
    } else if (mPlatformInfo->BoardRev == 0x01) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"FAB B");
    } else {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%s", L"FAB C");
    }
    HiiSetString (mHiiHandle,STRING_TOKEN(STR_FAB_ID_VALUE), Buffer, NULL);
  }
  //
  // Update SOC Version
  //
  switch (BxtStepping()) {
    case BxtPA0:
      SocVersion = 0xA0;
      break;

    case BxtPB0:
      SocVersion = 0xB0;
      break;

    case BxtPB1:
      SocVersion = 0xB1;
      break;

    case BxtPB2:
      SocVersion = 0xB2;
      break;

    default:
      SocVersion = 0x00;
      break;
  }
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", SocVersion);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_SOC_VALUE), Buffer, NULL);


  //
  // Microcode Revision
  //
  MicroCodeVersion = (UINT32) RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32);
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", MicroCodeVersion);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_PROCESSOR_MICROCODE_VALUE), Buffer, NULL);

  //
  // Punit Revision
  //
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_PUNIT_FW_VALUE), Buffer, NULL);

  //
  // PMC FW Revision
  //
  IpcSendCommandEx (IPC_CMD_ID_PMC_VER, IPC_SUBCMD_ID_PMC_VER_READ, PMCVersion, 8);
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%02x.%02x", PMCVersion[1], PMCVersion[0]);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_PMC_FW_VALUE), Buffer, NULL);

  //
  // CSE and ISH FW Revision
  //
  Status = GetImageFwVersion ();

  return EFI_SUCCESS;
}


#ifdef SATA_SUPPORT
/**
  Update SATA Drive size Strings for Setup and Boot order

  @param[in] NewString         Pointer to string.
  @param[in] DeviceSpeed       Speed of drive.

  @retval

**/
VOID
GetDeviceSpeedString (
  CHAR16                      *NewString,
  IN UINTN                    DeviceSpeed
  )
{
  if (DeviceSpeed == 0x01) {
    StrCatS (NewString, 0x100 / sizeof (CHAR16), L"1.5Gb/s");
  } else if (DeviceSpeed == 0x02) {
    StrCatS (NewString, 0x100 / sizeof (CHAR16), L"3.0Gb/s");
  } else if (DeviceSpeed == 0x03) {
    StrCatS (NewString, 0x100 / sizeof (CHAR16), L"6.0Gb/s");
  } else if (DeviceSpeed == 0x0) {
  }
}


UINT8
GetChipsetSataPortSpeed (
  UINTN        PortNum
  )
{
  UINT32                      DeviceSpeed;
  UINT8                       DeviceConfigStatus;
  UINT32                      IdeAhciBar;
  EFI_PHYSICAL_ADDRESS        MemBaseAddress = 0;
  UINT8                       FunNum;
  EFI_STATUS                  Status;

  DeviceSpeed = 0x01; // generation 1

  //
  // Allocate the AHCI BAR
  //
  FunNum = PCI_FUNCTION_NUMBER_SATA;
  MemBaseAddress = 0x0ffffffff;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_SATA_AHCI_BAR_ALIGNMENT,  // 2^11: 2K Alignment
                  V_SATA_AHCI_BAR_LENGTH,     // 2K Length
                  &MemBaseAddress,
                  mImageHandle,
                  NULL
                  );

  IdeAhciBar = MmioRead32 (
                 MmPciAddress (0,
                   0,
                   PCI_DEVICE_NUMBER_SATA,
                   FunNum,
                   R_SATA_AHCI_BAR
                   )
                 );
  IdeAhciBar &= 0xFFFFF800;
  DeviceConfigStatus = 0;
  if (IdeAhciBar == 0) {
    DeviceConfigStatus = 1;
    IdeAhciBar = (UINT32)MemBaseAddress;
    MmioWrite32 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_SATA, FunNum, R_SATA_AHCI_BAR), IdeAhciBar);
    MmioOr16 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_SATA, FunNum, R_SATA_COMMAND), B_SATA_COMMAND_MSE);
  }

  switch (PortNum) {
    case 0:
      DeviceSpeed = *(volatile UINT32 *) (UINTN) (IdeAhciBar + R_SATA_AHCI_P0SSTS);
      break;
    case 1:
      DeviceSpeed = *(volatile UINT32 *) (UINTN) (IdeAhciBar + R_SATA_AHCI_P1SSTS);
      break;
  }

  if (MemBaseAddress) {
    gDS->FreeMemorySpace (
           MemBaseAddress,
           V_SATA_AHCI_BAR_LENGTH
           );
  }

  if (DeviceConfigStatus) {
    IdeAhciBar = 0;
    MmioWrite32 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_SATA, FunNum, R_SATA_AHCI_BAR), IdeAhciBar);
  }

  DeviceSpeed = (UINT8)((DeviceSpeed >> 4) & 0x0F);

  return (UINT8)DeviceSpeed;
}


VOID
IdeDataFilter (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL      *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath, *DevicePathNode;
  PCI_DEVICE_PATH             *PciDevicePath;
  UINTN                       Index;
  UINT8                       Index1;
  UINT32                      BufferSize;
  UINT32                      DriveSize;
  UINT32                      IdeChannel;
  UINT32                      IdeDevice;
  EFI_ATA_IDENTIFY_DATA       *IdentifyDriveInfo;
  CHAR16                      *NewString;
  CHAR16                      SizeString[20];
  STRING_REF                  NameToUpdate;
  CHAR8                       StringBuffer[0x100];
  UINT32                      DeviceSpeed;
  UINTN                       PortNumber;

  //
  // Assume no line strings is longer than 256 bytes.
  //
  NewString = AllocateZeroPool (0x100);
  if (NewString == NULL)
    return;

  PciDevicePath = NULL;
  //
  // Fill IDE Infomation
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath
                    );
    ASSERT_EFI_ERROR (Status);

    DevicePathNode = DevicePath;
    while (!IsDevicePathEnd (DevicePathNode) ) {
      if  ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
            ( DevicePathSubType (DevicePathNode) == HW_PCI_DP)) {
        PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
        break;
      }
      DevicePathNode    = NextDevicePathNode (DevicePathNode);
    }

    if (PciDevicePath == NULL) {
      continue;
    }

    //
    // Check for onboard IDE
    //
    if (PciDevicePath->Device== PCI_DEVICE_NUMBER_SATA) {
      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDiskInfoProtocolGuid, (VOID **) &DiskInfo);
      ASSERT_EFI_ERROR (Status);

      Status = DiskInfo->WhichIde (
                           DiskInfo,
                           &IdeChannel,
                           &IdeDevice
                           );

      ASSERT_EFI_ERROR (Status);
      IdentifyDriveInfo = NULL;
      IdentifyDriveInfo = AllocatePool (sizeof (EFI_ATA_IDENTIFY_DATA));
      if (IdentifyDriveInfo == NULL)
        return;

      BufferSize = sizeof (EFI_ATA_IDENTIFY_DATA);
      Status = DiskInfo->Identify (
                           DiskInfo,
                           IdentifyDriveInfo,
                           &BufferSize
                           );
      ASSERT_EFI_ERROR(Status);

      //
      // Onboard SATA Devices
      //
      if (PciDevicePath->Function == PCI_FUNCTION_NUMBER_SATA) {
        if (IdeChannel == 0 && IdeDevice == 0) {
          NameToUpdate = (STRING_REF)STR_SATA0_NAME;
        } else if (IdeChannel == 1 && IdeDevice == 0) {
          NameToUpdate = (STRING_REF)STR_SATA1_NAME;
        } else {
          continue;
        }
      } else {
        continue;
      }

      ZeroMem (StringBuffer, sizeof (StringBuffer));
      CopyMem (StringBuffer, (CHAR8 *) &IdentifyDriveInfo->ModelName, sizeof (IdentifyDriveInfo->ModelName));
      SwapEntries(StringBuffer);
      AsciiStrToUnicodeStr(StringBuffer, NewString);

      //
      // Chap it off after 16 characters
      //
      NewString[16] = 0;

      //
      // For HardDisk append the size. Otherwise display atapi
      //
      if ((IdentifyDriveInfo->config & 0x8000) == 00) {
        //
        // 48 bit address feature set is supported, get maximum capacity
        //
        if ((IdentifyDriveInfo->command_set_supported_83 & 0x0400) == 0) {
          DriveSize = (((((IdentifyDriveInfo->user_addressable_sectors_hi << 16) +
                      IdentifyDriveInfo->user_addressable_sectors_lo) / 1000) * 512) / 1000);
        } else {
          DriveSize    = IdentifyDriveInfo->maximum_lba_for_48bit_addressing[0];
          for (Index1 = 1; Index1 < 4; Index1++) {
            //
            // Lower byte goes first: word[100] is the lowest word, word[103] is highest
            //
            DriveSize |= LShiftU64 (IdentifyDriveInfo->maximum_lba_for_48bit_addressing[Index1], 16 * Index1);
          }
          DriveSize = (UINT32) DivU64x32 (MultU64x32 (DivU64x32 (DriveSize, 1000), 512), 1000);
        }

        StrCatS (NewString, 0x100 / sizeof (CHAR16), L"(");
        EfiValueToString (SizeString, DriveSize / 1000, PREFIX_BLANK, 0);
        StrCatS (NewString, 0x100 / sizeof (CHAR16), SizeString);
        StrCatS (NewString, 0x100 / sizeof (CHAR16), L".");
        EfiValueToString (SizeString, (DriveSize % 1000) / 100, PREFIX_BLANK, 0);
        StrCatS (NewString, 0x100 / sizeof (CHAR16), SizeString);
        StrCatS (NewString, 0x100 / sizeof (CHAR16), L"GB");
      } else {
        StrCatS (NewString, 0x100 / sizeof (CHAR16), L"(ATAPI");
      }

      //
      // Update SPEED.
      //
      PortNumber = (IdeDevice << 1) + IdeChannel;
      DeviceSpeed = GetChipsetSataPortSpeed (PortNumber);

      if (DeviceSpeed) {
        StrCatS (NewString, 0x100 / sizeof (CHAR16), L"-");
        GetDeviceSpeedString (NewString, DeviceSpeed);
      }

      StrCatS (NewString, 0x100 / sizeof (CHAR16), L")");
      HiiSetString (mHiiHandle, NameToUpdate, NewString, NULL);

    }
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
  }

  gBS->FreePool (NewString);

  return;
}
#endif


VOID
UpdateSeCInformation (
  )
{
  EFI_STATUS               Status;
  CHAR16                   Buffer[20];
  GEN_GET_FW_CAPSKU        MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK  MsgGenGetFwCapsSkuAck;
  SECFWCAPS_SKU            RuleData;
  UINT32                   OEMTag;
  HECI_FWS_REGISTER        SeCFirmwareStatus;
  CHAR16                   *FwStatus;

  DEBUG ((EFI_D_INFO, "UpdateSeCInformation ++\n"));

  SetMem (Buffer, sizeof (Buffer), 0x0);

  //
  // Get Firmware Capability SKU
  //
  Status = HeciGetFwCapsSkuMsg (&MsgGenGetFwCapsSku, &MsgGenGetFwCapsSkuAck);
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (Buffer, sizeof (Buffer), L"%08X", MsgGenGetFwCapsSkuAck.Data.FWCapSku.Data);
  } else {
    StrCpyS (Buffer, sizeof (Buffer) / sizeof (CHAR16), L"N/A");
  }
  HiiSetString(mHiiHandle, STRING_TOKEN(STR_SEC_CAPABILITY_VALUE), Buffer, NULL);

  //
  // Get FW Feature Status Message
  //
  Status = HeciGetFwFeatureStateMsg (&RuleData);
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (Buffer, sizeof (Buffer), L"%08X", RuleData.Data);
  } else {
    StrCpyS (Buffer, sizeof (Buffer) / sizeof (CHAR16), L"N/A");
  }
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_SEC_FEATURE_VALUE), Buffer, NULL);

  //
  // Get OEM TAG message
  //
  Status = HeciGetOemTagMsg (&OEMTag);
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (Buffer, sizeof (Buffer), L"%08X", OEMTag);
  } else {
    StrCpyS (Buffer, sizeof (Buffer) / sizeof (CHAR16), L"N/A");
  }
  HiiSetString(mHiiHandle, STRING_TOKEN(STR_SEC_OEMTAG_VALUE), Buffer, NULL);

  SeCFirmwareStatus.ul = PciRead32 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_SEC_FW_STS0));
  switch (SeCFirmwareStatus.r.SeCOperationMode) {
    case SEC_OPERATION_MODE_NORMAL:
      FwStatus = L"Normal";
      break;

    case SEC_OPERATION_MODE_ALT_DISABLED:
      FwStatus = L"DEBUG MODE";
      break;

    case SEC_OPERATION_MODE_SOFT_TEMP_DISABLE:
      FwStatus = L"Soft Temporary Disable";
      break;

    default:
      FwStatus = L"Unknown";
      break;
  }
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_SEC_TEMP_DISABLE_PROMPT), FwStatus, NULL);

  DEBUG ((EFI_D_INFO, "UpdateSeCInformation --\n"));
}


VOID
UpdatePLInformation (
  )
{
  CHAR16                        Buffer[40];
  MSR_REGISTER                  PackagePowerLimitMsr;
  UINT16                        PowerLimit1;
  UINT16                        PowerLimit2;
  UINT16                        PowerUnitWatts;

  PowerUnitWatts = 256;

  PackagePowerLimitMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  PowerLimit1 = (UINT16) PackagePowerLimitMsr.Dwords.Low & POWER_LIMIT_MASK;
  PowerLimit1 /= PowerUnitWatts;
  PowerLimit2 = (UINT16) PackagePowerLimitMsr.Dwords.High & POWER_LIMIT_MASK;
  PowerLimit2 /= PowerUnitWatts;

  UnicodeSPrint (Buffer, sizeof (Buffer), L"%d", (UINT32) PowerLimit1);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_LONG_DUR_PWR_LIMIT_VALUE), Buffer, NULL);
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%d", (UINT32) PowerLimit2);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_SHORT_DUR_PWR_LIMIT_VALUE), Buffer, NULL);
}


EFI_STATUS
SaveTpmDeviceSelection (
  SYSTEM_CONFIGURATION        *SystemConfigPtr
  )
{
  EFI_STATUS                  Status;
  BOOLEAN                     PttEnabledState;
  BOOLEAN                     ResetRequired;
  EFI_INPUT_KEY               Key;
  CHAR16                      *StringBuffer1 = L"Perform TPM Device Selection is Changed.";
  CHAR16                      *StringBuffer2 = L"System will Restart!";
  CHAR16                      *StringBuffer3 = L"Press Enter Key To Continue";

  ResetRequired = FALSE;
  //
  // TPM Device Selector Override
  //
  Status = PttHeciGetState (&PttEnabledState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SystemConfigPtr->TPM == TPM_PTT) {
    DEBUG ((EFI_D_INFO, "TPM Device Selection is PTT.\n"));
    if (!PttEnabledState) {
      ResetRequired = TRUE;
      PttHeciSetState (TRUE);
    }
  } else if ((SystemConfigPtr->TPM == TPM_DTPM_1_2) || (SystemConfigPtr->TPM == TPM_DTPM_2_0)) {
    DEBUG ((EFI_D_INFO, "TPM Device Selection is dTPM.\n"));
    if (PttEnabledState) {
      ResetRequired = TRUE;
      PttHeciSetState (FALSE);
    }
  } else if (SystemConfigPtr->TPM == TPM_DISABLE) {
    DEBUG ((EFI_D_INFO, "TPM Device Selection is Disable.\n"));
    if (PttEnabledState) {
      ResetRequired = TRUE;
      PttHeciSetState (FALSE);
    }
  }

  if (ResetRequired) {
    DEBUG ((EFI_D_INFO, "Trigger cold reset to take effect.\n"));
    //
    // Popup a menu to notice user
    //
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, NULL);
    } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return EFI_SUCCESS;
}


VOID
EFIAPI
SetupInfo (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       VarSize;
  EFI_PEI_HOB_POINTERS        GuidHob;

  if (mSetupInfoDone) {
    return;
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &mSystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  //
  // Update HOB variable for PCI resource information
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      mPlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  PrepareSetupInformation ();
  UpdateAdditionalInformation ();
  UpdatePlatformInformation ();
  UpdateCPUInformation ();
  UpdateSeCInformation ();

#ifdef SATA_SUPPORT
  IdeDataFilter ();
#endif
  UpdatePLInformation ();
  mSetupInfoDone = TRUE;

  return;
}


VOID
CheckSystemConfigLoad (
  SYSTEM_CONFIGURATION    *SystemConfigPtr
  )
{
  EFI_STATUS              Status;
  SEC_OPERATION_PROTOCOL  *SeCOp;
  SEC_INFOMATION          SeCInfo;
  UINT8                   SecureBoot;
  UINTN                   DataSize;

  DataSize = sizeof (SecureBoot);
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &SecureBoot
                  );

  if (EFI_ERROR (Status)) {
    SystemConfigPtr->SecureBoot = 0;
  } else {
    SystemConfigPtr->SecureBoot = SecureBoot;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSeCOperationProtocolGuid,
                  NULL,
                  (VOID **) &SeCOp
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = SeCOp->GetPlatformSeCInfo (&SeCInfo);
  SystemConfigPtr->SecEnable = (UINT8) SeCInfo.SeCEnable;
  SystemConfigPtr->SeCOpEnable = (UINT8) SystemConfigPtr->SecEnable;
  SystemConfigPtr->SeCModeEnable = (UINT8) SeCInfo.SeCOpEnable;
  SystemConfigPtr->SecFirmwareUpdate = (UINT8) SeCInfo.FwUpdate;
  SystemConfigPtr->SecFlashUpdate = (UINT8) SeCInfo.HmrfpoEnable;

}


//
// "SecureBootEnable" variable for the Secure boot feature enable/disable.
//
#define EFI_SECURE_BOOT_ENABLE_NAME      L"SecureBootEnable"
extern EFI_GUID gEfiSecureBootEnableDisableGuid;

EFI_STATUS
CheckTPMActivePcrBanks (
  IN UINT32   PcrBanks
  )
{
  UINT32                           CurrentActivePcrBanks;
  EFI_TCG2_PROTOCOL               *Tcg2Protocol;
  EFI_STATUS                       Status;

  Status = gBS->LocateProtocol (
                  &gEfiTcg2ProtocolGuid,
                  NULL,
                  (VOID **) &Tcg2Protocol
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = Tcg2Protocol->GetActivePcrBanks (Tcg2Protocol, &CurrentActivePcrBanks);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (CurrentActivePcrBanks == PcrBanks) {
    return EFI_SUCCESS;
  }

  Status = Tcg2Protocol->SetActivePcrBanks (Tcg2Protocol, PcrBanks);

  return Status;
}


VOID
CheckSystemConfigSave (
  SYSTEM_CONFIGURATION    *SystemConfigPtr
  )
{
  EFI_STATUS              Status;
  SEC_OPERATION_PROTOCOL  *SeCOp;
  SEC_INFOMATION          SeCInfo;
  UINT8                   SecureBootCfg;
  UINTN                   DataSize;
  BOOLEAN                 SecureBootNotFound;

  Status = gBS->LocateProtocol (
                  &gEfiSeCOperationProtocolGuid,
                  NULL,
                  (VOID **) &SeCOp
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  Status = SeCOp->GetPlatformSeCInfo (&SeCInfo);
  SeCInfo.SeCEnable = SystemConfigPtr->SecEnable;
  SeCInfo.FwUpdate = SystemConfigPtr->SecFirmwareUpdate;
  SeCInfo.HmrfpoEnable = SystemConfigPtr->SecFlashUpdate;

  Status = SeCOp->SetPlatformSeCInfo (&SeCInfo);

  //
  // Secure Boot configuration changes
  //
  DataSize = sizeof (SecureBootCfg);
  SecureBootNotFound = FALSE;
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_ENABLE_NAME,
                  &gEfiSecureBootEnableDisableGuid,
                  NULL,
                  &DataSize,
                  &SecureBootCfg
                  );

  if (EFI_ERROR (Status)) {
    SecureBootNotFound = TRUE;
  }

  if (SecureBootNotFound) {
    Status = gRT->GetVariable (
                    EFI_SECURE_BOOT_ENABLE_NAME,
                    &gEfiSecureBootEnableDisableGuid,
                    NULL,
                    &DataSize,
                    &SecureBootCfg
                    );
    ASSERT_EFI_ERROR (Status);
  }

  if ((SecureBootCfg) != SystemConfigPtr->SecureBoot) {
    SecureBootCfg = !SecureBootCfg;
    Status = gRT->SetVariable (
                    EFI_SECURE_BOOT_ENABLE_NAME,
                    &gEfiSecureBootEnableDisableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UINT8),
                    &SecureBootCfg
                    );
  }

  CheckTPMActivePcrBanks (SystemConfigPtr->TPMSupportedBanks);
  //
  // Save TPM device selection and trigger a cold reset to take effect.
  //
  SaveTpmDeviceSelection (SystemConfigPtr);
}

