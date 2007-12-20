/** @file
  Public include file for the HII Library

  Copyright (c) 2006, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __HII_LIB_H__
#define __HII_LIB_H__

EFI_STATUS
EFIAPI
HiiLibCreateNewPackages (
  IN       UINTN      NumberOfPackages,
  IN CONST EFI_GUID   *GuidId,
  OUT      VOID      **HiiHandle,         //Framework is FRAMEWORK_HII_HANDLE; UEFI is EFI_HII_HANDLE; 
                                     // C:\D\Work\Tiano\Tiano_Main_Trunk\TIANO\Platform\IntelEpg\SR870BN4\MemorySubClassDriver\DualChannelDdr\MemorySubClass.c make use of this output value
  ...
  );



#endif
