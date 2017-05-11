/** @file
  CSE Variable Storage Library.

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CSE_VARIABLE_STORAGE_LIB_H_
#define _CSE_VARIABLE_STORAGE_LIB_H_

#include <Base.h>
#include <Uefi.h>
#include <VariableNvmStorageFormat.h>

#define CSE_VARIABLE_NVM_ALIGNMENT          16
#define CSE_MAX_NVM_FILE_NAME_LENGTH        48
#define CSE_PRIMARY_NVM_MAX_DATA_FILES      256

// Temp Dev Note (@todo) - Make the maximums below a PCD
#define CSE_MAX_FILE_DATA_SIZE              SIZE_64KB

#define CSE_PRE_MEMORY_FILE_MAX_STORE_SIZE  (SIZE_64KB - SIZE_16KB)
#define CSE_PRIMARY_NVM_MAX_STORE_SIZE      SIZE_512KB

#define CSE_PRIMARY_NVM_DATA_FILE_PREFIX    "NVS/VAR_"

#define CSE_PRE_MEMORY_FILE_STORE_OFFSET    0
#define CSE_PRIMARY_NVM_FILE_STORE_OFFSET   0

//
// Variable store file options provided by CSE (platform specific)
//
typedef enum {
  CseVariableFileTypePreMemoryFile,      ///< A special file available in pre-memory
  CseVariableFileTypePrimaryIndexFile,   ///< The primary store on CSE NVM
  CseVariableFileTypeMax                 ///< End of CSE variable store file types
} CSE_VARIABLE_FILE_TYPE;

typedef enum {
  CseVariableHeci1Protocol,     ///< HECI1 protocol
  CseVariableHeci2Protocol      ///< HECI2 protocol
} CSE_VARIABLE_HECI_PROTOCOL;

#pragma pack(push, 1)

typedef struct {
  BOOLEAN                FileEnabled;             ///< Indicates if the file is enabled (used for variable storage)
  BOOLEAN                FileStoreEstablished;    ///< Indicates whether the CSE NVM variable store has been established.
  UINT32                 FileStoreMaximumSize;    ///< The maximum size of this variable store
  CHAR8                  *FileName;               ///< Name of the CSE NVM file
  EFI_GUID               *IndexFileDataGuid;      ///< Pointer to the GUID of the HOB that contains the index file data read in PEI for this variable store
  EFI_PHYSICAL_ADDRESS   HeaderRegionBase;        ///< Memory address of the base of the variable header region
                                                  ///< kept in memory to search for variables.
} CSE_VARIABLE_FILE_INFO;

#pragma pack(pop)

/**
  Prints bytes in a buffer.
  Temp Dev Note: Only used for temporary testing.

  @param[in]  Data8      A pointer to the memory buffer to print.
  @param[in]  DataSize   The size of data to print from the buffer.

  @retval     None

**/
VOID
TemporaryDebugDumpBufferData (
  IN UINT8   *Data8,
  IN UINTN   DataSize
  );

/**
  Creates variable stores if in DXE and necessary. Loads the variable data region
  from CSE NVM needed to find variables.

  @param[in,out]  CseVariableFileInfo   A pointer to a structure that contains information the CSE files available.
  @param[in]      AtDxeRuntime          Determines how this function should allocate memory.
                                        Caller passes TRUE if in DXE and FALSE for pre-DXE (PEI).

  @retval         EFI_SUCCESS           The variable store was established successfully.
  @retval         Others                The variable store could not be established.

**/
EFI_STATUS
EFIAPI
EstablishAndLoadCseVariableStores (
  IN OUT CSE_VARIABLE_FILE_INFO   **CseVariableFileInfo,
  IN     BOOLEAN                  AtDxeRuntime
  );

/**
  Initializes the global variable structures for CSE storage modules.

  @param[in]  AtDxeRuntime           Determines how this function should allocate memory.
                                     Caller passes TRUE if in DXE and FALSE for pre-DXE (PEI).

  @retval     EFI_SUCCESS            The global variables were initialized successfully.
  @retval     EFI_OUT_OF_RESOURCES   Not enough memory resources needed for allocation were available.

**/
EFI_STATUS
EFIAPI
InitializeCseStorageGlobalVariableStructures (
  IN OUT CSE_VARIABLE_FILE_INFO   **CseVariableFileInfo,
  IN     BOOLEAN                  AtDxeRuntime
  );

/**
  Returns the next available variable in the given CSE variable stores.

  @param[in]  VariableName         Pointer to variable name.
  @param[in]  VendorGuid           Variable Vendor Guid.
  @param[in]  CseVariableFileInfo  A pointer to a structure that contains information the CSE files available.
  @param[out] VariablePtr          Pointer to variable header address.
  @param[out] IsAuthVariable       Indicates if the variable returned is an authenticated variable.

  @retval     EFI_SUCCESS          Found the specified variable.
  @retval     EFI_NOT_FOUND        Not found.

**/
EFI_STATUS
EFIAPI
GetNextCseVariableName (
  IN   CONST CHAR16                  *VariableName,
  IN   CONST EFI_GUID                *VariableGuid,
  IN   CSE_VARIABLE_FILE_INFO        **CseVariableFileInfo,
  OUT  VARIABLE_NVM_HEADER           **VariablePtr,
  OUT  BOOLEAN                       *IsAuthVariable
  );

/**
  Reads data from a CSE NVM file into a specified memory buffer.

  @param[in]  CseFileName            The name of the CSE file used in the read operation.
  @param[in]  FileOffset             The offset to begin reading in the CSE file.
  @param[out] Data                   A pointer to a buffer that is the destination for the read.
  @param[out] DataSize               The length in bytes of data to read from the file.
  @param[in]  HeciProtocolSelector   The HECI protocol to use to write the file.
                                     If not specified, the protocol will be determined internally.

  @retval     EFI_SUCCESS            The file was read successfully.
  @retval     Others                 An error occurred reading the file.

**/
EFI_STATUS
EFIAPI
ReadCseNvmFile (
  IN      CHAR8                       *CseFileName,
  IN      UINT32                      FileOffset,
  OUT     UINT8                       *Data,
  OUT     UINTN                       *DataSize,
  IN      CSE_VARIABLE_HECI_PROTOCOL  *HeciProtocolSelector  OPTIONAL
  );

/**
  Writes data in a memory buffer to a specified CSE NVM file.

  @param[in]  CseFileName            The name of the CSE file used in the write operation.
  @param[in]  FileOffset             The offset to begin writing in the CSE file.
  @param[in]  Data                   A pointer to a buffer of data to write to the file.
  @param[in]  DataSize               The length in bytes of data to write to the file.
  @param[in]  HeciProtocolSelector   The HECI protocol to use to write the file.
                                     If not specified, the protocol will be determined internally.
  @param[in]  Truncate               If true, the file will be truncated.

  @retval     EFI_SUCCESS            The file was updated successfully.
  @retval     Others                 An error occurred updating the file.

**/
EFI_STATUS
EFIAPI
UpdateCseNvmFile (
  IN      CHAR8                       *CseFileName,
  IN      UINT32                      FileOffset,
  IN      UINT8                       *Data,
  IN      UINTN                       DataSize,
  IN      BOOLEAN                     Truncate,
  IN      CSE_VARIABLE_HECI_PROTOCOL  *HeciProtocolSelector  OPTIONAL
  );

/**
  Creates an empty variable store on the CSE NVM file specified.

  @param[in]   StoreFileName   A pointer to the string representing the CSE file name containing this store.
  @param[in]   Offset          The offset from the beginning of the CSE NVM file to the first byte of the store.
  @param[out]  StoreSize       The size of the variable store created / found will be written to this pointer.

  @retval      EFI_SUCCESS     The variable store was created successfully.
  @retval      Others          The variable store could not be created.

**/
EFI_STATUS
EFIAPI
CreateCseNvmVariableStore (
  IN  CHAR8     *StoreFileName,
  IN  UINT32    Offset,
  OUT UINTN     *StoreSize
  );

/**
  Returns the file name for a CSE data file at the specified offset (file number).

  @param[in]   DataOffset               The file number on CSE NVM (each data file is an "offset")
  @param[out]  CseVariableDataFileName  The CSE file name.

  @retval      EFI_INVALID_PARAMETER    A valid buffer was not passed to fill with the file name.
  @retval      EFI_SUCCESS              File name was successfully created and copied to the buffer.

**/
EFI_STATUS
EFIAPI
BuildCseDataFileName (
  IN  UINT32     DataOffset,
  OUT CHAR8      *CseVariableDataFileName
  );

/**
  Finds the variable in CSE NVM.
  This code finds the variable in the CSE NVM variable stores enabled.

  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  VariableName            Name of the variable to be found.
  @param[in]  VendorGuid              Vendor GUID to be found.
  @param[in]  CseVariableFileInfo     A pointer to an array of structures that contains information the CSE files available.
  @param[out] PtrTrack                VARIABLE_NVM POINTER_TRACK structure for output,
                                      including the range searched and the target position.

  @retval     EFI_INVALID_PARAMETER   If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval     EFI_SUCCESS             Variable successfully found.
  @retval     EFI_NOT_FOUND           Variable not found

**/
EFI_STATUS
EFIAPI
FindVariable (
  IN   CONST  CHAR16                      *VariableName,
  IN   CONST  EFI_GUID                    *VendorGuid,
  IN   CSE_VARIABLE_FILE_INFO             **CseVariableFileInfo,
  OUT  VARIABLE_NVM_POINTER_TRACK         *VariablePtrTrack
  );

/**
  Finds deleted variables in the specified CSE NVM variable store.

  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]      VariableName            Name of the variable to be found.
  @param[in]      VendorGuid              Vendor GUID to be found.
  @param[in]      VariableStore           Pointer to a VARIABLE_NVM_STORE_HEADER structure which
                                          is the variable store to search.
  @param[in,out]  VariableTrack Ptr       Pointer to a variable NVM tracking structure which points
                                          current pointer to the variable if found.

  @retval         EFI_INVALID_PARAMETER   If VariableName is not an empty string, while
                                          VendorGuid is NULL.
  @retval         EFI_SUCCESS             Variable successfully found.
  @retval         EFI_NOT_FOUND           Variable not found

**/
EFI_STATUS
EFIAPI
FindDeletedVariable (
  IN  CONST  CHAR16                     *VariableName,
  IN  CONST  EFI_GUID                   *VendorGuid,
  IN         VARIABLE_NVM_STORE_HEADER  *VariableStorePtr,
  IN OUT     VARIABLE_NVM_POINTER_TRACK *VariablePtrTrack
  );

/**
  Retrieves a variable's value using its name and GUID.

  Read the specified variable from the CSE NVM variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  Note: It is the caller's responsibility to extensively validate the input parameters.

  @param[in]       CseVariableFileInfo    A pointer to an array of structures that contains information the CSE files available.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.
  @param[out]      VariableHeader         A pointer to the variable header.

  @retval          EFI_SUCCESS            The variable was read successfully.
  @retval          EFI_NOT_FOUND          The variable could not be found.
  @retval          EFI_BUFFER_TOO_SMALL   The DataSize is too small for the resulting data.
                                          DataSize is updated with the size required for
                                          the specified variable.
  @retval          EFI_INVALID_PARAMETER  VariableName, VariableGuid, DataSize or Data is NULL.
  @retval          EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
GetCseVariable (
  IN         CSE_VARIABLE_FILE_INFO     **CseVariableFileInfo,
  IN  CONST  CHAR16                     *VariableName,
  IN  CONST  EFI_GUID                   *VariableGuid,
  OUT        UINT32                     *Attributes            OPTIONAL,
  IN OUT     UINTN                      *DataSize,
  OUT        VOID                       *Data,
  OUT        VARIABLE_NVM_HEADER        *VariableHeader        OPTIONAL
  );

#endif

