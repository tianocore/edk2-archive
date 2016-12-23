/** @file
  This driver initialize the power and performance Setting.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PnPDxe.h"

PNP_SETTING mPnPValueForPerf[] = {
  VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT,
};

PNP_SETTING mPnPValueForPower[] = {
  VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT,
};

PNP_SETTING mPnPValueForPwrPerf[] = {
  VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT,
};

VOID
PnpSetting (
VOID
  )
{
  EFI_STATUS             Status;
  SYSTEM_CONFIGURATION   SystemConfiguration;
  UINTN                  VarSize;
  volatile UINT32        Index;
  UINT8                  Index1;
  UINT32                 Value;
  UINT32                 Mask;
  PNP_SETTING*           PnpSettingDataArray;
  UINT32                 PnpSettingArraySize;

  Status = EFI_SUCCESS;

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  if (SystemConfiguration.PnpSettings == PNP_POWER) {
    //
    // Power
    //
    PnpSettingDataArray = mPnPValueForPower;
    PnpSettingArraySize = sizeof (mPnPValueForPower);
  } else if (SystemConfiguration.PnpSettings == PNP_PERF) {
    //
    // Performance
    //
    PnpSettingDataArray = mPnPValueForPerf;
    PnpSettingArraySize = sizeof (mPnPValueForPerf);
  } else if (SystemConfiguration.PnpSettings == PNP_POWER_PERF) {
    //
    // Power & Performance
    //
    PnpSettingDataArray = mPnPValueForPwrPerf;
    PnpSettingArraySize = sizeof (mPnPValueForPwrPerf);
  } else {
    DEBUG ((EFI_D_INFO, "No PnP Setting been Configured !!!\n"));
    return;
  }

  for (Index = 0; Index <  PnpSettingArraySize / sizeof (PNP_SETTING); Index++) {
    if ((PnpSettingDataArray[Index].MsgPort ==0) && (PnpSettingDataArray[Index].MsgRegAddr == 0) &&(PnpSettingDataArray[Index].Value ==0)) {
      continue;
    }

    Mask = 0;
    for (Index1 = PnpSettingDataArray[Index].LSB; Index1 <= PnpSettingDataArray[Index].MSB; Index1++) {
      Mask |=  (1 << Index1);
    }

    DEBUG ((EFI_D_INFO, "Output PwrPerf Register: PortID: 0x%x, Offset:0x%x, Field[%2d:%2d], Value:%8x\n", PnpSettingDataArray[Index].MsgPort, PnpSettingDataArray[Index].MsgRegAddr,PnpSettingDataArray[Index].MSB, PnpSettingDataArray[Index].LSB, PnpSettingDataArray[Index].Value));

    Value = SideBandRead32(PnpSettingDataArray[Index].MsgPort, PnpSettingDataArray[Index].MsgRegAddr);
    Value &=  ~Mask;
    Value |=  (PnpSettingDataArray[Index].Value << PnpSettingDataArray[Index].LSB);
    SideBandWrite32 (PnpSettingDataArray[Index].MsgPort, PnpSettingDataArray[Index].MsgRegAddr, Value);
  }
}


/**
  Initialize the power and performance Setting.

  @param[in]  ImageHandle     Pointer to the loaded image protocol for this driver
  @param[in]  SystemTable     Pointer to the EFI System Table

  @retval     EFI_SUCCESS     The driver installed/initialized correctly.

**/
EFI_STATUS
EFIAPI
InitializePnpSetting (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  DEBUG ((EFI_D_INFO, "PnpSetting Entering !!!\n"));
  PnpSetting ();

  return EFI_SUCCESS;
}

