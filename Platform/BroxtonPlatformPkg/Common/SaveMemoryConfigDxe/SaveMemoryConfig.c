/** @file
  This is the driver that locates the MemoryConfigurationData HOB, if it
  exists, and saves the data to NVRAM.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PlatformBootMode.h>
#include <Protocol/SetupMode.h>
#include <Protocol/MemInfo.h>
#include <Guid/PlatformInfo.h>
#include <Guid/HobList.h>
#include <Guid/MemoryConfigData.h>
#include <Guid/GlobalVariable.h>
#include <Guid/BxtVariable.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include "MmrcData.h"

#define EFI_UNLOAD_IMAGE   EFIERR(29)

GLOBAL_REMOVE_IF_UNREFERENCED CHAR16  mMemoryConfigVariable[] = L"MemoryConfig";
GLOBAL_REMOVE_IF_UNREFERENCED CHAR16  mMemoryBootVariable[]   = L"MemoryBootData";

VOID
PrintBinaryBuffer (
  IN        UINT8*      Buffer,
  IN        UINTN       BufferSize
)
{
  UINTN    CurrentByte = 0;

  if (BufferSize == 0) {
    DEBUG ((EFI_D_INFO, "Skipping print of 0 size buffer\n"));
    return;
  }
  DEBUG ((EFI_D_INFO, "Base  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n"));
  DEBUG ((EFI_D_INFO, "%4d %2x ", CurrentByte/16, Buffer[0]));
  for (CurrentByte = 1; CurrentByte < BufferSize; CurrentByte++) {
    if ((CurrentByte % 16) == 0) {
      DEBUG ((EFI_D_INFO, "\n%4d %2x ", CurrentByte/16, Buffer[CurrentByte]));
    } else {
      DEBUG ((EFI_D_INFO, "%2x ", Buffer[CurrentByte]));
    }
  }
  DEBUG ((EFI_D_INFO, "\n"));

  return;
}


/**
  This function takes in an gEfiMemoryConfigVariableGuid Variable name and a pointer
  to the data that will be saved. The data is first compared against the currently
  stored variable for differences. If no difference is found, the save will be skipped.

  @param[in]  VariableName       The variable name to save using the Variable Service.
  @param[in]  Buffer             Pointer to the data to save
  @param[in]  BufferSize         The size of the data to save

  @retval     EFI_SUCCESS        If the data is successfully saved or there was no data
  @retval     EFI_UNLOAD_IMAGE   It is not success

**/
EFI_STATUS
SaveMrcData(
  IN        CHAR16      VariableName[],
  IN        UINT8*      Buffer,
  IN        UINTN       BufferSize
)
{
  EFI_STATUS               Status;
  VOID                     *CurrentVariableData = NULL;
  UINTN                    CurrentVariableSize  = 0;
  BOOLEAN                  SaveConfig           = FALSE;

  CurrentVariableSize = 1;

  CurrentVariableData = AllocatePool (CurrentVariableSize);

  DEBUG ((EFI_D_INFO, "SaveMrcData %s\n", VariableName));

  if (CurrentVariableData == NULL) {
    ASSERT (CurrentVariableData != NULL);
    return EFI_UNLOAD_IMAGE;
  }

  Status = gRT->GetVariable (
                  VariableName,
                  &gEfiMemoryConfigVariableGuid,
                  NULL,
                  &CurrentVariableSize,
                  CurrentVariableData
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (CurrentVariableData);
    CurrentVariableData = AllocatePool (CurrentVariableSize);

    if (CurrentVariableData == NULL) {
        DEBUG ((EFI_D_ERROR, "AllocatePool failed\n"));
        return EFI_UNLOAD_IMAGE;
    }

    Status = gRT->GetVariable (
                    VariableName,
                    &gEfiMemoryConfigVariableGuid,
                    NULL,
                    &CurrentVariableSize,
                    CurrentVariableData
                    );
  }

  //
  // The Memory Configuration will not exist during first boot or if memory/MRC
  // firmware changes have been made.
  //
  if (EFI_ERROR (Status) || CompareMem (Buffer, CurrentVariableData, BufferSize)) {
    SaveConfig = TRUE;
    DEBUG ((EFI_D_INFO, "GetVariable Status: %r\n", Status));
    DEBUG ((EFI_D_INFO, "Saving %s\n", VariableName));
  } else {
    DEBUG ((EFI_D_INFO, "Skip %s\n", VariableName));
  }

  if (SaveConfig) {
    Status = gRT->SetVariable (
                    VariableName,
                    &gEfiMemoryConfigVariableGuid,
                    (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS),
                    BufferSize,
                    (UINT8 *) Buffer
                    );

    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Failed to write %s\n", VariableName));
      ASSERT_EFI_ERROR (Status);
    }
  }

  return EFI_SUCCESS;
}


/**
  This is the standard EFI driver point that detects whether there is a
  MemoryConfigurationData HOB and, if so, saves its data to nvRAM.

  @param[in]  ImageHandle        Handle for the image of this driver
  @param[in]  SystemTable        Pointer to the EFI System Table

  @retval     EFI_SUCCESS        If the data is successfully saved or there was no data
  @retval     EFI_NOT_FOUND      If the HOB list could not be located.
  @retval     EFI_UNLOAD_IMAGE   It is not success

**/
EFI_STATUS
EFIAPI
SaveMemoryConfigEntryPoint (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS               Status;
  EFI_HANDLE               Handle                       = NULL;
  EFI_HOB_GUID_TYPE        *GuidHob                     = NULL;
  EFI_HOB_GUID_TYPE        *VariableGuidHob             = NULL;
  EFI_PLATFORM_INFO_HOB    *PlatformInfoHob             = NULL;
  EFI_PLATFORM_SETUP_ID    *BootModeBuffer              = NULL;
  MEM_INFO_PROTOCOL        *MemInfoHobProtocol          = NULL;
  MRC_PARAMS_SAVE_RESTORE  *MemoryConfigHobData         = NULL;
  BOOT_VARIABLE_NV_DATA    *VariableNvHobData           = NULL;
  UINTN                    MemoryConfigHobDataSize      = 0;
  UINT8                    Channel                      = 0;
  UINT8                    Slot                         = 0;

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);

  if (GuidHob == NULL) {
    ASSERT (GuidHob != NULL);
    return EFI_NOT_FOUND;
  }

  PlatformInfoHob = GET_GUID_HOB_DATA (GuidHob);

  GuidHob = GetFirstGuidHob (&gEfiPlatformBootModeGuid);

  if (GuidHob == NULL) {
    ASSERT (GuidHob != NULL);
    return EFI_NOT_FOUND;
  }

  BootModeBuffer = GET_GUID_HOB_DATA (GuidHob);

  if (!CompareMem (&BootModeBuffer->SetupName, MANUFACTURE_SETUP_NAME, StrSize (MANUFACTURE_SETUP_NAME))) {
    //
    // Don't save Memory Configuration in Manufacturing Mode.
    // Clear memory configuration.
    //
    DEBUG ((EFI_D_INFO, "Invalidating the MRC SaveParam Data for MfgMode...\n"));

    //
    // This driver does not produce any protocol services, so always unload it.
    //
    return EFI_UNLOAD_IMAGE;
  }

  //
  // Search for the Memory Configuration GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  if ((GuidHob = GetFirstGuidHob (&gFspNonVolatileStorageHobGuid)) != NULL) {
    MemoryConfigHobData = GET_GUID_HOB_DATA (GuidHob);
    MemoryConfigHobDataSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
  }

  if (MemoryConfigHobData == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((VariableGuidHob = GetFirstGuidHob (&gFspVariableNvDataHobGuid)) != NULL) {
    VariableNvHobData = GET_GUID_HOB_DATA(VariableGuidHob);
  }

  if (VariableNvHobData == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Populate and install the MemInfoHobProtocol
  //
  MemInfoHobProtocol = (MEM_INFO_PROTOCOL*) AllocateZeroPool (sizeof (MEM_INFO_PROTOCOL));

  if (MemInfoHobProtocol != NULL) {
    MemInfoHobProtocol->MemInfoData.memSize = 0;

    for (Channel = 0; Channel < CH_NUM; Channel++) {
      for (Slot = 0; Slot < DIMM_NUM; Slot++) {
        MemInfoHobProtocol->MemInfoData.memSize += MemoryConfigHobData->Channel[Channel].SlotMem[Slot];
        MemInfoHobProtocol->MemInfoData.dimmSize[Slot + (Channel * DIMM_NUM)] = MemoryConfigHobData->Channel[Channel].SlotMem[Slot];
        MemInfoHobProtocol->MemInfoData.DimmPresent[Slot + (Channel * DIMM_NUM)] = MemoryConfigHobData->Channel[Channel].DimmPresent[Slot];
        if (MemInfoHobProtocol->MemInfoData.DimmPresent[Slot + (Channel * DIMM_NUM)]) {
          MemInfoHobProtocol->MemInfoData.DimmsSpdData[Slot + (Channel * DIMM_NUM)] = MemoryConfigHobData->Channel[Channel].SpdData[Slot].Buffer;
        } else {
          MemInfoHobProtocol->MemInfoData.DimmsSpdData[Slot + (Channel * DIMM_NUM)] = NULL;
        }
      }
    }

    MemInfoHobProtocol->MemInfoData.ddrFreq   = MemoryConfigHobData->CurrentFrequency;
    MemInfoHobProtocol->MemInfoData.memSize   = MemoryConfigHobData->SystemMemorySize;
    MemInfoHobProtocol->MemInfoData.ddrType   = MemoryConfigHobData->Channel[0].DramType;
    MemInfoHobProtocol->MemInfoData.BusWidth  = MemoryConfigHobData->BusWidth;

    DEBUG ((EFI_D_INFO, "SaveMemoryConfigEntryPoint - Freq:0x%x\n", MemInfoHobProtocol->MemInfoData.ddrFreq));
    DEBUG ((EFI_D_INFO, "SaveMemoryConfigEntryPoint - Memsize:0x%x\n", MemInfoHobProtocol->MemInfoData.memSize));

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gMemInfoProtocolGuid,
                    MemInfoHobProtocol,
                    NULL
                    );
  }

  Status = SaveMrcData(mMemoryConfigVariable, (UINT8 *) MemoryConfigHobData, sizeof(MRC_PARAMS_SAVE_RESTORE));
  if (EFI_ERROR(Status)){
    return Status;
  }

  Status = SaveMrcData(mMemoryBootVariable, (UINT8 *) VariableNvHobData, sizeof(BOOT_VARIABLE_NV_DATA));
  if (EFI_ERROR(Status)){
    return Status;
  }

  //
  // This driver does not produce any protocol services, so always unload it.
  //
  return EFI_UNLOAD_IMAGE;
}

