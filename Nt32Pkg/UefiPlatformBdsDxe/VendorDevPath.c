/** @file

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  VendorDevPath.c

Abstract:

  This file provide the implementation of call back function to dump Vendro Device Path.
  

**/


#include "BdsPlatform.h"

#include <Protocol/DebugPort.h>
#include <Protocol/WinNtThunk.h>
#include <Protocol/WinNtIo.h>


EFI_GUID  mEfiWinNtThunkProtocolGuid  = EFI_WIN_NT_THUNK_PROTOCOL_GUID;
EFI_GUID  mEfiWinNtUgaGuid            = EFI_WIN_NT_UGA_GUID;
EFI_GUID  mEfiWinNtGopGuid            = EFI_WIN_NT_GOP_GUID;
EFI_GUID  mEfiWinNtSerialPortGuid     = EFI_WIN_NT_SERIAL_PORT_GUID;

VOID
DevPathVendor (
  IN OUT POOL_PRINT       *Str,
  IN VOID                 *DevPath
  )
/*++

Routine Description:

  Convert Vendor device path to device name

Arguments:

  Str     - The buffer store device name
  DevPath - Pointer to vendor device path

Returns:

  When it return, the device name have been stored in *Str.

--*/
{
  VENDOR_DEVICE_PATH  *Vendor;
  CHAR16              *Type;
  UINTN               DataLength;
  UINTN               Index;
  UINT32              FlowControlMap;
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  UINT16              Info;
#endif

  Vendor  = DevPath;

  switch (DevicePathType (&Vendor->Header)) {
  case HARDWARE_DEVICE_PATH:
    Type = L"Hw";
    //
    // If the device is a winntbus device, we will give it a readable device name.
    //
    if (CompareGuid (&Vendor->Guid, &mEfiWinNtThunkProtocolGuid)) {
      CatPrint (Str, L"%s", L"WinNtBus");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &mEfiWinNtGopGuid)) {
      CatPrint (Str, L"%s", L"GOP");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &mEfiWinNtSerialPortGuid)) {
      CatPrint (Str, L"%s", L"Serial");
      return ;
    }
    break;

  case MESSAGING_DEVICE_PATH:
    Type = L"Msg";
    if (CompareGuid (&Vendor->Guid, &gEfiPcAnsiGuid)) {
      CatPrint (Str, L"VenPcAnsi()");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVT100Guid)) {
      CatPrint (Str, L"VenVt100()");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVT100PlusGuid)) {
      CatPrint (Str, L"VenVt100Plus()");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVTUTF8Guid)) {
      CatPrint (Str, L"VenUft8()");
      return ;
    } else if (CompareGuid (&Vendor->Guid, &mEfiDevicePathMessagingUartFlowControlGuid)) {
      FlowControlMap = (((UART_FLOW_CONTROL_DEVICE_PATH *) Vendor)->FlowControlMap);
      switch (FlowControlMap & 0x00000003) {
      case 0:
        CatPrint (Str, L"UartFlowCtrl(%s)", L"None");
        break;

      case 1:
        CatPrint (Str, L"UartFlowCtrl(%s)", L"Hardware");
        break;

      case 2:
        CatPrint (Str, L"UartFlowCtrl(%s)", L"XonXoff");
        break;

      default:
        break;
      }

      return ;
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
    } else if (CompareGuid (&Vendor->Guid, &mEfiDevicePathMessagingSASGuid)) {
      CatPrint (
        Str,
        L"SAS(%lx,%lx,%x,",
        ((SAS_DEVICE_PATH *) Vendor)->SasAddress,
        ((SAS_DEVICE_PATH *) Vendor)->Lun,
        ((SAS_DEVICE_PATH *) Vendor)->RelativeTargetPort
        );
      Info = (((SAS_DEVICE_PATH *) Vendor)->DeviceTopology);
      if ((Info & 0x0f) == 0) {
        CatPrint (Str, L"NoTopology,0,0,0,");
      } else if (((Info & 0x0f) == 1) || ((Info & 0x0f) == 2)) {
        CatPrint (
          Str,
          L"%s,%s,%s,",
          (Info & (0x1 << 4)) ? L"SATA" : L"SAS",
          (Info & (0x1 << 5)) ? L"External" : L"Internal",
          (Info & (0x1 << 6)) ? L"Expanded" : L"Direct"
          );
        if ((Info & 0x0f) == 1) {
          CatPrint (Str, L"0,");
        } else {
          CatPrint (Str, L"%x,", (UINTN) ((Info >> 8) & 0xff));
        }
      } else {
        CatPrint (Str, L"0,0,0,0,");
      }

      CatPrint (Str, L"%x)", (UINTN) ((SAS_DEVICE_PATH *) Vendor)->Reserved);
      return ;
#endif
    } else if (CompareGuid (&Vendor->Guid, &gEfiDebugPortProtocolGuid)) {
      CatPrint (Str, L"DebugPort()");
      return ;
    }
    break;

  case MEDIA_DEVICE_PATH:
    Type = L"Media";
    break;

  default:
    Type = L"?";
    break;
  }

  CatPrint (Str, L"Ven%s(%g", Type, &Vendor->Guid);
  DataLength = DevicePathNodeLength (&Vendor->Header) - sizeof (VENDOR_DEVICE_PATH);
  if (DataLength > 0) {
    CatPrint (Str, L",");
    for (Index = 0; Index < DataLength; Index++) {
      CatPrint (Str, L"%02x", (UINTN) ((VENDOR_DEVICE_PATH_WITH_DATA *) Vendor)->VendorDefinedData[Index]);
    }
  }
  CatPrint (Str, L")");
}
