/** @file
  Header file for locate and install Firmware Volume Hob's Once there is main memory..

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FV_CALLBACK_H__
#define __FV_CALLBACK_H__

//
// Please do not put #includes here, they should be with the implementation code
//
EFI_STATUS
EFIAPI
GetFvNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

VOID
CopyMemSse4 (
  IN VOID* Dst,
  IN VOID* Src,
  IN UINTN SizeInBytes
  );

#endif

