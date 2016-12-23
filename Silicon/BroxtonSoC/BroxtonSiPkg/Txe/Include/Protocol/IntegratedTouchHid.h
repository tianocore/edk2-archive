/** @file
  HID interface for IntegratedTouch feature.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _INTEGRATED_TOUCH_HID_PROTOCOL_H_
#define _INTEGRATED_TOUCH_HID_PROTOCOL_H_

typedef struct _ITOUCH_HID_PROTOCOL ITOUCH_HID_PROTOCOL;

/**
  Performs GetFeature function as described in Human Interface Device spec.

  @param[in]  This          Pointer to instance of protocol.
  @param[in]  Length        Size of buffer.
  @param[in]  Buffer        On input, contains ReportId in 1st byte. On output, filled with Feature data from iTouch.
  @param[in]  Timeout       Parameter added to support Capsule Update. If provided value is 0 default is set GET_SET_FEATURE_TIMEOUT (5000).

  @retval     EFI_SUCCESS   When iTouch responded with data.
  @retval     EFI_TIMEOUT   When there was no response.

**/
typedef
EFI_STATUS
(EFIAPI *ITOUCH_HID_GET_FEATURE)(
  IN ITOUCH_HID_PROTOCOL *This,
  IN UINT32              Length,
  IN OUT UINT8           *Buffer,
  IN UINTN               Timeout
  );

/**
  Performs SetFeature function as described in Human Interface Device spec.

  @param[in]  This       Pointer to instance of protocol.
  @param[in]  Length     Size of buffer.
  @param[in]  Buffer     On input, contains data to be sent to iTouch.
  @param[in]  Timeout    Parameter added to support Capsule Update. If provided value is 0 default is set GET_SET_FEATURE_TIMEOUT (5000).

  @retval     EFI_SUCCESS

**/
typedef
EFI_STATUS
(EFIAPI *ITOUCH_HID_SET_FEATURE)(
  IN ITOUCH_HID_PROTOCOL *This,
  IN UINT32              Length,
  IN UINT8               *Buffer,
  IN UINTN               Timeout
  );

/**
  IntegratedTouchHid protocol is instaled by iTouch driver on HECI3 device handle apart from
  AbsolutePointer protocol.
**/
struct _ITOUCH_HID_PROTOCOL {
  ITOUCH_HID_GET_FEATURE GetFeature;
  ITOUCH_HID_SET_FEATURE SetFeature;
  };

extern EFI_GUID gIntegratedTouchHidProtocolGuid;

#endif

