/** @file
  Implementation file for PEI specific HECI Message functionality.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/HeciMsgLib.h>

/**
  Read NVM file data through HECI2.

  @param[in]     FileName          The file name.
  @param[in]     Offset            The offset of data.
  @param[out]    Data              The data buffer.
  @param[in,out] DataSize          Data's size.

  @retval        EFI_SUCCESS       Read NVM file success.
  @retval        Others            Read NVM file failed.

**/
EFI_STATUS
Heci2ReadNVMFile (
  IN      UINT8               *FileName,
  IN      UINT32              Offset,
  OUT     UINT8               *Data,
  IN OUT  UINTN               *DataSize,
  IN      EFI_HECI_PROTOCOL   *Heci2Protocol
  )
{
  Data = NULL;

  return EFI_NOT_AVAILABLE_YET;
}


/**
  Write data to NVM file through HECI2.

  @param[in] FileName      The file name.
  @param[in] Offset        The offset of data.
  @param[in] Data          The data content.
  @param[in] DataSize      Data's size.
  @param[in] Truncate      Truncate the file.

  @retval    EFI_SUCCESS   Write NVM file success.
  @retval    Others        Write NVM file failed.

**/
EFI_STATUS
Heci2WriteNVMFile (
  IN UINT8   *FileName,
  IN UINT32  Offset,
  IN UINT8   *Data,
  IN UINTN   DataSize,
  IN BOOLEAN Truncate
  )
{
  return EFI_NOT_AVAILABLE_YET;
}


/**
  Get NVM file's size through HECI2.

  @param[in]  FileName       The file name.
  @param[out] FileSize       The file's size.

  @retval     EFI_SUCCESS    Get NVM file size success.
  @retval     Others         Get NVM file size failed.

**/
EFI_STATUS
Heci2GetNVMFileSize (
  IN  UINT8              *FileName,
  OUT UINTN              *FileSize,
  IN EFI_HECI_PROTOCOL   *Heci2Protocol
  )
{
  *FileSize = 0;

  return EFI_NOT_AVAILABLE_YET;
}


/**
  Send Get Proxy State message through Heci2.

  @retval  EFI_SUCCESS         Send message success.
  @retval  Others              Send message failed.

**/
EFI_STATUS
Heci2GetProxyState (
  VOID
  )
{
  return EFI_NOT_AVAILABLE_YET;
}


/**
  Check is the HECI2 device has interrupt.

  @retval  TRUE       HECI2 device interrupt.
  @retval  FALSE      No interrupt.

**/
BOOLEAN
Heci2GetInterrupt (
  VOID
  )
{
  return FALSE;
}


/**
  Lock Directory message through HECI2.

  @param[in] DirName          The Directory name.
  @param[in] Heci2Protocol    The HECI protocol to send the message to HECI2 device.

  @retval    EFI_SUCCESS      Send EOP message success.
  @retval    Others           Send EOP message failed.

**/
EFI_STATUS
Heci2LockDirectory (
  IN UINT8               *DirName,
  IN EFI_HECI_PROTOCOL   *Heci2Protocol
  )
{
  return EFI_NOT_AVAILABLE_YET;
}


/**
  Send Get proxy State message through HEC2 but not waiting for response.

  @param[in] Heci2Protocol    The HECI protocol to send the message to HECI2 device.

  @retval    EFI_SUCCESS      Send get proxy state message success.
  @retval    Others           Send get proxy state message failed.

**/
EFI_STATUS
Heci2GetProxyStateNoResp (
  IN EFI_HECI_PROTOCOL         *Heci2Protocol
  )
{
  return EFI_NOT_AVAILABLE_YET;
}


/**
  Returns whether trusted channel is enabled.

  @param[in]  None.

  @retval     TRUE if trusted channel is enabled.
              FALSE if trusted channel is disabled.

**/
BOOLEAN
IsTrustedChannelEnabled (
  VOID
  )
{
  return FALSE;
}


/**
  Updates the SHA256 signature and monotonic counter fields of a HECI message header.

  @param[in]  MessageHeader   A pointer to the message header
  @param[in]  TotalHeaderSize The total header size
  @param[in]  TotalDataSize   The total data size

  @retval     Whether the header could be updated

**/
EFI_STATUS
EFIAPI
UpdateTrustedHeader (
  IN OUT  UINT8       *MessageHeader,
  IN      UINT32      TotalHeaderSize,
  IN      UINT32      TotalDataSize
  )
{
  return EFI_NOT_AVAILABLE_YET;
}

