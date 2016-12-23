/** @file
  This library class provides CMOS access functions.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CMOS_ACCESS_LIB_H_
#define _CMOS_ACCESS_LIB_H_

/**
  Returns the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to read from CMOS
  @param[out] Data         The Value read from CMOS location

  @retval     Status

**/
RETURN_STATUS
EFIAPI
ReadCmos (
  IN  UINT8 CmosAddress,
  OUT UINT8 *Data
  );

/**
  Write the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in] CmosAddress  Location to write to CMOS
  @param[in] Data         Value to be written to the CMOS

  @retval    Status

**/
RETURN_STATUS
EFIAPI
WriteCmos (
  IN UINT8 CmosAddress,
  IN UINT8 *Data
  );

/**
  Returns the 1 Byte value from a CMOS location.

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data The Value read from CMOS location

**/
UINT8
EFIAPI
ReadCmos8 (
  IN  UINT8  CmosAddress
  );

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
  );

/**
  Returns the 2 Bytes value from a CMOS location.

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data         The Value read from CMOS location

**/
UINT16
EFIAPI
ReadCmos16 (
  IN  UINT8  CmosAddress
  );

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
  );

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
  );

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
  );

/**
  Funtion to Initialize the CMOS.

  Checks the presence of CMOS battery, else it initialize CMOS to default.
  Perform a checksum computation and verify if the checksum is correct.
  If the input parameter ForceInit is TRUE, initialize all the CMOS
  location to their default values

  @param[in]  ForceInit         A boolean variable to initialize the CMOS to its default
                                without checking the RTC_PWR_STS or verifying the checksum.

  @param[out] DefaultsRestored  A boolean variable to indicate if the defaults were restored

  @retval     Status

**/
RETURN_STATUS
EFIAPI
InitCmos (
  IN  BOOLEAN     ForceInit,
  OUT BOOLEAN     *DefaultsRestored
  );

#endif // _CMOS_ACCESS_LIB_H_

