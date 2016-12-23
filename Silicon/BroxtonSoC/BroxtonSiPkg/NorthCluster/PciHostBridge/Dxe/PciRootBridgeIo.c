/** @file
  EFI Memory Controller PCI Root Bridge Io Protocol.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciRootBridge.h"
#include <IndustryStandard/Pci22.h>

typedef struct {
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR SpaceDesp[TypeMax];
  EFI_ACPI_END_TAG_DESCRIPTOR       EndDesp;
} RESOURCE_CONFIGURATION;

RESOURCE_CONFIGURATION  Configuration = {
  {
    {
      0x8A,
      0x2B,
      1,
      0,
      0,
      0,
      0,
      0,
      0,
      0
    },
    {
      0x8A,
      0x2B,
      0,
      0,
      0,
      32,
      0,
      0,
      0,
      0
    },
    {
      0x8A,
      0x2B,
      0,
      0,
      6,
      32,
      0,
      0,
      0,
      0
    },
    {
      0x8A,
      0x2B,
      0,
      0,
      0,
      64,
      0,
      0,
      0,
      0
    },
    {
      0x8A,
      0x2B,
      0,
      0,
      6,
      64,
      0,
      0,
      0,
      0
    },
    {
      0x8A,
      0x2B,
      2,
      0,
      0,
      0,
      0,
      0,
      0,
      0
    }
  },
  {
    0x79,
    0
  }
};

//
// Protocol Member Function Prototypes
//
EFI_STATUS
EFIAPI
RootBridgeIoPollMem (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );

EFI_STATUS
EFIAPI
RootBridgeIoPollIo (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );

EFI_STATUS
EFIAPI
RootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoIoWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoCopyMem (
  IN     struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   DestAddress,
  IN     UINT64                                   SrcAddress,
  IN     UINTN                                    Count
  );

EFI_STATUS
EFIAPI
RootBridgeIoPciRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoPciWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
RootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

EFI_STATUS
EFIAPI
RootBridgeIoUnmap (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  VOID                             *Mapping
  );

EFI_STATUS
EFIAPI
RootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  );

EFI_STATUS
EFIAPI
RootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  );

EFI_STATUS
EFIAPI
RootBridgeIoFlush (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
RootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  );

EFI_STATUS
EFIAPI
RootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength
  );

EFI_STATUS
EFIAPI
RootBridgeIoConfiguration (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT    VOID                             **Resources
  );

//
// Sub Function Prototypes
//
typedef union {
  UINT8 volatile  *buf;
  UINT8 volatile  *ui8;
  UINT16 volatile *ui16;
  UINT32 volatile *ui32;
  UINT64 volatile *ui64;
  UINTN volatile  ui;
} PTR;

STATIC
EFI_STATUS
RootBridgeIoPciRW (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     BOOLEAN                                Write,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

//
// Memory Controller Pci Root Bridge Io Module Variables
//
EFI_METRONOME_ARCH_PROTOCOL *mMetronome;
EFI_CPU_IO2_PROTOCOL        *mCpuIo;

/**
  Construct the Pci Root Bridge Io protocol.

  @param[in]  Protocol           Point to protocol instance.
  @param[in]  HostBridgeHandle   Handle of host bridge.
  @param[in]  Attri              Attribute of host bridge.
  @param[in]  ResAperture        ResourceAperture for host bridge.

  @retval     EFI_SUCCESS        Success to initialize the Pci Root Bridge.

**/
EFI_STATUS
RootBridgeConstructor (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    *Protocol,
  IN EFI_HANDLE                         HostBridgeHandle,
  IN UINT64                             Attri,
  IN PCI_ROOT_BRIDGE_RESOURCE_APPETURE  *ResAppeture
  )
{
  EFI_STATUS                Status;
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;
  PCI_RESOURCE_TYPE         Index;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (Protocol);

  //
  // The host to pci bridge, the host memory and io addresses are
  // direct mapped to pci addresses, so no need translate, set bases to 0.
  //
  PrivateData->MemBase  = ResAppeture->MemBase;
  PrivateData->IoBase   = ResAppeture->IoBase;

  //
  // The host bridge only supports 32bit addressing for memory
  // and standard IA32 16bit io
  //
  PrivateData->MemLimit = ResAppeture->MemLimit;
  PrivateData->IoLimit  = ResAppeture->IoLimit;

  //
  // Bus Aperture for this Root Bridge (Possible Range)
  //
  PrivateData->BusBase  = ResAppeture->BusBase;
  PrivateData->BusLimit = ResAppeture->BusLimit;

  //
  // Specific for this chipset
  //
  for (Index = TypeIo; Index < TypeMax; Index++) {
    PrivateData->ResAllocNode[Index].Type   = Index;
    PrivateData->ResAllocNode[Index].Base   = 0;
    PrivateData->ResAllocNode[Index].Length = 0;
    PrivateData->ResAllocNode[Index].Status = ResNone;
  }

  EfiInitializeLock (&PrivateData->PciLock, TPL_HIGH_LEVEL);
  PrivateData->PciAddress       = 0xCF8;
  PrivateData->PciData          = 0xCFC;
  PrivateData->RootBridgeAttrib = Attri;
  PrivateData->Attributes       = 0;

  PrivateData->Supports = EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO |
                          EFI_PCI_ATTRIBUTE_ISA_IO |
                          EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO |
                          EFI_PCI_ATTRIBUTE_VGA_MEMORY |
                          EFI_PCI_ATTRIBUTE_VGA_IO;

  Protocol->ParentHandle    = HostBridgeHandle;

  Protocol->PollMem         = RootBridgeIoPollMem;
  Protocol->PollIo          = RootBridgeIoPollIo;

  Protocol->Mem.Read        = RootBridgeIoMemRead;
  Protocol->Mem.Write       = RootBridgeIoMemWrite;

  Protocol->Io.Read         = RootBridgeIoIoRead;
  Protocol->Io.Write        = RootBridgeIoIoWrite;

  Protocol->CopyMem         = RootBridgeIoCopyMem;

  Protocol->Pci.Read        = RootBridgeIoPciRead;
  Protocol->Pci.Write       = RootBridgeIoPciWrite;

  Protocol->Map             = RootBridgeIoMap;
  Protocol->Unmap           = RootBridgeIoUnmap;

  Protocol->AllocateBuffer  = RootBridgeIoAllocateBuffer;
  Protocol->FreeBuffer      = RootBridgeIoFreeBuffer;

  Protocol->Flush           = RootBridgeIoFlush;

  Protocol->GetAttributes   = RootBridgeIoGetAttributes;
  Protocol->SetAttributes   = RootBridgeIoSetAttributes;

  Protocol->Configuration   = RootBridgeIoConfiguration;

  Protocol->SegmentNumber   = 0;

  Status                    = gBS->LocateProtocol (&gEfiMetronomeArchProtocolGuid, NULL, (VOID **) &mMetronome);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **) &mCpuIo);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


/**
  Polls an address in memory mapped I/O space until an exit condition is met, or
  a timeout occurs.

  @param[in]   This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]   Width                    Signifies the width of the memory operations.
  @param[in]   Address                  The base address of the memory operations. The caller is
                                         responsible for aligning Address if required.
  @param[in]   Mask                     Mask used for the polling criteria. Bytes above Width in Mask
                                         are ignored. The bits in the bytes below Width which are zero in
                                         Mask are ignored when polling the memory address.
  @param[in]   Value                    The comparison value used for the polling exit criteria.
  @param[in]   Delay                    The number of 100 ns units to poll. Note that timer available may
                                         be of poorer granularity.
  @param[out]  Result                   Pointer to the last value read from the memory location.

  @retval      EFI_SUCCESS              The last data returned from the access matched the poll exit criteria.
  @retval      EFI_INVALID_PARAMETER    Width is invalid or Result is NULL.
  @retval      EFI_TIMEOUT              Delay expired before a match occurred.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollMem (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS  Status;
  UINT64      NumberOfTicks;
  UINTN       Remainder;

  if (Result == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // No matter what, always do a single poll.
  //
  Status = This->Mem.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((*Result & Mask) == Value) {
    return EFI_SUCCESS;
  }

  if (Delay == 0) {
    return EFI_SUCCESS;

  } else {
    //
    // Determine the proper # of metronome ticks to wait for polling the
    // location.  The nuber of ticks is Roundup (Delay / mMetronome->TickPeriod)+1
    // The "+1" to account for the possibility of the first tick being short
    // because we started in the middle of a tick.
    //
    // BugBug: overriding mMetronome->TickPeriod with UINT32 until Metronome
    // protocol definition is updated.
    //
    NumberOfTicks = DivU64x32Remainder (Delay, (UINT32) mMetronome->TickPeriod, (UINT32 *) &Remainder);
    if (Remainder != 0) {
      NumberOfTicks += 1;
    }

    NumberOfTicks += 1;

    while (NumberOfTicks) {
      mMetronome->WaitForTick (mMetronome, 1);
      Status = This->Mem.Read (This, Width, Address, 1, Result);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if ((*Result & Mask) == Value) {
        return EFI_SUCCESS;
      }

      NumberOfTicks -= 1;
    }
  }

  return EFI_TIMEOUT;
}


/**
  Reads from the I/O space of a PCI Root Bridge. Returns when either the polling exit criteria is
  satisfied or after a defined duration.

  @param[in]  This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]  Width                    Signifies the width of the I/O operations.
  @param[in]  Address                  The base address of the I/O operations. The caller is responsible
                                        for aligning Address if required.
  @param[in]  Mask                     Mask used for the polling criteria. Bytes above Width in Mask
                                        are ignored. The bits in the bytes below Width which are zero in
                                        Mask are ignored when polling the I/O address.
  @param[in]  Value                    The comparison value used for the polling exit criteria.
  @param[in]  Delay                    The number of 100 ns units to poll. Note that timer available may
                                        be of poorer granularity.
  @param[out] Result                   Pointer to the last value read from the memory location.

  @retval     EFI_SUCCESS              The last data returned from the access matched the poll exit criteria.
  @retval     EFI_INVALID_PARAMETER    Width is invalid or Result is NULL.
  @retval     EFI_TIMEOUT              Delay expired before a match occurred.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPollIo (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  )
{
  EFI_STATUS  Status;
  UINT64      NumberOfTicks;
  UINTN       Remainder;

  //
  // No matter what, always do a single poll.
  //
  if (Result == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  Status = This->Io.Read (This, Width, Address, 1, Result);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((*Result & Mask) == Value) {
    return EFI_SUCCESS;
  }

  if (Delay == 0) {
    return EFI_SUCCESS;

  } else {
    //
    // Determine the proper # of metronome ticks to wait for polling the
    // location.  The number of ticks is Roundup (Delay / mMetronome->TickPeriod)+1
    // The "+1" to account for the possibility of the first tick being short
    // because we started in the middle of a tick.
    //
    NumberOfTicks = DivU64x32Remainder (Delay, (UINT32) mMetronome->TickPeriod, (UINT32 *) &Remainder);
    if (Remainder != 0) {
      NumberOfTicks += 1;
    }

    NumberOfTicks += 1;

    while (NumberOfTicks) {
      mMetronome->WaitForTick (mMetronome, 1);
      Status = This->Io.Read (This, Width, Address, 1, Result);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if ((*Result & Mask) == Value) {
        return EFI_SUCCESS;
      }

      NumberOfTicks -= 1;
    }
  }

  return EFI_TIMEOUT;
}


/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param[in]      This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                    Signifies the width of the memory operation.
  @param[in]      Address                  The base address of the memory operation. The caller is
                                            responsible for aligning the Address if required.
  @param[in]      Count                    The number of memory operations to perform. Bytes moved is
                                            Width size * Count, starting at Address.
  @param[in, out] Buffer                   For read operations, the destination buffer to store the results. For
                                            write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS              The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  PCI_ROOT_BRIDGE_INSTANCE              *PrivateData;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH OldWidth;
  UINTN                                 OldCount;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  //
  // Check memory access limit
  //
  if (Address < PrivateData->MemBase) {
    return EFI_INVALID_PARAMETER;
  }

  OldWidth  = Width;
  OldCount  = Count;
  if (Width >= EfiPciWidthFifoUint8 && Width <= EfiPciWidthFifoUint64) {
    Count = 1;
  }

  Width &= 0x03;
  if (Address + MultU64x32 (LShiftU64 (1, Width), (UINT32) Count) - 1 > PrivateData->MemLimit) {
    return EFI_INVALID_PARAMETER;
  }

  return mCpuIo->Mem.Read (
                       mCpuIo,
                       (EFI_CPU_IO_PROTOCOL_WIDTH) OldWidth,
                       Address,
                       OldCount,
                       Buffer
                       );
}


/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge memory space.

  @param[in]      This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                   Signifies the width of the memory operation.
  @param[in]      Address                 The base address of the memory operation. The caller is
                                           responsible for aligning the Address if required.
  @param[in]      Count                   The number of memory operations to perform. Bytes moved is
                                           Width size * Count, starting at Address.
  @param[in, out] Buffer                  For read operations, the destination buffer to store the results. For
                                           write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS             The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  PCI_ROOT_BRIDGE_INSTANCE               *PrivateData;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  OldWidth;
  UINTN                                  OldCount;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  //
  // Check memory access limit
  //
  if (Address < PrivateData->MemBase) {
    return EFI_INVALID_PARAMETER;
  }

  OldWidth  = Width;
  OldCount  = Count;
  if (Width >= EfiPciWidthFifoUint8 && Width <= EfiPciWidthFifoUint64) {
    Count = 1;
  }

  Width &= 0x03;
  if (Address + MultU64x32 (LShiftU64 (1, Width), (UINT32) Count) - 1 > PrivateData->MemLimit) {
    return EFI_INVALID_PARAMETER;
  }

  return mCpuIo->Mem.Write (
                       mCpuIo,
                       (EFI_CPU_IO_PROTOCOL_WIDTH) OldWidth,
                       Address,
                       OldCount,
                       Buffer
                       );
}

/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

  @param[in]      This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                    Signifies the width of the memory operations.
  @param[in]      UserAddress              The base address of the I/O operation. The caller is responsible for
                                            aligning the Address if required.
  @param[in]      Count                    The number of I/O operations to perform. Bytes moved is Width
                                            size * Count, starting at Address.
  @param[in, out] UserBuffer               For read operations, the destination buffer to store the results. For
                                            write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS              The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  )
{
  UINTN                                  AlignMask;
  PCI_ROOT_BRIDGE_INSTANCE               *PrivateData;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  OldWidth;
  UINTN                                  OldCount;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  //
  // AlignMask = (1 << Width) - 1;
  //
  AlignMask = (1 << (Width & 0x03)) - 1;

  //
  // Check Io access limit
  //
  if (Address < PrivateData->IoBase) {
    return EFI_INVALID_PARAMETER;
  }

  OldWidth  = Width;
  OldCount  = Count;
  if (Width >= EfiPciWidthFifoUint8 && Width <= EfiPciWidthFifoUint64) {
    Count = 1;
  }

  Width &= 0x03;
  if (Address + MultU64x32 (LShiftU64 (1, Width), (UINT32) Count) - 1 >= PrivateData->IoLimit) {
    return EFI_INVALID_PARAMETER;
  }

  if (Address & AlignMask) {
    return EFI_INVALID_PARAMETER;
  }

  return mCpuIo->Io.Read (
                      mCpuIo,
                      (EFI_CPU_IO_PROTOCOL_WIDTH) OldWidth,
                      Address,
                      OldCount,
                      Buffer
                      );

}


/**
  Enables a PCI driver to access PCI controller registers in the PCI root bridge I/O space.

  @param[in]      This                      A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                     Signifies the width of the memory operations.
  @param[in]      UserAddress               The base address of the I/O operation. The caller is responsible for
                                             aligning the Address if required.
  @param[in]      Count                     The number of I/O operations to perform. Bytes moved is Width
                                             size * Count, starting at Address.
  @param[in, out] UserBuffer                For read operations, the destination buffer to store the results. For
                                             write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS               The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER     Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoIoWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH   Width,
  IN UINT64                                  Address,
  IN UINTN                                   Count,
  IN OUT VOID                                *Buffer
  )
{
  UINTN                                  AlignMask;
  PCI_ROOT_BRIDGE_INSTANCE               *PrivateData;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  OldWidth;
  UINTN                                  OldCount;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  //
  // AlignMask = (1 << Width) - 1;
  //
  AlignMask = (1 << (Width & 0x03)) - 1;

  //
  // Check Io access limit
  //
  if (Address < PrivateData->IoBase) {
    return EFI_INVALID_PARAMETER;
  }

  OldWidth  = Width;
  OldCount  = Count;
  if (Width >= EfiPciWidthFifoUint8 && Width <= EfiPciWidthFifoUint64) {
    Count = 1;
  }

  Width &= 0x03;
  if (Address + MultU64x32 (LShiftU64 (1, Width), (UINT32) Count) - 1 >= PrivateData->IoLimit) {
    return EFI_INVALID_PARAMETER;
  }

  if (Address & AlignMask) {
    return EFI_INVALID_PARAMETER;
  }

  return mCpuIo->Io.Write (
                      mCpuIo,
                      (EFI_CPU_IO_PROTOCOL_WIDTH) OldWidth,
                      Address,
                      OldCount,
                      Buffer
                      );

}


/**
  Enables a PCI driver to copy one region of PCI root bridge memory space to another region of PCI
  root bridge memory space.

  @param[in] This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
  @param[in] Width                    Signifies the width of the memory operations.
  @param[in] DestAddress              The destination address of the memory operation. The caller is
                                       responsible for aligning the DestAddress if required.
  @param[in] SrcAddress               The source address of the memory operation. The caller is
                                       responsible for aligning the SrcAddress if required.
  @param[in] Count                    The number of memory operations to perform. Bytes moved is
                                       Width size * Count, starting at DestAddress and SrcAddress.

  @retval    EFI_SUCCESS              The data was copied from one memory region to another memory region.
  @retval    EFI_INVALID_PARAMETER    Width is invalid for this PCI root bridge.

**/
EFI_STATUS
EFIAPI
RootBridgeIoCopyMem (
  IN     struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   DestAddress,
  IN     UINT64                                   SrcAddress,
  IN     UINTN                                    Count
  )
{
  EFI_STATUS  Status;
  BOOLEAN     Direction;
  UINTN       Stride;
  UINTN       Index;
  UINT64      Result;

  if (Width < 0 || Width > EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  if (DestAddress == SrcAddress) {
    return EFI_SUCCESS;
  }

  Stride    = (UINTN) (LShiftU64 (1, Width));
  Direction = TRUE;
  if ((DestAddress > SrcAddress) && (DestAddress < (SrcAddress + Count * Stride))) {
    Direction   = FALSE;
    SrcAddress  = SrcAddress + (Count - 1) * Stride;
    DestAddress = DestAddress + (Count - 1) * Stride;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = RootBridgeIoMemRead (
               This,
               Width,
               SrcAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = RootBridgeIoMemWrite (
               This,
               Width,
               DestAddress,
               1,
               &Result
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Direction) {
      SrcAddress += Stride;
      DestAddress += Stride;
    } else {
      SrcAddress -= Stride;
      DestAddress -= Stride;
    }
  }

  return EFI_SUCCESS;
}


/**
  Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

  @param[in]      This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                   Signifies the width of the memory operations.
  @param[in]      Address                 The address within the PCI configuration space for the PCI controller.
  @param[in]      Count                   The number of PCI configuration operations to perform. Bytes
                                           moved is Width size * Count, starting at Address.
  @param[in, out] Buffer                  For read operations, the destination buffer to store the results. For
                                           write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS             The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 Address,
  IN UINTN                                  Count,
  IN OUT VOID                               *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Read Pci configuration space
  //
  return RootBridgeIoPciRW (This, FALSE, Width, Address, Count, Buffer);
}


/**
  Enables a PCI driver to access PCI controller registers in a PCI root bridge's configuration space.

  @param[in]      This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Width                   Signifies the width of the memory operations.
  @param[in]      Address                 The address within the PCI configuration space for the PCI controller.
  @param[in]      Count                   The number of PCI configuration operations to perform. Bytes
                                           moved is Width size * Count, starting at Address.
  @param[in, out] Buffer                  For read operations, the destination buffer to store the results. For
                                           write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS             The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge or Buffer is NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoPciWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 Address,
  IN UINTN                                  Count,
  IN OUT VOID                               *Buffer
  )
{
  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width < 0 || Width >= EfiPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Write Pci configuration space
  //
  return RootBridgeIoPciRW (This, TRUE, Width, Address, Count, Buffer);
}


/**
  Provides the PCI controller-specific addresses required to access system memory from a
  DMA bus master.

  @param[in]       This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]       Operation                Indicates if the bus master is going to read or write to system memory.
  @param[in]       HostAddress              The system memory address to map to the PCI controller.
  @param[in, out]  NumberOfBytes            On input the number of bytes to map. On output the number of bytes that were mapped.
  @param[out]      DeviceAddress            The resulting map address for the bus master PCI controller to use
                                             to access the system memory's HostAddress.
  @param[out]      Mapping                  The value to pass to Unmap() when the bus master DMA operation is complete.

  @retval          EFI_SUCCESS              The range was mapped for the returned NumberOfBytes.
  @retval          EFI_INVALID_PARAMETER    Operation is invalid.
  @retval          EFI_INVALID_PARAMETER    HostAddress or NumberOfBytes or DeviceAddress or Mapping is NULL.
  @retval          EFI_UNSUPPORTED          The HostAddress cannot be mapped as a common buffer.

**/
EFI_STATUS
EFIAPI
RootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  )
{
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  MAP_INFO              *MapInfo;

  if (HostAddress == NULL || NumberOfBytes == NULL || DeviceAddress == NULL || Mapping == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Initialize the return values to their defaults
  //
  *Mapping = NULL;

  //
  // Make sure that Operation is valid
  //
  if (Operation < 0 || Operation >= EfiPciOperationMaximum) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Most PCAT like chipsets can not handle performing DMA above 4GB.
  // If any part of the DMA transfer being mapped is above 4GB, then
  // map the DMA transfer to a buffer below 4GB.
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) HostAddress;
  if ((PhysicalAddress + *NumberOfBytes) > 0x100000000) {
    //
    // Common Buffer operations can not be remapped.  If the common buffer
    // if above 4GB, then it is not possible to generate a mapping, so return
    // an error.
    //
    if (Operation == EfiPciOperationBusMasterCommonBuffer || Operation == EfiPciOperationBusMasterCommonBuffer64) {
      return EFI_UNSUPPORTED;
    }
    //
    // Allocate a MAP_INFO structure to remember the mapping when Unmap() is
    // called later.
    //
    MapInfo = AllocatePool (sizeof (MAP_INFO));
    if (!MapInfo) {
      *NumberOfBytes = 0;
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Return a pointer to the MAP_INFO structure in Mapping
    //
    *Mapping = MapInfo;

    //
    // Initialize the MAP_INFO structure
    //
    MapInfo->Operation          = Operation;
    MapInfo->NumberOfBytes      = *NumberOfBytes;
    MapInfo->NumberOfPages      = EFI_SIZE_TO_PAGES (*NumberOfBytes);
    MapInfo->HostAddress        = PhysicalAddress;
    MapInfo->MappedHostAddress  = 0x00000000ffffffff;

    //
    // Allocate a buffer below 4GB to map the transfer to.
    //
    MapInfo->MappedHostAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) AllocatePages (MapInfo->NumberOfPages);
    if (!(MapInfo->MappedHostAddress)) {
      FreePool (MapInfo);
      *NumberOfBytes = 0;
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // If this is a read operation from the Bus Master's point of view,
    // then copy the contents of the real buffer into the mapped buffer
    // so the Bus Master can read the contents of the real buffer.
    //
    if (Operation == EfiPciOperationBusMasterRead || Operation == EfiPciOperationBusMasterRead64) {
      CopyMem (
        (VOID *) (UINTN) MapInfo->MappedHostAddress,
        (VOID *) (UINTN) MapInfo->HostAddress,
        MapInfo->NumberOfBytes
        );
    }
    //
    // The DeviceAddress is the address of the maped buffer below 4GB
    //
    *DeviceAddress = MapInfo->MappedHostAddress;
  } else {
    //
    // The transfer is below 4GB, so the DeviceAddress is simply the HostAddress
    //
    *DeviceAddress = PhysicalAddress;
  }

  return EFI_SUCCESS;
}


/**
  Completes the Map() operation and releases any corresponding resources.

  @param[in] This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in] Mapping                 The mapping value returned from Map().

  @retval    EFI_SUCCESS             The range was unmapped.
  @retval    EFI_INVALID_PARAMETER   Mapping is not a value that was returned by Map().

**/
EFI_STATUS
EFIAPI
RootBridgeIoUnmap (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN VOID                             *Mapping
  )
{
  MAP_INFO    *MapInfo;

  //
  // See if the Map() operation associated with this Unmap() required a mapping buffer.
  // If a mapping buffer was not required, then this function simply returns EFI_SUCCESS.
  //
  if (Mapping != NULL) {
    //
    // Get the MAP_INFO structure from Mapping
    //
    MapInfo = (MAP_INFO *) Mapping;

    //
    // If this is a write operation from the Bus Master's point of view,
    // then copy the contents of the mapped buffer into the real buffer
    // so the processor can read the contents of the real buffer.
    //
    if (MapInfo->Operation == EfiPciOperationBusMasterWrite || MapInfo->Operation == EfiPciOperationBusMasterWrite64) {
      CopyMem (
        (VOID *) (UINTN) MapInfo->HostAddress,
        (VOID *) (UINTN) MapInfo->MappedHostAddress,
        MapInfo->NumberOfBytes
        );
    }
    //
    // Free the mapped buffer and the MAP_INFO structure.
    //
#ifndef __GNUC__
#pragma warning (disable: 4305)
#endif
    FreePages ((VOID *) (UINTN) MapInfo->MappedHostAddress, MapInfo->NumberOfPages);
#ifndef __GNUC__
#pragma warning (default: 4305)
#endif
    FreePool (Mapping);
  }

  return EFI_SUCCESS;
}


/**
  Allocates pages that are suitable for an EfiPciOperationBusMasterCommonBuffer or
  EfiPciOperationBusMasterCommonBuffer64 mapping.

  @param[in]  This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]  Type                     This parameter is not used and must be ignored.
  @param[in]  MemoryType               The type of memory to allocate, EfiBootServicesData or EfiRuntimeServicesData.
  @param[in]  Pages                    The number of pages to allocate.
  @param[out] HostAddress              A pointer to store the base system memory address of the allocated range.
  @param[in]  Attributes               The requested bit mask of attributes for the allocated range. Only
                                       the attributes EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE, EFI_PCI_ATTRIBUTE_MEMORY_CACHED,
                                       and EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE may be used with this function.

  @retval     EFI_SUCCESS              The requested memory pages were allocated.
  @retval     EFI_INVALID_PARAMETER    MemoryType is invalid or HostAddress is NULL.
  @retval     EFI_UNSUPPORTED          Attributes is unsupported. The only legal attribute bits are
                                       MEMORY_WRITE_COMBINE, MEMORY_CACHED, and DUAL_ADDRESS_CYCLE.

**/
EFI_STATUS
EFIAPI
RootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  )
{
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  EFI_STATUS            Status;

  //
  // Validate Attributes
  //
  if (Attributes & EFI_PCI_ATTRIBUTE_INVALID_FOR_ALLOCATE_BUFFER) {
    return EFI_UNSUPPORTED;
  }
  //
  // Check for invalid inputs
  //
  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
  //
  if (MemoryType != EfiBootServicesData && MemoryType != EfiRuntimeServicesData) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Limit allocations to memory below 4GB
  //
  PhysicalAddress = (EFI_PHYSICAL_ADDRESS) (0xffffffff);

  Status = gBS->AllocatePages (AllocateMaxAddress, MemoryType, Pages, &PhysicalAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *HostAddress = (VOID *) (UINTN) PhysicalAddress;

  return EFI_SUCCESS;
}


/**
  Frees memory that was allocated with AllocateBuffer().
  The FreeBuffer() function frees memory that was allocated with AllocateBuffer().

  @param[in]  This                     A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]  Pages                    The number of pages to free.
  @param[out] HostAddress              The base system memory address of the allocated range.

  @retval     EFI_SUCCESS              The requested memory pages were freed.
  @retval     EFI_INVALID_PARAMETER    The memory range specified by HostAddress and Pages
                                       was not allocated with AllocateBuffer().

**/
EFI_STATUS
EFIAPI
RootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  )
{
  FreePages ((VOID *) HostAddress, Pages);
  return EFI_SUCCESS;
}


/**
  Flushes all PCI posted write transactions from a PCI host bridge to system memory.

  @param[in] This              A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.

  @retval    EFI_SUCCESS       The PCI posted write transactions were flushed from the PCI host
                               bridge to system memory.

**/
EFI_STATUS
EFIAPI
RootBridgeIoFlush (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This
  )
{
  //
  // not supported yet
  //
  return EFI_SUCCESS;
}


/**
  Gets the attributes that a PCI root bridge supports setting with SetAttributes(), and the
  attributes that a PCI root bridge is currently using.

  @param[in]  This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[out] Supported               A pointer to the mask of attributes that this PCI root bridge
                                      supports setting with SetAttributes().
  @param[out] Attributes              A pointer to the mask of attributes that this PCI root bridge is
                                      currently using.

  @retval     EFI_SUCCESS             If Supports is not NULL, then the attributes that the PCI root
                                      bridge supports is returned in Supports. If Attributes is
                                      not NULL, then the attributes that the PCI root bridge is currently
                                      using is returned in Attributes.
  @retval     EFI_INVALID_PARAMETER   Both Supports and Attributes are NULL.

**/
EFI_STATUS
EFIAPI
RootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  )
{
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  if (Attributes == NULL && Supported == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Set the return value for Supported and Attributes
  //
  if (Supported) {
    *Supported = PrivateData->Supports;
  }

  if (Attributes) {
    *Attributes = PrivateData->Attributes;
  }

  return EFI_SUCCESS;
}


/**
  Sets attributes for a resource range on a PCI root bridge.

  @param[in]       This              A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]       Attributes        The mask of attributes to set. If the attribute bit
                                     MEMORY_WRITE_COMBINE, MEMORY_CACHED, or
                                     MEMORY_DISABLE is set, then the resource range is specified by
                                     ResourceBase and ResourceLength. If
                                     MEMORY_WRITE_COMBINE, MEMORY_CACHED, and
                                     MEMORY_DISABLE are not set, then ResourceBase and
                                     ResourceLength are ignored, and may be NULL.
  @param[in, out]  ResourceBase      A pointer to the base address of the resource range to be modified
                                     by the attributes specified by Attributes.
  @param[in, out]  ResourceLength    A pointer to the length of the resource range to be modified by the
                                     attributes specified by Attributes.

  @retval          EFI_SUCCESS       The current configuration of this PCI root bridge was returned in Resources.
  @retval          EFI_UNSUPPORTED   The current configuration of this PCI root bridge could not be retrieved.

**/
EFI_STATUS
EFIAPI
RootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength
  )
{
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  if (Attributes) {
    if ((Attributes & (~(PrivateData->Supports))) != 0) {
      return EFI_UNSUPPORTED;
    }
  }

  if (Attributes == PrivateData->Attributes) {
    return EFI_SUCCESS;
  }
  //
  // It is just a trick for some attribute can only be enabled or disabled
  // otherwise it can impact on other devices
  //
  PrivateData->Attributes = Attributes;

  return EFI_SUCCESS;
}


/**
  Retrieves the current resource settings of this PCI root bridge in the form of a set of ACPI 2.0
  resource descriptors.

  @param[in]   This           A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[out]  Resources      A pointer to the ACPI 2.0 resource descriptors that describe the
                              current configuration of this PCI root bridge. The storage for the
                              ACPI 2.0 resource descriptors is allocated by this function. The
                              caller must treat the return buffer as read-only data, and the buffer
                              must not be freed by the caller.

  @retval      EFI_SUCCESS    The current configuration of this PCI root bridge was returned in Resources.

**/
EFI_STATUS
EFIAPI
RootBridgeIoConfiguration (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT    VOID                             **Resources
  )
{
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;
  UINTN                     Index;

  PrivateData = DRIVER_INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS (This);

  for (Index = 0; Index < TypeMax; Index++) {
    if (PrivateData->ResAllocNode[Index].Status == ResAllocated) {
      Configuration.SpaceDesp[Index].AddrRangeMin = PrivateData->ResAllocNode[Index].Base;
      Configuration.SpaceDesp[Index].AddrRangeMax = PrivateData->ResAllocNode[Index].Base +
        PrivateData->ResAllocNode[Index].Length -
        1;
      Configuration.SpaceDesp[Index].AddrLen = PrivateData->ResAllocNode[Index].Length;
    }
  }
  *Resources = &Configuration;

  return EFI_SUCCESS;
}


/**
  Internal help function for read and write PCI configuration space.

  @param[in]      This                    A pointer to the EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param[in]      Write                   Switch value for Read or Write.
  @param[in]      Width                   Signifies the width of the memory operations.
  @param[in]      UserAddress             The address within the PCI configuration space for the PCI controller.
  @param[in]      Count                   The number of PCI configuration operations to perform. Bytes
                                          moved is Width size * Count, starting at Address.
  @param[in, out] UserBuffer              For read operations, the destination buffer to store the results. For
                                          write operations, the source buffer to write data from.

  @retval         EFI_SUCCESS             The data was read from or written to the PCI root bridge.
  @retval         EFI_INVALID_PARAMETER   Width is invalid for this PCI root bridge or Buffer is NULL.

**/
STATIC
EFI_STATUS
RootBridgeIoPciRW (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN BOOLEAN                                Write,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 UserAddress,
  IN UINTN                                  Count,
  IN OUT VOID                               *UserBuffer
  )
{
  UINT32                                       InStride;
  UINT32                                       OutStride;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  *PciRbAddr;
  UINT8                                        *PcieRegAddr;

  if ((Width & 0x03) >= EfiPciWidthUint64) {
    return EFI_INVALID_PARAMETER;
  }

  InStride  = 1 << (Width & 0x03);
  OutStride = InStride;
  if (Width >= (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) EfiCpuIoWidthFifoUint8 && Width <= (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) EfiCpuIoWidthFifoUint64) {
    InStride = 0;
  }

  if (Width >= (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) EfiCpuIoWidthFillUint8 && Width <= (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) EfiCpuIoWidthFillUint64) {
    OutStride = 0;
  }

  PciRbAddr = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS *) &UserAddress;

  PcieRegAddr = (UINT8 *) MmPciAddress (
                            0,  // No segment support
                            PciRbAddr->Bus,
                            PciRbAddr->Device,
                            PciRbAddr->Function,
                            0   // Register is added next
                            );

  //
  // Add the register offset to the address
  //
  if (PciRbAddr->ExtendedRegister != 0) {
    PcieRegAddr += PciRbAddr->ExtendedRegister;
  } else {
    PcieRegAddr += PciRbAddr->Register;
  }

  while (Count) {
    if (Write) {
      This->Mem.Write (This, Width, (UINTN) PcieRegAddr, 1, UserBuffer);
    } else {
      This->Mem.Read (This, Width, (UINTN) PcieRegAddr, 1, UserBuffer);
    }

    UserBuffer = ((UINT8 *) UserBuffer) + OutStride;
    PcieRegAddr += InStride;
    Count -= 1;
  }

  return EFI_SUCCESS;
}

