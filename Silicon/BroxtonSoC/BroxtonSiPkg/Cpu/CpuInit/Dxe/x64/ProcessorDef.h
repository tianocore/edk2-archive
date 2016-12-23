/** @file
  Definition for EM64T processor.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PROCESSOR_DEF_H
#define _PROCESSOR_DEF_H

#pragma pack(1)

typedef struct {
  UINT16  Offset15To0;
  UINT16  SegmentSelector;
  UINT16  Attributes;
  UINT16  Offset31To16;
  UINT32  Offset63To32;
  UINT32  Reserved;
} INTERRUPT_GATE_DESCRIPTOR;

#pragma pack()

typedef struct {
  UINT8  *RendezvousFunnelAddress;
  UINTN  PModeEntryOffset;
  UINTN  FlatJumpOffset;
  UINTN  LModeEntryOffset;
  UINTN  LongJumpOffset;
  UINTN  Size;
} MP_ASSEMBLY_ADDRESS_MAP;

/**
  Get address map of RendezvousFunnelProc.

  @param[out]  AddressMap       Output buffer for address map information

**/
VOID
AsmGetAddressMap (
  OUT MP_ASSEMBLY_ADDRESS_MAP    *AddressMap
  );

#endif

