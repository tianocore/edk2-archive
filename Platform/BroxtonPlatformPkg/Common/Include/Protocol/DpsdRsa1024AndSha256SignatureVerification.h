/** @file
  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __EFI_DPSD_RSA1024_AND_SHA256_SIGNATURE_VERIFICATION_PROTOCOL_H__
#define __EFI_DPSD_RSA1024_AND_SHA256_SIGNATURE_VERIFICATION_PROTOCOL_H__


#define EFI_DPSD_RSA1024_AND_SHA256_SIGNATURE_VERIFICATION_PROTOCOL_GUID \
 {0x45b6f49, 0x9102, 0x4590, 0xa0, 0xa5, 0x35, 0x31, 0x1b, 0xa, 0xef, 0xc3}

//
// Prototypes for the Amt BIOS Extensions Loader Driver Protocol
//

typedef
EFI_STATUS
(EFIAPI *EFI_VERIFY_SIGNATURE) (
  IN UINT8  *PublicKeyBuffer,
  IN UINT32 PublicKeyBufferSize,
  IN UINT8  *PublicKeySignatureBuffer
  );

typedef struct _EFI_DPSD_RSA1024_AND_SHA256_SIGNATURE_VERIFICATION_PROTOCOL {
  EFI_VERIFY_SIGNATURE                                VerifySignature;
} EFI_DPSD_RSA1024_AND_SHA256_SIGNATURE_VERIFICATION_PROTOCOL;

extern EFI_GUID gEfiDpsdRSA1024AndSHA256SignatureVerificationProtocolGuid;

#endif

