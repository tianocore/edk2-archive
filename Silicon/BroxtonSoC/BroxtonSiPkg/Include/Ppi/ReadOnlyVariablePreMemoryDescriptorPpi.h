/** @file
  This file declares the Read Only Variable Pre-Memory Descriptor PPI
  This PPI stores the descriptor address for the readonly variable PPI.
  When the PPI is shadowed into permanent memory a unique instance of
  READ_ONLY_VARIABLE2_PPI can be discovered using this PPI.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_READ_ONLY_VARIABLE_PRE_MEMORY_DESC_PPI_H_
#define _PEI_READ_ONLY_VARIABLE_PRE_MEMORY_DESC_PPI_H_

extern EFI_GUID gReadOnlyVariablePreMemoryDescriptorPpiGuid;

typedef struct {
  EFI_PEI_PPI_DESCRIPTOR    *PreMemoryDescriptor;
} READ_ONLY_VARIABLE_PRE_MEMORY_DESCRIPTOR_PPI;

#endif

