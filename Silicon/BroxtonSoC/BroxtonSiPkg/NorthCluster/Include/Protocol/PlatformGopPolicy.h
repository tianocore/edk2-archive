/** @file
  Header file for Platform GOP Policy.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PLATFORM_GOP_POLICY_PROTOCOL_H_
#define _PLATFORM_GOP_POLICY_PROTOCOL_H_

#define EFI_PLATFORM_GOP_POLICY_PROTOCOL_GUID \
  { 0xec2e931b, 0x3281, 0x48a5, 0x81, 0x7, 0xdf, 0x8a, 0x8b, 0xed, 0x3c, 0x5d }

#define EFI_BMP_IMAGE_GUID \
  { 0x878AC2CC, 0x5343, 0x46F2, 0xB5, 0x63, 0x51, 0xF8, 0x9D, 0xAF, 0x56, 0xBA }

#define PLATFORM_GOP_POLICY_PROTOCOL_REVISION_01 0x01
#define PLATFORM_GOP_POLICY_PROTOCOL_REVISION_02 x0222

#pragma pack(1)

typedef enum {
  LidClosed,
  LidOpen,
  LidStatusMax
} LID_STATUS;

typedef enum {
  Docked,
  UnDocked,
  DockStatusMax
} DOCK_STATUS;

/**
  Get platform LID status.

  @param[out]  CurrentLidStatus  Current Lid status.

  @retval      EFI_STATUS

**/
typedef
EFI_STATUS
(EFIAPI *GET_PLATFORM_LID_STATUS) (
   OUT LID_STATUS *CurrentLidStatus
);

/**
  Get VBT data.

  @param[out]  VbtAddress      Address of VBT.
  @param[out]  VbtSize         Size of VBT.

  @retval      EFI_STATUS

**/
typedef
EFI_STATUS
(EFIAPI *GET_VBT_DATA) (
   OUT EFI_PHYSICAL_ADDRESS *VbtAddress,
   OUT UINT32 *VbtSize
);

#pragma pack()

//
// Platform GOP policy Protocol
//
typedef struct _PLATFORM_GOP_POLICY_PROTOCOL {
  UINT32                             Revision;
  GET_PLATFORM_LID_STATUS            GetPlatformLidStatus;
  GET_VBT_DATA                       GetVbtData;
} PLATFORM_GOP_POLICY_PROTOCOL;

//
// VBT Information
//
typedef struct {
  EFI_PHYSICAL_ADDRESS    VbtAddress;
  UINT32                  VbtSize;
} VBT_INFO;

//
// Extern the GUID for protocol users.
//
extern EFI_GUID  gPlatformGOPPolicyGuid;
extern EFI_GUID  gVbtInfoGuid;

#endif

