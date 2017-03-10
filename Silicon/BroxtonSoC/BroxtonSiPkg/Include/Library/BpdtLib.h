/** @file
  Interface definitons of BPDT(Boot Partition Description Table) library.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BPDT_HEADER_H__
#define __BPDT_HEADER_H__

extern EFI_GUID gEfiBpdtLibBp1DataGuid;
extern EFI_GUID gEfiBpdtLibBp2DataGuid;

#define BPDT_SIGN_GREEN       0x000055AA    //Normal Boot
#define BPDT_SIGN_YELLOW      0x00AA55AA    //Recovery Boot

typedef enum {
  BootPart1,
  BootPart2,
  BootPartMax
} BOOT_PARITION_SELECT;

typedef struct {
  VOID   *DataPtr;
  UINT32  Size;
} BPDT_PAYLOAD_DATA;

typedef enum {
  BpdtOemSmip,      // 0
  BpdtCseRbe,       // 1
  BpdtCseBup,       // 2
  BpdtUcode,        // 3
  BpdtIbb,          // 4
  BpdtSbpdt,        // 5 - Secondary BPDT within a BPDT Entry
  BpdtObb,          // 6
  BpdtCseMain,      // 7
  BpdtIsh,          // 8
  BpdtCseIdlm,      // 9
  BpdtIfpOverride,  //10
  BpdtDebugTokens,  //11
  BpdtUfsPhyConfig, //12
  BpdtUfsGppLunId,  //13
  BpdtPmc,          //14
  BpdtIunit,        //15
  BpdtMaxType
} BPDT_ENTRY_TYPES;

typedef struct {
  UINT32  Signature;
  UINT16  DscCount;
  UINT16  BpdtVer;
  UINT32  RedundantBlockXorSum;
  UINT32  IfwiVer;
  UINT64  FitToolVer;
} BPDT_HEADER;


typedef struct {
  UINT16  Type;
  UINT16  Flags;
  UINT32  LbpOffset;  // Offset of Sub-Partition starting from base of LBP
  UINT32  Size;       // Size of the Sub-Partition
} BPDT_ENTRY;


typedef struct {
  UINT32  HeaderMarker;
  UINT32  NumOfEntries;
  UINT8   HeaderVersion;
  UINT8   EntryVersion;
  UINT8   HeaderLength;
  UINT8   Checksum;
  UINT32  SubPartName;
} SUBPART_DIR_HEADER;


typedef struct {
  CHAR8   EntryName[12];
  UINT32  EntryOffset:25;
  UINT32  Huffman:1;
  UINT32  Reserved1:6;
  UINT32  EntrySize;
  UINT32  Reserved2;
} SUBPART_DIR_ENTRY;

typedef enum {
  HashIbbl = 0,
  HashIbbm,
  HashObb
} BPM_HASH_SELECT;

///
/// Boot Policy Manifest (BPM) data structure
///
typedef struct {
  UINT32                    ExtType;       ///< 0x00000013  (Little-Endian)
  UINT32                    ExtLength;
  UINT32                    NemData;
  UINT32                    IbblHashAlgo;  ///< 0x00000002
  UINT32                    IbblHashSize;  ///< 0x00000020
  UINT8                     IbblHash[0x20];
  UINT32                    IbbmHashAlgo;
  UINT32                    IbbmHashSize;
  UINT8                     IbbmHash[0x20];
  UINT32                    ObbHashAlgo;
  UINT32                    ObbHashSize;
  UINT8                     ObbHash[0x20];
  UINT8                     Reserved[124]; ///< Others data
} BPM_DATA_FILE;


/**
  Locate and return a pointer to the selected Logical Boot Partition.

  For block devices, the address will be the location in memory that the BP was copied to.
  For memory mapped device (eg SPI), the address will be the location on the device.

  @param[in]    LbpSel         Which BP to locate on the boot device
  @param[out]   Address        The address of the selected BP on the boot device

  @retval       EFI_SUCCESS    The operation completed successfully.
  @retval       other          The selected BP could not be found.
**/
EFI_STATUS
EFIAPI
GetBootPartitionPointer (
  IN  BOOT_PARITION_SELECT  LbpSel,
  OUT VOID                  **LbpPointer
  );

/**
  Parse the Boot Partition Descriptor Table of the provided Logical Boot Partition.
  Note: "payload" is the Sub-Partition pointed to by the BDPT Entry of the given type.

  @param[in]   LbpPointer      Pointer to the start of the BootPartition to be parsed
  @param[in]   EntryType       The entry type of the subparition(payload) to look for
  @param[out]  BpdtPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval      EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBpdtPayloadDataRaw (
  IN  VOID                *LbpPointer,
  IN  BPDT_ENTRY_TYPES    EntryType,
  OUT BPDT_PAYLOAD_DATA   *BpdtPayloadPtr
  );

/**
  Parse the Boot Partition Descriptor Table on the selected Logical Boot Partition.
  Takes the BP as input since some BPDT entry types can exist in both locations.
  Note: "payload" is the Sub-Partition pointed to by the BDPT Entry of the given type.
  
  @param[in]   BpSel           Boot partition index
  @param[in]   EntryType       The entry type of the subparition(payload) to look for
  @param[out]   BpdtPayloadPtr Pointer to a Struct (in a HOB) containing the Size and Absolute address
                               in memory(or spi) of the BPDT payload(subpart) of EntryType.
                               If the caller shadows the Payload for performance, it should update this value

  @retval  EFI_SUCCESS         The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBpdtPayloadData (
  IN  BOOT_PARITION_SELECT  BpSel,
  IN  BPDT_ENTRY_TYPES      EntryType,
  OUT BPDT_PAYLOAD_DATA     **BpdtPayloadPtr
  );


/**
  Search the Subpartition pointed to by BpdtPayloadPtr for a Directory entry with "EntryName",
  then fill in the Struct pointed to by SubPartPayloadPtr with the Addres and Size of the Dir Entry.
  Note: "payload" is the Data pointed to by the Directory Entry with the given name.

  @param[in]   BpdtPayloadPtr     Location and Size of the Payload to search
  @param[in]   EntryName          the String name of the Directory entry to look for
  @param[out]  SubPartPayloadPtr  Address of the Struct to put the results into (MUST be allocated by caller)

  @retval      EFI_SUCCESS        The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetSubPartitionPayloadData (
  IN  BPDT_PAYLOAD_DATA     *BpdtPayloadPtr,
  IN  CONST CHAR8           *EntryName,
  OUT BPDT_PAYLOAD_DATA     *SubPartPayloadPtr
  );


/**
  This is a wrapper function for using VerifyHashBpm that will only
  check the hash for the payloads used during normal boot.

  If you want to check other payloads (ie during recovery flow), then the
  calling code should locate and pass in the payload directly to VerifyHashBpm().

  @param[in]   PayloadSel   Which hash in BPM to compare with.

  @retval      EFI_SUCCESS  The function completed successfully and the Hash matched.
  @retval      other        An error occured when locating and computing the Hash.
**/
EFI_STATUS
EFIAPI
LocateAndVerifyHashBpm (
  IN BPM_HASH_SELECT   PayloadSel
  );


/**
  Computes the Hash of a given data block and compares to the one in the Boot Policy Metadata.

  @param[in]  PayloadSel   Which hash in BPM to compare with.
  @param[in]  PayloadPtr   Pointer to the begining of the data to be hashed.
  @param[in]  DataSize     Size of the data to be hashed.
  @param[in]  BpmPtr       Pointer to the BPM structure.

  @retval     EFI_SUCCESS             If the function completed successfully.
  @retval     EFI_ABORTED             If the attempt to compute the hash fails.
  @retval     EFI_INVALID_PARAMETER   If the hash type or size in BPM is unsupported.
  @retval     EFI_SECURITY_VIOLATION  If the has does not match the one in BPM.
**/
EFI_STATUS
EFIAPI
VerifyHashBpm (
  IN BPM_HASH_SELECT   PayloadSel,
  IN BPM_DATA_FILE     *BpmPtr,
  IN VOID              *PayloadPtr,
  IN UINT32            DataSize
  );

#endif
