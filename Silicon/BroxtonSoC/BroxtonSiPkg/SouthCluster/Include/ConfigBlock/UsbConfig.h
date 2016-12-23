/** @file
  USB policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _USB_CONFIG_H_
#define _USB_CONFIG_H_

#define USB_CONFIG_REVISION 2

extern EFI_GUID gUsbConfigGuid;

#pragma pack (push,1)

typedef enum {
  ScUsbOverCurrentPin0 = 0,
  ScUsbOverCurrentPin1,
  ScUsbOverCurrentPinSkip,
  ScUsbOverCurrentPinMax
} SC_USB_OVERCURRENT_PIN;

#define XHCI_MODE_OFF 0
#define XHCI_MODE_ON  1

typedef struct {
  UINT8  Mode     : 2;    /// 0: Disable; 1: Enable, 2: Auto, 3: Smart Auto
  UINT8  Rsvdbits : 4;
} SC_USB30_CONTROLLER_SETTINGS;

typedef struct {
  UINT32   Enable    :  1;   ///< 0: Disable; <b>1: Enable</b>.
  UINT32   RsvdBits0 : 31;   ///< Reserved bits
  UINT8   OverCurrentPin;
  UINT8   Rsvd0[3];         ///< Reserved bytes, align to multiple 4.
  UINT32  Rsvd1[1];         ///< Reserved bytes
} SC_USB20_PORT_CONFIG;

typedef struct {
  UINT32  Enable    :  1;   ///< 0: Disable; <b>1: Enable</b>.
  UINT32  RsvdBits0 : 31;   ///< Reserved bits
  UINT8   OverCurrentPin;
  UINT8   Rsvd0[3];         ///< Reserved bytes, align to multiple 4
  UINT32  Rsvd1[2];         ///< Reserved bytes
} SC_USB30_PORT_CONFIG;

typedef struct {
  UINT32  Enable   : 2;
  UINT32  RsvdBits : 30;  ///< Reserved bits
} SC_XDCI_CONFIG;

typedef struct {
  UINT32  Enable          : 1;
  UINT32  RsvdBits        : 31;
} SC_XHCI_HSIC_PORT;

/**
  These members describe some settings which are related to the SSIC ports.

**/
typedef struct {
  SC_XHCI_HSIC_PORT  HsicPort[XHCI_MAX_HSIC_PORTS];
} SC_HSIC_CONFIG;

typedef enum {
  XhciSsicRateA = 1,
  XhciSsicRateB
} SC_XHCI_SSIC_PORT_RATE;

typedef struct {
  UINT32  Enable          : 1;
  UINT32  Rate            : 2;
  UINT32  RsvdBits        : 29;
} SC_XHCI_SSIC_PORT;

typedef struct {
  SC_XHCI_SSIC_PORT  SsicPort[XHCI_MAX_SSIC_PORTS];
  UINT32  DlanePwrGating : 1;
  UINT32  RsvdBits       : 31;
} SC_SSIC_CONFIG;

typedef struct {
  CONFIG_BLOCK_HEADER           Header;                    ///< Config Block Header
  UINT32                        DisableComplianceMode : 1;
  UINT32                        UsbPerPortCtl         : 1; ///< @deprecated since revision 2
  UINT32                        RsvdBits              : 30;
  SC_USB30_CONTROLLER_SETTINGS  Usb30Settings;
  SC_USB20_PORT_CONFIG          PortUsb20[SC_MAX_USB2_PORTS];
  SC_USB30_PORT_CONFIG          PortUsb30[SC_MAX_USB3_PORTS];
  SC_XDCI_CONFIG                XdciConfig;
  SC_HSIC_CONFIG                HsicConfig;
  SC_SSIC_CONFIG                SsicConfig;
} SC_USB_CONFIG;

#pragma pack (pop)

#endif // _USB_CONFIG_H_

