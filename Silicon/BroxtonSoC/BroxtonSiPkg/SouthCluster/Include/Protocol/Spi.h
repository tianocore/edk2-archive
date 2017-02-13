/** @file
  This file defines the EFI SPI Protocol which implements the
  Intel(R) ICH SPI Host Controller Compatibility Interface.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SPI_PROTOCOL_H_
#define _SC_SPI_PROTOCOL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gScSpiProtocolGuid;
extern EFI_GUID                       gScSmmSpiProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _SC_SPI_PROTOCOL  SC_SPI_PROTOCOL;

//
// SPI protocol data structures and definitions
//

///
/// Flash Region Type
///
typedef enum {
  FlashRegionDescriptor,
  FlashRegionBios,
  FlashRegionMe,
  FlashRegionGbE,
  FlashRegionPlatformData,
  FlashRegionAll,
  FlashRegionMax
} FLASH_REGION_TYPE;


///
/// SPI protocol data structures and definitions
///
///
/// Number of Prefix Opcodes allowed on the SPI interface
///
#define SPI_NUM_PREFIX_OPCODE 2

///
/// Number of Opcodes in the Opcode Menu
///
#define SPI_NUM_OPCODE  8

///
/// Opcode Type
///   EnumSpiOpcodeCommand: Command without address
///   EnumSpiOpcodeRead: Read with address
///   EnumSpiOpcodeWrite: Write with address
///
typedef enum {
  EnumSpiOpcodeReadNoAddr,
  EnumSpiOpcodeWriteNoAddr,
  EnumSpiOpcodeRead,
  EnumSpiOpcodeWrite,
  EnumSpiOpcodeMax
} SPI_OPCODE_TYPE;

typedef enum {
  EnumSpiCycle20MHz,
  EnumSpiCycle33MHz,
  EnumSpiCycle66MHz,  /// Not supported by BXT
  EnumSpiCycle50MHz,
  EnumSpiCycleMax
} SPI_CYCLE_FREQUENCY;

///
/// Hardware Sequencing required operations (as listed in Broxton EDS "Hardware
/// Sequencing Commands and Opcode Requirements"
///
typedef enum {
  EnumSpiOperationWriteStatus,
  EnumSpiOperationProgramData_1_Byte,
  EnumSpiOperationProgramData_64_Byte,
  EnumSpiOperationReadData,
  EnumSpiOperationWriteDisable,
  EnumSpiOperationReadStatus,
  EnumSpiOperationWriteEnable,
  EnumSpiOperationFastRead,
  EnumSpiOperationEnableWriteStatus,
  EnumSpiOperationErase_256_Byte,
  EnumSpiOperationErase_4K_Byte,
  EnumSpiOperationErase_8K_Byte,
  EnumSpiOperationErase_64K_Byte,
  EnumSpiOperationFullChipErase,
  EnumSpiOperationJedecId,
  EnumSpiOperationDualOutputFastRead,
  EnumSpiOperationDiscoveryParameters,
  EnumSpiOperationOther,
  EnumSpiOperationMax
} SPI_OPERATION;

///
/// SPI Command Configuration
///   Frequency       The expected frequency to be used (value to be programmed to the SSFC
///                   Register)
///   Operation       Which Hardware Sequencing required operation this opcode respoinds to.
///                   The required operations are listed in EDS Table 5-55: "Hardware
///                   Sequencing Commands and Opcode Requirements"
///                   If the opcode does not corresponds to any operation listed, use
///                   EnumSpiOperationOther, and provides TYPE and Code for it in
///                   SpecialOpcodeEntry.
///
typedef struct _SPI_OPCODE_MENU_ENTRY {
  SPI_OPCODE_TYPE     Type;
  UINT8               Code;
  SPI_CYCLE_FREQUENCY Frequency;
  SPI_OPERATION       Operation;
} SPI_OPCODE_MENU_ENTRY;

//
// Initialization data table loaded to the SPI host controller
//    VendorId        Vendor ID of the SPI device
//    DeviceId0       Device ID0 of the SPI device
//    DeviceId1       Device ID1 of the SPI device
//    PrefixOpcode    Prefix opcodes which are loaded into the SPI host controller
//    OpcodeMenu      Opcodes which are loaded into the SPI host controller Opcode Menu
//    BiosStartOffset The offset of the start of the BIOS image relative to the flash device.
//                    Please note this is a Flash Linear Address, NOT a memory space address.
//                    This value is platform specific and depends on the system flash map.
//                    This value is only used on non Descriptor mode.
//    BiosSize        The the BIOS Image size in flash. This value is platform specific
//                    and depends on the system flash map. Please note BIOS Image size may
//                    be smaller than BIOS Region size (in Descriptor Mode) or the flash size
//                    (in Non Descriptor Mode), and in this case, BIOS Image is supposed to be
//                    placed at the top end of the BIOS Region (in Descriptor Mode) or the flash
//                    (in Non Descriptor Mode)
//
typedef struct _SPI_INIT_TABLE {
  UINT8                 VendorId;
  UINT8                 DeviceId0;
  UINT8                 DeviceId1;
  UINT8                 PrefixOpcode[SPI_NUM_PREFIX_OPCODE];
  SPI_OPCODE_MENU_ENTRY OpcodeMenu[SPI_NUM_OPCODE];
  UINTN                 BiosStartOffset;
  UINTN                 BiosSize;
} SPI_INIT_TABLE;

//
// Protocol member functions
//
/**
  Read data from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  FlashRegionType         The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in]  Address                 The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in]  ByteCount               Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer                  The Pointer to caller-allocated buffer containing the dada received.
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_READ) (
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
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_WRITE) (
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
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_ERASE) (
  IN     SC_SPI_PROTOCOL   *This,
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount
  );

/**
  Read SFDP data from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  ComponentNumber         The Componen Number for chip select
  @param[in]  ByteCount               Number of bytes in SFDP data portion of the SPI cycle, the max number is 64
  @param[out] SfdpData                The Pointer to caller-allocated buffer containing the SFDP data received
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_READ_SFDP) (
  IN     SC_SPI_PROTOCOL   *This,
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SfdpData
  );

/**
  Read Jedec Id from the flash part.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  ComponentNumber         The Componen Number for chip select
  @param[in]  ByteCount               Number of bytes in JedecId data portion of the SPI cycle, the data size is 3 typically
  @param[out] JedecId                 The Pointer to caller-allocated buffer containing JEDEC ID received
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_READ_JEDEC_ID) (
  IN     SC_SPI_PROTOCOL   *This,
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
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_WRITE_STATUS) (
  IN     SC_SPI_PROTOCOL   *This,
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
typedef
EFI_STATUS
(EFIAPI *SC_SPI_FLASH_READ_STATUS) (
  IN     SC_SPI_PROTOCOL   *This,
  IN     UINT32             ByteCount,
  OUT    UINT8              *StatusValue
  );

/**
  Read PCH Soft Strap Values.

  @param[in]  This                    Pointer to the SC_SPI_PROTOCOL instance.
  @param[in]  SoftStrapAddr           PCH Soft Strap address offset from FPSBA.
  @param[in]  ByteCount               Number of bytes in SoftStrap data portion of the SPI cycle
  @param[out] SoftStrapValue          The Pointer to caller-allocated buffer containing PCH Soft Strap Value.
                                      It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval     EFI_SUCCESS             Command succeed.
  @retval     EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval     EFI_DEVICE_ERROR        Device error, command aborts abnormally.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SPI_READ_PCH_SOFTSTRAP) (
  IN     SC_SPI_PROTOCOL   *This,
  IN     UINT32             SoftStrapAddr,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SoftStrapValue
  );

/**
  The SC SPI protocol implement the South Cluster SPI Host Controller Interface
  for DXE, SMM, and PEI environments, respectively.

  These protocols/PPI allows a platform module to perform SPI operations through the
  Intel PCH SPI Host Controller Interface.

**/
struct _SC_SPI_PROTOCOL {
  SC_SPI_FLASH_READ                FlashRead;          ///< Read data from the flash part.
  SC_SPI_FLASH_WRITE               FlashWrite;         ///< Write data to the flash part.
  SC_SPI_FLASH_ERASE               FlashErase;         ///< Erase some area on the flash part.
  SC_SPI_FLASH_READ_SFDP           FlashReadSfdp;      ///< Read SFDP data from the flash part.
  SC_SPI_FLASH_READ_JEDEC_ID       FlashReadJedecId;   ///< Read Jedec Id from the flash part.
  SC_SPI_FLASH_WRITE_STATUS        FlashWriteStatus;   ///< Write the status register in the flash part.
  SC_SPI_FLASH_READ_STATUS         FlashReadStatus;    ///< Read status register in the flash part.
  SC_SPI_READ_PCH_SOFTSTRAP        ReadPchSoftStrap;   ///< Read PCH Soft Strap Values
};

#endif

