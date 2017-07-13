/** @file
  This file does Multiplatform initialization.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitMiscs.h"

/**
  Configure GPIO group GPE tier.

  @param[in]  PlatformInfo

  @retval     none.

**/
VOID
LeafHillGpioGroupTierInit (
  IN EFI_PLATFORM_INFO_HOB  *PlatformInfoHob
  )
{
  DEBUG ((DEBUG_INFO, "LeafHillGpioGroupTierInit Start\n"));
  switch (PlatformInfoHob->BoardId) {
    default:
      GpioSetGroupToGpeDwX (GPIO_BXTP_GROUP_7,  // map group 7 to GPE 0 ~ 31
                            GPIO_BXTP_GROUP_0,  // map group 0 to GPE 32 ~ 63 // We don't have SCI pin in Group0 as of now, but still need to assign a unique group to this field.
                            GPIO_BXTP_GROUP_1); // map group 1 to GPE 64 ~ 95 // We don't have SCI pin in Group1 as of now, but still need to assign a unique group to this field.
      break;
  }

  DEBUG ((DEBUG_INFO, "LeafHillGpioGroupTierInit End\n"));
}


EFI_STATUS
EFIAPI
LeafHillMultiPlatformInfoInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB  *PlatformInfoHob
  )
{
  EFI_STATUS             Status;

#if (ENBDT_PF_ENABLE == 1)
   DEBUG ((EFI_D_INFO, "Platform BoardId:%x FabId%x\n", PlatformInfoHob->BoardId, PlatformInfoHob->FABID));
#endif

  //
  // Device ID
  //
  PlatformInfoHob->IohSku = MmPci16 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PCI_DEVICE_ID_OFFSET);
  PlatformInfoHob->IohRevision = MmPci8 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PCI_REVISION_ID_OFFSET);

  //
  // Don't support BASE above 4GB currently
  //
  PlatformInfoHob->PciData.PciExpressSize     = 0x04000000;
  PlatformInfoHob->PciData.PciExpressBase     = (UINTN) PcdGet64 (PcdPciExpressBaseAddress);

  PlatformInfoHob->PciData.PciResourceMem32Base  = (UINT32) (PlatformInfoHob->PciData.PciExpressBase - RES_MEM32_MIN_LEN);
  PlatformInfoHob->PciData.PciResourceMem32Limit = (UINT32) (PlatformInfoHob->PciData.PciExpressBase -1);

  PlatformInfoHob->PciData.PciResourceMem64Base   = RES_MEM64_36_BASE;
  PlatformInfoHob->PciData.PciResourceMem64Limit  = RES_MEM64_36_LIMIT;
  PlatformInfoHob->CpuData.CpuAddressWidth        = 36;

  PlatformInfoHob->MemData.MemMir0 = PlatformInfoHob->PciData.PciResourceMem64Base;
  PlatformInfoHob->MemData.MemMir1 = PlatformInfoHob->PciData.PciResourceMem64Limit + 1;

  PlatformInfoHob->PciData.PciResourceMinSecBus  = 1;  //can be changed by SystemConfiguration->PciMinSecondaryBus;

  //
  // Set MemMaxTolm to the lowest address between PCIe Base and PCI32 Base
  //
  if (PlatformInfoHob->PciData.PciExpressBase > PlatformInfoHob->PciData.PciResourceMem32Base ) {
    PlatformInfoHob->MemData.MemMaxTolm = (UINT32) PlatformInfoHob->PciData.PciResourceMem32Base;
  } else {
    PlatformInfoHob->MemData.MemMaxTolm = (UINT32) PlatformInfoHob->PciData.PciExpressBase;
  }
  PlatformInfoHob->MemData.MemTolm = PlatformInfoHob->MemData.MemMaxTolm;

  //
  // Platform PCI MMIO Size in unit of 1MB
  //
  PlatformInfoHob->MemData.MmioSize = 0x1000 - (UINT16) (PlatformInfoHob->MemData.MemMaxTolm >> 20);

  //
  // Enable ICH IOAPIC
  //
  PlatformInfoHob->SysData.SysIoApicEnable  = ICH_IOAPIC;

  DEBUG ((EFI_D_INFO,  "PlatformFlavor is %x (%x=tablet,%x=mobile,%x=desktop)\n", PlatformInfoHob->PlatformFlavor, FlavorTablet, FlavorMobile, FlavorDesktop));

  //
  // Get Platform Info and fill the Hob
  //
  PlatformInfoHob->RevisonId = PLATFORM_INFO_HOB_REVISION;

  //
  // Get GPIO table
  //
  Status = LeafHillMultiPlatformGpioTableInit (PeiServices, PlatformInfoHob);
  ASSERT_EFI_ERROR (Status);

  //
  // Program GPIO
  //
  Status = LeafHillMultiPlatformGpioProgram (PeiServices, PlatformInfoHob);

  if (GetBxtSeries () == BxtP) {
    LeafHillGpioGroupTierInit (PlatformInfoHob);
  }

  //
  // Update OemId
  //
  Status = LeafHillInitializeBoardOemId (PeiServices, PlatformInfoHob);
  Status = LeafHillInitializeBoardSsidSvid (PeiServices, PlatformInfoHob);

  return EFI_SUCCESS;
}


EFI_STATUS
LeafHillInitializeBoardOemId (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PLATFORM_INFO_HOB        *PlatformInfoHob
  )
{
  UINT64  OemId;
  UINT64  OemTableId;

  //
  // Set OEM ID according to Board ID.
  //
  switch (PlatformInfoHob->BoardId) {
    default:
    OemId = EFI_ACPI_OEM_ID_DEFAULT;
    OemTableId = EFI_ACPI_OEM_TABLE_ID_DEFAULT;
    break;
  }

  PlatformInfoHob->AcpiOemId = OemId;
  PlatformInfoHob->AcpiOemTableId = OemTableId;

  return  EFI_SUCCESS;
}


EFI_STATUS
LeafHillInitializeBoardSsidSvid (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PLATFORM_INFO_HOB        *PlatformInfoHob
  )
{
  UINT32  SsidSvidValue = 0;

  //
  // Set OEM ID according to Board ID.
  //
  switch (PlatformInfoHob->BoardId) {
    default:
    SsidSvidValue = SUBSYSTEM_SVID_SSID;//SUBSYSTEM_SVID_SSID_DEFAULT;
    break;
  }
  PlatformInfoHob->SsidSvid = SsidSvidValue;

  return  EFI_SUCCESS;
}

