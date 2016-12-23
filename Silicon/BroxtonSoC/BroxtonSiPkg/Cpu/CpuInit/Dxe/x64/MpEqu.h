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

#define VacantFlag    0x00
#define NotVacantFlag 0xff
#define BreakToRunApSignal             0x6E755200
#define MonitorFilterSize              0x40
#define WakeUpApCounterInit            0
#define WakeUpApPerHltLoop             1
#define WakeUpApPerMwaitLoop           2
#define WakeUpApPerRunLoop             3
#define WakeUpApPerMwaitLoop32         4
#define WakeUpApPerRunLoop32           5

#define LockLocation                   (0x1000 - 0x0400)
#define StackStartAddressLocation      (LockLocation + 0x08)
#define StackSizeLocation              (LockLocation + 0x10)
#define CProcedureLocation             (LockLocation + 0x18)
#define GdtrLocation                   (LockLocation + 0x20)
#define IdtrLocation                   (LockLocation + 0x2A)
#define BufferStartLocation            (LockLocation + 0x34)
#define Cr3OffsetLocation              (LockLocation + 0x38)
#define InitFlagLocation               (LockLocation + 0x3C)
#define WakeUpApManner                 (LockLocation + 0x40)
#define BistBuffer                     (LockLocation + 0x44)

