/** @file
  PCH configuration based on PCH policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PREMEM_POLICY_COMMON_H_
#define _SC_PREMEM_POLICY_COMMON_H_

#include <ConfigBlock.h>
#include "ScLimits.h"
#include "ConfigBlock/LpcConfig.h"
#include "ConfigBlock/PcieRpConfig.h"

#pragma pack (push,1)
#define SC_PREMEM_POLICY_REVISION  1

typedef struct _SC_PREMEM_POLICY {
  CONFIG_BLOCK_TABLE_HEADER      TableHeader;
} SC_PREMEM_POLICY;

#pragma pack (pop)

#endif // _PCH_PREMEM_POLICY_COMMON_H_

