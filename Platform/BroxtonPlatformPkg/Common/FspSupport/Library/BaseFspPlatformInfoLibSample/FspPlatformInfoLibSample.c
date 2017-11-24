/** @file
  Sample to provide FSP platform information related function.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FspWrapperApiLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/FspCommonLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/AcpiVariable.h>
#include <Guid/AcpiVariableCompatibility.h>
#include <Library/FspPlatformInfoLib.h>
#include <Ppi/DramPolicyPpi.h>
#include <FspmUpd.h>
#include <FspsUpd.h>
#include <BoardFunctionsPei.h>

#define  CAR_BASE_ADDR  0xFEF00000

STATIC
EFI_STATUS
UpdateSilicionInitUpd (
  IN EFI_PEI_SERVICES  **PeiServices,
  IN FSPS_UPD          *FspsUpd
  )
{
  return EFI_SUCCESS;
}


/**
  Get current boot mode.

  @note At this point, memory is ready, PeiServices are NOT available to use.
  Platform can get some data from chipset register.

  @return   BootMode     current boot mode.

**/
UINT32
EFIAPI
GetBootMode (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}


/**
  Get NVS buffer parameter.

  @note At this point, memory is NOT ready, PeiServices are available to use.

  @return   NvsBuffer    NVS buffer parameter.

**/
VOID *
EFIAPI
GetNvsBuffer (
  VOID
  )
{
  return NULL;
}


/**
  This function overrides the default configurations in the UPD data region.
  @note At this point, memory is NOT ready, PeiServices are available to use.

  @param[in,out] FspUpdRgnPtr   A pointer to the UPD data region data structure.

  @return        FspUpdRgnPtr   A pointer to the UPD data region data structure.

**/
VOID *
EFIAPI
UpdateFspUpdConfigs (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID        *FspUpdRgnPtr
  )
{
  FSP_INFO_HEADER             *pFspHeader = NULL;
  UINT32                      FspUpdRgnSize;
  FSPM_UPD                    *FspmUpd = NULL;
  EFI_BOOT_MODE               BootMode;
  UINT64                      StackSize;
  EFI_PHYSICAL_ADDRESS        StackBase;
  UPDATE_FSPM_UPD_FUNC        UpdateFspmUpdFunc;

  DEBUG ((DEBUG_INFO, "UpdateFspUpdConfigs\n"));
  if (NULL == FspUpdRgnPtr)
    return NULL;

  if (PcdGet32 (PcdFlashFvSecondFspBase) == 0) {
    pFspHeader = FspFindFspHeader (PcdGet32 (PcdFlashFvFspBase));
  } else {
    pFspHeader = FspFindFspHeader (PcdGet32 (PcdFlashFvSecondFspBase));
  }
  DEBUG ((DEBUG_INFO, "pFspHeader = %x\n", pFspHeader));
  if (NULL == pFspHeader) {
    return NULL;
  }

  DEBUG ((DEBUG_INFO, "pFspHeader->CfgRegionOffset = %08x\n", pFspHeader->CfgRegionOffset));
  FspUpdRgnSize = pFspHeader->CfgRegionSize;
  CopyMem (FspUpdRgnPtr, (void *) (pFspHeader->ImageBase + pFspHeader->CfgRegionOffset), FspUpdRgnSize);
  FspmUpd = (FSPM_UPD *)FspUpdRgnPtr;
  DEBUG ((DEBUG_INFO, "GetFspFspmUpdDataPointer - 0x%x\n", FspmUpd));

  FspmUpd->FspmConfig.SerialDebugPortDevice     = PcdGet8(PcdSerialIoUartNumber);
  FspmUpd->FspmConfig.SerialDebugPortType       = 2;// Skip the serial port init since we already did it
  FspmUpd->FspmConfig.SerialDebugPortStrideSize = 2;
  FspmUpd->FspmConfig.SerialDebugPortAddress    = 0;
  FspmUpd->FspmConfig.PreMemGpioTableEntryNum   = 0;
  FspmUpd->FspmConfig.PreMemGpioTablePtr        = 0;

  FspmUpd->FspmArchUpd.BootLoaderTolumSize = GetBootLoaderTolumSize ();
  DEBUG((DEBUG_INFO, "BootLoaderTolumSize - 0x%x\n", FspmUpd->FspmArchUpd.BootLoaderTolumSize));

  PeiServicesGetBootMode (&BootMode);
  FspmUpd->FspmArchUpd.BootMode = (UINT8) BootMode;
  DEBUG((DEBUG_INFO, "BootMode - 0x%x\n", FspmUpd->FspmArchUpd.BootMode));

  GetStackInfo (BootMode, FALSE, &StackSize, &StackBase);
  DEBUG ((DEBUG_INFO, "StackBase - 0x%x\n", StackBase));
  DEBUG ((DEBUG_INFO, "StackSize - 0x%x\n", StackSize));
  FspmUpd->FspmArchUpd.StackBase = (VOID *) (UINTN) StackBase;
  FspmUpd->FspmArchUpd.StackSize = (UINTN) StackSize;

  DEBUG ((DEBUG_INFO, "***** Calling UpdateFspmUpd ***** \n"));
  UpdateFspmUpdFunc = (UPDATE_FSPM_UPD_FUNC) (UINTN) PcdGet64 (PcdUpdateFspmUpdFunc);
  UpdateFspmUpdFunc(PeiServices, (FSPM_UPD *) FspmUpd);

  return (VOID *) FspmUpd;
}


/**
  Get BootLoader Tolum size.

  @note At this point, memory is NOT ready, PeiServices are available to use.

  @return   BootLoader    Tolum size.

**/
UINT32
EFIAPI
GetBootLoaderTolumSize (
  VOID
  )
{
  return 0;
}


/**
  Get TempRamExit parameter.

  @note At this point, memory is ready, PeiServices are available to use.

  @return   TempRamExit parameter.

**/
VOID *
EFIAPI
GetTempRamExitParam (
  VOID
  )
{
  return NULL;
}


/**
  Get FspSiliconInit parameter.

  @note At this point, memory is ready, PeiServices are available to use.

  @return   FspSiliconInit parameter.

**/
VOID *
EFIAPI
GetFspSiliconInitParam (
  VOID
  )
{
  return 0;
}


/**
  Get S3 PEI memory information.

  @note At this point, memory is ready, and PeiServices are available to use.
  Platform can get some data from SMRAM directly.

  @param[out]  S3PeiMemSize   PEI memory size to be installed in S3 phase.
  @param[out]  S3PeiMemBase   PEI memory base to be installed in S3 phase.

  @return      If S3 PEI memory information is got successfully.

**/
EFI_STATUS
EFIAPI
GetS3MemoryInfo (
  OUT UINT64               *S3PeiMemSize,
  OUT EFI_PHYSICAL_ADDRESS *S3PeiMemBase
  )
{
  ACPI_VARIABLE_SET               *AcpiVariableSet;
  EFI_PEI_HOB_POINTERS            GuidHob;
  EFI_PHYSICAL_ADDRESS            *AcpiVariableAddress;

  GuidHob.Raw = GetHobList ();
  AcpiVariableAddress = NULL;

  if (GuidHob.Raw != NULL) {
    if ((GuidHob.Raw = GetNextGuidHob (&gEfiAcpiVariableCompatiblityGuid, GuidHob.Raw)) != NULL) {
      AcpiVariableAddress = GET_GUID_HOB_DATA (GuidHob.Guid);
    }
  }

  DEBUG ((DEBUG_INFO, "InstallS3Memory() read AcpiVariableAddress\n"));
  if (AcpiVariableAddress != NULL) {
    DEBUG ((DEBUG_INFO, "0x%08x\n",(UINT32) ((UINTN) AcpiVariableAddress)));
    DEBUG ((DEBUG_INFO, "* 0x%08x\n",(UINT32) ((UINTN) *AcpiVariableAddress)));
    AcpiVariableSet = (ACPI_VARIABLE_SET *) ((UINTN) *AcpiVariableAddress);
    *S3PeiMemSize = (UINTN) (AcpiVariableSet->AcpiReservedMemorySize);
    *S3PeiMemBase = (UINTN) (AcpiVariableSet->AcpiReservedMemoryBase);
  } else {
    AcpiVariableSet = NULL;
    DEBUG ((DEBUG_INFO, " is NULL\n"));
  }

  return EFI_SUCCESS;
}


/**
  Get stack information according to boot mode.

  @note If BootMode is BOOT_ON_S3_RESUME or BOOT_ON_FLASH_UPDATE,
  this stack should be in some reserved memory space.

  @note If FspInitDone is TRUE, memory is ready, but no PeiServices there.
  Platform can get some data from SMRAM directly.
  @note If FspInitDone is FALSE, memory is NOT ready, but PeiServices are available to use.
  Platform can get some data from variable via VariablePpi.

  @param[in]  BootMode     Current boot mode.
  @param[in]  FspInitDone  If FspInit is called.
  @param[out] StackSize    Stack size to be used in PEI phase.
  @param[out] StackBase    Stack base to be used in PEI phase.

  @return    If Stack information is got successfully.
**/
EFI_STATUS
EFIAPI
GetStackInfo (
  IN  UINT32               BootMode,
  IN  BOOLEAN              FspInitDone,
  OUT UINT64               *StackSize,
  OUT EFI_PHYSICAL_ADDRESS *StackBase
  )
{
  *StackBase = PcdGet32 (PcdFspStackBase);
  *StackSize = PcdGet32 (PcdFspStackSize);

  if (BootMode == BOOT_ON_S3_RESUME) {
    if (!FspInitDone) {
    } else {
    }
  } else if (BootMode == BOOT_ON_FLASH_UPDATE) {
    if (!FspInitDone) {
    } else {
    }
  }

  return EFI_SUCCESS;
}

