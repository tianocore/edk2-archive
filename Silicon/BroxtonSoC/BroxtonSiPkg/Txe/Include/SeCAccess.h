/** @file
  Macros to simplify and abstract the interface to PCI configuration.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_ACCESS_H_
#define _SEC_ACCESS_H_

#include "SeCChipset.h"
#include <ScAccess.h>
#include <SaAccess.h>
#include <Library/PciLib.h>

//
// HECI PCI Access Macro
//
#define HeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define HeciPciRead16(Register) PciRead16 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define HeciPciRead8(Register)  PciRead8 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

//
// HECI2 PCI Access Macro
//
#define Heci2PciRead32(Register)  PciRead32 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define Heci2PciRead16(Register)  PciRead16 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define Heci2PciRead8(Register) PciRead8 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

//
// HECI3 PCI Access Macro
//
#define Heci3PciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (SEC_BUS, SEC_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER, Register))

#define Heci3PciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI3_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci3PciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI3_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci3PciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI3_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci3PciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (SEC_BUS, \
  SEC_DEVICE_NUMBER, \
  HECI3_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#endif

