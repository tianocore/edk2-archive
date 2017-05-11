/** @file
  File to contain all the hardware specific stuff for the Smm Gpi dispatch protocol.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"

//
//
// Structure for GPI SMI is a template which needs to have
// GPI Smi bit offset and Smi Status & Enable registers updated (accordingly
// to choosen group and pad number) after adding it to SMM Callback database
//
CONST SC_SMM_SOURCE_DESC  SC_GPI_SOURCE_DESC_TEMPLATE = {
  SC_SMM_NO_FLAGS,
  {
    {
      {
        GPIO_ADDR_TYPE, {0x0}
      },
      S_GPIO_GP_SMI_EN, 0x0,
    },
    NULL_BIT_DESC_INITIALIZER
  },

  {
    {
      {
        GPIO_ADDR_TYPE, {0x0}
      },
      S_GPIO_GP_SMI_STS, 0x0,
    },
  }
};

