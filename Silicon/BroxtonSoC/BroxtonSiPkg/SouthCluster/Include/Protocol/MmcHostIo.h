/** @file
  Interface definition for EFI_MMC_HOST_IO_PROTOCOL.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MMC_HOST_IO_H
#define _MMC_HOST_IO_H

#define EFI_MMC_HOST_IO_PROTOCOL_GUID \
  { 0x88b7c251, 0xf4e6, 0x4a1e, { 0x90, 0xbb, 0xda, 0x8b, 0x06, 0xb7, 0xdc, 0xda } }

typedef struct _EFI_MMC_HOST_IO_PROTOCOL EFI_MMC_HOST_IO_PROTOCOL;

//
// @todo Move to Pci22.h
//
#define PCI_SUBCLASS_SD_HOST_CONTROLLER   0x05
#define PCI_IF_STANDARD_HOST_NO_DMA       0x00
#define PCI_IF_STANDARD_HOST_SUPPORT_DMA  0x01

//
// @todo Retire
//
#define EFI_MMC_HOST_IO_PROTOCOL_REVISION_01          0x01

//
// @todo Do these belong in an Industry Standard include file?
//
// MMIO Registers definition for MMC/SDIO controller
//
#define  MMIO_DMAADR                     0x00
#define  MMIO_BLKSZ                      0x04
#define  MMIO_BLKCNT                     0x06
#define  MMIO_CMDARG                     0x08
#define  MMIO_XFRMODE                    0x0C
#define  MMIO_SDCMD                      0x0E
#define  MMIO_RESP                       0x10
#define  MMIO_BUFDATA                    0x20
#define  MMIO_PSTATE                     0x24
#define  MMIO_HOSTCTL                    0x28
#define  MMIO_PWRCTL                     0x29
#define  MMIO_BLKGAPCTL                  0x2A
#define  MMIO_WAKECTL                    0x2B
#define  MMIO_CLKCTL                     0x2C
#define  MMIO_TOCTL                      0x2E
#define  MMIO_SWRST                      0x2F
#define  MMIO_NINTSTS                    0x30
#define  MMIO_ERINTSTS                   0x32
#define  MMIO_NINTEN                     0x34
#define  MMIO_ERINTEN                    0x36
#define  MMIO_NINTSIGEN                  0x38
#define  MMIO_ERINTSIGEN                 0x3A
#define  MMIO_AC12ERRSTS                 0x3C
#define  MMIO_HOST_CTL2                  0x3E
#define  MMIO_CAP                        0x40
#define  MMIO_CAP2                       0x44
#define  MMIO_MCCAP                      0x48
#define  MMIO_FORCEEVENTCMD12ERRSTAT     0x50
#define  MMIO_FORCEEVENTERRINTSTAT       0x52
#define  MMIO_ADMAERRSTAT                0x54
#define  MMIO_ADMASYSADDR                0x58
#define  MMIO_PRESETVALUE0               0x60
#define  MMIO_PRESETVALUE1               0x64
#define  MMIO_PRESETVALUE2               0x68
#define  MMIO_PRESETVALUE3               0x6C
#define  MMIO_BOOTTIMEOUTCTRL            0x70
#define  MMIO_DEBUGSEL                   0x74
#define  MMIO_SHAREDBUS                  0xE0
#define  MMIO_SPIINTSUP                  0xF0
#define  MMIO_SLTINTSTS                  0xFC
#define  MMIO_CTRLRVER                   0xFE

typedef enum {
  ResponseNo = 0,
  ResponseR1,
  ResponseR1b,
  ResponseR2,
  ResponseR3,
  ResponseR4,
  ResponseR5,
  ResponseR5b,
  ResponseR6,
  ResponseR7
} RESPONSE_TYPE;

typedef enum {
  NoData = 0,
  InData,
  OutData
} TRANSFER_TYPE;

typedef enum {
  Reset_Auto = 0,
  Reset_DAT,
  Reset_CMD,
  Reset_DAT_CMD,
  Reset_All,
  Reset_HW
} RESET_TYPE;

typedef enum {
  SDMA = 0,
  ADMA2,
  PIO
} DMA_MOD;

typedef struct {
  UINT32  HighSpeedSupport:    1;  ///< High speed supported
  UINT32  V18Support:          1;  ///< 1.8V supported
  UINT32  V30Support:          1;  ///< 3.0V supported
  UINT32  V33Support:          1;  ///< 3.3V supported
  UINT32  SDR50Support:        1;
  UINT32  SDR104Support:       1;
  UINT32  DDR50Support:        1;
  UINT32  HS400Support:        1;
  UINT32  BusWidth4:           1;  ///< 4 bit width
  UINT32  BusWidth8:           1;  ///< 8 bit width
  UINT32  Reserved1:           6;
  UINT32  SDMASupport:         1;
  UINT32  ADMA2Support:        1;
  UINT32  DmaMode:             2;
  UINT32  ReTuneTimer:         4;
  UINT32  ReTuneMode:          2;
  UINT32  Reserved2:           6;
  UINT32  BoundarySize;
} HOST_CAPABILITY;

/**
  The main function used to send the command to the card inserted into the MMC host
  slot.
  It will assemble the arguments to set the command register and wait for the command
  and transfer completed until timeout. Then it will read the response register to fill
  the ResponseData

  @param[in]  This            Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  CommandIndex    The command index to set the command index field of command register
  @param[in]  Argument        Command argument to set the argument field of command register
  @param[in]  DataType        TRANSFER_TYPE, indicates no data, data in or data out
  @param[in]  Buffer          Contains the data read from / write to the device
  @param[in]  BufferSize      The size of the buffer
  @param[in]  ResponseType    RESPONSE_TYPE
  @param[in]  TimeOut         Time out value in 1 ms unit
  @param[out] ResponseData    Depending on the ResponseType, such as CSD or card status

  @retval     EFI_SUCCESS
  @retval     EFI_INVALID_PARAMETER
  @retval     EFI_OUT_OF_RESOURCES
  @retval     EFI_TIMEOUT
  @retval     EFI_DEVICE_ERROR

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SEND_COMMAND) (
  IN   EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData OPTIONAL
  );

/**
  Set max clock frequency of the host, the actual frequency
  may not be the same as MaxFrequency. It depends on
  the max frequency the host can support, divider, and host
  speed mode.

  @param[in]  This            Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  MaxFrequency    Max frequency in HZ

  @retval     EFI_SUCCESS
  @retval     EFI_TIMEOUT

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_CLOCK_FREQUENCY) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     MaxFrequency
  );

/**
  Set bus width of the host.

  @param[in]  This        Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  BusWidth    Bus width in 1, 4, 8 bits

  @retval     EFI_SUCCESS
  @retval     EFI_INVALID_PARAMETER

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_BUS_WIDTH) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     BusWidth
  );

/**
  Set voltage which could supported by the host.
  Support 0(Power off the host), 1.8V, 3.0V, 3.3V

  @param[in]  This       Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Voltage    Units in 0.1 V

  @retval     EFI_SUCCESS
  @retval     EFI_INVALID_PARAMETER

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_VOLTAGE) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     Voltage
  );

/**
  Set Host High Speed.

  @param[in]  This       Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  HighSpeed  True for High Speed Mode set, false for normal mode

  @retval     EFI_SUCCESS
  @retval     EFI_INVALID_PARAMETER

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_SPEED_MODE) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     HighSpeed
  );

/**
  Set High Speed Mode.

  @param[in]  This             Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  SetHostDdrMode   True for DDR Mode set, false for normal mode

  @retval     EFI_SUCCESS
  @retval     EFI_INVALID_PARAMETER

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_DDR_MODE) (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     DdrMode
  );

/**
  Set Host SDR Mode.

  @param[in] This                    Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in] DdrMode                 True for SDR Mode set, false for normal mode

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   A parameter was incorrect.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_SDR_MODE) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     SdrMode
  );

/**
  Reset the host

  @param[in]  This      Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  ResetAll  TRUE to reset all

  @retval     EFI_SUCCESS
  @retval     EFI_TIMEOUT

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_RESET_MMC_HOST) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  RESET_TYPE                 ResetType
  );

/**
  Reset the host.

  @param[in]  This     Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  Enable   TRUE to enable, FALSE to disable

  @retval     EFI_SUCCESS
  @retval     EFI_TIMEOUT

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_ENABLE_AUTO_STOP_CMD) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  BOOLEAN                    Enable
  );

/**
  Find whether these is a card inserted into the slot. If so
  init the host. If not, return EFI_NOT_FOUND.

  @param[in]  This       Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     EFI_SUCCESS
  @retval     EFI_NOT_FOUND

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_DETECT_CARD_AND_INIT_HOST) (
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This
  );

/**
  Set the Block length.

  @param[in]  This         Pointer to EFI_MMC_HOST_IO_PROTOCOL
  @param[in]  BlockLength  card supports block length

  @retval     EFI_SUCCESS
  @retval     EFI_TIMEOUT

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SET_BLOCK_LENGTH) (
  IN  EFI_MMC_HOST_IO_PROTOCOL   *This,
  IN  UINT32                     BlockLength
  );

/**
  @param[in]  This         Pointer to EFI_MMC_HOST_IO_PROTOCOL

  @retval     EFI_SUCCESS
  @retval     EFI_TIMEOUT

**/
typedef EFI_STATUS
(EFIAPI *EFI_MMC_HOST_IO_PROTOCOL_SETUP_DEVICE)(
  IN  EFI_MMC_HOST_IO_PROTOCOL    *This
  );

/**
  Interface structure for the EFI MMC Host I/O Protocol
**/
struct _EFI_MMC_HOST_IO_PROTOCOL {
  UINT32                                              Revision;
  HOST_CAPABILITY                                     HostCapability;
  EFI_MMC_HOST_IO_PROTOCOL_SEND_COMMAND               SendCommand;
  EFI_MMC_HOST_IO_PROTOCOL_SET_CLOCK_FREQUENCY        SetClockFrequency;
  EFI_MMC_HOST_IO_PROTOCOL_SET_BUS_WIDTH              SetBusWidth;
  EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_VOLTAGE           SetHostVoltage;
  EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_DDR_MODE          SetHostDdrMode;
  EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_SDR_MODE          SetHostSdrMode;
  EFI_MMC_HOST_IO_PROTOCOL_RESET_MMC_HOST             ResetMmcHost;
  EFI_MMC_HOST_IO_PROTOCOL_ENABLE_AUTO_STOP_CMD       EnableAutoStopCmd;
  EFI_MMC_HOST_IO_PROTOCOL_DETECT_CARD_AND_INIT_HOST  DetectCardAndInitHost;
  EFI_MMC_HOST_IO_PROTOCOL_SET_BLOCK_LENGTH           SetBlockLength;
  EFI_MMC_HOST_IO_PROTOCOL_SETUP_DEVICE               SetupDevice;
  EFI_MMC_HOST_IO_PROTOCOL_SET_HOST_SPEED_MODE        SetHostSpeedMode;
};

extern EFI_GUID gEfiMmcHostIoProtocolGuid;

#endif

