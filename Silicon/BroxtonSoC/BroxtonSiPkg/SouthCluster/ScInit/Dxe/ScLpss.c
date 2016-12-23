/** @file
  Initializes SC LPSS Devices.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <IndustryStandard/Acpi.h>
#include <Library/GpioLib.h>
#include <Library/PmcIpcLib.h>
#include <Library/SteppingLib.h>
#include <SaAccess.h>
#include "ScInit.h"

typedef struct {
  UINT32 PciDeviceNum;
  UINT32 PciFuncNum;
  UINT16 LpssPciCfgCtrl;
  UINT16 Psf3BaseAddress;
  UINT32 LpssFuncDisableBit;
} SERIAL_IO_CONTROLLER_DESCRIPTOR;

#define LPSS_IO_DEVICE_NUM 15

//
// Note: the devices order need to align with the order defined in SC_LPSS_CONFIG
//
SERIAL_IO_CONTROLLER_DESCRIPTOR mLpssIoDevices[LPSS_IO_DEVICE_NUM] =
{
  //
  // I2C 0~7
  //
  {PCI_DEVICE_NUMBER_LPSS_I2C0, PCI_FUNCTION_NUMBER_LPSS_I2C0, 0x200, 0x1900, B_PMC_FUNC_DIS_LPSS_I2C0},
  {PCI_DEVICE_NUMBER_LPSS_I2C0, PCI_FUNCTION_NUMBER_LPSS_I2C1, 0x204, 0x1800, B_PMC_FUNC_DIS_LPSS_I2C1},
  {PCI_DEVICE_NUMBER_LPSS_I2C0, PCI_FUNCTION_NUMBER_LPSS_I2C2, 0x208, 0x1700, B_PMC_FUNC_DIS_LPSS_I2C2},
  {PCI_DEVICE_NUMBER_LPSS_I2C0, PCI_FUNCTION_NUMBER_LPSS_I2C3, 0x20C, 0x1600, B_PMC_FUNC_DIS_LPSS_I2C3},
  {PCI_DEVICE_NUMBER_LPSS_I2C1, PCI_FUNCTION_NUMBER_LPSS_I2C4, 0x210, 0x1500, B_PMC_FUNC_DIS_LPSS_I2C4},
  {PCI_DEVICE_NUMBER_LPSS_I2C1, PCI_FUNCTION_NUMBER_LPSS_I2C5, 0x214, 0x1400, B_PMC_FUNC_DIS_LPSS_I2C5},
  {PCI_DEVICE_NUMBER_LPSS_I2C1, PCI_FUNCTION_NUMBER_LPSS_I2C6, 0x218, 0x1300, B_PMC_FUNC_DIS_LPSS_I2C6},
  {PCI_DEVICE_NUMBER_LPSS_I2C1, PCI_FUNCTION_NUMBER_LPSS_I2C7, 0x21C, 0x1200, B_PMC_FUNC_DIS_LPSS_I2C7},

  //
  // UART 0~3
  //
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART0, 0x220, 0x1100, B_PMC_FUNC_DIS_LPSS_UART0},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART1, 0x224, 0x1000, B_PMC_FUNC_DIS_LPSS_UART1},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART2, 0x228, 0x0F00, B_PMC_FUNC_DIS_LPSS_UART2},
  {PCI_DEVICE_NUMBER_LPSS_HSUART, PCI_FUNCTION_NUMBER_LPSS_HSUART3, 0x22C, 0x0E00, B_PMC_FUNC_DIS_LPSS_UART3},

  //
  // SPI 0~2
  //
  {PCI_DEVICE_NUMBER_LPSS_SPI, PCI_FUNCTION_NUMBER_LPSS_SPI0, 0x230, 0x0D00, B_PMC_FUNC_DIS_LPSS_SPI0},
  {PCI_DEVICE_NUMBER_LPSS_SPI, PCI_FUNCTION_NUMBER_LPSS_SPI1, 0x234, 0x0C00, B_PMC_FUNC_DIS_LPSS_SPI1},
  {PCI_DEVICE_NUMBER_LPSS_SPI, PCI_FUNCTION_NUMBER_LPSS_SPI2, 0x238, 0x0B00, B_PMC_FUNC_DIS_LPSS_SPI2}
};


/**
  Hide PCI config space of LPSS devices and do any final initialization.

  @param[in] ScPolicy                      The SC Policy instance

  @retval    EFI_SUCCESS                   The function completed successfully
  @retval    EFI_NOT_FOUND                 GNVS Protocol interface not found

**/
EFI_STATUS
ConfigureLpssAtBoot (
  IN SC_POLICY_HOB              *ScPolicy
  )
{
  UINTN                         LpssPciMmBase;
  UINT32                        LpssMmioBase0;
  UINT32                        LpssMmioBase0_High;
  UINT32                        P2sbMmioBar;
  UINT32                        Buffer32;
  EFI_STATUS                    AcpiTablePresent;
  EFI_STATUS                    Status;
  UINT64                        Data64;
  UINTN                         AcpiTableKey;
  UINT8                         Index;
  SC_LPSS_CONFIG                *LpssConfig;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
  UINT32                        PciP2sbBar0RegOffset;

  DEBUG ((DEBUG_INFO, "ConfigureLpssAtBoot() Start\n"));
  Status = GetConfigBlock ((VOID *) ScPolicy, &gLpssConfigGuid, (VOID *) &LpssConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize Variables
  //
  LpssPciMmBase            = 0;
  LpssMmioBase0            = 0;
  LpssMmioBase0_High       = 0;
  P2sbMmioBar              = 0;

  Buffer32                 = 0;
  AcpiTablePresent         = EFI_NOT_FOUND;
  AcpiTableKey             = 0;
  Data64                   = 0;
  Index                    = 0;
  Status                   = EFI_SUCCESS;

  //
  // Locate ACPI table
  //
  AcpiTablePresent = InitializeScAslUpdateLib ();

  for (Index = 0; Index < LPSS_IO_DEVICE_NUM; Index++) {
    LpssPciMmBase = MmPciBase(
      DEFAULT_PCI_BUS_NUMBER_SC,
      mLpssIoDevices[Index].PciDeviceNum,
      mLpssIoDevices[Index].PciFuncNum
      );

    LpssMmioBase0 = MmioRead32 ((UINTN) (LpssPciMmBase + R_LPSS_IO_BAR));
    LpssMmioBase0 &= 0xFFFFFFF0;  // Mask off the last byte read from BAR
    LpssMmioBase0_High = MmioRead32 ((UINTN) (LpssPciMmBase + R_LPSS_IO_BAR_HIGH));

    //
    // Assume the MMIO address for BAR0 is always 32 bits (high DW is 0).
    //
    MmioWrite32 ((UINTN) (LpssMmioBase0 + R_LPSS_IO_REMAP_ADDRESS_LOW), LpssMmioBase0);
    MmioWrite32 ((UINTN) (LpssMmioBase0 + R_LPSS_IO_REMAP_ADDRESS_HI), LpssMmioBase0_High);

    DEBUG ((DEBUG_INFO, "Read back LPSS REMAP Register, High DW = 0x%x, Low DW = 0x%x, Index = %d\n",
      MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_IO_REMAP_ADDRESS_HI)),
      MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_IO_REMAP_ADDRESS_LOW)),
      Index));

    // Hide the UART2 device if kernel debugger mode is enabled
    // BXT UART2 device is hidden in OnReadyToBoot (BIOS and OS use different debug COM port)
    //
    if (mLpssIoDevices[Index].PciDeviceNum == PCI_DEVICE_NUMBER_LPSS_HSUART
      && mLpssIoDevices[Index].PciFuncNum == PCI_FUNCTION_NUMBER_LPSS_HSUART2
      && GetBxtSeries () == BxtP
      && LpssConfig->OsDbgEnable
      && LpssMmioBase0 != 0xFFFFFFFF) {

      DEBUG ((DEBUG_INFO, "Kernel Debug is Enabled\n"));
      DEBUG ((DEBUG_INFO, "---------------------------------------------------------------------------------\n"));
      DEBUG ((DEBUG_INFO, "This concludes BIOS serial debug messages, open the kernel debugger (WinDBG) now.\n\n"));
      DEBUG ((DEBUG_INFO, "To enable continued serial and console redirection:\n"));
      DEBUG ((DEBUG_INFO, "    1) Open the BIOS setup menu\n"));
      DEBUG ((DEBUG_INFO, "    2) Navigate to Device Manager->System Setup->Debug Configuration->Kernel Debugger Enable\n"));
      DEBUG ((DEBUG_INFO, "    3) Select the \"Disabled\" option and save\n\n"));

      SideBandAndThenOr32 (
        0xC6,
        0x0F38,
        0xFFFFFFFF,
        BIT0
        );

      DEBUG ((DEBUG_INFO, "Kernel Debugger Mode Enabled - LPSS D24 F2 CFG_DIS: 0x%08X\n", SideBandRead32 (0xC6, 0x0F38)));
    }
  }

  //
  // Update LPSS devices ACPI variables
  //
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &GlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PciP2sbBar0RegOffset = (UINT32) MmPciAddress (
                                    0,
                                    DEFAULT_PCI_BUS_NUMBER_SC,
                                    PCI_DEVICE_NUMBER_P2SB,
                                    PCI_FUNCTION_NUMBER_P2SB,
                                    R_P2SB_BASE
                                    );

  //
  // Read back P2SB MMIO BAR Base Addr
  //
  P2sbMmioBar = MmioRead32 (PciP2sbBar0RegOffset);
  if (P2sbMmioBar == 0xFFFFFFFF) {
    //
    // P2SB has been hidden, read it from Pcd
    //
    P2sbMmioBar = PcdGet32 (PcdP2SBBaseAddress);
  } else {
    P2sbMmioBar &= B_P2SB_BAR_BA;
  }

  GlobalNvsArea->Area->GPIO0Addr = P2SB_MMIO_ADDR (P2sbMmioBar, NORTH, 0);
  GlobalNvsArea->Area->GPIO0Len = GPIO_NORTH_COMMUNITY_LENGTH;

  GlobalNvsArea->Area->GPIO1Addr = P2SB_MMIO_ADDR (P2sbMmioBar, NORTHWEST, 0);
  GlobalNvsArea->Area->GPIO1Len = GPIO_NORTHWEST_COMMUNITY_LENGTH;

  GlobalNvsArea->Area->GPIO2Addr = P2SB_MMIO_ADDR (P2sbMmioBar, WEST, 0);
  GlobalNvsArea->Area->GPIO2Len = GPIO_WEST_COMMUNITY_LENGTH;

  GlobalNvsArea->Area->GPIO3Addr = P2SB_MMIO_ADDR (P2sbMmioBar, SOUTHWEST, 0);
  GlobalNvsArea->Area->GPIO3Len = GPIO_SOUTHWEST_COMMUNITY_LENGTH;

  DEBUG ((DEBUG_INFO, "GPIOxAddr: North: 0x%X, Northwest: 0x%X, West: 0x%X, Southwest: 0x%X, South: 0x%X\n", \
    GlobalNvsArea->Area->GPIO0Addr,
    GlobalNvsArea->Area->GPIO1Addr,
    GlobalNvsArea->Area->GPIO2Addr,
    GlobalNvsArea->Area->GPIO3Addr,
    GlobalNvsArea->Area->GPIO4Addr
    ));

#ifdef SC_PM_ENABLE
  DEBUG ((DEBUG_INFO, "Start SC_PM_ENABLE for GPIO\n"));
  GpioWrite (NORTH,     R_GPIO_MISCCFG, GpioRead (NORTH    , R_GPIO_MISCCFG) | B_GPIP_MISCCFG);
  GpioWrite (NORTHWEST, R_GPIO_MISCCFG, GpioRead (NORTHWEST, R_GPIO_MISCCFG) | B_GPIP_MISCCFG);
  GpioWrite (WEST,      R_GPIO_MISCCFG, GpioRead (WEST     , R_GPIO_MISCCFG) | B_GPIP_MISCCFG);
  GpioWrite (SOUTHWEST, R_GPIO_MISCCFG, GpioRead (SOUTHWEST, R_GPIO_MISCCFG) | B_GPIP_MISCCFG);
  DEBUG ((DEBUG_INFO, "End SC_PM_ENABLE for GPIO\n"));
#endif

  DEBUG ((DEBUG_INFO, "ConfigureLpssAtBoot() End\n"));
  return EFI_SUCCESS;
}


/**
  Stop Lpss devices[I2C and SPI] if any is running.

  @retval EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
StopLpssAtBoot (
  VOID
  )
{
  UINTN                         LpssPciMmBase;
  UINT32                        LpssMmioBase0;
  UINT8                         Index;
  UINT32                        D32 = 0;

  DEBUG ((DEBUG_INFO, "Stop LPSS controllers if any controller is running(I2C and SPI)...\n"));
  for (Index = 0; Index < LPSS_IO_DEVICE_NUM; Index++) {
    LpssPciMmBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_SC,
                      mLpssIoDevices[Index].PciDeviceNum,
                      mLpssIoDevices[Index].PciFuncNum
                      );

    LpssMmioBase0 = MmioRead32 ((UINTN) (LpssPciMmBase + R_LPSS_IO_BAR));
    LpssMmioBase0 &= 0xFFFFFFF0;  // Mask off the last byte read from BAR

    if ((mLpssIoDevices[Index].PciDeviceNum  == PCI_DEVICE_NUMBER_LPSS_I2C0) || \
        (mLpssIoDevices[Index].PciDeviceNum  == PCI_DEVICE_NUMBER_LPSS_I2C1)) {
      D32 = MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_I2C_IC_ENABLE));
      if (D32 & B_LPSS_I2C_IC_ENABLE_ENABLE) {
        DEBUG ((DEBUG_INFO, "Stop I2C controller D:%02X, F: %02X...\n", mLpssIoDevices[Index].PciDeviceNum, mLpssIoDevices[Index].PciFuncNum));
        MmioWrite32 ((UINTN) LpssMmioBase0 + R_LPSS_I2C_IC_ENABLE, \
        MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_I2C_IC_ENABLE)) & (~(UINT32) B_LPSS_I2C_IC_ENABLE_ENABLE));
      }
    } else if (mLpssIoDevices[Index].PciDeviceNum  == PCI_DEVICE_NUMBER_LPSS_SPI) {
      D32 = MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_SPI_MEM_SSP_CONTROL0));
      if (D32 & B_LPSS_SPI_MEM_SSP_CONTROL0_SSE) {
        DEBUG ((DEBUG_INFO, "Stop SPI controller D:%02X, F: %02X...\n", mLpssIoDevices[Index].PciDeviceNum, mLpssIoDevices[Index].PciFuncNum));
        MmioWrite32 ((UINTN) LpssMmioBase0 + R_LPSS_SPI_MEM_SSP_CONTROL0, \
        MmioRead32 ((UINTN) (LpssMmioBase0 + R_LPSS_SPI_MEM_SSP_CONTROL0)) & (~(UINT32) B_LPSS_SPI_MEM_SSP_CONTROL0_SSE));
      }
    }
  }

  return EFI_SUCCESS;
}

