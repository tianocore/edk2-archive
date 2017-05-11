/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "IgdOpRegion.h"
#include <FrameworkDxe.h>
#include <Uefi.h>
#include <ScAccess.h>
#include <Guid/DataHubRecords.h>
#include <Protocol/IgdOpRegion.h>
#include <Protocol/FrameworkHii.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Protocol/PciIo.h>
#include <Protocol/CpuIo.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/IoLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DriverBinding.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/ConfigBlockLib.h>

UINT8 gSVER[12] = "Intel";

extern SA_POLICY_PROTOCOL  *SaPolicy;

//
// Global variables
//

IGD_OPREGION_PROTOCOL mIgdOpRegion;
EFI_GUID              mMiscSubClass = EFI_MISC_SUBCLASS_GUID;
EFI_EVENT             mReadyToBootEvent;

#define DEFAULT_FORM_BUFFER_SIZE    0xFFFF

/**
  Get VBT data.

  @param[out]  VbtFileBuffer          Pointer to VBT data buffer.

  @retval      EFI_SUCCESS            VBT data was returned.
  @retval      EFI_NOT_FOUND          VBT data not found.
  @retval      EFI_UNSUPPORTED        Invalid signature in VBT data.

**/
EFI_STATUS
GetIntegratedIntelVbtPtr (
  OUT VBIOS_VBT_STRUCTURE **VbtFileBuffer
  )
{
  EFI_PEI_HOB_POINTERS     GuidHob;
  VBT_INFO                 *VbtInfo = NULL;
  EFI_STATUS               Status;
  EFI_PHYSICAL_ADDRESS     VbtAddress = 0;
  UINT32                   VbtSize = 0;

  Status = EFI_NOT_FOUND;
  //
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  GuidHob.Raw = GetHobList ();
  if (GuidHob.Raw != NULL) {
   if ((GuidHob.Raw = GetNextGuidHob (&gVbtInfoGuid, GuidHob.Raw)) != NULL) {
     VbtInfo = GET_GUID_HOB_DATA (GuidHob.Guid);
     VbtAddress = VbtInfo->VbtAddress;
     VbtSize = VbtInfo->VbtSize;
     Status = EFI_SUCCESS;
   }
  }

  DEBUG ((EFI_D_INFO, "VbtAddress =0x%x \n", VbtAddress));
  DEBUG ((EFI_D_INFO, "VbtSize = 0x%x.\n", VbtSize));

  //
  // Check VBT signature
  //
  *VbtFileBuffer = (VBIOS_VBT_STRUCTURE *) (UINTN) VbtAddress;
  if (*VbtFileBuffer != NULL) {
    if ((*((UINT32 *) ((*VbtFileBuffer)->HeaderSignature))) != VBT_SIGNATURE) {
      if (*VbtFileBuffer != NULL) {
        *VbtFileBuffer = NULL;
      }
      return EFI_UNSUPPORTED;
    }
    //
    // Check VBT size
    //
    if ((*VbtFileBuffer)->HeaderVbtSize > VbtSize) {
      (*VbtFileBuffer)->HeaderVbtSize = (UINT16) VbtSize;
    }
  }

  return Status;
}


/**
  Get a pointer to an uncompressed image of the Intel video BIOS.
  Note: This function would only be called if the video BIOS at 0xC000 is
        missing or not an Intel video BIOS.  It may not be an Intel video BIOS
        if the Intel graphic contoller is considered a secondary adapter.

  @param[Out]  VBiosImage           Pointer to an uncompressed Intel video BIOS.  This pointer must
                                    be set to NULL if an uncompressed image of the Intel Video BIOS
                                    is not obtainable.

  @retval      EFI_SUCCESS          VBiosPtr is updated.
  @retval      EFI_UNSUPPORTED      No Intel video BIOS found.

**/
EFI_STATUS
GetIntegratedIntelVBiosPtr (
  OUT INTEL_VBIOS_OPTION_ROM_HEADER **VBiosImage
  )
{
  EFI_HANDLE                     *HandleBuffer;
  UINTN                          HandleCount;
  UINTN                          Index;
  INTEL_VBIOS_PCIR_STRUCTURE     *PcirBlockPtr;
  EFI_STATUS                     Status;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  INTEL_VBIOS_OPTION_ROM_HEADER  *VBiosRomImage;

  //
  // Set as if an umcompressed Intel video BIOS image was not obtainable.
  //
  VBiosRomImage = NULL;
  *VBiosImage = NULL;

  //
  // Get all PCI IO protocols
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Find the video BIOS by checking each PCI IO handle for an Intel video
  // BIOS OPROM.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    VBiosRomImage = PciIo->RomImage;

    //
    // If this PCI device doesn't have a ROM image, skip to the next device.
    //
    if (!VBiosRomImage) {
      continue;
    }

    //
    // Get pointer to PCIR structure
    //
    PcirBlockPtr = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosRomImage + VBiosRomImage->PcirOffset);

    //
    // Check if we have an Intel video BIOS OPROM.
    //
    if ((VBiosRomImage->Signature == OPTION_ROM_SIGNATURE) &&
        (PcirBlockPtr->VendorId == V_SA_MC_VID) &&
        (PcirBlockPtr->ClassCode[0] == 0x00) &&
        (PcirBlockPtr->ClassCode[1] == 0x00) &&
        (PcirBlockPtr->ClassCode[2] == 0x03)
       ) {
      //
      // Found Intel video BIOS.
      //
      *VBiosImage = VBiosRomImage;
      return EFI_SUCCESS;
    }
  }

  //
  // No Intel video BIOS found.
  //

  return EFI_UNSUPPORTED;
}


EFI_STATUS
SearchChildHandle(
  IN EFI_HANDLE Father,
  OUT EFI_HANDLE *Child
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
  UINTN                                mHandleCount;
  EFI_HANDLE                           *mHandleBuffer= NULL;

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
JudgeHandleIsPCIDevice(
  IN EFI_HANDLE            Handle,
  IN UINT8                 Device,
  IN UINT8                 Funs
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
GetDriverName(
  IN  EFI_HANDLE   Handle,
  OUT CHAR16       *GopVersion
  )
{
  EFI_DRIVER_BINDING_PROTOCOL           *BindHandle = NULL;
  EFI_STATUS                            Status;
  UINT32                                Version;
  UINT16                                *Ptr;

  Status = gBS->OpenProtocol(
                  Handle,
                  &gEfiDriverBindingProtocolGuid,
                  (VOID **) &BindHandle,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Version = BindHandle->Version;
  Ptr = (UINT16 *) &Version;
  UnicodeSPrint (GopVersion, 40, L"%d.%d.%d", Version >> 24 , (Version >> 16) & 0x0f ,*(Ptr));

  return EFI_SUCCESS;
}


EFI_STATUS
GetGOPDriverVersion(
  OUT CHAR16 *GopVersion
  )
{
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  UINTN                 Index;
  EFI_STATUS            Status;
  EFI_HANDLE            Child = 0;

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
        return GetDriverName (Handles[Index], GopVersion);
      }
    }
  }

  return EFI_UNSUPPORTED;
}


/**
  Get Intel GOP driver version and copy it into IGD OpRegion GVER. This version
  is picked up by IGD driver and displayed in CUI.

  @param[in]  Event                 A pointer to the Event that triggered the callback.
  @param[in]  Context               A pointer to private data registered with the callback function.

  @retval     EFI_SUCCESS           GOP driver version returned.
  @retval     EFI_UNSUPPORTED       Could not Get GOP driver version.

**/
EFI_STATUS
SetGOPVersionCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  CHAR16                GopVersion[16] = {0};
  EFI_STATUS            Status;

  DEBUG ((EFI_D_INFO, "SetGOPVersionCallback\n"));

  Status = GetGOPDriverVersion (GopVersion);
  if (!EFI_ERROR (Status)) {
    StrCpyS ((CHAR16 *) &(mIgdOpRegion.OpRegion->Header.DVER[0]), 0x10, GopVersion);
    DEBUG ((EFI_D_INFO, "GopVersion = %s\n", GopVersion));
    return Status;
  }

  return EFI_UNSUPPORTED;
}


/**
  Get Intel video BIOS VBT information (i.e. Pointer to VBT and VBT size).
  The VBT (Video BIOS Table) is a block of customizable data that is built
  within the video BIOS and edited by customers.

  @param[in]  Event             A pointer to the Event that triggered the callback.
  @param[in]  Context           A pointer to private data registered with the callback function.

  @retval     EFI_SUCCESS       Video BIOS VBT information returned.
  @retval     EFI_UNSUPPORTED   Could not find VBT information (*VBiosVbtPtr = NULL).

**/
VOID
EFIAPI
GetVBiosVbtCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  INTEL_VBIOS_PCIR_STRUCTURE     *PcirBlockPtr;
  UINT16                         PciVenderId;
  INTEL_VBIOS_OPTION_ROM_HEADER  *VBiosPtr;
  VBIOS_VBT_STRUCTURE            *VBiosVbtPtr;
  VBIOS_VBT_STRUCTURE            *VbtFileBuffer = NULL;
  VBIOS_VBT_STRUCTURE            *ExtVbtPtr     = NULL;
  EFI_STATUS                     Status;

  VBiosPtr = (INTEL_VBIOS_OPTION_ROM_HEADER *) (UINTN) (VBIOS_LOCATION_PRIMARY);
  PcirBlockPtr = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->PcirOffset);
  PciVenderId = PcirBlockPtr->VendorId;

  if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)) {
    GetIntegratedIntelVBiosPtr (&VBiosPtr);

    if (VBiosPtr) {
      //
      // Video BIOS found.
      //
      PcirBlockPtr = (INTEL_VBIOS_PCIR_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->PcirOffset);
      PciVenderId = PcirBlockPtr->VendorId;
      if ((VBiosPtr->Signature != OPTION_ROM_SIGNATURE) || (PciVenderId != V_SA_MC_VID)){
        //
        // Intel video BIOS not found.
        //
        VBiosVbtPtr = NULL;
        DEBUG ((EFI_D_ERROR, "Intel video BIOS not found\n"));
        return;
      }
    } else {
      //
      // No Video BIOS found, try to get VBT from FV.
      //
      GetIntegratedIntelVbtPtr (&VbtFileBuffer);
      if (VbtFileBuffer != NULL) {
        //
        // Video BIOS not found, use VBT from FV
        //
        DEBUG ((EFI_D_INFO, "VBT data found\n"));
        DEBUG ((EFI_D_INFO, "VbtFileBuffer->HeaderVbtSize = 0x%x \n", VbtFileBuffer->HeaderVbtSize));
        mIgdOpRegion.OpRegion->MBox3.RVDA = 0;
        mIgdOpRegion.OpRegion->MBox3.RVDS = 0;
        //
        // extended VBT support
        //
        if (VbtFileBuffer->HeaderVbtSize > 6 * SIZE_1KB) {
        DEBUG ((EFI_D_INFO, "extended VBT supported\n"));

        Status = gBS->AllocatePool (
                        EfiACPIMemoryNVS,
                        VbtFileBuffer->HeaderVbtSize,
                        (VOID **) &ExtVbtPtr
                        );

        ASSERT_EFI_ERROR (Status);

        gBS->SetMem (ExtVbtPtr, VbtFileBuffer->HeaderVbtSize, 0);
        mIgdOpRegion.OpRegion->MBox3.RVDA = (UINT64) ExtVbtPtr;
        mIgdOpRegion.OpRegion->MBox3.RVDS = VbtFileBuffer->HeaderVbtSize;
        gBS->CopyMem ((VOID *) (UINTN) mIgdOpRegion.OpRegion->MBox3.RVDA, (VOID *) (UINTN) VbtFileBuffer, mIgdOpRegion.OpRegion->MBox3.RVDS);

        } else {
          gBS->CopyMem (mIgdOpRegion.OpRegion->VBT.GVD1, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
          mIgdOpRegion.OpRegion->MBox3.RVDA = (UINT64) (mIgdOpRegion.OpRegion->VBT.GVD1);
          mIgdOpRegion.OpRegion->MBox3.RVDS = VbtFileBuffer->HeaderVbtSize;
        } // End of Size condition
        return;
      }
    }
    if ((VBiosPtr == NULL)) {
      //
      // Intel video BIOS not found.
      //
      VBiosVbtPtr = NULL;
      DEBUG ((EFI_D_ERROR, "Intel video BIOS not found\n"));
      return;
    }
  }

  DEBUG ((EFI_D_INFO, "VBIOS found at 0x%X\n", VBiosPtr));
  VBiosVbtPtr = (VBIOS_VBT_STRUCTURE *) ((UINT8 *) VBiosPtr + VBiosPtr->VbtOffset);

  if ((*((UINT32 *) (VBiosVbtPtr->HeaderSignature))) != VBT_SIGNATURE) {
    DEBUG ((EFI_D_ERROR, "Signature is NOT match\n"));
    return;
  }

  //
  // Initialize Video BIOS version with its build number.
  //
  mIgdOpRegion.OpRegion->Header.VVER[0] = VBiosVbtPtr->CoreBlockBiosBuild[0];
  mIgdOpRegion.OpRegion->Header.VVER[1] = VBiosVbtPtr->CoreBlockBiosBuild[1];
  mIgdOpRegion.OpRegion->Header.VVER[2] = VBiosVbtPtr->CoreBlockBiosBuild[2];
  mIgdOpRegion.OpRegion->Header.VVER[3] = VBiosVbtPtr->CoreBlockBiosBuild[3];
  (gBS->CopyMem) (mIgdOpRegion.OpRegion->VBT.GVD1, VBiosVbtPtr, VBiosVbtPtr->HeaderVbtSize);

  //
  // Return final status
  //
  return;
}


/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void            None

  @retval    EFI_SUCCESS     The driver installed without error.
  @retval    EFI_ABORTED     The driver encountered an error and could not complete
                             installation of the ACPI tables.

**/
EFI_STATUS
IgdOpRegionInit (
  void
  )
{
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
  UINT32                        DwordData;
  EFI_CPU_IO2_PROTOCOL          *CpuIo;
  UINT16                        Data16;
  UINT16                        AcpiBaseAddr;
  SA_DXE_MISC_CONFIG           *SaDxeMiscConfig = NULL;
  IGD_PANEL_CONFIG             *IgdPanelConfig  = NULL;

  //
  // Get the platform setup policy.
  //
  Status = gBS->LocateProtocol (&gSaPolicyProtocolGuid, NULL, (VOID **) &SaPolicy);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gIgdPanelConfigGuid, (VOID *) &IgdPanelConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gSaDxeMiscConfigGuid, (VOID *) &SaDxeMiscConfig);
  ASSERT_EFI_ERROR (Status);

  //
  //  Locate the Global NVS Protocol.
  //
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &GlobalNvsArea
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  sizeof (IGD_OPREGION_STRUC),
                  (VOID **) &mIgdOpRegion.OpRegion
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->SetMem (mIgdOpRegion.OpRegion, sizeof (IGD_OPREGION_STRUC), 0);
  GlobalNvsArea->Area->IgdOpRegionAddress = (UINT32) (UINTN) (mIgdOpRegion.OpRegion);

#if (ENBDT_PF_ENABLE == 1)
  GlobalNvsArea->Area->PanelSelect = IgdPanelConfig->PanelSelect;
#endif

  //
  // If IGD is disabled return
  //
  if (IgdMmPci32 (0) == 0xFFFFFFFF) {
    return EFI_SUCCESS;
  }

  //
  // Initialize OpRegion Header
  //
  gBS->CopyMem (mIgdOpRegion.OpRegion->Header.SIGN, HEADER_SIGNATURE, sizeof (HEADER_SIGNATURE));

  //
  // Set OpRegion Size in KBs
  //
  mIgdOpRegion.OpRegion->Header.SIZE = HEADER_SIZE / 1024;

  //
  // Note: Need to check Header OVER Field and the supported version.
  //
  mIgdOpRegion.OpRegion->Header.OVER = (UINT32) (LShiftU64 (HEADER_OPREGION_VER, 16) + LShiftU64 (HEADER_OPREGION_REV, 8));
  gBS->CopyMem (mIgdOpRegion.OpRegion->Header.SVER, gSVER, sizeof (gSVER));

  DEBUG ((EFI_D_INFO, "System BIOS ID is %a\n", mIgdOpRegion.OpRegion->Header.SVER));

  mIgdOpRegion.OpRegion->Header.MBOX = HEADER_MBOX_SUPPORT;

  if (1 == SaDxeMiscConfig->S0ixSupported) {
    mIgdOpRegion.OpRegion->Header.PCON = (mIgdOpRegion.OpRegion->Header.PCON & 0xFFFC) | BIT1;
    DEBUG ((EFI_D_INFO, "S0ix Supported!\n"));
  } else {
    mIgdOpRegion.OpRegion->Header.PCON = (mIgdOpRegion.OpRegion->Header.PCON & 0xFFFC) | (BIT1 | BIT0);
    DEBUG ((EFI_D_INFO, "S0ix not Supported!\n"));
  }

  //
  // Identify if LPE Audio/HD Audio is enabled on the platform
  //
  mIgdOpRegion.OpRegion->Header.PCON &= AUDIO_TYPE_SUPPORT_MASK;
  mIgdOpRegion.OpRegion->Header.PCON &= AUDIO_TYPE_FIELD_MASK;
  if (NO_AUDIO == SaDxeMiscConfig->AudioTypeSupport) {
    mIgdOpRegion.OpRegion->Header.PCON |=NO_AUDIO_SUPPORT;
    DEBUG ((EFI_D_ERROR, "No Audio Support!\n"));
    mIgdOpRegion.OpRegion->Header.PCON |= AUDIO_TYPE_FIELD_VALID;
  } else if ( HD_AUDIO== SaDxeMiscConfig->AudioTypeSupport ) {
    mIgdOpRegion.OpRegion->Header.PCON |= HD_AUDIO_SUPPORT;
    DEBUG ((EFI_D_INFO, "HD Audio Support!\n"));
    mIgdOpRegion.OpRegion->Header.PCON |= AUDIO_TYPE_FIELD_VALID;
  } else if ( LPE_AUDIO == SaDxeMiscConfig->AudioTypeSupport ) {
    mIgdOpRegion.OpRegion->Header.PCON |= LPE_AUDIO_SUPPORT;
    DEBUG ((EFI_D_INFO, "LPE Audio Support!\n"));
    mIgdOpRegion.OpRegion->Header.PCON |= AUDIO_TYPE_FIELD_VALID;
  } else {
    //
    // Audio type support field is not valid
    //
    mIgdOpRegion.OpRegion->Header.PCON |= AUDIO_TYPE_FIELD_INVALID;
    DEBUG ((EFI_D_ERROR, "Audio type support field is not valid!\n"));
  }

  mIgdOpRegion.OpRegion->MBox1.CLID = 1;
  mIgdOpRegion.OpRegion->MBox3.BCLP = BACKLIGHT_BRIGHTNESS;
  mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);

  if (IgdPanelConfig->PFITStatus == 2) {
    //
    // Center
    //
    mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_CENTER);
  } else if (IgdPanelConfig->PFITStatus == 1) {
    //
    // Stretch
    //
    mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);
  } else {
    //
    // Auto
    //
    mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_SETUP_AUTO);
  }

  //
  // Set Initial current Brightness
  //
  mIgdOpRegion.OpRegion->MBox3.CBLV = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);
  mIgdOpRegion.OpRegion->MBox3.BCLM[0]  = (0x0000 + WORD_FIELD_VALID_BIT);  // 0%
  mIgdOpRegion.OpRegion->MBox3.BCLM[1]  = (0x0A19 + WORD_FIELD_VALID_BIT);  // 10%
  mIgdOpRegion.OpRegion->MBox3.BCLM[2]  = (0x1433 + WORD_FIELD_VALID_BIT);  // 20%
  mIgdOpRegion.OpRegion->MBox3.BCLM[3]  = (0x1E4C + WORD_FIELD_VALID_BIT);  // 30%
  mIgdOpRegion.OpRegion->MBox3.BCLM[4]  = (0x2866 + WORD_FIELD_VALID_BIT);  // 40%
  mIgdOpRegion.OpRegion->MBox3.BCLM[5]  = (0x327F + WORD_FIELD_VALID_BIT);  // 50%
  mIgdOpRegion.OpRegion->MBox3.BCLM[6]  = (0x3C99 + WORD_FIELD_VALID_BIT);  // 60%
  mIgdOpRegion.OpRegion->MBox3.BCLM[7]  = (0x46B2 + WORD_FIELD_VALID_BIT);  // 70%
  mIgdOpRegion.OpRegion->MBox3.BCLM[8]  = (0x50CC + WORD_FIELD_VALID_BIT);  // 80%
  mIgdOpRegion.OpRegion->MBox3.BCLM[9]  = (0x5AE5 + WORD_FIELD_VALID_BIT);  // 90%
  mIgdOpRegion.OpRegion->MBox3.BCLM[10] = (0x64FF + WORD_FIELD_VALID_BIT);  // 100%

  //
  // Create the notification and register callback function
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             GetVBiosVbtCallback,
             NULL,
             &mReadyToBootEvent
             );
  //
  // Set GOP version in IGD Op-Region.
  //
  Status = EfiCreateEventReadyToBootEx(
             TPL_CALLBACK,
             (EFI_EVENT_NOTIFY)SetGOPVersionCallback,
             NULL,
             &mReadyToBootEvent
             );

  IgdMmPci32 (IGD_ASLS_OFFSET) = (UINT32) (UINTN) (mIgdOpRegion.OpRegion);
  IgdMmPci16AndThenOr (IGD_SWSCI_OFFSET, ~(BIT0), BIT15);

  DwordData = IgdMmPci32 (IGD_ASLS_OFFSET);
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint32,
    (UINTN) (EFI_PCI_ADDRESS  (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, IGD_ASLS_OFFSET)),
    1,
    &DwordData
    );

  DwordData = IgdMmPci32 (IGD_SWSCI_OFFSET);
  S3BootScriptSavePciCfgWrite (
    S3BootScriptWidthUint32,
    (UINTN) (EFI_PCI_ADDRESS  (SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, IGD_SWSCI_OFFSET)),
    1,
    &DwordData
    );

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Find the CPU I/O Protocol.  ASSERT if not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  (VOID **) &CpuIo
                  );
  ASSERT_EFI_ERROR (Status);

  CpuIo->Io.Read (
              CpuIo,
              EfiCpuIoWidthUint16,
              AcpiBaseAddr + R_ACPI_GPE0a_STS,
              1,
              &Data16
              );

  Data16 |= B_ACPI_GPE0a_STS_GUNIT_SCI;

  CpuIo->Io.Write (
              CpuIo,
              EfiCpuIoWidthUint16,
              AcpiBaseAddr + R_ACPI_GPE0a_STS,
              1,
              &Data16
              );

  //
  // Install OpRegion / Software SCI protocol
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gIgdOpRegionProtocolGuid,
                  &mIgdOpRegion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Return final status
  //
  return EFI_SUCCESS;
}

