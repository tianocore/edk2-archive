/** @file
  This is the driver that publishes the SMM Control Ppi.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "SmmControlDriver.h"

EFI_GUID  mPeiSmmControlPpiGuid = PEI_SMM_CONTROL_PPI_GUID;

STATIC PEI_SMM_CONTROL_PPI      mSmmControlPpi = {
  PeiActivate,
  PeiDeactivate
};


STATIC EFI_PEI_PPI_DESCRIPTOR   mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &mPeiSmmControlPpiGuid,
  &mSmmControlPpi
};


/**
  This is the constructor for the SMM Control ppi

  @param[in] FfsHeader            FfsHeader.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval    EFI_STATUS           Results of the installation of the SMM Control Ppi

**/
EFI_STATUS
EFIAPI
SmmControlPeiDriverEntryInit (
  IN      EFI_PEI_FILE_HANDLE             FfsHeader,
  IN      CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Trigger the software SMI

  @param[in] Data                 The value to be set on the software SMI data port

  @retval    EFI_SUCCESS          Function completes successfully

**/
EFI_STATUS
EFIAPI
SmmTrigger (
  IN UINT8   Data
  )
{
  UINT32  OutputData;
  UINT32  OutputPort;
  UINT16  AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Enable the APMC SMI
  //
  OutputPort  = AcpiBaseAddr + R_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= (B_SMI_EN_APMC | B_SMI_EN_GBL_SMI);

  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  OutputPort  = R_APM_CNT;
  OutputData  = Data;

  //
  // Generate the APMC SMI
  //
  IoWrite8 (
    (UINTN) OutputPort,
    (UINT8) (OutputData)
    );

  return EFI_SUCCESS;
}


/**
  Clear the SMI status

  @param[in] None

  @retval    EFI_SUCCESS             The function completes successfully
  @retval    EFI_DEVICE_ERROR        Something error occurred

**/
EFI_STATUS
EFIAPI
SmmClear (
  VOID
  )
{
  UINT32      OutputData;
  UINT32      OutputPort;
  UINT16      AcpiBaseAddr;

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16) PcdGet16 (PcdScAcpiIoPortBaseAddress);

  //
  // Clear the Power Button Override Status Bit, it gates EOS from being set.
  //
  OutputPort  = AcpiBaseAddr + R_ACPI_PM1_STS;
  OutputData  = B_ACPI_PM1_STS_PRBTNOR;

  IoWrite16 (
    (UINTN) OutputPort,
    (UINT16) (OutputData)
    );

  //
  // Clear the APM SMI Status Bit
  //
  OutputPort  = AcpiBaseAddr + R_SMI_STS;
  OutputData  = B_SMI_STS_APM;
  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  //
  // Set the EOS Bit
  //
  OutputPort  = AcpiBaseAddr + R_SMI_EN;
  OutputData  = IoRead32 ((UINTN) OutputPort);
  OutputData |= B_SMI_EN_EOS;

  IoWrite32 (
    (UINTN) OutputPort,
    (UINT32) (OutputData)
    );

  //
  // If the EOS bit did not get set, then we've got a problem.
  //
  DEBUG_CODE (
    OutputData = IoRead32 ((UINTN) OutputPort);
    if ((OutputData & B_SMI_EN_EOS) != B_SMI_EN_EOS) {
    DEBUG ((EFI_D_ERROR, "Bugger, EOS did not get set!\n"));
    return EFI_DEVICE_ERROR;
    }
  );

  return EFI_SUCCESS;
}


/**
  This routine generates an SMI

  @param[in]      PeiServices             General purpose services available to every PEIM.
  @param[in]      This                    The EFI SMM Control ppi instance
  @param[in, out] ArgumentBuffer          The buffer of argument
  @param[in, out] ArgumentBufferSize      The size of the argument buffer
  @param[in]      Periodic                Periodic or not
  @param[in]      ActivationInterval      Interval of periodic SMI

  @retval         EFI Status              Describing the result of the operation
  @retval         EFI_INVALID_PARAMETER   Some parameter value passed is not supported

**/
EFI_STATUS
EFIAPI
PeiActivate (
  IN      EFI_PEI_SERVICES                **PeiServices,
  IN      PEI_SMM_CONTROL_PPI             *This,
  IN OUT  INT8                            *ArgumentBuffer OPTIONAL,
  IN OUT  UINTN                           *ArgumentBufferSize OPTIONAL,
  IN      BOOLEAN                         Periodic OPTIONAL,
  IN      UINTN                           ActivationInterval OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       Data;

  if (Periodic) {
    DEBUG ((EFI_D_WARN, "Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (ArgumentBuffer == NULL) {
    Data = 0xFF;
  } else {
    if (ArgumentBufferSize == NULL || *ArgumentBufferSize != 1) {
      return EFI_INVALID_PARAMETER;
    }

    Data = *ArgumentBuffer;
  }

  //
  // Clear any pending the APM SMI
  //
  Status = SmmClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SmmTrigger (Data);
}


/**
  This routine clears an SMI

  @param[in] PeiServices             General purpose services available to every PEIM.
  @param[in] This                    The EFI SMM Control ppi instance
  @param[in] Periodic                Periodic or not

  @retval    EFI Status              Describing the result of the operation
  @retval    EFI_INVALID_PARAMETER   Some parameter value passed is not supported

**/
EFI_STATUS
EFIAPI
PeiDeactivate (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_CONTROL_PPI             *This,
  IN  BOOLEAN                        Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return SmmClear ();
}

