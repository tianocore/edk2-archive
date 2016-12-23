/** @file
  Interface definition details for SEC and UMA.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_UMA_PPI_H_
#define _SEC_UMA_PPI_H_

#define SEC_UMA_PPI_GUID \
  { \
    0xcbd86677, 0x362f, 0x4c04, 0x94, 0x59, 0xa7, 0x41, 0x32, 0x6e, 0x05, 0xcf \
  }

#define CSE_EMMC_SELECT_PPI_GUID \
  { \
    0x1e30e33d, 0x1854, 0x437a, 0xbd, 0x68, 0xfc, 0x15, 0x53, 0xaa, 0x8b, 0xe4 \
  }

#define CSE_UFS_SELECT_PPI_GUID  \
  { \
    0xc5a6189e, 0x8c33, 0x4ac6, 0xae, 0x9a, 0xae, 0xd1, 0x8c, 0xab, 0xe2, 0x6d \
  }

#define CSE_SPI_SELECT_PPI_GUID  \
  { \
    0xd35eda81, 0x07d0, 0x4142, 0x94, 0x9,  0xb0, 0x72, 0x33, 0xed, 0x2d, 0x7 \
  }

extern EFI_GUID  gSeCUmaPpiGuid;
extern EFI_GUID  gCseEmmcSelectPpiGuid;
extern EFI_GUID  gCseUfsSelectPpiGuid;
extern EFI_GUID  gCseSpiSelectPpiGuid;

//
// Revision
//
#define SEC_UMA_PPI_REVISION 1

//
// define the MRC recommended boot modes.
//
typedef enum {
  s3Boot, // In current implementation, bmS3 == bmWarm
  warmBoot,
  coldBoot,
  fastBoot,
} MRC_BOOT_MODE_T;

typedef
EFI_STATUS
(EFIAPI *SEC_SEND_UMA_SIZE) (
  IN EFI_PEI_SERVICES **PeiServices
  );

typedef
EFI_STATUS
(EFIAPI *SEC_CONFIG_DID_REG) (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  MRC_BOOT_MODE_T           MrcBootMode,
  UINT8                     InitStat,
  UINT32                    SeCUmaBase,
  UINT32                    *SeCUmaSize
  );

typedef
EFI_STATUS
(EFIAPI *SEC_TAKE_OWNER_SHIP) (
  );

//
// Interface definition details for SEC and UMA
//
typedef struct SEC_UMA_PPI {
  SEC_SEND_UMA_SIZE      SeCSendUmaSize;
  SEC_CONFIG_DID_REG     SeCConfigDidReg;
  SEC_TAKE_OWNER_SHIP    SeCTakeOwnerShip;
} SEC_UMA_PPI;

#endif

