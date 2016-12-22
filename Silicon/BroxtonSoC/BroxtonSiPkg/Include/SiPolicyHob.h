/** @file
  Silicon Policy HOB is used for gathering platform
  related Intel silicon information and policy setting.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_HOB_H_
#define _SI_POLICY_HOB_H_

#include "SiPolicy.h"

extern EFI_GUID gSiPolicyHobGuid;

typedef CONST struct _SI_POLICY SI_POLICY_HOB;

#endif // _SI_POLICY_HOB_H_

