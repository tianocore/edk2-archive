/** @file
  This DXE driver configures and supports DPTF.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Dptf.h"

EFI_GLOBAL_NVS_AREA_PROTOCOL  *mGlobalNvsArea;
extern EFI_GUID               gEfiGlobalNvsAreaProtocolGuid;

DPTF_DRIVER_DATA  gDptfDrvData;

typedef struct _LPAT_ENTRY {
  UINT16 TemperatureInKelvin;
  UINT16 RawTemperature;
} LPAT_ENTRY;

//
// LPAT table for a real sensor would be a 1:1 mapping in LPAT table.
//
LPAT_ENTRY LinearApproxTable[] = {
  //
  //Temp,  Raw Value
  //
  {2531,   977},
  {2581,   961},
  {2631,   941},
  {2681,   917},
  {2731,   887},
  {2781,   853},
  {2831,   813},
  {2881,   769},
  {2931,   720},
  {2981,   669},
  {3031,   615},
  {3081,   561},
  {3131,   508},
  {3181,   456},
  {3231,   407},
  {3281,   357},
  {3331,   315},
  {3381,   277},
  {3431,   243},
  {3481,   212},
  {3531,   186},
  {3581,   162},
  {3631,   140},
  {3731,   107}
};

/**
  SOC thermal initialization

**/
VOID
SocThermInit (
  VOID
);


/**
  Uninstalls a SSDT ACPI Table

  @retval  EFI_SUCCESS             Operation completed successfully.
  @retval  other                   Some error occurred when executing this function.

**/
EFI_STATUS
UninstallSSDTAcpiTable (
  UINT64  OemTableId
  )
{
  EFI_STATUS                    Status;
  UINTN                         Count = 0;
  UINTN                         Handle;
  EFI_ACPI_DESCRIPTION_HEADER   *Table;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_SUPPORT_PROTOCOL     *mAcpiSupport = NULL;

  //
  // Locate ACPI Support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **) &mAcpiSupport);
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status)) {
    do {
      Version = 0;
      Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Count, (VOID **) &Table, &Version, &Handle);
      if (EFI_ERROR (Status)) {
        break;
      }
      //
      // Check if this is a DPTF SSDT table. If so, uninstall the table
      //
      if (Table->Signature == EFI_ACPI_2_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
          Table->OemTableId == OemTableId) {
        DEBUG ((EFI_D_INFO, "DPTF SSDT Table found.Uninstalling it\n"));
        Status = mAcpiSupport->SetAcpiTable (mAcpiSupport, NULL, TRUE, Version, &Handle);
        ASSERT_EFI_ERROR (Status);
        Status = mAcpiSupport->PublishTables (
                   mAcpiSupport,
                   EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0
                 );
        ASSERT_EFI_ERROR (Status);
        break;
      }
      Count ++;
    } while (1);
  }

  return Status;
}


/**
  Uninstalls DPTF ACPI Tables

  @retval EFI_SUCCESS             Operation completed successfully.
  @retval other                   Some error occurred when executing this function.

**/
EFI_STATUS
UnInstallDptfAcpiTables (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = UninstallSSDTAcpiTable (DPTF_ACPI_CPU_TABLE_SIGNATURE);
  Status = UninstallSSDTAcpiTable (DPTF_ACPI_SOC_TABLE_SIGNATURE);
  Status = UninstallSSDTAcpiTable (DPTF_ACPI_LPM_TABLE_SIGNATURE);
  Status = UninstallSSDTAcpiTable (DPTF_ACPI_GEN_TABLE_SIGNATURE);

  return Status;
}


/**
  Reads DPTF policy to determine if DPTF is enabled in DPTF policy.

  @param[in, out] bDptfEnabled            TRUE, if DPTF is enabled in DPTF policy else FALSE.

  @retval         EFI_SUCCESS             Operation completed successfully.
  @retval         EFI_INVALID_PARAMETER   Input parameter is invalid
  @retval         other                   Some error occurred when executing this function.

**/
EFI_STATUS
DPTFEnabledInBIOS (
  IN OUT  BOOLEAN *bDptfEnabled
  )
{
  EFI_STATUS  Status  = EFI_SUCCESS;

  if (bDptfEnabled == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *bDptfEnabled = gDptfDrvData.bDptfEnabled;

  return Status;
}


/**
  Ready to Boot Event notification handler.

  Sequence of OS boot events is measured in this event notification handler.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context

**/
VOID
EFIAPI
OnReadyToBoot (
  IN  EFI_EVENT    Event,
  IN  VOID         *Context
  )
{
  EFI_STATUS  Status;
  BOOLEAN     bDptfEnabled = TRUE;

  //
  // Check if DPTF is enabled in DPTF policy
  //
  Status = DPTFEnabledInBIOS (&bDptfEnabled);
  DEBUG ((EFI_D_INFO, "DPTF OnReadyToBoot bDptfEnabled = 0x%x \n",bDptfEnabled));

  if (!bDptfEnabled) {
    //
    // DPTF is disabled in DPTF policy, Uninstall DPTF ACPI tables.
    //
    Status = UnInstallDptfAcpiTables ();
  } else {
    //
    // Update Global NVS data
    //
    Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &mGlobalNvsArea);
    ASSERT_EFI_ERROR (Status);
  }

  gBS->CloseEvent(Event);
}


INT16
GetRawTempValue (
  INT16 TempInCelsius
  )
{
  UINT32  Index        = 0;
  INT16   TempIn10K    = 0;
  INT16   RawTempValue = 0;
  INT16   LastEntry = sizeof (LinearApproxTable) / sizeof (LPAT_ENTRY) - 1;

  TempIn10K = (TempInCelsius*10+2731);

  //
  // Return maximum Raw value if temperature is above the maximum.
  //
  if (TempIn10K >= LinearApproxTable[LastEntry].TemperatureInKelvin)
    return LinearApproxTable[LastEntry].RawTemperature;

  //
  // Return minimum Raw value if temperature is below the minimum.
  //
  if (TempIn10K <= LinearApproxTable[0].TemperatureInKelvin)
    return LinearApproxTable[0].RawTemperature;

  //
  // Convert Degree Celsius to Raw data
  //
  for (Index = 0; Index < (sizeof (LinearApproxTable) / sizeof (LPAT_ENTRY) -1); Index++) {
    if (LinearApproxTable[Index].TemperatureInKelvin <= TempIn10K && LinearApproxTable[Index+1].TemperatureInKelvin >= TempIn10K) {
      //
      // Temperature lies between these entries
      //
      RawTempValue = LinearApproxTable[Index].RawTemperature - ((TempIn10K - LinearApproxTable[Index].TemperatureInKelvin) * (LinearApproxTable[Index].RawTemperature - LinearApproxTable[Index+1].RawTemperature)/(LinearApproxTable[Index+1].TemperatureInKelvin - LinearApproxTable[Index].TemperatureInKelvin));
      break;
    }
  }
  return RawTempValue;
}


/**
  This procedure initializes DPTF values.

  @retval   EFI_SUCCESS    Successfully initialized DPTF values.

**/
EFI_STATUS
InitializeDriverData (
  VOID
  )
{
  EFI_STATUS            Status;
  SYSTEM_CONFIGURATION  SystemConfiguration;
  UINTN                 VarSize;

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

  if (EFI_ERROR (Status) || (SystemConfiguration.DptfProcCriticalTemperature == 0 && SystemConfiguration.EnableDptf == 0)) {
    //
    // Setup options are invalid. Restore defaults
    //
    DEBUG ((EFI_D_WARN, "DPTF:Setup options are invalid. Restoring defaults.\n"));

    gDptfDrvData.bDptfEnabled                        = 0x1;
    gDptfDrvData.CpuParticipantCriticalTemperature   = PCT_DEFAULT;
    gDptfDrvData.CpuParticipantPassiveTemperature    = PPT_DEFAULT;
    gDptfDrvData.GenParticipant0CriticalTemperature  = GCT0_DEFAULT;
    gDptfDrvData.GenParticipant0PassiveTemperature   = GPT0_DEFAULT;
    gDptfDrvData.GenParticipant1CriticalTemperature  = GCT1_DEFAULT;
    gDptfDrvData.GenParticipant1PassiveTemperature   = GPT1_DEFAULT;
    gDptfDrvData.GenParticipant2CriticalTemperature  = GCT2_DEFAULT;
    gDptfDrvData.GenParticipant2PassiveTemperature   = GPT2_DEFAULT;
    gDptfDrvData.GenParticipant3CriticalTemperature  = GCT3_DEFAULT;
    gDptfDrvData.GenParticipant3PassiveTemperature   = GPT3_DEFAULT;
    gDptfDrvData.GenParticipant4CriticalTemperature  = GCT4_DEFAULT;
    gDptfDrvData.GenParticipant4PassiveTemperature   = GPT4_DEFAULT;
    //
    // Critical trip point is 5C above BIOS setup critical value. Reuse CriticalThresholdxForScu to set the critical trip point for PMIC sensors.
    //
    gDptfDrvData.CriticalThreshold0ForScu            = (gDptfDrvData.GenParticipant0CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant0CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold1ForScu            = (gDptfDrvData.GenParticipant1CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant1CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold2ForScu            = (gDptfDrvData.GenParticipant2CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant2CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold3ForScu            = (gDptfDrvData.GenParticipant3CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant3CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold4ForScu            = (gDptfDrvData.GenParticipant4CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant4CriticalTemperature + 5) : GetRawTempValue(60);
    //
    //CLPO Defaults
    //
    gDptfDrvData.LPOEnable                           = CLPO_DEFAULT_ENABLE;
    gDptfDrvData.LPOStartPState                      = CLPO_DEFAULT_START_PSTATE;
    gDptfDrvData.LPOStepSize                         = CLPO_DEFAULT_STEP_SIZE;
    gDptfDrvData.LPOPowerControlSetting              = CLPO_DEFAULT_PWR_CTRL_SETTING;
    gDptfDrvData.LPOPerformanceControlSetting        = CLPO_DEFAULT_PERF_CTRL_SETTING;
    gDptfDrvData.bDppmEnabled                        = DPPM_ENABLE_DEFAULT;
    } else {
    //
    // Setup Options are Valid.
    //
    gDptfDrvData.bDptfEnabled                        = (SystemConfiguration.EnableDptf == 0x1)? TRUE: FALSE;
    gDptfDrvData.CpuParticipantCriticalTemperature   = SystemConfiguration.DptfProcCriticalTemperature;
    gDptfDrvData.CpuParticipantPassiveTemperature    = SystemConfiguration.DptfProcPassiveTemperature;
    gDptfDrvData.GenParticipant0CriticalTemperature  = SystemConfiguration.GenericCriticalTemp0;
    gDptfDrvData.GenParticipant0PassiveTemperature   = SystemConfiguration.GenericPassiveTemp0;
    gDptfDrvData.GenParticipant1CriticalTemperature  = SystemConfiguration.GenericCriticalTemp1;
    gDptfDrvData.GenParticipant1PassiveTemperature   = SystemConfiguration.GenericPassiveTemp1;
    gDptfDrvData.GenParticipant2CriticalTemperature  = SystemConfiguration.GenericCriticalTemp2;
    gDptfDrvData.GenParticipant2PassiveTemperature   = SystemConfiguration.GenericPassiveTemp2;
    gDptfDrvData.GenParticipant3CriticalTemperature  = SystemConfiguration.GenericCriticalTemp3;
    gDptfDrvData.GenParticipant3PassiveTemperature   = SystemConfiguration.GenericPassiveTemp3;
    gDptfDrvData.GenParticipant4CriticalTemperature  = SystemConfiguration.GenericCriticalTemp4;
    gDptfDrvData.GenParticipant4PassiveTemperature   = SystemConfiguration.GenericPassiveTemp4;
    //
    // Critical trip point is 5C above BIOS setup critical value.Reuse CriticalThresholdxForScu to set the critical trip point for PMIC sensors.
    //
    gDptfDrvData.CriticalThreshold0ForScu            = (gDptfDrvData.GenParticipant0CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant0CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold1ForScu            = (gDptfDrvData.GenParticipant1CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant1CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold2ForScu            = (gDptfDrvData.GenParticipant2CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant2CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold3ForScu            = (gDptfDrvData.GenParticipant3CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant3CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.CriticalThreshold4ForScu            = (gDptfDrvData.GenParticipant4CriticalTemperature >= 60) ? GetRawTempValue(gDptfDrvData.GenParticipant4CriticalTemperature + 5) : GetRawTempValue(60);
    gDptfDrvData.LPOEnable                           = SystemConfiguration.LPOEnable;
    gDptfDrvData.LPOStartPState                      = SystemConfiguration.LPOStartPState;
    gDptfDrvData.LPOStepSize                         = SystemConfiguration.LPOStepSize;
    gDptfDrvData.LPOPowerControlSetting              = SystemConfiguration.LPOPowerControlSetting;
    gDptfDrvData.LPOPerformanceControlSetting        = SystemConfiguration.LPOPerformanceControlSetting;
    gDptfDrvData.bDppmEnabled                        = (SystemConfiguration.EnableDppm == 0x1)? TRUE: FALSE;
  }
  //
  // @todo: Assigning default values for these parameters till setup option is created.
  //
  gDptfDrvData.BidirectionalProchotEnable          = BI_DIRECTIONAL_PROCHOT_ENABLE;
  gDptfDrvData.ThermalMonitoring                   = THERMAL_MOINTER;
  gDptfDrvData.ThermalMonitoringHot                = TJTARGET_THRESHOLD;
  gDptfDrvData.ThermalMonitoringSystherm0Hot       = SYSTHERM0_DEFAULT;
  gDptfDrvData.ThermalMonitoringSystherm1Hot       = SYSTHERM1_DEFAULT;
  gDptfDrvData.ThermalMonitoringSystherm2Hot       = SYSTHERM2_DEFAULT;

  //
  //@todo:PMC to set PCI Cycling for PUnit OFF in APL.
  //
  gDptfDrvData.pSysTherm0BaseAddress = (THERISTOR_REG_DEF *) ((UINTN) A0_SYS_THERM0_BASE_ADDRESS);
  gDptfDrvData.pSysTherm1BaseAddress = (THERISTOR_REG_DEF *) ((UINTN) A0_SYS_THERM1_BASE_ADDRESS);
  gDptfDrvData.pSysTherm2BaseAddress = (THERISTOR_REG_DEF *) ((UINTN) A0_SYS_THERM2_BASE_ADDRESS);
  gDptfDrvData.pSysTherm3BaseAddress = (THERISTOR_REG_DEF *) ((UINTN) A0_SYS_THERM3_BASE_ADDRESS);

  DEBUG ((EFI_D_INFO, "DPTF:Dumping DPTF settings in DPTF driver.\n"));
  DEBUG ((EFI_D_INFO, "DPTF:DPTFEnabled = %d\n", gDptfDrvData.bDptfEnabled));
  DEBUG ((EFI_D_INFO, "DPTF:CpuParticipantCriticalTemperature = %d\n", gDptfDrvData.CpuParticipantCriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:CpuParticipantPassiveTemperature = %d\n", gDptfDrvData.CpuParticipantPassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant0CriticalTemperature = %d\n", gDptfDrvData.GenParticipant0CriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant0PassiveTemperature = %d\n", gDptfDrvData.GenParticipant0PassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant1CriticalTemperature = %d\n", gDptfDrvData.GenParticipant1CriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant1PassiveTemperature = %d\n", gDptfDrvData.GenParticipant1PassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant2CriticalTemperature = %d\n", gDptfDrvData.GenParticipant2CriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant2PassiveTemperature = %d\n", gDptfDrvData.GenParticipant2PassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant3CriticalTemperature = %d\n", gDptfDrvData.GenParticipant3CriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant3PassiveTemperature = %d\n", gDptfDrvData.GenParticipant3PassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant4CriticalTemperature = %d\n", gDptfDrvData.GenParticipant4CriticalTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:GenParticipant4PassiveTemperature = %d\n", gDptfDrvData.GenParticipant4PassiveTemperature));
  DEBUG ((EFI_D_INFO, "DPTF:LPOEnable = %d\n", gDptfDrvData.LPOEnable));
  DEBUG ((EFI_D_INFO, "DPTF:LPOStartPState = %d\n", gDptfDrvData.LPOStartPState));
  DEBUG ((EFI_D_INFO, "DPTF:LPOStepSize = %d\n", gDptfDrvData.LPOStepSize));
  DEBUG ((EFI_D_INFO, "DPTF:LPOPowerControlSetting = %d\n", gDptfDrvData.LPOPowerControlSetting));
  DEBUG ((EFI_D_INFO, "DPTF:LPOPerformanceControlSetting = %d\n", gDptfDrvData.LPOPerformanceControlSetting));
  DEBUG ((EFI_D_INFO, "DPTF:bDppmEnabled = %d\n", gDptfDrvData.bDppmEnabled));
  DEBUG ((EFI_D_INFO, "DPTF:BidirectionalProchotEnable = %d\n", gDptfDrvData.BidirectionalProchotEnable));
  DEBUG ((EFI_D_INFO, "DPTF:ThermalMonitoring = %d\n", gDptfDrvData.ThermalMonitoring));
  DEBUG ((EFI_D_INFO, "DPTF:ThermalMonitoringHot = %d\n", gDptfDrvData.ThermalMonitoringHot));
  DEBUG ((EFI_D_INFO, "DPTF:ThermalMonitoringSystherm0Hot = %d\n", gDptfDrvData.ThermalMonitoringSystherm0Hot));
  DEBUG ((EFI_D_INFO, "DPTF:ThermalMonitoringSystherm1Hot = %d\n", gDptfDrvData.ThermalMonitoringSystherm1Hot));
  DEBUG ((EFI_D_INFO, "DPTF:ThermalMonitoringSystherm2Hot = %d\n", gDptfDrvData.ThermalMonitoringSystherm2Hot));

  return EFI_SUCCESS;
}


/**
  Entry Point for this driver.This procedure does all the DPTF initialization and loads the ACPI tables.

  @param[in] ImageHandle       The firmware allocated handle for the EFI image.
  @param[in] SystemTable       A pointer to the EFI System Table.

  @retval    EFI_SUCCESS       The entry point is executed successfully.
  @retval    other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DptfDriverEntry (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  Status = InitializeDriverData ();
  ASSERT_EFI_ERROR (Status);

  //
  // SOC thermal initialization
  //
  SocThermInit ();

  if (!EFI_ERROR (Status)) {
    //
    // Register for OnReadyToBoot event
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               OnReadyToBoot,
               NULL,
               &Event
               );
  }

  return EFI_SUCCESS;
}

/**
  CPU thermal device used by DPTF is PCI DID 0x5A8C.
  It is default enabled in silicon at power-on.
  DPTF DXE driver must disable this during POST or S3 resume when DPTF is disabled.

**/
VOID
SocThermInit (
  VOID
  )
{
  UINTN       McD0BaseAddress;
  UINT32      Data32;
  EFI_STATUS  Status;
  BOOLEAN     bDptfEnabled = TRUE;

  //
  // Check if DPTF is enabled in DPTF policy
  //
  Status = DPTFEnabledInBIOS (&bDptfEnabled);
  DEBUG ((EFI_D_INFO, "DPTF: SocThermInit = 0x%x \n",bDptfEnabled));

  if (!bDptfEnabled) {
    DEBUG ((EFI_D_INFO, "DPTF: SocThermInit = 0x%x \n",GetBxtSeries ()));
    McD0BaseAddress = MmPciAddress (0,SA_MC_BUS,SA_MC_DEV,SA_MC_FUN,0);
    DEBUG ((EFI_D_INFO, "DPTF: McD0BaseAddress = 0x%x \n",McD0BaseAddress));
    DEBUG ((EFI_D_INFO, "DPTF: R_SA_MC_CAPID0_A = 0x%x \n",MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A)));
    if (MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT15 ) {
      DEBUG ((EFI_D_INFO, "DPTF: Write PSF Reg R_PCH_PCR_PSF1_T0_SHDW_PCIEN_CUNIT_RS0_D0_F1_OFFSET5 "));
      PchPcrAndThenOr32 (0xA9, 0x051C,
      (UINT32) ~0, BIT8);
    }

    //
    // PCI CFG space function disable register.
    //
    MmioAnd32 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_DEVEN, (UINT32)~BIT1);

    PchPcrAndThenOr32 (0xA9, 0x051C, (UINT32) ~ 0, BIT8);

    //
    // Store the PCI CFG space function disable register for the S3 resume path
    //
    Data32 = MmioRead32 (MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN) + R_SA_DEVEN);
    DEBUG ((EFI_D_INFO, "DPTF: PCI CFG space DPTF function disable register = 0x%x \n",Data32));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (MmPciBase (SA_MC_BUS,SA_MC_DEV,SA_MC_FUN) + R_SA_DEVEN),
      1,
      &Data32
      );

    DEBUG ((EFI_D_INFO, "DPTF: PSF fabric private configuration DPTF function disable register Address = 0x%x \n",SC_PCR_BASE_ADDRESS | ((UINT8) (0xA9) << 16) | (UINT16) (0x051C)));
    Data32 = MmioRead32 (SC_PCR_BASE_ADDRESS | ((UINT8)(0xA9) << 16) | (UINT16) (0x051C));
    DEBUG ((EFI_D_INFO, "DPTF: PSF fabric private configuration DPTF function disable register Data = 0x%x \n",Data32));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (SC_PCR_BASE_ADDRESS | ((UINT8) (0xA9) << 16) | (UINT16) (0x051C)),
      1,
      &Data32
      );
  }

}

