/** @file
  Macros to simplify and abstract the interface to PCI configuration.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SAACCESS_H_
#define _SAACCESS_H_

#include "SaRegs.h"
#include "SaCommonDefinitions.h"
#include <Library/IoLib.h>
#include "PlatformBaseAddresses.h"

//
// Memory Mapped IO access macros used by MSG BUS LIBRARY
//
#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
  )

//
// UINT64
//

#define Mmio64Ptr( BaseAddr, Register ) \
  ( (volatile UINT64 *)MmioAddress( BaseAddr, Register ) )

#define Mmio64( BaseAddr, Register ) \
  *Mmio64Ptr( BaseAddr, Register )

#define Mmio64Or( BaseAddr, Register, OrData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      Mmio64( BaseAddr, Register ) | \
      (UINT64)(OrData) \
    )

#define Mmio64And( BaseAddr, Register, AndData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      Mmio64( BaseAddr, Register ) & \
      (UINT64)(AndData) \
    )

#define Mmio64AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio64( BaseAddr, Register ) = \
    (UINT64) ( \
      ( Mmio64( BaseAddr, Register ) & \
          (UINT64)(AndData) \
      ) | \
      (UINT64)(OrData) \
    )

//
// UINT32
//

#define Mmio32Ptr( BaseAddr, Register ) \
  ( (volatile UINT32 *)MmioAddress( BaseAddr, Register ) )

#define Mmio32( BaseAddr, Register ) \
  *Mmio32Ptr( BaseAddr, Register )

#define Mmio32Or( BaseAddr, Register, OrData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      Mmio32( BaseAddr, Register ) | \
      (UINT32)(OrData) \
    )

#define Mmio32And( BaseAddr, Register, AndData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      Mmio32( BaseAddr, Register ) & \
      (UINT32)(AndData) \
    )

#define Mmio32AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio32( BaseAddr, Register ) = \
    (UINT32) ( \
      ( Mmio32( BaseAddr, Register ) & \
          (UINT32)(AndData) \
      ) | \
      (UINT32)(OrData) \
    )

//
// UINT16
//

#define Mmio16Ptr( BaseAddr, Register ) \
  ( (volatile UINT16 *)MmioAddress( BaseAddr, Register ) )

#define Mmio16( BaseAddr, Register ) \
  *Mmio16Ptr( BaseAddr, Register )

#define Mmio16Or( BaseAddr, Register, OrData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      Mmio16( BaseAddr, Register ) | \
      (UINT16)(OrData) \
    )

#define Mmio16And( BaseAddr, Register, AndData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      Mmio16( BaseAddr, Register ) & \
      (UINT16)(AndData) \
    )

#define Mmio16AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio16( BaseAddr, Register ) = \
    (UINT16) ( \
      ( Mmio16( BaseAddr, Register ) & \
          (UINT16)(AndData) \
      ) | \
      (UINT16)(OrData) \
    )

//
// UINT8
//

#define Mmio8Ptr( BaseAddr, Register ) \
  ( (volatile UINT8 *)MmioAddress( BaseAddr, Register ) )

#define Mmio8( BaseAddr, Register ) \
  *Mmio8Ptr( BaseAddr, Register )

#define Mmio8Or( BaseAddr, Register, OrData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      Mmio8( BaseAddr, Register ) | \
      (UINT8)(OrData) \
    )

#define Mmio8And( BaseAddr, Register, AndData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      Mmio8( BaseAddr, Register ) & \
      (UINT8)(AndData) \
    )

#define Mmio8AndThenOr( BaseAddr, Register, AndData, OrData ) \
  Mmio8( BaseAddr, Register ) = \
    (UINT8) ( \
      ( Mmio8( BaseAddr, Register ) & \
          (UINT8)(AndData) \
        ) | \
      (UINT8)(OrData) \
    )

/**
  All sideband access has moved to SideBandLib in BxtSocRefCodePkg/BroxtonSoc/Library/SideBandLib.

  Any sideband read and write operations should be performed by calling SideBandRead32() and
  SideBandWrite32() which will internally determine whether to access the sideband private
  configuration registers via MMIO or Sideband Message Interface based on whether the P2SB
  BAR (SBREG_BAR) is set.

**/

#define N_PCICFGCTRL_PCI_IRQ    20
#define N_PCICFGCTRL_ACPI_IRQ   12
#define N_PCICFGCTRL_INT_PIN    8
#define V_PCICFG_CTRL_NONE          0
#define V_PCICFG_CTRL_INTA          1
#define V_PCICFG_CTRL_INTB          2
#define V_PCICFG_CTRL_INTC          3
#define V_PCICFG_CTRL_INTD          4

//
// Memory mapped PCI IO
//
#define PciCfgPtr(Bus, Device, Function, Register )\
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register)

#define PciCfg32Read_CF8CFC(B,D,F,R) \
  (UINT32)(IoOut32(0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoIn32(0xCFC))

#define PciCfg32Write_CF8CFC(B,D,F,R,Data) \
  (IoOut32(0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoOut32(0xCFC,Data))

#define PciCfg32Or_CF8CFC(B,D,F,R,O) \
  PciCfg32Write_CF8CFC(B,D,F,R, \
    (PciCfg32Read_CF8CFC(B,D,F,R) | (O)))

#define PciCfg32And_CF8CFC(B,D,F,R,A) \
  PciCfg32Write_CF8CFC(B,D,F,R, \
    (PciCfg32Read_CF8CFC(B,D,F,R) & (A)))

#define PciCfg32AndThenOr_CF8CFC(B,D,F,R,A,O) \
  PciCfg32Write_CF8CFC(B,D,F,R, \
    (PciCfg32Read_CF8CFC(B,D,F,R) & (A)) | (O))

//
// Device 0, Function 0
//
#define McD0PciCfg64(Register)                              MmPci64           (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg64Or(Register, OrData)                    MmPci64Or         (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg64And(Register, AndData)                  MmPci64And        (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg64AndThenOr(Register, AndData, OrData)    MmPci64AndThenOr  (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg32(Register)                              MmPci32           (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg32Or(Register, OrData)                    MmPci32Or         (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg32And(Register, AndData)                  MmPci32And        (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg32AndThenOr(Register, AndData, OrData)    MmPci32AndThenOr  (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg16(Register)                              MmPci16           (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg16Or(Register, OrData)                    MmPci16Or         (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg16And(Register, AndData)                  MmPci16And        (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg16AndThenOr(Register, AndData, OrData)    MmPci16AndThenOr  (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg8(Register)                               MmPci8            (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg8Or(Register, OrData)                     MmPci8Or          (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg8And(Register, AndData)                   MmPci8And         (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg8AndThenOr( Register, AndData, OrData )   MmPci8AndThenOr   (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)


//
// Device 2, Function 0
//
#define McD2PciCfg64(Register)                              MmPci64           (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg64Or(Register, OrData)                    MmPci64Or         (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg64And(Register, AndData)                  MmPci64And        (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg64AndThenOr(Register, AndData, OrData)    MmPci64AndThenOr  (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg32(Register)                              MmPci32           (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg32Or(Register, OrData)                    MmPci32Or         (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg32And(Register, AndData)                  MmPci32And        (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg32AndThenOr(Register, AndData, OrData)    MmPci32AndThenOr  (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg16(Register)                              MmPci16           (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg16Or(Register, OrData)                    MmPci16Or         (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg16And(Register, AndData)                  MmPci16And        (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg16AndThenOr(Register, AndData, OrData)    MmPci16AndThenOr  (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg8(Register)                               MmPci8            (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg8Or(Register, OrData)                     MmPci8Or          (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg8And(Register, AndData)                   MmPci8And         (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg8AndThenOr(Register, AndData, OrData)     MmPci8AndThenOr   (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

//
// IO
//
#ifndef IoIn8
#define IoIn8(Port) \
  IoRead8(Port)

#define IoIn16(Port) \
  IoRead16(Port)

#define IoIn32(Port) \
  IoRead32(Port)

#define IoOut8(Port, Data) \
  IoWrite8(Port, Data)

#define IoOut16(Port, Data) \
  IoWrite16(Port, Data)

#define IoOut32(Port, Data) \
  IoWrite32(Port, Data)

#endif

#endif

