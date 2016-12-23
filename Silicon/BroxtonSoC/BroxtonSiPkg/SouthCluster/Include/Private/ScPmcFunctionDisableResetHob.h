/** @file
  This file contains definitions of SC PMC function disable reset hob.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PMC_FUNCTION_DISABLE_RESET_HOB_H_
#define _SC_PMC_FUNCTION_DISABLE_RESET_HOB_H_

#define SC_PMC_FUNCTION_DISABLE_NO_RESET     0
#define SC_PMC_FUNCTION_DISABLE_COLD_RESET   1
#define SC_PMC_FUNCTION_DISABLE_GLOBAL_RESET 2

//
// This structure contains the HOB for PMC function disable.
//
typedef struct {
  /**
    Using to do proper reset for PMC function disable.
    0: No Reset, 1: Cold reset, 2: Global reset
  **/
  UINT8    ResetType;
} SC_PMC_FUNCTION_DISABLE_RESET_HOB;

extern EFI_GUID gScPmcFunctionDisableResetHobGuid;

#endif ///< _SC_PMC_FUNCTION_DISABLE_RESET_HOB_H_

