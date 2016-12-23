/** @file
  PCH preserved MMIO resource definitions.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PRESERVED_RESOURCES_H_
#define _SC_PRESERVED_RESOURCES_H_
#define SC_PRESERVED_BASE_ADDRESS      0xD0000000     ///< SC preserved MMIO base address
#define SC_PRESERVED_MMIO_SIZE         0x01000000     ///< 16MB
#define SC_PCR_BASE_ADDRESS            0xD0000000     ///< SBREG MMIO base address
#define SC_PCR_MMIO_SIZE               0x01000000     ///< 16MB
#define SC_SPI_BASE_ADDRESS            0xFED01000     ///< SPI BAR0 MMIO base address
#define SC_SPI_MMIO_SIZE               0x00001000     ///< 4KB
#define SC_SERIAL_IO_BASE_ADDRESS      0xFE020000     ///< SerialIo MMIO base address
#define SC_SERIAL_IO_MMIO_SIZE         0x00016000     ///< 88KB
#define SC_TRACE_HUB_SW_BASE_ADDRESS   0xFE200000     ///< TraceHub SW MMIO base address
#define SC_TRACE_HUB_SW_MMIO_SIZE      0x00040000     ///< 2MB

#endif // _SC_PRESERVED_RESOURCES_H_

