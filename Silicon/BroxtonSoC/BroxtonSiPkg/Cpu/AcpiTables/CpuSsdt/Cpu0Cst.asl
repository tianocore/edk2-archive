/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "CPU0CST.aml",
  "SSDT",
  2,
  "PmRef",
  "Cpu0Cst",
  0x3001
  )
{
  External(\_PR.CPU0, DeviceObj)
  External(PWRS)
  External(\_PR.CFGD, IntObj)
  External(PDC0)
  External(FMBL)
  External(FEMD)
  External(PFLV)
  External(C3MW)       // Mwait Hint value for C3
  External(C6MW)       // Mwait Hint value for C6
  External(C7MW)       // Mwait Hint value for C7
  External(CDMW)       // Mwait Hint value for C8/C9/C10
  External(C3LT)       // Latency value for C3
  External(C6LT)       // Latency Value for C6
  External(C7LT)       // Latency Value for C7
  External(CDLT)       // Latency Value for C8/C9/C10
  External(CDLV)       // IO Level value for C8/C9/C10
  External(CDPW)       // Power value for  C8/C9/C10
  External(MWEN)

  External(BDID)
  External(BTDT)
  External(BTMB)

  Scope(\_PR.CPU0)
  {
    Method (_CST, 0)
    {
      If (LEqual(MWEN, 0)) {
        Return( Package()
        {
          3,
          Package()
          { // C1, LVT
            ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
            1,
            1,
            1000
          },
          Package()
          {
            // C6L, LVT
            ResourceTemplate(){Register(SystemIO, 8, 0, 0x415)},
            2,
            50,
            10
          },
          Package()
          {
            // C10, LVT
            ResourceTemplate(){Register(SystemIO, 8, 0, 0x419)},
            3,
            150,   //Pre-silicon setting, 11 ms for worst-case exit latency
            10
          }
        })
      }
        Return( Package()
        {
          3,
          Package()
          { // C1, MWAIT
            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x01, 1)},
            1,
            1,
            1000
          },
          Package()
          {
            // C6L, MWAIT Extension
            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x21, 1)},
            2,
            50,
            10
          },
          Package()
          {
            // C10, MWAIT Extension
            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x60, 1)},
            3,
            150,   //Pre-silicon setting, 11 ms for worst-case exit latency
            10
          }
        })
      }
    }
}

