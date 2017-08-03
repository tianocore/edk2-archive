/** @file
  Implementation file for common HECI Message functionality.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <Library/PerformanceLib.h>
#include <Library/TimerLib.h>
#include <SeCState.h>
#include <CoreBiosMsg.h>
#include <HeciRegs.h>
#include <SeCAccess.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/Heci.h>
#include <Private/Library/HeciInitLib.h>
#include <Library/PeiDxeSmmMmPciLib.h>

#define MIRROR_RANGE_MCHBAR                0x65c8 //0x6458 // 0x65C8
#define MOT_OUT_BASE_Bunit                 0x6AF0
#define MOT_OUT_MASK_Bunit                 0x6AF4
#define MOT_OUT_BASE_Aunit                 0x64C0
#define MOT_OUT_MASK_Aunit                 0x64C4
#define SPARE_BIOS_MCHBAR                  0x647C // BXTM 0x6474

#define MCHBASE                            MmPciBase(SA_MC_BUS, SA_MC_DEV, SA_MC_FUN)
#define MCHBASE_BAR                        MmioRead32(MCHBASE + 0x48) &~BIT0
#define MCHMmioRead32(offset)              MmioRead32((MCHBASE_BAR)+offset)
#define MCHMmioWrite32(offset, data)       MmioWrite32((MCHBASE_BAR)+offset, data)

VOID* WriteCacheBuffer = NULL;
VOID* ReadCacheBuffer  = NULL;

/**
  Send DID Request Message through HECI.

  @param[in]  UMABase          The IMR Base address
  @param[out] UMASize          The IMR region size.
  @param[out] BiosAction       ME response to DID

  @retval     EFI_SUCCESS      Send DID success.
  @retval     Others           Send DID failed.

**/
EFI_STATUS
HeciSendDIDMessage (
  IN  UINT32 UMABase,
  IN  UINT8  IsS3,
  OUT UINT32 *UMASize,
  OUT UINT8  *BiosAction
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  DRAM_INIT_DONE_CMD_REQ          *SendDID;
  DRAM_INIT_DONE_CMD_RESP_DATA    *DIDResp;
  UINT32                          SeCMode;
  UINT32                          DataBuffer[0x70];
  UINT32                          temp32;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciSendDIDMessage\n"));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || ((SeCMode != SEC_MODE_NORMAL)&&(SEC_MODE_RECOVER !=SeCMode))) {
    DEBUG ((EFI_D_INFO, "CSE Firmware not in normal and SV mode\n"));
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));
  temp32 = MCHMmioRead32 (MOT_OUT_MASK_Bunit);
  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendDID = (DRAM_INIT_DONE_CMD_REQ *) DataBuffer;
  SendDID->MKHIHeader.Data              = 0;
  SendDID->MKHIHeader.Fields.Command    = DRAM_INIT_DONE_CMD; ///<DRAM INIT DONE =0x01
  SendDID->MKHIHeader.Fields.IsResponse = 0;
  SendDID->MKHIHeader.Fields.GroupId    = COMMON_GROUP_ID;//Group ID = 0xF0
  SendDID->ImrData.BiosMinImrsBa    = UMABase;

  if (TRUE == IsS3) {
    DEBUG ((EFI_D_INFO, "Setting  NonDestructiveAliasCheck in DID command successful\n"));
    SendDID->Flags.NonDestructiveAliasCheck=1;
  }
  HeciSendLength = sizeof (DRAM_INIT_DONE_CMD_REQ);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  DIDResp = (DRAM_INIT_DONE_CMD_RESP_DATA *) DataBuffer;

  if (DIDResp->MKHIHeader.Fields.Result == 0) {
    DEBUG ((EFI_D_INFO, "HeciSend DRAM Init Done successful\n"));
  }
  MCHMmioWrite32 (MOT_OUT_MASK_Bunit,temp32);
  DEBUG ((EFI_D_INFO, "ImrsSortedRegionBa =%x\n", DIDResp->ImrsData.ImrsSortedRegionBa));
  DEBUG ((EFI_D_INFO, "ImrsSortedRegionLen=%x\n", DIDResp->ImrsData.ImrsSortedRegionLen));
  DEBUG ((EFI_D_INFO, "OemSettingsRejected=%x\n", DIDResp->ImrsData.OemSettingsRejected));
  DEBUG ((EFI_D_INFO, "BiosAction         =%x\n", DIDResp->BiosAction));
  ASSERT (UMABase == DIDResp->ImrsData.ImrsSortedRegionBa);

  *UMASize = DIDResp->ImrsData.ImrsSortedRegionLen;
  *BiosAction = DIDResp->BiosAction;

  return Status;
}


/**
  Get NVM file's size through HECI1.

  @param[in]  FileName      The file name.
  @param[out] FileSize      The file's size.

  @retval     EFI_SUCCESS   Get NVM file size success.
  @retval     Others        Get NVM file size failed.

**/
EFI_STATUS
HeciGetNVMFileSize (
  IN  UINT8 *FileName,
  OUT UINTN *FileSize
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  GET_FILESIZE_CMD_REQ_DATA       *SendNVMGet;
  GET_FILESIZE_CMD_RESP_DATA      *NVMGetResp;
  UINT32                          SeCMode;
  UINT32                          DataBuffer[0x70];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: GetNVMFileSize\n"));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendNVMGet = (GET_FILESIZE_CMD_REQ_DATA *) DataBuffer;
  SendNVMGet->MKHIHeader.Fields.GroupId = 0xA;
  SendNVMGet->MKHIHeader.Fields.Command = HECI1_FILE_SIZE;
  AsciiStrCpyS ((CHAR8 *) SendNVMGet->FileName, MCA_MAX_FILE_NAME, (CONST CHAR8 *) FileName);

  DEBUG ((EFI_D_INFO, "WRITE_TO_BIOS_DATA_CMD_REQ_DATA size if %x\n", sizeof (GET_FILESIZE_CMD_REQ_DATA)));
  HeciSendLength = sizeof (GET_FILESIZE_CMD_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  NVMGetResp = (GET_FILESIZE_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%x\n", NVMGetResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%x\n", NVMGetResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%x\n", NVMGetResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%x\n", NVMGetResp->MKHIHeader.Fields.Result));

  switch (NVMGetResp->MKHIHeader.Fields.Result) {
    case BIOS_HECI_STATUS_OK:
      Status = EFI_SUCCESS;
      break;
    case BIOS_HECI_STATUS_INVALID_PARAM:
      Status = EFI_INVALID_PARAMETER;
      break;
    case BIOS_HECI_STATUS_FILE_NOT_FOUND:
      Status = EFI_NOT_FOUND;
      break;
    case BIOS_HECI_STATUS_AFTER_EOP:
      Status = EFI_DEVICE_ERROR;
      break;
    case BIOS_HECI_STATUS_ERROR:
      Status = EFI_DEVICE_ERROR;
      break;
    default:
      Status = EFI_DEVICE_ERROR;
      break;
  }
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "FileSize =%x\n", NVMGetResp->FileSize));
    *FileSize = NVMGetResp->FileSize;
  }

  return Status;
}


/**
  Set Read Write Temp memory.

  @param[in]  Address   Temp memory.

**/
VOID
HeciSetReadWriteCache (
  VOID*  Address
  )
{
  WriteCacheBuffer = Address;
  ReadCacheBuffer  = Address;
}


/**
  Write Data to NVM file through HECI1.

  @param[in] FileName     The file name.
  @param[in] Offset       The offset of data.
  @param[in] Data         The data content.
  @param[in] DataSize     Data's size.
  @param[in] Truncate     Truncate the file.

  @retval   EFI_SUCCESS   Write NVM file success.
  @retval   Others        Write NVM file failed.

**/
EFI_STATUS
HeciWriteNVMFile (
  IN UINT8     *FileName,
  IN UINT32    Offset,
  IN UINT8     *Data,
  IN UINTN     DataSize,
  IN BOOLEAN   Truncate
  )
{
  EFI_STATUS                           Status;
  UINT32                               HeciSendLength;
  UINT32                               HeciRecvLength;
  WRITE_TO_RPMB_STORAGE_CMD_REQ_DATA   *SendNVMWrite;
  WRITE_TO_RPMB_STORAGE_CMD_RESP_DATA  *NVMWriteResp;
  UINT32                               SeCMode;
  UINT32                               DataBuffer[0x70];
  VOID                                 *TempBuffer;
  VOID                                 *AllocatedBuffer;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: WriteNVMFile, FileOffset : 0x%x, DataBuffer: 0x%x, DataSize : 0x%x \n", Offset, Data, DataSize));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  AllocatedBuffer = NULL;
  TempBuffer = Data;
  if (WriteCacheBuffer != NULL) {
    TempBuffer = WriteCacheBuffer;
  } else {
    AllocatedBuffer = AllocatePages (EFI_SIZE_TO_PAGES (DataSize));
    ASSERT (AllocatedBuffer != NULL);
    if (AllocatedBuffer != NULL) {
      TempBuffer = AllocatedBuffer;
    }
  }
  CopyMem (TempBuffer, Data, DataSize);

  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  SendNVMWrite = (WRITE_TO_RPMB_STORAGE_CMD_REQ_DATA *) DataBuffer;
  SendNVMWrite->MKHIHeader.Data = 0;
  SendNVMWrite->MKHIHeader.Fields.GroupId = GROUP_ID_MCA;
  SendNVMWrite->MKHIHeader.Fields.Command = HECI1_WRITE_DATA;
  AsciiStrCpyS ((CHAR8 *) SendNVMWrite->FileName, sizeof (SendNVMWrite->FileName), (CONST CHAR8 *) FileName);
  SendNVMWrite->Offset = Offset;
  SendNVMWrite->Size = (UINT32) DataSize;
  SendNVMWrite->Truncate = (Truncate) ? 1 : 0;
  SendNVMWrite->SrcAddressLower = (UINT32) (UINTN) TempBuffer;
  SendNVMWrite->SrcAddressUpper = (UINT32) RShiftU64 ((PHYSICAL_ADDRESS) (UINTN) TempBuffer, 32);
  DEBUG ((EFI_D_INFO, "TempBuffer: 0x%x \n", TempBuffer));

  DEBUG ((EFI_D_INFO, "WRITE_TO_RPMB_STORAGE_CMD_REQ_DATA size if %x\n", sizeof(WRITE_TO_RPMB_STORAGE_CMD_REQ_DATA)));
  HeciSendLength = sizeof (WRITE_TO_RPMB_STORAGE_CMD_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  NVMWriteResp = (WRITE_TO_RPMB_STORAGE_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", NVMWriteResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", NVMWriteResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", NVMWriteResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", NVMWriteResp->MKHIHeader.Fields.Result));

  if (NVMWriteResp->MKHIHeader.Fields.Result != 0) {
    DEBUG ((EFI_D_ERROR, "NVM Write Failed. Update is lost. If repeatedly encountered, clear GPP4 and try again\n"));
    Status = EFI_DEVICE_ERROR;
  }

  if (AllocatedBuffer != NULL) {
    FreePages (AllocatedBuffer, EFI_SIZE_TO_PAGES (DataSize));
  }
  return Status;
}


/**
  Read NVM file data from HECI1.

  @param[in] FileName       The file name.
  @param[in] Offset         The offset of data.
  @param[in] Data           The data buffer.
  @param[in] DataSize       Data's size.

  @retval    EFI_SUCCESS    Read NVM file success.
  @retval    Others         Read NVM file failed.

**/
EFI_STATUS
HeciReadNVMFile(
  IN UINT8 *FileName,
  IN UINT32 Offset,
  IN UINT8 *Data,
  IN UINTN *DataSize
  )
{
  EFI_STATUS                            Status;
  UINT32                                HeciSendLength;
  UINT32                                HeciRecvLength;
  READ_FROM_RPMB_STORAGE_CMD_REQ_DATA   *SendNVMRead;
  READ_FROM_RPMB_STORAGE_CMD_RESP_DATA  *NVMReadResp;
  UINT32                                SeCMode;
  UINT32                                DataBuffer[0x70];
  VOID                                  *TempBuffer;
  VOID                                  *AllocatedBuffer;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: ReadNVMFile FileOffset : 0x%x, DataBuffer: 0x%x, DataSize : 0x%x \n", Offset, Data, *DataSize));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  AllocatedBuffer = NULL;
  TempBuffer = Data;
  if (ReadCacheBuffer != NULL) {
    TempBuffer = ReadCacheBuffer;
  } else {
    AllocatedBuffer = AllocatePages (EFI_SIZE_TO_PAGES (*DataSize));
    ASSERT (AllocatedBuffer != NULL);
    if (AllocatedBuffer != NULL) {
      TempBuffer = AllocatedBuffer;
    }
  }

  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  SendNVMRead = (READ_FROM_RPMB_STORAGE_CMD_REQ_DATA *) DataBuffer;
  SendNVMRead->MKHIHeader.Fields.GroupId = GROUP_ID_MCA;
  SendNVMRead->MKHIHeader.Fields.Command = HECI1_READ_DATA;
  AsciiStrCpyS ((CHAR8 *) SendNVMRead->FileName, sizeof (SendNVMRead->FileName), (CONST CHAR8 *) FileName);
  SendNVMRead->Offset = Offset;
  SendNVMRead->Size = (UINT16)*DataSize;
  SendNVMRead->DstAddressLower = (UINT32) (UINTN) TempBuffer;
  SendNVMRead->DstAddressUpper = (UINT32) RShiftU64 ((PHYSICAL_ADDRESS) (UINTN) TempBuffer, 32);
  DEBUG((EFI_D_INFO, "TempBuffer: 0x%x \n", TempBuffer));

  DEBUG ((EFI_D_INFO, "READ_FROM_RPMB_STORAGE_CMD_REQ_DATA size if %x\n", sizeof (READ_FROM_RPMB_STORAGE_CMD_REQ_DATA)));
  HeciSendLength = sizeof (READ_FROM_RPMB_STORAGE_CMD_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  NVMReadResp = (READ_FROM_RPMB_STORAGE_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", NVMReadResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", NVMReadResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", NVMReadResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", NVMReadResp->MKHIHeader.Fields.Result));
  DEBUG ((EFI_D_INFO, "DataSize =%08x\n", NVMReadResp->DataSize));

  if (NVMReadResp->MKHIHeader.Fields.Result == 0) {
    if (Data != TempBuffer) {
      CopyMem(Data, TempBuffer, NVMReadResp->DataSize);
    }
    *DataSize = NVMReadResp->DataSize;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  if (AllocatedBuffer != NULL) {
    FreePages (AllocatedBuffer, EFI_SIZE_TO_PAGES (*DataSize));
  }

  return Status;
}


EFI_STATUS
HeciDataClearLock (
void
  )
{
  EFI_STATUS                Status;
  UINT32                    HeciSendLength;
  UINT32                    HeciRecvLength;
  DATA_CLEAR_LOCK_REQ       *SendDCL;
  DATA_CLEAR_LOCK_RES       *DCLResp;
  UINT32                    SeCMode;
  UINT32                    DataBuffer[0x40];

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));
  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  SendDCL = (DATA_CLEAR_LOCK_REQ *) DataBuffer;
  SendDCL->MKHIHeader.Fields.GroupId     = MKHI_IFWI_UPDATE_GROUP_ID;
  SendDCL->MKHIHeader.Fields.Command     = DATA_CLEAR_LOCK_CMD_ID;
  SendDCL->MKHIHeader.Fields.IsResponse  = 0;
  DEBUG ((EFI_D_INFO, "HeciDataClearLock size is %x\n", sizeof (DATA_CLEAR_LOCK_RES)));
  HeciSendLength = sizeof (DATA_CLEAR_LOCK_REQ);
  HeciRecvLength = sizeof (DataBuffer);
  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );
  DCLResp = (DATA_CLEAR_LOCK_RES *) DataBuffer;
  DEBUG ((EFI_D_INFO, "Group    =%08x\n", DCLResp->Header.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", DCLResp->Header.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", DCLResp->Header.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", DCLResp->Header.Fields.Result));

  return DCLResp->Header.Fields.Result;
}


/**
  Send EOP message through HECI1.

  @retval   EFI_SUCCESS         Send EOP message success.
  @retval   Others              Send EOP message failed.

**/
EFI_STATUS
HeciEndOfPost (
  VOID
  )
{
  EFI_STATUS                Status;
  UINT32                    HeciSendLength;
  UINT32                    HeciRecvLength;
  GEN_END_OF_POST           *SendEOP;
  GEN_END_OF_POST_ACK       *EOPResp;
  UINT32                    SeCMode;
  UINT32                    DataBuffer[0x70];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: EndOfPost\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || ((SeCMode != SEC_MODE_NORMAL) && (SeCMode !=SEC_MODE_RECOVER))) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendEOP = (GEN_END_OF_POST *) DataBuffer;
  SendEOP->MKHIHeader.Fields.GroupId = EOP_GROUP_ID;
  SendEOP->MKHIHeader.Fields.Command = EOP_CMD_ID;

  DEBUG ((EFI_D_INFO, "GEN_END_OF_POST size is %x\n", sizeof (GEN_END_OF_POST)));
  HeciSendLength = sizeof (GEN_END_OF_POST);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  EOPResp = (GEN_END_OF_POST_ACK *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", EOPResp->Header.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", EOPResp->Header.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", EOPResp->Header.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", EOPResp->Header.Fields.Result));
  DEBUG ((EFI_D_INFO, "RequestedActions   =%08x\n", EOPResp->Data.RequestedActions));

  return Status;
}


/**
  Send EOS message through HECI1.

  @retval  EFI_SUCCESS         Send EOS message success.
  @retval  Others              Send EOS message failed.

**/
EFI_STATUS
HeciEndOfServices(
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT32                        HeciSendLength;
  UINT32                        HeciRecvLength;
  GEN_END_OF_SERVICES           *SendEOS;
  GEN_END_OF_SERVICES_ACK       *EOSResp;
  UINT32                        SeCMode;
  UINT32                        DataBuffer[0x70];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: EndOfServices\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendEOS = (GEN_END_OF_SERVICES *) DataBuffer;
  SendEOS->MKHIHeader.Fields.GroupId = GROUP_ID_MCA;
  SendEOS->MKHIHeader.Fields.Command = HECI1_EOS;

  DEBUG ((EFI_D_INFO, "GEN_END_OF_SERVICES size is %x\n", sizeof (GEN_END_OF_SERVICES)));
  HeciSendLength = sizeof (GEN_END_OF_SERVICES);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  EOSResp = (GEN_END_OF_SERVICES_ACK *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", EOSResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", EOSResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", EOSResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", EOSResp->MKHIHeader.Fields.Result));

  return Status;
}


/**
  Get IFWI DNX request.

  @param[out] IfwiDnxRequestData       Dnx request data buffer.

  @retval     EFI_SUCCESS              Get IFWI DNX success.
  @retval     Others                   Get IFWI DNX failed.

**/

EFI_STATUS
HeciGetIfwiDnxRequest (
  OUT MBP_IFWI_DNX_REQUEST    *IfwiDnxRequestData
  )
{
  EFI_STATUS                      Status;
  UINT32                           DataBuffer[0x100];
  MBP_CMD_RESP_DATA               *MBPHeader;
  MBP_ITEM_HEADER                 *MBPItem;
  MBP_IFWI_DNX_REQUEST            *IfwiDnxRequest;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetIfwiDnxRequest\n"));
  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  Status = HeciMBP (DataBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MBPHeader = (MBP_CMD_RESP_DATA *) DataBuffer;
  DumpBuffer_HECI (MBPHeader, sizeof (MBP_CMD_RESP_DATA));
  MBPItem = (MBP_ITEM_HEADER *) (MBPHeader + 1);
  DumpBuffer_HECI (MBPItem, MBPHeader->Length);

  while ((UINT32 *) MBPItem < (UINT32 *) DataBuffer + MBPHeader->Length) {
    if (MBPItem->AppID == 7 && MBPItem->ItemID == 1) {
      DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
      IfwiDnxRequest = (MBP_IFWI_DNX_REQUEST *) (MBPItem);
      CopyMem ((VOID *) IfwiDnxRequestData, (VOID *) IfwiDnxRequest, sizeof (MBP_IFWI_DNX_REQUEST));

      return EFI_SUCCESS;
    }
    MBPItem = (MBP_ITEM_HEADER *) ((UINT32 *) MBPItem + MBPItem->Length);
    DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
  }

  return EFI_DEVICE_ERROR;
}


/**
  Get Image FW Versions.

  @param[out] MsgGetFwVersionRespData      Output FW version response data

  @retval     EFI_SUCCESS                  Get version success.
  @retval     Others                       Get version failed.

**/
EFI_STATUS
HeciGetImageFwVerMsg (
  OUT FW_VERSION_CMD_RESP_DATA    *MsgGetFwVersionRespData
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  FW_VER_CMD_REQ                  *MsgGetFwVersion;
  FW_VERSION_CMD_RESP_DATA        *MsgGetFwVersionResp;
  UINT32                          SeCMode;
  UINT32                          DataBuffer[0x20];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetImageFwVerMsg\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  //
  // Allocate MsgGenGetFwVersion data structure
  //
  SetMem (DataBuffer, sizeof (DataBuffer), 0x0);

  MsgGetFwVersion = (FW_VER_CMD_REQ *) DataBuffer;
  MsgGetFwVersion->MKHIHeader.Fields.GroupId = MKHI_GEN_GROUP_ID;
  MsgGetFwVersion->MKHIHeader.Fields.Command = FW_VER_CMD_REQ_ID;
  MsgGetFwVersion->MKHIHeader.Fields.IsResponse = 0;

  DEBUG ((EFI_D_INFO, "FW_VER_CMD_REQ size is 0x%x, FW_VERSION_CMD_RESP_DATA size is: 0x%x\n", sizeof (FW_VER_CMD_REQ), sizeof (FW_VERSION_CMD_RESP_DATA)));
  HeciSendLength = sizeof (FW_VER_CMD_REQ);
  HeciRecvLength = sizeof (DataBuffer);

  //
  // Send Get image Firmware Version Request to CSE
  //
  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  DumpBuffer_HECI (DataBuffer, sizeof (DataBuffer));
  MsgGetFwVersionResp = (FW_VERSION_CMD_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group      =%08x\n", MsgGetFwVersionResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command    =%08x\n", MsgGetFwVersionResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone  =%08x\n", MsgGetFwVersionResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result     =%08x\n", MsgGetFwVersionResp->MKHIHeader.Fields.Result));
  DEBUG ((EFI_D_INFO, "NumModules =%08x\n", MsgGetFwVersionResp->NumModules));

  CopyMem (MsgGetFwVersionRespData, DataBuffer, sizeof (FW_VERSION_CMD_RESP_DATA));

  return Status;
}


/**
  Get NFC Device Type through HECI1.

  @param[out] Mbp_Nfc_Device_Type  Output data buffer for NFC device type.

  @retval     EFI_SUCCESS          Get device success.
  @retval     Others               Get device failed.

**/
EFI_STATUS
HeciGetNfcDeviceType (
  OUT MBP_NFC_DEVICE_TYPE    *Mbp_Nfc_Device_Type
  )
{
  EFI_STATUS                       Status;
  UINT32                           DataBuffer[0x100];
  MBP_CMD_RESP_DATA               *MBPHeader;
  MBP_ITEM_HEADER                 *MBPItem;
  MBP_NFC_DEVICE_TYPE             *Nfc_Device_Type;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetNfcDeviceType\n"));
  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  Status = HeciMBP (DataBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "HeciGetNfcDeviceType HeciMBP: %r\n", Status));
    return Status;
  }

  MBPHeader = (MBP_CMD_RESP_DATA *) DataBuffer;
  DEBUG ((EFI_D_INFO, "HeciGetNfcDeviceType 1\n"));
  DumpBuffer_HECI (MBPHeader, sizeof (MBP_CMD_RESP_DATA));
  MBPItem = (MBP_ITEM_HEADER *) (MBPHeader + 1);
  DEBUG ((EFI_D_INFO, "HeciGetNfcDeviceType 2\n"));
  DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER));

  while ((UINT32 *) MBPItem < (UINT32 *) DataBuffer + MBPHeader->Length) {
    if (MBPItem->AppID == 6 && MBPItem->ItemID == 1) {
      DEBUG ((EFI_D_INFO, "HeciGetNfcDeviceType 3\n"));
      DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
      Nfc_Device_Type = (MBP_NFC_DEVICE_TYPE *) (MBPItem);
      CopyMem ((VOID *) Mbp_Nfc_Device_Type, (VOID *) Nfc_Device_Type, sizeof (MBP_NFC_DEVICE_TYPE));
      DEBUG ((EFI_D_INFO, "Mbp_Nfc_Device_Type= %x\n",Mbp_Nfc_Device_Type->NfcDeviceData));
      return EFI_SUCCESS;
    }
    MBPItem = (MBP_ITEM_HEADER *) ((UINT32 *) MBPItem + MBPItem->Length);
    DEBUG ((EFI_D_INFO, "HeciGetNfcDeviceType 4\n"));
    DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
  }

  return EFI_DEVICE_ERROR;
}


/**
  Get ME FW Capability from MBP.

  @param[out] MBP_ME_FW_CAPS      Output data buffer for ME FW Capability.

  @retval     EFI_SUCCESS         Get ME FW Capability success.
  @retval     Others              Get ME FW Capability failed.

**/

EFI_STATUS
HeciGetMeFwCapability (
  OUT MBP_ME_FW_CAPS    *Mbp_Me_Fw_Caps
  )
{
  EFI_STATUS                       Status;
  UINT32                           DataBuffer[0x100];
  MBP_CMD_RESP_DATA               *MBPHeader;
  MBP_ITEM_HEADER                 *MBPItem;
  MBP_ME_FW_CAPS                  *Me_Fw_Caps;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetMeFwCapability\n"));
  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  Status = HeciMBP (DataBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "HeciGetMeFwCapability HeciMBP: %r\n", Status));
    return Status;
  }

  MBPHeader = (MBP_CMD_RESP_DATA *) DataBuffer;
  DEBUG ((EFI_D_INFO, "HeciGetMeFwCapability 1\n"));
  DumpBuffer_HECI (MBPHeader, sizeof (MBP_CMD_RESP_DATA));
  MBPItem = (MBP_ITEM_HEADER *) (MBPHeader + 1);
  DEBUG ((EFI_D_INFO, "HeciGetMeFwCapability 2\n"));
  DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER));

  while ((UINT32 *) MBPItem < (UINT32 *) DataBuffer + MBPHeader->Length) {
    if (MBPItem->AppID == 1 && MBPItem->ItemID == 2) {
      DEBUG ((EFI_D_INFO, "HeciGetMeFwCapability 3\n"));
      DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
      Me_Fw_Caps = (MBP_ME_FW_CAPS *) (MBPItem);
      CopyMem ((VOID *) Mbp_Me_Fw_Caps, (VOID *) Me_Fw_Caps, sizeof (MBP_ME_FW_CAPS));
      DEBUG ((EFI_D_INFO, "Mbp_Me_Fw_Caps = %x\n", Mbp_Me_Fw_Caps->CurrentFeatures));
      return EFI_SUCCESS;
    }
    MBPItem = (MBP_ITEM_HEADER *) ((UINT32 *) MBPItem + MBPItem->Length);
    DEBUG ((EFI_D_INFO, "HeciGetMeFwCapability 4\n"));
    DumpBuffer_HECI (MBPItem, sizeof (MBP_ITEM_HEADER) + MBPItem->Length);
  }

  return EFI_DEVICE_ERROR;
}



/**
  Send IAFW DNX request set message throught HECI1.

  @param[in] Resp          Buffer to receive the CSE response data..

  @retval    EFI_SUCCESS   Set request success.
  @retval    Others        Set request failed.

**/
EFI_STATUS
HeciIafwDnxReqSet (
  IN OUT IAFW_DNX_REQ_SET_RESP_DATA                *Resp
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  IAFW_DNX_REQ_SET_REQ_DATA       *Req;
  IAFW_DNX_REQ_SET_RESP_DATA      *RespIn;
  UINT32                          SeCMode;
  UINT32                          DataBuffer[0x70];

  DEBUG ((EFI_D_INFO, "BIOS Starts to send HECI Message: HeciIafwDnxReqSet\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || ((SeCMode != SEC_MODE_NORMAL) && (SEC_MODE_RECOVER !=SeCMode))) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  Req = (IAFW_DNX_REQ_SET_REQ_DATA *) DataBuffer;
  Req->MKHIHeader.Fields.GroupId = MKHI_GEN_DNX_GROUP_ID;
  Req->MKHIHeader.Fields.Command = CSE_DNX_REQ_SET;

  DEBUG ((EFI_D_INFO, "IAFW_DNX_REQ_Set_REQ_DATA size is %x\n", sizeof (IAFW_DNX_REQ_CLEAR_REQ_DATA)));
  HeciSendLength = sizeof (IAFW_DNX_REQ_SET_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  RespIn = (IAFW_DNX_REQ_SET_RESP_DATA *) DataBuffer;
  CopyMem ((VOID *) Resp, (VOID *) RespIn, sizeof (IAFW_DNX_REQ_SET_RESP_DATA));

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", Resp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", Resp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", Resp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", Resp->MKHIHeader.Fields.Result));
  DEBUG ((EFI_D_INFO, "Response ReqBiosAction   =%08x\n", Resp->ReqBiosAction));

  return Status;
}


/**
  Send IAFW DNX request clear message throught HECI1.

  @param[in] Flag                Flag to decide which type clear operation need be done.

  @retval    EFI_SUCCESS         Clear request success.
  @retval    Others              Clear request failed.

**/
EFI_STATUS
HeciIafwDnxReqClear (
  IN UINT32         Flag
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  IAFW_DNX_REQ_CLEAR_REQ_DATA     *Req;
  IAFW_DNX_REQ_CLEAR_RESP_DATA    *Resp;
  UINT32                          SeCMode;
  UINT32                          DataBuffer[0x70];

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciIafwDnxReqClear\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || ((SeCMode != SEC_MODE_NORMAL) && (SEC_MODE_RECOVER !=SeCMode))) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  Req = (IAFW_DNX_REQ_CLEAR_REQ_DATA *) DataBuffer;
  Req->MKHIHeader.Fields.GroupId = MKHI_GEN_DNX_GROUP_ID;
  Req->MKHIHeader.Fields.Command = IAFW_DNX_REQ_CLEAR;
  Req->Flag = Flag;

  DEBUG ((EFI_D_INFO, "IAFW_DNX_REQ_CLEAR_REQ_DATA size is %x\n", sizeof (IAFW_DNX_REQ_CLEAR_REQ_DATA)));
  HeciSendLength = sizeof (IAFW_DNX_REQ_CLEAR_REQ_DATA);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x7
             );

  Resp = (IAFW_DNX_REQ_CLEAR_RESP_DATA *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group    =%08x\n", Resp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command  =%08x\n", Resp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone=%08x\n", Resp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result   =%08x\n", Resp->MKHIHeader.Fields.Result));

  return Status;
}


/**
  The Get FW Feature Status message is based on MKHI interface.
  This command is used by BIOS/IntelR MEBX to get firmware runtime status.
  The GET FW RUNTIME STATUS message doesn't need to check the HFS.
  FWInitComplete value before sending the command.
  It means this message can be sent regardless of HFS.FWInitComplete.

  @param[out] RuleData                MEFWCAPS_SKU message

  @retval     EFI_UNSUPPORTED         Current ME mode doesn't support this function.

**/
EFI_STATUS
HeciGetFwFeatureStateMsgII (
  OUT MEFWCAPS_SKU                *RuleData
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RecvLength;
  GEN_GET_FW_FEATURE_STATUS_ACK   GetFwFeatureStatus;
  UINT32                          SeCMode;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetFwFeatureStateMsgII\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  GetFwFeatureStatus.MKHIHeader.Data              = 0;
  GetFwFeatureStatus.MKHIHeader.Fields.GroupId    = MKHI_FWCAPS_GROUP_ID;
  GetFwFeatureStatus.MKHIHeader.Fields.Command    = FWCAPS_GET_RULE_CMD;
  GetFwFeatureStatus.MKHIHeader.Fields.IsResponse = 0;
  GetFwFeatureStatus.RuleId                       = 0x20;
  Length                                          = sizeof (GEN_GET_FW_FEATURE_STATUS);
  RecvLength                                      = sizeof (GEN_GET_FW_FEATURE_STATUS_ACK);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32*) &GetFwFeatureStatus,
             Length,
             &RecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  if (!EFI_ERROR (Status)) {
    RuleData->Data = GetFwFeatureStatus.RuleData.Data;
    DEBUG ((EFI_D_INFO, "BIOS Get Feature Message: %08x\n", RuleData->Data = GetFwFeatureStatus.RuleData.Data));
  } else {
    DEBUG ((EFI_D_ERROR, "BIOS Get Feature Message ERROR: %08x\n", Status));
  }

  return Status;
}

EFI_STATUS
HeciFwFeatureStateOverride (
  IN UINT32                       EnableState,
  IN UINT32                       DisableState
  )
{
  EFI_STATUS                        Status;
  UINT32                            HeciLength;
  UINT32                            HeciRecvLength;
  UINT32                            SeCMode;
  FIRMWARE_CAPABILITY_OVERRIDE      MngStateCmd;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciFwFeatureStateOverride\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  MngStateCmd.MKHIHeader.Data               = 0;
  MngStateCmd.MKHIHeader.Fields.Command     = FIRMWARE_CAPABILITY_OVERRIDE_CMD;
  MngStateCmd.MKHIHeader.Fields.IsResponse  = 0;
  MngStateCmd.MKHIHeader.Fields.GroupId     = MKHI_GEN_GROUP_ID;
  MngStateCmd.MKHIHeader.Fields.Reserved    = 0;
  MngStateCmd.MKHIHeader.Fields.Result      = 0;
  MngStateCmd.FeatureState.EnableFeature    = EnableState;
  MngStateCmd.FeatureState.DisableFeature   = DisableState;
  HeciLength                                = sizeof (FIRMWARE_CAPABILITY_OVERRIDE);
  HeciRecvLength                            = sizeof (FIRMWARE_CAPABILITY_OVERRIDE_ACK);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32*) &MngStateCmd,
             HeciLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  DEBUG ((EFI_D_INFO, "BIOS HeciFwFeatureStateOverride Message: %x\n", Status));

  return Status;
}


/**
  Send IFWI PREPARE FOR UPDATE(IPFU) Command through HECI1.
  This command provide necessary synchronization between HOST & CSE when
  BIOS Performance IFWI Update process.

  @retval  EFI_SUCCESS         Send IFWI Prepare For Update command succeeded
  @retval  Others              Send IFWI Prepare For Update command failed.

**/
EFI_STATUS
HeciIfwiPrepareForUpdate(
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT32                      HeciSendLength;
  UINT32                      HeciRecvLength;
  IFWI_PREPARE_FOR_UPDATE     *SendDC;
  IFWI_PREPARE_FOR_UPDATE_ACK *DCResp;
  UINT32                      SeCMode;
  UINT32                      DataBuffer[0x2];
  UINT8                       Flags;
  UINT8                       Result;
  UINT8                       TimeOut;
  UINT8                       Count;
  BOOLEAN                     Reset_Status;

  DEBUG((EFI_D_INFO, "BIOS Start Send HECI Message: HeciIfwiPrepareForUpdate\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));
  DEBUG ((EFI_D_ERROR, "IFWI Prepare For Update: In Progress...\n"));

  Flags = IPFU_TRY_AGAIN;
  Result  = 0;
  TimeOut = 0;

  //
  // Keep in while loop until Successful or Error or timeout of 1 min duration
  //
  while ((Flags == IPFU_TRY_AGAIN) && (TimeOut <= IPFU_TIMEOUT)) {
    SetMem (DataBuffer, sizeof (DataBuffer), 0);
    SendDC = (IFWI_PREPARE_FOR_UPDATE *) DataBuffer;
    SendDC->MKHIHeader.Fields.GroupId = MKHI_IFWI_UPDATE_GROUP_ID;
    SendDC->MKHIHeader.Fields.Command = IFWI_PREPARE_FOR_UPDATE_CMD_ID;
    SendDC->ResetType = 0x01;

    HeciSendLength = sizeof (IFWI_PREPARE_FOR_UPDATE);
    HeciRecvLength = sizeof (DataBuffer);

    Status = HeciSendwACK (
               HECI1_DEVICE,
               DataBuffer,
               HeciSendLength,
               &HeciRecvLength,
               BIOS_FIXED_HOST_ADDR,
               HECI_CORE_MESSAGE_ADDR
               );
    //
    // When CSE is in reset, Status may fail, so ignore checking of Status
    //
    DCResp = (IFWI_PREPARE_FOR_UPDATE_ACK *) DataBuffer;

    DEBUG ((EFI_D_INFO, "Group            = %08x\n", DCResp->MKHIHeader.Fields.GroupId));
    DEBUG ((EFI_D_INFO, "Command          = %08x\n", DCResp->MKHIHeader.Fields.Command));
    DEBUG ((EFI_D_INFO, "IsRespone        = %08x\n", DCResp->MKHIHeader.Fields.IsResponse));
    DEBUG ((EFI_D_INFO, "Result           = %08x\n", DCResp->MKHIHeader.Fields.Result));
    DEBUG ((EFI_D_INFO, "Flags value = %08x\n", DCResp->Flags));

    Result = (UINT8) DCResp->MKHIHeader.Fields.Result;
    Flags = (UINT8) DCResp->Flags;

    if (Result == EFI_SUCCESS) {
      if (Flags != IPFU_SUCCESS) {
        Count = 0;
        Reset_Status = FALSE;
        while ((Reset_Status == FALSE) && (Count <= 60)) {   // Try checking for 1 min and exit
          Status = CheckCseResetAndIssueHeciReset (&Reset_Status);
          if (Reset_Status == FALSE) {
            MicroSecondDelay (1000 * 1000);
            Count++;
          }
        }
        if (Count > 60){
          DEBUG ((EFI_D_ERROR, "CSE Reset or HECI reset not occurred to send Command again \n"));
          return EFI_TIMEOUT;
        }
        TimeOut += Count;   // Add time delay counted for reset of CSE too.
        MicroSecondDelay (1000 * 1000);
        TimeOut++;
      }  // end if
    } else {
      DEBUG ((EFI_D_ERROR, "IFWI Prepare For Update - Failed! Result: 0x%x \n", Result));
      return Result;
    }  // end if

  }  // End of While Loop

  if (Flags == IPFU_SUCCESS) {
    DEBUG ((EFI_D_INFO, "IFWI Prepare For Update - SUCCESS! Result = 0x%x, Flags = 0x%x \n", Result, Flags));
    return EFI_SUCCESS;
  } else {
    if (TimeOut > IPFU_TIMEOUT) {
      DEBUG ((EFI_D_ERROR, "IFWI Prepare For Update - TIME OUT! \n"));
      return EFI_TIMEOUT;
    }
    DEBUG ((EFI_D_ERROR, "IFWI Prepare For Update - Flag ERROR! Result = 0x%x, Flags = 0x%x \n", Result, Flags));
    return EFI_PROTOCOL_ERROR;
  }  //end if

}


/**
  Send RPMB/Device Extention region Data Clear message through HECI1.
  Note: This command should be send only after successful execution of
  IFWI_PREPARE_FOR_UPDATE Command.

  @retval  EFI_SUCCESS         Send DataClear message success.
  @retval  Others              Send DataClear message failed.

**/
EFI_STATUS
HeciDataClear (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT32                      HeciSendLength;
  UINT32                      HeciRecvLength;
  IFWI_UPDATE_DATA_CLEAR      *SendDC;
  IFWI_UPDATE_DATA_CLEAR_ACK  *DCResp;
  UINT32                      SeCMode;
  UINT32                      DataBuffer[0x4];
  UINT8                       Result = 0xFF; // Status Undefined

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: DataClear\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful \n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendDC = (IFWI_UPDATE_DATA_CLEAR *) DataBuffer;
  SendDC->MKHIHeader.Fields.GroupId = MKHI_IFWI_UPDATE_GROUP_ID;
  SendDC->MKHIHeader.Fields.Command = DATA_CLEAR_CMD_ID;

  DEBUG ((EFI_D_INFO, "IFWI_UPDATE_DATA_CLEAR size is %x\n", sizeof (IFWI_UPDATE_DATA_CLEAR)));
  HeciSendLength = sizeof (IFWI_UPDATE_DATA_CLEAR);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  DCResp = (IFWI_UPDATE_DATA_CLEAR_ACK *) DataBuffer;

  DEBUG ((EFI_D_INFO, "Group            = %08x\n", DCResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command          = %08x\n", DCResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone        = %08x\n", DCResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result           = %08x\n", DCResp->MKHIHeader.Fields.Result));
  Result = (UINT8) DCResp->MKHIHeader.Fields.Result;

  if (Result == 0) {
    DEBUG ((EFI_D_INFO, "Data Clear SUCCESS! \n"));
    Status = EFI_SUCCESS;
  } else {
    DEBUG ((EFI_D_INFO, "Data Clear FAILED! Result = %08x\n", Result));
    Status = Result;
  }

  return Status;
}


/**
  Send 'Update Image Check' command to CSE to verify IFWI Image and confirm
  that appropriate for FW Update. (This command will be sending as part of Capsule Update)

  @param[in] ImageBaseAddr      FW Image Base address. 64 Bit Wide
  @param[in] ImageSize          FW Image Size
  @param[in] *HeciResponse      To capture Response Info (Optional)
  @param[in] ResponseSize       Size of Response (Optional)

  @return    EFI_SUCCESS        Image was checked (best effort) and verified to be appropriate for FW update.
  @return    0x01               IMAGE_FAILED.
  @return    0x02               IMG_SIZE_INVALID.
  @return    0x05               SIZE_ERROR
  @return    0x89               STATUS_NOT_SUPPORTED.
  @return    0x8D               STATUS_INVALID_COMMAND.
  @return    0xFF               STATUS_UNDEFINED.

**/
EFI_STATUS
HeciUpdateImageCheck (
  IN UINT64        ImageBaseAddr,
  IN UINT32        ImageSize,
  IN OUT UINT32    *HeciResponse,
  IN UINT32        ResponseSize
  )
{
  EFI_STATUS                  Status;
  UINT32                      HeciSendLength;
  UINT32                      HeciRecvLength;
  IFWI_UPDATE_IMAGE_CHECK     *SendRequest;
  IFWI_UPDATE_IMAGE_CHECK_ACK *ReceiveResp;
  UINT32                      SeCMode;
  UINT32                      DataBuffer[0x7];
  UINT8                       Result = 0xFF;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciUpdateImageCheck\n"));

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR(Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful \n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendRequest = (IFWI_UPDATE_IMAGE_CHECK *) DataBuffer;
  SendRequest->MKHIHeader.Fields.GroupId = MKHI_IFWI_UPDATE_GROUP_ID;
  SendRequest->MKHIHeader.Fields.Command = UPDATE_IMAGE_CHECK_CMD_ID;
  SendRequest->ImageBaseAddrLower32b = (UINT32) ImageBaseAddr;
  SendRequest->ImageBaseAddrUpper32b = (UINT32) (ImageBaseAddr >> 32);
  SendRequest->ImageSize = ImageSize;

  DEBUG ((EFI_D_INFO, "IFWI_UPDATE_IMAGE_CHECK size is %x\n", sizeof (IFWI_UPDATE_IMAGE_CHECK)));
  HeciSendLength = sizeof (IFWI_UPDATE_IMAGE_CHECK);
  HeciRecvLength = sizeof (DataBuffer);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  ReceiveResp = (IFWI_UPDATE_IMAGE_CHECK_ACK *) DataBuffer;
  DEBUG ((EFI_D_INFO, "Group            = %08x\n", ReceiveResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command          = %08x\n", ReceiveResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone        = %08x\n", ReceiveResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result           = %08x\n", ReceiveResp->MKHIHeader.Fields.Result));

  //
  // Copy Response information if response pointer is available
  //
  if ((HeciResponse != NULL) && (ResponseSize != 0)) {
    if (HeciRecvLength < ResponseSize) {
      ResponseSize = HeciRecvLength;
    }
    CopyMem (HeciResponse, DataBuffer, (UINTN) ResponseSize);
  }

  Result = (UINT8) ReceiveResp->MKHIHeader.Fields.Result;
  if (Result == 0) {
    DEBUG ((EFI_D_INFO, "Update Image Check SUCCESS! \n"));
    Status = EFI_SUCCESS;
  } else {
    DEBUG ((EFI_D_INFO, "Update Image Check FAILED! Result = %08x\n", Result));
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/**
  Checks Platform Trust Technology enablement state.

  @param[out] IsPttEnabledState      TRUE if PTT is enabled, FALSE othewrwise.

  @retval     EFI_SUCCESS            Command succeeded
  @retval     EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
PttHeciGetState (
  OUT BOOLEAN   *IsPttEnabledState
  )
{
  EFI_STATUS      Status;
  MEFWCAPS_SKU    CurrentFeatures;

  *IsPttEnabledState = FALSE;

  Status = HeciGetFwFeatureStateMsgII (&CurrentFeatures);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG ((EFI_D_INFO, "SkuMgr Data = 0x%X\n", CurrentFeatures.Data));

  if (CurrentFeatures.Fields.PTT) {
    *IsPttEnabledState = TRUE;
  }

  DEBUG ((EFI_D_INFO, "PTT SkuMgr: PttState = %d\n", *IsPttEnabledState));

  return EFI_SUCCESS;
}


/**
  Changes current Platform Trust Technology state.

  @param[in] PttEnabledState        TRUE to enable, FALSE to disable.

  @retval    EFI_SUCCESS            Command succeeded
  @retval    EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
PttHeciSetState (
  IN  BOOLEAN   PttEnabledState
  )
{
  EFI_STATUS      Status;
  UINT32          EnableBitmap;
  UINT32          DisableBitmap;

  if (PttEnabledState) {
    //
    // Enable PTT
    //
    DEBUG ((EFI_D_INFO, "PTT SkuMgr: Enable PTT\n"));
    EnableBitmap  = PTT_BITMASK;
    DisableBitmap = CLEAR_PTT_BIT;
  } else {
    //
    // Disable PTT
    //
    DEBUG ((EFI_D_INFO, "PTT SkuMgr: Disable PTT\n"));
    EnableBitmap  = CLEAR_PTT_BIT;
    DisableBitmap = PTT_BITMASK;
  }

  Status = HeciFwFeatureStateOverride (EnableBitmap, DisableBitmap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}


/**
  Send Reset Request Message through HECI.

  @param[in] ResetOrigin             Reset source.
  @param[in] ResetType               Global or Host reset.

  @retval    EFI_UNSUPPORTED         Current Sec mode doesn't support this function.
  @retval    EFI_SUCCESS             Command succeeded.
  @retval    EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally.
  @retval    EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/
EFI_STATUS
HeciSendResetRequest (
  IN  UINT8                             ResetOrigin,
  IN  UINT8                             ResetType
  )
{
  EFI_STATUS                Status;
  UINT32                    HeciLength;
  CBM_RESET_REQ             CbmResetRequest;
  UINT32                    HeciRecvLength;
  UINT32                    SeCMode;

  CbmResetRequest.MKHIHeader.Data               = 0;
  CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
  CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
  CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
  CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
  CbmResetRequest.MKHIHeader.Fields.Result      = 0;
  CbmResetRequest.Data.RequestOrigin            = ResetOrigin;
  CbmResetRequest.Data.ResetType                = ResetType;

  HeciLength = sizeof (CBM_RESET_REQ);
  HeciRecvLength = sizeof (CBM_RESET_REQ);

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  Status = HeciSendwoACK (
             (UINT32 *) &CbmResetRequest,
             HeciLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Send Reset Request - %r\n", Status));
  }

  return Status;
}


/**
  Wait for CSE reset bit set and then Reset HECI interface.

  @param[out] Reset_Status            Reset Status value.

  @retval     EFI_SUCCESS             Command succeeded.
  @retval     EFI_TIMEOUT             HECI does not return the buffer before timeout.

**/

EFI_STATUS
CheckCseResetAndIssueHeciReset (
  OUT BOOLEAN *Reset_Status
  )
{
  EFI_STATUS                             Status;
  volatile HECI_SEC_CONTROL_REGISTER     *SecControlReg;
  UINTN                                  HeciMbar;

  HeciMbar = CheckAndFixHeciForAccess (HECI1_DEVICE);
  if (HeciMbar == 0) {
    return EFI_DEVICE_ERROR;
  }

  *Reset_Status = FALSE;
  SecControlReg = (volatile HECI_SEC_CONTROL_REGISTER  *) (UINTN) (HeciMbar + SEC_CSR_HA);
  if (SecControlReg->r.SEC_RST_HRA == 1) {
    DEBUG ((EFI_D_INFO, "CheckCseResetAndIssueHeciReset, SecControlReg: %08x\n", SecControlReg->ul));
    *Reset_Status = TRUE;
    //
    // Reset HECI Interface
    //
    Status = ResetHeciInterface (HECI1_DEVICE);
    if (EFI_ERROR (Status)) {
      *Reset_Status = FALSE;
    } else {
      DEBUG((EFI_D_INFO, "CheckCseResetAndIssueHeciReset, HECI Reset success\n"));
    }
  }

  DEBUG ((EFI_D_INFO, "CheckCseResetAndIssueHeciReset, SecControlReg: %08x\n", SecControlReg->ul));

  return EFI_SUCCESS;
}


VOID
EFIAPI
HeciCoreBiosDoneMsg (
  void
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Length;
  UINT32                                    RespLength;
  CORE_BIOS_DONE                            CoreBiosDone;
  UINT32                                    SeCMode;
  Length = sizeof (CORE_BIOS_DONE);
  RespLength = sizeof (CORE_BIOS_DONE_ACK);
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return;
  }

  DEBUG ((EFI_D_INFO, "Sending the HeciCoreBiosDoneMsg - Start \n"));
  CoreBiosDone.MkhiHeader.Data              = 0;
  CoreBiosDone.MkhiHeader.Fields.GroupId    = HECI_MKHI_MCA_GROUP_ID;
  CoreBiosDone.MkhiHeader.Fields.Command    = HECI_MCA_CORE_BIOS_DONE_CMD;
  CoreBiosDone.MkhiHeader.Fields.IsResponse = 0;
  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32 *) &CoreBiosDone,
             Length,
             &RespLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  DEBUG ((EFI_D_INFO, "Sending the HeciCoreBiosDoneMsg - done \n"));

  return;
}


VOID
EFIAPI
HeciDXECallback (
  IN EFI_EVENT        Event,
  IN VOID             *ParentImageHandle
  )
{
  HeciDataClearLock ();
  HeciCoreBiosDoneMsg ();
}


/**
  Get SMM Trusted Key.

  @param[out] SmmTrustedKeyData       Smm trusted data buffer.

  @retval     EFI_SUCCESS             Get SMM Trusted success.
  @retval     Others                  Get SMM Trusted failed.

**/

EFI_STATUS
HeciGetSMMTrustedKey (
  OUT MBP_SMM_TRUSTED_KEY    *SmmTrustedKeyData
  )
{
  EFI_STATUS                      Status;
  UINT32                          DataBuffer[0x100];
  MBP_CMD_RESP_DATA               *MBPHeader;
  MBP_ITEM_HEADER                 *MBPItem;
  MBP_SMM_TRUSTED_KEY             *SmmTrustedKey;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetSMMTrustedKey\n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);
  Status = HeciMBP (DataBuffer);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  MBPHeader = (MBP_CMD_RESP_DATA *) DataBuffer;
  MBPItem   = (MBP_ITEM_HEADER *) (MBPHeader + 1);

  while ((UINT32 *) MBPItem < (UINT32 *) DataBuffer + MBPHeader->Length) {
    if (MBPItem->AppID == 8 && MBPItem->ItemID == 2) {
      SmmTrustedKey = (MBP_SMM_TRUSTED_KEY *) (MBPItem + 1);
      CopyMem ((VOID *) SmmTrustedKeyData, (VOID *) SmmTrustedKey, sizeof (MBP_SMM_TRUSTED_KEY));
      return EFI_SUCCESS;
    }
    MBPItem = (MBP_ITEM_HEADER *) ((UINT32 *) MBPItem + MBPItem->Length);
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS
HeciGetArbStatus (
  IN OUT GET_ARB_STATUS_ACK     *GetArbStatus
  )
{
  EFI_STATUS                  Status;
  UINT32                      HeciSendLength;
  UINT32                      HeciRecvLength;
  GET_ARB_STATUS              *SendRequest;
  GET_ARB_STATUS_ACK          *ReceiveResp;
  UINT32                      SeCMode;
  UINT32                      DataBuffer[sizeof (GET_ARB_STATUS_ACK)];
  UINT8                       Result = 0xFF;

  DEBUG ((EFI_D_INFO, "BIOS Start Send HECI Message: HeciGetArbStatus\n"));
  if (GetArbStatus == NULL) {
    DEBUG ((EFI_D_INFO, "Invalid Parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR(Status) || (SeCMode != SEC_MODE_NORMAL)) {
    DEBUG ((EFI_D_INFO, "HeciGetArbStatus-EFI_NOT_READY\n"));
    return EFI_NOT_READY;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful \n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendRequest = (GET_ARB_STATUS *) DataBuffer;
  SendRequest->MKHIHeader.Fields.GroupId = MKHI_SECURE_BOOT_GROUP_ID;
  SendRequest->MKHIHeader.Fields.Command = GET_ARB_STATUS_CMD_ID;

  HeciSendLength = sizeof (GET_ARB_STATUS);
  HeciRecvLength = sizeof (GET_ARB_STATUS_ACK);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  ReceiveResp = (GET_ARB_STATUS_ACK *) DataBuffer;
  DEBUG ((EFI_D_INFO, "Group            = %08x\n", ReceiveResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command          = %08x\n", ReceiveResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone        = %08x\n", ReceiveResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result           = %08x\n", ReceiveResp->MKHIHeader.Fields.Result));

  //
  // Copy Response information
  //
  CopyMem (GetArbStatus, DataBuffer, (UINTN) HeciRecvLength);

  Result = (UINT8) ReceiveResp->MKHIHeader.Fields.Result;
  if (Result == 0) {
    DEBUG ((EFI_D_INFO, "HeciGetArbStatus SUCCESS! \n"));
    Status = EFI_SUCCESS;
  } else {
    DEBUG ((EFI_D_INFO, "HeciGetArbStatus FAILED! Result = %08x\n", Result));
    Status = Result;
  }

  return Status;
}

EFI_STATUS
HeciCommitArbSvnUpdates (
  IN UINT8     *CommitArbSvns
  )
{
  EFI_STATUS                  Status;
  UINT32                      HeciSendLength;
  UINT32                      HeciRecvLength;
  COMMIT_ARB_SVN_UPDATES      *SendRequest;
  COMMIT_ARB_SVN_UPDATES_ACK  *ReceiveResp;
  UINT32                      SeCMode;
  UINT8                       DataBuffer[sizeof (COMMIT_ARB_SVN_UPDATES)];

  DEBUG ((EFI_D_INFO, "\nBIOS Start Send HECI Message: HeciCommitArbSvnUpdates\n"));
  if (CommitArbSvns == NULL) {
    DEBUG ((EFI_D_INFO, "Invalid Parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    if (SeCMode == SEC_OPERATION_MODE_IN_FWUPDATE_PROGRESS){
      return EFI_NOT_READY;
    } else {
      return EFI_UNSUPPORTED;
    }
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful \n"));

  SetMem (DataBuffer, sizeof (DataBuffer), 0);

  SendRequest = (COMMIT_ARB_SVN_UPDATES *) DataBuffer;
  SendRequest->MKHIHeader.Fields.GroupId = MKHI_SECURE_BOOT_GROUP_ID;
  SendRequest->MKHIHeader.Fields.Command = COMMIT_ARB_SVN_UPDATES_CMD_ID;
  CopyMem (SendRequest->CommitSvns, CommitArbSvns, sizeof (SendRequest->CommitSvns));  // 16 Bytes = Size of SVN to commit as per BIOS - CSE Ref doc

  HeciSendLength = sizeof (COMMIT_ARB_SVN_UPDATES);
  HeciRecvLength = sizeof (COMMIT_ARB_SVN_UPDATES_ACK);

  Status = HeciSendwACK (
             HECI1_DEVICE,
             (UINT32 *) DataBuffer,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             HECI_CORE_MESSAGE_ADDR
             );

  ReceiveResp = (COMMIT_ARB_SVN_UPDATES_ACK *) DataBuffer;
  DEBUG ((EFI_D_INFO, "Group            = %08x\n", ReceiveResp->MKHIHeader.Fields.GroupId));
  DEBUG ((EFI_D_INFO, "Command          = %08x\n", ReceiveResp->MKHIHeader.Fields.Command));
  DEBUG ((EFI_D_INFO, "IsRespone        = %08x\n", ReceiveResp->MKHIHeader.Fields.IsResponse));
  DEBUG ((EFI_D_INFO, "Result           = %08x\n", ReceiveResp->MKHIHeader.Fields.Result));

  Status =(UINT8) ReceiveResp->MKHIHeader.Fields.Result;;
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_INFO, "HeciCommitArbSvnUpdates FAILED! Result = %08x\n", Status));
  } else {
    DEBUG ((EFI_D_INFO, "HeciCommitArbSvnUpdates SUCCESS -- Result = %08x\n", Status));
  }

  return Status;
}


/**
  Write Data to NVM file for ISH through HECI1.

  @param[in] FileName      The file name.
  @param[in] Offset        The offset of data.
  @param[in] Data          The data content.
  @param[in] DataSize      Data's size.

  @retval    EFI_SUCCESS   Write NVM file success.
  @retval    Others        Write NVM file failed.

**/
EFI_STATUS
HeciWriteIshNVMFile (
  IN  ISH_SRV_HECI_SET_FILE_REQUEST *Ish2CseData
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength = 0;
  ISH_SRV_HECI_STATUS_REPLY       ReceiveData;
  UINT32                          SeCMode;
  UINT32                          Index = 0;
  UINT32                          *TempPtr = NULL;

  DEBUG ((EFI_D_INFO, "HeciWriteIshNVMFile Entry\n"));
  Status = HeciGetSeCMode (HECI1_DEVICE, &SeCMode);
  if (EFI_ERROR (Status) || (SeCMode != SEC_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "GetSeCMode successful\n"));

  if (Ish2CseData == NULL) {
    DEBUG ((EFI_D_ERROR, "Invalid Parameter\n"));
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "Dump Bios2ish buffer\n"));
  TempPtr = (UINT32 *) Ish2CseData;
  for (Index = 0; Index < 10; Index++ ){
    DEBUG((EFI_D_INFO, "%x\n", *TempPtr++));
  }

  HeciSendLength = Ish2CseData->Header.Length + 4; // Including command and msglength

  Status = HeciIshSendwAck (
             HECI1_DEVICE,
             (VOID *) Ish2CseData,
             (VOID *) &ReceiveData,
             HeciSendLength,
             &HeciRecvLength,
             BIOS_FIXED_HOST_ADDR,
             0x03
             );

  DEBUG ((EFI_D_INFO, "Command    =%08x\n", ReceiveData.Header.Command));
  DEBUG ((EFI_D_INFO, "Length    =%08x\n", ReceiveData.Header.Length));
  DEBUG ((EFI_D_INFO, "Status    =%08x\n", ReceiveData.Status));

  if (ReceiveData.Status != 0) {
    DEBUG ((EFI_D_ERROR, "HeciWriteIshNVMFile Failed.\n"));
    Status = EFI_DEVICE_ERROR;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

