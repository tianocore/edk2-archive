/** @file
  Header file for ScHdaLib Endpoint descriptors.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_HDA_ENDPOINTS_H_
#define _SC_HDA_ENDPOINTS_H_

#include <Private/Library/DxeScHdaNhlt.h>

typedef enum {
  HdaDmicX2       = 0,
  HdaDmicX4,
  HdaBtRender,
  HdaBtCapture,
  HdaI2sRenderSKP,
  HdaI2sCaptureSKP,
  HdaI2sRenderHP,
  HdaI2sCaptureHP,
  HdaModem1Render,
  HdaModem1Capture,
  HdaModem2Render,
  HdaModem2Capture,
  HdaEndpointMax
} NHLT_ENDPOINT;

typedef struct {
  NHLT_ENDPOINT EndpointType;
  BOOLEAN Enable;
} SC_HDA_NHLT_ENDPOINTS;

#define SC_HDA_NHLT_TABLE_SIZE 0x2000

//
// Format bitmask
//
#define B_HDA_2CH_48KHZ_16BIT_FORMAT BIT0
#define B_HDA_2CH_48KHZ_24BIT_FORMAT BIT1
#define B_HDA_2CH_48KHZ_32BIT_FORMAT BIT2
#define B_HDA_4CH_48KHZ_16BIT_FORMAT BIT3
#define B_HDA_4CH_48KHZ_32BIT_FORMAT BIT4
#define B_HDA_NARROWBAND_FORMAT      BIT5
#define B_HDA_WIDEBAND_FORMAT        BIT6
#define B_HDA_A2DP_FORMAT            BIT7
#define V_HDA_FORMAT_MAX             8

//
// Formats
//
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz24bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE NarrowbandFormat;
extern CONST WAVEFORMATEXTENSIBLE WidebandFormat;
extern CONST WAVEFORMATEXTENSIBLE A2dpFormat;

//
// Format Config
//
extern CONST UINT32 DmicStereo16BitFormatConfig[];
extern CONST UINT32 DmicStereo32BitFormatConfig[];
extern CONST UINT32 DmicQuad16BitFormatConfig[];
extern CONST UINT32 DmicQuad32BitFormatConfig[];
extern CONST UINT32 DmicFormatConfigSize;
extern CONST UINT32 DmicCommonFormatConfig[];
extern CONST UINT32 DmicCommonFormatConfigSize;

extern CONST UINT32 I2sFormatConfigRender[];
extern CONST UINT32 I2sFormatConfigRenderSize;
extern CONST UINT32 I2sFormatConfigRender_Bxtp[];
extern CONST UINT32 I2sFormatConfigRenderSize_Bxtp;
extern CONST UINT32 I2sFormatConfigCapture[];
extern CONST UINT32 I2sFormatConfigCaptureSize;
extern CONST UINT32 I2sFormatConfigCapture_Bxtp[];
extern CONST UINT32 I2sFormatConfigCaptureSize_Bxtp;
extern CONST UINT32 BtFormatConfig[];
extern CONST UINT32 BtFormatConfigSize;

//
// Endpoints
//
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX2;
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX4;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtRender;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtCapture;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sRenderSKP;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sRenderHP;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sCapture;
extern ENDPOINT_DESCRIPTOR  HdaEndpointModem1Render;
extern ENDPOINT_DESCRIPTOR  HdaEndpointModem1Capture;
extern ENDPOINT_DESCRIPTOR  HdaEndpointModem2Render;
extern ENDPOINT_DESCRIPTOR  HdaEndpointModem2Capture;

//
// Endpoint Config
//
extern CONST UINT8 DmicX2Config[];
extern CONST UINT32 DmicX2ConfigSize;
extern CONST UINT8 DmicX4Config[];
extern CONST UINT32 DmicX4ConfigSize;
extern CONST UINT8 BtConfig[];
extern CONST UINT32 BtConfigSize;
extern CONST UINT8 I2sConfig[];
extern CONST UINT32 I2sConfigSize;
extern CONST UINT8 I2sConfigSKP[];
extern CONST UINT32 I2sConfigSKPSize;
extern CONST UINT8 Modem1Config[];
extern CONST UINT32 Modem1ConfigSize;
extern CONST UINT8 Modem2Config[];
extern CONST UINT32 Modem2ConfigSize;

//
// Oed Configuration
//
extern CONST UINT32 NhltConfiguration[];
extern CONST UINT32 NhltConfigurationSize;

#endif // _SC_HDA_ENDPOINTS_H_

