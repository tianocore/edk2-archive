/** @file
  HD Audio Verb Table header file.
  This file includes package header files, library classes.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HDA_VERBTABLES_H_
#define _HDA_VERBTABLES_H_

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
#pragma pack (pop)

extern HDAUDIO_VERB_TABLE  HdaVerbTableAlc662;

#endif // _HDA_VERBTABLES_H_
