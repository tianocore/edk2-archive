/** @file
  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "HdaVerbTables.h"

HDAUDIO_VERB_TABLE HdaVerbTableAlc662 = {
  //
  // VerbTable: (Realtek ALC662)
  // Revision ID = 0xff
  // Codec Verb Table for IOTG CRB boards
  // Codec Address: CAd value (0/1/2)
  // Codec Vendor: 0x10EC0662
  //
  {
    0x10EC0662,     // Vendor ID / Device ID
    0xFF,           // Revision ID
    0xFF,           // SDI number, 0xFF matches any SDI.
    15 * 4          // Number of data DWORDs following the header.
  },
  {
    //
    // Realtek Semiconductor Corp.
    //
    // Realtek High Definition Audio Configuration - Version : 5.0.2.6
    // Realtek HD Audio Codec : ALC662-VD
    // PCI PnP ID : PCI\VEN_8086&DEV_2668&SUBSYS_72708086
    // HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0662&SUBSYS_80860000
    // The number of verb command block : 15
    //
    //     NID 0x12 : 0x40130000
    //     NID 0x14 : 0x01014010
    //     NID 0x15 : 0x01011012
    //     NID 0x16 : 0x01016011
    //     NID 0x18 : 0x01A19030
    //     NID 0x19 : 0x02A19040
    //     NID 0x1A : 0x0181303F
    //     NID 0x1B : 0x0221401F
    //     NID 0x1C : 0x411111F0
    //     NID 0x1D : 0x4045E601
    //     NID 0x1E : 0x01441120
    //
    //
    // HDA Codec Subsystem ID Verb-table
    // HDA Codec Subsystem ID  : 0x80860000
    //
    0x00172000,
    0x00172100,
    0x00172286,
    0x00172380,

    //
    // Pin Widget Verb-table
    // Widget node 0x01 :
    //
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,
    0x0017FF00,

    //
    // Pin widget 0x12 - DMIC
    //
    0x01271C00,
    0x01271D00,
    0x01271E13,
    0x01271F40,

    //
    // Pin widget 0x14 - FRONT (Port-D)
    //
    0x01471C10,
    0x01471D40,
    0x01471E01,
    0x01471F01,

    //
    // Pin widget 0x15 - SURR (Port-A)
    //
    0x01571C12,
    0x01571D10,
    0x01571E01,
    0x01571F01,

    //
    // Pin widget 0x16 - CEN/LFE (Port-G)
    //
    0x01671C11,
    0x01671D60,
    0x01671E01,
    0x01671F01,

    //
    // Pin widget 0x18 - MIC1 (Port-B)
    //
    0x01871C30,
    0x01871D90,
    0x01871EA1,
    0x01871F01,

    //
    // Pin widget 0x19 - MIC2 (Port-F)
    //
    0x01971C40,
    0x01971D90,
    0x01971EA1,
    0x01971F02,

    //
    // Pin widget 0x1A - LINE1 (Port-C)
    //
  0x01A71C3F,
    0x01A71D30,
    0x01A71E81,
    0x01A71F01,

    //
    // Pin widget 0x1B - LINE2 (Port-E)
    //
  0x01B71C1F,
    0x01B71D40,
    0x01B71E21,
    0x01B71F02,

    //
    // Pin widget 0x1C - CD-IN
    //
    0x01C71CF0,
    0x01C71D11,
    0x01C71E11,
    0x01C71F41,

    //
    // Pin widget 0x1D - BEEP-IN
    //
  0x01D71C01,
    0x01D71DE6,
    0x01D71E45,
    0x01D71F40,

    //
    // Pin widget 0x1E - S/PDIF-OUT
    //
    0x01E71C20,
    0x01E71D11,
    0x01E71E44,
    0x01E71F01,

    //
    // Widget node 0x20 :
    //
    0x02050004,
    0x02040001,
    0x02050004,
    0x02040001,

    //
    // Widget node 0x20 - 1 :
    //
    0x02050005,
    0x02040080,
    0x02050001,
    0x0204A9B8
  }
};

