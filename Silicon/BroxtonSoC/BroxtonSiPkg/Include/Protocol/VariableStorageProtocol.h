/** @file
  This file declares the Variable Storage Protocol.

  This protocol abstracts read-only access to the UEFI variable store
  on a NVM (Non-Volatile Memory) device during the Runtime DXE phase.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _VARIABLE_STORAGE_PROTOCOL_H_
#define _VARIABLE_STORAGE_PROTOCOL_H_

extern EFI_GUID gVariableStorageProtocolGuid;

///
/// Revision
///
#define VARIABLE_STORAGE_PROTOCOL_REVISION  1

typedef struct _VARIABLE_STORAGE_PROTOCOL  VARIABLE_STORAGE_PROTOCOL;

/**
  Retrieves a protocol instance-specific GUID.

  Returns a unique GUID per VARIABLE_STORAGE_PROTOCOL instance.

  @param[out]      VariableGuid           A pointer to an EFI_GUID that is this protocol instance's GUID.

  @retval          EFI_SUCCESS            The data was returned successfully.
  @retval          EFI_INVALID_PARAMETER  A required parameter is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_ID)(
  OUT       EFI_GUID                        *InstanceGuid
  );

/**
  This service retrieves a variable's value using its name and GUID.

  Read the specified variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  @param[in]       This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.

  @retval          EFI_SUCCESS            The variable was read successfully.
  @retval          EFI_NOT_FOUND          The variable could not be found.
  @retval          EFI_BUFFER_TOO_SMALL   The DataSize is too small for the resulting data.
                                          DataSize is updated with the size required for
                                          the specified variable.
  @retval          EFI_INVALID_PARAMETER  VariableName, VariableGuid, DataSize or Data is NULL.
  @retval          EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_VARIABLE)(
  IN CONST  VARIABLE_STORAGE_PROTOCOL    *This,
  IN CONST  CHAR16                       *VariableName,
  IN CONST  EFI_GUID                     *VariableGuid,
  OUT       UINT32                       *Attributes,
  IN OUT    UINTN                        *DataSize,
  OUT       VOID                         *Data
  );

/**
  This service retrieves an authenticated variable's value using its name and GUID.

  Read the specified authenticated variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable,
  the error EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the
  required buffer size to obtain the data.

  @param[in]       This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]       VariableName           A pointer to a null-terminated string that is the variable's name.
  @param[in]       VariableGuid           A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                          VariableGuid and VariableName must be unique.
  @param[out]      Attributes             If non-NULL, on return, points to the variable's attributes.
  @param[in, out]  DataSize               On entry, points to the size in bytes of the Data buffer.
                                          On return, points to the size of the data returned in Data.
  @param[out]      Data                   Points to the buffer which will hold the returned variable value.
  @param[out]      KeyIndex               Index of associated public key in database
  @param[out]      MonotonicCount         Associated monotonic count value to protect against replay attack
  @param[out]      TimeStamp              Associated TimeStamp value to protect against replay attack

  @retval          EFI_SUCCESS            The variable was read successfully.
  @retval          EFI_NOT_FOUND          The variable could not be found.
  @retval          EFI_BUFFER_TOO_SMALL   The DataSize is too small for the resulting data.
                                          DataSize is updated with the size required for
                                          the specified variable.
  @retval          EFI_INVALID_PARAMETER  VariableName, VariableGuid, DataSize or Data is NULL.
  @retval          EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_AUTHENTICATED_VARIABLE)(
  IN CONST  VARIABLE_STORAGE_PROTOCOL    *This,
  IN CONST  CHAR16                       *VariableName,
  IN CONST  EFI_GUID                     *VariableGuid,
  OUT       UINT32                       *Attributes,
  IN OUT    UINTN                        *DataSize,
  OUT       VOID                         *Data,
  OUT       UINT32                       *KeyIndex,
  OUT       UINT64                       *MonotonicCount,
  OUT       EFI_TIME                     *TimeStamp
  );

/**
  Return the next variable name and GUID.

  This function is called multiple times to retrieve the VariableName
  and VariableGuid of all variables currently available in the system.
  On each call, the previous results are passed into the interface,
  and, on return, the interface returns the data for the next
  interface. When the entire variable list has been returned,
  EFI_NOT_FOUND is returned.

  @param[in]      This                   A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.

  @param[in, out] VariableNameSize       On entry, points to the size of the buffer pointed to by
                                         VariableName. On return, the size of the variable name buffer.
  @param[in, out] VariableName           On entry, a pointer to a null-terminated string that is the
                                         variable's name. On return, points to the next variable's
                                         null-terminated name string.
  @param[in, out] VariableGuid           On entry, a pointer to an EFI_GUID that is the variable's GUID.
                                         On return, a pointer to the next variable's GUID.
  @param[out]     VariableAttributes     A pointer to the variable attributes.

  @retval         EFI_SUCCESS            The variable was read successfully.
  @retval         EFI_NOT_FOUND          The variable could not be found.
  @retval         EFI_BUFFER_TOO_SMALL   The VariableNameSize is too small for the resulting
                                         data. VariableNameSize is updated with the size
                                         required for the specified variable.
  @retval         EFI_INVALID_PARAMETER  VariableName, VariableGuid or
                                         VariableNameSize is NULL.
  @retval         EFI_DEVICE_ERROR       The variable could not be retrieved because of a device error.
**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_NEXT_VARIABLE_NAME)(
  IN CONST  VARIABLE_STORAGE_PROTOCOL       *This,
  IN OUT    UINTN                           *VariableNameSize,
  IN OUT    CHAR16                          *VariableName,
  IN OUT    EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *VariableAttributes
  );

/**
  Returns information on the amount of space available in the variable store. If the amount of data that can be written
  depends on if the platform is in Pre-OS stage or OS stage, the AtRuntime parameter should be used to compute usage.

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  AtRuntime                      TRUE is the platform is in OS Runtime, FALSE if still in Pre-OS stage
  @param[out] VariableStoreSize              The total size of the NV storage. Indicates the maximum amount
                                             of data that can be stored in this NV storage area.
  @param[out] CommonVariablesTotalSize       The total combined size of all the common UEFI variables that are
                                             stored in this NV storage area. Excludes variables with the
                                             EFI_VARIABLE_HARDWARE_ERROR_RECORD attribute set.
  @param[out] HwErrVariablesTotalSize        The total combined size of all the UEFI variables that have the
                                             EFI_VARIABLE_HARDWARE_ERROR_RECORD attribute set and which are
                                             stored in this NV storage area. Excludes all other variables.

  @retval     EFI_INVALID_PARAMETER          Any of the given parameters are NULL
  @retval     EFI_SUCCESS                    Space information returned successfully.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_STORAGE_USAGE)(
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  IN          BOOLEAN                     AtRuntime,
  OUT         UINT32                      *VariableStoreSize,
  OUT         UINT32                      *CommonVariablesTotalSize,
  OUT         UINT32                      *HwErrVariablesTotalSize
  );

/**
  Returns whether this NV storage area supports storing authenticated variables or not

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[out] AuthSupported                  TRUE if this NV storage area can store authenticated variables,
                                             FALSE otherwise

  @retval     EFI_SUCCESS                    AuthSupported was returned successfully.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_GET_AUTHENTICATED_SUPPORT)(
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  OUT         BOOLEAN                     *AuthSupported
  );

/**
  Returns whether this NV storage area is ready to accept calls to SetVariable() or not

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.

  @retval     TRUE                           The NV storage area is ready to accept calls to SetVariable()
  @retval     FALSE                          The NV storage area is not ready to accept calls to SetVariable()

**/
typedef
BOOLEAN
(EFIAPI *VARIABLE_STORAGE_WRITE_SERVICE_IS_READY)(
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This
  );

/**
  A callback to be invoked by the VARIABLE_STORAGE_PROTOCOL to indicate to the core variable driver that
  the WriteServiceIsReady() function is now returning TRUE instead of FALSE.

  The VARIABLE_STORAGE_PROTOCOL is required to call this function as quickly as possible after the core
  variable driver invokes RegisterWriteServiceReadyCallback() to set the callback.

  @retval     EFI_SUCCESS                    Change to WriteServiceIsReady() status was processed successfully.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_WRITE_SERVICE_READY_CALLBACK)(
  VOID
  );

/**
  Sets the callback to be invoked when the VARIABLE_STORAGE_PROTOCOL is ready to accept calls to SetVariable()

  The VARIABLE_STORAGE_PROTOCOL is required to invoke the callback as quickly as possible after the core
  variable driver invokes RegisterWriteServiceReadyCallback() to set the callback.

  @param[in]  This                           A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  CallbackFunction               The callback function

  @retval     EFI_SUCCESS                    The callback function was sucessfully registered

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_REGISTER_WRITE_SERVICE_READY_CALLBACK)(
  IN CONST    VARIABLE_STORAGE_PROTOCOL             *This,
  IN VARIABLE_STORAGE_WRITE_SERVICE_READY_CALLBACK  CallbackFunction
  );

/**
  This code sets a variable's value using its name and GUID.

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parsing the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in]  This                             A pointer to this instance of the VARIABLE_STORAGE_PROTOCOL.
  @param[in]  VariableName                     Name of Variable to be found.
  @param[in]  VendorGuid                       Variable vendor GUID.
  @param[in]  Attributes                       Attribute value of the variable found
  @param[in]  DataSize                         Size of Data found. If size is less than the
                                               data, this value contains the required size.
  @param[in]  Data                             Data pointer.
  @param[in]  AtRuntime                        TRUE is the platform is in OS Runtime, FALSE if still in Pre-OS stage
  @param[in]  KeyIndex                         If writing an authenticated variable, the public key index
  @param[in]  MonotonicCount                   If writing a monotonic counter authenticated variable, the counter value
  @param[in]  TimeStamp                        If writing a timestamp authenticated variable, the timestamp value

  @retval     EFI_INVALID_PARAMETER            Invalid parameter.
  @retval     EFI_SUCCESS                      Set successfully.
  @retval     EFI_OUT_OF_RESOURCES             Resource not enough to set variable.
  @retval     EFI_NOT_FOUND                    Not found.
  @retval     EFI_WRITE_PROTECTED              Variable is read-only.

**/
typedef
EFI_STATUS
(EFIAPI *VARIABLE_STORAGE_SET_VARIABLE)(
  IN CONST    VARIABLE_STORAGE_PROTOCOL   *This,
  IN          CHAR16                      *VariableName,
  IN          EFI_GUID                    *VendorGuid,
  IN          UINT32                      Attributes,
  IN          UINTN                       DataSize,
  IN          VOID                        *Data,
  IN          BOOLEAN                     AtRuntime,
  IN          UINT32                      KeyIndex       OPTIONAL,
  IN          UINT64                      MonotonicCount OPTIONAL,
  IN          EFI_TIME                    *TimeStamp     OPTIONAL
  );

///
/// Variable Storage Protocol
/// Interface functions for variable NVM storage access in DXE phase.
///
struct _VARIABLE_STORAGE_PROTOCOL {
  VARIABLE_STORAGE_GET_ID                                 GetId;                              ///< Retrieves a protocol instance-specific GUID
  VARIABLE_STORAGE_GET_VARIABLE                           GetVariable;                        ///< Retrieves a variable's value given its name and GUID
  VARIABLE_STORAGE_GET_AUTHENTICATED_VARIABLE             GetAuthenticatedVariable;           ///< Retrieves an authenticated variable's value given its name and GUID
  VARIABLE_STORAGE_GET_NEXT_VARIABLE_NAME                 GetNextVariableName;                ///< Return the next variable name and GUID
  VARIABLE_STORAGE_GET_STORAGE_USAGE                      GetStorageUsage;                    ///< Returns information on storage usage in the variable store
  VARIABLE_STORAGE_GET_AUTHENTICATED_SUPPORT              GetAuthenticatedSupport;            ///< Returns whether this NV storage area supports authenticated variables
  VARIABLE_STORAGE_SET_VARIABLE                           SetVariable;                        ///< Sets a variable's value using its name and GUID.
  VARIABLE_STORAGE_WRITE_SERVICE_IS_READY                 WriteServiceIsReady;                ///< Indicates if SetVariable() is ready or not
  VARIABLE_STORAGE_REGISTER_WRITE_SERVICE_READY_CALLBACK  RegisterWriteServiceReadyCallback;  ///< Sets the callback to notify that SetVariable() is ready
};

#endif

