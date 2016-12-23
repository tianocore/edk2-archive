/** @file
  Header file for SmbiosProcessorLib implementation.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMBIOS_PROCESSOR_LIB_H_
#define _SMBIOS_PROCESSOR_LIB_H_

/**
  Add Smbios Cache information (type 7) table and Processor information (type 4) table
  using the HOB info from Silicon.

  @retval  EFI_SUCCESS           Successfully installed SMBIOS cache information and processor information.

**/
EFI_STATUS
EFIAPI
AddSmbiosProcessorAndCacheTables (
  VOID
  );

#endif // _SMBIOS_PROCESSOR_LIB_H_

