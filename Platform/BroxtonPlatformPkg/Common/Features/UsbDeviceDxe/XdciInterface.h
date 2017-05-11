/** @file
  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _USB_DCD_IF_H_
#define _USB_DCD_IF_H_

/* Core driver for device controller
 * @DevCoreInit: Intializes device controller
 * @DevCoreDeinit: De-initializes device controller
 * @DevCoreRegisterCallback: Registers callback function for
 * an event to be called by the controller driver
 * @DevCoreUnregisterCallback: Unregisters callback function
 * for an event
 * @DevCoreIsrRoutine: core interrupt service routine for
 * device controller to be used by OS/stack-i/f layer
 * @DevCoreConnect: Enable device controller to connect to USB host
 * @DevCoreDisconnect: Soft disconnect device controller from
 * USB host
 * @DevCoreGetSpeed: Get USB bus Speed on which device controller
 * is attached
 * @DevCoreSetAddress: Set USB device address in device controller
 * @DevCoreSetConfig: Set configuration number for device controller
 * @DevCoreSetLinkState: Set link state for device controller
 * @DevCoreInitEp: Initialize non-EP0 endpoint
 * @DevCoreEpEnable: Enable endpoint
 * @DevCoreEpDisable: Disable endpoint
 * @DevCoreEpStall: Stall/Halt endpoint
 * @DevCoreEpClearStall: Clear Stall/Halt on endpoint
 * @DevCoreEpSetNrdy: Set endpoint to not ready state
 * @DevCoreEp0RxSetupPkt: Receive SETUP packet on EP0
 * @DevCoreEp0RxStatusPkt: Receive status packet on EP0
 * @DevCoreEp0TxStatusPkt: Transmit status packet from EP0
 * @DevCoreEpTxData: Transmit data from EP
 * @DevCoreEpRxData: Received data on EP
 * @DevCoreEpCancelTransfer: Cancel transfer on EP
 */

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_INIT) (
  IN USB_DEV_CONFIG_PARAMS     *ConfigParams,
  IN VOID                      *ParentHandle,
  IN VOID                      **CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_DEINIT) (
  IN VOID                      *CoreHandle,
  IN UINT32                    Flags
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_REG_CALLBACK) (
  IN VOID                      *CoreHandle,
  IN USB_DEVICE_EVENT_ID       Event,
  IN USB_DEVICE_CALLBACK_FUNC  CallbackFn
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_UNREG_CALLBACK) (
  IN VOID                      *CoreHandle,
  IN USB_DEVICE_EVENT_ID       Event
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_ISR_ROUTINE) (
  IN VOID                      *CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_CONNECT) (
  IN VOID                      *CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_DISCONNECT) (
  IN VOID                      *CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_GET_SPEED) (
  IN VOID                      *CoreHandle,
  IN USB_SPEED                 *Speed
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_SET_ADDRESS) (
  IN VOID                      *CoreHandle,
  IN UINT32                    Address
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_SET_CONFIG) (
  IN VOID                      *CoreHandle,
  IN UINT32                    ConfigNum
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_SET_LINK_STATE) (
  IN VOID                      *CoreHandle,
  IN USB_DEVICE_SS_LINK_STATE  State
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_INIT_EP) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_ENABLE) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_DISABLE) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_STALL) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_CLEAR_STALL) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_SET_NRDY) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP0_RX_SETUP_PKT) (
  IN VOID                      *CoreHandle,
  IN UINT8                     *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP0_RX_STATUS_PKT) (
  IN VOID                      *CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP0_TX_STATUS_PKT) (
  IN VOID                      *CoreHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_TX_DATA) (
  IN VOID                      *CoreHandle,
  IN USB_XFER_REQUEST          *XferHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_RX_DATA) (
  IN VOID                      *CoreHandle,
  IN USB_XFER_REQUEST          *XferHandle
  );

typedef
EFI_STATUS
(EFIAPI *DEV_CORE_EP_CANCEL_TRANSFER) (
  IN VOID                      *CoreHandle,
  IN USB_EP_INFO               *EpInfo
  );

struct UsbDeviceCoreDriver {
  DEV_CORE_INIT                 DevCoreInit;
  DEV_CORE_DEINIT               DevCoreDeinit;
  DEV_CORE_REG_CALLBACK         DevCoreRegisterCallback;
  DEV_CORE_UNREG_CALLBACK       DevCoreUnregisterCallback;
  DEV_CORE_ISR_ROUTINE          DevCoreIsrRoutine;
  DEV_CORE_ISR_ROUTINE          DevCoreIsrRoutineTimerBased;
  DEV_CORE_CONNECT              DevCoreConnect;
  DEV_CORE_DISCONNECT           DevCoreDisconnect;
  DEV_CORE_GET_SPEED            DevCoreGetSpeed;
  DEV_CORE_SET_ADDRESS          DevCoreSetAddress;
  DEV_CORE_SET_CONFIG           DevCoreSetConfig;
  DEV_CORE_SET_LINK_STATE       DevCoreSetLinkState;
  DEV_CORE_INIT_EP              DevCoreInitEp;
  DEV_CORE_EP_ENABLE            DevCoreEpEnable;
  DEV_CORE_EP_DISABLE           DevCoreEpDisable;
  DEV_CORE_EP_STALL             DevCoreEpStall;
  DEV_CORE_EP_CLEAR_STALL       DevCoreEpClearStall;
  DEV_CORE_EP_SET_NRDY          DevCoreEpSetNrdy;
  DEV_CORE_EP0_RX_SETUP_PKT     DevCoreEp0RxSetupPkt;
  DEV_CORE_EP0_RX_STATUS_PKT    DevCoreEp0RxStatusPkt;
  DEV_CORE_EP0_TX_STATUS_PKT    DevCoreEp0TxStatusPkt;
  DEV_CORE_EP_TX_DATA           DevCoreEpTxData;
  DEV_CORE_EP_RX_DATA           DevCoreEpRxData;
  DEV_CORE_EP_CANCEL_TRANSFER   DevCoreEpCancelTransfer;
};

//
// This API is used to obtain the driver handle for HW-independent API
// @id: The ID of the core for which this driver is requested
//
const struct UsbDeviceCoreDriver *UsbDeviceGetCoreDriver(
  USB_CONTROLLER_ID id);

#endif

