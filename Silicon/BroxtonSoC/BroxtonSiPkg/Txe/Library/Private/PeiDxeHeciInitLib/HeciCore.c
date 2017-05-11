/** @file
  Heci driver core. For Dxe Phase, determines the HECI device and initializes it.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "HeciCore.h"
#include "HeciHpet.h"
#include <Uefi/UefiBaseType.h>
#include <Pi/PiPeiCis.h>
#include <Library/HeciMsgLib.h>
#include <Library/Heci2PowerManagementLib.h>
#include <Library/HobLib.h>
#include <Protocol/Heci.h>
#include <Private/Library/HeciInitLib.h>

extern EFI_GUID gEfiHeciMbpDataHobGuid;

BOOLEAN  mFdoModeEnabled = FALSE;

//
// Macro definition for function used in Heci driver
//
/**
  The routing of MmIo Read Dword

  @param[in]  a     The address of Mmio

  @retval           Return the value of MmIo Read

**/
UINT32
MmIoReadDword (
  UINTN a
  )
{
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr = (HECI_HOST_CONTROL_REGISTER *) a;

  return HeciRegHCsrPtr->ul;
}


/**
  The routing of MmIo Write Dword

  @param[in]  a   The address of Mmio
  @param[in]  b   Value revised

  @retval     None

**/
VOID
MmIoWriteDword (
  UINTN  a,
  UINT32 b
  )
{
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr      = (HECI_HOST_CONTROL_REGISTER *) a;

  HeciRegHCsrPtr->ul  = b;
}

#define MMIOREADDWORD(a)      MmIoReadDword (a)
#define MMIOWRITEDWORD(a, b)  MmIoWriteDword (a, b)

//
// Heci driver function definitions
//

/**
  WaitForCseReady

  @param[in]  HECI Device Number

  @retval     EFI_STATUS

**/
EFI_STATUS
WaitForCseReady (
  IN HECI_DEVICE                        HeciDev
  );

#ifdef EFI_DEBUG
/**
  For serial debugger used, it will show the buffer message to serila consol.

  @param[in]  Message    The address point of buffer message
  @param[in]  Length     Message length

  @retval     None
**/
VOID
ShowBuffer (
  UINT8  *Message,
  UINT32 Length
  )
{
  UINT32    LineBreak;
  UINT32    Index;
  LineBreak = 0;
  Index     = 0;

  while (Length-- > 0) {
    if (LineBreak == 0) {
      DEBUG ((EFI_D_ERROR, "%02x: ", (Index & 0xF0)));
    }
    DEBUG ((EFI_D_ERROR, "%02x ", Message[Index++]));
    LineBreak++;
    if (LineBreak == 16) {
      DEBUG ((EFI_D_ERROR, "\n"));
      LineBreak = 0;
    }
    if (LineBreak == 8) {
      DEBUG ((EFI_D_ERROR, "- "));
    }
  }
  DEBUG ((EFI_D_ERROR, "\n"));

  return ;
}

#endif // End Of EFI_DEBUG

//
// Heci driver function definitions
//
/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @retval EFI_STATUS

**/
EFI_STATUS
InitializeHeciPrivate (
  IN HECI_DEVICE                  HeciDev
  )
{
  volatile SICR_HOST_ALIVENESS_REQ    *SicrHostAlivenessReqPtr;
  volatile HICR_HOST_ALIVENESS_RESP   *HicrHostAlivenessRespPtr;
  volatile SICR_HOST_IPC_READINESS    *SicrHostIPCReadinessPtr;
  volatile HICR_SEC_IPC_READINESS     *HicrSeCIPCReadinessPtr;
  volatile HHISR                      *HostInterruptPtr;

  EFI_STATUS                          Status;
  HECI_FWS_REGISTER                   SeCFirmwareStatus;
  UINT32                              SeC_Exclusion_req;
  UINT32                              SeC_Exclusion_cause;
  UINTN                               HeciBaseAddress;
  UINT16                              DeviceInfo;
  UINTN                               HeciMBAR = 0;


  SeC_Exclusion_req = Mmio32 (PMC_BASE_ADDRESS, 0xc);
  if ((SeC_Exclusion_req & B_EXCLUSION) == B_EXCLUSION) {
    DEBUG ((EFI_D_INFO, "Error: SeC exclusion error \n"));
    SeC_Exclusion_cause = Mmio32 (PMC_BASE_ADDRESS, 0x2c);
    switch (SeC_Exclusion_cause & 0x0f) {
      case 0x0:
      case 0x4:
      case 0x5:
      case 0x6:
        DEBUG ((EFI_D_INFO, "Error: No HECI command send \n"));
        return EFI_UNSUPPORTED;
      default:
        break;
    }
  }
  Status = EFI_SUCCESS;

  //
  // Check for SEC MemValid status
  //
  HeciBaseAddress = MmPciBase (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev);

  if ((MmioRead32 (HeciBaseAddress + R_SEC_MEM_REQ) & B_SEC_MEM_REQ_INVALID) == B_SEC_MEM_REQ_INVALID) {
    //
    // SEC failed to start so no HECI
    //
    DEBUG ((EFI_D_ERROR, "SEC failed to start so no HECI\n"));
    return EFI_UNSUPPORTED;
  }

  SaveHpet ();
  do {
    //
    // Store HECI vendor and device information away
    //
    DeviceInfo = (UINT16) MmioRead32 (HeciBaseAddress + PCI_DEVICE_ID_OFFSET);

    //
    // Check for HECI-1 PCI device availability
    //
    if (DeviceInfo == 0xFFFF) {
      Status = EFI_DEVICE_ERROR;
      break;
    }

    SeCFirmwareStatus.ul = MmioRead32 (HeciBaseAddress + R_SEC_FW_STS0);

    //
    if (SeCFirmwareStatus.r.SeCOperationMode != SEC_MODE_NORMAL && SeCFirmwareStatus.r.SeCOperationState != SEC_IN_RECOVERY_MODE) {
      return EFI_UNSUPPORTED;
    }
    //
    // Check for SEC FPT Bad
    //
    if (SeCFirmwareStatus.r.FptBad) {
      Status = EFI_DEVICE_ERROR;
      break;
    }

    //
    // Check for SEC error status
    //
    if (SeCFirmwareStatus.r.ErrorCode) {
      //
      // SEC failed to start so no HECI
      //
      Status = EFI_DEVICE_ERROR;
      break;
    }

    //
    // Get HECI_MBAR and see if it is programmed
    // to a useable value
    //
    HeciMBAR = MmioRead32 (HeciBaseAddress + R_HECIMBAR0) & 0xFFFFFFF0;

    //
    // Load temporary address for HECI_MBAR if one is not assigned
    //
    if (HeciMBAR == 0) {
      DEBUG ((EFI_D_ERROR, "Heci MMIO Bar not programmed in DXE phase\n"));
    }
    //
    // Enable HECI BME, MSE and SERR
    //
    MmioOr32(
      HeciBaseAddress + PCI_COMMAND_OFFSET,
      EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_SERR
    );

    //
    // Set HECI interrupt delivery mode.
    // HECI-1 using legacy/MSI interrupt
    //
    MmioOr32 (HeciBaseAddress + R_HIDM, 0xFC);
#ifdef TESTMENU_FLAG
    SeCPolicyLibInit ();
    if (SeCHECIEnabled () != TRUE) {
      DEBUG ((EFI_D_ERROR, "HECI Unsupported ++ \n "));
      return EFI_UNSUPPORTED;
    }
#endif

    //
    // HECI MSG is unsupported if SEC MODE is in SEC ALT Disabled & SECOVR JMPR
    //
    if ((SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_SECOVR_JMPR) ||
        (SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_ALT_DISABLED) ||
        (SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_SOFT_TEMP_DISABLE) ||
        (SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_SECOVR_HECI_MSG)) {
      return EFI_UNSUPPORTED;
    }


    SicrHostAlivenessReqPtr =  (VOID *) (UINTN) (HeciMBAR + 0x214C);
    HicrHostAlivenessRespPtr =  (VOID *) (UINTN) (HeciMBAR + 0x2044);

    while (SicrHostAlivenessReqPtr->r.H_ALIVE_REQ != HicrHostAlivenessRespPtr->r.H_ACK);

    if (SicrHostAlivenessReqPtr->r.H_ALIVE_REQ == 1) {
      SeCAlivenessRequest (&HeciMBAR, 0);
    }

    SicrHostIPCReadinessPtr =  (VOID *) (UINTN) (HeciMBAR + 0x2150);
    SicrHostIPCReadinessPtr->r.RDY_CLR = 1;


    HicrSeCIPCReadinessPtr =  (VOID *) (UINTN) (HeciMBAR + 0x2040);

    while (HicrSeCIPCReadinessPtr->r.SEC_RDY != 1);


    HostInterruptPtr =  (VOID *) (UINTN) (HeciMBAR + 0x2020);
    HostInterruptPtr->r.INT_BAR0_STS = 0;
    HostInterruptPtr->r.INT_BAR1_STS = 0;


    Mmio32Or (HeciMBAR, 0x2154, 1);


    SicrHostIPCReadinessPtr->r.HOST_RDY = 1;

  } while (EFI_ERROR (Status));

  RestoreHpet ();

  DEBUG ((EFI_D_INFO, "InitializeHeciPrivate -- \n "));
  SeCAlivenessRequest (&HeciMBAR, 1);

  return Status;
}

UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE        HeciDev
  )
{
  UINTN   HeciBaseAddress;
  UINT32  Buffer[2];

  //
  // Check if HECI_MBAR has changed
  //
  HeciBaseAddress = MmPciBase (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER + HeciDev);

  DEBUG ((DEBUG_INFO, "CheckAndFixHeciForAccess Entry, For HECI Dev-%d \n", HeciDev));
  //
  // Check for HECI PCI device availability
  //
  if (MmioRead16 (HeciBaseAddress + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    DEBUG ((DEBUG_ERROR, "HECI PCI Device %d not found\n", HeciDev));
    return 0;
  }

  Buffer[0] = MmioRead32(HeciBaseAddress + R_HECIMBAR0) & 0xFFFFFFF0;
  Buffer[1] = 0x0;
  if ((MmioRead32 (HeciBaseAddress + R_HECIMBAR0) & 0x6) == 0x4) {
    Buffer[1] = MmioRead32 (HeciBaseAddress + R_HECIMBAR1);
  }

  DEBUG ((DEBUG_INFO, "HECI Device's R_HECIMBAR0 = 0x%x, R_HECIMBAR1 = 0x%x\n", Buffer[0], Buffer[1]));

  if (Buffer[0] == 0x0 && Buffer[1] == 0x0) {
    DEBUG ((DEBUG_ERROR, "HECI Device %d MMIO Bar isn't programmed in this phase\n", HeciDev ));
  } else if (Buffer[0] == 0xFFFFFFFF) {
    DEBUG ((DEBUG_ERROR, "HECI Device %d is not enabled in this phase\n", HeciDev));
  } else {
    //
    // Enable HECI BME, MSE and SERR
    //
    MmioOr32 (
      HeciBaseAddress + PCI_COMMAND_OFFSET,
      EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_SERR
      );
    DEBUG ((DEBUG_INFO, "CheckAndFixHeciForAccess Exit with MBAR\n"));
    return *((UINTN *) (&Buffer[0]));
  }

  return 0;
}


/**
  Waits for the CSE to report that it is ready for communication over the HECI
  interface.

  @param[in]  HECI Device Number

  @retval     EFI_STATUS

**/
EFI_STATUS
WaitForCseReady (
  IN HECI_DEVICE                        HeciDev
  )
{
  UINT32                                TimerStart;
  UINT32                                TimerEnd;
  volatile HECI_SEC_CONTROL_REGISTER    *HeciRegSeCCsrHaPtr;
  UINTN                                 HeciMBAR;
  volatile UINT32                       *HpetTimer;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  //
  //  Wait for SEC ready
  //
  DEBUG((DEBUG_INFO, "WaitForCseReady \n"));
  //
  // Check for SEC ready status
  //
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  HeciRegSeCCsrHaPtr = (VOID *) (UINTN) (HeciMBAR + SEC_CSR_HA);
  while (1) {
    if (HeciRegSeCCsrHaPtr->r.SEC_RST_HRA != 0) {
      //
      // CSE requests HECI reset
      //
      return ResetHeciInterface (HeciDev);
    }
    if (HeciRegSeCCsrHaPtr->r.SEC_RDY_HRA != 0) {
      //
      // CSE is ready!!!
      //
      return EFI_SUCCESS;
    }

    //
    // If 15 second timeout has expired, return fail
    //
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay (HECI_WAIT_DELAY);
  }
  //
  // CSE is ready!!!
  //
  return EFI_SUCCESS;
}


/**
  Checks if HECI reset has occured.

  @param[in]  HECI      Device Number

  @retval     TRUE      HECI reset occurred
  @retval     FALSE     No HECI reset occurred

**/
BOOLEAN
CheckForHeciReset (
  IN HECI_DEVICE                  HeciDev
  )
{
  UINTN                           HeciMBAR;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_SEC_CONTROL_REGISTER       HeciRegSeCCsrHa;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  if (HeciMBAR == 0) {
    return FALSE;
  }

  //
  // Init Host & CSE CSR
  //
  HeciRegHCsr.ul    = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegSeCCsrHa.ul = MMIOREADDWORD (HeciMBAR + SEC_CSR_HA);

  if ((HeciRegSeCCsrHa.r.SEC_RDY_HRA == 0) || (HeciRegHCsr.r.H_RDY == 0)) {
    return TRUE;
  }

  return FALSE;
}


/**
  HeciSendwACK

  @param[in, out]  Message
  @param[in, out]  Length
  @param[in, out]  RecLength
  @param[in]       HostAddress
  @param[in]       SeCAddress

  @return          EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendwACK(
  IN      HECI_DEVICE   HeciDev,
  IN OUT  UINT32        *Message,
  IN OUT  UINT32        Length,
  IN OUT  UINT32        *RecLength,
  IN      UINT8         HostAddress,
  IN      UINT8         SeCAddress
  )
{
  EFI_STATUS   Status;

  Status = EFI_SUCCESS;

  //
  // Send the message
  //
  DEBUG ((DEBUG_INFO, "HeciSendwAck () - Start\n"));
  Status = HeciSend (HeciDev, Message, Length, HostAddress, SeCAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Wait for ACK message
  //
  Status = HeciReceive (HeciDev, BLOCKING, Message, RecLength);

  return Status;
}


/**
  HeciReceive

  @param[in]       Blocking
  @param[in, out]  Message
  @param[in, out]  Length

  @retval          EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE   HeciDev,
  IN      UINT32        Blocking,
  IN OUT  UINT32        *MessageBody,
  IN OUT  UINT32        *Length
)
{
  UINTN                                ReadSize;
  UINTN                                Index;
  UINTN                                HeciMBAR;
  HECI_MESSAGE_HEADER                  MessageHeader;
  volatile     UINT32                  *ReadBuffer;
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER   *SecControlReg;
  UINTN                                StallCount;
  UINTN                                MaxCount;
  UINTN                                OverAllDelay;
  BOOLEAN                              TimeOut;
  EFI_HECI2_PM_PROTOCOL                *Heci2PmProtocol = NULL;
  EFI_STATUS                           Status;

  DEBUG ((EFI_D_INFO, "Start HeciReceive\n"));

  ReadSize = 0;

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  if (HeciMBAR == 0) {
    if (EFI_ERROR (GetHeci2PmProtocol (&Heci2PmProtocol))) {
      Heci2PmProtocol = NULL;
      DEBUG ((EFI_D_ERROR, "HeciReceive: Error getting HECI2 PM protocol.\n"));
      return EFI_DEVICE_ERROR;
    } else if (Heci2PmProtocol != NULL) {
      HeciMBAR = Heci2PmProtocol->GetHeciBar ();
    } else {
      return EFI_DEVICE_ERROR;
    }
  }

  if (CheckForHeciReset (HeciDev)) {
    //
    // If HECI reset than try to re-init HECI
    //
    Status = HeciInitialize (HeciDev);
    if (EFI_ERROR (Status)) {
     return EFI_DEVICE_ERROR;
    }
  }

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (HeciMBAR + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (HeciMBAR + SEC_CSR_HA);
  ReadBuffer = (UINT32 *) (UINTN) (HeciMBAR + SEC_CB_RW);
  DEBUG ((EFI_D_INFO, "Waiting for CSE notify, HostControlReg: %08x SecControlReg: %08x\n", HostControlReg->ul, SecControlReg->ul));

  MaxCount = 0;
  OverAllDelay = 0;
  TimeOut = FALSE;

  DEBUG_CODE_BEGIN ();
  MaxCount = CSE_WAIT_TIMEOUT * 1000 * 6;
  DEBUG_CODE_END ();

  if (MaxCount == 0) {
    MaxCount = CSE_WAIT_TIMEOUT * 1000;
  }

  while (TRUE) {
    DEBUG ((EFI_D_INFO, "Waiting for CSE notify, HostControlReg: %08x\n", HostControlReg->ul));
    if (HostControlReg->ul == 0x800B0909) {
      DEBUG ((EFI_D_INFO, "Need CSE to finish loading and release NVM control...\n"));
      DEBUG ((EFI_D_INFO, "\nIf you're stuck here for more than 30sec, Please check that your eMMC has GPP4.\n"));
    }
    StallCount = 0;
    while ((HostControlReg->r.H_IS == 0) && (SecControlReg->r.SEC_CBRP_HRA == SecControlReg->r.SEC_CBWP_HRA) && (StallCount < MaxCount)) {
      MicroSecondDelay (STALL_1_MILLISECOND);
      StallCount += 1;
    }
    if (StallCount == MaxCount) {
      TimeOut = TRUE;
      break;
    }

    OverAllDelay += StallCount;  // in Millisec
    DEBUG ((EFI_D_INFO, "Get CSE notify, HostControlReg: %08x\n", HostControlReg->ul));
    HostControlReg->r.H_RDY = 1;
    HostControlReg->r.H_IE = 0;
    DEBUG ((EFI_D_INFO, "Disable Interrupt, HostControlReg: %08x\n", HostControlReg->ul));
    DEBUG ((EFI_D_INFO, "Check SecControlReg: %08x\n", SecControlReg->ul));

    StallCount = 0;
    while ((SecControlReg->r.SEC_CBRP_HRA == SecControlReg->r.SEC_CBWP_HRA) && (StallCount < MaxCount)) {
      MicroSecondDelay(STALL_1_MILLISECOND);
      StallCount += 1;
    }
    if (StallCount >= MaxCount) {
      TimeOut = TRUE;
      break;
    }

    OverAllDelay += StallCount;  // in Millisec
    MessageHeader.Data = *ReadBuffer;
    *Length = MessageHeader.Fields.Length;
    DEBUG ((EFI_D_INFO, "Get Message Header: %08x\n", MessageHeader.Data));
    for (Index = 0; Index < (MessageHeader.Fields.Length + 3) / 4; Index++) {
      StallCount = 0;
      while ((SecControlReg->r.SEC_CBRP_HRA == SecControlReg->r.SEC_CBWP_HRA) && (StallCount < MaxCount)) {
        MicroSecondDelay (STALL_1_MILLISECOND);
        StallCount += 1;
      }
      if (StallCount >= MaxCount) {
        TimeOut = TRUE;
        break;
      }
      OverAllDelay += StallCount;  // in Millisec

      MessageBody[Index + ReadSize] = *ReadBuffer;
      DEBUG ((EFI_D_INFO, "MessageBody[%x] = %08x\n", Index+ReadSize, MessageBody[Index + ReadSize]));
    }
    DEBUG ((EFI_D_INFO, "Enable Host to get CSE interrupt: %08x\n", HostControlReg->ul));
    HostControlReg->r.H_IS = 1;
    HostControlReg->r.H_RDY = 1;
    HostControlReg->r.H_IE = 1;
    HostControlReg->r.H_IG = 1;
    DEBUG ((EFI_D_INFO, "get CSE interrupt Enabled: %08x\n", HostControlReg->ul));
    if (MessageHeader.Fields.MessageComplete == 1) {
      DEBUG((EFI_D_INFO, "Not more data need be receive, end\n"));
      TimeOut = FALSE;
      break;
    } else {
      MicroSecondDelay (STALL_1_MILLISECOND);
      OverAllDelay += 1;
      ReadSize += Index;
      if (OverAllDelay >= MaxCount) {
        TimeOut = TRUE;
        break;
      }
    }
  }

  if (TimeOut == TRUE) {
    DEBUG ((EFI_D_INFO, "HeciReceive TIMEOUT\n"));
    return EFI_TIMEOUT;
  }
  DEBUG ((EFI_D_INFO, "End HeciReceive \n"));

  return EFI_SUCCESS;
}


/**
  HeciSend

  @param[in]  HeciDev
  @param[in]  Message
  @param[in]  Length
  @param[in]  HostAddress
  @param[in]  SeCAddress

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSend (
  IN     HECI_DEVICE  HeciDev,
  IN     UINT32       *Message,
  IN     UINT32       Length,
  IN     UINT8        HostAddress,
  IN     UINT8        SeCAddress
  )
{
  UINTN                  LeftSize;
  UINTN                  MaxBuffer;
  UINTN                  WriteSize;
  UINTN                  Size;
  UINTN                  Index;
  UINTN                  HeciMBAR;
  UINTN                  StallCount;
  UINTN                  MaxCount;

  BOOLEAN                TimeOut;
  HECI_MESSAGE_HEADER    MessageHeader;
  EFI_HECI2_PM_PROTOCOL  *Heci2PmProtocol;
  EFI_STATUS             Status;

  volatile UINT32                      *WriteBuffer;
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER   *SecControlReg;

  Heci2PmProtocol = NULL;

  DEBUG ((EFI_D_INFO, "Start HeciSend \n"));

  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  if (HeciMBAR == 0) {
    if (EFI_ERROR (GetHeci2PmProtocol (&Heci2PmProtocol))) {
      Heci2PmProtocol = NULL;
      DEBUG ((EFI_D_ERROR, "HeciReceive: Error getting HECI2 PM protocol.\n"));
      return EFI_DEVICE_ERROR;
    } else if (Heci2PmProtocol != NULL) {
      HeciMBAR = Heci2PmProtocol->GetHeciBar ();
    } else {
      return EFI_DEVICE_ERROR;
    }
  }

  if (CheckForHeciReset (HeciDev)) {
    //
    // If HECI reset than try to re-init HECI
    //
    Status = HeciInitialize (HeciDev);
    if (EFI_ERROR (Status)) {
     return EFI_DEVICE_ERROR;
    }
  }

  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER  *) (UINTN) (HeciMBAR + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (HeciMBAR + SEC_CSR_HA);
  WriteBuffer = (UINT32 *)(UINTN)(HeciMBAR + H_CB_WW);

  MaxBuffer = HostControlReg->r.H_CBD;

  MaxCount = 0;

  TimeOut = FALSE;

  DEBUG_CODE_BEGIN ();
  MaxCount = CSE_WAIT_TIMEOUT * 1000 * 6;
  DEBUG_CODE_END ();

  if (MaxCount == 0) {
    MaxCount = CSE_WAIT_TIMEOUT * 1000;
  }

  MaxBuffer -= 1;
  LeftSize = (Length + 3) / 4;
  WriteSize = 0;
  HostControlReg->r.H_RDY = 1;
  while (LeftSize > 0) {
    DEBUG ((EFI_D_INFO, "Wait for CSE ready, SecControlReg %x\n", SecControlReg->ul));
    StallCount = 0;
    while ((SecControlReg->r.SEC_RDY_HRA == 0) && (StallCount < MaxCount)) {
      MicroSecondDelay (STALL_1_MILLISECOND);
      StallCount += 1;
    }
    if (StallCount == MaxCount) {
      TimeOut = TRUE;
      break;
    }

    DEBUG ((EFI_D_INFO, "CSE ready SecControlReg %x\n", SecControlReg->ul));
    HostControlReg->r.H_RDY = 1;
    HostControlReg->r.H_IE = 0;

    Size = (LeftSize > MaxBuffer) ? MaxBuffer : LeftSize;

    LeftSize -= Size;
    //
    // Prepare message header
    //
    MessageHeader.Data = 0;
    MessageHeader.Fields.SeCAddress = SeCAddress;
    MessageHeader.Fields.HostAddress = HostAddress;
    MessageHeader.Fields.MessageComplete = (LeftSize > 0) ? 0 : 1;
    MessageHeader.Fields.Length = (UINT32) ((LeftSize > 0) ? Size * sizeof (UINT32) : Length - WriteSize * sizeof (UINT32));
    DEBUG ((EFI_D_INFO, "Heci Message Header: %08x\n", MessageHeader.Data));
    *WriteBuffer = MessageHeader.Data;
    for (Index = 0; Index < Size; Index++) {
      DEBUG((EFI_D_INFO, "Message[%x] = %08x\n", Index, Message[Index + WriteSize]));
      *WriteBuffer = Message[Index + WriteSize];
    }
    //
    // Send the Interrupt;
    //
    DEBUG ((EFI_D_INFO, "Prepare Send Interrupt to CSE: %08x\n", HostControlReg->ul));
    HostControlReg->r.H_IS = 1;
    HostControlReg->r.H_RDY = 1;
    HostControlReg->r.H_IE = 1;
    HostControlReg->r.H_IG = 1;
    DEBUG ((EFI_D_INFO, "Send Interrupt to CSE: %08x\n", HostControlReg->ul));

    WriteSize += Size;
    if (LeftSize > 0) {
      DEBUG ((EFI_D_INFO, "More Data Need be sent, waiting CSE notify\n"));
      DEBUG ((EFI_D_INFO, "HostControlReg %x\n", SecControlReg->ul));
      StallCount = 0;
      while ((HostControlReg->r.H_IS == 0) && (StallCount < MaxCount)) {
        MicroSecondDelay (STALL_1_MILLISECOND);
        StallCount += 1;
      }
      if (StallCount == MaxCount) {
        TimeOut = TRUE;
        break;
      }

      DEBUG ((EFI_D_INFO, "Get CSE notify, HostControlReg %x\n", SecControlReg->ul));
    }
  }
  DEBUG ((EFI_D_INFO, "No More Data Need be sent."));
  DEBUG ((EFI_D_INFO, "End HeciSend \n"));

  if (TimeOut == TRUE) {
    DEBUG ((EFI_D_INFO, "HeciReceive TIMEOUT\n"));
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm

  @param[in] HeciDev    Heci Device


  @retval    EFI_STATUS

**/
EFI_STATUS
EFIAPI
ResetHeciInterface(
  IN      HECI_DEVICE      HeciDev
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  HECI_SEC_CONTROL_REGISTER   HeciRegSeCCsrHa;
  UINT32                      TimerStart;
  UINT32                      TimerEnd;
  volatile UINT32             *HpetTimer;
  UINTN                       HeciMBAR;

  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  //
  // Enable Reset
  //
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);

  //
  // CSE reset may be asserted, mask H_IS bit as write 1 will clear the bit.
  //
  HeciRegHCsr.ul &= (~ BIT1);

  HeciRegHCsr.r.H_RST = 1;
  HeciRegHCsr.r.H_IG = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  //
  // Make sure that the reset started
  //
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    //
    // If 5 second timeout has expired, return fail
    //
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    //
    // Read the SEC CSR
    //
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  } while (HeciRegHCsr.r.H_RDY == 1);

  //
  // Wait for SEC to perform reset
  //
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    //
    // If 5 second timeout has expired, return fail
    //
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    //
    // Read the SEC CSR
    //
    HeciRegSeCCsrHa.ul = MMIOREADDWORD (HeciMBAR + SEC_CSR_HA);
  } while (HeciRegSeCCsrHa.r.SEC_RDY_HRA == 0);

  //
  // Make sure IS has been signaled on the HOST side
  //
  HpetTimer = StartTimer(&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    //
    // If 5 second timeout has expired, return fail
    //
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    //
    // Read the SEC CSR
    //
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  } while (HeciRegHCsr.r.H_IS == 0);

  //
  // Enable host side interface
  //
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);;
  HeciRegHCsr.r.H_RST = 0;
  HeciRegHCsr.r.H_IG = 1;
  HeciRegHCsr.r.H_RDY = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  return EFI_SUCCESS;
}


/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in]  HeciDev        HECI Device No.

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciInitialize(
  IN  HECI_DEVICE      HeciDev
  )
{
  UINTN                               HeciBaseAddress;
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  HECI_FWS_REGISTER                   SeCFirmwareStatus;
  UINTN                               HeciMBAR;
  EFI_STATUS                          Status;


  DEBUG ((EFI_D_INFO, "HECI Initialize  ++ \n "));


  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  HeciBaseAddress = MmPciBase (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER);

  HeciRegHCsrPtr = (VOID *) (UINTN) (HeciMBAR + H_CSR);
  HeciRegHCsr.ul = HeciRegHCsrPtr->ul;

  //
  // Read H_RDY bit to check if we're already initialized
  //
  if (HeciRegHCsr.r.H_RDY == 1) {
    return EFI_SUCCESS;
  }
  SeCFirmwareStatus.ul = MmioRead32 (HeciBaseAddress + R_SEC_FW_STS0);
  DEBUG ((DEBUG_INFO, "HECI Dev HfSts1 = 0x%x \n", SeCFirmwareStatus.ul));
  if (SeCFirmwareStatus.ul == 0 || SeCFirmwareStatus.ul == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Check for SEC FPT Bad
  //
  if (SeCFirmwareStatus.r.FptBad != 0) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Check for SEC error status
  //
  if (SeCFirmwareStatus.r.ErrorCode != SEC_ERROR_CODE_NO_ERROR) {
    //
    // SEC failed to start so no HECI
    //
    return EFI_DEVICE_ERROR;
  }

  //
  // HECI MSG is unsupported if ME MODE is in ME ALT Disabled & SECOVR JMPR
  //
  if ((SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_SECOVR_JMPR) ||
    (SeCFirmwareStatus.r.SeCOperationMode == SEC_OPERATION_MODE_ALT_DISABLED)) {
    return EFI_UNSUPPORTED;
  }

  MmioAnd8 (HeciBaseAddress + R_HIDM, 0xFC);

  Status = WaitForCseReady (HeciDev);

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "InitializeHeci () -  Set H_RDY\n"));
    HeciRegHCsr.ul = HeciRegHCsrPtr->ul;
    if (HeciRegHCsrPtr->r.H_RDY == 0) {
      HeciRegHCsr.r.H_RST = 0;
      HeciRegHCsr.r.H_RDY = 1;
      HeciRegHCsr.r.H_IG = 1;
      HeciRegHCsrPtr->ul = HeciRegHCsr.ul;
    }
  }

  DEBUG ((EFI_D_INFO, "HECI Initialize  -- \n "));

  return Status;
}


/**
  Heci Re-initializes it for Host

  @param[in] HeciDev     Heci Device

  @retval    EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  IN      HECI_DEVICE      HeciDev
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  EFI_STATUS                  Status;
  UINTN                       HeciMBAR;

  Status = EFI_SUCCESS;

  if (SeCResetWait (HeciDev, HECI_INIT_TIMEOUT) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    Status = ResetHeciInterface (HeciDev);
  }

  return Status;
}


/**
  SeC reset and waiting for ready.

  @param[in]  Delay           The biggest waiting time.

  @retval     EFI_TIMEOUT     Time out.
  @retval     EFI_SUCCESS     SeC ready.

**/
EFI_STATUS
EFIAPI
SeCResetWait (
  IN  HECI_DEVICE   HeciDev,
  IN  UINT32  Delay
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINT32                      TimerStart;
  UINT32                      TimerEnd;
  volatile UINT32             *HpetTimer;
  UINTN                       HeciMBAR;

  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HeciDev);

  //
  // Wait for the HOST Ready bit to be cleared to signal a reset
  //
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, Delay);
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  while (HeciRegHCsr.r.H_RDY == 1) {
    //
    // If timeout has expired, return fail
    //
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  }

  return EFI_SUCCESS;
}


/**
  Return SEC Status.

  @param[in]  SeCStatus      Pointer for status report.


  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciGetSeCStatus (
  IN UINT32                     *SeCStatus
  )
{
  HECI_FWS_REGISTER    SeCFirmwareStatus;

  if (SeCStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SeCFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);

  if (SeCFirmwareStatus.r.CurrentState == SEC_STATE_NORMAL && SeCFirmwareStatus.r.ErrorCode == SEC_ERROR_CODE_NO_ERROR) {
    *SeCStatus = SEC_READY;
  } else if (SeCFirmwareStatus.r.CurrentState == SEC_STATE_RECOVERY) {
    *SeCStatus = SEC_IN_RECOVERY_MODE;
  } else if (SeCFirmwareStatus.r.CurrentState == SEC_STATE_INIT) {
    *SeCStatus = SEC_INITIALIZING;
  } else if (SeCFirmwareStatus.r.CurrentState == SEC_STATE_DISABLE_WAIT) {
    *SeCStatus = SEC_DISABLE_WAIT;
  } else if (SeCFirmwareStatus.r.CurrentState == SEC_STATE_TRANSITION) {
    *SeCStatus = SEC_TRANSITION;
  } else {
    *SeCStatus = SEC_NOT_READY;
  }

  if (SeCFirmwareStatus.r.FwUpdateInprogress) {
    *SeCStatus |= SEC_FW_UPDATES_IN_PROGRESS;
  }

  if (SeCFirmwareStatus.r.FwInitComplete == SEC_FIRMWARE_COMPLETED) {
    *SeCStatus |= SEC_FW_INIT_COMPLETE;
  }

  if (SeCFirmwareStatus.r.SeCBootOptionsPresent == SEC_BOOT_OPTIONS_PRESENT) {
    *SeCStatus |= SEC_FW_BOOT_OPTIONS_PRESENT;
  }

  DEBUG ((EFI_D_INFO, "HECI SeCStatus %X\n", SeCFirmwareStatus.ul));

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
HeciGetSeCMode (
  IN  HECI_DEVICE               HeciDev,
  IN UINT32                     *SeCMode
  )
{
  HECI_FWS_REGISTER   SeCFirmwareStatus;

  if (SeCMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mFdoModeEnabled) {
    *SeCMode = SEC_MODE_RECOVER;
    DEBUG ((EFI_D_INFO, "HECI FDO Jumper ASSERT SeCMode %X\n", *SeCMode));
    return EFI_SUCCESS;
  }

  switch (HeciDev) {
    case HECI1_DEVICE:
      SeCFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);
      break;

    case HECI2_DEVICE:
      SeCFirmwareStatus.ul = Heci2PciRead32 (R_SEC_FW_STS0);
      break;

    case HECI3_DEVICE:
      SeCFirmwareStatus.ul = Heci3PciRead32 (R_SEC_FW_STS0);
      break;

    default:
      SeCFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);
  }

  switch (SeCFirmwareStatus.r.SeCOperationMode) {
    case SEC_OPERATION_MODE_NORMAL:
      *SeCMode = SEC_MODE_NORMAL;
      break;

    case SEC_OPERATION_MODE_ALT_DISABLED:
      *SeCMode = SEC_DEBUG_MODE_ALT_DIS; //debug Mode
      break;

    case SEC_OPERATION_MODE_SOFT_TEMP_DISABLE:
      *SeCMode = SEC_MODE_TEMP_DISABLED;
      break;

    case SEC_OPERATION_MODE_SECOVR_JMPR:
    case SEC_OPERATION_MODE_SECOVR_HECI_MSG:
      *SeCMode = SEC_MODE_RECOVER;
      break;

    default:
      *SeCMode = SEC_MODE_FAILED;
  }

  DEBUG ((EFI_D_INFO, "HECI SeCMode %X\n", SeCFirmwareStatus.r.SeCOperationMode));

  return EFI_SUCCESS;
}


/**
  HeciTakeOwnerShip

  @param[in]    None

  @return       EFI_STATUS

**/
EFI_STATUS
HeciTakeOwnerShip (
  )
{
  EFI_STATUS                     Status;
  UINT32                         HeciSendLength;
  UINT32                         HeciRecvLength;
  GET_OWNERSHIP_CMD_REQ_DATA     *SendNVMGet;
  GET_OWNERSHIP_CMD_RESP_DATA    *NVMGetResp;
  UINT32                         SeCMode;
  UINT32                         DataBuffer[0x40];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: TakeOwnerShip\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (SeCMode != SEC_MODE_NORMAL && SeCMode != SEC_MODE_RECOVER) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendNVMGet = (GET_OWNERSHIP_CMD_REQ_DATA *) DataBuffer;
  SendNVMGet->MKHIHeader.Fields.GroupId = 0xA;
  SendNVMGet->MKHIHeader.Fields.Command = HECI1_REQUEST_DEVICE_OWNERSHIP;

  DEBUG ((EFI_D_INFO, "WRITE_TO_BIOS_DATA_CMD_REQ_DATA size if %x\n", sizeof (GET_OWNERSHIP_CMD_REQ_DATA)));
  HeciSendLength = sizeof (GET_OWNERSHIP_CMD_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  NVMGetResp = (GET_OWNERSHIP_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%x\n", NVMGetResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%x\n", NVMGetResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%x\n", NVMGetResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%x\n", NVMGetResp->MKHIHeader.Fields.Result));

  return Status;
}


/**
  Calculate if the circular buffer has overflowed.

  @param[in]  ReadPointer    Location of the read pointer.
  @param[in]  WritePointer   Location of the write pointer.

  @retval     Number of filled slots.

**/
UINT8
FilledSlots (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer
  )
{
  UINT8  FilledSlots;

  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}


/**
  Calculate if the circular buffer has overflowed

  @param[in]  ReadPointer         Value read from host/me read pointer
  @param[in]  WritePointer        Value read from host/me write pointer
  @param[in]  BufferDepth         Value read from buffer depth register

  @retval     EFI_STATUS

**/
EFI_STATUS
OverflowCB (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer,
  IN  UINT32 BufferDepth
  )
{
  UINT8  FilledSlots;

  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  //
  // test for overflow
  //
  if (FilledSlots > ((UINT8) BufferDepth)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  HeciReset

  @param[in]   HeciBar

  @retval      EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciReset (
  IN    HECI_DEVICE   HeciDev
  )
{
  EFI_STATUS                           Status;
  UINTN                                HeciBar;
  UINT32                               SeCMode;
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER   *SecControlReg;

  DEBUG ((EFI_D_INFO, "HeciReset-CSE status updated only in HECI1 interface not in HECI2 \n"));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (SeCMode != SEC_MODE_NORMAL) {
    return EFI_UNSUPPORTED;
  }

  HeciBar = CheckAndFixHeciForAccess (HeciDev);
  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER *) (UINTN) (HeciBar + H_CSR);
  SecControlReg  = (volatile HECI_SEC_CONTROL_REGISTER *) (UINTN) (HeciBar + SEC_CSR_HA);

  //
  // SEC_RDY could already be set in CSE BUP
  // Only reset if SEC_RDY is not set
  //
  if (SecControlReg->r.SEC_RDY_HRA != 1) {
    while (TRUE) {
      HostControlReg->r.H_RST = 1;
      HostControlReg->r.H_IG = 1;
      if (HostControlReg->r.H_RDY == 0) {
        break;
      }
    }
  }

  while (TRUE) {
    if (SecControlReg->r.SEC_RDY_HRA == 1) {
      break;
    }
  }

  HostControlReg->r.H_RDY = 1;
  HostControlReg->r.H_IG = 1;
  HostControlReg->r.H_RST = 0;

  return EFI_SUCCESS;
}


/**
  HeciSendTest

  @param[in]  HeciBar
  @param[in]  Message
  @param[in]  Length
  @param[in]  HostAddress
  @param[in]  SeCAddress

  @retval     EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendTest (
  IN UINTN            HeciBar,
  IN UINT32           *Message,
  IN UINT32           Length,
  IN UINT8            HostAddress,
  IN UINT8            SeCAddress
  )
{
  UINTN                                LeftSize;
  UINTN                                MaxBuffer;
  UINTN                                WriteSize;
  UINTN                                Size;
  UINTN                                Index;
  UINT32                               *MessageBody;
  HECI_MESSAGE_HEADER                  MessageHeader;
  volatile UINT32                      *WriteBuffer;
  volatile HECI_HOST_CONTROL_REGISTER  *HostControlReg;
  volatile HECI_SEC_CONTROL_REGISTER   *SecControlReg;

  DEBUG ((EFI_D_INFO, "Start HeciSendTest \n"));
  HostControlReg = (volatile HECI_HOST_CONTROL_REGISTER *) (UINTN) (HeciBar + H_CSR);
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER *) (UINTN) (HeciBar + SEC_CSR_HA);
  DEBUG((EFI_D_INFO, "Start HeciSendTest %08x %08x\n", SecControlReg->ul, HostControlReg->ul));
  WriteBuffer = (UINT32 *) (UINTN) (HeciBar + H_CB_WW);
  MessageBody = (UINT32 *) Message;

  MaxBuffer = HostControlReg->r.H_CBD;

  //
  // The first DWORD used for send MessageHeader, so useable Buffer Size should Be MaxBuffer -1;
  //
  MaxBuffer -= 1;
  LeftSize = (Length + 3) / 4;
  WriteSize = 0;
  HostControlReg->r.H_RDY = 1;
  while (LeftSize > 0) {
    DEBUG ((EFI_D_INFO, "Wait for CSE ready, SecControlReg %x\n", SecControlReg->ul));
    while (SecControlReg->r.SEC_RDY_HRA == 0) {
      MicroSecondDelay (100000);
    }
    DEBUG ((EFI_D_INFO, "CSE ready SecControlReg %x\n", SecControlReg->ul));
    HostControlReg->r.H_RDY = 1;
    HostControlReg->r.H_IE = 0;

    Size = (LeftSize > MaxBuffer) ? MaxBuffer : LeftSize;

    LeftSize -= Size;
    //
    // Prepare message header
    //
    MessageHeader.Data                   = 0;
    MessageHeader.Fields.SeCAddress      = SeCAddress;
    MessageHeader.Fields.HostAddress     = HostAddress;
    MessageHeader.Fields.MessageComplete = 1;
    MessageHeader.Fields.Length          = (UINT32)((LeftSize > 0) ? Size * sizeof (UINT32) : Length - WriteSize * sizeof (UINT32));
    DEBUG((EFI_D_INFO, "Heci Message Header: %08x\n", MessageHeader.Data));
    *WriteBuffer = MessageHeader.Data;
    for (Index = 0; Index < Size; Index ++) {
      DEBUG ((EFI_D_INFO, "MessageBody[%x] = %08x\n", Index, MessageBody[Index + WriteSize]));
      *WriteBuffer = MessageBody[Index + WriteSize];
    }
    //
    // Send the Interrupt;
    //
    DEBUG ((EFI_D_INFO, "Prepare Send Interrupt to CSE: %08x\n", HostControlReg->ul));
    HostControlReg->r.H_IE = 1;
    HostControlReg->r.H_IG = 1;
    DEBUG ((EFI_D_INFO, "Send Interrupt to CSE: %08x\n", HostControlReg->ul));

    while (HostControlReg->r.H_IS == 0) {
      MicroSecondDelay (100000);
    }
    HostControlReg->r.H_IS = 1;
    WriteSize += Size;
    if (LeftSize > 0) {
      DEBUG ((EFI_D_INFO, "More Data Need be sent, waiting CSE notify\n"));
      DEBUG ((EFI_D_INFO, "HostControlReg %x\n", SecControlReg->ul));
      while (HostControlReg->r.H_IS == 0) {
        MicroSecondDelay (100000);
      }
      DEBUG ((EFI_D_INFO, "Get CSE notify, HostControlReg %x\n", SecControlReg->ul));
    }
  }
  DEBUG ((EFI_D_INFO, "No More Data Need be sent.\n"));
  DEBUG ((EFI_D_INFO, "End HeciSend %08x %08x\n", SecControlReg->ul, HostControlReg->ul));

  return EFI_SUCCESS;
}


/**
  Function sends one messsage through the HECI buffer and waits
  for the corresponding ACK message.

  @param[in, out] Message         Pointer to the message buffer.
                                  SendLength  - Length of the message in bytes.
                                  RecLength   - Length of the message response in bytes.
  @param[in]      Length          EDES_TODO: Add parameter description
  @param[in, out] RecLength       EDES_TODO: Add parameter description
  @param[in]      HostAddress     Address of the sending entity.
                                  MeAddress  - Address of the SEC entity that should receive the message.
  @param[in]      SeCAddress      EDES_TODO: Add parameter description

  @return         EFI_STATUS

**/
EFI_STATUS
EFIAPI
Heci2SendwACK(
  IN OUT  UINT32  *Message,
  IN      UINT32  Length,
  IN OUT  UINT32  *RecLength,
  IN      UINT8   HostAddress,
  IN      UINT8   SeCAddress
  )
{
  UINTN                  Heci2Bar;
  EFI_STATUS             Status           = EFI_SUCCESS;
  EFI_HECI2_PM_PROTOCOL  *Heci2PmProtocol = NULL;
  BOOLEAN                Heci2Idle        = FALSE;

  Heci2Bar = CheckAndFixHeciForAccess (HECI2_DEVICE);

  if (EFI_ERROR (GetHeci2PmProtocol (&Heci2PmProtocol))) {
    Heci2PmProtocol = NULL;
    DEBUG ((EFI_D_INFO, "Heci2SendwACK: Error getting HECI2 PM protocol.\n"));
  } else if (Heci2PmProtocol != NULL) {
    if (Heci2Bar != 0) {
      Heci2PmProtocol->SetHeciBar (Heci2Bar);
    }
    if (Heci2PmProtocol->IsIdle ()) {
      if (Heci2PmProtocol->AtRuntime ()) {
        Heci2Idle = TRUE;
      }
      Heci2PmProtocol->SetActive ();
    }
  }
  //
  // Send the message
  //
  DEBUG ((DEBUG_INFO, "Heci2SendwAck () - Start\n"));
  DEBUG ((DEBUG_INFO, "Message at 0x%x. Length = %d. RecLength at 0x%x. HostAddress = 0x%x. SecAddress = 0x%x.\n", (UINTN) Message, (UINTN) Length, (UINTN) RecLength, (UINTN) HostAddress, SeCAddress));

  Status = HeciSend (HECI2_DEVICE, Message, Length, HostAddress, SeCAddress);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Heci2SendwACK: Error in HeciSend.\n"));
    if (Heci2Idle) {
      Heci2PmProtocol->SetIdle ();
    }

    return Status;
  }

  //
  // Wait for ACK message
  //
  Status = HeciReceive (HECI2_DEVICE, BLOCKING, Message, RecLength);

  if (Heci2Idle) {
    Heci2PmProtocol->SetIdle ();
  }

  return Status;
}


/**
  Heci2SendwoACK

  @param[in, out] Message         Pointer to the message buffer.
                                  SendLength  - Length of the message in bytes.
                                  RecLength   - Length of the message response in bytes.
  @param[in]      Length          EDES_TODO: Add parameter description
  @param[in, out] RecLength       EDES_TODO: Add parameter description
  @param[in]      HostAddress     Address of the sending entity.
                                  MeAddress  - Address of the SEC entity that should receive the message.
  @param[in]      SeCAddress      EDES_TODO: Add parameter description

  @return         EFI_STATUS

**/
EFI_STATUS
EFIAPI
Heci2SendwoACK (
  IN OUT  UINT32  *Message,
  IN      UINT32  Length,
  IN OUT  UINT32  *RecLength,
  IN      UINT8   HostAddress,
  IN      UINT8   SeCAddress
  )
{
  EFI_STATUS  Status;
  UINTN       HeciBar;

  Status = EFI_SUCCESS;

  HeciBar = CheckAndFixHeciForAccess (HECI2_DEVICE);
  DEBUG ((EFI_D_INFO, "Heci2SendwACK HeciBar %08x\n", HeciBar));

  //
  // Send the message
  //
  DEBUG ((DEBUG_INFO, "HeciSendwAck () - Start\n"));
  Status = HeciSendTest (HeciBar, Message, Length, HostAddress, SeCAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}


/**
  HeciSendwoACK

  @param[in, out] Message
  @param[in]      Length
  @param[in, out] RecLength
  @param[in]      HostAddress
  @param[in]      SeCAddress

  @return         EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendwoACK(
  IN OUT  UINT32  *Message,
  IN      UINT32  Length,
  IN OUT  UINT32  *RecLength,
  IN      UINT8   HostAddress,
  IN      UINT8   SeCAddress
  )
{
  EFI_STATUS   Status;

  Status = EFI_SUCCESS;

  //
  // Send the message
  //
  DEBUG ((DEBUG_INFO, "HeciSendwoACK () - HeciSend\n"));
  Status = HeciSend (HECI1_DEVICE, Message, Length, HostAddress, SeCAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}


/**

  HeciGetMBPData

  @param[in] MBPData

  @return   EFI_STATUS

**/
EFI_STATUS
HeciGetMBPData (
  UINT32 * MBPData
  )
{
  EFI_STATUS                   Status;
  UINT32                       HeciSendLength;
  UINT32                       HeciRecvLength;
  MBP_CMD_REQ_DATA             *SendMBP;
  MBP_CMD_RESP_DATA            *MBPResp;
  UINT32                       SeCMode;
  UINT32                       DataBuffer[0x100];

  DEBUG((EFI_D_INFO, "BIOS Start Send HECI Message: HeciMBP\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (SeCMode != SEC_MODE_NORMAL && SeCMode != SEC_MODE_RECOVER) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendMBP= (MBP_CMD_REQ_DATA *) DataBuffer;
  SendMBP->MKHIHeader.Fields.GroupId = 0xF0;
  SendMBP->MKHIHeader.Fields.Command = 2;

  DEBUG ((EFI_D_INFO, "MBP_CMD_REQ_DATA size is %x\n", sizeof (MBP_CMD_REQ_DATA)));
  HeciSendLength = sizeof (MBP_CMD_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  MBPResp = (MBP_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", MBPResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", MBPResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", MBPResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", MBPResp->MKHIHeader.Fields.Result));

  if (MBPResp->MKHIHeader.Fields.Result == 0) {
    CopyMem (MBPData, DataBuffer, (MBPResp->Length + 1)* sizeof (UINT32));
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}


/**
  HeciMBP

  @param[in]  MBPData

  @return    EFI_STATUS

**/
EFI_STATUS
HeciMBP(
  UINT32 * MBPData
  )
{
  UINT32                   HeciMBPData[0x100];
  UINT32                   *pHeciMBPData = NULL;
  EFI_HOB_GUID_TYPE        *GuidHobPtr;
  EFI_STATUS               Status;

  GuidHobPtr = GetFirstGuidHob (&gEfiHeciMbpDataHobGuid);
  if (GuidHobPtr == NULL) {
    Status = HeciGetMBPData (HeciMBPData);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BuildGuidDataHob (
      &gEfiHeciMbpDataHobGuid,
      &HeciMBPData,
      (sizeof (UINT32) * 0x100)
      );
    //
    // Get Hob again for the first time to read the MBP data.
    //
    GuidHobPtr = GetFirstGuidHob (&gEfiHeciMbpDataHobGuid);
  }

  pHeciMBPData = (UINT32 *) GET_GUID_HOB_DATA (GuidHobPtr);
  CopyMem (MBPData, pHeciMBPData , 0x100 * sizeof (UINT32));

  return EFI_SUCCESS;
}


/**
  DumpBuffer_HECI

  @param[in] Buffer1

  @return    None

**/
VOID
DumpBuffer_HECI (
  VOID   *Buffer1,
  UINT8  bufferSize
  )
{
  DEBUG_CODE_BEGIN ();

  UINTN  Index;
  UINT8  *Buffer;

  Buffer = (UINT8 *) Buffer1;
  DEBUG ((EFI_D_INFO, "DumpBuffer 0x%08x\n", Buffer));
  for (Index = 0; Index < bufferSize; Index++) {
    DEBUG ((EFI_D_INFO, "%02x   ", Buffer[Index]));
    if ((bufferSize % 16) == 0){
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG_CODE_END ();
}


/**
  HeciGetBootDevice

  @param[in] BootMediaData

  @return    EFI_STATUS

**/
EFI_STATUS
HeciGetBootDevice (
  MBP_CURRENT_BOOT_MEDIA    *BootMediaData
  )
{
  EFI_STATUS                      Status;
  UINT32                          DataBuffer[0x100];
  MBP_CMD_RESP_DATA               *MBPHeader;
  MBP_ITEM_HEADER                 *MBPItem;
  MBP_CURRENT_BOOT_MEDIA          *BootMedia;

  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  Status = HeciMBP (DataBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MBPHeader = (MBP_CMD_RESP_DATA *) DataBuffer;
  DEBUG ((EFI_D_INFO, "HeciGetBootDevice 1\n"));
  DumpBuffer_HECI (MBPHeader, sizeof (MBP_CMD_RESP_DATA));

  MBPItem = (MBP_ITEM_HEADER *) (MBPHeader + 1);
  DEBUG ((EFI_D_INFO, "HeciGetBootDevice 2\n"));
  DumpBuffer_HECI (MBPItem, MBPHeader->Length);

  while ((UINT32 *) MBPItem < (UINT32 *) DataBuffer + MBPHeader->Length) {
    if (MBPItem->AppID == 8 && MBPItem->ItemID == 1) {
      DEBUG ((EFI_D_INFO, "HeciGetBootDevice 3\n"));
      DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
      BootMedia = (MBP_CURRENT_BOOT_MEDIA *) (MBPItem + 1);
      CopyMem ((VOID *) BootMediaData, (VOID *) BootMedia, sizeof (MBP_CURRENT_BOOT_MEDIA));
      return EFI_SUCCESS;
    }
    MBPItem = (MBP_ITEM_HEADER *) ((UINT32 *) MBPItem + MBPItem->Length);
    DEBUG ((EFI_D_INFO, "HeciGetBootDevice 4\n"));
    DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
  }

  return EFI_DEVICE_ERROR;
}


EFI_STATUS
SeCAlivenessRequest (
  IN      UINTN      *HeciMemBar,
  IN      UINT32     Request
  )
{
  UINT32             HostAlivenessResponse;
  UINT32             TimerStart;
  UINT32             TimerEnd;
  volatile UINT32    *HpetTimer;

  //
  // Check for SEC ready status
  //
  HpetTimer = StartTimer (&TimerStart, &TimerEnd, PEI_HECI_INIT_TIMEOUT);

  if (Request == 1) {
    Mmio32Or (*HeciMemBar, R_SICR_HOST_ALIVENESS_REQ, Request);
  } else {
    Mmio32And (*HeciMemBar, R_SICR_HOST_ALIVENESS_REQ, 0xfffffffe);
  }
  HostAlivenessResponse =  Mmio32 (*HeciMemBar, 0x2044);
  while ((HostAlivenessResponse & Request) != Request) {
    HostAlivenessResponse = Mmio32 (*HeciMemBar, 0x2044);
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
HeciInitLibConstructor (
  VOID
  )
{
  EFI_HOB_GUID_TYPE  *FdoEnabledGuidHob = NULL;

  FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);
  mFdoModeEnabled = (FdoEnabledGuidHob != NULL);

  return RETURN_SUCCESS;
}


/**
  HeciIshSendwAck

  @param[in]      SendMessage
  @param[out]     ReceiveMessage
  @param[in, out] Length
  @param[in, out] RecLength
  @param[in]      HostAddress
  @param[in]      SeCAddress

  @retval         EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciIshSendwAck (
  IN      HECI_DEVICE   HeciDev,
  IN      VOID          *SendMessage,
  OUT     VOID          *ReceiveMessage,
  IN      UINT32        SendLength,
  IN OUT  UINT32        *RecLength,
  IN      UINT8         HostAddress,
  IN      UINT8         SeCAddress
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  //
  // Send the message
  //
  DEBUG ((DEBUG_INFO, "HeciIshSendwAck () - Start\n"));
  Status = HeciSend (HeciDev, (UINT32 *) SendMessage, SendLength, HostAddress, SeCAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Wait for ACK message
  //
  Status = HeciReceive (HeciDev, BLOCKING, (UINT32 *) ReceiveMessage, RecLength);

  return Status;
}

