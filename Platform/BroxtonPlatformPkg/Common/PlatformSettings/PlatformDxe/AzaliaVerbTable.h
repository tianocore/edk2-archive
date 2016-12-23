/** @file
  Header file for Azalia Verb Table.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

UINT32 mHdaVerbTableData[] = {
  //
  // Rear Audio Verb Table - 10EC0880/0000/03
  //
  //
  // Pin Complex 1       (NID 1Eh)
  //
  0x01E71F01,
  0x01E71E44,
  0x01E71D21,
  0x01E71C00,

  //
  // Pin Complex 2       (NID 1Fh)
  //
  0x01F71F01,
  0x01F71EC4,
  0x01F71D21,
  0x01F71C10,

  //
  // Pin Complex 3       (NID 14h)
  //
  0x01471F01,
  0x01471E01,
  0x01471D40,
  0x01471C20,

  //
  // Pin Complex 4       (NID 15h)
  //
  0x01571F01,
  0x01571E01,
  0x01571D40,
  0x01571C21,

  //
  // Pin Complex 5       (NID 16h)
  //
  0x01671F01,
  0x01671E01,
  0x01671D90,
  0x01671C22,

  //
  // Pin Complex 6       (NID 17h)
  //
  0x01771F01,
  0x01771E01,
  0x01771D30,
  0x01771C23,

  //
  // Pin Complex 7       (NID 1Ah)
  //
  0x01A71F01,
  0x01A71E81,
  0x01A71D30,
  0x01A71C30,

  //
  // Pin Complex 8       (NID 18h)
  //
  0x01871F01,
  0x01871EA1,
  0x01871D90,
  0x01871C31,

  //
  // Pin Complex 11       (NID 1Ch)
  //
  0x01C71F99,
  0x01C71E33,
  0x01C71DF1,
  0x01C71C70,

  //
  // Pin Complex 12       (NID 1Dh)
  //
  0x01D71F99,
  0x01D71EF3,
  0x01D71DF1,
  0x01D71C80,

  //
  // front panel
  //

  //
  // Pin Complex 9       (NID 1Bh)
  //
  0x01B71F02,
  0x01B71E21,
  0x01B71D40,
  0x01B71C50,

  //
  // Pin Complex 10       (NID 19h)
  //
  0x01971F02,
  0x01971EA1,
  0x01971D90,
  0x01971C60
 };

SC_HDAUDIO_VERB_TABLE mHdaVerbTable[] = {
  {
    //
    // VerbTable:
    //  Revision ID = 0xFF, support all steps
    //  Codec Verb Table For AZALIA
    //  Codec Address: CAd value (0/1/2)
    //  Codec Vendor:  0x10EC0880
    //
    {
      0x10EC0880,     // Vendor ID/Device ID
      0x0000,         // SubSystem ID
      0xFF,           // Revision ID
      0x01,           // Front panel support (1=yes, 2=no)
      0x000A,         // Number of Rear Jacks = 10
      0x0002          // Number of Front Jacks = 2
    },
    0                 // Pointer to verb table data, need to be inited in the code.
  }
};

