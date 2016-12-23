/** @file
  Platform specific information.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "ChipsetAccess.h"
#include "PlatformBaseAddresses.h"
#include "CMOSMap.h"

#define PLATFORM_MAX_BUS_NUM 0x3f
#define  SOC_DEVICE_ENABLE       1
#define  SOC_DEVICE_DISABLE      0

//
// Number of P & T states supported
//
#define NPTM_P_STATES_SUPPORTED         16
#define NPTM_T_STATES_SUPPORTED         8

//
// I/O APIC IDs, the code uses math to generate the numbers
// instead of using these defines.
//
#define ICH_IOAPIC                     (1 << 0)
#define ICH_IOAPIC_ID                   0x01

//
// Possible SMBus addresses that will be present
//
#define SMBUS_ADDR_CH_A_1     0xA0
#define SMBUS_ADDR_CH_A_2     0xA2
#define SMBUS_ADDR_CH_B_1     0xA4
#define SMBUS_ADDR_CH_B_2     0xA6
#define SMBUS_ADDR_CH_C_1     0xA8
#define SMBUS_ADDR_CH_C_2     0xAA
#define SMBUS_ADDR_CH_D_1     0xAC
#define SMBUS_ADDR_CH_D_2     0xAE
#define SMBUS_ADDR_HOST_CLK_BUFFER  0xDC
#define SMBUS_ADDR_ICH_SLAVE        0x44
#define SMBUS_ADDR_HECETA     0x5C
#define SMBUS_ADDR_SMBARP     0xC2
#define SMBUS_ADDR_82573E     0xC6
#define SMBUS_ADDR_CLKCHIP    0xD2
#define SMBUS_ADDR_BRD_REV          0x4E
#define SMBUS_ADDR_DB803            0x82

//
// SMBus addresses that used on this platform
//
#define PLATFORM_SMBUS_RSVD_ADDRESSES { \
  SMBUS_ADDR_CH_A_1, \
  SMBUS_ADDR_CH_A_2, \
  SMBUS_ADDR_HOST_CLK_BUFFER, \
  SMBUS_ADDR_ICH_SLAVE, \
  SMBUS_ADDR_SMBARP, \
  SMBUS_ADDR_CLKCHIP, \
  SMBUS_ADDR_BRD_REV, \
  SMBUS_ADDR_DB803 \
  }
//
// Count of addresses present in PLATFORM_SMBUS_RSVD_ADDRESSES
//
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES 8

//
// GPIO Index Data Structure
//
typedef struct {
  UINT8   Register;
  UINT32  Value;
} ICH_GPIO_DEV;

//
// CPU Equates
//
#define MAX_THREAD                      2
#define MAX_CORE                        1
#define MAX_DIE                         2
#define MAX_CPU_SOCKET                  1
#define MAX_CPU_NUM                     (MAX_THREAD * MAX_CORE * MAX_DIE * MAX_CPU_SOCKET)

#define MEM64_LEN                       0x00100000000
#define RES_MEM64_36_BASE               0x01000000000 - MEM64_LEN   // 2^36
#define RES_MEM64_36_LIMIT              0x01000000000 - 1           // 2^36
#define RES_MEM64_39_BASE               0x08000000000 - MEM64_LEN   // 2^39
#define RES_MEM64_39_LIMIT              0x08000000000 - 1           // 2^39
#define RES_MEM64_40_BASE               0x10000000000 - MEM64_LEN   // 2^40
#define RES_MEM64_40_LIMIT              0x10000000000 - 1           // 2^40

//
//LT Equates
//
#ifdef LT_FLAG
    #define ACM_BASE                      AUTHENTICATED_CODE_BASE_ADDR
    #define ACM_SIZE                      UTHENTICATED_CODE_SIZE
#endif

#endif

