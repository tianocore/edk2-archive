/** @file
  Io Apic policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IOAPIC_CONFIG_H_
#define _IOAPIC_CONFIG_H_

#define IOAPIC_CONFIG_REVISION 1

extern EFI_GUID gIoApicConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to IO Apic.

**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;                       ///< Config Block Header
  UINT32               BdfValid            :  1;     ///< Set to 1 if BDF value is valid, SC code will not program these fields if this bit is not TRUE. <b>0: Disable</b>; 1: Enable.
  UINT32               RsvdBits0           :  7;     ///< Reserved bits
  UINT32               BusNumber           :  8;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0xFA</b>.
  UINT32               DeviceNumber        :  5;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0x0F</b>.
  UINT32               FunctionNumber      :  3;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0x00</b>.
  UINT32               IoApicEntry24_119   :  1;     ///< 0: Disable; <b>1: Enable</b> IOAPIC Entry 24-119
  UINT32               RsvdBits1           :  7;     ///< Reserved bits
  UINT8                IoApicId;                     ///< This member determines IOAPIC ID.
  UINT8                ApicRangeSelect;              ///< Define address bits 19:12 for the IOxAPIC range. Default is <b>0</b>
  UINT8                Rsvd0[2];                     ///< Reserved bytes
} SC_IOAPIC_CONFIG;

#pragma pack (pop)

#endif // _IOAPIC_CONFIG_H_

