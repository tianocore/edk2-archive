/** @file
  CMOS access library instance.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BaseCmosAccessLib.h"


/**
  Writes an 8-bit to I/O port.

  Writes the 8-bit to I/O port specified by PortAdd with the value specified by Value
  and returns Value.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param[in]  PortAdd  The I/O port to write.
  @param[in]  Value    The value to write to the I/O port.

  @retval     Value    The value written to the I/O port.

**/
UINT8
WriteIoPort (
  IN UINT8 PortAdd,
  IN UINT8 Value
  )
{
  return IoWrite8 (PortAdd, Value);
}


/**
  Reads an 8-bit from I/O port.

  Reads the 8-bit from I/O port specified by PortAdd and returns value.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param[in]  PortAdd  The I/O port to read.

  @retval     Value    The value read from the I/O port.

**/
UINT8
ReadIoPort (
  IN UINT8 PortAdd
  )
{
  return IoRead8 (PortAdd);
}


/**
  Check if the CMOS location  has to be included for the Checksum calculation.

  @param[in]  CmosAddCnt Cmos address to be checked

  @retval     TRUE       CMOS location need to be skipped for Checksum calculation.
  @retval     FALSE      CMOS location need to be included for Checksum calculation.

**/
BOOLEAN
CheckIfSkipChkSum (
  IN UINT8 CmosAddCnt
  )
{
  UINTN              TabLen;
  UINT16             Count;
  CMOS_ENTRY         *TabPtr;
  EFI_STATUS         Status;

  Status = GetPlatformCmosEntry (&TabPtr, &TabLen);
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  for (Count = 0; Count < TabLen; Count++) {
    if (TabPtr[Count].CmosAddress == CmosAddCnt) {
      if (TabPtr[Count].Attribute & CMOS_ATTRIBUTE_EXCLUDE_FROM_CHECKSUM) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
  }

  return TRUE;
}


/**
  Check if the CMOS location  has to be included for filling default data.

  @param[in]  CmosAddCnt Cmos address to be checked

  @retval     TRUE       CMOS location need to be skipped for filling default data.
  @retval     FALSE      CMOS location need to be included for filling default data.

**/
BOOLEAN
CheckIfSkipFillData (
  IN UINT8 CmosAddCnt
  )
{
  UINTN              TabLen;
  UINT16             Count;
  CMOS_ENTRY         *TabPtr;
  EFI_STATUS         Status;

  Status = GetPlatformCmosEntry (&TabPtr, &TabLen);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  for (Count = 0; Count < TabLen; Count++) {
    if (TabPtr[Count].CmosAddress == CmosAddCnt) {
      if (TabPtr[Count].Attribute & CMOS_ATTRIBUTE_EXCLUDE_FROM_INIT_DATA) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
  }

  return FALSE;
}


/**
  Performs the calculation of Checksum.

  @retval  ChkSumVal   calculated CheckSum value

**/
UINT16
CalculateCmosCheckSum (
  VOID
  )
{
  UINT16  ChkSumVal;
  UINT16  CmosReg;
  UINT8   Data;
  BOOLEAN NeedSkip;

  ChkSumVal = 0;
  Data = 0;

  for (CmosReg = CMOS_START_ADDR; CmosReg <= CMOS_END_ADDR; CmosReg ++) {
    NeedSkip = CheckIfSkipChkSum ((UINT8) CmosReg);

    //
    // Skip these during checksum calculation.
    // If the CMOS table Attribute is 1 and
    // If the place of saving checksum
    //
    if (NeedSkip || (CmosReg == CMOS_CHECKSUM_ADDR_LOW) || (CmosReg == CMOS_CHECKSUM_ADDR_HIGH)) {
      continue;
    }

    ReadCmos ((UINT8) CmosReg, &Data);
    ChkSumVal = ChkSumVal + Data;
  }

  return ChkSumVal;
}


/**
  Reads from the checksum address.

  Reads the 8-bit checksum from specified CheckSum address. The 8-bit read value is returned.
  If 8-bit I/O port operations are not supported, then ASSERT().

  @retval ChkSumVal  The value read.

**/
UINT16
ReadCheckSum (
  VOID
  )
{
  UINT16 ChkSumVal;
  UINT8  LVal;
  UINT8  HVal;
  UINT8  APort;
  UINT8  DPort;
  UINT8  TCmosAddress;

  //
  // Check if Check sum address lies in the CMOS Lower memory or CMOS upper memory
  //
  if ((CMOS_CHECKSUM_ADDR_LOW >= CMOS_LOW_MEM_ST) && (CMOS_CHECKSUM_ADDR_LOW < CMOS_LOW_MEM_END)) {
    APort = PORT_70;
    DPort = PORT_71;
  } else {
    APort = PORT_72;
    DPort = PORT_73;
  }

  TCmosAddress = CMOS_CHECKSUM_ADDR_HIGH;
  WriteIoPort (APort, TCmosAddress);
  HVal = ReadIoPort (DPort);

  TCmosAddress = CMOS_CHECKSUM_ADDR_LOW;
  WriteIoPort (APort, TCmosAddress);
  LVal = ReadIoPort (DPort);

  ChkSumVal = HVal;
  ChkSumVal = ChkSumVal << 8;
  ChkSumVal = ChkSumVal + LVal;

  return ChkSumVal;
}


/**
  Write the Checksum to appropriate address.

  Write the 8-bit checksum to specified CheckSum address.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param[in]  ChecksumValue

**/
VOID
WriteCheckSum (
  IN UINT16 ChecksumValue
  )
{
  UINT8 LVal;
  UINT8 HVal;
  UINT8 APort;
  UINT8 DPort;
  UINT8 TCmosAddress;

  //
  // spliting the 16 bit checksum
  //
  LVal = (UINT8) (ChecksumValue & 0x00FF);
  HVal = (UINT8) ((ChecksumValue >> 8) & 0x00FF);

  //
  // Check if Check sum address lies in the CMOS Lower memory or CMOS upper memory
  //
  if ((CMOS_CHECKSUM_ADDR_LOW >= CMOS_LOW_MEM_ST) && (CMOS_CHECKSUM_ADDR_LOW < CMOS_LOW_MEM_END)) {
    APort = PORT_70;
    DPort = PORT_71;
  } else {
    APort = PORT_72;
    DPort = PORT_73;
  }

  TCmosAddress = CMOS_CHECKSUM_ADDR_LOW ;
  WriteIoPort (APort, TCmosAddress);
  WriteIoPort (DPort, LVal);

  TCmosAddress = CMOS_CHECKSUM_ADDR_HIGH ;
  WriteIoPort (APort, TCmosAddress);
  WriteIoPort (DPort, HVal);

  return ;
}


/**
  Calculates and update the Checksum to appropriate address.

  Write the 8-bit checksum to specified CheckSum address.

**/
VOID
UpdateCheckSum (
  VOID
  )
{
  UINT16      NewChkSum;

  //
  // Calculate the new checksum
  //
  NewChkSum = CalculateCmosCheckSum ();

  //
  // Write the New checksum to the Checksum field
  //
  WriteCheckSum (NewChkSum);

  return;
}


/**
  Check if the CMOS is corrupted by verifing the Checksum value.

  @retval  TRUE      CMOS checksum is correct
  @retval  FALSE     CMOS checksum is wrong

**/
BOOLEAN
VerifyCmosCheckSum (
  VOID
  )
{
  UINT16      StoredChkSum;
  UINT16      CurrentChkSum;

  StoredChkSum = (UINT16) ReadCheckSum ();
  CurrentChkSum = (UINT16) CalculateCmosCheckSum ();

  if (CurrentChkSum == StoredChkSum) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
  Fill CMOS registers with the default values.

**/
VOID
CmosFillDefaults (
  VOID
  )
{
  EFI_STATUS         Status;
  UINT16             CmosAddress;
  UINT16             Count;
  UINTN              TabLen;
  UINT8              DefVal;
  CMOS_ENTRY         *TabPtr;

  Status = GetPlatformCmosEntry (&TabPtr, &TabLen);
  if (EFI_ERROR (Status)) {
    return ;
  }

  //
  // Traverse thro entire CMOS location and fill it with zero
  //
  for (CmosAddress = CMOS_START_ADDR ; CmosAddress <= CMOS_END_ADDR; CmosAddress++) {
    DefVal = 0;
    if (!CheckIfSkipFillData ((UINT8) CmosAddress)) {
      WriteCmos ((UINT8) CmosAddress, &DefVal);
    }
  }

  //
  // Now fill only the CMOS location specified in the table
  // with default value from the table
  //
  for (Count = 0; Count < TabLen; Count++) {
    CmosAddress = (UINT8) (TabPtr[Count].CmosAddress);
    DefVal = TabPtr[Count].DefaultValue;
    if (!CheckIfSkipFillData ((UINT8) CmosAddress)) {
      WriteCmos ((UINT8) CmosAddress, &DefVal);
    }
  }

  //
  // Calculate and update the new checksum
  //
  UpdateCheckSum ();
  return ;
}


/**
  Returns the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress    Location to read from CMOS
  @param[out] Data           Contains the value read from the CMOS

  @retval     RETURN_SUCCESS
  @retval     RETURN_NOT_FOUND

**/
RETURN_STATUS
EFIAPI
ReadCmos (
  IN  UINT8 CmosAddress,
  OUT UINT8 *Data
  )
{
  //
  // Initial validation of the CMOS address
  //
  if ((CmosAddress < CMOS_START_ADDR) || (CmosAddress > CMOS_END_ADDR) ||
      (CmosAddress == CMOS_CHECKSUM_ADDR_LOW) || (CmosAddress == CMOS_CHECKSUM_ADDR_HIGH)) {
    return RETURN_NOT_FOUND;
  }

  if (CmosAddress <= CMOS_LOW_MEM_END) {
    WriteIoPort (PORT_70, CmosAddress);
    *Data = (UINT8) ReadIoPort (PORT_71);
  } else {
    WriteIoPort (PORT_72, CmosAddress);
    *Data= (UINT8) ReadIoPort (PORT_73);
  }

  return RETURN_SUCCESS;
}


/**
  Write the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     RETURN_SUCCESS
  @retval     RETURN_NOT_FOUND
  @retval     Status

**/
RETURN_STATUS
EFIAPI
WriteCmos (
  IN UINT8 CmosAddress,
  IN UINT8 *Data
  )
{
  RETURN_STATUS  Status;
  UINT16         OldChkSum;
  UINT16         NewChkSum;
  UINT8          OldCmosVal;
  BOOLEAN        NeedSkip;

  OldCmosVal = 0;

  if ((CmosAddress < CMOS_START_ADDR) || (CmosAddress > CMOS_END_ADDR) ||
      (CmosAddress == CMOS_CHECKSUM_ADDR_LOW) || (CmosAddress == CMOS_CHECKSUM_ADDR_HIGH)) {
    return RETURN_NOT_FOUND;
  }

  //
  // Read the actual value in the CMOS location to Write
  //
  NeedSkip = CheckIfSkipChkSum (CmosAddress);
  if (!NeedSkip) {
    Status = ReadCmos (CmosAddress, &OldCmosVal);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Write data to CMOS address given
  //
  if (CmosAddress <= CMOS_LOW_MEM_END) {
    WriteIoPort (PORT_70, CmosAddress);
    WriteIoPort (PORT_71, *Data);
  } else {
    WriteIoPort (PORT_72, CmosAddress);
    WriteIoPort (PORT_73, *Data);
  }

  //
  // Update the checksum optimization
  //
  if (!NeedSkip) {
    OldChkSum = ReadCheckSum ();
    NewChkSum = (OldChkSum - (UINT16) OldCmosVal + (UINT16) (*Data));
    WriteCheckSum (NewChkSum);
  }

  return RETURN_SUCCESS;
}


/**
  Returns the 16bit value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to read from CMOS
  @param[out] Data         The Value read from CMOS location

  @retval     RETURN_SUCCESS
  @retval     Status

**/
RETURN_STATUS
InternalReadCmosN (
  IN  UINT8  CmosAddress,
  OUT UINTN  *Data,
  IN  UINTN  ByteCount
  )
{
  RETURN_STATUS Status;
  UINT8         Index;
  UINT8         *Value;

  Value = (UINT8 *) Data;

  for (Index = 0; Index < ByteCount; Index++) {
    Status = ReadCmos (CmosAddress + Index, Value);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
    Value++;
  }

  return RETURN_SUCCESS;
}


/**
  Write the 16bit value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     RETURN_SUCCESS
  @retval     Status

**/
RETURN_STATUS
InternalWriteCmosN (
  IN UINT8  CmosAddress,
  IN UINTN  *Data,
  IN UINTN  ByteCount
  )
{
  RETURN_STATUS Status;
  UINT8         Index;
  UINT8         *Value;

  Value = (UINT8 *) Data;

  for (Index = 0; Index < ByteCount; Index++) {
    Status = WriteCmos (CmosAddress + Index, Value);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
    Value++;
  }

  return RETURN_SUCCESS;
}


/**
  Returns the 1 Byte value from a CMOS location.

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data         The Value read from CMOS location

**/
UINT8
EFIAPI
ReadCmos8 (
  IN  UINT8  CmosAddress
  )
{
  UINT8         Data;

  Data = 0;
  InternalReadCmosN (CmosAddress, (UINTN *) &Data, 1);
  return Data;
}


/**
  Write the 1 Byte value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status

**/
RETURN_STATUS
EFIAPI
WriteCmos8 (
  IN UINT8  CmosAddress,
  IN UINT8  Data
  )
{
  return InternalWriteCmosN (CmosAddress, (UINTN *) &Data, 1);
}


/**
  Returns the 2 Bytes value from a CMOS location.

  If the passed address is beyond the max address return 0.

  @param[in]    CmosAddress  Location to read from CMOS

  @retval       Data         The Value read from CMOS location

**/
UINT16
EFIAPI
ReadCmos16 (
  IN  UINT8  CmosAddress
  )
{
  UINT16        Data;

  Data = 0;
  InternalReadCmosN (CmosAddress, (UINTN *) &Data, 2);
  return Data;
}


/**
  Write the 2 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status

**/
RETURN_STATUS
EFIAPI
WriteCmos16 (
  IN UINT8  CmosAddress,
  IN UINT16 Data
  )
{
  return InternalWriteCmosN (CmosAddress, (UINTN *) &Data, 2);
}


/**
  Returns the 4 Bytes value from a CMOS location.

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data         The Value read from CMOS location

**/
UINT32
EFIAPI
ReadCmos32 (
  IN  UINT8  CmosAddress
  )
{
  UINT32        Data;

  Data = 0;
  InternalReadCmosN (CmosAddress, (UINTN *) &Data, 4);
  return Data;
}


/**
  Write the 4 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status

**/
RETURN_STATUS
EFIAPI
WriteCmos32 (
  IN UINT8  CmosAddress,
  IN UINT32 Data
  )
{
  return InternalWriteCmosN (CmosAddress, (UINTN *) &Data, 4);
}


/**
  Funtion to Initialize the CMOS.

  Checks the presence of CMOS battery, else it initialize CMOS to default.
  Perform a checksum computation and verify if the checksum is correct.
  If the input parameter ForceInit is TRUE, initialize all the CMOS
  location to their default values

  @param[in]  ForceInit         A boolean variable to initialize the CMOS to its default
                                without checking the RTC_PWR_STS or verifying the checksum.
  @param[out] DefaultsRestored  A boolean variable to indicate if the defaults were restored

  @retval     RETURN_SUCCESS

**/
RETURN_STATUS
EFIAPI
InitCmos (
  IN  BOOLEAN     ForceInit,
  OUT BOOLEAN     *DefaultsRestored
  )
{
  //
  // Check if the CMOS battery is present
  //
  if (!CheckCmosBatteryStatus ()) {
    ForceInit = TRUE;
  }

  //
  // Check if the ForceInit is True
  //
  if (ForceInit) {
    CmosFillDefaults ();
    *DefaultsRestored = TRUE;
    return RETURN_SUCCESS;
  }

  //
  // Check if CMOS is BAD
  //
  if (!VerifyCmosCheckSum ()) {
    CmosFillDefaults ();
    *DefaultsRestored = TRUE;
    return RETURN_SUCCESS;
  }

  *DefaultsRestored = FALSE;
  return RETURN_SUCCESS;
}

