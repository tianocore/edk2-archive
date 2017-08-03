/** @file
  This library provides the GPIO library definitions.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/SideBandLib.h>
#include <Library/ScPlatformLib.h>
#include <SaAccess.h>
#include <Library/GpioLib.h>
#include <Library/SteppingLib.h>
#include <ScAccess.h>
#include <Guid/PlatformInfo.h>
#include <Library/HobLib.h>

#define GPIO_GET_GROUP_FROM_PAD(Pad)  ((Pad) >> 16)
#define GPIO_GET_PAD_OFFSET(Pad)      ((Pad) & 0xFFFF)
#define GPIO_GET_PAD_NUMBER(Offset)   ((Offset - GPIO_PADBAR) / 8)

//
// If in GPIO_GROUP_INFO structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// Possible registers to be accessed
//
typedef enum {
  GpioHostOwnershipRegister = 0,
  GpioGpeEnableRegister,
  GpioGpeStatusRegister,
  GpioSmiEnableRegister,
  GpioSmiStatusRegister,
  GpioNmiEnableRegister,
  GpioPadConfigLockRegister,
  GpioPadLockOutputRegister
} GPIO_REG;

//
// Structure for storing information about registers offset, community,
// maximal pad number for available groups
//
typedef struct {
  UINT8  Community;
  UINT16 SmiStsOffset;
  UINT16 SmiEnOffset;
  UINT16 PadCfgLockOffset;
  UINT16 PadCfgLockTxOffset;
  UINT8  PadPerGroup;
} GPIO_GROUP_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mBxtpGpioGroupInfo[] = {
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 11},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 15},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 13},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 13},
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mBxtGpioGroupInfo[] = {
  {GPIO_SOUTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 20},
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 31},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 10},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 8},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 19},
};

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval      Pointer to GPIO group table

**/
GPIO_GROUP_INFO*
GpioGetGroupInfoTable (
  OUT UINT32 *GpioGroupInfoTableLength
  )
{
  BXT_SERIES  BxtSeries;

  BxtSeries = GetBxtSeries ();
  if (BxtSeries == BxtP) {
    *GpioGroupInfoTableLength = sizeof (mBxtpGpioGroupInfo) / sizeof (GPIO_GROUP_INFO);
    return mBxtpGpioGroupInfo;
  } else if (BxtSeries == Bxt || (BxtSeries == Bxt1)) {
    *GpioGroupInfoTableLength = sizeof (mBxtGpioGroupInfo) / sizeof (GPIO_GROUP_INFO);
    return mBxtGpioGroupInfo;
  } else {
    *GpioGroupInfoTableLength = 0;
    return NULL;
  }
}


/**
  Common interface to read an 32-bit GPIO register.

  @param[in]  Community       Community Port ID definition.
                                SouthWest Community:     0xC0
                                South Community    :     0xC2
                                NorthWest Community:     0xC4
                                North Community    :     0xC5
                                West Community     :     0xC7
  @param[in]  Offset          The GPIO register to read.

  @retval     UINT32          The 32-bit value read from side band.

**/
UINT32
GpioRead (
  IN UINT8     Community,
  IN UINT16    Offset
  )
{
  return SideBandRead32 (Community, Offset);
}


/**
  Common interface to write an 32-bit GPIO register.

  @param[in]  Community       Community Port ID definition.
                                SouthWest Community:     0xC0
                                South Community    :     0xC2
                                NorthWest Community:     0xC4
                                North Community    :     0xC5
                                West Community     :     0xC7
  @param[in]  Offset          The GPIO register to write.
  @param[in]  Value           The value to write.

  @retval     EFI_SUCCESS     Write GPIO register successfully.

**/
EFI_STATUS
GpioWrite (
  IN UINT8     Community,
  IN UINT16    Offset,
  IN UINT32    Value
  )
{
  SideBandWrite32 (Community, Offset, Value);

  return EFI_SUCCESS;
}


/**
  Common interface to read an 32-bit specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadRead (N_GPIO_20)

  @param[in]  CommAndOffset    It contains GPIO community and offset.

  @retval     UINT32           The 32-bit value read from GpioRead operation.

**/
UINT32
GpioPadRead (
  IN UINT32    CommAndOffset
  )
{
  UINT8   Community;
  UINT16  GpioPadOffset;

  GpioPadOffset = (UINT16) (CommAndOffset & 0xFFFF);
  Community  = (UINT8) (CommAndOffset >> 16);

  return GpioRead (Community, GpioPadOffset);
}


/**
  Common interface to write an 32-bit to a specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadWrite (N_GPIO_20, 0xXXXXXXXX)

  @param[in]  CommAndOffset   It contains GPIO community and offset.
  @param[in]  Value           The value to write.

  @retval     EFI_SUCCESS     Write to a specific GPIO pad register successfully.
  @retval     Others          Status depends on GpioWrite operation.

**/
EFI_STATUS
GpioPadWrite (
  IN UINT32    CommAndOffset,
  IN UINT32    Value
  )
{
  UINT8   Community;
  UINT16  GpioPadOffset;

  GpioPadOffset = (UINT16) (CommAndOffset & 0xFFFF);
  Community  = (UINT8) (CommAndOffset >> 16);

  return GpioWrite (Community, GpioPadOffset, Value);
}


//
// Common interface for GPIO pad ownership check
//
GPIO_PAD_OWNERSHIP
GpioPadOwnershipCheck (
  IN  UINT32 CommAndOffset
  )
{
  GPIO_PAD_OWNERSHIP  Ownership = Host_Gpio_Mode;
  UINT8               Community = 0;
  UINT16              GpioPadOffset = 0;
  UINT16              Index = 0;
  UINT32              D32 = 0;

  GpioPadOffset = (UINT16) (CommAndOffset & 0xFFFF);
  Community  = (UINT8) (CommAndOffset >> 16);
  Index = (GpioPadOffset - GPIO_PADBAR) / 8;

  //
  // check pad ownership
  //
  D32 = GpioRead (Community, PAD_OWNERSHIP_0 + (Index / 8) * 4);
  Ownership = (GPIO_PAD_OWNERSHIP) (D32 >> ((Index % 8) << 2)) & V_PAD_OWNERSHIP_MASK;

  return Ownership;
}


/**
  Program GPIO wake event for direct IRQ GPIO pins at power on.
  Assume EVOUTEN_0 and EVMAP_0 are not initialized.

  @param[in]  Community       GPIO community.
  @param[in]  Index           GPIO pad index in its community.

**/
VOID
ConfigureDirectIrqWakeEvent (
  IN UINT8     Community,
  IN UINT16    Index
  )
{
  UINT64  d64 = 0;
  UINT32  d32;

  //
  // Both North and NorthWest community GPIO index are linear
  // But some of the GPIO pins need to be filtered due to not support Direct IRQ
  //
  if (Community == NORTH) {
    if (GetBxtSeries () == BxtP) {
      if (Index > 61) { // filter the GPIO that doesn't support Direct IRQ
        DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
        return;
      }
    } else {
      if (Index > 73) { // filter the GPIO that doesn't support Direct IRQ
        DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
        return;
      }
    }
  } else if (Community == NORTHWEST) {
    if (GetBxtSeries () == BxtP) {
      if ((Index < 33) || (Index == 52) || (Index == 60)) { // filter the GPIO that doesn't support Direct IRQ
        DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
        return;
      }
    } else {
      if ((Index < 21) || (Index == 44) || (Index == 52)) { // filter the GPIO that doesn't support Direct IRQ
        DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
        return;
      }
    }
  } else { // Direct IRQ wake event is only supported on North and Northwest communities.
    return;
  }

  //
  // program Event Trigger Mapping
  //
  d64 = (LShiftU64 ((UINT64) GpioRead (Community, EVMAP_0 + 4), 32)) | GpioRead (Community, EVMAP_0);
  d64 |= LShiftU64 ((UINT64) (Index / EVENT_MUX_SIZE), ((Index % EVENT_MUX_SIZE) << 2));
  GpioWrite (Community, EVMAP_0, (UINT32) (d64 & 0xFFFFFFFF));
  GpioWrite (Community, EVMAP_0 + 4, (UINT32) RShiftU64(d64, 32));

  //
  // program Event Trigger Output Enable
  //
  d32 = GpioRead (Community, EVOUTEN_0);
  d32 |=  1 << (Index % WAKE_EVENT_NUM);
  GpioWrite (Community, EVOUTEN_0, d32);

  return;
}


/**
  Set GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num       GPIO Pin Number to configure.
  @param[in]  Gpio_Conf_Data     Data array of default setting for each GPIO communities.

**/
VOID
GpioPadConfigTable (
  IN UINT32               Gpio_Pin_Num,
  IN BXT_GPIO_PAD_INIT    *Gpio_Conf_Data
  )
{
  UINT32           index;
  BXT_GPIO_CONFIG  mGpioConfigItem = {0};

  for (index = 0; index < Gpio_Pin_Num; index++) {
    mGpioConfigItem.CommAndOffset = (((UINT32) Gpio_Conf_Data[index].Community) << 16) + Gpio_Conf_Data[index].MMIO_ADDRESS;
    mGpioConfigItem.padConfg0.padCnf0 = Gpio_Conf_Data[index].padConfg0.padCnf0;
    mGpioConfigItem.padConfg1.padCnf1 = Gpio_Conf_Data[index].padConfg1.padCnf1;
    mGpioConfigItem.HostSw = Gpio_Conf_Data[index].HostSw;
    mGpioConfigItem.WakeEnabled = Gpio_Conf_Data[index].WakeEnabled;
    GpioPadConfigItem (mGpioConfigItem);
  }
}


/**
  Program SMIP GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num           GPIO Pin Number to configure.
  @param[in]  Gpio_Smip_Conf_Data    Data array from SMIP setting for each GPIO communities.

**/
VOID
GpioSmipPadConfigTable (
  IN UINT32                    Gpio_Pin_Num,
  IN GPIO_CONFIG_SMIP          *Gpio_Smip_Conf_Data
)
{
  UINT32           index = 0;
  BXT_GPIO_CONFIG  mGpioConfigItem = {0};
  UINT8            Community = 0;

  for (index = 0; index < Gpio_Pin_Num; index++) {
    Community = (UINT8) Gpio_Smip_Conf_Data[index].Half0.r.Community;
    //
    // Skip null entry in SMIP GPIO table
    //
    if ((Community != SOUTH) && (Community != WEST) && (Community != NORTHWEST) && \
        (Community != SOUTHWEST) && (Community != NORTH)) {
      continue;
    }
    mGpioConfigItem.CommAndOffset = (UINT32) Gpio_Smip_Conf_Data[index].Half0.r.Community << 16 | Gpio_Smip_Conf_Data[index].Half0.r.Offset;
    mGpioConfigItem.padConfg0.padCnf0 = SMIP_Config0_Convert (Gpio_Smip_Conf_Data[index].Half1.r.padConfig0);
    mGpioConfigItem.padConfg1.padCnf1 = SMIP_Config1_Convert (Gpio_Smip_Conf_Data[index].Half1.r.padConfig1);
    mGpioConfigItem.HostSw = (UINT8) Gpio_Smip_Conf_Data[index].Half0.r.HostSw;
    mGpioConfigItem.WakeEnabled = (UINT8) Gpio_Smip_Conf_Data[index].Half0.r.DirectIrqWake;
    GpioPadConfigItem (mGpioConfigItem);
  }
}


/**
  Set GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Conf_Data        The structure contains GPIO community and pad 0/1 value.

**/
VOID
GpioPadConfigItem (
  IN BXT_GPIO_CONFIG    Gpio_Conf_Data
  )
{
  UINT8               community;
  UINT16              index;
  UINT32              pad_conf0, pad_conf1;
  UINT16              mmio_conf0, mmio_conf1;
  UINT32              D32;
  GPIO_PAD_OWNERSHIP  Ownership;

  //
  // Calculate the MMIO Address for specific GPIO pin CONF0 register pointed by index.
  //
  mmio_conf0 = (UINT16) (Gpio_Conf_Data.CommAndOffset & 0xFFFF) + BXT_GPIO_PAD_CONF0_OFFSET;
  mmio_conf1 = (UINT16) (Gpio_Conf_Data.CommAndOffset & 0xFFFF) + BXT_GPIO_PAD_CONF1_OFFSET;
  community  = (UINT8) (Gpio_Conf_Data.CommAndOffset >> 16);
  index = (mmio_conf0 - GPIO_PADBAR) / 8;

  //
  // check pad ownership before any programing, if it is not owned by host, skip it.
  //
  Ownership = GpioPadOwnershipCheck (Gpio_Conf_Data.CommAndOffset);
  if (Ownership != Host_Gpio_Mode) {
    DEBUG ((EFI_D_INFO, "GPIO pad is not owned by host! skip it. Ownership: %02X\n", Ownership));
    return;
  }

  pad_conf0 = GpioRead (community, mmio_conf0);
  pad_conf0 = (pad_conf0 & PAD_DW0_MASK) | Gpio_Conf_Data.padConfg0.padCnf0;
  GpioWrite (community, mmio_conf0, pad_conf0);

  pad_conf1 = GpioRead (community, mmio_conf1);
  pad_conf1 = (pad_conf1 & PAD_DW1_MASK) | Gpio_Conf_Data.padConfg1.padCnf1;
  GpioWrite (community, mmio_conf1, pad_conf1);

  //
  // ACPI/GPIO driver ownership program
  //
  if (Gpio_Conf_Data.HostSw == GPIO_D) {
    D32 = GpioRead (community, HOSTSW_OWNERSHIP_0 + (index / 32) * 4);
    D32 = D32 | (UINT32)(1 << (index%32));
    GpioWrite (community, HOSTSW_OWNERSHIP_0 + (index / 32) * 4, D32);
  } else {
    D32 = GpioRead (community, HOSTSW_OWNERSHIP_0 + (index / 32) *4 );
    D32 = D32 & (~(UINT32) (1 << (index%32)));
    GpioWrite (community, HOSTSW_OWNERSHIP_0 + (index / 32) * 4, D32);
  }

  //
  // GPI/GPE/SMI/NMI program
  //
  switch (Gpio_Conf_Data.padConfg0.r.GPIRout) {
    case SCI: // SCI/GPE
      if (GetBxtSeries () == BxtP) {
        if (Gpio_Conf_Data.HostSw == ACPI_D) {
          DEBUG ((EFI_D_INFO, "BXT-P configure GPIO to SCI/GPE. \n"));
          //
          // clear GPE status
          //
          D32 = GpioRead (community, GPI_GPE_STS_0 + (index / 32) * 4);
          DEBUG ((EFI_D_INFO, "Original value of 0x%X: 0x%08X\n", GPI_GPE_STS_0 + (index / 32) * 4, D32));
          D32 = D32 | (UINT32) (1 << (index % 32));
          GpioWrite (community, GPI_GPE_STS_0 + (index / 32) * 4, D32);
          //
          // Set GPE enable
          //
          D32 = GpioRead (community, GPI_GPE_EN_0 + (index / 32) * 4);
          DEBUG ((EFI_D_INFO, "Original value of 0x%X: 0x%08X\n", GPI_GPE_EN_0 + (index / 32) * 4, D32));
          D32 = D32 | (UINT32) (1 << (index % 32));
          GpioWrite (community, GPI_GPE_EN_0 + (index / 32) * 4, D32);
          D32 = GpioRead (community, GPI_GPE_EN_0 + (index / 32) *4 );
          DEBUG ((EFI_D_INFO, "New value of 0x%X: 0x%08X\n", GPI_GPE_EN_0 + (index / 32) * 4, D32));
          break;
        }
      }
    case SMI:// SMI
      if (GetBxtSeries () == BxtP) {
        if (Gpio_Conf_Data.HostSw == ACPI_D) {
          DEBUG ((EFI_D_INFO, "BXT-P configure GPIO to SMI. \n"));
          //
          // clear SMI status
          //
          D32 = GpioRead (community, GPI_SMI_STS_0 + (index / 32) * 4);
          DEBUG ((EFI_D_INFO, "Original value of 0x%X: 0x%08X\n", GPI_SMI_STS_0 + (index / 32) * 4, D32));
          D32 = D32 | (UINT32) (1 << (index % 32));
          GpioWrite (community, GPI_SMI_STS_0 + (index / 32) * 4, D32);

          //
          // Set SMI enable
          //
          D32 = GpioRead (community, GPI_SMI_EN_0 + (index / 32) * 4);
          DEBUG ((EFI_D_INFO, "Original value of 0x%X: 0x%08X\n", GPI_SMI_EN_0 + (index / 32) * 4, D32));
          D32 = D32 | (UINT32) (1 << (index % 32));
          GpioWrite (community, GPI_SMI_EN_0 + (index / 32) * 4, D32);
          D32 = GpioRead (community, GPI_SMI_EN_0 + (index / 32) * 4);
          DEBUG ((EFI_D_INFO, "New value of 0x%X: 0x%08X\n", GPI_SMI_EN_0 + (index / 32) * 4, D32));
        }
      }
      break;
    case IOAPIC: // Direct IRQ
      DEBUG ((EFI_D_INFO, "BXT-P configure GPIO to direct IRQ. \n"));
      if (Gpio_Conf_Data.WakeEnabled == Wake_Enabled) {
        ConfigureDirectIrqWakeEvent (community, index);
      }
      break;
    case NMI: //No BXT pins are capable of NMI operation
      break;
    case NONE: // GPI(Shared interrupt) should be enabled by OS GPIO driver
      break;
    default:
      break;
  }
}


/**
  This procedure will get GPIO pad input inversion value

  @param[in]  GpioPad              GPIO pad
  @param[out] InvertState          GPIO inversion state
                                   0: No input inversion, 1: Inverted input

  @retval     EFI_SUCCESS          The function completed successfully.

**/
EFI_STATUS
GpioGetInputInversion (
  IN  UINT32                   GpioPad,
  OUT UINT32                   *InvertState
  )
{
  UINT32     PadConfig0;

  PadConfig0 = GpioPadRead (GpioPad);
  PadConfig0 &= 0x08FFFFFF;
  PadConfig0 >>= 23;

  *InvertState = PadConfig0;

  return EFI_SUCCESS;
}


/**
  This procedure will set GPIO pad input inversion value

  @param[in]  GpioPad               GPIO pad
  @param[in]  Value                 GPIO inversion value

  @retval     EFI_SUCCESS           Set GPIO pad input inversion value successfully.
  @retval     Others                Status depends on GpioPadWrite operation.

**/
EFI_STATUS
GpioSetInputInversion (
  IN UINT32                   GpioPad,
  IN UINT64                   Value
  )
{
  EFI_STATUS  Status ;
  UINT32      PadConfig0;

  PadConfig0 = GpioPadRead (GpioPad);
  PadConfig0 &= 0x08FFFFFF;
  PadConfig0 |= (Value >>= 32 );

  Status = GpioPadWrite (GpioPad, PadConfig0);

  return Status;
}


/**
  This procedure is used to clear SMI STS for a specified Pad

  @param[in]  GpioPad        GPIO pad

  @retval     EFI_SUCCESS    The function completed successfully.
  @retval     Others         Status depends on GpioWrite operation.

**/
EFI_STATUS
GpioClearGpiSmiSts (
  IN UINT32 GpioPad
  )
{
  UINT8       Community;
  UINT16      Offset;
  UINT32      PadNumber;
  UINT16      SmiEnableOffset;
  UINT32      SmiEnable;
  UINT32      Data32;
  EFI_STATUS  Status ;

  Community = (UINT8) ((GpioPad & 0xFFFF0000) >> 16) ;
  Offset = GpioPad &0xFFFF;

  PadNumber = (Offset - GPIO_PADBAR) / 8;
  SmiEnableOffset = GPI_SMI_STS_0 + (UINT16) (PadNumber / 32) * 4;
  SmiEnable = PadNumber % 32;

  Data32 = GpioRead (Community, SmiEnableOffset);
  Data32 |= SmiEnable;

  Status = GpioWrite (Community, SmiEnableOffset,Data32);

  return Status;
}


/**
  This procedure is used by ScSmiDispatcher and will clear
  all GPI SMI Status bits

  @retval   EFI_SUCCESS    The function completed successfully.

**/
EFI_STATUS
GpioClearAllGpiSmiSts (
  VOID
  )
{
  GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32           GpioGroupInfoSize;
  UINT8            Index;

  GpioGroupInfo     = NULL;
  GpioGroupInfoSize = 0;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoSize);
  DEBUG ((DEBUG_INFO, "GpioGroupInfoSize = %x\n", GpioGroupInfoSize));

  for (Index = 0; Index < GpioGroupInfoSize; Index++) {
    DEBUG ((DEBUG_INFO, "Community = %x,  SmiStsOffset = %x\n", GpioGroupInfo[Index].Community, GpioGroupInfo[Index].SmiStsOffset));
    //
    // Clear all GPI SMI
    //
    GpioWrite (GpioGroupInfo[Index].Community, GpioGroupInfo[Index].SmiStsOffset, ~0u);
  }
  return EFI_SUCCESS;
}


/**
  This procedure is used to register GPI SMI dispatch function.

  @param[in]  GpioPad       GPIO pad
  @param[out] GpiNum        GPI number

  @retval     EFI_SUCCESS   The function completed successfully.

**/
EFI_STATUS
GpioGetGpiSmiNum (
  IN  UINT32  GpioPad,
  OUT UINT64  *GpiNum
  )
{
  GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32           GpioGroupInfoSize;
  UINT8            Community;
  UINT16           Offset;
  UINT8            Index;
  UINT32           PadNumber;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoSize);
  Community     = (UINT8) ((GpioPad & 0xFFFF0000) >> 16) ;
  Offset        = GpioPad &0xFFFF;
  PadNumber     = (Offset - GPIO_PADBAR) / 8;
  *GpiNum       = 0;

  for (Index = 0; Index < GpioGroupInfoSize; Index++) {
    if (GpioGroupInfo[Index].Community == Community) {
      break;
    }
    *GpiNum += (UINTN) (GpioGroupInfo[Index].PadPerGroup);
  }
  *GpiNum += (UINTN) PadNumber;

  return EFI_SUCCESS;
}


/**
  This procedure is used by PchSmiDispatcher and will return information
  needed to register GPI SMI. Relation between Index and GpioPad number is:
  Index = GpioGroup + 24 * GpioPad

  @param[in]  Index                   GPI SMI number
  @param[out] GpiSmiBitOffset         GPI SMI bit position within GpiSmi Registers
  @param[out] GpiSmiEnRegAddress      Address of GPI SMI Enable register
  @param[out] GpiSmiStsRegAddress     Address of GPI SMI status register

  @retval     EFI_SUCCESS             The function completed successfully
  @retval     EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
GpioGetPadAndSmiRegs (
  IN UINT32            Index,
  OUT UINT8            *GpiSmiBitOffset,
  OUT UINT32           *GpiSmiEnRegAddress,
  OUT UINT32           *GpiSmiStsRegAddress
  )
{
  UINT32               GroupIndex;
  UINT32               PadNumber;
  GPIO_GROUP_INFO      *GpioGroupInfo;
  UINT32               GpioGroupInfoLength;
  UINT32               SmiRegOffset;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  if (GpioGroupInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = 0;
  for (GroupIndex = 0; GroupIndex < GpioGroupInfoLength; GroupIndex++) {
    PadNumber = Index;
    if (PadNumber < GpioGroupInfo[GroupIndex].PadPerGroup) {
      //
      // Found group and pad number
      //
      break;
    }
    Index = Index - GpioGroupInfo[GroupIndex].PadPerGroup;
  }

  //
  // Check if legal pad number
  //
  if (PadNumber >= GpioGroupInfo[GroupIndex].PadPerGroup){
    return EFI_INVALID_PARAMETER;
  }


  *GpiSmiBitOffset = (UINT8) PadNumber;

  SmiRegOffset = GpioGroupInfo[GroupIndex].SmiEnOffset;
  *GpiSmiEnRegAddress = SC_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, SmiRegOffset);

  SmiRegOffset = GpioGroupInfo[GroupIndex].SmiStsOffset;
  *GpiSmiStsRegAddress = SC_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, SmiRegOffset);

  return EFI_SUCCESS;
}


/**
  This procedure will write GPIO Lock/LockTx register using SBI.

  @param[in] RegType              GPIO register (Lock or LockTx)
  @param[in] Group                GPIO group number
  @param[in] PadNumber            Pad number of Group.

  @retval    EFI_SUCCESS          The function completed successfully
  @retval    EFI_UNSUPPORTED      Feature is not supported for this group or pad

**/
STATIC
EFI_STATUS
GpioWriteLockReg (
  IN GPIO_REG     RegType,
  IN GPIO_GROUP   Group,
  IN UINT32       PadNumber
  )
{
  UINT8            Response;
  EFI_STATUS       Status;
  GPIO_GROUP_INFO  *GpioGroupInfo;
  UINT32           GpioGroupInfoLength;
  UINT32           RegOffset;
  UINT32           OldPadCfgLockRegVal;
  UINT32           NewPadCfgLockRegVal;
  UINT32           GroupIndex;

  RegOffset = NO_REGISTER_FOR_PROPERTY;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  if (GpioGroupInfo == NULL) {
    return EFI_UNSUPPORTED;
  }
  for (GroupIndex = 0; GroupIndex < GpioGroupInfoLength; GroupIndex++) {
    if (Group == GpioGroupInfo[GroupIndex].Community) {
      if (PadNumber < GpioGroupInfo[GroupIndex].PadPerGroup) {
        //
        // Found group and pad number
        //
        break;
      }
      PadNumber = PadNumber - GpioGroupInfo[GroupIndex].PadPerGroup;
    }
  }

  switch (RegType) {
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      break;
    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  OldPadCfgLockRegVal = MmioRead32 (SC_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, RegOffset));
  NewPadCfgLockRegVal = OldPadCfgLockRegVal | (1 << PadNumber);

  Status = PchSbiExecution (
             GpioGroupInfo[GroupIndex].Community,
             RegOffset,
             GpioLockUnlock,
             FALSE,
             &NewPadCfgLockRegVal,
             &Response
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
GpioLockPadCfg (
  IN UINT32    GpioPad
  )
{
  EFI_STATUS  Status;
  UINT8       Community;
  UINT16      Offset;
  UINT32      PadNumber;

  Community = (UINT8) GPIO_GET_GROUP_FROM_PAD (GpioPad);
  Offset    = GPIO_GET_PAD_OFFSET (GpioPad);
  PadNumber = GPIO_GET_PAD_NUMBER (Offset);

  Status = GpioWriteLockReg (
             GpioPadConfigLockRegister,
             Community,
             PadNumber
             );

  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
GpioLockPadCfgTx (
  IN UINT32    GpioPad
  )
{
  EFI_STATUS  Status;
  UINT8       Community;
  UINT16      Offset;
  UINT32      PadNumber;

  Community = (UINT8) GPIO_GET_GROUP_FROM_PAD (GpioPad);
  Offset    = GPIO_GET_PAD_OFFSET (GpioPad);
  PadNumber = GPIO_GET_PAD_NUMBER (Offset);

  Status = GpioWriteLockReg (
             GpioPadLockOutputRegister,
             Community,
             PadNumber
             );

  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This procedure will set GPIO output level

  @param[in] GpioPad               GPIO pad
  @param[in] Value                 Output value
                                   0: OutputLow, 1: OutputHigh

  @retval    EFI_SUCCESS           The function completed successfully.
  @retval    Others                Status depends on GpioPadWrite operation.

**/
EFI_STATUS
GpioSetOutputValue (
  IN UINT32                   GpioPad,
  IN UINT32                   Value
  )
{
  EFI_STATUS        Status;
  UINT32            PadConfig0;

  PadConfig0 = GpioPadRead (GpioPad);
  PadConfig0 = (PadConfig0 & 0xFFFFFFFE) | Value;

  Status = GpioPadWrite (GpioPad, PadConfig0);

  return Status;
}


/**
  This procedure will get GPIO input level

  @param[in]  GpioPad              GPIO pad
  @param[out] InputVal             GPIO Input value
                                   0: InputLow, 1: InpuHigh

  @retval     EFI_SUCCESS          The function completed successfully.
  @retval     Others               Status depends on GpioPadRead operation.

**/
EFI_STATUS
GpioGetInputValue (
  IN UINT32         GpioPad,
  OUT UINT32        *InputVal
  )
{
  EFI_STATUS        Status;
  UINT32            PadConfig0;

  Status = EFI_SUCCESS;
  PadConfig0 = GpioPadRead (GpioPad);
  *InputVal = (PadConfig0 & 0x02) >> 1;

  return Status;
}


/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup            GPIO Group

  @retval    Value                Group Index

**/
UINT32
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  )
{
  return (UINT32) GPIO_GET_GROUP_INDEX (GpioGroup);
}


/**
  This procedure will get lowest group

  @retval  GPIO_GROUP          Lowest Group

**/
GPIO_GROUP
GpioGetLowestGroup (
  VOID
  )
{
  if (GetBxtSeries () == Bxt) {
    return (UINT32) GPIO_BXTM_GROUP_0;
  } else {
    return (UINT32) GPIO_BXTP_GROUP_0;
  }
}


/**
  This procedure will get highest group

  @retval  GPIO_GROUP          Highest Group
**/
GPIO_GROUP
GpioGetHighestGroup (
  VOID
  )
{
  if (GetBxtSeries () == Bxt) {
    return (UINT32) GPIO_BXTM_GROUP_9;
  } else {
    return (UINT32) GPIO_BXTP_GROUP_8;
  }
}


/**
  This procedure will set Group to GPE mapping.

  @param[in]  GroupToGpeDw0           GPIO group to be mapped to GPE_DW0
  @param[in]  GroupToGpeDw1           GPIO group to be mapped to GPE_DW1
  @param[in]  GroupToGpeDw2           GPIO group to be mapped to GPE_DW2

  @retval     EFI_SUCCESS             The function completed successfully.
  @retval     EFI_INVALID_PARAMETER   Invalid group or pad number.

**/
EFI_STATUS
GpioSetGroupToGpeDwX (
  IN GPIO_GROUP                GroupToGpeDw0,
  IN GPIO_GROUP                GroupToGpeDw1,
  IN GPIO_GROUP                GroupToGpeDw2
  )
{
  UINT32               Data32Or;
  UINT32               Data32And;
  GPIO_GROUP           GpioGroupLowest;
  GPIO_GROUP           GpioGroupHighest;

  GpioGroupLowest = GpioGetLowestGroup ();
  GpioGroupHighest = GpioGetHighestGroup ();

  //
  // Check if group argument exceeds GPIO group range
  //
  if (((UINT32) GroupToGpeDw0 < GpioGroupLowest) || ((UINT32) GroupToGpeDw0 > GpioGroupHighest) ||
      ((UINT32) GroupToGpeDw1 < GpioGroupLowest) || ((UINT32) GroupToGpeDw1 > GpioGroupHighest) ||
      ((UINT32) GroupToGpeDw2 < GpioGroupLowest) || ((UINT32) GroupToGpeDw2 > GpioGroupHighest)) {
    DEBUG ((DEBUG_ERROR, "GPIO ERROR: Group argument exceeds GPIO group range\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if each group number is unique
  //
  if ((GroupToGpeDw0 == GroupToGpeDw1) ||
      (GroupToGpeDw0 == GroupToGpeDw2) ||
      (GroupToGpeDw1 == GroupToGpeDw2)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Values in GPE0_DWx registers are 0 based
  //
  GroupToGpeDw0 = GpioGetGroupIndexFromGroup (GroupToGpeDw0);
  GroupToGpeDw1 = GpioGetGroupIndexFromGroup (GroupToGpeDw1);
  GroupToGpeDw2 = GpioGetGroupIndexFromGroup (GroupToGpeDw2);

  //
  // Program GPIO_MISCCFG
  //
  Data32And = (UINT32) ~(B_GPIO_MISCCFG_GPE0_DW2 | B_GPIO_MISCCFG_GPE0_DW1 | B_GPIO_MISCCFG_GPE0_DW0);
  Data32Or  = (UINT32) ((GroupToGpeDw2 << N_GPIO_MISCCFG_GPE0_DW2) |
              (GroupToGpeDw1 << N_GPIO_MISCCFG_GPE0_DW1) |
              (GroupToGpeDw0 << N_GPIO_MISCCFG_GPE0_DW0));

  if (GetBxtSeries () == Bxt) {
    //
    // Program MISCCFG register for Community SOUTH
    //
    GpioWrite (
      SOUTH,
      R_GPIO_MISCCFG,
      ((GpioRead(SOUTH ,R_GPIO_MISCCFG) & Data32And) | Data32Or)
      );
  }

  //
  // Program MISCCFG register for Community SOUTHWEST
  //
  GpioWrite (
    SOUTHWEST,
    R_GPIO_MISCCFG,
    ((GpioRead (SOUTHWEST ,R_GPIO_MISCCFG) & Data32And) | Data32Or)
    );

  //
  // Program MISCCFG register for Community WEST
  //
  GpioWrite (
    WEST,
    R_GPIO_MISCCFG,
    ((GpioRead (WEST ,R_GPIO_MISCCFG) & Data32And) | Data32Or)
    );

  //
  // Program MISCCFG register for Community NORTHWEST
  //
  GpioWrite (
    NORTHWEST,
    R_GPIO_MISCCFG,
    ((GpioRead (NORTHWEST ,R_GPIO_MISCCFG) & Data32And) | Data32Or)
    );

  //
  // Program MISCCFG register for Community NORTH
  //
  GpioWrite (
    NORTH,
    R_GPIO_MISCCFG,
    ((GpioRead (NORTH ,R_GPIO_MISCCFG) & Data32And) | Data32Or)
    );

  return EFI_SUCCESS;
}


VOID
PrintCommunity (
  IN UINT8  community
  )
{
  DEBUG ((EFI_D_INFO, "\n"));
  switch (community) {
    case SOUTH:
      DEBUG ((EFI_D_INFO, "SOUTH"));
      break;
    case WEST:
      DEBUG ((EFI_D_INFO, "WEST"));
      break;
    case NORTHWEST:
      DEBUG ((EFI_D_INFO, "NORTHWEST"));
      break;
    case SOUTHWEST:
      DEBUG ((EFI_D_INFO, "SOUTHWEST"));
      break;
    case NORTH:
      DEBUG ((EFI_D_INFO, "NORTH"));
      break;
    default:
      DEBUG ((EFI_D_INFO, "Unknown"));
      break;
  }
  DEBUG ((EFI_D_INFO, "\n"));
}


VOID
PrintGpioPadTitle (
  VOID
  )
{
  DEBUG ((EFI_D_INFO, "\nPadName                        "));
  DEBUG ((EFI_D_INFO, "Mode#     GPIO_CFG GPO_State IntType  PullUp/Down    Inverted    GPI_ROUT   IOSState  IOSTerm "));
  DEBUG ((EFI_D_INFO, "MMIO_CONF0       PAD_CONF0      MMIO_CONF1       PAD_CONF1\n"));
  DEBUG ((EFI_D_INFO, "----------------------------------------------------------------------------------------------"));
  DEBUG ((EFI_D_INFO, "-----------------------------------------------------------------------------------------------\n"));
}


VOID
DumpGpioCommunityRegisters (
  IN UINT8 community
  )
{
  UINT16  index;
  DEBUG ((EFI_D_INFO, "\nDump community registers of: "));

  PrintCommunity (community);

  for (index = 0; index < GPIO_PADBAR; index = index +4){
    DEBUG ((EFI_D_INFO, "Offset: 0x%03X Value: 0x%08X\t", index, GpioRead(community, index)));
    if (((index+4) % 16) == 0) {
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }
}


VOID
DumpGpioPadRegisters (
  IN UINT32 CommAndOffset,
  IN UINT16 *PadName
  )
{
  BXT_CONF_PAD0       pad_conf0;
  BXT_CONF_PAD1       pad_conf1;
  GPIO_PAD_OWNERSHIP  Ownership;

  Ownership = GpioPadOwnershipCheck (CommAndOffset);
  if (Ownership != Host_Gpio_Mode) {
    DEBUG ((EFI_D_INFO, " %27s GPIO pad is not owned by host! Its ownership: %02X\n", PadName, Ownership));
    return;
  }

  pad_conf0.padCnf0 = GpioPadRead(CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  pad_conf1.padCnf1 = GpioPadRead(CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);

  DEBUG ((EFI_D_INFO, " %27s >>", PadName));
  DEBUG ((EFI_D_INFO, " M%1d    %11s     %d     %5s   %9s     %12s  %6s   %10s%8s",
    pad_conf0.r.PMode,
    (pad_conf0.r.GPIORxTxDis==0)?L"RxEn,TxEn":((pad_conf0.r.GPIORxTxDis==1)?L"RxEn,TxDis":((pad_conf0.r.GPIORxTxDis==2)?L"RxDis,TxEn":L"RxDis,TxDis")),
    pad_conf0.r.GPIOTxState,
    (pad_conf0.r.RxEvCfg==0)?L"Level":((pad_conf0.r.RxEvCfg==1)?L"Edge":((pad_conf0.r.RxEvCfg==2)?L"Dis":L"Both")),
    (pad_conf1.r.Term==0||pad_conf1.r.Term==8)?L"none":((pad_conf1.r.Term==2)?L"5k wpd":((pad_conf1.r.Term==4)?L"20k wpd":((pad_conf1.r.Term==9)?L"1k wpu":((pad_conf1.r.Term==11)?L"2k wpu":((pad_conf1.r.Term==10)?L"5k wpu":((pad_conf1.r.Term==12)?L"20k wpu":((pad_conf1.r.Term==13)?L"1k&2k wpu":((pad_conf1.r.Term==15)?L"Native":L"NA")))))))),
    (pad_conf0.r.RXINV==1)?L"Inverted":L"No_Inverted",
    (pad_conf0.r.GPIRout==0)?L"NONE":((pad_conf0.r.GPIRout==1)?L"NMI":((pad_conf0.r.GPIRout==2)?L"SMI":((pad_conf0.r.GPIRout==4)?L"SCI":((pad_conf0.r.GPIRout==8)?L"IOAPIC":L"NA")))),
    (pad_conf1.r.IOSState == 0)?L"Last_Value":((pad_conf1.r.IOSState == 1)?L"D0RxDRx0I":((pad_conf1.r.IOSState == 2)?L"D0RxDRx1I":((pad_conf1.r.IOSState == 3)?L"D1RxDRx0I":((pad_conf1.r.IOSState == 4)?L"D1RxDRx1I":((pad_conf1.r.IOSState == 5)?L"D0RxE":((pad_conf1.r.IOSState == 6)?L"D1RxE":((pad_conf1.r.IOSState == 7)?L"HizRx0I":((pad_conf1.r.IOSState == 8)?L"HizRx1I":((pad_conf1.r.IOSState == 9)?L"TxDRxE":((pad_conf1.r.IOSState == 15)?L"IOS_Masked":L"NA")))))))))),
    (pad_conf1.r.IOSTerm == 0)?L"SAME":((pad_conf1.r.IOSTerm == 1)?L"DisPuPd":((pad_conf1.r.IOSTerm == 2)?L"EnPd":L"EnPu"))
    ));

  DEBUG ((EFI_D_INFO, "  0x%08X       0x%08X     0x%08X       0x%08X        \n", CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, pad_conf0.padCnf0, CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, pad_conf1.padCnf1));
}

VOID
DumpGpioPadTable (
  IN UINT32              Gpio_Pin_Size,
  IN BXT_GPIO_PAD_INIT*  Gpio_Conf_Data
  )
{
  UINT32  CommAndOffset;
  UINT16  index;

  PrintCommunity (Gpio_Conf_Data[0].Community);
  PrintGpioPadTitle ();
  for (index = 0; index < Gpio_Pin_Size; index++) {
    CommAndOffset = (((UINT32) Gpio_Conf_Data[index].Community)<<16) + Gpio_Conf_Data[index].MMIO_ADDRESS;
    DumpGpioPadRegisters (CommAndOffset, Gpio_Conf_Data[index].pad_name);
  }
}

