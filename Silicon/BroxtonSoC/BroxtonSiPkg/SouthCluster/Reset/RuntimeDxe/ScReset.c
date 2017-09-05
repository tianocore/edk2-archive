/** @file
  Reset Architectural Protocol implementation.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScReset.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ConfigBlockLib.h>
#include "CmosMap.h"
#include "ScAccess.h"
#include "HeciRegs.h"

SC_RESET_INSTANCE  *mResetInstance;


/**
  Install and initialize reset protocols.

  @param[in] ImageHandle                   Image handle of the loaded driver
  @param[in] SystemTable                   Pointer to the System Table

  @retval    EFI_SUCCESS                   Thread can be successfully created
  @retval    EFI_OUT_OF_RESOURCES          Cannot allocate protocol data structure
  @retval    EFI_DEVICE_ERROR              Cannot create the timer service

**/
EFI_STATUS
EFIAPI
InitializeScReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        ResetHandle;
#if ((TIANO_RELEASE_VERSION != 0) && (EFI_SPECIFICATION_VERSION < 0x00020000))
  DXE_SC_PLATFORM_POLICY_PROTOCOL   *ScPlatformPolicy;
#endif
  EFI_EVENT                         Event;
  EFI_PEI_HOB_POINTERS              HobPtr;
  SC_POLICY_HOB                     *ScPolicy;
  SC_GENERAL_CONFIG                 *GeneralConfig;

  DEBUG ((DEBUG_INFO, "InitializeScReset() Start\n"));
  ResetHandle = NULL;
  ScPolicy    = NULL;

  //
  // Get SC Policy HOB.
  //
  Status = EFI_NOT_FOUND;
  ZeroMem (&HobPtr, sizeof (EFI_PEI_HOB_POINTERS));
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);

  ScPolicy = GET_GUID_HOB_DATA (HobPtr.Guid);
  Status = GetConfigBlock ((VOID *) ScPolicy, &gScGeneralConfigGuid, (VOID *) &GeneralConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate Runtime memory for the ScExtendedReset protocol instance.
  //
  mResetInstance = AllocateRuntimePool (sizeof (SC_RESET_INSTANCE));
  if (mResetInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

#if ((TIANO_RELEASE_VERSION != 0) && (EFI_SPECIFICATION_VERSION < 0x00020000))
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformPolicyProtocolGuid, NULL, (VOID **) &ScPlatformPolicy);
  ASSERT_EFI_ERROR (Status);

  //
  // Check whether the CapsuleVariableName is filled.
  //
  if (ScPlatformPolicy->CapsuleVariableName != NULL) {
    //
    // Allocate a runtime space and copy string from CapsuleVariableName
    //
    mResetInstance->CapsuleVariableName = AllocateRuntimePool (StrSize (ScPlatformPolicy->CapsuleVariableName));
    CopyMem (
      mResetInstance->CapsuleVariableName,
      ScPlatformPolicy->CapsuleVariableName,
      StrSize (ScPlatformPolicy->CapsuleVariableName)
      );
  }
#endif
  //
  // Install protocol interface
  //
  mResetInstance->Signature                     = RESET_SIGNATURE;
  mResetInstance->Handle                        = NULL;
  mResetInstance->ScExtendedResetProtocol.Reset = ScExtendedReset;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mResetInstance->Handle,
                  &gEfiExtendedResetProtocolGuid,
                  &mResetInstance->ScExtendedResetProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  mResetInstance->PmcBase = PMC_BASE_ADDRESS;

  mResetInstance->AcpiBar = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  mResetInstance->ResetSelect = (UINT8) GeneralConfig->ResetSelect;

  //
  // Make sure the Reset Architectural Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiResetArchProtocolGuid);

  //
  // Hook the runtime service table
  //
  SystemTable->RuntimeServices->ResetSystem = (EFI_RESET_SYSTEM) IntelScResetSystem;

  //
  // Now install the Reset RT AP on a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ResetHandle,
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install a notification to convert the SCReset driver to Virtual Addr
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ScResetVirtualddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // The Lib Deconstruct will automatically be called when entrypoint return error.
  //
  DEBUG ((DEBUG_INFO, "InitializeScReset() End\n"));

  return Status;
}


/**
  Set flag to indicate the reset is controlled reset or uncontrolled reset.

**/
VOID
NotifyControlledReset(
  VOID
  )
{
  //
  // The following magic value in CMOS register is used by DXE module "NullMemoryTest"
  // to differentiate a reset as Controlled or Uncontrolled.
  //
  IoWrite8 (CmosIo_72, EFI_CMOS_CLEAN_RESET);
  IoWrite8 (CmosIo_73, B_MAGIC_CLEAN_RESET_VALUE); //Signal a controlled Reset
}


#if ((TIANO_RELEASE_VERSION != 0) && (EFI_SPECIFICATION_VERSION < 0x00020000))
/**
  If need be, do any special reset required for capsules. For this
  implementation where we're called from the ResetSystem() api,
  just set our capsule variable and return to let the caller
  do a soft reset.

  @param[in] CapsuleDataPtr            Pointer to the capsule block descriptors

**/
VOID
CapsuleReset (
  IN UINTN   CapsuleDataPtr
  )
{
  UINT32  Data32;
  UINT32  Eflags;
  UINT16  AcpiBaseAddr;

  //
  // Check the CapsuleVariableName, asset if it is not filled.
  //
  ASSERT (mResetInstance->CapsuleVariableName != NULL);

  //
  // This implementation assumes we're using a variable for the capsule
  // data pointer.
  //
  EfiSetVariable (
    mResetInstance->CapsuleVariableName,
    &gEfiCapsuleVendorGuid,
    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,  ///< attributes
    sizeof (UINTN),
    (VOID *) &CapsuleDataPtr
    );

  //
  // Read the ACPI Base address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);
  ASSERT (AcpiBaseAddr != 0);

  //
  // Read the PM1_CNT register
  //
  Data32  = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT));
  Data32  = (UINT32) ((Data32 & ~(B_ACPI_PM1_CNT_SLP_TYP + B_ACPI_PM1_CNT_SLP_EN)) | V_ACPI_PM1_CNT_S3);
  Eflags  = (UINT32) AsmReadEflags ();

  if ((Eflags & 0x200)) {
    DisableInterrupts ();
  }

  AsmWbinvd ();
  AsmWriteCr0 (AsmReadCr0 () | 0x060000000);

  IoWrite32 (
    (UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT),
    (UINT32) Data32
    );

  Data32 = Data32 | B_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 (
    (UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT),
    (UINT32) Data32
    );

  if ((Eflags & 0x200)) {
    EnableInterrupts ();
  }
  //
  // Should not return
  //
  EFI_DEADLOOP ();
}
#endif


/**
  Generate system reset by Reset Control Register (IO Cf9h)
  Note: This reset is hooked to RuntimeServices->ResetSystem and will be called
        in OS environment and if any debug messages added then need to handle it
        carefully, else debug build cause to BSOD.
        Its recommended not to have debug messages over here.

  @param[in] ResetType                 Warm or cold
  @param[in] ResetStatus               Possible cause of reset
  @param[in] DataSize                  Size of ResetData in bytes
  @param[in] ResetData                 Optional Unicode string

**/
VOID
EFIAPI
IntelScResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  )
{
  UINT8          InitialData;
  UINT8          OutputData;
  UINT16         AcpiBase;
  UINT16         Data16;
  UINT32         Data32;

  NotifyControlledReset ();

  switch (ResetType) {
    case EfiResetWarm:
      InitialData = V_RST_CNT_HARDSTARTSTATE;
      OutputData  = mResetInstance->ResetSelect;
      if (mResetInstance->ResetSelect == V_RST_CNT_FULLRESET) {
      }
      break;

    case EfiResetCold:
      InitialData = V_RST_CNT_HARDSTARTSTATE;
      OutputData  = V_RST_CNT_HARDRESET;
      break;

    case EfiResetShutdown:
      AcpiBase = (UINT16) mResetInstance->AcpiBar;

      //
      // Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
      //
      Data16 = 0;

      IoWrite16 ((UINTN) (AcpiBase + R_ACPI_GPE0a_EN), Data16);

      //
      // Clear Sleep SMI Status
      //
      IoWrite16 (AcpiBase + R_SMI_STS,
                 (UINT16) (IoRead16 (AcpiBase + R_SMI_STS) | B_SMI_STS_ON_SLP_EN));
      //
      // Clear Sleep Type Enable
      //
      IoWrite16 (AcpiBase + R_SMI_EN,
                 (UINT16) (IoRead16 (AcpiBase + R_SMI_EN) & (~B_SMI_EN_ON_SLP_EN)));
      //
      // Clear Power Button Status
      //
      IoWrite16(AcpiBase + R_ACPI_PM1_STS, B_ACPI_PM1_STS_PWRBTN);

      //
      // Secondly, Power Button Status bit must be cleared
      //
      // Write a "1" to bit[8] of power button status register at
      // (ABASE + PM1_STS) to clear this bit
      // Clear it through SMI Status register
      //
      Data16 = B_SMI_STS_PM1_STS_REG;
      IoWrite16 ((UINTN) (AcpiBase + R_SMI_STS), Data16);

      //
      // Finally, transform system into S5 sleep state
      //
      Data32  = IoRead32 ((UINTN) (AcpiBase + R_ACPI_PM1_CNT));
      Data32  = (UINT32) ((Data32 & ~(B_ACPI_PM1_CNT_SLP_TYP + B_ACPI_PM1_CNT_SLP_EN)) | V_ACPI_PM1_CNT_S5);

      IoWrite32 ((UINTN) (AcpiBase + R_ACPI_PM1_CNT), Data32);

      Data32 = Data32 | B_ACPI_PM1_CNT_SLP_EN;

      IoWrite32 ((UINTN) (AcpiBase + R_ACPI_PM1_CNT), Data32);
      //
      // Should not return
      //
      CpuDeadLoop();
      return ;

    default:
      return ;
  }

  IoWrite8 (
    (UINTN) R_RST_CNT,
    (UINT8) InitialData
    );

  IoWrite8 (
    (UINTN) R_RST_CNT,
    (UINT8) OutputData
    );

  //
  // Given we should have reset getting here would be bad
  //
  CpuDeadLoop();

  ASSERT (FALSE);
}


/**
  Execute SC Extended Reset from the host controller.

  @param[in] This                      Pointer to the EFI_SC_EXTENDED_RESET_PROTOCOL instance.
  @param[in] ScExtendedResetTypes      SC Extended Reset Types which includes PowerCycle, Globalreset.

  @retval    EFI_SUCCESS               Successfully completed.
  @retval    EFI_INVALID_PARAMETER     If ResetType is invalid.

**/
EFI_STATUS
EFIAPI
ScExtendedReset (
  IN EFI_SC_EXTENDED_RESET_PROTOCOL   *This,
  IN SC_EXTENDED_RESET_TYPES          ScExtendedResetTypes
  )
{
  //
  // Check if the parameters are valid.
  //
  if ((ScExtendedResetTypes.PowerCycle == 0) && (ScExtendedResetTypes.GlobalReset == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  NotifyControlledReset ();

  if (ScExtendedResetTypes.GlobalReset == 1) {
    MmioOr32 (mResetInstance->PmcBase + R_PMC_PMIR, (UINT32) (B_PMC_PMIR_CF9GR));
  }

  IoWrite8 ((UINTN) R_RST_CNT, (UINT8) V_RST_CNT_FULLRESET);

  //
  // Waiting for system reset
  //
  while (TRUE);

  return EFI_SUCCESS;
}


/**
  Fixup internal data pointers so that the services can be called in virtual mode.

  @param[in]  Event                     The event registered.
  @param[in]  Context                   Event context. Not used in this event handler.

**/
VOID
EFIAPI
ScResetVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EfiConvertPointer (0x0, (VOID **) &(mResetInstance->ScExtendedResetProtocol.Reset));
#if ((TIANO_RELEASE_VERSION != 0) && (EFI_SPECIFICATION_VERSION < 0x00020000))
  EfiConvertPointer (0x0, (VOID **) &(mResetInstance->CapsuleVariableName));
#endif
  EfiConvertPointer (0x0, (VOID **) &(mResetInstance->AcpiBar));
  EfiConvertPointer (0x0, (VOID **) &(mResetInstance->PmcBase));
  EfiConvertPointer (0x0, (VOID **) &(mResetInstance));
}

