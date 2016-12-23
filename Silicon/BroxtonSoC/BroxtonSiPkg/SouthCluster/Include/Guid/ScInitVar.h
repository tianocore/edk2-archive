/** @file
  This file defines variable shared between SC Init DXE driver and SC
  Init S3 Resume PEIM.

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_INIT_VAR_H_
#define _SC_INIT_VAR_H_

//
// Define the SC Init Var GUID
//
#define INIT_VARIABLE_GUID {0xe6c2f70a, 0xb604, 0x4877,{0x85, 0xba, 0xde, 0xec, 0x89, 0xe1, 0x17, 0xeb}}

//
// Extern the GUID for PPI users.
//
extern EFI_GUID gScInitVariableGuid;

#define INIT_VARIABLE_NAME  L"PchInit"

//
// Define the SC Init Variable structure
//
typedef struct {
  UINT32  StorePosition;
  UINT32  ExecutePosition;
} SC_S3_PARAMETER_HEADER;

#pragma pack(1)
typedef struct _SC_INIT_VARIABLE {
  SC_S3_PARAMETER_HEADER *ScS3Parameter;
} SC_INIT_VARIABLE;
#pragma pack()

#endif

