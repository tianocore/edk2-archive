/** @file
  Access to SC relevant IP base addresses.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScPlatformLibrary.h"

EFI_STATUS
EFIAPI
PchAcpiBaseSet (
  IN UINT16                                Address
  )
{
  return EFI_SUCCESS;
}

/**
  Get PCH ACPI base address.

  @param[in] Address                       Address of ACPI base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid pointer passed.

**/
EFI_STATUS
EFIAPI
PchAcpiBaseGet (
  IN UINT16                              *Address
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PchPwrmBaseSet (
  IN UINT32                             Address
  )
{
  return EFI_SUCCESS;
}

/**
  Get PCH PWRM base address.

  @param[in] Address                       Address of PWRM base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid pointer passed.

**/
EFI_STATUS
EFIAPI
PchPwrmBaseGet (
  IN UINT32                             *Address
  )
{
  return EFI_SUCCESS;
}

/**
  Set TCO base address for legacy Smbus

  @param[in] Address                       Address for TCO base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address passed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
SetTcoBase (
  IN UINT16                             Address
  )
{
  UINTN                                 SmbusBase;

  if ((Address & B_SMBUS_TCOBASE_BAR) == 0) {
    DEBUG ((DEBUG_ERROR, "SetTcoBase Error. Invalid Address: %x.\n", Address));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  SmbusBase = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_SC,
                PCI_DEVICE_NUMBER_SMBUS,
                PCI_FUNCTION_NUMBER_SMBUS
                );
  if (MmioRead16 (SmbusBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  //
  // Verify TCO base is not locked.
  //
  if ((MmioRead8 (SmbusBase + R_SMBUS_TCOCTL) & R_SMBUS_TCOCTL_TCO_BASE_LOCK) != 0) {
    ASSERT (FALSE);
    return EFI_DEVICE_ERROR;
  }
  //
  // Disable TCO in SMBUS Device first before changing base address.
  //
  MmioAnd16 (
    SmbusBase + R_SMBUS_TCOCTL,
    (UINT16) ~B_SMBUS_TCOCTL_TCO_BASE_EN
    );
  //
  // Program TCO in SMBUS Device
  //
  MmioAndThenOr16 (
    SmbusBase + R_SMBUS_TCOBASE,
    (UINT16) (~B_SMBUS_TCOBASE_BAR),
    Address
    );
  //
  // Enable TCO in SMBUS Device
  //
  MmioOr16 (
    SmbusBase + R_SMBUS_TCOCTL,
    B_SMBUS_TCOCTL_TCO_BASE_EN
    );

  return EFI_SUCCESS;
}


/**
  Get TCO base address.

  @param[in] Address                       Address of TCO base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid pointer passed.

**/
EFI_STATUS
EFIAPI
GetTcoBase (
  IN UINT16                            *Address
  )
{
  UINTN                                 SmbusBase;

  if (Address == NULL) {
    DEBUG((DEBUG_ERROR, "GetTcoBase Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  SmbusBase = MmPciBase (
                DEFAULT_PCI_BUS_NUMBER_SC,
                PCI_DEVICE_NUMBER_SMBUS,
                PCI_FUNCTION_NUMBER_SMBUS
                );
  if (MmioRead16 (SmbusBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  *Address = MmioRead16 (SmbusBase + R_SMBUS_TCOBASE) & B_SMBUS_TCOBASE_BAR;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PchLpcGenIoRangeSet (
  IN  UINT16                            Address,
  IN  UINTN                             Length
  )
{
  EFI_STATUS                            Status;
  PCH_LPC_GEN_IO_RANGE_LIST             LpcGenIoRangeList;
  UINTN                                 LpcBase;
  UINTN                                 Index;
  UINTN                                 BaseAddr;
  UINTN                                 MaskLength;
  UINTN                                 TempMaxAddr;
  UINT32                                Data32;
  UINTN                                 ArraySize;
  static struct EXCEPT_RANGE {
    UINT8 Start;
    UINT8 Length;
  } ExceptRanges[] = { {0x00, 0x20}, {0x44, 0x08}, {0x54, 0x0C}, {0x68, 0x08}, {0x80, 0x10}, {0x90, 0x10}, {0xC0, 0x40} };

  if (((Length & (Length - 1)) != 0)  ||
      ((Address & (UINT16)~B_PCH_LPC_GENX_DEC_IOBAR) != 0) ||
      (Length > 256))
  {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (Address < 0x100) {
    ArraySize = sizeof (ExceptRanges) / sizeof (struct EXCEPT_RANGE);
    for (Index = 0; Index < ArraySize; Index++) {
      if ((Address >= ExceptRanges[Index].Start) &&
          ((Address + Length) <= ((UINTN) ExceptRanges[Index].Start + (UINTN) ExceptRanges[Index].Length))) {
        break;
      }
    }
    if (Index >= ArraySize) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // check if range overlap
  //
  Status  = PchLpcGenIoRangeGet (&LpcGenIoRangeList);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
    BaseAddr = LpcGenIoRangeList.Range[Index].BaseAddr;
    MaskLength   = LpcGenIoRangeList.Range[Index].Length;
    if (BaseAddr == 0) {
      continue;
    }
    if (((Address >= BaseAddr) && (Address < (BaseAddr + MaskLength))) ||
        (((Address + Length) > BaseAddr) && ((Address + Length) <= (BaseAddr + MaskLength)))) {
      if ((Address >= BaseAddr) && (Length <= MaskLength)) {
        //
        // return SUCCESS while range is covered.
        //
        return EFI_SUCCESS;
      }

      if ((Address + Length) > (BaseAddr + MaskLength)) {
        TempMaxAddr = Address + Length;
      } else {
        TempMaxAddr = BaseAddr + MaskLength;
      }
      if (Address > BaseAddr) {
        Address = (UINT16) BaseAddr;
      }
      Length = TempMaxAddr - Address;
      break;
    }
  }
  //
  // If no range overlap
  //
  if (Index >= PCH_LPC_GEN_IO_RANGE_MAX) {
    //
    // Find a empty register
    //
    for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
      BaseAddr = LpcGenIoRangeList.Range[Index].BaseAddr;
      if (BaseAddr == 0) {
        break;
      }
    }
    if (Index >= PCH_LPC_GEN_IO_RANGE_MAX) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  //
  // Program LPC/eSPI generic IO range register accordingly.
  //
  LpcBase      = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );
  Data32 =  (UINT32) (((Length - 1) << 16) & B_PCH_LPC_GENX_DEC_IODRA);
  Data32 |= (UINT32) Address;
  Data32 |= B_PCH_LPC_GENX_DEC_EN;
  //
  // Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
  //
  MmioWrite32 (
    LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4,
    Data32
    );

  return EFI_SUCCESS;
}


/**
  Get PCH LPC/eSPI generic IO range list.
  This function returns a list of base address, length, and enable for all LPC/eSPI generic IO range regsiters.

  @param[out] LpcGenIoRangeList             Return all LPC/eSPI generic IO range register status.

  @retval     EFI_SUCCESS                   Successfully completed.
  @retval     EFI_INVALID_PARAMETER         Invalid base address passed.

**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeGet (
  OUT PCH_LPC_GEN_IO_RANGE_LIST         *LpcGenIoRangeList
  )
{
  UINTN                                 Index;
  UINTN                                 LpcBase;
  UINT32                                Data32;

  if (LpcGenIoRangeList == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  LpcBase      = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );

  for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
    Data32 = MmioRead32 (LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4);
    LpcGenIoRangeList->Range[Index].BaseAddr = Data32 & B_PCH_LPC_GENX_DEC_IOBAR;
    LpcGenIoRangeList->Range[Index].Length   = ((Data32 & B_PCH_LPC_GENX_DEC_IODRA) >> 16) + 4;
    LpcGenIoRangeList->Range[Index].Enable   = Data32 & B_PCH_LPC_GENX_DEC_EN;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PchLpcMemRangeSet (
  IN UINT32                                Address
  )
{
  UINTN                                 LpcBase;

  if ((Address & (~B_PCH_LPC_LGMR_MA)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchLpcMemRangeSet Error. Invalid Address: %x.\n", Address));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  LpcBase      = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );

  //
  // Program LPC/eSPI PCI Offset 98h [0] to [0] to disable memory decoding first before changing base address.
  //
  MmioAnd32 (
    LpcBase + R_PCH_LPC_LGMR,
    (UINT32) ~B_PCH_LPC_LGMR_LMRD_EN
    );

  //
  // Program LPC/eSPI PCI Offset 98h [31:16, 0] to [Address, 1].
  //
  MmioWrite32 (
    LpcBase + R_PCH_LPC_LGMR,
    (Address | B_PCH_LPC_LGMR_LMRD_EN)
    );

  return EFI_SUCCESS;
}


/**
  Get PCH LPC/eSPI memory range decoding address.

  @param[in] Address                       Address of LPC/eSPI memory decoding base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address passed.

**/
EFI_STATUS
EFIAPI
PchLpcMemRangeGet (
  IN UINT32                             *Address
  )
{
  UINTN                                 LpcBase;

  if (Address == NULL) {
    DEBUG ((DEBUG_ERROR, "PchLpcMemRangeGet Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  LpcBase      = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );
  *Address = MmioRead32 (LpcBase + R_PCH_LPC_LGMR) & B_PCH_LPC_LGMR_MA;

  return EFI_SUCCESS;
}


/**
  Set PCH BIOS range deocding.
  This will check General Control and Status bit 10 (GCS.BBS) to identify SPI or LPC/eSPI and program BDE register accordingly.
  Please check EDS for detail of BiosDecodeEnable bit definition.
    bit 15: F8-FF Enable
    bit 14: F0-F8 Enable
    bit 13: E8-EF Enable
    bit 12: E0-E8 Enable
    bit 11: D8-DF Enable
    bit 10: D0-D7 Enable
    bit  9: C8-CF Enable
    bit  8: C0-C7 Enable
    bit  7: Legacy F Segment Enable
    bit  6: Legacy E Segment Enable
    bit  5: Reserved
    bit  4: Reserved
    bit  3: 70-7F Enable
    bit  2: 60-6F Enable
    bit  1: 50-5F Enable
    bit  0: 40-4F Enable
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. if GCS.BBS is 0 (SPI), program SPI offset D8h to BiosDecodeEnable.
     if GCS.BBS is 1 (LPC/eSPi), program LPC offset D8h to BiosDecodeEnable.
  2. program LPC BIOS Decode Enable, PCR[DMI] + 2744h to the same value programmed in LPC or SPI Offset D8h.

  @param[in] BiosDecodeEnable              Bios decode enable setting.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchBiosDecodeEnableSet (
  IN UINT16                             BiosDecodeEnable
  )
{
  UINTN  BaseAddr;
  UINT8  Bbs;

  BaseAddr = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_SC,
               PCI_DEVICE_NUMBER_PCH_LPC,
               PCI_FUNCTION_NUMBER_PCH_LPC
               );
  Bbs = MmioRead8(BaseAddr + R_PCH_LPC_BC) & B_PCH_LPC_BC_BBS;
  //
  // Check General Control and Status (GCS) [10]
  // '0': SPI
  // '1': LPC/eSPI
  //
  if (Bbs == 0) {
    BaseAddr = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_SC,
                 PCI_DEVICE_NUMBER_SPI,
                 PCI_FUNCTION_NUMBER_SPI
                 );
    //
    // if GCS.BBS is 0 (SPI), program SPI offset D8h to BiosDecodeEnable.
    //
    MmioWrite16 (BaseAddr + R_SPI_BDE, BiosDecodeEnable);
  } else {
    BaseAddr = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_SC,
                 PCI_DEVICE_NUMBER_PCH_LPC,
                 PCI_FUNCTION_NUMBER_PCH_LPC
                 );
    //
    // if GCS.BBS is 1 (LPC/eSPI), program LPC offset D8h to BiosDecodeEnable.
    //
    MmioWrite16 (BaseAddr + R_PCH_LPC_BDE, BiosDecodeEnable);
  }

  return EFI_SUCCESS;
}


/**
  Set PCH LPC/eSPI IO decode ranges.
  Program LPC/eSPI I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC/eSPI PCI offset 80h.
  Please check EDS for detail of LPC/eSPI IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges             LPC/eSPI IO decode ranges bit settings.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN UINT16                             LpcIoDecodeRanges
  )
{
  UINTN                                 LpcBaseAddr;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //
  LpcBaseAddr  = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_SC,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );
  //
  // check if setting is identical
  //
  if (LpcIoDecodeRanges == MmioRead16 (LpcBaseAddr + R_PCH_LPC_IOD)) {
    return EFI_SUCCESS;
  }

  //
  // program LPC/eSPI PCI offset 80h.
  //
  MmioWrite16 (LpcBaseAddr + R_PCH_LPC_IOD, LpcIoDecodeRanges);

  return EFI_SUCCESS;
}


/**
  Set PCH LPC/eSPI IO enable decoding.
  Setup LPC/eSPI I/O Enables, PCR[DMI] + 2774h[15:0] to the same value program in LPC/eSPI PCI offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC/eSPI PCI offset 82h[13:10] is always forwarded by DMI to subtractive agent for handling.
  Please check EDS for detail of Lpc/eSPI IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding           LPC/eSPI IO enable decoding bit settings.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN UINT16                             LpcIoEnableDecoding
  )
{
  UINTN                                 LpcBaseAddr;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //
  LpcBaseAddr  = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_SC,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );

  if (LpcIoEnableDecoding == MmioRead16 (LpcBaseAddr + R_PCH_LPC_IOE)) {
    return EFI_SUCCESS;
  }
  //
  // program LPC/eSPI PCI offset 82h.
  //
  MmioWrite16 (LpcBaseAddr + R_PCH_LPC_IOE, LpcIoEnableDecoding);

  return EFI_SUCCESS;
}


/**
  Set PCH TraceHub base address.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program TraceHub PCI Offset 0x70~0x77 to the 64-bit base address.
  2. Program PCR[PSF3] + TraceHub RS0 offset 0x000 and 0x004 to TraceHub 64-bit base address.
  3. Manually write 1 to MSEN, PCR[PSF3] + TraceHub RS0 offset 0x01C[1] to activate the shadow.

  @param[in] AddressHi                     High 32-bits for TraceHub base address.
  @param[in] AddressLo                     Low 32-bits for TraceHub base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchTraceHubBaseSet (
  IN UINT32                             AddressHi,
  IN UINT32                             AddressLo
  )
{
  return EFI_SUCCESS;
}

//
// PCH-LP RPR destination ID table
//
UINT16 PchLpRprDidTable[] = {
  0x2188, ///< Dest ID of RP1
  0x2189, ///< Dest ID of RP2
  0x218A, ///< Dest ID of RP3
  0x218B, ///< Dest ID of RP4
  0x2198, ///< Dest ID of RP5
  0x2199, ///< Dest ID of RP6
  0x219A, ///< Dest ID of RP7
  0x219B, ///< Dest ID of RP8
  0x21A8, ///< Dest ID of RP9
  0x21A9, ///< Dest ID of RP10
  0x21AA, ///< Dest ID of RP11
  0x21AB  ///< Dest ID of RP12
};

//
// PCH-H RPR destination ID table
//
UINT16 PchHRprDidTable[] = {
  0x2180, ///< Dest ID of RP1
  0x2181, ///< Dest ID of RP2
  0x2182, ///< Dest ID of RP3
  0x2183, ///< Dest ID of RP4
  0x2188, ///< Dest ID of RP5
  0x2189, ///< Dest ID of RP6
  0x218A, ///< Dest ID of RP7
  0x218B, ///< Dest ID of RP8
  0x2198, ///< Dest ID of RP9
  0x2199, ///< Dest ID of RP10
  0x219A, ///< Dest ID of RP11
  0x219B, ///< Dest ID of RP12
  0x21A8, ///< Dest ID of RP13
  0x21A9, ///< Dest ID of RP14
  0x21AA, ///< Dest ID of RP15
  0x21AB, ///< Dest ID of RP16
  0x21B8, ///< Dest ID of RP17
  0x21B9, ///< Dest ID of RP18
  0x21BA, ///< Dest ID of RP19
  0x21BB, ///< Dest ID of RP20
};

/**
  Set PCH IO port 80h cycle decoding to PCIE root port.
  System BIOS is likely to do this very soon after reset before PCI bus enumeration.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program "RPR Destination ID", PCR[DMI] + 274Ch[31:16] to the Dest ID of RP.
  2. Program "Reserved Page Route", PCR[DMI] + 274Ch[11] to '1'. Use byte write on GCS+1 and leave the BILD bit which is RWO.

  @param[in] RpPhyNumber                   PCIE root port physical number.

  @retval    EFI_SUCCESS                   Successfully completed.

**/
EFI_STATUS
EFIAPI
PchIoPort80DecodeSet (
  IN UINTN                              RpPhyNumber
  )
{
  return EFI_SUCCESS;
}

/**
  return SPI linear Base address of descriptor region section

  @param[in] RegNum                     FLREG number of region section defined in the descriptor

  @retval UINT32                        Base address of the FLREG
**/
UINT32
GetSpiFlashRegionBase (
  IN UINTN        RegNum
  )
{
  UINTN        SpiPciBase;
  UINT32       SpiBar0;
  UINT32       FlashRegBase;
  UINT16       TempCmd;

  SpiPciBase = MmPciBase ( DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI);
  SpiBar0 = MmioRead32 (SpiPciBase + R_SPI_BASE) & B_SPI_BASE_BAR;
  TempCmd = MmioRead16 (SpiPciBase + PCI_COMMAND_OFFSET);
  MmioOr16 (SpiPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  FlashRegBase = MmioRead32 (SpiBar0 + R_SPI_FREG0_FLASHD + RegNum * 4) & B_SPI_FREG0_BASE_MASK;
  if (FlashRegBase == V_SPI_FLREG_DISABLED) {
    FlashRegBase = 0;
    DEBUG((DEBUG_ERROR, "SPI FLREG%d is disabled!!!\n", RegNum));
  }
  FlashRegBase <<= N_SPI_FREG0_BASE;

  DEBUG ((DEBUG_INFO, "SPI FLREG%d base = 0x%x\n", RegNum, FlashRegBase));
  MmioWrite16 (SpiPciBase + PCI_COMMAND_OFFSET, TempCmd);

  return FlashRegBase;
}


/**
  return SPI linear Base address of descriptor region section

  @param[in] RegNum                     FLREG number of region section defined in the descriptor

  @retval    UINTN                      Base address of the FLREG

**/
UINT32
GetSpiFlashRegionLimit (
  UINTN        RegNum
  )
{
  UINTN        SpiPciBase;
  UINT32       SpiBar0;
  UINT32       FlashRegLimit;
  UINT16       TempCmd;

  SpiPciBase = MmPciBase ( DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI);
  SpiBar0 = MmioRead32 (SpiPciBase + R_SPI_BASE) & B_SPI_BASE_BAR;
  TempCmd = MmioRead16 (SpiPciBase + PCI_COMMAND_OFFSET);
  MmioOr16 (SpiPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  FlashRegLimit = MmioRead32 (SpiBar0 + R_SPI_FREG0_FLASHD + RegNum * 4) & B_SPI_FREG0_LIMIT_MASK;
  FlashRegLimit >>= N_SPI_FREG1_LIMIT;

  if (FlashRegLimit == V_SPI_FLREG_DISABLED) {
    FlashRegLimit = 0;
    DEBUG((DEBUG_ERROR, "SPI FLREG%d is disabled!!!\n", RegNum));
  }

  DEBUG ((DEBUG_INFO, "SPI FLREG%d limit = 0x%x\n", RegNum, FlashRegLimit));
  MmioWrite16 (SpiPciBase + PCI_COMMAND_OFFSET, TempCmd);

  return FlashRegLimit;
}

