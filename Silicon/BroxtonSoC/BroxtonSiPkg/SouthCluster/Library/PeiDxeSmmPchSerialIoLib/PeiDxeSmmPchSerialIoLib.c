/** @file
  PCH Serial IO Lib implementation.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/Pci30.h>
#include <ScAccess.h>
#include <ScRegs/RegsLpss.h>
#include <Library/ScPcrLib.h>
#include <Library/ScInfoLib.h>
#include <Library/MmPciLib.h>
#include <Library/ScSerialIoLib.h>

typedef struct {
  UINT32 Bar0;
  UINT32 Bar1;
} SERIAL_IO_CONTROLLER_DESCRIPTOR;

typedef struct {
  UINT8  DevNum;
  UINT8  FuncNum;
} SERIAL_IO_BDF_NUMBERS;

typedef struct {
  UINT16 PciDevIdLp;
  CHAR8  AcpiHid[SERIALIO_HID_LENGTH];
} SERIAL_IO_ID;

GLOBAL_REMOVE_IF_UNREFERENCED SERIAL_IO_BDF_NUMBERS mSerialIoBdf [4] =
{
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART0},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART1},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART2},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART3},
};

GLOBAL_REMOVE_IF_UNREFERENCED SERIAL_IO_CONTROLLER_DESCRIPTOR mSerialIoAcpiAddress [4] =
{

  {LPSS_I2C0_TMP_BAR0 + 0x80000,LPSS_I2C0_TMP_BAR0 + 0x81000},
  {LPSS_I2C0_TMP_BAR0 + 0x90000,LPSS_I2C0_TMP_BAR0 + 0x91000},
  {LPSS_I2C0_TMP_BAR0 + 0x100000,LPSS_I2C0_TMP_BAR0 + 0x101000},
  {LPSS_I2C0_TMP_BAR0 + 0x110000,LPSS_I2C0_TMP_BAR0 + 0x111000},
};

typedef struct {
  UINT16 PciCfgCtrAddr;
  UINT16 Psf3BaseAddress;
} SERIAL_IO_CONTROLLER_PSF3_OFFSETS;

GLOBAL_REMOVE_IF_UNREFERENCED SERIAL_IO_CONTROLLER_PSF3_OFFSETS mPchLpSerialIoPsf3Offsets [4] =
{
  { 0x220, 0x1100},
  { 0x224, 0x1000},
  { 0x228, 0x0F00},
  { 0x22C, 0x0E00},
};


/**
  Finds PCI Device Number of SerialIo devices.
  SerialIo devices' BDF is configurable

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2

  @retval    UINT8                      SerialIo device number

**/
UINT8
GetSerialIoDeviceNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  )
{
  return mSerialIoBdf[SerialIoNumber].DevNum;
}


/**
  Finds PCI Function Number of SerialIo devices.
  SerialIo devices' BDF is configurable

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2

  @retval    UINT8                      SerialIo funciton number

**/
UINT8
GetSerialIoFunctionNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  )
{
  return mSerialIoBdf[SerialIoNumber].FuncNum;
}


/**
  Finds BAR value of SerialIo devices.

  SerialIo devices can be configured to not appear on PCI so traditional method of reading BAR might not work.
  If the SerialIo device is in PCI mode, a request for BAR1 will return its PCI CFG space instead

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2
  @param[in] BarNumber                  0=BAR0, 1=BAR1

  @retval    UINTN                      SerialIo Bar value

**/
UINTN
FindSerialIoBar (
  IN PCH_SERIAL_IO_CONTROLLER           SerialIoDevice,
  IN UINT8                              BarNumber
  )
{
  UINT32  Bar;
  UINTN   PcieBase;
  UINT32  VenId;

  PcieBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_SC, GetSerialIoDeviceNumber (SerialIoDevice), GetSerialIoFunctionNumber (SerialIoDevice));

  VenId = MmioRead32 (PcieBase + PCI_VENDOR_ID_OFFSET) & 0xFFFF;

  if (VenId == V_INTEL_VENDOR_ID) {
    if (BarNumber == 1) {
      return PcieBase;
    }
    Bar = (MmioRead32 (PcieBase + PCI_BASE_ADDRESSREG_OFFSET + 8*BarNumber) & 0xFFFFF000);
    return Bar;
  }

  return 0xFFFFFFFF;
}


/**
  Configures Serial IO Controller.

  @param[in] Controller                 0=I2C0, ..., 11=UART2
  @param[in] DeviceMode                 Different type of serial io mode defined in PCH_SERIAL_IO_MODE
  @param[in] SerialIoSafeRegister       D0i3 Max Power On Latency and Device PG config

**/
VOID
ConfigureSerialIoController (
  IN PCH_SERIAL_IO_CONTROLLER Controller,
  IN PCH_SERIAL_IO_MODE       DeviceMode
#ifdef PCH_PO_FLAG
  , IN UINT32                 SerialIoSafeRegister
#endif
  )
{
  UINTN                               PciCfgBase;
  UINTN                               Bar;
  UINT32                              Data32;
  SERIAL_IO_CONTROLLER_PSF3_OFFSETS   *SerialIoPsf3Offsets;

  PciCfgBase = MmPciBase (0, GetSerialIoDeviceNumber (Controller), GetSerialIoFunctionNumber (Controller));

  //
  // Do not modify a device that has already been initialized
  //
  if (MmioRead16(PciCfgBase + PCI_VENDOR_ID_OFFSET) != V_INTEL_VENDOR_ID) {
    return;
  }

#ifdef PCH_PO_FLAG
  if (!SerialIoSafeRegister) {
#endif
  //
  // Step 1. Set Bit 16,17,18.
  //
  MmioOr32 (PciCfgBase + R_LPSS_IO_D0I3MAXDEVPG, BIT18 | BIT17 | BIT16);
#ifdef PCH_PO_FLAG
  }
#endif

  SerialIoPsf3Offsets = mPchLpSerialIoPsf3Offsets;

  Bar = MmioRead32(PciCfgBase + R_LPSS_IO_BAR) & 0xFFFFF000;

  switch (DeviceMode) {
    case PchSerialIoDisabled:
      MmioOr32 (PciCfgBase + R_LPSS_IO_PCS, BIT1 | BIT0);
      PchPcrAndThenOr32 (0xC6, SerialIoPsf3Offsets[Controller].Psf3BaseAddress+0x001C, 0xFFFFFFFF, BIT8);
      break;

    case PchSerialIoAcpi:
    case PchSerialIoAcpiHidden:
    case PchSerialIoLegacyUart:
      //
      // Assign BAR0
      // Assign BAR1
      //
      if (Bar == 0) {
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR, mSerialIoAcpiAddress[Controller].Bar0);
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR_HIGH, 0x0);
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR1, mSerialIoAcpiAddress[Controller].Bar1);
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR1_HIGH, 0x0);
        Bar = MmioRead32 (PciCfgBase + R_LPSS_IO_BAR) & 0xFFFFF000;
      }

      MmioWrite32 (Bar + R_LPSS_IO_REMAP_ADDRESS_LOW, (UINT32)Bar);

      //
      // Set Memory space Enable
      //
      MmioOr32 (PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);

      //
      // Get controller out of reset
      //
      MmioWrite32 (Bar + R_LPSS_IO_MEM_RESETS, 0);
      MmioWrite32 (Bar + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_HC_RESET_REL | B_LPSS_IO_MEM_iDMA_RESET_REL);
      break;

    case PchSerialIoPci:
      if (Bar == 0) {

        PchPcrAndThenOr32 (0x90, SerialIoPsf3Offsets[Controller].PciCfgCtrAddr, 0xFFFFFFFF, BIT7);
        PchPcrAndThenOr32 (0xC6, SerialIoPsf3Offsets[Controller].Psf3BaseAddress + 0x001C, 0xFFFFFFFF, BIT18);

        do {
          PchPcrRead32(0xC6, SerialIoPsf3Offsets[Controller].Psf3BaseAddress + 0x001C, &Data32);
        } while ((Data32 & BIT18) != BIT18);

        //
        // Assign BAR0 and Set Memory space Enable
        //
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR,  mSerialIoAcpiAddress[Controller].Bar0);
        MmioWrite32 (PciCfgBase + R_LPSS_IO_BAR_HIGH, 0x0);
        MmioOr32    (PciCfgBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);

        //
        // Read Newly Assigned BAR
        //
        Bar = MmioRead32(PciCfgBase + R_LPSS_IO_BAR) & 0xFFFFF000;
      }

      //
      // Update Address Remap Register with Current BAR
      //
      MmioWrite32 (Bar + R_LPSS_IO_REMAP_ADDRESS_LOW, (UINT32)Bar);

      ///
      /// Get controller out of reset
      ///
      MmioWrite32 (Bar + R_LPSS_IO_MEM_RESETS, 0);

      MmioOr32 (Bar + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_HC_RESET_REL | B_LPSS_IO_MEM_iDMA_RESET_REL);
      break;

    default:
      return;
  }

  //
  // Program clock dividers for UARTs in legacy mode
  // Enable Byte addressing for UARTs in legacy mode
  //
  if (Controller >= PchSerialIoIndexUart0 && Controller <= PchSerialIoIndexUart3) {
      Data32 = B_LPSS_IO_MEM_PCP_CLK_UPDATE | (V_LPSS_IO_PPR_CLK_N_DIV << 16) | (V_LPSS_IO_PPR_CLK_M_DIV << 1) | B_LPSS_IO_MEM_PCP_CLK_EN;
      MmioWrite32 (Bar + R_LPSS_IO_MEM_PCP, Data32);

      if (DeviceMode == PchSerialIoLegacyUart) {
        //
        // Set UART Byte Address Control - Control bit for 16550 8-Bit Addressing Mode.
        //
        PchPcrAndThenOr32(0x90, 0x618, 0xFFFFFFFF, (BIT0 << (Controller - PchSerialIoIndexUart0)));

        //
        // An MMIO Read Must Immediately Be Issued to UART2 BAR0 + 0xF8 for 8-bit Legacy Mode to Activate
        //
        MmioRead32(Bar + R_LPSS_IO_MANID);
      }
  }
}


