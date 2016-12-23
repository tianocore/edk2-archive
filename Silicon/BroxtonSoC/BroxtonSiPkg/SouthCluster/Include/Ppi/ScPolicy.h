/** @file
  SC policy PPI produced by a platform driver specifying various
  expected SC settings. This PPI is consumed by the SC PEI modules
  and carried over to SC DXE modules.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_POLICY_PPI_H_
#define _SC_POLICY_PPI_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//

#include <ScAccess.h>
#include <ScPolicyCommon.h>

extern EFI_GUID gScPolicyPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct SC_POLICY SC_POLICY_PPI;

#endif

