/** @file
  Implements Overclocking Interface for OS Application ie Iron city.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <UsbTypeC.h>
#include <Protocol/AcpiTable.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/GlobalNvsArea.h>

GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;


#define USBTYPEC_DATA_VAR         L"UsbTypeC"
EFI_GUID UsbTypeCVarGuid      =  { 0xfc876842, 0xd8f0, 0x4844, {0xae, 0x32, 0x1f, 0xf8, 0x43, 0x79, 0x7b, 0x17} };


/**
  Allocate MemoryType below 4G memory address.

  @param[in] Size              Size of memory to allocate.
  @param[in] Buffer            Allocated address for output.

  @retval    EFI_SUCCESS       Memory successfully allocated.
  @retval    Other             Other errors occur.

**/
static EFI_STATUS
AllocateMemoryBelow4G (
  IN   EFI_MEMORY_TYPE MemoryType,
  IN   UINTN           Size,
  OUT  VOID           **Buffer
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  =  gBS->AllocatePages (
                    AllocateMaxAddress,
                    MemoryType,
                    Pages,
                    &Address
                    );

  *Buffer = (VOID *) (UINTN) Address;

  return Status;
};


static UINT16 GetUcsiRev(VOID)
{
  return 0;
}


/**
  This function is the entry point for this DXE driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval    EFI_DEVICE_ERROR     Initialization fails
  @retval    EFI_SUCCESS          Initialization completes successfully.

**/
EFI_STATUS
EFIAPI
UsbTypeCEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS           Status;
  USBTYPEC_OPM_BUF     *OpmBuffer;

  DEBUG ((DEBUG_INFO, "UsbTypeC entrypoint.\n"));

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  //
  // Allocate memory in ACPI NVS
  //
  Status = AllocateMemoryBelow4G (EfiACPIMemoryNVS, 0x1000, (VOID **)&OpmBuffer);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetMem (OpmBuffer, sizeof (USBTYPEC_OPM_BUF), 0x0);
  OpmBuffer->Version =  GetUcsiRev();

  mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr = (UINT32) (UINTN) OpmBuffer;
  DEBUG ((DEBUG_INFO, "UsbTypeC EntryPoint: mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr = 0x%X\n", mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr));

  Status = gRT->SetVariable (
                  USBTYPEC_DATA_VAR,
                  &UsbTypeCVarGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (UINT32),
                  &(mGlobalNvsAreaPtr->UsbTypeCOpBaseAddr)
                  );

  return Status;
}

