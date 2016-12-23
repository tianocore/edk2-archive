/** @file
  Header file for SMM Control Driver.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_PEI_SMM_CONTROL_DRIVER_H_
#define _EFI_PEI_SMM_CONTROL_DRIVER_H_

///
/// Driver private data
///
#include <PiPei.h>
#include "Ppi/SmmControl.h"
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <ScAccess.h>

///
/// Prototypes
///
/**
  This is the constructor for the SMM Control ppi

  @param[in] FfsHeader            FfsHeader.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval    EFI_STATUS           Results of the installation of the SMM Control Ppi

**/
EFI_STATUS
EFIAPI
SmmControlPeiDriverEntryInit (
  IN    EFI_PEI_FILE_HANDLE       FfsHeader,
  IN    CONST EFI_PEI_SERVICES    **PeiServices
  );

/**
  This routine generates an SMI.

  @param[in]      PeiServices             General purpose services available to every PEIM.
  @param[in]      This                    The EFI SMM Control ppi instance.
  @param[in, out] ArgumentBuffer          The buffer of argument.
  @param[in, out] ArgumentBufferSize      The size of the argument buffer.
  @param[in]      Periodic                Periodic or not
  @param[in]      ActivationInterval      Interval of periodic SMI.

  @retval         EFI Status              Describing the result of the operation.
  @retval         EFI_INVALID_PARAMETER   Some parameter value passed is not supported.

**/
EFI_STATUS
EFIAPI
PeiActivate (
  IN       EFI_PEI_SERVICES               **PeiServices,
  IN       PEI_SMM_CONTROL_PPI            *This,
  IN OUT  INT8                            *ArgumentBuffer OPTIONAL,
  IN OUT  UINTN                           *ArgumentBufferSize OPTIONAL,
  IN      BOOLEAN                         Periodic OPTIONAL,
  IN      UINTN                           ActivationInterval OPTIONAL
  );

/**
  This routine clears an SMI.

  @param[in] PeiServices             General purpose services available to every PEIM.
  @param[in] This                    The EFI SMM Control ppi instance.
  @param[in] Periodic                Periodic or not.

  @retval    EFI Status              Describing the result of the operation.
  @retval    EFI_INVALID_PARAMETER   Some parameter value passed is not supported.

**/
EFI_STATUS
EFIAPI
PeiDeactivate (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN PEI_SMM_CONTROL_PPI          *This,
  IN  BOOLEAN                     Periodic OPTIONAL
  );

#endif

