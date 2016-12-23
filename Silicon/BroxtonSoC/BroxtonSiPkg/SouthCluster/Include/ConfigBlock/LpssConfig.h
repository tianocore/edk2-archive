/** @file
  LPSS policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _LPSS_CONFIG_H_
#define _LPSS_CONFIG_H_

#define LPSS_CONFIG_REVISION 2

extern EFI_GUID gLpssConfigGuid;

#pragma pack (push,1)

#define  LPSS_I2C_DEVICE_NUM     8
#define  LPSS_HSUART_DEVICE_NUM  4
#define  LPSS_SPI_DEVICE_NUM     3

/**
  The SC_LPSS_CONFIG block describes Low Power Sub System (LPSS) settings for SC.
  @note: the order defined below is per the PCI BDF sequence, and MUST not change.
  Items defined will be accessed by its index in ScInit module

**/
typedef struct {
  /**
    Revision 1: Init version
    Revision 2: Deprecated ExiEnable
  **/
  CONFIG_BLOCK_HEADER Header;           ///< Config Block Header
  /**
    Determine if I2C_n is enabled.
    0: Disabled; <b>1: PCI Mode</b>; 2: ACPI Mode;
  **/
  SC_DEV_MODE         I2c0Enable;
  SC_DEV_MODE         I2c1Enable;
  SC_DEV_MODE         I2c2Enable;
  SC_DEV_MODE         I2c3Enable;
  SC_DEV_MODE         I2c4Enable;
  SC_DEV_MODE         I2c5Enable;
  SC_DEV_MODE         I2c6Enable;
  SC_DEV_MODE         I2c7Enable;
  /**
    Determine if UART_n is enabled.
    0: Disabled; <b>1: PCI Mode</b>; 2: ACPI Mode;
  **/
  SC_DEV_MODE         Hsuart0Enable;
  SC_DEV_MODE         Hsuart1Enable;
  SC_DEV_MODE         Hsuart2Enable;
  SC_DEV_MODE         Hsuart3Enable;
  /**
    Determine if SPI_n is enabled.
    0: Disabled; <b>1: PCI Mode</b>; 2: ACPI Mode;
  **/
  SC_DEV_MODE         Spi0Enable;
  SC_DEV_MODE         Spi1Enable;
  SC_DEV_MODE         Spi2Enable;
  UINT32              Uart2KernelDebugBaseAddress;
  UINT8               I2cClkGateCfg[LPSS_I2C_DEVICE_NUM];
  UINT8               HsuartClkGateCfg[LPSS_HSUART_DEVICE_NUM];
  UINT8               SpiClkGateCfg[LPSS_SPI_DEVICE_NUM];
  UINT32              S0ixEnable  : 1;  ///< <b>0: Disabled</b>; 1: Enabled
  UINT32              OsDbgEnable : 1;  ///< <b>0: Disabled</b>; 1: Enabled
  UINT32              ExiEnable   : 1;  ///< @deprecated, please use DciEn from SC_DCI_CONFIG
  UINT32              RsvdBits    : 29;
} SC_LPSS_CONFIG;

#pragma pack (pop)

#endif // _LPSS_CONFIG_H_

