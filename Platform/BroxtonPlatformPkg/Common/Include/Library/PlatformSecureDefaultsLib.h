/** @file
  IPC based PlatformFvbLib library instance.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PLATFORM_SECURE_DEFAULTS_LIB_H__
#define __PLATFORM_SECURE_DEFAULTS_LIB_H__

#include "Uefi.h"
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/ImageAuthentication.h>
#include <Library/PcdLib.h>
#include <Protocol/FirmwareVolume2.h>

//
// Cryptograhpic Key Information
//
#pragma pack (push, 1)
typedef struct _CPL_KEY_INFO{
  UINT32                            KeyLengthInBits;    // Key Length In Bits
  UINT32                            BlockSize;          // Operation Block Size in Bytes
  UINT32                            CipherBlockSize;    // Output Cipher Block Size in Bytes
  UINT32                            KeyType;            // Key Type
  UINT32                            CipherMode;         // Cipher Mode for Symmetric Algorithm
  UINT32                            Flags;              // Additional Key Property Flags
} CPL_KEY_INFO;
#pragma pack (pop)


/**
  Converts a nonnegative integer to an octet string of a specified length.

  @param[in]  Integer                 Pointer to the nonnegative integer to be converted
  @param[in]  IntSizeInWords          Length of integer buffer in words
  @param[out] OctetString             Converted octet string of the specified length
  @param[in]  OSSizeInBytes           Intended length of resulting octet string in bytes

  @retval     EFI_SUCCESS             Data conversion successfully
  @retval     EFI_BUFFER_TOOL_SMALL   Buffer is too small for output string

**/
EFI_STATUS
EFIAPI
Int2OctStr (
  IN     CONST UINTN                *Integer,
  IN     UINTN                      IntSizeInWords,
  OUT    UINT8                      *OctetString,
  IN     UINTN                      OSSizeInBytes
  );

/**
  Form the PK signature list from the publick key storing file (*.pbk).

  @param[in]   PkKeyPbkData          DataBuffer from the .pbk file.
  @param[out]  PkCert                Point to the data buffer to store the signature list.

  @retval EFI_UNSUPPORTED            Unsupported Key Length.
  @retval EFI_OUT_OF_RESOURCES       There are not enough memory resourse to from the signature list.

**/
EFI_STATUS
CreatePkCertificateList (
  IN  UINT8                       *PkKeyPbkData,
  OUT EFI_SIGNATURE_LIST          **PkCert
  );

/**
  Enroll new PKpub into the System. If there is already a exisitng PKpub in NVRAM,
  the new PKpub should be signed with old PKpriv before passing into SetVariable.
  If there is no PKpub existing, which means current system is in SETUP mode.
  Platform firmware will not require authentication in order to modify the
  Platform Key

  Note:
  For PKpub entry in NVRAM, the format of PKpub should be as EFI_SIGNATURE_LIST.
  The SignatureType will be WIN_CERT_UEFI_RSA2048_GUID. And only 1 PKpub is
  allowed in this entry.
  The SignatureOwner GUID will be the same with PK's vendorguid.

  @retval   EFI_SUCCESS            New PK enrolled successful.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollPlatformKey (
  IN  VOID                     *Buf,
  IN  UINTN                    BufSize
  );

/**
  Function to Load Secure Keys given the binary GUID

  @param[in]      VendorGuid       GUID of the Variable.
  @param[in]      VariableName     Name of the Variable.
  @param[in]      VendorGuid       GUID of the Variable.

  @retval         EFI_SUCCESS      Set the variable successfully.
  @retval         Others           Set variable failed.

**/
EFI_STATUS
SetSecureVariabeKeys (
  IN  EFI_GUID                  *ImageGuid,
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  );

/**
  Internal function to Update User Mode to Setup Mode given its name and GUID, no authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval         EFI_SUCCESS             Updated to Setup Mode successfully.
  @retval         Others                  The driver failed to start the device.

**/
EFI_STATUS
UpdateSetupModetoUserMode (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  );

/**
  Enrolls PK, KEK, Db and Dbx.

 **/
VOID
EnrollKeys();

/**
  Internal function to delete a Variable given its name and GUID, no authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval         EFI_SUCCESS             Variable deleted successfully.
  @retval         Others                  The driver failed to start the device.

**/
EFI_STATUS
DeleteVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  );

/**
  Internal function to Update User Mode to Setup Mode given its name and GUID, no authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval         EFI_SUCCESS             Updated to Setup Mode successfully.
  @retval         Others                  The driver failed to start the device.

**/
EFI_STATUS
UpdateUserModetoSetupMode (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  );

/**
  Deletes PK, KEK, Db and Dbx.

 **/
VOID
DeleteKeys();

/**
 Enable Custom Mode.

 **/
 VOID
 EnableCustomMode();

#endif

