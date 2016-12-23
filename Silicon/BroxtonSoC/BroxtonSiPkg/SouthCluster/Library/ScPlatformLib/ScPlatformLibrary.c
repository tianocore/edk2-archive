/** @file
  SC Platform Lib implementation.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScPlatformLibrary.h"
#include <Library/SteppingLib.h>

//
// For BXT-P, it supports two express port controllers:
//   Controller 1:
//     Port 1-2, Device 20, function 0-1
//   Controller 2:
//     Port 3-6, Device 19, function 0-3
// For BXT-X, it supports one express port controllers:
//   Controller 1:
//     Port 1-4, Device 19, function 0-3
// For BXT-0/1, it supports one express port controllers:
//   Controller 1:
//     Port 1-2, Device 20, function 0-1
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST PCIE_CONTROLLER_INFO mPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1, 0xB3,  0 },
  { PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2, 0xB4,  2 }
};
GLOBAL_REMOVE_IF_UNREFERENCED CONST UINT32 mPcieControllerInfoSize = sizeof (mPcieControllerInfo) / sizeof (mPcieControllerInfo[0]);


/**
  Get Sc Maximum Pcie Root Port Number

  @retval UINT8                         Sc Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetScMaxPciePortNum (
  VOID
  )
{
  BXT_SERIES  BxtSeries;

  BxtSeries = GetBxtSeries ();
  switch (BxtSeries) {
    case BxtX:
      return SC_BXTX_PCIE_MAX_ROOT_PORTS;
    case BxtP:
      return SC_BXTP_PCIE_MAX_ROOT_PORTS;
    case Bxt1:
      return SC_BXT_PCIE_MAX_ROOT_PORTS;
    default:
      return 0;
  }
}


/**
  Delay for at least the request number of microseconds.
  This function would be called by runtime driver, please do not use any MMIO marco here.

  @param[in] Microseconds               Number of microseconds to delay.

**/
VOID
EFIAPI
ScPmTimerStall (
  IN  UINTN   Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;
  UINT16  AcpiBaseAddr;

  if (Microseconds == 0) {
    return;
  }

  //
  // Read ACPI Base Address
  //
  AcpiBaseAddr = (UINT16)PcdGet16(PcdScAcpiIoPortBaseAddress);

  OriginalTick  = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_TMR)) & B_ACPI_PM1_TMR_VAL;
  CurrentTick   = OriginalTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  //
  // The loops needed by timer overflow
  //
  Counts = Ticks / V_ACPI_PM1_TMR_MAX_VAL;

  //
  // Remaining clocks within one loop
  //
  RemainingTick = Ticks % V_ACPI_PM1_TMR_MAX_VAL;

  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 ((UINTN) (AcpiBaseAddr + R_ACPI_PM1_TMR)) & B_ACPI_PM1_TMR_VAL;
    //
    // Check if timer overflow
    //
    if ((CurrentTick < OriginalTick)) {
      if (Counts != 0) {
        Counts--;
      } else {
        //
        // If timer overflow and Counts equ to 0, that means we already stalled more than
        // RemainingTick, break the loop here
        //
        break;
      }
    }

    OriginalTick = CurrentTick;
  }
}


/**
  Check whether SPI is in descriptor mode

  @param[in] SpiBase                   The SC SPI Base Address

  @retval    TRUE                      SPI is in descriptor mode
  @retval    FALSE                     SPI is not in descriptor mode

**/
BOOLEAN
EFIAPI
ScIsSpiDescriptorMode (
  IN  UINTN   SpiBase
  )
{
  if ((MmioRead16 (SpiBase + R_SPI_HSFS) & B_SPI_HSFS_FDV) == B_SPI_HSFS_FDV) {
    MmioAndThenOr32 (
      SpiBase + R_SPI_FDOC,
      (UINT32) (~(B_SPI_FDOC_FDSS_MASK | B_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_SPI_FDOC_FDSS_FSDM | R_SPI_FDBAR_FLVALSIG)
      );
    if ((MmioRead32 (SpiBase + R_SPI_FDOD)) == V_SPI_FDBAR_FLVALSIG) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}


/**
  This function can be called to poll for certain value within a time given.

  @param[in] MmioAddress               The Mmio Address.
  @param[in] BitMask                   Bits to be masked.
  @param[in] BitValue                  Value to be polled.
  @param[in] DelayTime                 Delay time in terms of 100 micro seconds.

  @retval    EFI_SUCCESS               Successfully polled the value.
  @retval    EFI_TIMEOUT               Timeout while polling the value.

**/
EFI_STATUS
EFIAPI
ScMmioPoll32 (
  IN  UINTN         MmioAddress,
  IN  UINT32        BitMask,
  IN  UINT32        BitValue,
  IN  UINT16        DelayTime
  )
{
  UINT32  LoopTime;
  UINT8   PollSuccess;

  LoopTime    = 0;
  PollSuccess = 0;

  for (LoopTime = 0; LoopTime < DelayTime; LoopTime++) {
    if ((MmioRead32 (MmioAddress) & BitMask) == BitValue) {
      PollSuccess = 1;
      break;
    } else {
      ScPmTimerStall (100);
    }
  }

  if (PollSuccess) {
    return EFI_SUCCESS;
  } else {
    return EFI_TIMEOUT;
  }
}


/**
  Determine if the specificed device is available

  @param[in] RpDev     Device number

  @retval    FALSE     Device is not available.
             TRUE      Device is available.

**/
BOOLEAN
IsPcieControllerAvailable (
  IN UINTN   *RpDev
)
{
  UINT32  FuncDisableReg;
  UINT32  FuncDisableMask;

  if (*RpDev == PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1) {
    FuncDisableReg  = MmioRead32 (PMC_BASE_ADDRESS + R_PMC_FUNC_DIS);
    FuncDisableMask = (B_PMC_FUNC_DIS_PCIE0_P1 | B_PMC_FUNC_DIS_PCIE0_P0);
  } else {
    FuncDisableReg = MmioRead32 (PMC_BASE_ADDRESS + R_PMC_FUNC_DIS_1);
    FuncDisableMask = (B_PMC_FUNC_DIS_1_PCIE1_P3 | B_PMC_FUNC_DIS_1_PCIE1_P2 | B_PMC_FUNC_DIS_1_PCIE1_P1 | B_PMC_FUNC_DIS_1_PCIE1_P0);
  }

  if ((FuncDisableReg & FuncDisableMask) == FuncDisableMask) {
    return FALSE;
  } else {
    return TRUE;
  }
}


/**
  Get Sc Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid

**/
EFI_STATUS
EFIAPI
GetScPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  UINTN       Index;
  UINTN       FuncIndex;
  UINT32      PciePcd;

  //
  // if BXT SC , RpNumber must be < 6.
  //
  if (RpNumber >= GetScMaxPciePortNum ()) {
    DEBUG ((DEBUG_ERROR, "GetScPcieRpDevFun invalid RpNumber %x\n", RpNumber));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (RpNumber >= 2) {
    Index = 1;
  }else {
    Index = 0;
  }
  FuncIndex = RpNumber - mPcieControllerInfo[Index].RpNumBase;
  *RpDev = mPcieControllerInfo[Index].DevNum;

  //
  // Before doing the actual PCD read, ensure the device is available
  //
  if (IsPcieControllerAvailable (RpDev) == TRUE) {
    PchPcrRead32 (mPcieControllerInfo[Index].Pid, 0x0000, &PciePcd);
    *RpFun = (PciePcd >> (FuncIndex * 4)) & (BIT2 | BIT1 | BIT0);
  } else {
    *RpFun = FuncIndex;
  }

  return EFI_SUCCESS;
}

/**
  Get Root Port physical Number by Sc Pcie Root Port Device and Function Number

  @param[in]  RpDev                 Root port device number.
  @param[in]  RpFun                 Root port function number.
  @param[out] RpNumber              Return corresponding Root port physical number.

  @retval     EFI_SUCCESS           Physical root port is retrieved
  @retval     EFI_INVALID_PARAMETER RpDev and/or RpFun are invalid
  @retval     EFI_UNSUPPORTED       Root port device and function is not assigned to any physical root port

**/
EFI_STATUS
EFIAPI
GetScPcieRpNumber (
  IN  UINTN   RpDev,
  IN  UINTN   RpFun,
  OUT UINTN   *RpNumber
  )
{
  UINTN       Index;
  UINTN       FuncIndex;
  UINT32      PciePcd;

  //
  // if BXT SC, must be Dev == 19 and Fun < 4, or Dev == 20 and Fun < 2.
  //
  if (!((RpDev == PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_1) && (RpFun < 2)) &&
      !((RpDev == PCI_DEVICE_NUMBER_SC_PCIE_DEVICE_2) && (RpFun < 4))) {
    DEBUG ((DEBUG_ERROR, "GetPchPcieRpNumber invalid RpDev %x RpFun %x", RpDev, RpFun));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < mPcieControllerInfoSize; Index++) {
    if (mPcieControllerInfo[Index].DevNum == RpDev) {
      PchPcrRead32 (mPcieControllerInfo[Index].Pid, 0x0000, &PciePcd);
      for (FuncIndex = 0; FuncIndex < 4; FuncIndex ++) {
        if (RpFun == ((PciePcd >> (FuncIndex * 4)) & (BIT2 | BIT1 | BIT0))) {
          break;
        }
      }
      if (FuncIndex < 4) {
        *RpNumber = mPcieControllerInfo[Index].RpNumBase + FuncIndex;
        break;
      }
    }
  }
  if (Index >= mPcieControllerInfoSize) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


/**
  Read PCR register. (This is internal function)
  It returns PCR register and size in 1byte/2bytes/4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[in]  Size                      Size for read. Must be 1 or 2 or 4.
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
STATIC
EFI_STATUS
PchPcrRead (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINTN                             Size,
  OUT UINT32                            *OutData
  )
{
  if ((Offset & (Size - 1)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchPcrRead error. Invalid Offset: %x Size: %x", Offset, Size));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  switch (Size) {
    case 4:
      *(UINT32 *) OutData = MmioRead32 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    case 2:
      *(UINT16 *) OutData = MmioRead16 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    case 1:
      *(UINT8 *) OutData = MmioRead8 (SC_PCR_ADDRESS (Pid, Offset));
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}


/**
  Read PCR register.
  It returns PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT32                            *OutData
  )
{
  return PchPcrRead (Pid, Offset, 4, (UINT32*) OutData);
}


/**
  Read PCR register.
  It returns PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT16                            *OutData
  )
{
  return PchPcrRead (Pid, Offset, 2, (UINT32*) OutData);
}


/**
  Read PCR register.
  It returns PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT8                             *OutData
  )
{
  return PchPcrRead (Pid, Offset, 1, (UINT32*) OutData);
}


#ifdef EFI_DEBUG
/**
  Check if the port ID is available for sideband mmio read/write

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID

  @retval     TRUE                      available for sideband Mmio read/write method
  @retval     FALSE                     inavailable for sideband Mmio read/write method

**/
BOOLEAN
PchPcrWriteMmioCheck (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset
  )
{
  return TRUE;
}
#endif


/**
  Write PCR register. (This is internal function)
  It programs PCR register and size in 1byte/2bytes/4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  Size                      Size for read. Must be 1 or 2 or 4.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
STATIC
EFI_STATUS
PchPcrWrite (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINTN                             Size,
  IN  UINT32                            InData
  )
{
  if ((Offset & (Size - 1)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchPcrWrite error. Invalid Offset: %x Size: %x", Offset, Size));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
#ifdef EFI_DEBUG
  if (!PchPcrWriteMmioCheck (Pid, Offset)) {
    DEBUG ((DEBUG_ERROR, "PchPcrWrite error. Pid: %x Offset: %x should access through SBI interface", Pid, Offset));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
#endif

  //
  // Write the PCR register with provided data
  // Then read back PCR register to prevent from back to back write.
  //
  switch (Size) {
    case 4:
      MmioWrite32 (SC_PCR_ADDRESS (Pid, Offset), (UINT32)InData);
      break;
    case 2:
      MmioWrite16 (SC_PCR_ADDRESS (Pid, Offset), (UINT16)InData);
      break;
    case 1:
      MmioWrite8  (SC_PCR_ADDRESS (Pid, Offset), (UINT8) InData);
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}


/**
  Write PCR register.
  It programs PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData
  )
{
  return PchPcrWrite (Pid, Offset, 4, InData);
}


/**
  Write PCR register.
  It programs PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            InData
  )
{
  return PchPcrWrite (Pid, Offset, 2, InData);
}


/**
  Write PCR register.
  It programs PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             InData
  )
{
  return PchPcrWrite (Pid, Offset, 1, InData);
}


/**
  Reads an 4-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 4-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  )
{
  EFI_STATUS                            Status;
  UINT32                                Data32;

  Status  = PchPcrRead (Pid, Offset, 4, &Data32);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Data32 &= AndData;
  Data32 |= OrData;
  Status  = PchPcrWrite (Pid, Offset, 4, Data32);

  return Status;
}


/**
  Reads an 2-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 2-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            AndData,
  IN  UINT16                            OrData
  )
{
  EFI_STATUS                            Status;
  UINT16                                Data16;

  Status  = PchPcrRead (Pid, Offset, 2, (UINT32*) &Data16);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Data16 &= AndData;
  Data16 |= OrData;
  Status  = PchPcrWrite (Pid, Offset, 2, Data16);

  return Status;
}


/**
  Reads an 1-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 1-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             AndData,
  IN  UINT8                             OrData
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Data8;

  Status  = PchPcrRead (Pid, Offset, 1, (UINT32 *) &Data8);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Data8 &= AndData;
  Data8 |= OrData;
  Status  = PchPcrWrite (Pid, Offset, 1, Data8);

  return Status;
}


/**
  Hide P2SB device.

  @param[in]  P2sbBase                      Pci base address of P2SB controller.

  @retval     EFI_SUCCESS                   Always return success.

**/
EFI_STATUS
PchHideP2sb (
  IN UINTN                                 P2sbBase
  )
{
  MmioWrite8 (P2sbBase + R_PCH_P2SB_E0 + 1, BIT0);
  return EFI_SUCCESS;
}


/**
  Reveal P2SB device.
  Also return the original P2SB status which is for Hidding P2SB or not after.
  If OrgStatus is not NULL, then TRUE means P2SB is unhidden,
  and FALSE means P2SB is hidden originally.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.
  @param[out] OrgStatus                 Original P2SB hidding/unhidden status

  @retval     EFI_SUCCESS               Always return success.

**/
EFI_STATUS
PchRevealP2sb (
  IN UINTN                                 P2sbBase,
  OUT BOOLEAN                              *OrgStatus
  )
{
  BOOLEAN                               DevicePresent;

  DevicePresent = (MmioRead16 (P2sbBase + PCI_VENDOR_ID_OFFSET) != 0xFFFF);
  if (OrgStatus != NULL) {
    *OrgStatus = DevicePresent;
  }
  if (!DevicePresent) {
    MmioWrite8 (P2sbBase + R_PCH_P2SB_E0 + 1, 0);
  }

  return EFI_SUCCESS;
}


/**
  Get Sc Maximum Usb3 Port Number of XHCI Controller

  @retval UINT8                         Sc Maximum Usb3 Port Number of XHCI Controller

**/
UINT8
EFIAPI
GetScXhciMaxUsb3PortNum (
  VOID
  )
{
  BXT_SERIES  BxtSeries;

  BxtSeries = GetBxtSeries ();
  switch (BxtSeries) {
    case Bxt:
    case Bxt1:
      return SC_BXT_MAX_USB3_PORTS;

    case BxtP:
      return SC_BXTP_MAX_USB3_PORTS;

    default:
      return 0;
  }
}


/**
  Get Sc Maximum Usb2 Port Number of XHCI Controller

  @retval UINT8                         Sc Maximum Usb2 Port Number of XHCI Controller

**/
UINT8
EFIAPI
GetScXhciMaxUsb2PortNum (
  VOID
  )
{
  BXT_SERIES  BxtSeries;

  BxtSeries = GetBxtSeries ();
  switch (BxtSeries) {
    case Bxt:
    case Bxt1:
      return SC_BXT_MAX_USB2_PORTS;

    case BxtP:
      return SC_BXTP_MAX_USB2_PORTS;

    default:
      return 0;
  }
}

