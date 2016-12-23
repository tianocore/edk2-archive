/** @file
  PCI Library using PC Express access.

  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "DxeRuntimePciLibPciExpress.h"

#define ASSERT_INVALID_PCI_ADDRESS(A) ASSERT (((A) &~0xfffffff) == 0)

STATIC UINTN  mPciExpressBaseAddress;

typedef struct _REGISTERED_ADDRESS_MAP {
  UINTN  PciAddress;
  UINTN  Length;
  UINTN  RuntimeAddress;
} REGISTERED_ADDRESS_MAP;

#define PCI_LIB_ADDRESS_MAP_MAX_ITEM  64

STATIC REGISTERED_ADDRESS_MAP mPciLibAddressMap[PCI_LIB_ADDRESS_MAP_MAX_ITEM];

STATIC UINTN                  mPciLibAddressMapIndex = 0;

STATIC EFI_EVENT              mVirtualAddressChangeEvent;

#ifndef __GNUC__

//
// Code not used
//
/**
  Get the base address of PCI Express memory space.

  @retval   VOID*                   Return the pointer which points to base address of PCI Express.

**/
static
VOID *
EFIAPI
GetPciExpressBaseAddress (
  VOID
  )
{
  return (VOID *) (mPciExpressBaseAddress);
}
#endif


/**
  Generate Pci Express address.
  If Address > 0x0FFFFFFF or can't get the match Pci address, then ASSERT().

  @param[in] Address                    Pci address.

  @retval    UINTN                      Pci Express address.

**/
static
UINTN
EFIAPI
PreparePciExpressAddress (
  IN  UINTN                             Address
  )
{
  UINTN  Index;

  ASSERT_INVALID_PCI_ADDRESS (Address);

  if (EfiAtRuntime () == FALSE) {
    return mPciExpressBaseAddress + Address;
  }

  for (Index = 0; Index < PCI_LIB_ADDRESS_MAP_MAX_ITEM; Index++) {
    if ((Address >= mPciLibAddressMap[Index].PciAddress) &&
        (Address < mPciLibAddressMap[Index].PciAddress + mPciLibAddressMap[Index].Length)
       ) {
      return mPciLibAddressMap[Index].RuntimeAddress + (Address - mPciLibAddressMap[Index].PciAddress);
    }
  }

  ASSERT (FALSE);
  CpuDeadLoop ();
  return 0;
}


/**
  Reads and returns the 8-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                       Address that encodes the PCI Bus, Device, Function and Register.

  @retval    UINT8                         return The read value from the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressRead8 (
  IN      UINTN                         Address
  )
{
  return MmioRead8 (PreparePciExpressAddress (Address));
}


/**
  Writes the 8-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Value                      The value to write

  @retval    UINT8                      The value to write to the MMIO register.

**/
UINT8
EFIAPI
PciExpressWrite8 (
  IN      UINTN                         Address,
  IN      UINT8                         Value
  )
{
  return MmioWrite8 (PreparePciExpressAddress (Address), Value);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressOr8 (
  IN      UINTN                         Address,
  IN      UINT8                         OrData
  )
{
  return MmioOr8 (PreparePciExpressAddress (Address), OrData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 8-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressAnd8 (
  IN      UINTN                         Address,
  IN      UINT8                         AndData
  )
{
  return MmioAnd8 (PreparePciExpressAddress (Address), AndData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 8-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressAndThenOr8 (
  IN      UINTN                         Address,
  IN      UINT8                         AndData,
  IN      UINT8                         OrData
  )
{
  return MmioAndThenOr8 (
           PreparePciExpressAddress (Address),
           AndData,
           OrData
           );
}


/**
  Reads the bit field in an 8-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.

  @retval    UINT8                      The value of the bit field read from the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressBitFieldRead8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return MmioBitFieldRead8 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit
           );
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  8-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] Value                      New value of the bit field.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressBitFieldWrite8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                     Value
  )
{
  return MmioBitFieldWrite8 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           Value
           );
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressBitFieldOr8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         OrData
  )
{
  return MmioBitFieldOr8 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           OrData
           );
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 8-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressBitFieldAnd8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         AndData
  )
{
  return MmioBitFieldAnd8 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData
           );
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 8-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciExpressBitFieldAndThenOr8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         AndData,
  IN      UINT8                         OrData
  )
{
  return MmioBitFieldAndThenOr8 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData,
           OrData
           );
}


/**
  Reads and returns the 16-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.

  @retval    UINT16                     The read value from the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressRead16 (
  IN      UINTN                         Address
  )
{
  return MmioRead16 (PreparePciExpressAddress (Address));
}


/**
  Writes the 16-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Value                      The value to write.

  @retval    UINT16                     The value written to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressWrite16 (
  IN      UINTN                         Address,
  IN      UINT16                        Value
  )
{
  return MmioWrite16 (PreparePciExpressAddress (Address), Value);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressOr16 (
  IN      UINTN                         Address,
  IN      UINT16                        OrData
  )
{
  return MmioOr16 (PreparePciExpressAddress (Address), OrData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 16-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressAnd16 (
  IN      UINTN                         Address,
  IN      UINT16                        AndData
  )
{
  return MmioAnd16 (PreparePciExpressAddress (Address), AndData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 16-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressAndThenOr16 (
  IN      UINTN                         Address,
  IN      UINT16                        AndData,
  IN      UINT16                        OrData
  )
{
  return MmioAndThenOr16 (
           PreparePciExpressAddress (Address),
           AndData,
           OrData
           );
}


/**
  Reads the bit field in a 16-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.

  @retval    UINT16                     The value of the bit field read from the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressBitFieldRead16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return MmioBitFieldRead16 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit
           );
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  16-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] Value                      New value of the bit field.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressBitFieldWrite16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        Value
  )
{
  return MmioBitFieldWrite16 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           Value
           );
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressBitFieldOr16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        OrData
  )
{
  return MmioBitFieldOr16 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           OrData
           );
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 16-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressBitFieldAnd16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        AndData
  )
{
  return MmioBitFieldAnd16 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData
           );
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 16-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciExpressBitFieldAndThenOr16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        AndData,
  IN      UINT16                        OrData
  )
{
  return MmioBitFieldAndThenOr16 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData,
           OrData
           );
}


/**
  Reads and returns the 32-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.

  @retval    UINT32                     The read value from the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressRead32 (
  IN      UINTN                         Address
  )
{
  return MmioRead32 (PreparePciExpressAddress (Address));
}


/**
  Writes the 32-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Value                      The value to write.

  @retval    UINT32                     The value written to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressWrite32 (
  IN      UINTN                         Address,
  IN      UINT32                        Value
  )
{
  return MmioWrite32 (PreparePciExpressAddress (Address), Value);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressOr32 (
  IN      UINTN                         Address,
  IN      UINT32                        OrData
  )
{
  return MmioOr32 (PreparePciExpressAddress (Address), OrData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 32-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressAnd32 (
  IN      UINTN                         Address,
  IN      UINT32                        AndData
  )
{
  return MmioAnd32 (PreparePciExpressAddress (Address), AndData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 32-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressAndThenOr32 (
  IN      UINTN                         Address,
  IN      UINT32                        AndData,
  IN      UINT32                        OrData
  )
{
  return MmioAndThenOr32 (
           PreparePciExpressAddress (Address),
           AndData,
           OrData
           );
}


/**
  Reads the bit field in a 32-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.

  @retval    UNT32                      The value of the bit field read from the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressBitFieldRead32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return MmioBitFieldRead32 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit
           );
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  32-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] Value                      New value of the bit field.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressBitFieldWrite32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        Value
  )
{
  return MmioBitFieldWrite32 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           Value
           );
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressBitFieldOr32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        OrData
  )
{
  return MmioBitFieldOr32 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           OrData
           );
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 32-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressBitFieldAnd32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        AndData
  )
{
  return MmioBitFieldAnd32 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData
           );
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 32-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciExpressBitFieldAndThenOr32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        AndData,
  IN      UINT32                        OrData
  )
{
  return MmioBitFieldAndThenOr32 (
           PreparePciExpressAddress (Address),
           StartBit,
           EndBit,
           AndData,
           OrData
           );
}


/**
  Reads the range of PCI configuration registers specified by StartAddress and
  Size into the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be read. Size is
  returned. When possible 32-bit PCI configuration read cycles are used to read
  from StartAdress to StartAddress + Size. Due to alignment restrictions, 8-bit
  and 16-bit PCI configuration read cycles may be used at the beginning and the
  end of the range.
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param[in]  StartAddress               Starting address that encodes the PCI Bus, Device, Function and Register.
  @param[in]  Size                       Size in bytes of the transfer.
  @param[out] Buffer                     Pointer to a buffer receiving the data read.

  @retval     UINTN                      Size in bytes of the transfer.

**/
UINTN
EFIAPI
PciExpressReadBuffer (
  IN      UINTN                         StartAddress,
  IN      UINTN                         Size,
  OUT     VOID                          *Buffer
  )
{
  UINTN  ReturnValue;

  ASSERT (((StartAddress & 0xFFF) + Size) <= 0x1000);

  if (Size == 0) {
    return Size;
  }

  if (Buffer == NULL) {
    ASSERT (Buffer != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Save Size for return
  //
  ReturnValue = Size;

  if ((StartAddress & 1) != 0) {
    //
    // Read a byte if StartAddress is byte aligned
    //
    *(volatile UINT8 *) Buffer = PciExpressRead8 (StartAddress);
    StartAddress += sizeof (UINT8);
    Size -= sizeof (UINT8);
    Buffer = (UINT8 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT16) && (StartAddress & 2) != 0) {
    //
    // Read a word if StartAddress is word aligned
    //
    *(volatile UINT16 *) Buffer = PciExpressRead16 (StartAddress);
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16 *) Buffer + 1;
  }

  while (Size >= sizeof (UINT32)) {
    //
    // Read as many double words as possible
    //
    *(volatile UINT32 *) Buffer = PciExpressRead32 (StartAddress);
    StartAddress += sizeof (UINT32);
    Size -= sizeof (UINT32);
    Buffer = (UINT32 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT16)) {
    //
    // Read the last remaining word if exist
    //
    *(volatile UINT16 *) Buffer = PciExpressRead16 (StartAddress);
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT8)) {
    //
    // Read the last remaining byte if exist
    //
    *(volatile UINT8 *) Buffer = PciExpressRead8 (StartAddress);
  }

  return ReturnValue;
}


/**
  Writes the range of PCI configuration registers specified by StartAddress and
  Size from the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be written. Size is
  returned. When possible 32-bit PCI configuration write cycles are used to
  write from StartAdress to StartAddress + Size. Due to alignment restrictions,
  8-bit and 16-bit PCI configuration write cycles may be used at the beginning
  and the end of the range.
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param[in] StartAddress               Starting address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Size                       Size in bytes of the transfer.
  @param[in] Buffer                     Pointer to a buffer containing the data to write.

  @retval    UINTN                      Size in bytes of the transfer.

**/
UINTN
EFIAPI
PciExpressWriteBuffer (
  IN      UINTN                         StartAddress,
  IN      UINTN                         Size,
  IN      VOID                          *Buffer
  )
{
  UINTN  ReturnValue;

  ASSERT (((StartAddress & 0xFFF) + Size) <= 0x1000);

  if (Size == 0) {
    return 0;
  }

  if (Buffer == NULL) {
    ASSERT (Buffer != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Save Size for return
  //
  ReturnValue = Size;

  if ((StartAddress & 1) != 0) {
    //
    // Write a byte if StartAddress is byte aligned
    //
    PciExpressWrite8 (StartAddress, *(UINT8 *) Buffer);
    StartAddress += sizeof (UINT8);
    Size -= sizeof (UINT8);
    Buffer = (UINT8 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT16) && (StartAddress & 2) != 0) {
    //
    // Write a word if StartAddress is word aligned
    //
    PciExpressWrite16 (StartAddress, *(UINT16 *) Buffer);
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16 *) Buffer + 1;
  }

  while (Size >= sizeof (UINT32)) {
    //
    // Write as many double words as possible
    //
    PciExpressWrite32 (StartAddress, *(UINT32 *) Buffer);
    StartAddress += sizeof (UINT32);
    Size -= sizeof (UINT32);
    Buffer = (UINT32 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT16)) {
    //
    // Write the last remaining word if exist
    //
    PciExpressWrite16 (StartAddress, *(UINT16 *) Buffer);
    StartAddress += sizeof (UINT16);
    Size -= sizeof (UINT16);
    Buffer = (UINT16 *) Buffer + 1;
  }

  if (Size >= sizeof (UINT8)) {
    //
    // Write the last remaining byte if exist
    //
    PciExpressWrite8 (StartAddress, *(UINT8 *) Buffer);
  }

  return ReturnValue;
}


/**
  Reads and returns the 8-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in]  Address                    Address that encodes the PCI Bus, Device, Function and Register.

  @retval     UINT8                      The read value from the PCI configuration register.

**/
UINT8
EFIAPI
PciRead8 (
  IN      UINTN                         Address
  )
{
  return PciExpressRead8 (Address);
}


/**
  Writes the 8-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Data                       The value to write.

  @retval    UINT8                      The value written to the PCI configuration register.

**/
UINT8
EFIAPI
PciWrite8 (
  IN      UINTN                         Address,
  IN      UINT8                         Data
  )
{
  return PciExpressWrite8 (Address, Data);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address              Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData               The value to OR with the PCI configuration register.

  @retval    UINT8                The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciOr8 (
  IN      UINTN                         Address,
  IN      UINT8                         OrData
  )
{
  return PciExpressOr8 (Address, OrData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 8-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciAnd8 (
  IN      UINTN                         Address,
  IN      UINT8                         AndData
  )
{
  return PciExpressAnd8 (Address, AndData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 8-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciAndThenOr8 (
  IN      UINTN                         Address,
  IN      UINT8                         AndData,
  IN      UINT8                         OrData
  )
{
  return PciExpressAndThenOr8 (Address, AndData, OrData);
}


/**
  Reads the bit field in an 8-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.

  @retval    UINT8                      The value of the bit field read from the PCI configuration register.

**/
UINT8
EFIAPI
PciBitFieldRead8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return PciExpressBitFieldRead8 (Address, StartBit, EndBit);
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  8-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] Value                      New value of the bit field.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciBitFieldWrite8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                          Value
  )
{
  return PciExpressBitFieldWrite8 (Address, StartBit, EndBit, Value);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 8-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciBitFieldOr8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         OrData
  )
{
  return PciExpressBitFieldOr8 (Address, StartBit, EndBit, OrData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 8-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciBitFieldAnd8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         AndData
  )
{
  return PciExpressBitFieldAnd8 (Address, StartBit, EndBit, AndData);
}


/**
  Reads the 8-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 8-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..7.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..7.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT8                      The value written back to the PCI configuration register.

**/
UINT8
EFIAPI
PciBitFieldAndThenOr8 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT8                         AndData,
  IN      UINT8                         OrData
  )
{
  return PciExpressBitFieldAndThenOr8 (Address, StartBit, EndBit, AndData, OrData);
}


/**
  Reads and returns the 16-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.

  @retval    UINT16                     The read value from the PCI configuration register.

**/
UINT16
EFIAPI
PciRead16 (
  IN      UINTN                         Address
  )
{
  return PciExpressRead16 (Address);
}


/**
  Writes the 16-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Data                       The value to write.

  @retval    UINT16                     The value written to the PCI configuration register.

**/
UINT16
EFIAPI
PciWrite16 (
  IN      UINTN                         Address,
  IN      UINT16                        Data
  )
{
  return PciExpressWrite16 (Address, Data);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciOr16 (
  IN      UINTN                         Address,
  IN      UINT16                        OrData
  )
{
  return PciExpressOr16 (Address, OrData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 16-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciAnd16 (
  IN      UINTN                         Address,
  IN      UINT16                        AndData
  )
{
  return PciExpressAnd16 (Address, AndData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 16-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciAndThenOr16 (
  IN      UINTN                         Address,
  IN      UINT16                        AndData,
  IN      UINT16                        OrData
  )
{
  return PciExpressAndThenOr16 (Address, AndData, OrData);
}


/**
  Reads the bit field in a 16-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.

  @retval    UINT16                     The value of the bit field read from the PCI configuration register.

**/
UINT16
EFIAPI
PciBitFieldRead16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return PciExpressBitFieldRead16 (Address, StartBit, EndBit);
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  16-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] Value                      New value of the bit field.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciBitFieldWrite16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        Value
  )
{
  return PciExpressBitFieldWrite16 (Address, StartBit, EndBit, Value);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 16-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciBitFieldOr16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        OrData
  )
{
  return PciExpressBitFieldOr16 (Address, StartBit, EndBit, OrData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 16-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT16                     The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciBitFieldAnd16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        AndData
  )
{
  return PciExpressBitFieldAnd16 (Address, StartBit, EndBit, AndData);
}


/**
  Reads the 16-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 16-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..15.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..15.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval   UINT16                      The value written back to the PCI configuration register.

**/
UINT16
EFIAPI
PciBitFieldAndThenOr16 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT16                        AndData,
  IN      UINT16                        OrData
  )
{
  return PciExpressBitFieldAndThenOr16 (Address, StartBit, EndBit, AndData, OrData);
}


/**
  Reads and returns the 32-bit PCI configuration register specified by Address.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.

  @retval    UINT32                     The read value from the PCI configuration register.

**/
UINT32
EFIAPI
PciRead32 (
  IN      UINTN                         Address
  )
{
  return PciExpressRead32 (Address);
}


/**
  Writes the 32-bit PCI configuration register specified by Address with the
  value specified by Value. Value is returned. This function must guarantee
  that all PCI read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Data                       The value to write.

  @retval    UINT32                     The value written to the PCI configuration register.

**/
UINT32
EFIAPI
PciWrite32 (
  IN      UINTN                         Address,
  IN      UINT32                        Data
  )
{
  return PciExpressWrite32 (Address, Data);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciOr32 (
  IN      UINTN                         Address,
  IN      UINT32                        OrData
  )
{
  return PciExpressOr32 (Address, OrData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 32-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                       Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                       The value to AND with the PCI configuration register.

  @retval    UINT32                        The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciAnd32 (
  IN      UINTN                         Address,
  IN      UINT32                        AndData
  )
{
  return PciExpressAnd32 (Address, AndData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData,
  performs a bitwise inclusive OR between the result of the AND operation and
  the value specified by OrData, and writes the result to the 32-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized.
  If Address > 0x0FFFFFFF, then ASSERT().

  @param[in] Address                    Address that encodes the PCI Bus, Device, Function and Register.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciAndThenOr32 (
  IN      UINTN                         Address,
  IN      UINT32                        AndData,
  IN      UINT32                        OrData
  )
{
  return PciExpressAndThenOr32 (Address, AndData, OrData);
}


/**
  Reads the bit field in a 32-bit PCI configuration register. The bit field is
  specified by the StartBit and the EndBit. The value of the bit field is
  returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to read.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.

  @retval    UINT32                     The value of the bit field read from the PCI configuration register.

**/
UINT32
EFIAPI
PciBitFieldRead32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit
  )
{
  return PciExpressBitFieldRead32 (Address, StartBit, EndBit);
}


/**
  Writes Value to the bit field of the PCI configuration register. The bit
  field is specified by the StartBit and the EndBit. All other bits in the
  destination PCI configuration register are preserved. The new value of the
  32-bit register is returned.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] Value                      New value of the bit field.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciBitFieldWrite32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        Value
  )
{
  return PciExpressBitFieldWrite32 (Address, StartBit, EndBit, Value);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise inclusive OR between the read result and the value specified by
  OrData, and writes the result to the 32-bit PCI configuration register
  specified by Address. The value written to the PCI configuration register is
  returned. This function must guarantee that all PCI read and write operations
  are serialized. Extra left bits in OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] OrData                     The value to OR with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciBitFieldOr32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        OrData
  )
{
  return PciExpressBitFieldOr32 (Address, StartBit, EndBit, OrData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND between the read result and the value specified by AndData, and
  writes the result to the 32-bit PCI configuration register specified by
  Address. The value written to the PCI configuration register is returned.
  This function must guarantee that all PCI read and write operations are
  serialized. Extra left bits in AndData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] AndData                    The value to AND with the PCI configuration register.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciBitFieldAnd32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        AndData
  )
{
  return PciExpressBitFieldAnd32 (Address, StartBit, EndBit, AndData);
}


/**
  Reads the 32-bit PCI configuration register specified by Address, performs a
  bitwise AND followed by a bitwise inclusive OR between the read result and
  the value specified by AndData, and writes the result to the 32-bit PCI
  configuration register specified by Address. The value written to the PCI
  configuration register is returned. This function must guarantee that all PCI
  read and write operations are serialized. Extra left bits in both AndData and
  OrData are stripped.
  If Address > 0x0FFFFFFF, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param[in] Address                    PCI configuration register to write.
  @param[in] StartBit                   The original of the least significant bit in the bit field. Range 0..31.
  @param[in] EndBit                     The original of the most significant bit in the bit field. Range 0..31.
  @param[in] AndData                    The value to AND with the PCI configuration register.
  @param[in] OrData                     The value to OR with the result of the AND operation.

  @retval    UINT32                     The value written back to the PCI configuration register.

**/
UINT32
EFIAPI
PciBitFieldAndThenOr32 (
  IN      UINTN                         Address,
  IN      UINTN                         StartBit,
  IN      UINTN                         EndBit,
  IN      UINT32                        AndData,
  IN      UINT32                        OrData
  )
{
  return PciExpressBitFieldAndThenOr32 (Address, StartBit, EndBit, AndData, OrData);
}


/**
  Reads the range of PCI configuration registers specified by StartAddress and
  Size into the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be read. Size is
  returned. When possible 32-bit PCI configuration read cycles are used to read
  from StartAdress to StartAddress + Size. Due to alignment restrictions, 8-bit
  and 16-bit PCI configuration read cycles may be used at the beginning and the
  end of the range.
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param[in]  StartAddress               Starting address that encodes the PCI Bus, Device, Function and Register.
  @param[in]  Size                       Size in bytes of the transfer.

  @param[out] Buffer                     Pointer to a buffer receiving the data read.

  @retval     UINTN                      Size in bytes of the transfer.

**/
UINTN
EFIAPI
PciReadBuffer (
  IN      UINTN                         StartAddress,
  IN      UINTN                         Size,
  OUT     VOID                          *Buffer
  )
{
  return PciExpressReadBuffer (StartAddress, Size, Buffer);
}


/**
  Writes the range of PCI configuration registers specified by StartAddress and
  Size from the buffer specified by Buffer. This function only allows the PCI
  configuration registers from a single PCI function to be written. Size is
  returned. When possible 32-bit PCI configuration write cycles are used to
  write from StartAdress to StartAddress + Size. Due to alignment restrictions,
  8-bit and 16-bit PCI configuration write cycles may be used at the beginning
  and the end of the range.
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If ((StartAddress & 0xFFF) + Size) > 0x1000, then ASSERT().
  If Size > 0 and Buffer is NULL, then ASSERT().

  @param[in] StartAddress               Starting address that encodes the PCI Bus, Device, Function and Register.
  @param[in] Size                       Size in bytes of the transfer.
  @param[in] Buffer                     Pointer to a buffer containing the data to write.

  @retval    UINTN                      The value written back to the PCI configuration register.

**/
UINTN
EFIAPI
PciWriteBuffer (
  IN      UINTN                         StartAddress,
  IN      UINTN                         Size,
  IN      VOID                          *Buffer
  )
{
  return PciExpressWriteBuffer (StartAddress, Size, Buffer);
}

/**
  Register memory space
  If StartAddress > 0x0FFFFFFF, then ASSERT().
  If SmPciLibAddressMapIndex) > PCI_LIB_ADDRESS_MAP_MAX_ITEM, then ASSERT().

  @param[in] Address                    Starting address of the memory space
  @param[in] Length                     Length of the memory space

  @retval    EFI_SUCCESS                The function completed successfully

**/
EFI_STATUS
EFIAPI
PciLibRegisterMemory (
  IN  UINTN                             Address,
  IN  UINTN                             Length
  )
{
  UINTN  Index;

  ASSERT_INVALID_PCI_ADDRESS (Address);
  ASSERT (mPciLibAddressMapIndex < PCI_LIB_ADDRESS_MAP_MAX_ITEM);

  //
  //  If already registered
  //
  for (Index = 0; Index < mPciLibAddressMapIndex; Index++) {
    if (mPciLibAddressMap[Index].PciAddress == Address) {
      return EFI_SUCCESS;
    }
  }

  mPciLibAddressMap[mPciLibAddressMapIndex].PciAddress      = Address;
  mPciLibAddressMap[mPciLibAddressMapIndex].Length          = Length;
  mPciLibAddressMap[mPciLibAddressMapIndex].RuntimeAddress  = mPciExpressBaseAddress + Address;
  mPciLibAddressMapIndex++;

  return EFI_SUCCESS;
}


/**
  Virtual address notify.
  The event handler changes PCIE base address to an virtual address.
  Starting address of registered memory scope is converted as well.

  @param[in] Event                      The event that be siganlled when virtual address changed
  @param[in] Context                    The pointer of the ESAL procedure instance

**/
VOID
EFIAPI
VirtualAddressNotifyEvent (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  UINTN  Index;

  for (Index = 0; Index < PCI_LIB_ADDRESS_MAP_MAX_ITEM; Index++) {
    if (mPciLibAddressMap[Index].PciAddress != 0) {
      EfiConvertPointer (0x0, (VOID **) &(mPciLibAddressMap[Index].RuntimeAddress));
    }
  }

  EfiConvertPointer (0x0, (VOID **) &mPciExpressBaseAddress);
}


/**
  Constructor for Pci library. Register VirtualAddressNotifyEvent() notify function
  It will ASSERT() if that operation fails

  @param[in] ImageHandle                    The firmware allocated handle for the EFI image.
  @param[in] SystemTable                    A pointer to the EFI System Table.

  @retval    EFI_SUCCESS                    The function completed successfully

**/
EFI_STATUS
EFIAPI
PciLibConstructor (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS  Status;

  mPciExpressBaseAddress = (UINTN) PcdGet64 (PcdPciExpressBaseAddress);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VirtualAddressNotifyEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  ZeroMem (mPciLibAddressMap, sizeof (mPciLibAddressMap));

  return EFI_SUCCESS;
}

