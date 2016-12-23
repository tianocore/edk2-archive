/** @file
  Framework PEIM to SeCUma.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <SeCUma.h>
#include <Library/HeciMsgLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SideBandLib.h>
#include <Private/Library/HeciInitLib.h>
#include <Library/HobLib.h>

#ifndef FSP_FLAG
#include <Library/PeiServicesLib.h>
#include <Library/PerformanceLib.h>
#include <Ppi/EndOfPeiPhase.h>
#endif

#include <Library/PcdLib.h>
#include <Library/MmPciLib.h>

#ifdef FSP_FLAG
#include <Library/FspCommonLib.h>
#include <FspmUpd.h>
#endif

extern EFI_GUID gEfiBootMediaHobGuid;
extern EFI_GUID gEfiIfwiDnxRequestHobGuid;
extern EFI_PEI_STALL_PPI  mStallPpi;
extern BOOLEAN ImageInMemory;

#define S3          0x20

#ifndef FSP_FLAG
/**
  Txe End of PEI callback function. This is the last event before entering DXE and OS in S3 resume.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval    EFI_SUCCESS          Succeeds.

**/
EFI_STATUS
EFIAPI
TxeOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mTxeNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    TxeOnEndOfPei
  }
};
#endif

//
// Function Declarations
//
static SEC_UMA_PPI         mSeCUmaPpi = {
  SeCSendUmaSize,
  SeCConfigDidReg,
  SeCTakeOwnerShip
};

static EFI_PEI_PPI_DESCRIPTOR mSeCUmaPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gSeCUmaPpiGuid,
    &mSeCUmaPpi
  }
};
EFI_PEI_PPI_DESCRIPTOR mCseEmmcSelectPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCseEmmcSelectPpiGuid,
    NULL
  }
};
EFI_PEI_PPI_DESCRIPTOR mCseUfsSelectPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCseUfsSelectPpiGuid,
    NULL
  }
};
EFI_PEI_PPI_DESCRIPTOR mCseSpiSelectPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCseSpiSelectPpiGuid,
    NULL
  }
};

/**

  This procedure will read and return the amount of SeC UMA requested
  by SeC ROM from the HECI device.


  @param[in] PeiServices      General purpose services available to every PEIM.

  @return   Return SeC UMA Size

**/
UINT32
SeCSendUmaSize (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  return 0;
}

/**
  This procedure will enforce the BIOS Action that was requested by SEC FW
  as part of the DRAM Init Done message.

  @param[in] BiosAction           Me requests BIOS to act

  @retval   EFI_SUCCESS           Always return EFI_SUCCESS

**/
EFI_STATUS
HandleSecBiosAction (
  IN UINT8                        BiosAction
  )
{
  EFI_STATUS              Status;
  HECI_FWS_REGISTER       SeCFirmwareStatus;

  //
  // Read SEC FWSTS
  //
  SeCFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);
  DEBUG ((DEBUG_INFO, "SecFwsts = %x.\n", SeCFirmwareStatus.ul));

  switch (BiosAction) {
    case 0:
      //
      // Case: DID ACK was not received
      //
      DEBUG ((DEBUG_ERROR, "DID Ack was not received, no BIOS Action to process.\n"));
      break;

    case CBM_DIR_NON_PCR:
      //
      // Case: Perform Non-Power Cycle Reset
      //
      DEBUG ((DEBUG_ERROR, "SEC FW has requested a Non-PCR.\n"));
      Status = PerformReset (CBM_DIR_NON_PCR);
      break;

    case CBM_DIR_PCR:
      //
      // Case: Perform Power Cycle Reset
      //
      DEBUG ((DEBUG_ERROR, "SEC FW has requested a PCR.\n"));
      Status = PerformReset (CBM_DIR_PCR);
      break;

    case 3:
      //
      // Case: Go To S3
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S3.  Not defined, continuing to POST.\n"));
      break;

    case 4:
      //
      // Case: Go To S4
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S4.  Not defined, continuing to POST.\n"));
      break;

    case 5:
      //
      // Case: Go To S5
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S5.  Not defined, continuing to POST.\n"));
      break;

    case CBM_DIR_GLOBAL_RESET:
      //
      // Case: Perform Global Reset
      //
      DEBUG ((DEBUG_ERROR, "SEC FW has requested a Global Reset.\n"));
      Status = PerformReset (CBM_DIR_GLOBAL_RESET);
      break;

    case CBM_DIR_CONTINUE_POST:
      //
      // Case: Continue to POST
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID Ack requested to continue to POST.\n"));
      break;
  }

  return EFI_SUCCESS;
}


/**
  This procedure will configure the SEC Host General Status register,
  indicating that DRAM Initialization is complete and SeC FW may
  begin using the allocated SeC UMA space.


  @param PeiServices     General purpose services available to every PEIM.
  @param MrcBootMode     MRC BootMode
  @param InitStat        H_GS[27:24] Status
  @param SeCUmaBase      LSB of base address
                         SeCUmaBaseEx - MSB of base address
                         SeCUmaSIze -  Allocated size of UMA
  @param SeCUmaSize      EDES_TODO: Add parameter description

  @retval EFI_SUCCESS

**/

EFI_STATUS
SeCConfigDidReg (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  MRC_BOOT_MODE_T           MrcBootMode,
  UINT8                     InitStat,
  UINT32                    SeCUmaBase,
  UINT32                    *SeCUmaSize
  )
{
  return EFI_SUCCESS;
}

#ifdef FSP_FLAG
VOID FspCheckBootDevice (
  IN OUT INTN                *DeviceIndex,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
}
#endif

VOID CheckBootDevice (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  INTN                    DeviceIndex;
  EFI_STATUS              Status;
  MBP_CURRENT_BOOT_MEDIA  BootMediaData;
  MBP_IFWI_DNX_REQUEST    IfwiDnxRequest;

  SetMem (&BootMediaData, sizeof (MBP_CURRENT_BOOT_MEDIA), 0xFF);


  Status = HeciGetIfwiDnxRequest (&IfwiDnxRequest);
  if (EFI_ERROR (Status)) {
    SetMem (&IfwiDnxRequest, sizeof (MBP_IFWI_DNX_REQUEST), 0x0);
  }

  Status = HeciGetBootDevice (&BootMediaData);

  DeviceIndex = BootMediaData.PhysicalData;

  if (DeviceIndex == 0) {
    DEBUG ((DEBUG_INFO, "CSE Boot Device is EMMC.\n"));
    Status = (*PeiServices)->InstallPpi(PeiServices, mCseEmmcSelectPpiList);
  } else if (DeviceIndex == 1) {
    DEBUG ((DEBUG_INFO, "CSE Boot Device is UFS.\n"));
    Status = (*PeiServices)->InstallPpi(PeiServices, mCseUfsSelectPpiList);
  } else if (DeviceIndex == 2) {
    DEBUG ((DEBUG_INFO, "CSE Boot Device is SPI.\n"));
    Status = (*PeiServices)->InstallPpi(PeiServices, mCseSpiSelectPpiList);
  }  else {
    DEBUG ((EFI_D_ERROR, "\nCSE Boot device is unknown (DevIndx: 0x%x).  Cannot continue!\n", DeviceIndex));
    CpuDeadLoop ();
  }

  //
  // Build HOB for BootMediaData
  //
  BuildGuidDataHob (
    &gEfiBootMediaHobGuid,
    &BootMediaData,
    sizeof (MBP_CURRENT_BOOT_MEDIA)
    );

  //
  // Build HOB for IfwiDnxRequest
  //
  BuildGuidDataHob (
    &gEfiIfwiDnxRequestHobGuid,
    &IfwiDnxRequest,
    sizeof (MBP_IFWI_DNX_REQUEST)
    );
  if (DeviceIndex != 2) {
    HeciTakeOwnerShip();
  }
}


EFI_STATUS
SeCTakeOwnerShip (
  )
{
  return EFI_SUCCESS;
}


/**
  This procedure will issue a Non-Power Cycle, Power Cycle, or Global Rest.

  @param   ResetType         Type of reset to be issued.

  @retval  EFI_SUCCESS

**/
EFI_STATUS
PerformReset (
  UINT8       ResetType
  )
{
  EFI_STATUS  Status;
  UINT32      Data32;
  UINT32      GpioBase;
  UINT8       Reset;
  UINT32      ETR;

  Reset     = 0;
  GpioBase  = 0;

  Status = ClearDISB ();

  ETR = (UINT32) MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC_IPC1, R_PMC_PMIR);
  MmioAnd32 (
    (UINTN) ETR,
    (UINT32)~(B_PMC_PMIR_CF9GR)
    );

  Reset = IoRead8 (R_RST_CNT);
  Reset &= 0xF1;

  switch (ResetType) {
    case 0:
      //
      // Case: DID ACK was not received
      //
      DEBUG ((DEBUG_ERROR, "DID Ack was not received, no BIOS Action to process.\n"));
      break;

    case CBM_DIR_NON_PCR:
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested a Non Power Cycle Reset.\n"));
      Reset |= 0x06;
      break;

    case CBM_DIR_PCR:
      //
      // Power Cycle Reset requested
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested a Power Cycle Reset.\n"));
      Reset |= 0x0E;
      break;

    case 3:
      //
      // Case: Go To S3
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S3.  Not defined, continuing to POST.\n"));
      break;

    case 4:
      //
      // Case: Go To S4
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S4.  Not defined, continuing to POST.\n"));
      break;

    case 5:
      //
      // Case: Go To S5
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID ACK has requested entry to S5.  Not defined, continuing to POST.\n"));
      break;

    case CBM_DIR_GLOBAL_RESET:
      //
      // Global Reset
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID Ack requested a global reset.\n"));

      Data32 = IoRead32 (GpioBase + 0x60);
      Data32 |= BIT30;
      IoWrite32 (GpioBase + 0x60, Data32);
      MmioOr32 (
      (UINTN) ETR,
      (UINT32) (B_PMC_PMIR_CF9GR)
      );

      DEBUG ((DEBUG_INFO, "Issuing global reset.\n"));
      Reset |= 0x0E;
      break;

    case CBM_DIR_CONTINUE_POST:
      //
      // Case: Continue to POST
      //
      DEBUG ((DEBUG_INFO, "SEC FW DID Ack requested to continue to POST.\n"));
      break;
  }
  //
  // Write SC RST CNT, Issue Reset
  //
  IoWrite8 (R_RST_CNT, Reset);

  return EFI_SUCCESS;
}


/**
  This procedure will clear the DISB.

  @param   VOID

  @retval  EFI_SUCCESS

**/
EFI_STATUS
ClearDISB (
  VOID
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SeCUmaEntry (
  IN EFI_PEI_FILE_HANDLE     *FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_PPI_DESCRIPTOR            *PeiPpiDescriptor;
#ifndef FSP_FLAG
  EFI_BOOT_MODE                     BootMode;
#endif

  if (!ImageInMemory) {
    Status = (*PeiServices)->InstallPpi (PeiServices, mSeCUmaPpiList);
    ASSERT_EFI_ERROR (Status);
  } else {
    //
    // locate the SecUma PPI
    //
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gSeCUmaPpiGuid, // GUID
                               0,                                  // INSTANCE
                               &PeiPpiDescriptor,          // EFI_PEI_PPI_DESCRIPTOR
                               NULL // PPI
                               );
    if (Status == EFI_SUCCESS) {
      //
      // Reinstall the SecUma PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 PeiPpiDescriptor,
                                 mSeCUmaPpiList
                                 );
    }
#ifndef FSP_FLAG
    Status = (*PeiServices)->GetBootMode (
                               PeiServices,
                               &BootMode
                               );

    if (EFI_ERROR (Status) || BootMode != BOOT_ON_S3_RESUME) {
      DEBUG((DEBUG_INFO, "SeCUmaEntry() CheckBootDevice, %r\n", Status));
      CheckBootDevice (PeiServices);
    } else {
      Status = PeiServicesNotifyPpi (mTxeNotifyList);
      HeciReset (HECI2_DEVICE);
    }
#endif
  }

  return Status;
}


#ifndef FSP_FLAG
VOID
HideHeci23 (
  VOID
  )
{
}


/**
  Check it's SPI boot path or not.

  @retval  TRUE                 SPI Boot path
  @retval  FALSE                Not SPI boot path

**/
BOOLEAN
IsSpiBoot (
  VOID
  )
{
  VOID                                  *HobList;
  MBP_CURRENT_BOOT_MEDIA                *BootMediaData;

  DEBUG ((EFI_D_INFO, "IsSpiBoot Start!\n"));

  HobList = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  if (HobList != NULL) {
    DEBUG ((EFI_D_INFO, "IsSpiBoot HobList != NULL\n"));
    BootMediaData = GET_GUID_HOB_DATA (HobList);
    if (BootMediaData->PhysicalData == BOOT_FROM_SPI) {
      DEBUG ((EFI_D_INFO, "BootMediaData->PhysicalData ==  IsSpiBoot\n"));
      return TRUE;
    } else {
      DEBUG ((EFI_D_INFO, "Not boot from SPI\n"));
      return FALSE;
    }
  }

  return FALSE;
}


/**
  Txe End of PEI callback function. This is the last event before entering DXE and OS in S3 resume.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval    EFI_SUCCESS          Succeeds.

**/
EFI_STATUS
EFIAPI
TxeOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  )
{
  DEBUG ((EFI_D_INFO, "TxeOnEndOfPei Start!\n"));

  PERF_START_EX (NULL, NULL, NULL, 0, 0x8100);
  HideHeci23 ();
  PERF_END_EX (NULL, NULL, NULL, 0, 0x8101);

  DEBUG ((EFI_D_INFO, "TxeOnEndOfPei Exit!\n"));

  return EFI_SUCCESS;
}
#endif

