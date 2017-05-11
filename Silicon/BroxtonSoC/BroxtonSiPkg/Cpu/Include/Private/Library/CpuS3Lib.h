/** @file
  Header file for Cpu Init Lib Pei Phase.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_S3_LIB_H_
#define _CPU_S3_LIB_H_

/**
  Notification function that gets called during S3 resume to take care
  of CPU related activities in PEI phase

  @param[in]  PeiServices     Pointer to PEI Services Table

  @retval     EFI_SUCCESS     Multiple processors are intialized successfully

**/
EFI_STATUS
S3InitializeCpu (
  IN  EFI_PEI_SERVICES    **PeiServices
  );
#endif

