/** @file
  This file defines the SC S3 support Protocol.

  Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_S3_SUPPORT_PROTOCOL_H_
#define _SC_S3_SUPPORT_PROTOCOL_H_

///
/// Define the SC S3 Support protocol GUID
///
#define EFI_SC_S3_SUPPORT_PROTOCOL_GUID \
  { \
    0xe287d20b, 0xd897, 0x4e1e, 0xa5, 0xd9, 0x97, 0x77, 0x63, 0x93, 0x6a, 0x4 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                             gEfiScS3SupportProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SC_S3_SUPPORT_PROTOCOL EFI_SC_S3_SUPPORT_PROTOCOL;

typedef enum {
  ScS3ItemTypeSendCodecCommand,
  ScS3ItemTypeInitPcieRootPortDownstream,
  ScS3ItemTypePcieSetPm,
  ScS3ItemTypeMax
} EFI_SC_S3_DISPATCH_ITEM_TYPE;

///
/// It's better not to use pointer here because the size of pointer in DXE is 8, but it's 4 in PEI
/// plug 4 to ParameterSize in PEIM if you really need it
///
typedef struct {
  UINT32                        HdaBar;
  UINT32                        CodecCmdData;
} EFI_SC_S3_PARAMETER_SEND_CODEC_COMMAND;

typedef struct {
  UINT8                         RootPortBus;
  UINT8                         RootPortDevice;
  UINT8                         RootPortFunc;
  UINT8                         TempBusNumberMin;
  UINT8                         TempBusNumberMax;
} EFI_SC_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM;

typedef struct {
  UINT8                         RootPortBus;
  UINT8                         RootPortDevice;
  UINT8                         RootPortFunc;
  UINT8                         NumOfDevAspmOverride;
  UINT32                        DevAspmOverrideAddr;
  UINT8                         TempBusNumberMin;
  UINT8                         TempBusNumberMax;
  UINT8                         NumOfDevLtrOverride;
  UINT32                        DevLtrOverrideAddr;
} EFI_SC_S3_PARAMETER_PCIE_SET_PM;

typedef struct {
  EFI_SC_S3_DISPATCH_ITEM_TYPE Type;
  VOID                          *Parameter;
} EFI_SC_S3_DISPATCH_ITEM;

//
// Member functions
//
/**
  Set an item to be dispatched at S3 resume time. At the same time, the entry point
  of the SC S3 support image is returned to be used in subsequent boot script save
  call.

  @param[in]  This                       Pointer to the protocol instance.
  @param[in]  DispatchItem               The item to be dispatched.
  @param[out] S3DispatchEntryPoint       The entry point of the SC S3 support image.

  @retval     EFI_STATUS                 Successfully completed.
  @retval     EFI_OUT_OF_RESOURCES       Out of resources.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SC_S3_SUPPORT_SET_S3_DISPATCH_ITEM) (
  IN     EFI_SC_S3_SUPPORT_PROTOCOL   * This,
  IN     EFI_SC_S3_DISPATCH_ITEM      * DispatchItem,
  OUT    EFI_PHYSICAL_ADDRESS         * S3DispatchEntryPoint
  );

/**
  This is Protocol is used to set an item to be dispatched at S3 resume time.
  At the same time, the entry point of the SC S3 support image is returned to
 be used in subsequent boot script save call.
**/
struct _EFI_SC_S3_SUPPORT_PROTOCOL {
  EFI_SC_S3_SUPPORT_SET_S3_DISPATCH_ITEM SetDispatchItem;  ///< Set the item to be dispatched at S3 resume time.
};

#endif

