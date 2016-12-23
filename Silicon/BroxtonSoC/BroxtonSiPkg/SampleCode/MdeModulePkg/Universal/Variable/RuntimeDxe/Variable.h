/** @file
  The internal header file includes the common header files, defines
  internal structure and functions used by Variable modules.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <PiDxe.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/Variable.h>
#include <Protocol/VariableLock.h>
#include <Protocol/VarCheck.h>
#include <Protocol/VariableStorageProtocol.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AuthVariableLib.h>
#include <Library/VarCheckLib.h>
#include <Library/VariableStorageSelectorLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>
#include <Guid/VariableFormat.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/VarErrorFlag.h>

#define EFI_VARIABLE_ATTRIBUTES_MASK (EFI_VARIABLE_NON_VOLATILE | \
                                      EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                      EFI_VARIABLE_RUNTIME_ACCESS | \
                                      EFI_VARIABLE_HARDWARE_ERROR_RECORD | \
                                      EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | \
                                      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS | \
                                      EFI_VARIABLE_APPEND_WRITE)

///
/// The size of a 3 character ISO639 language code.
///
#define ISO_639_2_ENTRY_SIZE    3

#define MAX_VARIABLE_NAME_SIZE  1024
#define DEFAULT_NV_STORE_SIZE   0x20000

typedef enum {
  VariableStoreTypeVolatile,
  VariableStoreTypeHob,
  VariableStoreTypeNvCache,
  VariableStoreTypeMax
} VARIABLE_STORE_TYPE;

typedef struct {
  VARIABLE_HEADER *CurrPtr;
  //
  // If both ADDED and IN_DELETED_TRANSITION variable are present,
  // InDeletedTransitionPtr will point to the IN_DELETED_TRANSITION one.
  // Otherwise, CurrPtr will point to the ADDED or IN_DELETED_TRANSITION one,
  // and InDeletedTransitionPtr will be NULL at the same time.
  //
  VARIABLE_HEADER *InDeletedTransitionPtr;
  VARIABLE_HEADER *EndPtr;
  VARIABLE_HEADER *StartPtr;
  BOOLEAN         Volatile;
} VARIABLE_POINTER_TRACK;

typedef struct {
  EFI_PHYSICAL_ADDRESS        HobVariableBase;
  EFI_PHYSICAL_ADDRESS        VolatileVariableBase;
  VARIABLE_STORAGE_PROTOCOL   **VariableStores;
  UINTN                       VariableStoresCount;
  EFI_LOCK                    VariableServicesLock;
  UINT32                      ReentrantState;
  BOOLEAN                     AuthFormat;
  BOOLEAN                     AuthSupport;
} VARIABLE_GLOBAL;

typedef struct {
  VARIABLE_GLOBAL VariableGlobal;
  UINTN           VolatileLastVariableOffset;
  UINTN           CommonVariableSpace;
  UINTN           CommonMaxUserVariableSpace;
  UINTN           CommonRuntimeVariableSpace;
  UINTN           CommonVariableTotalSize;
  UINTN           CommonUserVariableTotalSize;
  UINTN           HwErrVariableTotalSize;
  UINTN           MaxVariableSize;
  UINTN           MaxAuthVariableSize;
  UINTN           ScratchBufferSize;
  CHAR8           *PlatformLangCodes;
  CHAR8           *LangCodes;
  CHAR8           *PlatformLang;
  CHAR8           Lang[ISO_639_2_ENTRY_SIZE + 1];
  BOOLEAN         WriteServiceReady;
} VARIABLE_MODULE_GLOBAL;

/**
  Flush the HOB variable to flash.

  @param[in] VariableName       Name of variable has been updated or deleted.
  @param[in] VendorGuid         Guid of variable has been updated or deleted.

**/
VOID
FlushHobVariableToFlash (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid
  );

/**
  Writes a buffer to variable storage space, in the working block.

  This function writes a buffer to variable storage space into a firmware
  volume block device. The destination is specified by the parameter
  VariableBase. Fault Tolerant Write protocol is used for writing.

  @param[in]  VariableBase   Base address of the variable to write.
  @param[in]  VariableBuffer Point to the variable data buffer.

  @retval     EFI_SUCCESS    The function completed successfully.
  @retval     EFI_NOT_FOUND  Fail to locate Fault Tolerant Write protocol.
  @retval     EFI_ABORTED    The function could not complete successfully.

**/
EFI_STATUS
FtwVariableSpace (
  IN EFI_PHYSICAL_ADDRESS   VariableBase,
  IN VARIABLE_STORE_HEADER  *VariableBuffer
  );

/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  If IgnoreRtCheck is TRUE, then we ignore the EFI_VARIABLE_RUNTIME_ACCESS attribute check
  at runtime when searching existing variable, only VariableName and VendorGuid are compared.
  Otherwise, variables without EFI_VARIABLE_RUNTIME_ACCESS are not visible at runtime.

  @param[in]   VariableName           Name of the variable to be found.
  @param[in]   VendorGuid             Vendor GUID to be found.
  @param[out]  PtrTrack               VARIABLE_POINTER_TRACK structure for output,
                                      including the range searched and the target position.
  @param[in]   Global                 Pointer to VARIABLE_GLOBAL structure, including
                                      base of volatile variable storage area, base of
                                      NV variable storage area, and a lock.
  @param[in]   IgnoreRtCheck          Ignore EFI_VARIABLE_RUNTIME_ACCESS attribute
                                      check at runtime when searching variable.

  @retval      EFI_INVALID_PARAMETER  If VariableName is not an empty string, while
                                      VendorGuid is NULL.
  @retval      EFI_SUCCESS            Variable successfully found.
  @retval      EFI_NOT_FOUND          Variable not found.

**/
EFI_STATUS
FindVariable (
  IN  CHAR16                  *VariableName,
  IN  EFI_GUID                *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack,
  IN  VARIABLE_GLOBAL         *Global,
  IN  BOOLEAN                 IgnoreRtCheck
  );

/**

  Gets the pointer to the end of the variable storage area.

  This function gets pointer to the end of the variable storage
  area, according to the input variable store header.

  @param[in] VarStoreHeader  Pointer to the Variable Store Header.

  @return    Pointer to the end of the variable storage area.

**/
VARIABLE_HEADER *
GetEndPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  );

/**
  This code gets the size of variable header.

  @return Size of variable header in bytes in type UINTN.

**/
UINTN
GetVariableHeaderSize (
  VOID
  );

/**

  This code gets the pointer to the variable name.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Pointer to Variable Name which is Unicode encoding.

**/
CHAR16 *
GetVariableNamePtr (
  IN  VARIABLE_HEADER   *Variable
  );

/**
  This code gets the pointer to the variable guid.

  @param[in] Variable   Pointer to the Variable Header.

  @return    A EFI_GUID* pointer to Vendor Guid.

**/
EFI_GUID *
GetVendorGuidPtr (
  IN VARIABLE_HEADER    *Variable
  );

/**

  This code gets the pointer to the variable data.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Pointer to Variable Data.

**/
UINT8 *
GetVariableDataPtr (
  IN  VARIABLE_HEADER   *Variable
  );

/**

  This code gets the size of variable data.

  @param[in] Variable        Pointer to the Variable Header.

  @return    Size of variable in bytes.

**/
UINTN
DataSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable
  );

/**
  This function is to check if the remaining variable space is enough to set
  all Variables from argument list successfully. The purpose of the check
  is to keep the consistency of the Variables to be in variable storage.

  Note: Variables are assumed to be in same storage.
  The set sequence of Variables will be same with the sequence of VariableEntry from argument list,
  so follow the argument sequence to check the Variables.

  @param[in] Attributes         Variable attributes for Variable entries.
  @param[in] Marker             VA_LIST style variable argument list.
                                The variable argument list with type VARIABLE_ENTRY_CONSISTENCY *.
                                A NULL terminates the list. The VariableSize of
                                VARIABLE_ENTRY_CONSISTENCY is the variable data size as input.
                                It will be changed to variable total size as output.

  @retval   TRUE                Have enough variable space to set the Variables successfully.
  @retval   FALSE               No enough variable space to set the Variables successfully.

**/
BOOLEAN
EFIAPI
CheckRemainingSpaceForConsistencyInternal (
  IN UINT32                     Attributes,
  IN VA_LIST                    Marker
  );

/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in]      VariableName          Name of variable.
  @param[in]      VendorGuid            Guid of variable.
  @param[in]      Data                  Variable data.
  @param[in]      DataSize              Size of data. 0 means delete.
  @param[in]      Attributes            Attributes of the variable.
  @param[in]      KeyIndex              Index of associated public key.
  @param[in]      MonotonicCount        Value of associated monotonic count.
  @param[in, out] Variable              The variable information that is used to keep track of variable usage.

  @param[in]      TimeStamp             Value of associated TimeStamp.

  @retval         EFI_SUCCESS           The update operation is success.
  @retval         EFI_OUT_OF_RESOURCES  Variable region is full, cannot write other data into this region.

**/
EFI_STATUS
UpdateVariable (
  IN      CHAR16          *VariableName,
  IN      EFI_GUID        *VendorGuid,
  IN      VOID            *Data,
  IN      UINTN           DataSize,
  IN      UINT32          Attributes OPTIONAL,
  IN      UINT32          KeyIndex  OPTIONAL,
  IN      UINT64          MonotonicCount  OPTIONAL,
  IN OUT  VARIABLE_POINTER_TRACK *Variable,
  IN      EFI_TIME        *TimeStamp  OPTIONAL
  );


/**
  Return TRUE if ExitBootServices () has been called.

  @retval TRUE If ExitBootServices () has been called.

**/
BOOLEAN
AtRuntime (
  VOID
  );

/**
  Initializes a basic mutual exclusion lock.

  This function initializes a basic mutual exclusion lock to the released state
  and returns the lock.  Each lock provides mutual exclusion access at its task
  priority level.  Since there is no preemption or multiprocessor support in EFI,
  acquiring the lock only consists of raising to the locks TPL.
  If Lock is NULL, then ASSERT().
  If Priority is not a valid TPL value, then ASSERT().

  @param[in]  Lock       A pointer to the lock data structure to initialize.
  @param[in]  Priority   EFI TPL is associated with the lock.

  @return The lock.

**/
EFI_LOCK *
InitializeLock (
  IN OUT EFI_LOCK  *Lock,
  IN EFI_TPL        Priority
  );

/**
  Acquires lock only at boot time. Simply returns at runtime.

  This is a temperary function that will be removed when
  EfiAcquireLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiAcquireLock() at boot time, and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to acquire.

**/
VOID
AcquireLockOnlyAtBootTime (
  IN EFI_LOCK  *Lock
  );

/**
  Releases lock only at boot time. Simply returns at runtime.

  This is a temperary function which will be removed when
  EfiReleaseLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiReleaseLock() at boot time and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to release.

**/
VOID
ReleaseLockOnlyAtBootTime (
  IN EFI_LOCK  *Lock
  );

/**
  Initializes variable store area for non-volatile and volatile variable.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.

**/
EFI_STATUS
VariableCommonInitialize (
  VOID
  );

/**
  This function reclaims variable storage if free size is below the threshold.

**/
VOID
ReclaimForOS(
  VOID
  );

/**
  Get non-volatile maximum variable size.

  @return Non-volatile maximum variable size.

**/
UINTN
GetNonVolatileMaxVariableSize (
  VOID
  );

/**
  Initializes variable write service after FVB was ready.

  @retval EFI_SUCCESS          Function successfully executed.
  @retval Others               Fail to initialize the variable service.

**/
EFI_STATUS
VariableWriteServiceInitialize (
  VOID
  );

/**
  This code finds variable in storage blocks (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.

  @param[in]      VariableName               Name of Variable to be found.
  @param[in]      VendorGuid                 Variable vendor GUID.
  @param[out]     Attributes                 Attribute value of the variable found.
  @param[in, out] DataSize                   Size of Data found. If size is less than the
                                             data, this value contains the required size.
  @param[out]     Data                       Data pointer.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  );

/**
  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in] VariableNameSize   Size of the variable name.
  @param[in] VariableName       Pointer to variable name.
  @param[in] VendorGuid         Variable Vendor Guid.

  @return    EFI_SUCCESS        Find the specified variable.
  @return    EFI_NOT_FOUND      Not found.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextVariableInternal (
  IN OUT UINTN              *VariableNameSize,
  IN OUT CHAR16             *VariableName,
  IN OUT EFI_GUID           *VariableGuid
  );

/**

  This code Finds the Next available variable.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in, out] VariableNameSize           Size of the variable name.
  @param[in, out] VariableName               Pointer to variable name.
  @param[in, out] VendorGuid                 Variable Vendor Guid.

  @return         EFI_INVALID_PARAMETER      Invalid parameter.
  @return         EFI_SUCCESS                Find the specified variable.
  @return         EFI_NOT_FOUND              Not found.
  @return         EFI_BUFFER_TO_SMALL        DataSize is too small for the result.

**/
EFI_STATUS
EFIAPI
VariableServiceGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  );

/**
  This code sets variable in storage blocks (Volatile or Non-Volatile).

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in] VariableName                     Name of Variable to be found.
  @param[in] VendorGuid                       Variable vendor GUID.
  @param[in] Attributes                       Attribute value of the variable found
  @param[in] DataSize                         Size of Data found. If size is less than the
                                              data, this value contains the required size.
  @param[in] Data                             Data pointer.

  @return    EFI_INVALID_PARAMETER            Invalid parameter.
  @return    EFI_SUCCESS                      Set successfully.
  @return    EFI_OUT_OF_RESOURCES             Resource not enough to set variable.
  @return    EFI_NOT_FOUND                    Not found.
  @return    EFI_WRITE_PROTECTED              Variable is read-only.

**/
EFI_STATUS
EFIAPI
VariableServiceSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  );

/**
  This code returns information about the EFI variables.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in]  Attributes                     Attributes bitmask to specify the type of variables
                                             on which to return information.
  @param[out] MaximumVariableStorageSize     Pointer to the maximum size of the storage space available
                                             for the EFI variables associated with the attributes specified.
  @param[out] RemainingVariableStorageSize   Pointer to the remaining size of the storage space available
                                             for EFI variables associated with the attributes specified.
  @param[out] MaximumVariableSize            Pointer to the maximum size of an individual EFI variables
                                             associated with the attributes specified.

  @return     EFI_SUCCESS                    Query successfully.

**/
EFI_STATUS
EFIAPI
VariableServiceQueryVariableInfoInternal (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  );

/**
  This code returns information about the EFI variables.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode. This function will do basic validation, before parse the data.

  @param[in] Attributes                     Attributes bitmask to specify the type of variables
                                            on which to return information.
  @param[out] MaximumVariableStorageSize    Pointer to the maximum size of the storage space available
                                            for the EFI variables associated with the attributes specified.
  @param[out] RemainingVariableStorageSize  Pointer to the remaining size of the storage space available
                                            for EFI variables associated with the attributes specified.
  @param[out] MaximumVariableSize           Pointer to the maximum size of an individual EFI variables
                                            associated with the attributes specified.

  @return     EFI_INVALID_PARAMETER         An invalid combination of attribute bits was supplied.
  @return     EFI_SUCCESS                   Query successfully.
  @return     EFI_UNSUPPORTED               The attribute is not supported on this platform.

**/
EFI_STATUS
EFIAPI
VariableServiceQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  );

/**
  Mark a variable that will become read-only after leaving the DXE phase of execution.

  @param[in] This                  The VARIABLE_LOCK_PROTOCOL instance.
  @param[in] VariableName          A pointer to the variable name that will be made read-only subsequently.
  @param[in] VendorGuid            A pointer to the vendor GUID that will be made read-only subsequently.

  @retval    EFI_SUCCESS           The variable specified by the VariableName and the VendorGuid was marked
                                   as pending to be read-only.
  @retval    EFI_INVALID_PARAMETER VariableName or VendorGuid is NULL.
                                   Or VariableName is an empty string.
  @retval    EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                   already been signaled.
  @retval    EFI_OUT_OF_RESOURCES  There is not enough resource to hold the lock request.

**/
EFI_STATUS
EFIAPI
VariableLockRequestToLock (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  );

/**
  Register SetVariable check handler.

  @param[in] Handler               Pointer to check handler.

  @retval    EFI_SUCCESS           The SetVariable check handler was registered successfully.
  @retval    EFI_INVALID_PARAMETER Handler is NULL.
  @retval    EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                   already been signaled.
  @retval    EFI_OUT_OF_RESOURCES  There is not enough resource for the SetVariable check handler register request.
  @retval    EFI_UNSUPPORTED       This interface is not implemented.
                                   For example, it is unsupported in VarCheck protocol if both VarCheck and SmmVarCheck protocols are present.

**/
EFI_STATUS
EFIAPI
VarCheckRegisterSetVariableCheckHandler (
  IN VAR_CHECK_SET_VARIABLE_CHECK_HANDLER   Handler
  );

/**
  Variable property set.

  @param[in] Name                  Pointer to the variable name.
  @param[in] Guid                  Pointer to the vendor GUID.
  @param[in] VariableProperty      Pointer to the input variable property.

  @retval    EFI_SUCCESS           The property of variable specified by the Name and Guid was set successfully.
  @retval    EFI_INVALID_PARAMETER Name, Guid or VariableProperty is NULL, or Name is an empty string,
                                   or the fields of VariableProperty are not valid.
  @retval    EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                   already been signaled.
  @retval    EFI_OUT_OF_RESOURCES  There is not enough resource for the variable property set request.

**/
EFI_STATUS
EFIAPI
VarCheckVariablePropertySet (
  IN CHAR16                         *Name,
  IN EFI_GUID                       *Guid,
  IN VAR_CHECK_VARIABLE_PROPERTY    *VariableProperty
  );

/**
  Variable property get.

  @param[in]  Name                  Pointer to the variable name.
  @param[in]  Guid                  Pointer to the vendor GUID.
  @param[out] VariableProperty      Pointer to the output variable property.

  @retval     EFI_SUCCESS           The property of variable specified by the Name and Guid was got successfully.
  @retval     EFI_INVALID_PARAMETER Name, Guid or VariableProperty is NULL, or Name is an empty string.
  @retval     EFI_NOT_FOUND         The property of variable specified by the Name and Guid was not found.

**/
EFI_STATUS
EFIAPI
VarCheckVariablePropertyGet (
  IN CHAR16                         *Name,
  IN EFI_GUID                       *Guid,
  OUT VAR_CHECK_VARIABLE_PROPERTY   *VariableProperty
  );

/**
  Initialize variable quota.

**/
VOID
InitializeVariableQuota (
  VOID
  );

extern VARIABLE_MODULE_GLOBAL  *mVariableModuleGlobal;
extern BOOLEAN                  mForceVolatileVariable;
extern AUTH_VAR_LIB_CONTEXT_OUT mAuthContextOut;

/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  VariableName              Name of the variable to be found.
  @param[in]  VendorGuid                Variable vendor GUID to be found.
  @param[out] AuthVariableInfo          Pointer to AUTH_VARIABLE_INFO structure for
                                        output of the variable found.

  @retval     EFI_INVALID_PARAMETER     If VariableName is not an empty string,
                                        while VendorGuid is NULL.
  @retval     EFI_SUCCESS               Variable successfully found.
  @retval     EFI_NOT_FOUND             Variable not found

**/
EFI_STATUS
EFIAPI
VariableExLibFindVariable (
  IN  CHAR16                *VariableName,
  IN  EFI_GUID              *VendorGuid,
  OUT AUTH_VARIABLE_INFO    *AuthVariableInfo
  );

/**
  Finds next variable in storage blocks of volatile and non-volatile storage areas.

  This code finds next variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  VariableName              Name of the variable to be found.
  @param[in]  VendorGuid                Variable vendor GUID to be found.
  @param[out] AuthVariableInfo          Pointer to AUTH_VARIABLE_INFO structure for
                                        output of the next variable.

  @retval     EFI_INVALID_PARAMETER     If VariableName is not an empty string,
                                        while VendorGuid is NULL.
  @retval     EFI_SUCCESS               Variable successfully found.
  @retval     EFI_NOT_FOUND             Variable not found

**/
EFI_STATUS
EFIAPI
VariableExLibFindNextVariable (
  IN  CHAR16                *VariableName,
  IN  EFI_GUID              *VendorGuid,
  OUT AUTH_VARIABLE_INFO    *AuthVariableInfo
  );

/**
  Update the variable region with Variable information.

  @param[in] AuthVariableInfo          Pointer AUTH_VARIABLE_INFO structure for
                                       input of the variable.

  @retval    EFI_SUCCESS               The update operation is success.
  @retval    EFI_INVALID_PARAMETER     Invalid parameter.
  @retval    EFI_WRITE_PROTECTED       Variable is write-protected.
  @retval    EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
EFIAPI
VariableExLibUpdateVariable (
  IN AUTH_VARIABLE_INFO     *AuthVariableInfo
  );

/**
  Get scratch buffer.

  @param[in, out] ScratchBufferSize Scratch buffer size. If input size is greater than
                                    the maximum supported buffer size, this value contains
                                    the maximum supported buffer size as output.
  @param[out]     ScratchBuffer     Pointer to scratch buffer address.

  @retval         EFI_SUCCESS       Get scratch buffer successfully.
  @retval         EFI_UNSUPPORTED   If input size is greater than the maximum supported buffer size.

**/
EFI_STATUS
EFIAPI
VariableExLibGetScratchBuffer (
  IN OUT UINTN      *ScratchBufferSize,
  OUT    VOID       **ScratchBuffer
  );

/**
  This function is to check if the remaining variable space is enough to set
  all Variables from argument list successfully. The purpose of the check
  is to keep the consistency of the Variables to be in variable storage.

  Note: Variables are assumed to be in same storage.
  The set sequence of Variables will be same with the sequence of VariableEntry from argument list,
  so follow the argument sequence to check the Variables.

  @param[in] Attributes         Variable attributes for Variable entries.
  @param ...                    The variable argument list with type VARIABLE_ENTRY_CONSISTENCY *.
                                A NULL terminates the list. The VariableSize of
                                VARIABLE_ENTRY_CONSISTENCY is the variable data size as input.
                                It will be changed to variable total size as output.

  @retval   TRUE                Have enough variable space to set the Variables successfully.
  @retval   FALSE               No enough variable space to set the Variables successfully.

**/
BOOLEAN
EFIAPI
VariableExLibCheckRemainingSpaceForConsistency (
  IN UINT32                     Attributes,
  ...
  );

/**
  Return TRUE if at OS runtime.

  @retval TRUE If at OS runtime.
  @retval FALSE If at boot time.

**/
BOOLEAN
EFIAPI
VariableExLibAtRuntime (
  VOID
  );

/**
  Non-Volatile variable write service is ready event handler.

**/
VOID
EFIAPI
InstallVariableWriteReady (
  VOID
  );

#endif

