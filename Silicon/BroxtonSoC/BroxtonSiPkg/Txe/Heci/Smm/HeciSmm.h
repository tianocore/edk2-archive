/** @file
  Definitions for HECI SMM driver.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_SMM_H_
#define _HECI_SMM_H_

#define SMM_HECI_FUNCTION_SEND_MESSAGE_WITH_ACK    1
#define SMM_HECI_FUNCTION_READ_MESSAGE             2
#define SMM_HECI_FUNCTION_SEND_MESSAGE             3
#define SMM_HECI_FUNCTION_GET_STATUS               8
#define SMM_HECI_TRUSTED_CHANNEL_ERROR             5

#define SMM_HECI_MESSAGE_END_OF_POST               0x100
#define SMM_HECI_MESSAGE_END_OF_SERVICES           0x101

#define PCI_CFG_GS3                 0x74
#define PCI_CFG_GS3_PANIC_SUPPORTED 0x00080000
#define PCI_CFG_GS3_PANIC           0x00000008

typedef struct {
  UINTN       Function;
  EFI_STATUS  ReturnStatus;
  UINT8       Data[1];
} SMM_HECI_COMMUNICATE_HEADER;

typedef struct {
  UINT32      Length;
  UINT32      HostAddress;
  UINT32      CSEAddress;
  UINT8       MessageData[1];
} SMM_HECI_SEND_MESSAGE_BODY;

typedef struct {
  UINT32      Length;
  UINT8       MessageData[1];
} SMM_HECI_READ_MESSAGE_BODY;

typedef struct {
  UINT32      Length;
  UINT32      RecLength;
  UINT32      HostAddress;
  UINT32      CSEAddress;
  UINT8       MessageData[1];
} SMM_HECI_SEND_MESSAGE_W_ACK_BODY;

#define MAX_HECI_BUFFER_SIZE   0x10000

#endif

