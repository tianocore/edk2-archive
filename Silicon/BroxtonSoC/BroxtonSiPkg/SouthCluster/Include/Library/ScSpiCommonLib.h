/** @file
  Header file for the SC SPI Common Driver.

  Copyright (c) 2008 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SPI_COMMON_LIB_H_
#define _SC_SPI_COMMON_LIB_H_

///
/// Maximum time allowed while waiting the SPI cycle to complete
/// Wait Time = 6 seconds = 6000000 microseconds
/// Wait Period = 10 microseconds
///
#define WAIT_TIME   6000000     ///< Wait Time = 6 seconds = 6000000 microseconds
#define WAIT_PERIOD 10          ///< Wait Period = 10 microseconds

///
/// Flash cycle Type
///
typedef enum {
  FlashCycleRead,
  FlashCycleWrite,
  FlashCycleErase,
  FlashCycleReadSfdp,
  FlashCycleReadJedecId,
  FlashCycleWriteStatus,
  FlashCycleReadStatus,
  FlashCycleMax
} FLASH_CYCLE_TYPE;

///
/// Flash Component Number
///
typedef enum {
  FlashComponent0,
  FlashComponent1,
  FlashComponentMax
} FLASH_COMPONENT_NUM;

///
/// Private data structure definitions for the driver
///
#define SC_SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('P', 'S', 'P', 'I')

typedef struct {
  UINTN                 Signature;
  EFI_HANDLE            Handle;
  SC_SPI_PROTOCOL       SpiProtocol;
  UINT16                PchAcpiBase;
  UINTN                 PchSpiBase;
  UINT16                RegionPermission;
  UINT32                SfdpVscc0Value;
  UINT32                SfdpVscc1Value;
  UINT32                StrapBaseAddress;
  UINT8                 NumberOfComponents;
  UINT32                Component1StartAddr;
} SPI_INSTANCE;

#define SPI_INSTANCE_FROM_SPIPROTOCOL(a)  CR (a, SPI_INSTANCE, SpiProtocol, SC_SPI_PRIVATE_DATA_SIGNATURE)

///
/// Function prototypes used by the SPI protocol.
///
/**
  Initialize an SPI protocol instance.

  @param[in] SpiInstance             Pointer to SpiInstance to initialize

  @retval    EFI_SUCCESS             The protocol instance was properly initialized
  @retval    EFI_UNSUPPORTED         The SC is not supported by this module

**/
EFI_STATUS
SpiProtocolConstructor (
  IN     SPI_INSTANCE       *SpiInstance
  );

/**
  This function is a hook for Spi to disable BIOS Write Protect.

  @retval   EFI_SUCCESS             The protocol instance was properly initialized
  @retval   EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  );

/**
  This function is a hook for Spi to enable BIOS Write Protect.

**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  );

/**
  Acquire SC spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval    UINT32               Return SPI MMIO address

**/
UINT32
AcquireSpiBar0 (
  IN  SPI_INSTANCE                *SpiInstance
  );

/**
  Release SC spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

**/
VOID
ReleaseSpiBar0 (
  IN  SPI_INSTANCE                *SpiInstance
  );

/**
  Read data from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  FlashRegionType         The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in]  Address                 The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in]  ByteCount               Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer                  The Pointer to caller-allocated buffer containing the data received.
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashRead (
  IN     SC_SPI_PROTOCOL   *This,
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  );

/**
  Write data to the flash part.

  @param[in] This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in] FlashRegionType         The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address                 The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount               Number of bytes in the data portion of the SPI cycle.
  @param[in] Buffer                  Pointer to caller-allocated buffer containing the data sent during the SPI cycle.

  @retval    EFI_SUCCESS             Command succeed.
  @retval    EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval    EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashWrite (
  IN     SC_SPI_PROTOCOL   *This,
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN     UINT8              *Buffer
  );

/**
  Erase some area on the flash part.

  @param[in] This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in] FlashRegionType         The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address                 The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount               Number of bytes in the data portion of the SPI cycle.

  @retval    EFI_SUCCESS             Command succeed.
  @retval    EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval    EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashErase (
  IN     SC_SPI_PROTOCOL   *This,
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount
  );

/**
  Read SFDP data from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  ComponentNumber         The Component Number for chip select
  @param[in]  ByteCount               Number of bytes in SFDP data portion of the SPI cycle, the max number is 64
  @param[out] SfdpData                The Pointer to caller-allocated buffer containing the SFDP data received
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashReadSfdp (
  IN     SC_SPI_PROTOCOL   *This,
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SfdpData
  );

/**
  Read Jedec Id from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  ComponentNumber         The Component Number for chip select
  @param[in]  ByteCount               Number of bytes in JedecId data portion of the SPI cycle, the data size is 3 typically
  @param[out] JedecId                 The Pointer to caller-allocated buffer containing JEDEC ID received
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashReadJedecId (
  IN     SC_SPI_PROTOCOL    *This,
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *JedecId
  );

/**
  Write the status register in the flash part.

  @param[in] This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in] ByteCount               Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[in] StatusValue             The Pointer to caller-allocated buffer containing the value of Status register writing

  @retval    EFI_SUCCESS             Command succeed.
  @retval    EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval    EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashWriteStatus (
  IN     SC_SPI_PROTOCOL    *This,
  IN     UINT32             ByteCount,
  IN     UINT8              *StatusValue
  );

/**
  Read status register in the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  ByteCount               Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[out] StatusValue             The Pointer to caller-allocated buffer containing the value of Status register received.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolFlashReadStatus (
  IN     SC_SPI_PROTOCOL    *This,
  IN     UINT32             ByteCount,
  OUT    UINT8              *StatusValue
  );

/**
  Read SC Soft Strap Values.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  SoftStrapAddr           SC Soft Strap address offset from FPSBA.
  @param[in]  ByteCount               Number of bytes in SoftStrap data portion of the SPI cycle
  @param[out] SoftStrapValue          The Pointer to caller-allocated buffer containing SC Soft Strap Value.
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
EFI_STATUS
EFIAPI
SpiProtocolReadPchSoftStrap (
  IN     SC_SPI_PROTOCOL    *This,
  IN     UINT32             SoftStrapAddr,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SoftStrapValue
  );

/**
  This function sends the programmed SPI command to the slave device.

  @param[in]      This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]      SpiRegionType           The SPI Region type for flash cycle which is listed in the Descriptor
  @param[in]      FlashCycleType          The Flash SPI cycle type list in HSFC (Hardware Sequencing Flash Control Register) register
  @param[in]      Address                 The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in]      ByteCount               Number of bytes in the data portion of the SPI cycle.
  @param[in, out] Buffer                  Pointer to caller-allocated buffer containing the data received or sent during the SPI cycle.

  @retval         EFI_SUCCESS             SPI command completes successfully.
  @retval         EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
  @retval         EFI_ACCESS_DENIED       Some unrecognized command encountered in hardware sequencing mode
  @retval         EFI_INVALID_PARAMETER   The parameters specified are not valid.

**/
EFI_STATUS
SendSpiCmd (
  IN     SC_SPI_PROTOCOL    *This,
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     FLASH_CYCLE_TYPE   FlashCycleType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN OUT UINT8              *Buffer
  );

/**
  Wait execution cycle to complete on the SPI interface.

  @param[in] This                 The SPI protocol instance
  @param[in] PchSpiBar0           Spi MMIO base address
  @param[in] ErrorCheck           TRUE if the SpiCycle needs to do the error check

  @retval    TRUE                 SPI cycle completed on the interface.
  @retval    FALSE                Time out while waiting the SPI cycle to complete.
                                  It's not safe to program the next command on the SPI interface.

**/
BOOLEAN
WaitForSpiCycleComplete (
  IN     SC_SPI_PROTOCOL    *This,
  IN     UINT32             PchSpiBar0,
  IN     BOOLEAN            ErrorCheck
  );

#endif

