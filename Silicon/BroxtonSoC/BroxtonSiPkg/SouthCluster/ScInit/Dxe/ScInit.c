/** @file
  This is the driver that initializes the Intel SC devices

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInit.h"
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/PciIo.h>
#include <Library/PmcIpcLib.h>
#include <Library/DxeVtdLib.h>
#include <SaAccess.h>
#include <ScAccess.h>
#include <ScRegs/RegsPcu.h>
#include <Library/SteppingLib.h>
#include <Library/HeciMsgLib.h>
#include <SeCChipset.h>
#include <SeCState.h>
#include <Private/ScPmcFunctionDisableResetHob.h>

#define FUNC_ACPI_ENUM_1 0x74

//
// Global Variables
//
EFI_HANDLE  mImageHandle;
GLOBAL_REMOVE_IF_UNREFERENCED  SC_POLICY_HOB   *mScPolicy;

/**
  SC initialization before booting to OS

  @param[in]  Event                   A pointer to the Event that triggered the callback.
  @param[in]  Context                 A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScOnReadyToBoot (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS             Status;
  BXT_SERIES             BxtSeries;
  SC_LOCK_DOWN_CONFIG    *LockDownConfig;

  DEBUG ((DEBUG_INFO, "ScOnReadyToBoot() Start\n"));

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  BxtSeries = GetBxtSeries ();
  Status = GetConfigBlock ((VOID *) mScPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Trigger an SW SMI to config PCIE power management setting after PCI enumeration is done
  //
  if (BxtSeries == BxtP || (BxtSeries == Bxt1)) {
    IoWrite8 (R_APM_CNT, SW_SMI_PCIE_ASPM_OVERRIDE);
  }

  //
  // Trigger an SW SMI to do BiosWriteProtect
  //
  if ((BxtSeries == BxtP) && (LockDownConfig->BiosLock == TRUE)) {
    IoWrite8 (R_APM_CNT, (UINT8) LockDownConfig->BiosLockSwSmiNumber);
  }
  DEBUG ((DEBUG_INFO, "ScOnReadyToBoot() End\n"));
}


/**
  This is the callback function for PCI Enumeration Complete.

**/
VOID
EFIAPI
ScOnPciEnumComplete (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                        Status;
  VOID                              *ProtocolPointer;
  UINT8                             Reset;
  EFI_PEI_HOB_POINTERS              HobPtr;
  SC_PMC_FUNCTION_DISABLE_RESET_HOB *FuncDisHob;
  EFI_HOB_GUID_TYPE                 *FdoEnabledGuidHob = NULL;

  DEBUG ((DEBUG_INFO, "ScOnPciEnumComplete ()\n"));

  //
  // Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  // if it is, we will skip it until real event is triggered
  //
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }
  gBS->CloseEvent (Event);

  //
  // Restore P2SB and GPIO controlers's Base Address to original value which is overridden by PCI BUS driver.
  // P2SB will be switched to ACPI mode at exit boot service, so it's resource must be allocated from ACPI_MMIO_BASE_ADDRESS 
  // region, to avoid resource conflict with PCI resource used by PCI_HOST.asl. 
  //
  MmioWrite32 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, R_P2SB_BASE),
    (UINT32) ((PcdGet32 (PcdP2SBBaseAddress)))
    );
   
  //
  // Get SC PMC fuction disable reset HOB.
  //
  HobPtr.Guid = GetFirstGuidHob (&gScPmcFunctionDisableResetHobGuid);
  if (HobPtr.Guid != NULL) {
    FuncDisHob = GET_GUID_HOB_DATA (HobPtr.Guid);
    //
    // CSE status. When CSE is in recovery mode. CSE convert the  cold reset to the Global reset
    // so function disable register is not preserved
    //
    FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);
    if (FdoEnabledGuidHob == NULL) {
      //
      // Do not change the order of Global reset and Cold reset.
      // Global reset clears the PMC function disable register.
      // If done after cold reset, it will end up in a dead loop.
      //
      if (FuncDisHob->ResetType == SC_PMC_FUNCTION_DISABLE_GLOBAL_RESET) {
        DEBUG ((DEBUG_INFO, "PMC function disable updated, do global reset. \n"));
        HeciSendResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
        CpuDeadLoop();    // Should not continue execution after global reset
      }
      if (FuncDisHob->ResetType == SC_PMC_FUNCTION_DISABLE_COLD_RESET) {
        DEBUG ((DEBUG_INFO, "PMC function disable updated, do cold reset. \n"));
        Reset = IoRead8 (R_RST_CNT);
        Reset &= 0xF1;
        Reset |= 0xE;
          IoWrite8 (R_RST_CNT, Reset);
          CpuDeadLoop ();    // Should not continue execution after cold reset
      }
    }
  }
  return;
}


/**
  Register EndOfDXE, ReadyToBoot, ExitBootService event functions
  for SC configuration in different stages

**/
VOID
ScRegisterNotifications (
  VOID
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBoot;
  EFI_EVENT   LegacyBootEvent;
  EFI_EVENT   ExitBootServicesEvent;
  VOID        *Registration;

  //
  // Create PCI Enumeration Completed callback
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    ScOnPciEnumComplete,
    NULL,
    &Registration
    );

  //
  // Register an end of DXE event for PCH to do tasks before invoking any UEFI drivers,
  // applications, or connecting consoles,...
  //
  //
  // Create an ExitPmAuth protocol call back event.
  //
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    ScOnEndOfDxe,
    NULL,
    &Registration
    );

  //
  // Register a Ready to boot event to config PCIE power management setting after OPROM executed
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ScOnReadyToBoot,
             NULL,
             &ReadyToBoot
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Create events for SC to do the task before ExitBootServices/LegacyBoot.
  // It is guaranteed that only one of two events below will be signalled
  //
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  ScExitBootServicesEvent,
                  NULL,
                  &ExitBootServicesEvent
                  );
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_NOTIFY,
                  ScLpssExitBootServicesEvent,
                  NULL,
                  &ExitBootServicesEvent
                  );

  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventLegacyBootEx (
             TPL_CALLBACK,
             ScExitBootServicesEvent,
             NULL,
             &LegacyBootEvent
             );
  Status = EfiCreateEventLegacyBootEx (
             TPL_NOTIFY,
             ScLpssExitBootServicesEvent,
             NULL,
             &LegacyBootEvent
             );
  ASSERT_EFI_ERROR (Status);
}


/**
  South cluster initialization entry point

  @param[in] ImageHandle                Handle for the image of this driver
  @param[in] SystemTable                Pointer to the EFI System Table

  @retval    EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
EFIAPI
ScInitEntryPoint (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS               Status;
  UINT32                   PmcBase;
  EFI_PEI_HOB_POINTERS     HobPtr;
  SC_INSTANCE_PRIVATE_DATA *ScInstance;
  UINT16                   AcpiBaseAddr;
  EFI_HANDLE               Handle;

  DEBUG ((DEBUG_INFO, "ScInitEntryPoint() Start\n"));

  ScInstance   = NULL;
  mScPolicy    = NULL;
  AcpiBaseAddr = 0;

  Handle       = NULL;
  mImageHandle = ImageHandle;

  //
  // Get SC Policy HOB.
  //
  Status = EFI_NOT_FOUND;
  HobPtr.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mScPolicy = GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  // Allocate and install the SC Info protocol
  //
  PmcBase      = PMC_BASE_ADDRESS;
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  DEBUG ((DEBUG_INFO, "SC Base Addresses:\n-------------------\n"));
  DEBUG ((DEBUG_INFO, "  PmcBase   0x%X\n", PmcBase));
  DEBUG ((DEBUG_INFO, "  AcpiBase  0x%X\n", AcpiBaseAddr));
  DEBUG ((DEBUG_INFO, "-------------------\n"));
  ASSERT (PmcBase != 0);
  ASSERT (AcpiBaseAddr != 0);
  DEBUG ((DEBUG_INFO, "SC Base Addresses:\n----START---------------\n"));
  DEBUG ((DEBUG_INFO, "  AcpiBase  0x%X\n", AcpiBaseAddr));
#if (ENBDT_PF_ENABLE == 1)
  DEBUG ((DEBUG_INFO, "CAUTION Broxton P ACPI offsets\n"));
#else
  DEBUG ((DEBUG_INFO, "Broxton M ACPI offsets\n"));
#endif
  DEBUG ((DEBUG_INFO, "  PM1_STS_EN  0x%X  0x%X\n", R_ACPI_PM1_STS, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_STS))));;
  DEBUG ((DEBUG_INFO, "  PM1_CNT     0x%X  0x%X\n", R_ACPI_PM1_CNT, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_CNT))));;
  DEBUG ((DEBUG_INFO, "  PM1_TMR     0x%X  0x%X\n", R_ACPI_PM1_TMR, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_TMR))));;
  DEBUG ((DEBUG_INFO, "  GPE0a_STS   0x%X  0x%X\n", R_ACPI_GPE0a_STS, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_GPE0a_STS))));;
  DEBUG ((DEBUG_INFO, "  GPE0a_EN    0x%X  0x%X\n", R_ACPI_GPE0a_EN, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_GPE0a_EN))));;
  DEBUG ((DEBUG_INFO, "  SMI_EN      0x%X  0x%X\n", R_SMI_EN, IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_EN))));;
  DEBUG ((DEBUG_INFO, "  SMI_STS     0x%X  0x%X\n", R_SMI_STS, IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_STS))));;
  DEBUG ((DEBUG_INFO, "  GPE_CTRL    0x%X  0x%X\n", R_ACPI_GPE_CNTL, IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_GPE_CNTL))));;
  DEBUG ((DEBUG_INFO, "  TCO_RLD     0x%X  0x%X\n", R_TCO_RLD, IoRead32 ((UINTN) (AcpiBaseAddr + R_TCO_RLD))));;
  DEBUG ((DEBUG_INFO, "  TCO_STS     0x%X  0x%X\n", R_TCO_STS, IoRead32 ((UINTN) (AcpiBaseAddr + R_TCO_STS))));;
  DEBUG ((DEBUG_INFO, "  TCO1_CNT    0x%X  0x%X\n", R_TCO_CNT, IoRead32 ((UINTN) (AcpiBaseAddr + R_TCO_CNT))));;
  DEBUG ((DEBUG_INFO, "  TCO_TMR     0x%X  0x%X\n", R_TCO_TMR, IoRead32 ((UINTN) (AcpiBaseAddr + R_TCO_TMR))));;
  DEBUG ((DEBUG_INFO, "SC Base Addresses:\n----END---------------\n"));

  //
  // Initialize the SC device
  //
  InitializeScDevice (ScInstance, mScPolicy, PmcBase, AcpiBaseAddr);

  ScRegisterNotifications ();

  ScInstance = AllocateZeroPool (sizeof (SC_INSTANCE_PRIVATE_DATA));
  if (ScInstance == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ScInstance->ScInfo.Revision   = INFO_PROTOCOL_REVISION_1;
  ScInstance->ScInfo.BusNumber  = DEFAULT_PCI_BUS_NUMBER_SC;
  ScInstance->ScInfo.RCVersion  = SC_POLICY_REVISION;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &(ScInstance->ScInfo)
                  );

  DEBUG ((DEBUG_INFO, "ScInitEntryPoint() End\n"));

  return EFI_SUCCESS;
}


/**
  Initialize the SC device according to the SC Platform Policy

  @param[in, out] ScInstance         SC instance private data. May get updated by this function
  @param[in]     ScPolicy            The SC Platform Policy protocol instance
  @param[in]     PmcBase             PMC base address of this SC device
  @param[in]     AcpiBaseAddr        ACPI IO base address of this SC device

  @retval        EFI_SUCCESS         The function completed successfully

**/
EFI_STATUS
InitializeScDevice (
  IN OUT SC_INSTANCE_PRIVATE_DATA       *ScInstance,
  IN     SC_POLICY_HOB                  *ScPolicy,
  IN     UINT32                         PmcBase,
  IN     UINT16                         AcpiBaseAddr
  )
{
  EFI_STATUS            Status;
  UINT32                FuncDisableReg;
  BXT_SERIES            BxtSeries;

  DEBUG ((DEBUG_INFO, "InitializeScDevice() Start\n"));
  FuncDisableReg  = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);
  BxtSeries = GetBxtSeries ();

  //
  // VT-d Initialization
  //
  DEBUG ((DEBUG_INFO, "Initializing VT-d in Dxe to update DMAR table\n"));
  VtdInit ();

  //
  // Configure PMC Devices
  //
  Status = ConfigurePmcAfterPciEnum (ScPolicy);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InitializeScDevice() End\n"));

  return Status;
}


/**
  Program Sc devices dedicated IRQ#.

  @param[in] ScPolicy                      The SC Policy instance

  @retval    EFI_SUCCESS                   The function completed successfully

**/
EFI_STATUS
ConfigureIrqAtBoot (
  IN      SC_POLICY_HOB    *ScPolicy
  )
{
  UINTN                                      Index;
  UINTN                                      TblIndex;
  UINTN                                      Segment;
  UINTN                                      BusNum;
  UINTN                                      DevNum;
  UINTN                                      FunNum;
  UINTN                                      HandleCount;
  EFI_STATUS                                 Status;
  EFI_HANDLE                                 *Handles;
  EFI_PCI_IO_PROTOCOL                        *PciIo;
  STATIC SC_IRQ_INIT_ENTRY                   IrqInitTable[] = {
    //
    // Bus  Dev  Fun  Irq
    //
    {0,    0,    1,  24},  // PUINT
    {0,    0,    3,  23},  // gmm
    {0,    3,    0,  21},  // iunit


    {0,    0xE,  0,  25},  // HD-Audio

    {0,    0x11, 0,  26},  // ISH

    {0,    0x15, 0,  17},  // XHCI
    {0,    0x15, 1,  13},  // xDCI

    {0,    0x16, 0,  27},  // I2C1
    {0,    0x16, 1,  28},  // I2C2
    {0,    0x16, 2,  29},  // I2C3
    {0,    0x16, 3,  30},  // I2C4

    {0,    0x17, 0,  31},  // I2C5
    {0,    0x17, 1,  32},  // I2C6
    {0,    0x17, 2,  33},  // I2C7
    {0,    0x17, 3,  34},  // I2C8

    {0,    0x18, 0,   4},  // UART1
    {0,    0x18, 1,   5},  // UART2
    {0,    0x18, 2,   6},  // UART3
    {0,    0x18, 3,   7},  // UART4

    {0,    0x19, 0,   35},  // SPI1
    {0,    0x19, 1,   36},  // SPI2
    {0,    0x19, 2,   37},  // SPI3

    {0,    0x1B, 0,    3 }, // SDCard
    {0,    0x1C, 0,   39 }, // eMMC
    {0,    0x1D, 0,   38 }, // UFS
    {0,    0x1E, 0,   42 }, // SDIO
  };

  DEBUG ((EFI_D_INFO, "ConfigureIrqAtBoot() Start\n"));

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );

  DEBUG ((EFI_D_INFO, "Status = %r\n", Status));

  if (EFI_ERROR (Status)) {
    return Status;
  }

  for(Index = 0; Index < HandleCount; Index ++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );

    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = PciIo->GetLocation(
                      PciIo,
                      &Segment,
                      &BusNum,
                      &DevNum,
                      &FunNum
                      );

    for (TblIndex = 0; TblIndex < (sizeof (IrqInitTable) / sizeof (SC_IRQ_INIT_ENTRY)); TblIndex++) {
      if ((BusNum == IrqInitTable[TblIndex].BusNumber) &&
          (DevNum == IrqInitTable[TblIndex].DeviceNumber) &&
          (FunNum == IrqInitTable[TblIndex].FunctionNumber)) {

        Status = PciIo->Pci.Write(
                              PciIo,
                              EfiPciIoWidthUint8,
                              0x3c,                //Offset 0x3c :PCI Interrupt Line
                              1,
                              &IrqInitTable[TblIndex].Irq
                              );

          DEBUG ((EFI_D_INFO, "Writing IRQ#%d for B%d/D%d/F%d\n", IrqInitTable[TblIndex].Irq, BusNum, DevNum, FunNum));
      } else {
        continue;
      }

    }

  }

  DEBUG ((EFI_D_INFO, "ConfigureIrqAtBoot() End\n"));
  return EFI_SUCCESS;
}


/**
  SC initialization triggered by ExitBootServices events
  Useful for operations which must happen later than at the end of post

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScExitBootServicesEvent (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  UINTN       P2SBBase;
  UINT32      Buffer9;
  BXT_SERIES  BxtSeries;

  Buffer9   = 0;
  BxtSeries = GetBxtSeries ();

  //
  // Closed the event to avoid call twice
  //
  gBS->CloseEvent (Event);

  UpdateNvsArea (mScPolicy);
  ConfigureHdaAtBoot (mScPolicy);

  ConfigureIrqAtBoot (mScPolicy);
  StopLpssAtBoot ();

  if (BxtSeries != BxtP) {
    SideBandAndThenOr32(
      0xC6,
      0x2338,
      0xFFFFFFFF,
      BIT0
     );
  }

  //
  // Hide P2SB device in OS phase
  //
  DEBUG ((DEBUG_INFO, "Hide P2SB on normal mode..........\n"));
  P2SBBase = MmPciAddress (
               0,
               0,
               PCI_DEVICE_NUMBER_P2SB,
               PCI_FUNCTION_NUMBER_P2SB,
               R_P2SB_P2SBC
               );
  MmioOr32 (P2SBBase, B_P2SB_P2SBC_HIDE);

  //
  // Set the P2SB bit in FUNC_ACPI_ENUM_1.
  //
  Buffer9 = MmioRead32 (GCR_BASE_ADDRESS + FUNC_ACPI_ENUM_1);
  Buffer9 = Buffer9 | BIT0;
  MmioWrite32 (GCR_BASE_ADDRESS + FUNC_ACPI_ENUM_1, Buffer9);

  return;
}


/**
  SC initialization triggered by ExitBootServices for LPSS events
  This should be at a higher TPL than the callback that sets Untrusted Mode.

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScLpssExitBootServicesEvent (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  //
  // Closed the event to avoid call twice
  //
  gBS->CloseEvent (Event);

  ConfigureLpssAtBoot (mScPolicy);
}


/**
  SC initialization before Boot Script Table is closed

  @param[in] Event                      A pointer to the Event that triggered the callback.
  @param[in] Context                    A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
ScOnEndOfDxe (
  IN EFI_EVENT               Event,
  IN VOID                    *Context
  )
{
  EFI_STATUS                 Status;
  UINT32                     FuncDisableReg;
  UINT32                     Data32And;
  UINT32                     Data32Or;
  UINT16                     AcpiBaseAddr;
  UINT32                     PmcBase;
  UINT8                      NumOfDevltrOverride;
  UINTN                      PciLpcRegBase;
  UINTN                      PciSpiRegBase;
  BXT_SERIES                 BxtSeries;
  UINT8                      Data8;
  SC_LOCK_DOWN_CONFIG        *LockDownConfig;
  UINT16                     Data16And;
  UINT16                     Data16Or;
  SI_POLICY_HOB              *SiPolicyHob;
  EFI_PEI_HOB_POINTERS       HobPtr;

  NumOfDevltrOverride = 0;
  PciLpcRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_PCH_LPC,
                    PCI_FUNCTION_NUMBER_PCH_LPC
                    );
  PciSpiRegBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_SPI,
                    PCI_FUNCTION_NUMBER_SPI
                    );

  DEBUG ((DEBUG_INFO, "ScOnEndOfDxe() Start\n"));

  //
  // Closed the event to avoid call twice when launch shell
  //
  gBS->CloseEvent (Event);

  BxtSeries = GetBxtSeries ();
  if (BxtSeries == BxtP) {
    ConfigureSataDxe (mScPolicy);
  }
  AcpiBaseAddr   = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);
  PmcBase        = PMC_BASE_ADDRESS;
  FuncDisableReg = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);

  MmioAnd32 (
    (UINTN) (PmcBase + R_PMC_PMIR),
    (UINT32) ~(B_PMC_PMIR_CF9GR)
    );
  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (PmcBase + R_PMC_PMIR),
    1,
    (VOID *) (UINTN) (PmcBase + R_PMC_PMIR)
    );

  Status = GetConfigBlock ((VOID *) mScPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);
  if (LockDownConfig->GlobalSmi == TRUE) {
    //
    // Save Global SMI Enable bit setting before BIOS enables SMI_LOCK during S3 resume
    //
    Data32Or  = IoRead32 ((UINTN) (AcpiBaseAddr + R_SMI_EN));
    if ((Data32Or & B_SMI_EN_GBL_SMI) != 0) {
      Data32And = 0xFFFFFFFF;
      Data32Or &= B_SMI_EN_GBL_SMI;
      S3BootScriptSaveIoReadWrite (
        EfiBootScriptWidthUint32,
        (UINTN) (AcpiBaseAddr + R_SMI_EN),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }

    MmioOr8 ((UINTN) (PmcBase + R_PMC_GEN_PMCON_2), B_PMC_GEN_PMCON_SMI_LOCK);
    S3BootScriptSaveMemWrite (
      EfiBootScriptWidthUint8,
      (UINTN) (PmcBase + R_PMC_GEN_PMCON_2),
      1,
      (VOID *) (UINTN) (PmcBase + R_PMC_GEN_PMCON_2)
      );
  }

  if (GetBxtSeries () == BxtP) {
    if (LockDownConfig->BiosLock == TRUE) {
      //
      // LPC
      //
      if (! (MmioRead8 (PciLpcRegBase + R_PCH_LPC_BC) & B_PCH_LPC_BC_LE)) {
        DEBUG ((DEBUG_INFO, "Set LPC bios lock\n"));
        MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), B_PCH_LPC_BC_LE);
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint8,
          (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
          1,
          (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
          );
      }

      //
      // SPI
      //
      if (! (MmioRead8 (PciSpiRegBase + R_SPI_BCR) & B_SPI_BCR_BLE)) {
        DEBUG ((DEBUG_INFO, "Set SPI bios lock\n"));
        MmioOr8 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT8) B_SPI_BCR_BLE);
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint8,
          (UINTN) (PciSpiRegBase + R_SPI_BCR),
          1,
          (VOID *) (UINTN) (PciSpiRegBase + R_SPI_BCR)
          );
      }
    }

    if (LockDownConfig->BiosInterface == TRUE) {
      //
      // LPC
      //
      MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), (UINT8) B_PCH_LPC_BC_BILD);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
        1,
        (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
        );

      //
      // Reads back for posted write to take effect
      //
      Data8 = MmioRead8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC));
      S3BootScriptSaveMemPoll  (
        S3BootScriptWidthUint8,
        (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
        &Data8,     // BitMask
        &Data8,     // BitValue
        1,          // Duration
        1           // LoopTimes
        );

      //
      // SPI
      //
      MmioOr8 ((UINTN) (PciSpiRegBase + R_SPI_BCR), (UINT8) B_SPI_BCR_BILD);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciSpiRegBase + R_SPI_BCR),
        1,
        (VOID *) (UINTN) (PciSpiRegBase + R_SPI_BCR)
        );

      //
      // Reads back for posted write to take effect
      //
      Data8 = MmioRead8 ((UINTN) (PciSpiRegBase + R_SPI_BCR));
      S3BootScriptSaveMemPoll (
        S3BootScriptWidthUint8,
        (UINTN) (PciSpiRegBase + R_SPI_BCR),
        &Data8,     // BitMask
        &Data8,     // BitValue
        1,          // Duration
        1           // LoopTimes
        );
    }
  }

  if (LockDownConfig->TcoLock) {
  //
  // Enable TCO and Lock Down TCO
  //
  DEBUG ((DEBUG_INFO, "Enable TCO and Lock Down TCO\n"));
  Data16And = 0xFFFF;
  Data16Or = B_SMI_EN_TCO;
  IoOr16 (AcpiBaseAddr + R_SMI_EN, Data16Or);

  Data16And = 0xFFFF;
  Data16Or = B_TCO_CNT_LOCK;
  IoOr16 (AcpiBaseAddr + R_TCO_CNT, Data16Or);

  S3BootScriptSaveIoReadWrite (
    EfiBootScriptWidthUint16,
    (UINTN) (AcpiBaseAddr + R_TCO_CNT),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
  );

  HobPtr.Guid = GetFirstGuidHob (&gSiPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  SiPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);
  UsbInitBeforeBoot (SiPolicyHob, mScPolicy);
  DEBUG ((DEBUG_INFO, "Enable TCO and Lock Down TCO ---- END\n"));

  }

  if (LockDownConfig->RtcLock == TRUE) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = (B_PCH_PCR_RTC_CONF_UCMOS_LOCK | B_PCH_PCR_RTC_CONF_LCMOS_LOCK | B_PCH_PCR_RTC_CONF_BILD);
    PchPcrAndThenOr32 (
      0xD1,
      R_PCH_PCR_RTC_CONF,
      Data32And,
      Data32Or
      );
    PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      0xD1,
      R_PCH_PCR_RTC_CONF,
      &Data32Or,
      &Data32And
      );
  }

  if ((BxtSeries == BxtP) || (BxtSeries == Bxt1)){
    Data8 = SW_SMI_PCIE_ASPM_OVERRIDE;
    S3BootScriptSaveIoWrite (
      EfiBootScriptWidthUint8,
      (UINTN) (R_APM_CNT),
      1,
      &Data8
      );
  }
  DEBUG ((DEBUG_INFO, "ScOnEndOfDxe() End\n"));

  return;
}

