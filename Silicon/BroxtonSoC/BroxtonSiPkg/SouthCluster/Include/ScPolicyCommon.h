/** @file
  SC configuration based on SC policy.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_POLICY_COMMON_H_
#define _SC_POLICY_COMMON_H_

#include <ConfigBlock.h>
#include <ScLimits.h>
#include "ConfigBlock/ScGeneralConfig.h"
#include "ConfigBlock/SataConfig.h"
#include "ConfigBlock/PcieRpConfig.h"
#include "ConfigBlock/HpetConfig.h"
#include "ConfigBlock/SmbusConfig.h"
#include "ConfigBlock/IoApicConfig.h"
#include "ConfigBlock/HdAudioConfig.h"
#include "ConfigBlock/GmmConfig.h"
#include "ConfigBlock/PmConfig.h"
#include "ConfigBlock/LockDownConfig.h"
#include "ConfigBlock/SerialIrqConfig.h"
#include "ConfigBlock/ScsConfig.h"
#include "ConfigBlock/LpssConfig.h"
#include "ConfigBlock/VtdConfig.h"
#include "ConfigBlock/IshConfig.h"
#include "ConfigBlock/UsbConfig.h"
#include "ConfigBlock/FlashProtectionConfig.h"
#include "ConfigBlock/DciConfig.h"
#include "ConfigBlock/P2sbConfig.h"
#include "ConfigBlock/ScInterruptAssign.h"

#pragma pack (push,1)

#ifndef FORCE_ENABLE
#define FORCE_ENABLE  1
#endif
#ifndef FORCE_DISABLE
#define FORCE_DISABLE 2
#endif
#ifndef PLATFORM_POR
#define PLATFORM_POR  0
#endif

//
// Generic definitions for device enabling/disabling used by SC code.
//
#define DEVICE_ENABLE   1
#define DEVICE_DISABLE  0

#define SC_POLICY_REVISION  2

struct SC_POLICY {
  CONFIG_BLOCK_TABLE_HEADER      TableHeader;
};

#pragma pack (pop)

#endif // _SC_POLICY_COMMON_H_

