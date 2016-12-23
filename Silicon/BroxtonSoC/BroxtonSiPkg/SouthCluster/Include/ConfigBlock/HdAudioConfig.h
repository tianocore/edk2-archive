/** @file
  HD-Audio policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HDAUDIO_CONFIG_H_
#define _HDAUDIO_CONFIG_H_

#define HDAUDIO_CONFIG_REVISION 3

extern EFI_GUID gHdAudioConfigGuid;

#pragma pack (push,1)

typedef struct {
  UINT32  VendorDeviceId;       ///< Codec Vendor/Device ID
  UINT8   RevisionId;           ///< Revision ID of the codec. 0xFF matches any revision.
  UINT8   SdiNo;                ///< SDI number, 0xFF matches any SDI.
  UINT16  DataDwords;           ///< Number of data DWORDs following the header.
} HDA_VERB_TABLE_HEADER;

typedef struct {
  HDA_VERB_TABLE_HEADER         VerbTableHeader;
  UINT32                        VerbTableData[];
} HDAUDIO_VERB_TABLE;

//
// The SC_HDAUDIO_CONFIG block describes the expected configuration of the Intel HD Audio feature.
//
enum SC_HDAUDIO_IO_BUFFER_OWNERSHIP {
  ScHdaIoBufOwnerHdaLink        = 0,   ///< HD-Audio link owns all the I/O buffers.
  ScHdaIoBufOwnerHdaLinkI2sPort = 1,   ///< HD-Audio link owns 4 and I2S port owns 4 of the I/O buffers.
  ScHdaIoBufOwnerI2sPort        = 3    ///< I2S0 and I2S1 ports own all the I/O buffers.
};

enum SC_HDAUDIO_IO_BUFFER_VOLTAGE {
  ScHdaIoBuf33V = 0,
  ScHdaIoBuf18V = 1
};

enum SC_HDAUDIO_DMIC_TYPE {
  ScHdaDmicDisabled = 0,
  ScHdaDmic2chArray = 1,
  ScHdaDmic4chArray = 2
};

enum SC_HDAUDIO_VC_TYPE {
  ScHdaVc0 = 0,
  ScHdaVc1 = 1,
  ScHdaVc2 = 2
};

typedef enum {
  ScHdaLinkFreq6MHz  = 0,
  ScHdaLinkFreq12MHz = 1,
  ScHdaLinkFreq24MHz = 2,
  ScHdaLinkFreq48MHz = 3,
  ScHdaLinkFreq96MHz = 4,
  ScHdaLinkFreqInvalid
} SC_HDAUDIO_LINK_FREQUENCY;

typedef enum  {
  ScHdaIDispMode2T = 0,
  ScHdaIDispMode1T = 1
} SC_HDAUDIO_IDISP_TMODE;

typedef struct {
  CONFIG_BLOCK_HEADER   Header;       ///< Config Block Header
  UINT32 Enable               : 2;
  UINT32 DspEnable            : 1;    ///< DSP enablement: 0: Disable; <b>1: Enable</b>
  UINT32 Pme                  : 1;    ///< Azalia wake-on-ring, <b>0: Disable</b>; 1: Enable
  UINT32 IoBufferOwnership    : 2;    ///< I/O Buffer Ownership Select: <b>0: HD-A Link</b>; 1: Shared, HD-A Link and I2S Port; 3: I2S Ports
  UINT32 IoBufferVoltage      : 1;    ///< I/O Buffer Voltage Mode Select: <b>0: 3.3V</b>; 1: 1.8V
  UINT32 VcType               : 1;    ///< Virtual Channel Type Select: <b>0: VC0</b>, 1: VC1
  UINT32 HdAudioLinkFrequency : 4;    ///< HDA-Link frequency (SC_HDAUDIO_LINK_FREQUENCY enum): <b>2: 24MHz</b>, 1: 12MHz, 0: 6MHz
  UINT32 IDispLinkFrequency   : 4;    ///< iDisp-Link frequency (SC_HDAUDIO_LINK_FREQUENCY enum): <b>4: 96MHz</b>, 3: 48MHz
  UINT32 IDispLinkTmode       : 1;    ///< iDisp-Link T-Mode (SC_HDAUDIO_IDISP_TMODE enum): <b>0: 2T</b>, 1: 1T
  UINT32 RsvdBits0            : 15;   ///< Reserved bits
  UINT32 DspEndpointDmic      : 2;    ///< DMIC Select (SC_HDAUDIO_DMIC_TYPE enum): 0: Disable; 1: 2ch array; <b>2: 4ch array</b>
  UINT32 DspEndpointBluetooth : 1;    ///< Bluetooth enablement: <b>0: Disable</b>; 1: Enable
  UINT32 DspEndpointI2sSkp    : 1;    ///< I2S SHK enablement: <b>0: Disable</b>; 1: Enable
  UINT32 DspEndpointI2sHp     : 1;    ///< I2S HP enablement: <b>0: Disable</b>; 1: Enable
  UINT32 AudioCtlPwrGate      : 1;    ///< Deprecated
  UINT32 AudioDspPwrGate      : 1;    ///< Deprecated
  UINT32 Mmt                  : 1;    ///< CSME Memory Transfers : 0: VC0, 1: VC2
  UINT32 Hmt                  : 1;    ///< Host Memory Transfers : 0: VC0, 1: VC2
  UINT32 BiosCfgLockDown      : 1;    ///< BIOS Configuration Lock Down : 0: Disable, 1: Enable
  UINT32 PwrGate              : 1;    ///< Power Gating : 0: Disable, 1: Enable
  UINT32 ClkGate              : 1;    ///< Clock Gating : 0: Disable, 1: Enable
  UINT32 RsvdBits1            : 20;   ///< Reserved bits
  UINT32 DspFeatureMask;
  UINT32 DspPpModuleMask;
  UINT16 ResetWaitTimer;              ///< < <b>(Test)</b> The delay timer after Azalia reset, the value is number of microseconds. Default is <b>300</b>.
  UINT8  Rsvd0[2];                    ///< Reserved bytes, align to multiple 4
  UINT32 RsvdBits3             : 2;
  UINT32 RsvdBits2             : 30;  ///< Reserved bits
  UINT8  VerbTableEntryNum;
  UINT32 VerbTablePtr;
} SC_HDAUDIO_CONFIG;

#pragma pack (pop)

#endif // _HDAUDIO_CONFIG_H_

