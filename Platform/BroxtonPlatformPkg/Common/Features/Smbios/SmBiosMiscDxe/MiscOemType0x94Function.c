/** @file
  The function that processes the Smbios data type 0x94.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CommonHeader.h"
#include "MiscSubclassDriver.h"
#include <Protocol/DataHub.h>
#include <Library/HiiLib.h>
#include <Protocol/SeCOperation.h>
#include <Protocol/CpuIo2.h>
#include <Library/PrintLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/MpService.h>
#include <Protocol/SeCOperation.h>
#include <Protocol/CpuIo2.h>
#include <Library/IoLib.h>
#include <Library/PlatformSecureDefaultsLib.h>
#include <Library/I2clib.h>
#include <Library/CpuIA32.h>
#include <Library/DriverLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PmicLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include "SetupMode.h"
#include <PlatformBaseAddresses.h>
#include <Library/SteppingLib.h>
#include <Library/PmcIpcLib.h>
#include <Library/BiosIdLib.h>

#include <Guid/MemoryConfigData.h>
#include <Guid/HobList.h>
#include <Library/HeciMsgLib.h>

#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include <MmrcData.h>

#define LEFT_JUSTIFY  0x01
#define PREFIX_SIGN   0x02
#define PREFIX_BLANK  0x04
#define COMMA_TYPE    0x08
#define LONG_TYPE     0x10
#define PREFIX_ZERO   0x20
#define CHARACTER_NUMBER_FOR_VALUE  30
#define MSR_IA32_PLATFORM_ID        0x17

#define MSR_IA32_BIOS_SIGN_ID       0x0000008B
#define IPC_CMD_ID_PMC_VER          0xE7  // PMC FW version
#define IPC_SUBCMD_ID_PMC_VER_READ  0x00  //   Read PMC FW version

#define PREFIX_ZERO   0x20

#ifndef MmPciAddress
#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)0xE0000000 + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )
#endif

static CHAR16 mHexStr[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
                            L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };


extern EFI_GUID gIFWIVersionHobGuid;
extern EFI_GUID gEfiHobListGuid;
extern EFI_GUID gEfiMemoryConfigDataGuid;


/**
  VSPrint worker function that prints a Value as a decimal number in Buffer.

  @param[in]  Buffer   Location to place ASCII decimal number string of Value.
  @param[in]  Value    Decimal value to convert to a string in Buffer.
  @param[in]  Flags    Flags to use in printing decimal string, see file header for details.
  @param[in]  Width    Width of hex value.

  @Returns             Number of characters printed.

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
    Value = (INT64) DivU64x32 ((UINT64)Value, 10);
    Remainder = (UINTN) ((UINT64)TempValue - 10 * Value);
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
    *(TempStr++)    = '-';
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


/**
  VSPrint worker function that prints a Value as a hex number in Buffer

  @param[in]  Buffer  Location to place ASCII hex string of Value.
  @param[in]  Value   Hex value to convert to a string in Buffer.
  @param[in]  Flags   Flags to use in printing Hex string, see file header for details.
  @param[in]  Width   Width of hex value.

  @Returns            Number of characters printed.

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
    Index = ((UINTN) Value & 0xf);
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

  @param[in]  String     String that will contain the value
  @param[in]  MacAddr    Converts MAC address
  @param[in]  AddrSize   Size of MAC address

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


EFI_STATUS
TJudgeHandleIsPCIDevice(
  EFI_HANDLE    Handle,
  UINT8            Device,
  UINT8            Funs
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
  if (!EFI_ERROR(Status)) {
    DevicePath = DPath;
    while (!IsDevicePathEnd (DPath)) {
      if ((DPath->Type == HARDWARE_DEVICE_PATH) && (DPath->SubType == HW_PCI_DP)) {
        PCI_DEVICE_PATH   *PCIPath;

        PCIPath = (PCI_DEVICE_PATH*) DPath;
        DPath = NextDevicePathNode (DPath);
        if (IsDevicePathEnd(DPath) && (PCIPath->Device == Device) && (PCIPath->Function == Funs)) {
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
TSearchChildHandle (
  EFI_HANDLE Father,
  EFI_HANDLE *Child
  )
{
  EFI_STATUS                             Status;
  UINTN                                  HandleIndex;
  EFI_GUID                               **ProtocolGuidArray = NULL;
  UINTN                                  ArrayCount;
  UINTN                                  ProtocolIndex;
  UINTN                                  OpenInfoCount;
  UINTN                                  OpenInfoIndex;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY    *OpenInfo = NULL;
  UINTN                                  mHandleCount;
  EFI_HANDLE                             *mHandleBuffer= NULL;

  //
  // Retrieve the list of all handles from the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &mHandleCount,
                  &mHandleBuffer
                  );

  for (HandleIndex = 0; HandleIndex < mHandleCount; HandleIndex++) {
    //
    // Retrieve the list of all the protocols on each handle
    //
    Status = gBS->ProtocolsPerHandle (
                    mHandleBuffer[HandleIndex],
                    &ProtocolGuidArray,
                    &ArrayCount
                    );
    if (!EFI_ERROR (Status)) {
      for (ProtocolIndex = 0; ProtocolIndex < ArrayCount; ProtocolIndex++) {
        Status = gBS->OpenProtocolInformation (
                        mHandleBuffer[HandleIndex],
                        ProtocolGuidArray[ProtocolIndex],
                        &OpenInfo,
                        &OpenInfoCount
                        );
        if (!EFI_ERROR (Status)) {
          for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++) {
            if (OpenInfo[OpenInfoIndex].AgentHandle == Father) {
              if ((OpenInfo[OpenInfoIndex].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) {
                *Child = mHandleBuffer[HandleIndex];
                Status = EFI_SUCCESS;
                goto TryReturn;
              }
            }
          }
          Status = EFI_NOT_FOUND;
        }
      }
      if (OpenInfo != NULL) {
        FreePool (OpenInfo);
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
  if (mHandleBuffer != NULL) {
    FreePool (mHandleBuffer);
    mHandleBuffer = NULL;
  }

  return Status;
}


EFI_STATUS
TGetDriverName(
  EFI_HANDLE   Handle,
  CHAR16         *Name
  )
{
  EFI_DRIVER_BINDING_PROTOCOL        *BindHandle = NULL;
  EFI_STATUS                         Status;
  UINT32                             Version;
  UINT16                             *Ptr;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiDriverBindingProtocolGuid,
                  (VOID**)&BindHandle,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Version = BindHandle->Version;
  Ptr = (UINT16*) &Version;
  UnicodeSPrint (Name, 40, L"%d.%d.%d", Version >> 24 , (Version >>16)& 0x0f ,*(Ptr));

  return EFI_SUCCESS;
}


EFI_STATUS
TGetGOPDriverName(
  CHAR16 *Name
  )
{
  UINTN                        HandleCount;
  EFI_HANDLE                   *Handles= NULL;
  UINTN                        Index;
  EFI_STATUS                   Status;
  EFI_HANDLE                   Child = 0;

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiDriverBindingProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = TSearchChildHandle (Handles[Index], &Child);
    if (!EFI_ERROR (Status)) {
      Status = TJudgeHandleIsPCIDevice (Child, 0x02, 0x00);
      if (!EFI_ERROR (Status)) {
        return TGetDriverName (Handles[Index], Name);
      }
    }
  }

  return EFI_UNSUPPORTED;
}


EFI_STATUS
SmbiosGetImageFwVersion (
  VOID
  )
{
  EFI_STATUS                  Status;
  FW_VERSION_CMD_RESP_DATA    MsgGetFwVersionRespData;
  UINT8                       Index;
  CHAR16                      Buffer[40];
  CHAR8                       *CseManName;
  CHAR8                       *IshManName;

  DEBUG ((EFI_D_INFO, "Executing SmbiosGetImageFwVersion().\n"));
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
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_CSE_VERSION_VALUE), Buffer, NULL);
    }

    if (!CompareMem (MsgGetFwVersionRespData.ModuleEntries[Index].EntryName, IshManName, AsciiStrLen (IshManName))) {
      UnicodeSPrint (Buffer, sizeof (Buffer), L"%d.%d.%d.%d",
        MsgGetFwVersionRespData.ModuleEntries[Index].Major,
        MsgGetFwVersionRespData.ModuleEntries[Index].Minor,
        MsgGetFwVersionRespData.ModuleEntries[Index].Hotfix,
        MsgGetFwVersionRespData.ModuleEntries[Index].Build);
      HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_ISH_FW_VALUE), Buffer, NULL);
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
UpdatePlatformInformation (
  )
{
  UINT32                   MicroCodeVersion;
  CHAR16                   Buffer[0x100];
  EFI_STATUS               Status;
  UINT8                    CpuFlavor = 0;
  EFI_PEI_HOB_POINTERS     GuidHob;
  EFI_PLATFORM_INFO_HOB    *PlatformInfo = NULL;
  UINT32                   SoCStepping;
  UINT8                    Data8;
  CHAR16                   Name[40];
  SYSTEM_CONFIGURATION     SystemConfiguration;
  UINTN                    VarSize;
  EFI_BOOT_MODE            BootMode;
  UINT8                    PMCVersion[8];
  CHAR16                   ReleaseDate[100];
  CHAR16                   ReleaseTime[100];
  VOID                     *HobList = NULL;
  VOID                     *HobData = NULL;
  UINTN                    DataSize;
  MRC_PARAMS_SAVE_RESTORE  *MemInfoHob = NULL;
  UINT32                   MrcVersion;
  UINTN                    Index;

  DEBUG ((EFI_D_INFO, "Executing SMBIOS T0x94 Update.\n"));
  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiPlatformInfoGuid, GuidHob.Raw)) != NULL) {
      PlatformInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  if (GetBxtSeries () == BxtP) {
    UnicodeSPrint (Buffer, sizeof (Buffer), L"Broxton-P");
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_FVI_PLATFORM), Buffer, NULL);
  }

  Status = TGetGOPDriverName (Name);
  if (!EFI_ERROR (Status)) {
    HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_GOP_VERSION_VALUE), Name, NULL);
  }

  //
  //CpuFlavor
  //
  //BXT
  //BXT Tablet        000
  //BXT Notebook      001
  //BXT Desktop       010
  //
  // CPU flavor
  //
  CpuFlavor = RShiftU64 (EfiReadMsr (MSR_IA32_PLATFORM_ID), 50) & 0x07;
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", CpuFlavor);
  HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_CPU_FLAVOR_VALUE), Buffer, NULL);

  if (PlatformInfo != NULL) {
    //
    // Board Id
    //
    UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", PlatformInfo->BoardId);
    HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_BOARD_ID_VALUE), Buffer, NULL);

    //
    // FAB ID
    //
    UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", PlatformInfo->BoardRev);
    HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_FAB_ID_VALUE), Buffer, NULL);
  }

  //
  // SOC
  //
  SoCStepping = BxtStepping ();
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", SoCStepping);
  HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_SOC_VALUE), Buffer, NULL);

  //
  // uCode
  //
  MicroCodeVersion = (UINT32) RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32);
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", MicroCodeVersion);
  HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_UCODE_VALUE), Buffer, NULL);

  //
  // PUNIT
  //
  HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_PUNIT_FW_VALUE), Buffer, NULL);

  //
  // PMC
  //
  IpcSendCommandEx (IPC_CMD_ID_PMC_VER, IPC_SUBCMD_ID_PMC_VER_READ, PMCVersion, 8);
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%02x.%02x", PMCVersion[1], PMCVersion[0]);
  HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_PMC_FW_VALUE), Buffer, NULL);

  //
  // CSE and ISH
  //
  Status = SmbiosGetImageFwVersion ();

  //
  // MRC
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  if ((HobList = GetNextGuidHob (&gFspNonVolatileStorageHobGuid, HobList)) != NULL) {
    HobData = GET_GUID_HOB_DATA (HobList);
    DataSize = GET_GUID_HOB_DATA_SIZE (HobList);
    MemInfoHob = (MRC_PARAMS_SAVE_RESTORE *) HobData;
  }

  if (!EFI_ERROR (Status) && NULL != HobData ) {
    MrcVersion = MemInfoHob->MrcVer >> 16;
    MrcVersion &= 0xffff;
    Index = EfiValueToString (Buffer, MrcVersion/256, PREFIX_ZERO, 0);
    StrCatS (Buffer, sizeof (Buffer) / sizeof (CHAR16), L".");
    EfiValueToString (Buffer + Index + 1, MrcVersion%256, PREFIX_ZERO, 0);
    HiiSetString (mHiiHandle,STRING_TOKEN (STR_MISC_MRC_VERSION_VALUE), Buffer, NULL);
  }

  //
  // BIOS
  //
  GetBiosVersionDateTime (Buffer, ReleaseDate, ReleaseTime);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_BIOS_VERSION_VALUE), Buffer, NULL);


  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Secure boot
  //
  Data8 = SystemConfiguration.SecureBoot;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SECURE_BOOT_VALUE), Buffer, NULL);

  //
  //Bootmode
  //
  BootMode = GetBootModeHob ();
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", BootMode);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_BOOT_MODE_VALUE), Buffer, NULL);

  //
  //SpeedStep
  //
  Data8 = SystemConfiguration.EnableGv;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_SPEED_STEP_VALUE), Buffer, NULL);

  //
  //CPU Turbo
  //
  if (BxtStepping () == BxtA0) {
    Data8 = 0;
  } else {
    Data8 = SystemConfiguration.TurboModeEnable;
  }
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_CPU_TURBO_VALUE), Buffer, NULL);

  //
  //C-states
  //
  Data8 = SystemConfiguration.EnableCx;
  UnicodeSPrint (Buffer, sizeof (Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_CSTATE_VALUE), Buffer, NULL);

  //
  //Enhanced C-states
  //
  Data8 = SystemConfiguration.EnableCxe;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_ENHANCED_CSTATE_VALUE), Buffer, NULL);

  //
  //PkgCState
  //
  Data8 = SystemConfiguration.MaxPkgCState;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_PACKAGE_CSTATE_VALUE), Buffer, NULL);

  //
  //CoreCState
  //
  Data8 = SystemConfiguration.MaxCoreCState;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_CORE_CSTATE_VALUE), Buffer, NULL);

  //
  //S0ix
  //
  Data8 = SystemConfiguration.LowPowerS0Idle;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_S0IX_VALUE), Buffer, NULL);

  //
  //RC6
  //
  Data8 = SystemConfiguration.EnableRenderStandby;
  UnicodeSPrint (Buffer, sizeof(Buffer), L"%x", Data8);
  HiiSetString (mHiiHandle, STRING_TOKEN (STR_MISC_RC6_VALUE), Buffer, NULL);

  DEBUG ((EFI_D_INFO, "Executing SMBIOS T0x94 Update end.\n"));

  return EFI_SUCCESS;
}


/**
  Smbios OEM type 0x94 callback.

  @param[in]  Event     Event whose notification function is being invoked.
  @param[in]  Context   Pointer to the notification functions context, which is implementation dependent.

  @retval     None

**/
VOID
AddSmbiosT0x94Callback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS            Status;
  //
  // For FVI name string length definition
  //
  UINTN                 StrFviPlatformStrLen = 0;
  UINTN                 StrIFWIVerStrLen = 0;
  UINTN                 StrCseVerStrLen = 0;
  UINTN                 StruCodeVerStrLen = 0;
  UINTN                 StrGOPStrLen = 0;
  UINTN                 StrMRCVersionStrLen = 0;
  UINTN                 StrPMCVersionStrLen = 0;
  UINTN                 StrPunitVersionStrLen = 0;
  UINTN                 StrISHVersionStrLen = 0;
  UINTN                 StrSOCVersionStrLen = 0;
  UINTN                 StrBOARDVersionStrLen = 0;
  UINTN                 StrFABVersionStrLen = 0;
  UINTN                 StrCPUFLAVORStrLen = 0;
  UINTN                 StrBIOSVersionStrLen = 0;
  UINTN                 StrPMICVersionStrLen = 0;
  UINTN                 StrSecureBootModeLen = 0;
  UINTN                 StrBootModeLen = 0;
  UINTN                 StrSpeedStepModeLen = 0;
  UINTN                 StrCStateEnLen = 0;
  UINTN                 StrEnhancedCStateEnLen = 0;
  UINTN                 StrMaxCoreCStateLen = 0;
  UINTN                 StrMaxPkgCStateLen = 0;
  UINTN                 StrCpuTurboLen = 0;
  UINTN                 StrGfxTurboLen = 0;
  UINTN                 StrS0ixLen = 0;
  UINTN                 StrRC6Len = 0;

  //
  // For FVI value length definition
  //
  UINTN                 IFWIVerStrLen = 0;
  UINTN                 CseVerStrLen = 0;
  UINTN                 uCodeVerStrLen = 0;
  UINTN                 GOPStrLen = 0;
  UINTN                 MRCVersionStrLen = 0;
  UINTN                 PMCVersionStrLen = 0;
  UINTN                 PunitVersionStrLen = 0;
  UINTN                 ISHVersionStrLen = 0;
  UINTN                 SOCVersionStrLen = 0;
  UINTN                 BOARDVersionStrLen = 0;
  UINTN                 FABVersionStrLen = 0;
  UINTN                 CPUFLAVORStrLen = 0;
  UINTN                 BIOSVersionStrLen = 0;
  UINTN                 PMICVersionStrLen = 0;
  UINTN                 SecureBootModeLen = 0;
  UINTN                 BootModeLen = 0;
  UINTN                 SpeedStepModeLen = 0;
  UINTN                 CStateEnLen = 0;
  UINTN                 EnhancedCStateEnLen = 0;
  UINTN                 MaxCoreCStateLen = 0;
  UINTN                 MaxPkgCStateLen = 0;
  UINTN                 CpuTurboLen = 0;
  UINTN                 GfxTurboLen = 0;
  UINTN                 S0ixLen = 0;
  UINTN                 RC6Len = 0;

  SMBIOS_TABLE_TYPE94   *SmbiosRecord;
  EFI_SMBIOS_HANDLE     SmbiosHandle;
  EFI_MISC_OEM_TYPE_0x94 *ForType94InputData;

  //
  // FVI name string pointer
  //
  CHAR16                *StrFviPlatform;
  CHAR16                *StrIFWIVer;
  CHAR16                *StrCseVer;
  CHAR16                *StruCodeVer;
  CHAR16                *StrGOPVer;
  CHAR16                *StrMrcVer;
  CHAR16                *StrPmcVer;
  CHAR16                *StrPunitVer;
  CHAR16                *StrIshVer;
  CHAR16                *StrSocVer;
  CHAR16                *StrBoardVer;
  CHAR16                *StrFabVer;
  CHAR16                *StrCpuFlavor;
  CHAR16                *StrBiosVer;
  CHAR16                *StrPmicVer;
  CHAR16                *StrSecureBootMode;
  CHAR16                *StrBootMode;
  CHAR16                *StrSpeedStepMode;
  CHAR16                *StrMaxPkgCState;
  CHAR16                *StrMaxCoreCState;
  CHAR16                *StrCStateEn;
  CHAR16                *StrEnhancedCStateEn;
  CHAR16                *StrCpuTurbo;
  CHAR16                *StrGfxTurbo;
  CHAR16                *StrS0ix;
  CHAR16                *StrRC6;

  //
  // FVI value string pointer
  //
  CHAR16                *IFWIVer;
  CHAR16                *CseVer;
  CHAR16                *uCodeVer;
  CHAR16                *GOPVer;
  CHAR16                *MrcVer;
  CHAR16                *PmcVer;
  CHAR16                *PunitVer;
  CHAR16                *IshVer;
  CHAR16                *SocVer;
  CHAR16                *BoardVer;
  CHAR16                *FabVer;
  CHAR16                *CpuFlavor;
  CHAR16                *BiosVer;
  CHAR16                *PmicVer;
  CHAR16                *SecureBootMode;
  CHAR16                *BootMode;
  CHAR16                *SpeedStepMode;
  CHAR16                *MaxPkgCState;
  CHAR16                *MaxCoreCState;
  CHAR16                *CStateEn;
  CHAR16                *EnhancedCStateEn;
  CHAR16                *CpuTurbo;
  CHAR16                *GfxTurbo;
  CHAR16                *S0ix;
  CHAR16                *RC6;

  UINTN                 RecordLen = 0;
  UINTN                 StrIdx = 0;


  STRING_REF            TokenToGet;
  CHAR8                 *OptionalStrStart;
  EFI_SMBIOS_PROTOCOL   *SmbiosProtocol;

  ForType94InputData = (EFI_MISC_OEM_TYPE_0x94 *) Context;

  DEBUG ((EFI_D_INFO, "Executing SMBIOS T0x94 callback.\n"));

  gBS->CloseEvent (Event);    // Unload this event.

  //
  // First check for invalid parameters.
  //
  if (Context == NULL) {
    return;
  }

  UpdatePlatformInformation ();

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID *) &SmbiosProtocol);
  ASSERT_EFI_ERROR (Status);

  TokenToGet = STRING_TOKEN (STR_MISC_FVI_PLATFORM);
  StrFviPlatform = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrFviPlatformStrLen = StrLen(StrFviPlatform);
  if (StrFviPlatformStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_IFWI_VERSION_STRING);
  StrIFWIVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrIFWIVerStrLen = StrLen (StrIFWIVer);
  if (StrIFWIVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_IFWI_VERSION_VALUE);
  IFWIVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  IFWIVerStrLen = StrLen (IFWIVer);
  if (IFWIVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BIOS_VERSION_STRING);
  StrBiosVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrBIOSVersionStrLen = StrLen (StrBiosVer);
  if (StrBIOSVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BIOS_VERSION_VALUE);
  BiosVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  BIOSVersionStrLen = StrLen (BiosVer);
  if (BIOSVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CSE_VERSION_STRING);
  StrCseVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrCseVerStrLen = StrLen (StrCseVer);
  if (StrCseVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CSE_VERSION_VALUE);
  CseVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  CseVerStrLen = StrLen (CseVer);
  if (CseVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_GOP_VERSION_STRING);
  StrGOPVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrGOPStrLen = StrLen (StrGOPVer);
  if (StrGOPStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_GOP_VERSION_VALUE);
  GOPVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  GOPStrLen = StrLen (GOPVer);
  if (GOPStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_MRC_VERSION_STRING);
  StrMrcVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrMRCVersionStrLen = StrLen (StrMrcVer);
  if (StrMRCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_MRC_VERSION_VALUE);
  MrcVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  MRCVersionStrLen = StrLen (MrcVer);
  if (MRCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_UCODE_STRING);
  StruCodeVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StruCodeVerStrLen = StrLen (StruCodeVer);
  if (StruCodeVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_UCODE_VALUE);
  uCodeVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  uCodeVerStrLen = StrLen (uCodeVer);
  if (uCodeVerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PUNIT_FW_STRING);
  StrPunitVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrPunitVersionStrLen = StrLen (StrPunitVer);
  if (StrPunitVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PUNIT_FW_VALUE);
  PunitVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  PunitVersionStrLen = StrLen (PunitVer);
  if (PunitVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PMC_FW_STRING);
  StrPmcVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrPMCVersionStrLen = StrLen (StrPmcVer);
  if (StrPMCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PMC_FW_VALUE);
  PmcVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  PMCVersionStrLen = StrLen(PmcVer);
  if (PMCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_ISH_FW_STRING);
  StrIshVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrISHVersionStrLen = StrLen (StrIshVer);
  if (StrISHVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_ISH_FW_VALUE);
  IshVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  ISHVersionStrLen = StrLen (IshVer);
  if (ISHVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SOC_STRING);
  StrSocVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrSOCVersionStrLen = StrLen (StrSocVer);
  if (StrSOCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SOC_VALUE);
  SocVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SOCVersionStrLen = StrLen (SocVer);
  if (SOCVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BOARD_ID_STRING);
  StrBoardVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrBOARDVersionStrLen = StrLen (StrBoardVer);
  if (StrBOARDVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BOARD_ID_VALUE);
  BoardVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  BOARDVersionStrLen = StrLen (BoardVer);
  if (BOARDVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_FAB_ID_STRING);
  StrFabVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrFABVersionStrLen = StrLen (StrFabVer);
  if (StrFABVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_FAB_ID_VALUE);
  FabVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  FABVersionStrLen = StrLen (FabVer);
  if (FABVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CPU_FLAVOR_STRING);
  StrCpuFlavor = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrCPUFLAVORStrLen = StrLen (StrCpuFlavor);
  if (StrCPUFLAVORStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CPU_FLAVOR_VALUE);
  CpuFlavor = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  CPUFLAVORStrLen = StrLen (CpuFlavor);
  if (CPUFLAVORStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PMIC_VERSION_STRING);
  StrPmicVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrPMICVersionStrLen = StrLen (StrPmicVer);
  if (StrPMICVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PMIC_VERSION_VALUE);
  PmicVer = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  PMICVersionStrLen = StrLen (PmicVer);
  if (PMICVersionStrLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SECURE_BOOT_STRING);
  StrSecureBootMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrSecureBootModeLen = StrLen (StrSecureBootMode);
  if (StrSecureBootModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SECURE_BOOT_VALUE);
  SecureBootMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SecureBootModeLen = StrLen (SecureBootMode);
  if (SecureBootModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BOOT_MODE_STRING);
  StrBootMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrBootModeLen = StrLen (StrBootMode);
  if (StrBootModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_BOOT_MODE_VALUE);
  BootMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  BootModeLen = StrLen (BootMode);
  if (BootModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SPEED_STEP_STRING);
  StrSpeedStepMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrSpeedStepModeLen = StrLen (StrSpeedStepMode);
  if (StrSpeedStepModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_SPEED_STEP_VALUE);
  SpeedStepMode = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  SpeedStepModeLen = StrLen (SpeedStepMode);
  if (SpeedStepModeLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CPU_TURBO_STRING);
  StrCpuTurbo = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrCpuTurboLen = StrLen (StrCpuTurbo);
  if (StrCpuTurboLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CPU_TURBO_VALUE);
  CpuTurbo = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  CpuTurboLen = StrLen (CpuTurbo);
  if (CpuTurboLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CSTATE_STRING);
  StrCStateEn= HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrCStateEnLen = StrLen (StrCStateEn);
  if (StrCStateEnLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CSTATE_VALUE);
  CStateEn= HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  CStateEnLen = StrLen (CStateEn);
  if (CStateEnLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_ENHANCED_CSTATE_STRING);
  StrEnhancedCStateEn = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrEnhancedCStateEnLen = StrLen (StrEnhancedCStateEn);
  if (StrEnhancedCStateEnLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_ENHANCED_CSTATE_VALUE);
  EnhancedCStateEn = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  EnhancedCStateEnLen = StrLen (EnhancedCStateEn);
  if (EnhancedCStateEnLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PACKAGE_CSTATE_STRING);
  StrMaxPkgCState = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrMaxPkgCStateLen = StrLen (StrMaxPkgCState);
  if (StrMaxPkgCStateLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_PACKAGE_CSTATE_VALUE);
  MaxPkgCState = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  MaxPkgCStateLen = StrLen (MaxPkgCState);
  if (MaxPkgCStateLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CORE_CSTATE_STRING);
  StrMaxCoreCState = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrMaxCoreCStateLen = StrLen (StrMaxCoreCState);
  if (StrMaxCoreCStateLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_CORE_CSTATE_VALUE);
  MaxCoreCState = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  MaxCoreCStateLen = StrLen (MaxCoreCState);
  if (MaxCoreCStateLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_GFX_TURBO_STRING);
  StrGfxTurbo = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrGfxTurboLen = StrLen (StrGfxTurbo);
  if (StrGfxTurboLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_GFX_TURBO_VALUE);
  GfxTurbo = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  GfxTurboLen = StrLen (GfxTurbo);
  if (GfxTurboLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_S0IX_STRING);
  StrS0ix = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrS0ixLen = StrLen (StrS0ix);
  if (StrS0ixLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_S0IX_VALUE);
  S0ix = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  S0ixLen = StrLen (S0ix);
  if (S0ixLen > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_RC6_STRING);
  StrRC6 = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  StrRC6Len = StrLen (StrRC6);
  if (StrRC6Len > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  TokenToGet = STRING_TOKEN (STR_MISC_RC6_VALUE);
  RC6 = HiiGetPackageString (&gEfiCallerIdGuid, TokenToGet, NULL);
  RC6Len = StrLen (RC6);
  if (RC6Len > SMBIOS_STRING_MAX_LENGTH) {
    return;
  }

  RecordLen = sizeof (SMBIOS_TABLE_TYPE94) + StrFviPlatformStrLen + 1 + StrIFWIVerStrLen + 1 + IFWIVerStrLen + 1 + StrBIOSVersionStrLen + 1 + \
                      BIOSVersionStrLen + 1 + StrCseVerStrLen + 1 + CseVerStrLen + 1 + StrGOPStrLen + 1 + GOPStrLen + 1 + \
                      StrMRCVersionStrLen + 1 + MRCVersionStrLen + 1 + StruCodeVerStrLen + 1 + uCodeVerStrLen + 1 + StrPunitVersionStrLen + 1 + \
                      PunitVersionStrLen + 1 + StrPMCVersionStrLen + 1 + PMCVersionStrLen + 1 + StrISHVersionStrLen + 1 + ISHVersionStrLen + 1 + \
                      StrSOCVersionStrLen + 1 + SOCVersionStrLen + 1 + StrBOARDVersionStrLen + 1 + BOARDVersionStrLen + 1 + StrFABVersionStrLen + 1 + \
                      FABVersionStrLen + 1 + StrCPUFLAVORStrLen + 1 + CPUFLAVORStrLen + 1 + StrPMICVersionStrLen + 1 + PMICVersionStrLen + 1 + \
                      StrSecureBootModeLen + 1 + SecureBootModeLen + 1 + StrBootModeLen + 1 + BootModeLen + 1 + StrSpeedStepModeLen + 1 + SpeedStepModeLen + 1 + \
                      StrCpuTurboLen + 1 + CpuTurboLen + 1 + StrCStateEnLen + 1 + CStateEnLen + 1 + StrEnhancedCStateEnLen + 1 + EnhancedCStateEnLen + 1 + \
                      StrMaxPkgCStateLen + 1 + MaxPkgCStateLen + 1 + StrMaxCoreCStateLen + 1 + MaxCoreCStateLen + 1 + StrGfxTurboLen + 1 + GfxTurboLen + 1 + \
                      StrS0ixLen + 1 + S0ixLen + 1 + StrRC6Len + 1 + RC6Len + 1 + 1;

  SmbiosRecord = AllocatePool (RecordLen);
  if (SmbiosRecord == NULL) {
    return;
  }

  ZeroMem(SmbiosRecord, RecordLen);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MISC_VERSION_INFO;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE94);

  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle     = 0;

  SmbiosRecord->FviPlatform    = 1;

  SmbiosRecord->IFWIVersionStr = 2;
  SmbiosRecord->IFWIVersion    = 3;

  SmbiosRecord->BiosVersionStr = 4;
  SmbiosRecord->BiosVersion    = 5;

  SmbiosRecord->CseVersionStr  = 6;
  SmbiosRecord->CseVersion     = 7;

  SmbiosRecord->GopVersionStr  = 8;
  SmbiosRecord->GopVersion     = 9;

  SmbiosRecord->MRCVersionStr  = 10;
  SmbiosRecord->MRCVersion     = 11;

  SmbiosRecord->UCodeVersionStr= 12;
  SmbiosRecord->UCodeVersion   = 13;

  SmbiosRecord->PunitVersionStr= 14;
  SmbiosRecord->PunitVersion   = 15;

  SmbiosRecord->PMCVersionStr  = 16;
  SmbiosRecord->PMCVersion     = 17;

  SmbiosRecord->ISHVersionStr  = 18;
  SmbiosRecord->ISHVersion     = 19;

  SmbiosRecord->SoCVersionStr  = 20;
  SmbiosRecord->SoCVersion     = 21;

  SmbiosRecord->BoardVersionStr= 22;
  SmbiosRecord->BoardVersion   = 23;

  SmbiosRecord->FabVersionStr  = 24;
  SmbiosRecord->FabVersion     = 25;

  SmbiosRecord->CPUFlavorStr   = 26;
  SmbiosRecord->CPUFlavor      = 27;

  SmbiosRecord->PmicVersionStr = 28;
  SmbiosRecord->PmicVersion    = 29;

  SmbiosRecord->SecureBootStr  = 30;
  SmbiosRecord->SecureBoot     = 31;

  SmbiosRecord->BootModeStr    = 32;
  SmbiosRecord->BootMode       = 33;

  SmbiosRecord->SpeedStepModeStr = 34;
  SmbiosRecord->SpeedStepMode  = 35;

  SmbiosRecord->CPUTurboModeStr= 36;
  SmbiosRecord->CPUTurboMode   = 37;

  SmbiosRecord->CStateStr      = 38;
  SmbiosRecord->CState         = 39;

  SmbiosRecord->EnhancedCStateStr = 40;
  SmbiosRecord->EnhancedCState = 41;

  SmbiosRecord->MaxPkgCStateStr= 42;
  SmbiosRecord->MaxPkgCState   = 43;

  SmbiosRecord->MaxCoreCStateStr = 44;
  SmbiosRecord->MaxCoreCState  = 45;

  SmbiosRecord->GfxTurboStr    = 46;
  SmbiosRecord->GfxTurbo       = 47;

  SmbiosRecord->S0ixStr        = 48;
  SmbiosRecord->S0ix           = 49;

  SmbiosRecord->RC6Str         = 50;
  SmbiosRecord->RC6            = 51;

  //
  // Type 0x94 Version
  //
  // Version [4:0]: Version Field
  //
  // Different platform maintains different table version.
  // Version number should start from 1 and increase when table structure is changed.
  //
  // Version[7:5]: Format Field
  // 0x0 - Indicate that this table uses fixed definition to describe platform information.
  // 0x1 - Indicate that this table       uses  [key, value] combination to describe platform information.
  // 0x2 - Indicate that this table uses  [key, type, value] combination to describe platform information.
  //
  SmbiosRecord->FviVersion = 0x21;

  OptionalStrStart = (CHAR8 *) (SmbiosRecord + 1);

  UnicodeStrToAsciiStr (StrFviPlatform, OptionalStrStart);
  StrIdx +=  StrFviPlatformStrLen + 1;

  UnicodeStrToAsciiStr (StrIFWIVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrIFWIVerStrLen + 1;

  UnicodeStrToAsciiStr (IFWIVer, OptionalStrStart + StrIdx);
  StrIdx +=  IFWIVerStrLen + 1;

  UnicodeStrToAsciiStr (StrBiosVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrBIOSVersionStrLen + 1;

  UnicodeStrToAsciiStr (BiosVer, OptionalStrStart + StrIdx);
  StrIdx +=  BIOSVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrCseVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrCseVerStrLen + 1;

  UnicodeStrToAsciiStr (CseVer, OptionalStrStart + StrIdx);
  StrIdx +=  CseVerStrLen + 1;

  UnicodeStrToAsciiStr (StrGOPVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrGOPStrLen + 1;

  UnicodeStrToAsciiStr (GOPVer, OptionalStrStart + StrIdx);
  StrIdx +=  GOPStrLen + 1;

  UnicodeStrToAsciiStr (StrMrcVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrMRCVersionStrLen + 1;

  UnicodeStrToAsciiStr (MrcVer, OptionalStrStart + StrIdx);
  StrIdx +=  MRCVersionStrLen + 1;

  UnicodeStrToAsciiStr (StruCodeVer, OptionalStrStart + StrIdx);
  StrIdx +=  StruCodeVerStrLen + 1;

  UnicodeStrToAsciiStr (uCodeVer, OptionalStrStart + StrIdx);
  StrIdx +=  uCodeVerStrLen + 1;

  UnicodeStrToAsciiStr (StrPunitVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrPunitVersionStrLen + 1;

  UnicodeStrToAsciiStr (PunitVer, OptionalStrStart + StrIdx);
  StrIdx +=  PunitVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrPmcVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrPMCVersionStrLen + 1;

  UnicodeStrToAsciiStr (PmcVer, OptionalStrStart + StrIdx);
  StrIdx +=  PMCVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrIshVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrISHVersionStrLen + 1;

  UnicodeStrToAsciiStr (IshVer, OptionalStrStart + StrIdx);
  StrIdx +=  ISHVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrSocVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrSOCVersionStrLen + 1;

  UnicodeStrToAsciiStr (SocVer, OptionalStrStart + StrIdx);
  StrIdx +=  SOCVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrBoardVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrBOARDVersionStrLen + 1;

  UnicodeStrToAsciiStr (BoardVer, OptionalStrStart + StrIdx);
  StrIdx +=  BOARDVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrFabVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrFABVersionStrLen + 1;

  UnicodeStrToAsciiStr (FabVer, OptionalStrStart + StrIdx);
  StrIdx +=  FABVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrCpuFlavor, OptionalStrStart + StrIdx);
  StrIdx +=  StrCPUFLAVORStrLen + 1;

  UnicodeStrToAsciiStr (CpuFlavor, OptionalStrStart + StrIdx);
  StrIdx +=  CPUFLAVORStrLen + 1;

  UnicodeStrToAsciiStr (StrPmicVer, OptionalStrStart + StrIdx);
  StrIdx +=  StrPMICVersionStrLen + 1;

  UnicodeStrToAsciiStr (PmicVer, OptionalStrStart + StrIdx);
  StrIdx +=  PMICVersionStrLen + 1;

  UnicodeStrToAsciiStr (StrSecureBootMode, OptionalStrStart + StrIdx);
  StrIdx +=  StrSecureBootModeLen + 1;

  UnicodeStrToAsciiStr (SecureBootMode, OptionalStrStart + StrIdx);
  StrIdx +=  SecureBootModeLen + 1;

  UnicodeStrToAsciiStr (StrBootMode, OptionalStrStart + StrIdx);
  StrIdx +=  StrBootModeLen + 1;

  UnicodeStrToAsciiStr (BootMode, OptionalStrStart + StrIdx);
  StrIdx +=  BootModeLen + 1;

  UnicodeStrToAsciiStr (StrSpeedStepMode, OptionalStrStart + StrIdx);
  StrIdx +=  StrSpeedStepModeLen + 1;

  UnicodeStrToAsciiStr (SpeedStepMode, OptionalStrStart + StrIdx);
  StrIdx +=  SpeedStepModeLen + 1;

  UnicodeStrToAsciiStr (StrCpuTurbo, OptionalStrStart + StrIdx);
  StrIdx +=  StrCpuTurboLen + 1;

  UnicodeStrToAsciiStr (CpuTurbo, OptionalStrStart + StrIdx);
  StrIdx +=  CpuTurboLen + 1;

  UnicodeStrToAsciiStr (StrCStateEn, OptionalStrStart + StrIdx);
  StrIdx +=  StrCStateEnLen + 1;

  UnicodeStrToAsciiStr (CStateEn, OptionalStrStart + StrIdx);
  StrIdx +=  CStateEnLen + 1;

  UnicodeStrToAsciiStr (StrEnhancedCStateEn, OptionalStrStart + StrIdx);
  StrIdx +=  StrEnhancedCStateEnLen + 1;

  UnicodeStrToAsciiStr (EnhancedCStateEn, OptionalStrStart + StrIdx);
  StrIdx +=  EnhancedCStateEnLen + 1;

  UnicodeStrToAsciiStr (StrMaxPkgCState, OptionalStrStart + StrIdx);
  StrIdx +=  StrMaxPkgCStateLen + 1;

  UnicodeStrToAsciiStr (MaxPkgCState, OptionalStrStart + StrIdx);
  StrIdx +=  MaxPkgCStateLen + 1;

  UnicodeStrToAsciiStr (StrMaxCoreCState, OptionalStrStart + StrIdx);
  StrIdx +=  StrMaxCoreCStateLen + 1;

  UnicodeStrToAsciiStr (MaxCoreCState, OptionalStrStart + StrIdx);
  StrIdx +=  MaxCoreCStateLen + 1;

  UnicodeStrToAsciiStr (StrGfxTurbo, OptionalStrStart + StrIdx);
  StrIdx +=  StrGfxTurboLen + 1;

  UnicodeStrToAsciiStr (GfxTurbo, OptionalStrStart + StrIdx);
  StrIdx +=  GfxTurboLen + 1;

  UnicodeStrToAsciiStr (StrS0ix, OptionalStrStart + StrIdx);
  StrIdx +=  StrS0ixLen + 1;

  UnicodeStrToAsciiStr (S0ix, OptionalStrStart + StrIdx);
  StrIdx +=  S0ixLen + 1;

  UnicodeStrToAsciiStr (StrRC6, OptionalStrStart + StrIdx);
  StrIdx +=  StrRC6Len + 1;

  UnicodeStrToAsciiStr (RC6, OptionalStrStart + StrIdx);
  StrIdx +=  RC6Len + 1;

  //
  // Now we have got the full smbios record, call smbios protocol to add this record.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = SmbiosProtocol->Add (
                             SmbiosProtocol,
                             NULL,
                             &SmbiosHandle,
                             (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                             );

  FreePool (SmbiosRecord);

  DEBUG ((EFI_D_INFO, "Executing SMBIOS T0x94 callback end.\n"));
  return;
}

/**
  This function makes boot time changes to the contents of the
  MiscOemType0x94 (Type 0x94).

  @param  RecordData                 Pointer to copy of RecordData from the Data Table.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_UNSUPPORTED            Unexpected RecordType value.
  @retval EFI_INVALID_PARAMETER      Invalid parameter was found.

**/

MISC_SMBIOS_TABLE_FUNCTION(MiscOemType0x94)
{
  EFI_STATUS                    Status;
  EFI_EVENT                     AddSmbiosT0x94CallbackEvent;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             AddSmbiosT0x94Callback,
             RecordData,
             &AddSmbiosT0x94CallbackEvent
             );

  ASSERT_EFI_ERROR (Status);
  return Status;
}

