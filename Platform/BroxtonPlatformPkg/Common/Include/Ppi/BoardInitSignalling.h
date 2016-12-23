/** @file
  The definition of Board detected.

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_BOARD_SIGNALLING_H_
#define _PEI_BOARD_SIGNALLING_H_

#include <Guid/PlatformInfo_Aplk.h>

//
// PEI_BOARD_PRE_MEM_INIT_PPI is produced by board code and consumed by platform code.
//
// PEI_BOARD_PRE_MEM_INIT_PPI conducts pre memory board initializations, which typically
// sets a set of dynamic PCDs to be consumed by platform codes.
//
// Each board produces one instance of PEI_BOARD_PRE_MEM_INIT_PPI; Platform code
// enumerates each instance and calls the PPI functions one by one.
//
typedef struct _PEI_BOARD_PRE_MEM_INIT_PPI PEI_BOARD_PRE_MEM_INIT_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_BOARD_PRE_MEM_INIT) (
  IN  CONST EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BOARD_PRE_MEM_INIT_PPI               * This
  );

typedef struct _PEI_BOARD_PRE_MEM_INIT_PPI {
  PEI_BOARD_PRE_MEM_INIT  PreMemInit;
} PEI_BOARD_PRE_MEM_INIT_PPI;

extern EFI_GUID gBoardPreMemInitPpiGuid;

//
// gBoardPreMemInitDoneGuid signals the completion of one board's pre memory init.
// gBoardPreMemInitDoneGuid is installed by board code in PreMemInit()
//
extern EFI_GUID gBoardPreMemInitDoneGuid;

//
// gBoardPostMemInitStartGuid signals the start of board post mem init process.
//
// gBoardPostMemInitStartGuid is installed by platform code.
//
// Board codes listen to the installation of this PPI through PPI notification mechanism,
// and upon the PPI installation, starts board post memory initialzation, which typically
// sets a set of dynamic PCDs to be consumed by platform codes.
//
extern EFI_GUID gBoardPostMemInitStartGuid;

//
// gBoardPostMemInitDoneGuid signals the completion of one board's pre memory init.
// gBoardPostMemInitDoneGuid is installed by board code in its post memory initialization.
//
extern EFI_GUID gBoardPostMemInitDoneGuid;

//
// BOARD_POST_MEM_INIT_FUNC is a function pointer to be set as a dynamic PCD by board codes;
// The PCD is to be consumed by platform code so platform code will call the function through
// the set pointer
//
typedef
EFI_STATUS
(EFIAPI *BOARD_POST_MEM_INIT_FUNC) (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB  *PlatformInfoHob
  );

#endif

