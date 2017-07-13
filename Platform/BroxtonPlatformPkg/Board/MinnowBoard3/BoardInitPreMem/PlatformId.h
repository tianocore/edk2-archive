/** @file
  Header file for the Platform ID code.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __MINNOWBOARD_PLATFORM_ID_H__
#define __MINNOWBOARD_PLATFORM_ID_H__

//
// Strap Fw Cfg ID define
//
#define IO_EXPANDER_I2C_BUS_NO   0x06
#define IO_EXPANDER_SLAVE_ADDR   0x22
#define IO_EXPANDER_INPUT_REG_0  0x00
#define IO_EXPANDER_INPUT_REG_1  0x01
#define IO_EXPANDER_INPUT_REG_2  0x02

EFI_STATUS
EFIAPI
Minnow3GetFwCfgId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *FwCfgId
  );

EFI_STATUS
EFIAPI
Minnow3GetBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  );

EFI_STATUS
EFIAPI
Minnow3GetEmbeddedBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  );

EFI_STATUS
EFIAPI
Minnow3GetIVIBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  );

EFI_STATUS
EFIAPI
Minnow3GetDockId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *DockId
  );

EFI_STATUS
EFIAPI
Minnow3GetOsSelPss (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *OsSelPss
  );

EFI_STATUS
EFIAPI
Minnow3GetBomIdPss (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BomIdPss
  );

#endif

