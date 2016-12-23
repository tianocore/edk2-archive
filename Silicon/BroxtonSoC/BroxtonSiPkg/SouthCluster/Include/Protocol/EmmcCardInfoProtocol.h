/** @file
  Interface definition for EFI_EMMC_CARD_INFO_PROTOCOL.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EMMC_CARD_INFO_H_
#define _EMMC_CARD_INFO_H_

#define EFI_EMMC_CARD_INFO_PROTOCOL_GUID \
  { \
    0x1ebe5ab9, 0x2129, 0x49e7, {0x84, 0xd7, 0xee, 0xb9, 0xfc, 0xe5, 0xde, 0xdd } \
  }

typedef struct _EFI_EMMC_CARD_INFO_PROTOCOL  EFI_EMMC_CARD_INFO_PROTOCOL;


/**
  This is Protocol is used to get CARD_DATA pointer.

**/
struct _EFI_EMMC_CARD_INFO_PROTOCOL{
  CARD_DATA  *CardData;
};

extern EFI_GUID gEfiEmmcCardInfoProtocolGuid;

#endif

