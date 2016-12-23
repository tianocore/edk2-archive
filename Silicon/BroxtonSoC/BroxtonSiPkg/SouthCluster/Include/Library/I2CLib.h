/** @file
  Register Definitions for I2C Library.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_LIB_H_
#define _I2C_LIB_H_

#include <Uefi.h>
#include <Library/IoLib.h>

//
// FIFO write workaround value.
//
#define FIFO_WRITE_DELAY    2

//
// MMIO Register Definitions
//
#define    R_IC_CON                          (0x00)  ///< I2C Control
#define    B_IC_RESTART_EN                  BIT5
#define    B_IC_SLAVE_DISABLE               BIT6
#define    V_SPEED_STANDARD                 0x02
#define    V_SPEED_FAST                     0x04
#define    V_SPEED_HIGH                     0x06
#define    B_MASTER_MODE                    BIT0

#define    R_IC_TAR                         (0x04)  ///< I2C Target Address
#define    IC_TAR_10BITADDR_MASTER          BIT12

#define    R_IC_SAR                         (0x08)  ///< I2C Slave Address
#define    R_IC_HS_MADDR                    (0x0C)  ///< I2C HS MasterMode Code Address
#define    R_IC_DATA_CMD                    (0x10)  ///< I2C Rx/Tx Data Buffer and Command

#define    B_READ_CMD                        BIT8   ///< 1 = read, 0 = write
#define    B_CMD_STOP                        BIT9   ///< 1 = STOP
#define    B_CMD_RESTART                     BIT10  ///< 1 = IC_RESTART_EN

#define    V_WRITE_CMD_MASK                  (0xFF)

#define    R_IC_SS_SCL_HCNT                  (0x14)  ///< Standard Speed I2C Clock SCL High Count
#define    R_IC_SS_SCL_LCNT                  (0x18)  ///< Standard Speed I2C Clock SCL Low Count
#define    R_IC_FS_SCL_HCNT                  (0x1C)  ///< Full Speed I2C Clock SCL High Count
#define    R_IC_FS_SCL_LCNT                  (0x20)  ///< Full Speed I2C Clock SCL Low Count
#define    R_IC_HS_SCL_HCNT                  (0x24)  ///< High Speed I2C Clock SCL High Count
#define    R_IC_HS_SCL_LCNT                  (0x28)  ///< High Speed I2C Clock SCL Low Count
#define    R_IC_INTR_STAT                    (0x2C)  ///< I2C Inetrrupt Status
#define    R_IC_INTR_MASK                    (0x30)  ///< I2C Interrupt Mask
#define    I2C_INTR_GEN_CALL                 BIT11   ///< General call received
#define    I2C_INTR_START_DET                BIT10
#define    I2C_INTR_STOP_DET                 BIT9
#define    I2C_INTR_ACTIVITY                 BIT8
#define    I2C_INTR_TX_ABRT                  BIT6    ///< Set on NACK
#define    I2C_INTR_TX_EMPTY                 BIT4
#define    I2C_INTR_TX_OVER                  BIT3
#define    I2C_INTR_RX_FULL                  BIT2    ///< Data bytes in RX FIFO over threshold
#define    I2C_INTR_RX_OVER                  BIT1
#define    I2C_INTR_RX_UNDER                 BIT0
#define    R_IC_RAW_INTR_STAT                (0x34)  ///< I2C Raw Interrupt Status
#define    R_IC_RX_TL                        (0x38)  ///< I2C Receive FIFO Threshold
#define    R_IC_TX_TL                        (0x3C)  ///< I2C Transmit FIFO Threshold
#define    R_IC_CLR_INTR                     (0x40)  ///< Clear Combined and Individual Interrupts
#define    R_IC_CLR_RX_UNDER                 (0x44)  ///< Clear RX_UNDER Interrupt
#define    R_IC_CLR_RX_OVER                  (0x48)  ///< Clear RX_OVERinterrupt
#define    R_IC_CLR_TX_OVER                  (0x4C)  ///< Clear TX_OVER interrupt
#define    R_IC_CLR_RD_REQ                   (0x50)  ///< Clear RD_REQ interrupt
#define    R_IC_CLR_TX_ABRT                  (0x54)  ///< Clear TX_ABRT interrupt
#define    R_IC_CLR_RX_DONE                  (0x58)  ///< Clear RX_DONE interrupt
#define    R_IC_CLR_ACTIVITY                 (0x5C)  ///< Clear ACTIVITY interrupt
#define    R_IC_CLR_STOP_DET                 (0x60)  ///< Clear STOP_DET interrupt
#define    R_IC_CLR_START_DET                (0x64)  ///< Clear START_DET interrupt
#define    R_IC_CLR_GEN_CALL                 (0x68)  ///< Clear GEN_CALL interrupt
#define    R_IC_ENABLE                       (0x6C)  ///< I2C Enable
#define    R_IC_STATUS                       (0x70)  ///< I2C Status

#define    R_IC_SDA_HOLD                     (0x7C)  ///< I2C IC_DEFAULT_SDA_HOLD//16bits

#define    STAT_MST_ACTIVITY                 BIT5   ///< Master FSM Activity Status.
#define    STAT_RFF                          BIT4   ///< RX FIFO is completely full
#define    STAT_RFNE                         BIT3   ///< RX FIFO is not empty
#define    STAT_TFE                          BIT2   ///< TX FIFO is completely empty
#define    STAT_TFNF                         BIT1   ///< TX FIFO is not full

#define    R_IC_TXFLR                        (0x74)  ///< Transmit FIFO Level Register
#define    R_IC_RXFLR                        (0x78)  ///< Receive FIFO Level Register
#define    R_IC_TX_ABRT_SOURCE               (0x80)  ///< I2C Transmit Abort Status Register
#define    R_IC_SLV_DATA_NACK_ONLY           (0x84)  ///< Generate SLV_DATA_NACK Register
#define    R_IC_DMA_CR                       (0x88)  ///< DMA Control Register
#define    R_IC_DMA_TDLR                     (0x8C)  ///< DMA Transmit Data Level
#define    R_IC_DMA_RDLR                     (0x90)  ///< DMA Receive Data Level
#define    R_IC_SDA_SETUP                    (0x94)  ///< I2C SDA Setup Register
#define    R_IC_ACK_GENERAL_CALL             (0x98)  ///< I2C ACK General Call Register
#define    R_IC_ENABLE_STATUS                (0x9C)  ///< I2C Enable Status Register
#define    R_IC_COMP_PARAM                   (0xF4)  ///< Component Parameter Register
#define    R_IC_COMP_VERSION                 (0xF8)  ///< Component Version ID
#define    R_IC_COMP_TYPE                    (0xFC)  ///< Component Type

#define    R_IC_CLK_GATE                     (0xC0)  ///< Clock Gate

#define     IC_TAR_10BITADDR_MASTER           BIT12
#define     FIFO_SIZE                         32


/**
  Program LPSS I2C PCI controller's BAR0 and enable memory decode.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected

  @retval     EFI_SUCCESS           I2C controller's BAR0 is programmed and memory decode enabled.
  @retval     EFI_NOT_READY         I2C controller's is not exist or its function has been disabled.

**/
EFI_STATUS
ProgramPciLpssI2C (
  IN UINT8        BusNo
  );

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device
  @param[in]  ReadBytes             Number of bytes to be read
  @param[out] ReadBuffer            Address to which the value read has to be stored
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value read successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to read a byte via I2C

**/
EFI_STATUS
ByteReadI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device
  @param[in]  WriteBytes            Number of bytes to be written
  @param[in]  WriteBuffer           Address to which the byte value has to be written
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value written successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to write a byte via I2C

**/
EFI_STATUS
ByteWriteI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Read bytes from I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  ReadBytes           Number of bytes to be read
  @param[out] ReadBuffer          Address to which the value read has to be stored

  @retval     EFI_SUCCESS         Read bytes from I2C device successfully
  @retval     Others              Return status depends on ByteReadI2C_Basic

**/
EFI_STATUS
ByteReadI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer
  );

/**
  Write bytes to I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  WriteBytes          Number of bytes to be written
  @param[in]  WriteBuffer         Address to which the byte value has to be written

  @retval     EFI_SUCCESS         Write bytes to I2C device successfully
  @retval     Others              Return status depends on ByteWriteI2C_Basic

**/
EFI_STATUS
ByteWriteI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer
  );

#endif  // _I2C_LIB_H_

