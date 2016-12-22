/** @file
  Describes the functions visible to the rest of the BIOS Guard.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BIOSGUARD_DEFINITIONS_H_
#define _BIOSGUARD_DEFINITIONS_H_

#ifndef ALIGN_256KB
#define ALIGN_256KB    0x00040000
#endif
#ifndef EFI_PAGE_SIZE
#define EFI_PAGE_SIZE  0x00001000
#endif

///
/// SPI component size selection
///
typedef enum {
  EnumSpiCompSize512KB  = 0,
  EnumSpiCompSize1MB,
  EnumSpiCompSize2MB,
  EnumSpiCompSize4MB,
  EnumSpiCompSize8MB,
  EnumSpiCompSize16MB,
  EnumSpiCompSize32MB,
  EnumSpiCompSize64MB,
  EnumSpiCompSize128MB
} SPI_COMPONENT_SIZE;

#define MIN_SFAM_COUNT  1
#define MAX_SFAM_COUNT  64

///
/// Signed Flash map descriptor definition.
///
typedef struct {
  UINT32 FirstByte; ///< Linear flash address of the first byte of the signed range, must be aligned to be first byte in the block. Ordering is little-endian.
  UINT32 LastByte;  ///< Linear flash address of the last byte of the signed range, must be aligned to be last byte in the block. Ordering is little-endian.
} SFAM_DATA;

///
/// BIOS Guard Platform Data Table  (BGPDT)
/// Provides platform specific data required by BIOS Guard Module
///
typedef struct {
  UINT32    BgpdtSize;                 ///< Size in bytes of BGPDT including SFAM.
  UINT16    BgpdtMajVer;               ///< Indicates major version of BGPDT.
  UINT16    BgpdtMinVer;               ///< Indicates minor version of BGPDT.
  UINT8     PlatId[16];                ///< PLAT_ID used to be compared against the one found in the BGUP Header to prevent cross platform flashing.
  UINT8     PkeySlot0[32];             ///< SHA256 hash for BGUP verification key 0.
  UINT8     PkeySlot1[32];             ///< SHA256 hash for BGUP verification key 1.
  UINT8     PkeySlot2[32];             ///< SHA256 hash for BGUP verification key 2.
  UINT32    BgModSvn;                  ///< BIOS Guard Module SVN.
  UINT32    BiosSvn;                   ///< BIOS_SVN to prevent back-flashing.
  UINT32    ExecLim;                   ///< Limit the number of opcodes that can be executed on any invocation of BIOS Guard.
  /**
  Bitmap of Policy attributes
   - BIT[0]: Reserved. Must be 0
   - BIT[2:1]: EC_PRESENT
      - 00b = EC does not exist in the system.
      - 01b = There exists an EC in the system, BIOS Guard does not extend any protection to the EC.
      - 11b = There exist an EC in the system, BIOS Guard extends protection to the EC.
      - 10b = Reserved.  Must not be used.
   - BIT[3]: DESCRIPTOR_OVERRIDE_POLICY
       - 0b = Do not override BIOS Guard security policy.
       - 1b = Override BIOS Guard security policy.
   - BIT[4]: FLASH WEAR-OUT POLICY
       - 0b = Legacy BIOS Guard behavior.
       - 1b = Enable Flash Wear-Out Protection mitigation.
   - BIT[24:5]: Reserved, must be 0

   - BIT[31:4]: Reserved. Must be 0
  **/
  UINT32    PlatAttr;
  /**
  Read/Write command sent to EC
   - BIT[9:0]: 8 bit IO port used for sending EC commands (writes), and reading EC status (reads).
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:10]: Reserved.  Must be 0.
  **/
  UINT32    EcCmd;
  /**
  Data read or written to EC
   - BIT[9:0]: 8 bit IO port used for reading and writing data to the EC based on a command issued to EC_CMD.
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:10]: Reserved.  Must be 0
  **/
  UINT32    EcData;
  /**
  EC command indicating a read of the current EC firmware SVN.
   - BIT[7:0]: EC command.
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:8]: Reserved.  Must be 0.
  **/
  UINT32    EcCmdGetSvn;
  /**
  EC command indicating begin of flash update session.
   - BIT[7:0]:  EC command.
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:8]: Reserved.  Must be 0.
  **/
  UINT32    EcCmdOpen;
  /**
  EC command indicating the termination of BIOS Guard protected session.
   - BIT[7:0]:  EC command indicating the termination of BIOS Guard protected session.
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:8]: Reserved.  Must be 0.
  **/
  UINT32    EcCmdClose;
  /**
  EC command used to verify connectivity between BIOS Guard and EC.
   - BIT[7:0]:  EC command.
      - This field must be populated if PLAT_ATTR.EC_PRESENT != 0.
      - This field must be zero if PLAT_ATTR.EC_PRESENT == 0.
   - BIT[31:8]: Reserved.  Must be 0.
  **/
  UINT32    EcCmdPortTest;
  UINT8     Reserved1[4];              ///< Reserved bits.
  /**
  Defines number of elements in SFAM array
   - BIT[5..0]: Index of the last SFAM element
   - BIT[7..6]: Reserved for future use.  Must be 0
  **/
  UINT8     LastSfam;
  UINT8     Reserved2[3];              ///< Reserved 3 bits.
  SFAM_DATA SfamData[MAX_SFAM_COUNT];  ///< Array of flash address map descriptors.  sizeof (SFAM_DATA) == 8
} BGPDT;

///
/// BIOS Guard update Package Header
///
typedef struct {
  UINT16  Version;           ///< Version of the update package header.  Must be 0x0002.
  UINT8   Reserved3[2];      ///< Reserved bits.
  UINT8   PlatId[16];        ///< PLAT_ID used to be compared against the one found in the BGPDT to prevent cross platform flashing.
  /**
  If any bit set in this field then BGUP must be signed and valid BGUPC must be provided for BGUP to be processed.
  - BIT[0]: Indicates write/erase operations will be executed on protected flash area indicated in the BGPDT SFAM.
  - BIT[1]: Indicates protected EC operations included.
  **/
  UINT16  PkgAttributes;
  UINT8   Reserved4[2];      ///< Reserved bits.
  UINT16  PslMajorVer;       ///< Indicates the PSL major version. Must be 2.
  UINT16  PslMinorVer;       ///< Indicates the PSL minor version. Must be 0.
  UINT32  ScriptSectionSize; ///< Size in bytes of the script.
  UINT32  DataSectionSize;   ///< Size of the data region in bytes.
  UINT32  BiosSvn;           ///< BIOS SVN.
  UINT32  EcSvn;             ///< EC SVN.
  UINT32  VendorSpecific;    ///< Vendor specific data.
} BGUP_HEADER;


///
/// Memory Size for BIOS Guard Update Package when in TSEG
///
#define BGUP_TSEG_BUFFER_SIZE        0x00014000  ///< 16KB Script + 64KB Flash Block.

///
/// BIOS Guard update package definition for BIOS SMM Initiated runtime calls
///
typedef struct {
  BGUP_HEADER  BgupHeader;                            ///< BIOS Guard update package header.
  UINT64       BgupBuffer[BGUP_TSEG_BUFFER_SIZE / 8]; ///< BIOS Guard update buffer - Designed to contain the BIOS Guard Script followed immediately by the Update Data
} BGUP;

///
/// BIOS Guard Log
/// The logging facility is used to communicate detailed information regarding the execution of a BIOS Guard script
/// from the SMI handler which invoked the BIOS Guard module itself.
///
typedef struct {
  UINT16  Version;           ///< Indicates the version of the log.  Must be 0x0001.
  UINT16  LastPage;          ///< Last valid page index for the log.
  /**
  Bitmap indicating what events to log
  - BIT[0]:    Step trace. This indicates a full execution trace.  Each line is entered into the log with an EXECUTION_TRACE entry.
  - BIT[1]:    Branch trace. All taken jumps are logged with a BRANCH_TRACE entry.
  - BIT[2]:    Flash write  All flash write operations are logged with a FLASH_WRITE entry.
  - BIT[3]:    Flash erase  All flash erase operations are logged with a FLASH_ERASE entry.
  - BIT[4]:    Flash error. All error conditions from flash operations are logged with FLASH_ERROR entry.
  - BIT[5]:    Debug. Log Debug opcode execution.
  - BIT[6]:    BIOS Guard module debug message. Log implementation specific debug messages from debug module.
  - BIT[31:7]: Reserved. Must be 0. If any reserved bits are set in Header.LoggingOptions, the BIOS Guard module must disable the logging feature.
  **/
  UINT32  LoggingOptions;
  UINT8   Reserved5[8];      ///< Reserved bits.
  UINT32  BgModSvn;          ///< Indicates a version number of the BIOS Guard module.
  UINT32  NumOfEntriesInLog; ///< Total number of log entries that have been written to the log.
} BIOSGUARD_LOG;

///
/// HOB used to pass data through every phase of BIOS Guard Bios.
/// BIOS Guard BIOS code is executed in PEI, DXE and SMM and HOB is the only method to properly pass data between every phase.
///
typedef struct {
  EFI_HOB_GUID_TYPE  EfiHobGuidType;
  BGPDT              Bgpdt;                             ///< BIOS Guard Platform Data Table.
  BGUP_HEADER        BgupHeader;                        ///< BIOS Guard update package header, this header will be appended to all flash updates along with PSL script.
  UINT16             TotalFlashSize;                    ///< Total Flash Size on the system in KB
  UINT16             BiosSize;                          ///< BIOS Size in KB
  UINT64             BiosGuardToolsIntIoTrapAdd;        ///< IO Trap address required to Initialize BIOS Guard Tools Interface.
  BIOSGUARD_LOG      BiosGuardLog;                      ///< Header for BIOS Guard Log Buffer.
} BIOSGUARD_HOB;

#endif

