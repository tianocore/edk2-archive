/** @file
  Ihis protocol is defined to abstract TPM2 platform behavior.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __TREE_PLATFORM_H__
#define __TREE_PLATFORM_H__

#include <IndustryStandard/Tpm20.h>
#include <Protocol/TrEEProtocol.h>

typedef struct _EFI_TREE_PLATFORM_PROTOCOL EFI_TREE_PLATFORM_PROTOCOL;

/**
  This service returns the platform auth value.

  @param[in]  This                     Indicates the calling context
  @param[out] AuthSize                 Tpm2 Auth size

  @retval     EFI_SUCCESS              Auth size returned.
  @retval     EFI_SECURITY_VIOLATION   Can not return platform auth due to security reason.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_PLATFORM_GET_AUTH_SIZE) (
  IN EFI_TREE_PLATFORM_PROTOCOL *This,
  OUT UINT16                    *AuthSize
  );

/**
  This service returns the Tpm2 auth value.

  Platform need figour out a way to decide if it is proper to publish Tpm2 auth value.

  @param[in]  This                     Indicates the calling context
  @param[in]  AuthHandle               TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}
  @param[out] AuthValue                Tpm2 Auth value

  @retval     EFI_SUCCESS              Tpm2 auth value returned.
  @retval     EFI_SECURITY_VIOLATION   Can not return Tpm2 auth value due to security reason.
  @retval     EFI_UNSUPPORTED          Unsupported AuthHandle.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_PLATFORM_GET_AUTH_VALUE) (
  IN EFI_TREE_PLATFORM_PROTOCOL *This,
  IN TPMI_RH_HIERARCHY_AUTH     AuthHandle,
  OUT TPM2B_AUTH                *AuthValue
  );

/**
  This service sets the Tpm2 auth value.

  Platform need figour out a way to decide if it is proper to set Tpm2 auth value.

  @param[in]  This                     Indicates the calling context
  @param[in]  AuthHandle               TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}
  @param[in]  AuthValue                Tpm2 Auth value

  @retval     EFI_SUCCESS              Tpm2 auth value set.
  @retval     EFI_SECURITY_VIOLATION   Can not set Tpm2 auth value due to security reason.
  @retval     EFI_UNSUPPORTED          Unsupported AuthHandle.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_PLATFORM_SET_AUTH_VALUE) (
  IN EFI_TREE_PLATFORM_PROTOCOL *This,
  IN TPMI_RH_HIERARCHY_AUTH     AuthHandle,
  IN TPM2B_AUTH                 *AuthValue
  );

struct _EFI_TREE_PLATFORM_PROTOCOL {
  EFI_TREE_PLATFORM_GET_AUTH_SIZE         GetAuthSize;
  EFI_TREE_PLATFORM_GET_AUTH_VALUE        GetAuthValue;
  EFI_TREE_PLATFORM_SET_AUTH_VALUE        SetAuthValue;
};

extern EFI_GUID gEfiTrEEPlatformProtocolGuid;

#endif // __TREE_PLATFORM_H__

