/** @file
  Header file for PCH Serial IO Lib implementation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_SERIAL_IO_LIB_H_
#define _PCH_SERIAL_IO_LIB_H_

typedef enum {
  PchSerialIoIndexUart0,
  PchSerialIoIndexUart1,
  PchSerialIoIndexUart2,
  PchSerialIoIndexUart3,
  PchSerialIoIndexMax
} PCH_SERIAL_IO_CONTROLLER;

typedef enum {
  PchSerialIoDisabled,
  PchSerialIoAcpi,
  PchSerialIoPci,
  PchSerialIoAcpiHidden,
  PchSerialIoLegacyUart
} PCH_SERIAL_IO_MODE;

enum PCH_LP_SERIAL_IO_VOLTAGE_SEL {
  PchSerialIoIs33V = 0,
  PchSerialIoIs18V
};
enum PCH_LP_SERIAL_IO_CS_POLARITY {
  PchSerialIoCsActiveLow = 0,
  PchSerialIoCsActiveHigh = 1
};
enum PCH_LP_SERIAL_IO_HW_FLOW_CTRL {
  PchSerialIoHwFlowCtrlDisabled = 0,
  PchSerialIoHwFlowControlEnabled = 1
};

#define SERIALIO_HID_LENGTH 8 // including null terminator
#define SERIALIO_UID_LENGTH 1
#define SERIALIO_CID_LENGTH 1
#define SERIALIO_TOTAL_ID_LENGTH SERIALIO_HID_LENGTH+SERIALIO_UID_LENGTH+SERIALIO_CID_LENGTH

/**
  Returns index of the last i2c controller.

  @retval Value           Index of I2C controller

**/
PCH_SERIAL_IO_CONTROLLER
GetMaxI2cNumber (
  VOID
  );

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
  );

#if 0
/**
  Initializes GPIO pins used by SerialIo I2C devices.

  @param[in] Controller                 0=I2C0, ..., 11=UART2
  @param[in] DeviceMode                 Different type of serial io mode defined in PCH_SERIAL_IO_MODE
  @param[in] I2cVoltage                 Select I2C voltage, 1.8V or 3.3V

**/
VOID
SerialIoI2cGpioInit (
  IN PCH_SERIAL_IO_CONTROLLER Controller,
  IN PCH_SERIAL_IO_MODE       DeviceMode,
  IN UINT32                   I2cVoltage
  );

/**
  Initializes GPIO pins used by SerialIo SPI devices.

  @param[in] Controller                 0=I2C0, ..., 11=UART2
  @param[in] DeviceMode                 Different type of serial io mode defined in PCH_SERIAL_IO_MODE
  @param[in] SpiCsPolarity              SPI CS polarity

**/
VOID
SerialIoSpiGpioInit (
  IN PCH_SERIAL_IO_CONTROLLER Controller,
  IN PCH_SERIAL_IO_MODE       DeviceMode,
  IN UINT32                   SpiCsPolarity
  );

/**
  Initializes GPIO pins used by SerialIo devices.

  @param[in] Controller                 0=I2C0, ..., 11=UART2
  @param[in] DeviceMode                 Different type of serial io mode defined in PCH_SERIAL_IO_MODE
  @param[in] HardwareFlowControl        Hardware flow control method

**/
VOID
SerialIoUartGpioInit (
  IN PCH_SERIAL_IO_CONTROLLER Controller,
  IN PCH_SERIAL_IO_MODE       DeviceMode,
  IN BOOLEAN                  HardwareFlowControl
  );
#endif

/**
  Finds PCI Device Number of SerialIo devices.
  SerialIo devices' BDF is configurable.

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2

  @retval UINT8                         SerialIo device number

**/
UINT8
GetSerialIoDeviceNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  );

/**
  Finds PCI Function Number of SerialIo devices.
  SerialIo devices' BDF is configurable.

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2

  @retval UINT8                         SerialIo funciton number

**/
UINT8
GetSerialIoFunctionNumber (
  IN PCH_SERIAL_IO_CONTROLLER  SerialIoNumber
  );

/**
  Finds BAR values of SerialIo devices.
  SerialIo devices can be configured to not appear on PCI so traditional method of reading BAR might not work.
  If the SerialIo device is in PCI mode, a request for BAR1 will return its PCI CFG space instead.

  @param[in] SerialIoDevice             0=I2C0, ..., 11=UART2
  @param[in] BarNumber                  0=BAR0, 1=BAR1

  @retval UINTN                         SerialIo Bar value

**/
UINTN
FindSerialIoBar (
  IN PCH_SERIAL_IO_CONTROLLER           SerialIoDevice,
  IN UINT8                              BarNumber
  );

#endif // _PEI_DXE_SMM_PCH_SERIAL_IO_LIB_H_

