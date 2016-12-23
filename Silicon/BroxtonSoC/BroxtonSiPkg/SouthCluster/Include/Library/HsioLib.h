/** @file
  Header file for ScHsioLib.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_HSIO_LIB_H_
#define _SC_HSIO_LIB_H_

/**
  The function returns the Port Id and lane owner for the specified lane.

  @param[in]  LaneNum                 Lane number that needs to be checked
  @param[out] PortId                  Common Lane End Point ID
  @param[out] LaneOwner               Lane Owner

  @retval     EFI_SUCCESS             Read success
  @retval     EFI_INVALID_PARAMETER   Invalid lane number

**/
EFI_STATUS
EFIAPI
ScGetLaneInfo (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  );

/**
  Determine the lane number of a specified port.

  @param[in]  PcieLaneIndex                 PCIE Root Port Lane Index
  @param[out] LaneNum                       Lane Number

  @retval     EFI_SUCCESS                   Lane number valid.
  @retval     EFI_UNSUPPORTED               Incorrect input device port

**/
EFI_STATUS
ScGetPcieLaneNum (
  IN UINT32              PcieLaneIndex,
  OUT UINT8              *LaneNum
  );

/**
  Determine the lane number of a specified port.

  @param[in]  SataLaneIndex                 Sata Lane Index
  @param[out] LaneNum                       Lane Number

  @retval     EFI_SUCCESS                   Lane number valid.
  @retval     EFI_UNSUPPORTED               Incorrect input device port

**/
EFI_STATUS
ScGetSataLaneNum (
  IN UINT32              SataLaneIndex,
  OUT UINT8              *LaneNum
  );

/**
  Determine the lane number of a specified port.

  @param[in]  Usb3LaneIndex                 USB3 Lane Index
  @param[out] LaneNum                       Lane Number

  @retval     EFI_SUCCESS                   Lane number valid.
  @retval     EFI_UNSUPPORTED               Incorrect input device port

**/
EFI_STATUS
ScGetUsb3LaneNum (
  IN UINT32              Usb3LaneIndex,
  OUT UINT8              *LaneNum
  );

#endif // _SC_HSIO_LIB_H_

