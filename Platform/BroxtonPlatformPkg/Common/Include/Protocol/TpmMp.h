/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_TPM_MP_DRIVER_PROTOCOL_H__
#define __EFI_TPM_MP_DRIVER_PROTOCOL_H__

#define EFI_TPM_MP_DRIVER_PROTOCOL_GUID \
  { 0xde161cfe, 0x1e60, 0x42a1, 0x8c, 0xc3, 0xee, 0x7e, 0xf0, 0x73, 0x52, 0x12 }

EFI_FORWARD_DECLARATION (EFI_TPM_MP_DRIVER_PROTOCOL);

#define TPM_DRIVER_STATUS         0
#define TPM_DEVICE_STATUS         1

#define TPM_DRIVER_OK             0
#define TPM_DRIVER_FAILED         1
#define TPM_DRIVER_NOT_OPENED     2
#define TPM_DEVICE_OK             0
#define TPM_DEVICE_UNRECOVERABLE  1
#define TPM_DEVICE_RECOVERABLE    2
#define TPM_DEVICE_NOT_FOUND      3

//
// Prototypes for the TPM MP Driver Protocol
//

typedef
EFI_STATUS
(EFIAPI *EFI_TPM_MP_INIT) (
  IN EFI_TPM_MP_DRIVER_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TPM_MP_CLOSE) (
  IN EFI_TPM_MP_DRIVER_PROTOCOL   *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TPM_MP_GET_STATUS_INFO) (
  IN EFI_TPM_MP_DRIVER_PROTOCOL   *This,
  IN UINT32                        ReqStatusType,
  OUT UINT32                      *Status
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TPM_MP_TRANSMIT) (
  IN EFI_TPM_MP_DRIVER_PROTOCOL   *This,
  IN UINT8                        *TransmitBuffer,
  IN UINT32                        TransmitBufferLen,
  OUT UINT8                        *ReceiveBuf,
  IN OUT UINT32                    *ReceiveBufLen
  );

typedef struct _EFI_TPM_MP_DRIVER_PROTOCOL {
  EFI_TPM_MP_INIT                    Init;
  EFI_TPM_MP_CLOSE                  Close;
  EFI_TPM_MP_GET_STATUS_INFO         GetStatusInfo;
  EFI_TPM_MP_TRANSMIT                Transmit;
} EFI_TPM_MP_DRIVER_PROTOCOL;

extern EFI_GUID gEfiTpmMpDriverProtocolGuid;

#endif

