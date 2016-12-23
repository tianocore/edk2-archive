/** @file
  This file is part of the IGD OpRegion Implementation. The IGD OpRegion is
  an interface between system BIOS, ASL code, and Graphics drivers.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IGD_OPREGION_PROTOCOL_H_
#define _IGD_OPREGION_PROTOCOL_H_

//
// OpRegion / Software SCI protocol GUID
//
#define IGD_OPREGION_PROTOCOL_GUID \
  { \
    0xcdc5dddf, 0xe79d, 0x41ec, 0xa9, 0xb0, 0x65, 0x65, 0x49, 0xd, 0xb9, 0xd3 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gIgdOpRegionProtocolGuid;

//
// Forward reference for pure ANSI compatability
//
typedef struct _IGD_OPREGION_PROTOCOL IGD_OPREGION_PROTOCOL;

//
// Protocol data definitions
//

//
// OpRegion structures:
// Sub-structures define the different parts of the OpRegion followed by the
// main structure representing the entire OpRegion.
//
// Note: These structures are packed to 1 byte offsets because the exact
// data location is requred by the supporting design specification due to
// the fact that the data is used by ASL and Graphics driver code compiled
// separatly.
//
#pragma pack(1)
//
// OpRegion header (mailbox 0) structure and defines.
//
typedef struct {
  CHAR8   SIGN[0x10]; ///< Offset 0    OpRegion signature
  UINT32  SIZE;       ///< Offset 16   OpRegion size
  UINT32  OVER;       ///< Offset 20   OpRegion structure version
  UINT8   SVER[0x20]; ///< Offset 24   System BIOS build version
  UINT8   VVER[0x10]; ///< Offset 56   Video BIOS build version
  UINT8   GVER[0x10]; ///< Offset 72   Graphic driver build version
  UINT32  MBOX;       ///< Offset 88   Mailboxes supported
  UINT32  DMOD;       ///< Offset 92   Driver Model
  UINT32  PCON;       ///< Offset 96   Platform Capabilities
  CHAR16  DVER[0x10]; ///< Offset 100  GOP Version
  UINT8   RHD1[0x7C]; ///< Offset 132  Reserved
} OPREGION_HEADER;
#pragma pack()

#pragma pack(1)
typedef struct {
  UINT32  DRDY;       ///< Offset 0    Driver readiness
  UINT32  CSTS;       ///< Offset 4    Status
  UINT32  CEVT;       ///< Offset 8    Current event
  UINT8   RM11[0x14]; ///< Offset 12   Reserved
  UINT32  DIDL;       ///< Offset 32   Supported display device 1
  UINT32  DDL2;       ///< Offset 36   Supported display device 2
  UINT32  DDL3;       ///< Offset 40   Supported display device 3
  UINT32  DDL4;       ///< Offset 44   Supported display device 4
  UINT32  DDL5;       ///< Offset 48   Supported display device 5
  UINT32  DDL6;       ///< Offset 52   Supported display device 6
  UINT32  DDL7;       ///< Offset 56   Supported display device 7
  UINT32  DDL8;       ///< Offset 60   Supported display device 8
  UINT32  CPDL;       ///< Offset 64   Currently present display device 1
  UINT32  CPL2;       ///< Offset 68   Currently present display device 2
  UINT32  CPL3;       ///< Offset 72   Currently present display device 3
  UINT32  CPL4;       ///< Offset 76   Currently present display device 4
  UINT32  CPL5;       ///< Offset 80   Currently present display device 5
  UINT32  CPL6;       ///< Offset 84   Currently present display device 6
  UINT32  CPL7;       ///< Offset 88   Currently present display device 7
  UINT32  CPL8;       ///< Offset 92   Currently present display device 8
  UINT32  CADL;       ///< Offset 96   Currently active display device 1
  UINT32  CAL2;       ///< Offset 100  Currently active display device 2
  UINT32  CAL3;       ///< Offset 104  Currently active display device 3
  UINT32  CAL4;       ///< Offset 108  Currently active display device 4
  UINT32  CAL5;       ///< Offset 112  Currently active display device 5
  UINT32  CAL6;       ///< Offset 116  Currently active display device 6
  UINT32  CAL7;       ///< Offset 120  Currently active display device 7
  UINT32  CAL8;       ///< Offset 124  Currently active display device 8
  UINT32  NADL;       ///< Offset 128  Next active device 1
  UINT32  NDL2;       ///< Offset 132  Next active device 2
  UINT32  NDL3;       ///< Offset 136  Next active device 3
  UINT32  NDL4;       ///< Offset 140  Next active device 4
  UINT32  NDL5;       ///< Offset 144  Next active device 5
  UINT32  NDL6;       ///< Offset 148  Next active device 6
  UINT32  NDL7;       ///< Offset 152  Next active device 7
  UINT32  NDL8;       ///< Offset 156  Next active device 8
  UINT32  ASLP;       ///< Offset 160  ASL sleep timeout
  UINT32  TIDX;       ///< Offset 164  Toggle table index
  UINT32  CHPD;       ///< Offset 168  Current hot plug enable indicator
  UINT32  CLID;       ///< Offset 172  Current lid state indicator
  UINT32  CDCK;       ///< Offset 176  Current docking state indicator
  UINT32  SXSW;       ///< Offset 180  Display Switch notification on Sx State resume
  UINT32  EVTS;       ///< Offset 184  Events supported by ASL
  UINT32  CNOT;       ///< Offset 188  Current OS Notification
  UINT32  NRDY;       ///< Offset 192  Reasons for DRDY = 0
  UINT32  DDL9;       ///< Offset 196  Extended Supported display device 1
  UINT32  DD10;       ///< Offset 200  Extended Supported display device 2
  UINT32  DD11;       ///< Offset 204  Extended Supported display device 3
  UINT32  DD12;       ///< Offset 208  Extended Supported display device 4
  UINT32  DD13;       ///< Offset 212  Extended Supported display device 5
  UINT32  DD14;       ///< Offset 216  Extended Supported display device 6
  UINT32  DD15;       ///< Offset 220  Extended Supported display device 7
  UINT32  CPL9;       ///< Offset 224  Extended Currently present device 1
  UINT32  CP10;       ///< Offset 228  Extended Currently present device 2
  UINT32  CP11;       ///< Offset 232  Extended Currently present device 3
  UINT32  CP12;       ///< Offset 236  Extended Currently present device 4
  UINT32  CP13;       ///< Offset 240  Extended Currently present device 5
  UINT32  CP14;       ///< Offset 244  Extended Currently present device 6
  UINT32  CP15;       ///< Offset 248  Extended Currently present device 7
  UINT8   RM12[0x4];  ///< Offset 252  Reserved 4 bytes
} OPREGION_MBOX1;
#pragma pack()

#pragma pack(1)
//
// OpRegion mailbox 2 (Software SCI Interface).
//
typedef struct {
  UINT32  SCIC;       ///< Offset 0    Software SCI function number parameters
  UINT32  PARM;       ///< Offset 4    Software SCI additional parameters
  UINT32  DSLP;       ///< Offset 8    Driver sleep timeout
  UINT8   RM21[0xF4]; ///< Offset 12   Reserved
} OPREGION_MBOX2;
#pragma pack()

#pragma pack(1)
//
// OpRegion mailbox 3 (Power Conservation).
//
typedef struct {
  UINT32  ARDY;       ///< Offset 0    Driver readiness
  UINT32  ASLC;       ///< Offset 4    ASLE interrupt command / status
  UINT32  TCHE;       ///< Offset 8    Technology enabled indicator
  UINT32  ALSI;       ///< Offset 12   Current ALS illuminance reading
  UINT32  BCLP;       ///< Offset 16   Backlight britness to set
  UINT32  PFIT;       ///< Offset 20   Panel fitting Request
  UINT32  CBLV;       ///< Offset 24   Brightness Current State
  UINT16  BCLM[0x14]; ///< Offset 28   Backlight Brightness Level Duty Cycle Mapping Table
  UINT32  CPFM;       ///< Offset 68   Panel Fitting Current Mode
  UINT32  EPFM;       ///< Offset 72   Enabled Panel Fitting Modes
  UINT8   PLUT[0x4A]; ///< Offset 76   Panel Look Up Table
  UINT32  PFMB;       ///< Offset 150  PWM Frequency and Minimum Brightness
  UINT32  CCDV;       ///< Offset 154  Color Correction Default Values
  UINT32  PCFT;       ///< Offset 158  Power Conservation Features
  UINT32  SROT;       ///< Offset 162  Supported Rotation angle
  UINT32  IUER;       ///< Offset 166  Intel Ultrabook Event Register
  UINT64  FDSP;       ///< Offset 170  FFS Display Physical address
  UINT32  FDSS;       ///< Offset 178  FFS Display Size
  UINT32  STAT;       ///< Offset 182  State Indicator
  UINT64  RVDA;       ///< Offset 186  (Igd opregion offset 0x3BAh) Physical address of Raw VBT data
  UINT32  RVDS;       ///< Offset 194  (Igd opregion offset 0x3C2h) Size of Raw VBT data
  UINT8   RMEM[0x3A]; ///< Offset 198  Reserved
} OPREGION_MBOX3;
#pragma pack()

#pragma pack(1)
//
// OpRegion mailbox 4 (VBT).
//
typedef struct {
  UINT8   GVD1[0x1800]; ///< Offset 1024 6K Reserved
} OPREGION_VBT;
#pragma pack ()

#pragma pack (1)
typedef struct {
  UINT32  PHED;         ///< Offset 7168 Panel Header
  UINT8   BDDC[0x100];  ///< Offset 7172 Panel EDID
  UINT8   RM51[0x2FC];  ///< Offset 7428 764 bytes
} OPREGION_MBOX5;
#pragma pack ()
//
// Entire OpRegion
//
#pragma pack (1)
typedef struct {
  OPREGION_HEADER Header; ///< OpRegion header
  OPREGION_MBOX1  MBox1;  ///< Mailbox 1: Public ACPI Methods
  OPREGION_MBOX2  MBox2;  ///< Mailbox 2: Software SCI Inteface
  OPREGION_MBOX3  MBox3;  ///< Mailbox 3: Power Conservation
  OPREGION_VBT    VBT;    ///< VBT: Video BIOS Table (OEM customizable data)
  OPREGION_MBOX5  MBox5;  ///< Mailbox 5: BIOS to Driver Notification Extension
} IGD_OPREGION_STRUC;
#pragma pack()
//
// IGD OpRegion Protocol
//
struct _IGD_OPREGION_PROTOCOL {
  IGD_OPREGION_STRUC  *OpRegion; ///< IGD Operation Region Structure
};

#endif

