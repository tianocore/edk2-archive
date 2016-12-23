/** @file
  Performance library instance mainly used by DxeCore.

  This library provides the performance measurement interfaces and initializes performance
  logging for DXE phase. It first initializes its private global data structure for
  performance logging and saves the performance GUIDed HOB passed from PEI phase.
  It initializes DXE phase performance logging by publishing the PerformanceEx Protocol,
  which are consumed by DxePerformanceLib to logging performance data in DXE phase.

  This library is mainly used by DxeCore to start performance logging to ensure that
  PerformanceEx Protocol is installed at the very beginning of DXE phase.

  This library also converts performance log to FPDT record, and report them to boot FPDT table.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Guid/Performance.h>
#include <Guid/FirmwarePerformance.h>
#include <PeiFirmwarePerformance.h>
#include <Guid/ZeroGuid.h>
#include <Guid/EventGroup.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePathToText.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/LocalApicLib.h>
#include <Library/UefiLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/DxeServicesLib.h>
#include <ExtendedFirmwarePerformanceData.h>

//
// Data buffer for FPDT performance records.
//
#define FIRMWARE_RECORD_BUFFER             0x10000
UINT8                                      *mPerformancePointer  = NULL;
UINTN                                      mPerformanceLength    = 0;
UINTN                                      mMaxPerformanceLength = 0;
UINT16                                     mBdsAttemptNumber     = 0;
BOOLEAN                                    mFpdtDataIsReport     = FALSE;
EFI_DEVICE_PATH_TO_TEXT_PROTOCOL           *mDevicePathToText    = NULL;
CHAR8                                      *mPlatformLanguage    = NULL;

/**
  Get a human readable module name and module guid for the given image handle.
  If module name can't be found, "" string will return.
  If module guid can't be found, Zero Guid will return.

  @param[in]    Handle                 Image handle or Controller handle.
  @param[out]   NameString             The ascii string will be filled into it. If not found, null string will return.
  @param[in]    BufferSize             Size of the input NameString buffer.
  @param[out]   ModuleGuid             Point to the guid buffer to store the got module guid value.

  @retval       EFI_SUCCESS            Successfully get module name and guid.
  @retval       EFI_INVALID_PARAMETER  The input parameter NameString is NULL.
  @retval       Other value            Module Name can't be got.

**/
EFI_STATUS
EFIAPI
GetModuleInfoFromHandle (
  IN EFI_HANDLE        Handle,
  OUT CHAR8            *NameString,
  IN UINTN             BufferSize,
  OUT EFI_GUID         *ModuleGuid OPTIONAL
  )
{
  EFI_STATUS                        Status;
  EFI_LOADED_IMAGE_PROTOCOL         *LoadedImage;
  EFI_DRIVER_BINDING_PROTOCOL       *DriverBinding;
  CHAR8                             *PdbFileName;
  EFI_GUID                          *TempGuid;
  UINTN                             StartIndex;
  UINTN                             Index;
  BOOLEAN                           ModuleGuidIsGet;
  UINTN                             StringSize;
  CHAR16                            *StringPtr;
  EFI_COMPONENT_NAME2_PROTOCOL      *ComponentName2;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvFilePath;

  if (NameString == NULL || BufferSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_INVALID_PARAMETER;
  LoadedImage     = NULL;
  ModuleGuidIsGet = FALSE;

  //
  // Initialize GUID as zero value.
  //
  TempGuid    = &gZeroGuid;

  //
  // Initialize it as "" string.
  //
  NameString[0] = 0;

  if (Handle != NULL) {
    //
    // Try Handle as ImageHandle.
    //
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiLoadedImageProtocolGuid,
                    (VOID **) &LoadedImage
                    );

    if (EFI_ERROR (Status)) {
      //
      // Try Handle as Controller Handle
      //
      Status = gBS->OpenProtocol (
                      Handle,
                      &gEfiDriverBindingProtocolGuid,
                      (VOID **) &DriverBinding,
                      NULL,
                      NULL,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );

      if (!EFI_ERROR (Status)) {
        //
        // Get Image protocol from ImageHandle
        //
        Status = gBS->HandleProtocol (
                        DriverBinding->ImageHandle,
                        &gEfiLoadedImageProtocolGuid,
                        (VOID **) &LoadedImage
                        );
      }
    }
  }

  if (!EFI_ERROR (Status) && LoadedImage != NULL) {
    //
    // Get Module Guid from DevicePath.
    //
    if (LoadedImage->FilePath != NULL &&
        LoadedImage->FilePath->Type == MEDIA_DEVICE_PATH &&
        LoadedImage->FilePath->SubType == MEDIA_PIWG_FW_FILE_DP
       ) {
      //
      // Determine GUID associated with module logging performance
      //
      ModuleGuidIsGet = TRUE;
      FvFilePath      = (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LoadedImage->FilePath;
      TempGuid        = &FvFilePath->FvFileName;
    }

    //
    // Method 1 Get Module Name from PDB string.
    //
    PdbFileName = PeCoffLoaderGetPdbPointer (LoadedImage->ImageBase);
    if (PdbFileName != NULL && BufferSize > 0) {
      StartIndex = 0;
      for (Index = 0; PdbFileName[Index] != 0; Index++) {
        if ((PdbFileName[Index] == '\\') || (PdbFileName[Index] == '/')) {
          StartIndex = Index + 1;
        }
      }

      //
      // Copy the PDB file name to our temporary string.
      // If the length is bigger than BufferSize, trim the redudant characters to avoid overflow in array boundary.
      //
      for (Index = 0; Index < BufferSize - 1; Index++) {
        NameString[Index] = PdbFileName[Index + StartIndex];
        if (NameString[Index] == 0 || NameString[Index] == '.') {
          NameString[Index] = 0;
          break;
        }
      }

      if (Index == BufferSize - 1) {
        NameString[Index] = 0;
      }
      //
      // Module Name is got.
      //
      goto Done;
    }
  }

  //
  // Method 2: Get the name string from ComponentName2 protocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiComponentName2ProtocolGuid,
                  (VOID **) &ComponentName2
                  );

  if (!EFI_ERROR (Status)) {
    //
    // Get the current platform language setting
    //
    if (mPlatformLanguage == NULL) {
      mPlatformLanguage = GetEfiGlobalVariable (L"PlatformLang");
    }
    if (mPlatformLanguage != NULL) {
      Status = ComponentName2->GetDriverName (
                                 ComponentName2,
                                 mPlatformLanguage != NULL ? mPlatformLanguage : "en-US",
                                 &StringPtr
                                 );
      if (!EFI_ERROR (Status)) {
        for (Index = 0; Index < BufferSize - 1 && StringPtr[Index] != 0; Index++) {
          NameString[Index] = (CHAR8) StringPtr[Index];
        }
        NameString[Index] = 0;
        //
        // Module Name is got.
        //
        goto Done;
      }
    }
  }

  if (ModuleGuidIsGet) {
    //
    // Method 3 Try to get the image's FFS UI section by image GUID
    //
    StringPtr  = NULL;
    StringSize = 0;
    Status = GetSectionFromAnyFv (
              TempGuid,
              EFI_SECTION_USER_INTERFACE,
              0,
              (VOID **) &StringPtr,
              &StringSize
              );

    if (!EFI_ERROR (Status)) {
      //
      // Method 3. Get the name string from FFS UI section
      //
      for (Index = 0; Index < BufferSize - 1 && StringPtr[Index] != 0; Index++) {
        NameString[Index] = (CHAR8) StringPtr[Index];
      }
      NameString[Index] = 0;
      FreePool (StringPtr);
    }
  }

Done:
  //
  // Copy Module Guid
  //
  if (ModuleGuid != NULL) {
    CopyGuid (ModuleGuid, TempGuid);
    if (CompareGuid (TempGuid, &gZeroGuid) && (Handle != NULL) && !ModuleGuidIsGet) {
      //
      // Handle is GUID
      //
      CopyGuid (ModuleGuid, (EFI_GUID *) Handle);
    }
  }

  return Status;
}


/**
  Convert performance token to FPDT record.

  @param[in]  IsStart                 TRUE if the performance log is start log.
  @param[in]  Token                   Pointer to environment specific context used
                                      to identify the component being measured.
  @param[out] RecordType              Type of FPDT record
  @param[out] Identifier              Identifier for FPDT records

  @retval     EFI_SUCCESS             The data was converted correctly.
  @retval     EFI_NOT_FOUND           No matched FPDT record is for the input Token.
  @retval     EFI_INVALID_PARAMETER   Input Pointer is NULL.

**/
EFI_STATUS
ConvertTokenToType (
  IN BOOLEAN                  IsStart,
  IN CONST CHAR8              *Token,
  OUT UINTN                   *FpdtRecType,
  OUT UINT32                  *Identifier
  )
{
  UINTN    RecordType;

  if (Token == NULL || FpdtRecType == NULL || Identifier == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RecordType  = STRING_EVENT_TYPE;

  //
  // Token to Type and Id.
  //
  if (AsciiStrCmp (Token, START_IMAGE_TOK) == 0) {
    if (IsStart) {
      *Identifier = MODULE_START_ID;
    } else {
      *Identifier = MODULE_END_ID;
    }
  } else if (AsciiStrCmp (Token, LOAD_IMAGE_TOK) == 0) {
    if (IsStart) {
      *Identifier = MODULE_LOADIMAGE_START_ID;
    } else {
      *Identifier = MODULE_LOADIMAGE_END_ID;
    }
  } else if (AsciiStrCmp (Token, DRIVERBINDING_START_TOK) == 0) {
    if (IsStart) {
      *Identifier = MODULE_DRIVERBINDING_START_ID;
    } else {
      *Identifier = MODULE_DRIVERBINDING_END_ID;
    }
  } else if (AsciiStrCmp (Token, DXE_TOK) == 0) {
    if (IsStart) {
      *Identifier = DXE_START_ID;
    } else {
      *Identifier = DXE_END_ID;
    }
  } else if (AsciiStrCmp (Token, DXE_CORE_DISP_INIT_TOK) == 0) {
    if (IsStart) {
      *Identifier = DXE_CORE_DISP_START_ID;
    } else {
      *Identifier = DXE_CORE_DISP_END_ID;
    }
  } else if (AsciiStrCmp (Token, COREDISPATCHER_TOK) == 0) {
    if (IsStart) {
      *Identifier = COREDISPATCHER_START_ID;
    } else {
      *Identifier = COREDISPATCHER_END_ID;
    }
  } else if (AsciiStrCmp (Token, BDS_TOK) == 0) {
    if (IsStart) {
      *Identifier = BDS_START_ID;
    } else {
      *Identifier = BDS_END_ID;
    }
  } else if (IsStart && AsciiStrCmp (Token, BDS_ATTEMPT_TOK) == 0) {
    RecordType = BDS_ATTEMPT_EVENT_TYPE;
  } else if (IsStart && AsciiStrCmp (Token, SMM_MODULE_TOK) == 0) {
    RecordType = RUNTIME_MODULE_TABLE_PTR_TYPE;
  } else if (IsStart && AsciiStrCmp (Token, BOOT_MODULE_TOK) == 0) {
    RecordType = BOOT_MODULE_TABLE_PTR_TYPE;
  } else if (AsciiStrCmp (Token, EVENT_REC_TOK) == 0) {
    RecordType = STRING_EVENT_TYPE;
  } else if (IsStart && AsciiStrCmp (Token, PLATFORM_BOOT_TOK) == 0) {
    RecordType = PLATFORM_BOOT_TABLE_PTR_TYPE;
  } else {
    return EFI_NOT_FOUND;
  }

  *FpdtRecType = RecordType;
  return EFI_SUCCESS;
}


/**
  Add performance log to FPDT boot record table.

  @param[in]  IsStart                 TRUE if the performance log is start log.
  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  Ticker                  64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             Add FPDT boot record.
  @retval     EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.
  @retval     EFI_ABORTED             No matched FPDT record.

**/
EFI_STATUS
InsertFpdtMeasurement (
  IN BOOLEAN      IsStart,
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       Ticker,
  IN UINT32       Identifier
  )
{
  EFI_GUID                     ModuleGuid;
  CHAR8                        ModuleName[STRING_EVENT_RECORD_NAME_LENGTH];
  UINTN                        FpdtRecType;
  EFI_STATUS                   Status;
  FPDT_RECORD_PTR              FpdtRecordPtr;
  FPDT_RECORD                  FpdtRecord;
  CHAR16                       BootOptionStr[0x10];
  UINT8                        *BootOption;
  EFI_DEVICE_PATH_PROTOCOL     *BootDevice;
  CHAR16                       *BootString;
  UINTN                        Length;
  BDS_ATTEMPT_RECORD           *BdsAttempRecordPtr;

  //
  // Convert performance log to FPDT record.
  //
  Status = ConvertTokenToType (IsStart, Token, &FpdtRecType, &Identifier);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mFpdtDataIsReport) {
    //
    // Cached FPDT data has been reported. Now, report FPDT record one by one.
    //
    FpdtRecordPtr.RecordHeader = &FpdtRecord.RecordHeader;
  } else {
    //
    // Check if pre-allocated buffer is full
    //
    if (mPerformanceLength + sizeof (FPDT_RECORD) > mMaxPerformanceLength) {
      mPerformancePointer = ReallocatePool (
                              mPerformanceLength,
                              mPerformanceLength + sizeof (FPDT_RECORD) + FIRMWARE_RECORD_BUFFER,
                              mPerformancePointer
                              );

      ASSERT (mPerformancePointer != NULL);
      mMaxPerformanceLength = mPerformanceLength + sizeof (FPDT_RECORD) + FIRMWARE_RECORD_BUFFER;
    }

    //
    // Covert buffer to FPDT Ptr Union type.
    //
    FpdtRecordPtr.RecordHeader = (EFI_ACPI_5_0_FPDT_PERFORMANCE_RECORD_HEADER *) (mPerformancePointer + mPerformanceLength);
  }
  FpdtRecordPtr.RecordHeader->Length = 0;

  switch (FpdtRecType) {
    case GUID_EVENT_TYPE:
    case STRING_EVENT_TYPE:
      //
      // Get real ticker
      //
      if (Ticker == 0) {
        Ticker = GetPerformanceCounter ();
      }

      //
      // Get Module Guid and Name based on ImageHandle
      //
      GetModuleInfoFromHandle ((EFI_HANDLE) Handle, ModuleName, sizeof (ModuleName), &ModuleGuid);
      if ((AsciiStrLen (ModuleName) != 0) || (Module != NULL && AsciiStrLen (Module) != 0)) {
        //
        // String Event Record
        //
        FpdtRecordPtr.StringEvent->Header.Type     = STRING_EVENT_TYPE;
        FpdtRecordPtr.StringEvent->Header.Length   = sizeof (STRING_EVENT_RECORD);
        FpdtRecordPtr.StringEvent->Header.Revision = RECORD_REVISION_1;
        FpdtRecordPtr.StringEvent->ProgressID      = (UINT16) Identifier;
        FpdtRecordPtr.StringEvent->ApicID          = GetApicId ();
        FpdtRecordPtr.StringEvent->Timestamp       = GetTimeInNanoSecond (Ticker);
        CopyMem (&FpdtRecordPtr.StringEvent->Guid, &ModuleGuid, sizeof (FpdtRecordPtr.StringEvent->Guid));
        if (AsciiStrLen (ModuleName) != 0) {
          AsciiStrnCpyS (FpdtRecordPtr.StringEvent->NameString, sizeof (FpdtRecordPtr.StringEvent->NameString), ModuleName, AsciiStrLen (ModuleName));
        } else {
          AsciiStrnCpyS (FpdtRecordPtr.StringEvent->NameString, sizeof (FpdtRecordPtr.StringEvent->NameString), Module, AsciiStrLen (Module));
        }
      } else {
        //
        // GUID Event Record
        //
        FpdtRecordPtr.GuidEvent->Header.Type     = GUID_EVENT_TYPE;
        FpdtRecordPtr.GuidEvent->Header.Length   = sizeof (GUID_EVENT_RECORD);
        FpdtRecordPtr.GuidEvent->Header.Revision = RECORD_REVISION_1;
        FpdtRecordPtr.GuidEvent->ProgressID      = (UINT16) Identifier;
        FpdtRecordPtr.GuidEvent->ApicID          = GetApicId ();
        FpdtRecordPtr.GuidEvent->Timestamp       = GetTimeInNanoSecond (Ticker);
        CopyMem (&FpdtRecordPtr.GuidEvent->Guid, &ModuleGuid, sizeof (FpdtRecordPtr.GuidEvent->Guid));
      }
      break;

    case BDS_ATTEMPT_EVENT_TYPE:
      //
      // Identifier should be less than 0xFFFF, it will be recorded in "Boot####".
      //
      if (Identifier >= 0x10000) {
        return EFI_UNSUPPORTED;
      }

      //
      // Get real ticker
      //
      if (Ticker == 0) {
        Ticker = GetPerformanceCounter ();
      }

      //
      // BDS Boot Attempt Record
      //
      FpdtRecordPtr.BdsAttempt->Header.Type      = BDS_ATTEMPT_EVENT_TYPE;
      FpdtRecordPtr.BdsAttempt->Header.Revision  = RECORD_REVISION_1;
      FpdtRecordPtr.BdsAttempt->ApicID           = GetApicId ();
      FpdtRecordPtr.BdsAttempt->BdsAttemptNo     = ++ mBdsAttemptNumber;
      FpdtRecordPtr.BdsAttempt->Timestamp        = GetTimeInNanoSecond (Ticker);
      UnicodeSPrint (BootOptionStr, sizeof (BootOptionStr), L"Boot%04X", (UINT16) Identifier);
      UnicodeStrToAsciiStr (BootOptionStr, (CHAR8 *) &FpdtRecordPtr.BdsAttempt->UEFIBootVar);
      FpdtRecordPtr.BdsAttempt->DevicePathString[0] = 0;
      FpdtRecordPtr.BdsAttempt->DevicePathString[1] = 0;
      FpdtRecordPtr.BdsAttempt->Header.Length       = sizeof (BDS_ATTEMPT_RECORD);

      //
      // DevicePath matches to Boot variable "Boot%04X"
      // Get DevicePath, and convert it to text string by DevicePathToText protocol.
      //

      //
      // Get DevicePathToText protocol first.
      //
      Status = EFI_SUCCESS;
      if (mDevicePathToText == NULL) {
        Status = gBS->LocateProtocol (
                        &gEfiDevicePathToTextProtocolGuid,
                        NULL,
                        (VOID **) &mDevicePathToText
                        );
      }

      //
      // Get DevicePath that matches to Boot variable "Boot%04X"
      //
      if (!EFI_ERROR (Status) && mDevicePathToText != NULL) {
        BootOption = GetEfiGlobalVariable (BootOptionStr);
        if (BootOption != NULL) {
          //
          // Get BootDevice from LoadOption.
          //
          BootString    = (CHAR16 *) (BootOption + sizeof (UINT32) + sizeof (UINT16));
          BootDevice    = (EFI_DEVICE_PATH_PROTOCOL *) ((UINT8 *) BootString + StrSize (BootString));
          //
          // Convert DevicePath to Text string.
          //
          BootString    = mDevicePathToText->ConvertDevicePathToText (BootDevice, TRUE, FALSE);
          FreePool (BootOption);
          if (BootString != NULL) {
            //
            // Make Length is 4 byte alignment and less than 0x100
            //
            Length = (StrLen (BootString) + sizeof (BDS_ATTEMPT_RECORD) + 3) & ~3;
            if (Length >= 0x100) {
              Length = 0x100 - 4;
              BootString[Length - sizeof (BDS_ATTEMPT_RECORD) + 1] = 0;
            }

            //
            // Allocate BdsAttempRecord buffer.
            //
            BdsAttempRecordPtr = (BDS_ATTEMPT_RECORD *) AllocateZeroPool (Length);
            ASSERT (BdsAttempRecordPtr != NULL);
            CopyMem (BdsAttempRecordPtr, FpdtRecordPtr.BdsAttempt, sizeof (BDS_ATTEMPT_RECORD));

            //
            // Copy DevicePath string as ascii string into BdsAttempt record.
            //
            BdsAttempRecordPtr->Header.Length = (UINT8) Length;
            UnicodeStrToAsciiStr (BootString, BdsAttempRecordPtr->DevicePathString);
            FreePool (BootString);

            //
            // Report allocated BdsAttempt record
            //
            REPORT_STATUS_CODE_EX (
              EFI_PROGRESS_CODE,
              EFI_SOFTWARE_DXE_BS_DRIVER,
              0,
              NULL,
              &gEfiFirmwarePerformanceGuid,
              BdsAttempRecordPtr,
              BdsAttempRecordPtr->Header.Length
              );

            //
            // Free allocated memory.
            //
            FreePool (BdsAttempRecordPtr);
            return EFI_SUCCESS;
          }
        }
    }
      break;

    case BOOT_MODULE_TABLE_PTR_TYPE:
      //
      // Get Module Guid and Name based on ImageHandle
      // Ticker is the table address of boot module pointer table.
      //
      GetModuleInfoFromHandle ((EFI_HANDLE) Handle, ModuleName, sizeof (ModuleName), &ModuleGuid);
      FpdtRecordPtr.BootTablePtr->Header.Type             = BOOT_MODULE_TABLE_PTR_TYPE;
      FpdtRecordPtr.BootTablePtr->Header.Length           = sizeof (BOOT_MODULE_PERFORMANCE_TABLE_POINTER_RECORD);
      FpdtRecordPtr.BootTablePtr->Header.Revision         = RECORD_REVISION_1;
      FpdtRecordPtr.BootTablePtr->PerformanceTablePointer = Ticker;
      CopyGuid (&FpdtRecordPtr.BootTablePtr->Guid, &ModuleGuid);
      break;

    case PLATFORM_BOOT_TABLE_PTR_TYPE:
      //
      // Ticker is the table address of platform boot pointer table.
      //
      FpdtRecordPtr.PlatformBootTablePtr->Header.Type             = PLATFORM_BOOT_TABLE_PTR_TYPE;
      FpdtRecordPtr.PlatformBootTablePtr->Header.Length           = sizeof (PLATFORM_BOOT_PERFORMANCE_TABLE_POINTER_RECORD);
      FpdtRecordPtr.PlatformBootTablePtr->Header.Revision         = RECORD_REVISION_1;
      FpdtRecordPtr.PlatformBootTablePtr->PerformanceTablePointer = Ticker;
      break;

    case RUNTIME_MODULE_TABLE_PTR_TYPE:
      //
      // Get Module Guid and Name based on ImageHandle
      // Ticker is the table address of runtime module table.
      //
      GetModuleInfoFromHandle ((EFI_HANDLE) Handle, ModuleName, sizeof (ModuleName), &ModuleGuid);
      FpdtRecordPtr.RuntimeTablePtr->Header.Type        = RUNTIME_MODULE_TABLE_PTR_TYPE;
      FpdtRecordPtr.RuntimeTablePtr->Header.Length      = sizeof (RUNTIME_MODULE_PERFORMANCE_TABLE_POINTER_RECORD);
      FpdtRecordPtr.RuntimeTablePtr->Header.Revision    = RECORD_REVISION_1;
      FpdtRecordPtr.RuntimeTablePtr->PerformanceTablePointer = Ticker;
      CopyGuid (&FpdtRecordPtr.RuntimeTablePtr->Guid, &ModuleGuid);
      break;

    default:
      //
      // Record is undefined, return EFI_ABORTED
      //
      return EFI_ABORTED;
      break;
    }

  //
  // Report record one by one after records have been reported together.
  //
  if (mFpdtDataIsReport) {
    REPORT_STATUS_CODE_EX (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_DXE_BS_DRIVER,
      0,
      NULL,
      &gEfiFirmwarePerformanceGuid,
      FpdtRecordPtr.RecordHeader,
      FpdtRecordPtr.RecordHeader->Length
      );
  } else {
    //
    // Update the cached FPDT record buffer.
    //
    mPerformanceLength += FpdtRecordPtr.RecordHeader->Length;
  }

  return EFI_SUCCESS;
}


/**
  Dumps all the PEI performance .

  This internal function dumps all the PEI performance log to the DXE performance gauge array.
  It retrieves the optional GUID HOB for PEI performance and then saves the performance data
  to DXE performance data structures.

**/
VOID
InternalGetPeiPerformance (
  VOID
  )
{
  UINT32                        Index;
  PEI_FIRMWARE_PERFORMANCE_HOB  *FirmwarePerformanceHob;
  PEI_GUID_EVENT_RECORD         *PeiGuidRec;
  GUID_EVENT_RECORD             *GuidEventRec;
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetFirstGuidHob (&gPeiFirmwarePerformanceGuid);
  if (GuidHob != NULL) {
    FirmwarePerformanceHob = GET_GUID_HOB_DATA (GuidHob);
    if (mPerformanceLength + sizeof (GUID_EVENT_RECORD) * FirmwarePerformanceHob->NumberOfEntries > mMaxPerformanceLength) {
      mPerformancePointer = ReallocatePool (
                              mPerformanceLength,
                              mPerformanceLength +
                              sizeof (GUID_EVENT_RECORD) * FirmwarePerformanceHob->NumberOfEntries +
                              FIRMWARE_RECORD_BUFFER,
                              mPerformancePointer
                              );

      ASSERT (mPerformancePointer != NULL);
      mMaxPerformanceLength = mPerformanceLength +
                              sizeof (GUID_EVENT_RECORD) * FirmwarePerformanceHob->NumberOfEntries +
                              FIRMWARE_RECORD_BUFFER;
    }

    GuidEventRec = (GUID_EVENT_RECORD *) (mPerformancePointer + mPerformanceLength);
    for (Index = 0; Index < FirmwarePerformanceHob->NumberOfEntries; Index ++, GuidEventRec ++) {
      PeiGuidRec  = &(FirmwarePerformanceHob->GuidEventRecord[Index]);

      //
      // GUID Event Records from PEI phase
      //
      GuidEventRec->Header.Type       = GUID_EVENT_TYPE;
      GuidEventRec->Header.Length     = sizeof (GUID_EVENT_RECORD);
      GuidEventRec->Header.Revision   = RECORD_REVISION_1;
      GuidEventRec->ProgressID        = PeiGuidRec->ProgressID;
      GuidEventRec->ApicID            = PeiGuidRec->ApicID;
      GuidEventRec->Timestamp         = PeiGuidRec->Timestamp;
      GuidEventRec->Guid              = PeiGuidRec->Guid;
    }
    //
    // Update the used buffer size.
    //
    mPerformanceLength += sizeof (GUID_EVENT_RECORD) * FirmwarePerformanceHob->NumberOfEntries;
    DEBUG ((DEBUG_INFO, "FPDT: Performance PEI Boot Performance Record Number is 0x%x\n", FirmwarePerformanceHob->NumberOfEntries));
  }
}


/**
  Report all FPDT record as report status code.

  @param[in]  Event       The event of notify protocol.
  @param[in]  Context     Notify event context.

**/
VOID
EFIAPI
ReportFpdtRecordData (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
  UINT8     *PerfBuffer;
  UINTN     MaxUint16;

  PerfBuffer = mPerformancePointer;
  MaxUint16  = 0xFFFF;

  while (mPerformanceLength > MaxUint16) {
    //
    // Report extension data size is UINT16. So, the size of report data can't exceed 0xFFFF.
    //
    REPORT_STATUS_CODE_EX (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_DXE_BS_DRIVER,
      0,
      NULL,
      &gEfiFirmwarePerformanceGuid,
      PerfBuffer,
      MaxUint16
      );
    mPerformanceLength = mPerformanceLength - MaxUint16;
    PerfBuffer         = PerfBuffer + MaxUint16;
  }

  REPORT_STATUS_CODE_EX (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_DXE_BS_DRIVER,
    0,
    NULL,
    &gEfiFirmwarePerformanceGuid,
    PerfBuffer,
    mPerformanceLength
    );

  //
  // Free Cached FPDT record Buffer
  //
  FreePool (mPerformancePointer);
  mPerformanceLength    = 0;
  mMaxPerformanceLength = 0;

  //
  // Set FPDT report state to TRUE.
  //
  mFpdtDataIsReport     = TRUE;
}


/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, Module and Identifier.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             The data was read correctly from the device.
  @retval     EFI_OUT_OF_RESOURCES    There are not enough resources to record the measurement.

**/
EFI_STATUS
EFIAPI
StartGaugeEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return InsertFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, Module and Identifier and has an end time value of zero.
  If the record can not be found then return EFI_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the found record
                                      is same as the one found by EndGauge of PERFORMANCE_PROTOCOL.

  @retval     EFI_SUCCESS             The end of  the measurement was recorded.
  @retval     EFI_NOT_FOUND           The specified measurement record could not be found.

**/
EFI_STATUS
EFIAPI
EndGaugeEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return InsertFpdtMeasurement (FALSE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Retrieves a previously logged performance measurement.
  It can also retrieve the log created by StartGauge and EndGauge of PERFORMANCE_PROTOCOL,
  and then assign the Identifier with 0.

  Retrieves the performance log entry from the performance log specified by LogEntryKey.
  If it stands for a valid entry, then EFI_SUCCESS is returned and
  GaugeDataEntryEx stores the pointer to that entry.

  @param[in]  LogEntryKey             The key for the previous performance measurement log entry.
                                      If 0, then the first performance measurement log entry is retrieved.
  @param[out] GaugeDataEntryEx        The indirect pointer to the extended gauge data entry specified by LogEntryKey
                                      if the retrieval is successful.

  @retval     EFI_SUCCESS             The GuageDataEntryEx is successfully found based on LogEntryKey.
  @retval     EFI_NOT_FOUND           The LogEntryKey is the last entry (equals to the total entry number).
  @retval     EFI_INVALIDE_PARAMETER  The LogEntryKey is not a valid entry (greater than the total entry number).
  @retval     EFI_INVALIDE_PARAMETER  GaugeDataEntryEx is NULL.

**/
EFI_STATUS
EFIAPI
GetGaugeEx (
  IN  UINTN                 LogEntryKey,
  OUT GAUGE_DATA_ENTRY_EX   **GaugeDataEntryEx
  )
{
  return EFI_UNSUPPORTED;
}

//
// Interfaces for PerformanceEx Protocol.
//
PERFORMANCE_EX_PROTOCOL mPerformanceExInterface = {
  StartGaugeEx,
  EndGaugeEx,
  GetGaugeEx
  };


/**
  The constructor function initializes Performance infrastructure for DXE phase.

  The constructor function publishes Performance and PerformanceEx protocol, allocates memory to log DXE performance
  and merges PEI performance data to DXE performance log.
  It will ASSERT() if one of these operations fails and it will always return EFI_SUCCESS.

  @param[in]  ImageHandle     The firmware allocated handle for the EFI image.
  @param[in]  SystemTable     A pointer to the EFI System Table.

  @retval     EFI_SUCCESS     The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
DxeCoreFpdtPerformanceLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_EVENT                 EndOfDxeEvent;

  if (!PerformanceMeasurementEnabled ()) {
    //
    // Do not initialize performance infrastructure if not required.
    //
    return EFI_SUCCESS;
  }

  InternalGetPeiPerformance ();

  //
  // Install the protocol interfaces for DXE performance library instance.
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gPerformanceExProtocolGuid,
                  &mPerformanceExInterface,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register End of DXE event to report StatusCode data
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  ReportFpdtRecordData,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, Module and Identifier.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the created record
                                      is same as the one created by StartPerformanceMeasurement.

  @retval     RETURN_SUCCESS          The start of the measurement was recorded.
  @retval     RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return (RETURN_STATUS) InsertFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, Module and Identifier and has an end time value of zero.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.
  @param[in]  Identifier              32-bit identifier. If the value is 0, the found record
                                      is same as the one found by EndPerformanceMeasurement.

  @retval    RETURN_SUCCESS           The end of  the measurement was recorded.
  @retval    RETURN_NOT_FOUND         The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  return (RETURN_STATUS) InsertFpdtMeasurement (FALSE, Handle, Token, Module, TimeStamp, Identifier);
}


/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurement and EndPerformanceMeasurement,
  and then assign the Identifier with 0.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, EndTimeStamp and Identifier.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().
  If Identifier is NULL, then ASSERT().

  @param[in]   LogEntryKey             On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance log entry.
  @param[out]  Handle                  Pointer to environment specific context used to identify the component
                                       being measured.
  @param[out]  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                       being measured.
  @param[out]  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                       being measured.
  @param[out]  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was started.
  @param[out]  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was ended.
  @param[out]  Identifier              Pointer to the 32-bit identifier that was recorded when the measurement
                                       was ended.

  @return      The key for the next performance log entry

**/
UINTN
EFIAPI
GetPerformanceMeasurementEx (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp,
  OUT UINT32      *Identifier
  )
{
  return 0;
}


/**
  Adds a record at the end of the performance measurement log
  that records the start time of a performance measurement.

  Adds a record to the end of the performance measurement log
  that contains the Handle, Token, and Module.
  The end time of the new record must be set to zero.
  If TimeStamp is not zero, then TimeStamp is used to fill in the start time in the record.
  If TimeStamp is zero, the start time in the record is filled in with the value
  read from the current time stamp.

  @param[in]  Handle                     Pointer to environment specific context used
                                         to identify the component being measured.
  @param[in]  Token                      Pointer to a Null-terminated ASCII string
                                         that identifies the component being measured.
  @param[in]  Module                     Pointer to a Null-terminated ASCII string
                                         that identifies the module being measured.
  @param[in]  TimeStamp                  64-bit time stamp.

  @retval     RETURN_SUCCESS             The start of the measurement was recorded.
  @retval     RETURN_OUT_OF_RESOURCES    There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return (RETURN_STATUS) InsertFpdtMeasurement (TRUE, Handle, Token, Module, TimeStamp, 0);
}


/**
  Searches the performance measurement log from the beginning of the log
  for the first matching record that contains a zero end time and fills in a valid end time.

  Searches the performance measurement log from the beginning of the log
  for the first record that matches Handle, Token, and Module and has an end time value of zero.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then the end time in the record is filled in with the value specified by TimeStamp.
  If the record is found and TimeStamp is zero, then the end time in the matching record
  is filled in with the current time stamp value.

  @param[in]  Handle                  Pointer to environment specific context used
                                      to identify the component being measured.
  @param[in]  Token                   Pointer to a Null-terminated ASCII string
                                      that identifies the component being measured.
  @param[in]  Module                  Pointer to a Null-terminated ASCII string
                                      that identifies the module being measured.
  @param[in]  TimeStamp               64-bit time stamp.

  @retval     RETURN_SUCCESS          The end of  the measurement was recorded.
  @retval     RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return (RETURN_STATUS) InsertFpdtMeasurement (FALSE, Handle, Token, Module, TimeStamp, 0);
}


/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurementEx and EndPerformanceMeasurementEx,
  and then eliminate the Identifier.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, and EndTimeStamp.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().

  @param[in]  LogEntryKey              On entry, the key of the performance measurement log entry to retrieve.
                                       0, then the first performance measurement log entry is retrieved.
                                       On exit, the key of the next performance log entry.
  @param[out]  Handle                  Pointer to environment specific context used to identify the component
                                       being measured.
  @param[out]  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                       being measured.
  @param[out]  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                       being measured.
  @param[out]  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was started.
  @param[out]  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                       was ended.

  @return The key for the next performance log entry

**/
UINTN
EFIAPI
GetPerformanceMeasurement (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp
  )
{
  return 0;
}


/**
  Returns TRUE if the performance measurement macros are enabled.

  This function returns TRUE if the PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
  PcdPerformanceLibraryPropertyMask is set.  Otherwise FALSE is returned.

  @retval TRUE                    The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is set.
  @retval FALSE                   The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is clear.

**/
BOOLEAN
EFIAPI
PerformanceMeasurementEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8 (PcdPerformanceLibraryPropertyMask) & PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED) != 0);
}

