/** @file
  IoApic policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _INTERRUPT_CONFIG_H_
#define _INTERRUPT_CONFIG_H_

#define INTERRUPT_CONFIG_REVISION 1

extern EFI_GUID gInterruptConfigGuid;

#pragma pack (push,1)

//
// --------------------- Interrupts Config ------------------------------
//
typedef struct {
  UINT8   Port;
  UINT16  PciCfgOffset;
  UINT8   PciIrqNumber;
  UINT8   IrqPin;
} PRIVATE_PCICFGCTRL;

typedef enum {
  ScNoInt,        ///< No Interrupt Pin
  ScIntA,
  ScIntB,
  ScIntC,
  ScIntD
} SC_INT_PIN;

//
// The SC_DEVICE_INTERRUPT_CONFIG block describes interrupt pin, IRQ and interrupt mode for SC device.
//
typedef struct {
  UINT8        Device;                  ///< Device number
  UINT8        Function;                ///< Device function
  UINT8        IntX;                    ///< Interrupt pin: INTA-INTD (see SC_INT_PIN)
  UINT8        Irq;                     ///< IRQ to be set for device.
} SC_DEVICE_INTERRUPT_CONFIG;

#define SC_MAX_DEVICE_INTERRUPT_CONFIG  64       ///< Number of all SC devices
#define SC_MAX_PXRC_CONFIG              8        ///< Number of PXRC registers in ITSS
#define SC_MAX_DIRECT_IRQ_CONFIG        25       ///< Number of direct Irq Table

//
// The SC_INTERRUPT_CONFIG block describes interrupt settings for SC.
//
typedef struct {
  CONFIG_BLOCK_HEADER          Header;                                          ///< Config Block Header
  UINT8                        NumOfDevIntConfig;                               ///< Number of entries in DevIntConfig table
  UINT8                        NumOfDirectIrqTable;
  PRIVATE_PCICFGCTRL           DirectIrqTable[SC_MAX_DIRECT_IRQ_CONFIG];
  SC_DEVICE_INTERRUPT_CONFIG   DevIntConfig[SC_MAX_DEVICE_INTERRUPT_CONFIG];   ///< Array which stores SC devices interrupts settings
  UINT8                        PxRcRouting[SC_MAX_PXRC_CONFIG];
} SC_INTERRUPT_CONFIG;

#pragma pack (pop)

#endif // _INTERRUPT_CONFIG_H_

