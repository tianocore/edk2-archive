/** @file
  This is the equates file for HT (Hyper-threading) support.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#define VacantFlag             0x00
#define NotVacantFlag          0xff

#define LockLocation                   (0x1000 - 0x0200)
#define StackStartAddressLocation      (LockLocation + 0x04)
#define StackSizeLocation              (LockLocation + 0x08)
#define CProcedureLocation             (LockLocation + 0x0C)
#define GdtrLocation                   (LockLocation + 0x10)
#define IdtrLocation                   (LockLocation + 0x16)
#define BufferStartLocation            (LockLocation + 0x1C)
#define PmodeOffsetLocation            (LockLocation + 0x20)
#define AcpiCpuDataAddressLocation     (LockLocation + 0x24)
#define MtrrValuesAddressLocation      (LockLocation + 0x28)
#define FinishedCountAddressLocation   (LockLocation + 0x2C)
#define WakeupCountAddressLocation     (LockLocation + 0x30)
#define SerializeLockAddressLocation   (LockLocation + 0x34)
#define MicrocodeAddressLocation       (LockLocation + 0x38)
#define BootScriptAddressLocation      (LockLocation + 0x3C)
#define StartStateLocation             (LockLocation + 0x40)
#define VirtualWireMode                (LockLocation + 0x44)
#define SemaphoreCheck                 (LockLocation + 0x48)
#define PeiServices                    (LockLocation + 0x4C)
#define PeiStall                       (LockLocation + 0x50)
#define CpuPerfCtrlValue               (LockLocation + 0x54)
#define SiCpuPolicyPpi                 (LockLocation + 0x5C)
#define MpSystemDataAddressLocation    (LockLocation + 0x64)
#define MpServicePpiAddressLocation    (LockLocation + 0x68)
#define CArgumentLocation              (LockLocation + 0x6C)
#define BistBufferLocation             (LockLocation + 0x70)

