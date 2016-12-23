/** @file
  Digital Thermal Sensor (DTS) driver.
  This SMM driver configures and supports the Digital Thermal Sensor features for the platform.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtSmm.h"
#include "PowerMgmtDts.h"

//
// Global variables
//
GLOBAL_REMOVE_IF_UNREFERENCED CPU_GLOBAL_NVS_AREA *mCpuGlobalNvsAreaPtr;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8               mDtsValue;

GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN             mDtsEnabled;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8               mDtsTjMax;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16              mAcpiBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN             mUpdateDtsInEverySmi;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8               mNoOfThresholdRanges;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8               (*mDtsThresholdTable)[3];
GLOBAL_REMOVE_IF_UNREFERENCED UINT8               mIsPackageTempMsrAvailable;
///
/// The table is updated for the current CPU.
///
GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mDigitalThermalSensorThresholdTable[DTS_NUMBER_THRESHOLD_RANGES][3] = {
  ///
  /// TJ_MAX = 110                ///< Current Temp.  Low Temp. High Temp.
  ///
  {TJ_MAX-80,100,75},     ///<    <= 30            10       35
  {TJ_MAX-70,85,65},      ///< 30  ~ 39            25       45
  {TJ_MAX-60,75,55},      ///< 40  ~ 49            35       55
  {TJ_MAX-50,65,45},      ///< 50  ~ 59            45       65
  {TJ_MAX-40,55,35},      ///< 60  ~ 69            55       75
  {TJ_MAX-30,45,25},      ///< 70  ~ 79            65       85
  {TJ_MAX-20,35,15},      ///< 80  ~ 89            75       95
  {TJ_MAX-10,25,05},      ///< 90  ~ 99            85       105
  {TJ_MAX-00,15,00}       ///< 100 ~ 109           95       110
};

//
// Function implementations
//
/**
  Read the temperature data per core/thread.
  This function must be AP safe.

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval    EFI_SUCCESS   Digital Thermal Sensor temperature has updated successfully.

**/
VOID
EFIAPI
DigitalThermalSensorUpdateTemperature (
  IN VOID *Buffer
  )
{
  MSR_REGISTER        MsrData;
  UINT8               Temperature;
  UINT8               DefApicId;
  EFI_CPUID_REGISTER  CpuidRegisters;
  UINT8               *TempPointer;

  AsmCpuid (
    CPUID_VERSION_INFO,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );

  //
  // Default APIC ID = CPUID Function 01, EBX[31:24]
  //
  DefApicId = (UINT8) RShiftU64 (CpuidRegisters.RegEbx, 24);

  //
  // Read the temperature
  //
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

  //
  // Find the DTS temperature.
  //
  Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);

  //
  // We only update the temperature if it is above the current temperature.
  //
  TempPointer = Buffer;
  if (Temperature > *((UINT8 *) (TempPointer + DefApicId))) {
    *((UINT8 *) (TempPointer + DefApicId)) = Temperature;
  }

  return;
}


/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal Out Of Spec interrupt

  @param[in]      SmmImageHandle             Image handle returned by the SMM driver.
  @param[in]      ContextData                Pointer to data buffer to be used for handler.
  @param[in, out] CommunicationBuffer        Pointer to the buffer that contains the communication Message
  @param[in, out] SourceSize                 Size of the memory image to be used for handler.

  @retval         EFI_SUCCESS                Callback Function Executed

**/
EFI_STATUS
EFIAPI
DtsOutOfSpecSmiCallback (
  IN EFI_HANDLE  SmmImageHandle,
  IN CONST VOID  *ContextData,         OPTIONAL
  IN OUT VOID    *CommunicationBuffer, OPTIONAL
  IN OUT UINTN   *SourceSize           OPTIONAL
  )
{
  DTS_EVENT_TYPE     EventType;

  //
  // If not enabled; return.  (The DTS will be disabled upon S3 entry
  // and will remain disabled until after re-initialized upon wake.)
  //
  if (!mDtsEnabled) {
    return EFI_SUCCESS;
  }

  EventType = DtsEventNone;

  if (mIsPackageTempMsrAvailable) {
    //
    // Get the Package DTS Event Type
    //
    DigitalThermalSensorEventCheckPackageMsr (&EventType);
  } else {
    //
    // Get the DTS Event Type
    //
    DigitalThermalSensorEventCheck (&EventType);
  }

  //
  // Check if this a DTS Out Of Spec SMI event
  //
  if (EventType == DtsEventOutOfSpec) {

    //
    // Return Critical temperature value to _TMP and generate GPE event for critical shutdown.
    //
    mCpuGlobalNvsAreaPtr->EnableDigitalThermalSensor = DTS_OUT_OF_SPEC_OCCURRED;

    //
    // Generate SCI to shut down the system
    //
    DigitalThermalSensorSetSwGpeSts ();
  }

  return EFI_SUCCESS;
}


/**
  Call from SMI handler to handle Package thermal temperature Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

**/
VOID
PackageThermalDTS (
  VOID
  )
{
  DTS_EVENT_TYPE      PkgEventType;

  PkgEventType = DtsEventNone;

  //
  // Check is this a Platform SMI event or the flag of update DTS temperature and threshold value in every SMI
  //
  if (DigitalThermalSensorEventCheckPackageMsr (&PkgEventType) || mUpdateDtsInEverySmi) {
    //
    // Disable Local APIC SMI before programming the threshold
    //
    RunOnAllLogicalProcessors (DigitalThermalSensorDisableSmi, NULL);

    do {
      //
      // Handle Package events
      //
      if ((PkgEventType == DtsEventOutOfSpec) && (mCpuGlobalNvsAreaPtr->DtsAcpiEnable == 0)) {

      }
      //
      // Set the thermal trip toints as needed.
      //
      mCpuGlobalNvsAreaPtr->PackageDTSTemperature = 0;

      //
      // Set the Package thermal sensor thresholds
      //
      PackageDigitalThermalSensorSetThreshold (&mCpuGlobalNvsAreaPtr->PackageDTSTemperature);

      //
      // Set SWGPE Status to generate an SCI if we had any events
      //
      if ((PkgEventType != DtsEventNone) || mUpdateDtsInEverySmi) {
        DigitalThermalSensorSetSwGpeSts ();
      }

    } while (DigitalThermalSensorEventCheckPackageMsr (&PkgEventType));

    //
    // Enable Local APIC SMI on all logical processors
    //
    RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
  }
}


/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @param[in]      SmmImageHandle             Image handle returned by the SMM driver.
  @param[in]      ContextData                Pointer to data buffer to be used for handler.
  @param[in, out] CommunicationBuffer        Pointer to the buffer that contains the communication Message
  @param[in, out] SourceSize                 Size of the memory image to be used for handler.

  @retval         EFI_SUCCESS                Callback Function Executed

**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE  SmmImageHandle,
  IN CONST VOID  *ContextData,         OPTIONAL
  IN OUT VOID    *CommunicationBuffer, OPTIONAL
  IN OUT UINTN   *SourceSize           OPTIONAL
  )
{
  UINTN           Index;
  DTS_EVENT_TYPE  EventType;

  //
  // If not enabled; return.  (The DTS will be disabled upon S3 entry
  // and will remain disabled until after re-initialized upon wake.)
  //
  if (!mDtsEnabled) {
    return EFI_SUCCESS;
  }
  //
  // Get the Package thermal temperature
  //
  if (mIsPackageTempMsrAvailable) {
    RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
    PackageThermalDTS ();
  } else {
    //
    // We enable the Thermal interrupt on the AP's prior to the event check
    // for the case where the AP has gone through the INIT-SIPI-SIPI sequence
    // and does not have the interrupt enabled.  (This allows the AP thermal
    // interrupt to be re-enabled due to chipset-based SMIs without waiting
    // to receive a DTS event on the BSP.)
    //
    for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
      RunOnSpecificLogicalProcessor (DigitalThermalSensorEnableSmi, Index, NULL);
    }
    //
    // Check is this a DTS SMI event or the flag of update DTS temperature and threshold value in every SMI
    //
    if (DigitalThermalSensorEventCheck (&EventType) || mUpdateDtsInEverySmi) {
      //
      // Disable Local APIC SMI before programming the threshold
      //
      RunOnAllLogicalProcessors (DigitalThermalSensorDisableSmi, NULL);
     do {
        //
        // Handle BSP events
        //

        //
        // Set the thermal trip toints as needed.
        // Note:  We only save the highest temperature of each die in
        // the NVS area when more than two logical processors are
        // present as only the highest DTS reading is actually used by
        // the current ASL solution.
        //
        mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
        mCpuGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;

        //
        // Set the BSP thermal sensor thresholds
        //
        DigitalThermalSensorSetThreshold (&mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);

        //
        // Set the AP thermal sensor thresholds and update temperatures
        //
        for (Index = 1; Index < gSmst->NumberOfCpus / 2; Index++) {
          RunOnSpecificLogicalProcessor (
            DigitalThermalSensorSetThreshold,
            Index,
            &mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature
            );
        }

        for (Index = gSmst->NumberOfCpus / 2; Index < gSmst->NumberOfCpus; Index++) {
          RunOnSpecificLogicalProcessor (
            DigitalThermalSensorSetThreshold,
            Index,
            &mCpuGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature
            );
        }
        //
        // Set SWGPE Status to generate an SCI if we had any events
        //
        if ((EventType != DtsEventNone) || mUpdateDtsInEverySmi) {
          DigitalThermalSensorSetSwGpeSts ();
        }

      } while (DigitalThermalSensorEventCheck (&EventType));
      //
      // Enable Local APIC SMI on all logical processors
      //
      RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
    }
  }

  return EFI_SUCCESS;
}


/**
  This function executes DTS procedures for preparing to enter S3.

  @param[in]      Handle               Handle of the callback
  @param[in]      Context              The dispatch context
  @param[in, out] CommBuffer           Pointer to the buffer that contains the communication Message
  @param[in, out] CommBufferSize       The size of the CommBuffer.

  @retval         EFI_SUCCESS          DTS disabled

**/
EFI_STATUS
EFIAPI
DtsS3EntryCallBack (
  IN  EFI_HANDLE                DispatchHandle,
  IN CONST VOID                 *Context         OPTIONAL,
  IN OUT VOID                   *CommBuffer      OPTIONAL,
  IN OUT UINTN                  *CommBufferSize  OPTIONAL
  )
{

  //
  // Clear the Digital Thermal Sensor flag in ACPI NVS.
  //
  mCpuGlobalNvsAreaPtr->EnableDigitalThermalSensor = CPU_FEATURE_DISABLE;

  //
  // Clear the enable flag.
  //
  mDtsEnabled = FALSE;

  return EFI_SUCCESS;
}


/**
  Performs initialization of the threshold table.

  @retval  EFI_SUCCESS      Threshold tables initialized successfully.

**/
EFI_STATUS
InitThresholdTable (
  VOID
  )
{
  UINTN      i;
  UINT8      Delta;

  //
  // If the table must be updated, shift the thresholds by the difference between
  // TJ_MAX=110 and DtsTjMax.
  //
  if (mDtsTjMax != TJ_MAX) {
    Delta = TJ_MAX - mDtsTjMax;

    for (i = 0; i < mNoOfThresholdRanges; i++) {
      if (mDtsThresholdTable[i][1] <= mDtsTjMax) {
        mDtsThresholdTable[i][0] = mDtsThresholdTable[i][0] - Delta;
      } else {
        mDtsThresholdTable[i][0] = 0;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval  EFI_SUCCESS        Init Digital Thermal Sensor successfully

**/
EFI_STATUS
InitDigitalThermalSensor (
  VOID
  )
{
  UINTN      Index;

  if (mDtsValue != DTS_OUT_OF_SPEC_ONLY) {
    //
    // Initialize the DTS threshold table.
    //
    InitThresholdTable ();

    //
    // Set the thermal trip points on all logical processors.
    // Note:  We only save the highest temperature of each die in the NVS area when
    // more than two logical processors are present as only the highest DTS reading
    // is actually used by the current ASL solution.
    //
    mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
    mCpuGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
    mCpuGlobalNvsAreaPtr->PackageDTSTemperature              = 0;
    if (mIsPackageTempMsrAvailable) {
      PackageDigitalThermalSensorSetThreshold (&mCpuGlobalNvsAreaPtr->PackageDTSTemperature);
    } else {
      DigitalThermalSensorSetThreshold (&mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);
      for (Index = 1; Index < gSmst->NumberOfCpus / 2; Index++) {
        RunOnSpecificLogicalProcessor (
          DigitalThermalSensorSetThreshold,
          Index,
          &mCpuGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature
          );
      }
      for (Index = gSmst->NumberOfCpus / 2; Index < gSmst->NumberOfCpus; Index++) {
        RunOnSpecificLogicalProcessor (
          DigitalThermalSensorSetThreshold,
          Index,
          &mCpuGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature
          );
      }
    }
    mCpuGlobalNvsAreaPtr->EnableDigitalThermalSensor = CPU_FEATURE_ENABLE;
  } else {
    //
    // Enable Out Of Spec Interrupt
    //
    if (mIsPackageTempMsrAvailable) {
      PackageDigitalThermalSensorSetOutOfSpecInterrupt (NULL);
    } else {
      RunOnAllLogicalProcessors (DigitalThermalSensorSetOutOfSpecInterrupt, NULL);
    }

    mCpuGlobalNvsAreaPtr->EnableDigitalThermalSensor = DTS_OUT_OF_SPEC_ONLY;
  }

  //
  // Enable the Local APIC SMI on all logical processors
  //
  RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);

  //
  // Set Digital Thermal Sensor flag in ACPI NVS
  //
  mUpdateDtsInEverySmi  = UPDATE_DTS_EVERY_SMI;
  mDtsEnabled           = TRUE;

  return EFI_SUCCESS;
}

/**
  Initializes the Thermal Sensor Control MSR

  This function must be AP safe.

  @param[in] Buffer        Unused.

  @retval    EFI_SUCCESS   The function completed successfully.

**/
VOID
EFIAPI
DigitalThermalSensorEnable (
  IN VOID *Buffer
  )
{
  MSR_REGISTER      MsrData;

  //
  // First, clear our log bits
  //
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
  if (mDtsValue != DTS_OUT_OF_SPEC_ONLY) {
    MsrData.Qword &= (UINT64) ~THERM_STATUS_LOG_MASK;
  } else {
    MsrData.Qword &= (UINT64) ~B_OUT_OF_SPEC_STATUS_LOG;
  }

  AsmWriteMsr64 (MSR_IA32_THERM_STATUS, MsrData.Qword);

  //
  // Second, configure the thermal sensor control
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_MISC_PWR_MGMT);

  //
  // Only lock interrupts if in CMP mode
  //
  if (gSmst->NumberOfCpus > 1) {
    MsrData.Qword |= (UINT64) B_LOCK_THERMAL_INT;
  }

  AsmWriteMsr64 (EFI_MSR_MISC_PWR_MGMT, MsrData.Qword);

  return;
}


/**
  Initializes the Package Thermal Sensor Control MSR

  @param[in] Buffer        Unused.

  @retval    EFI_SUCCESS   The function completed successfully.

**/
EFI_STATUS
PackageDigitalThermalSensorEnable (
  IN VOID *Buffer
  )
{
  MSR_REGISTER      MsrData;

  //
  // First, clear our log bits
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);
  if (mDtsValue != DTS_OUT_OF_SPEC_ONLY) {
    MsrData.Qword &= ~THERM_STATUS_LOG_MASK;
  } else {
    MsrData.Qword &= ~B_OUT_OF_SPEC_STATUS_LOG;
  }

  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS, MsrData.Qword);

  //
  // Second, configure the thermal sensor control
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_MISC_PWR_MGMT);

  //
  // Only lock interrupts if in CMP mode
  //
  if (gSmst->NumberOfCpus > 1) {
    MsrData.Qword |= B_LOCK_THERMAL_INT;
  }

  AsmWriteMsr64 (EFI_MSR_MISC_PWR_MGMT, MsrData.Qword);

  return EFI_SUCCESS;
}


/**
  Generates a _GPE._L02 SCI to an ACPI OS.

**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  )
{
  EFI_STATUS      Status;
  UINT8           Pm1Cntl;
  UINT8           GpeCntl;

  //
  // Check SCI enable
  //
  Status = gSmst->SmmIo.Io.Read (
                             &gSmst->SmmIo,
                             SMM_IO_UINT8,
                             mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                             1,
                             &Pm1Cntl
                             );

  if ((Pm1Cntl & B_PCH_ACPI_PM1_CNT_SCI_EN) != 0) {
    //
    // Set SWGPE Status
    //
    Status = gSmst->SmmIo.Io.Read (
                               &gSmst->SmmIo,
                               SMM_IO_UINT8,
                               mAcpiBaseAddr + R_ACPI_GPE_CNTL,
                               1,
                               &GpeCntl
                               );
    GpeCntl |= B_SWGPE_CTRL;
    Status = gSmst->SmmIo.Io.Write (
                               &gSmst->SmmIo,
                               SMM_IO_UINT8,
                               mAcpiBaseAddr + R_ACPI_GPE_CNTL,
                               1,
                               &GpeCntl
                               );
  }
}


/**
  Checks for a Core Thermal Event on any processor

  @param[in]  EventType      DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval     TRUE           Means this is a DTS Thermal event
  @retval     FALSE          Means this is not a DTS Thermal event.

**/
BOOLEAN
DigitalThermalSensorEventCheck (
  IN DTS_EVENT_TYPE *EventType
  )
{
  //
  // Clear event status
  //
  *EventType = DtsEventNone;

  RunOnAllLogicalProcessors (DigitalThermalSensorEventCheckMsr, EventType);

  //
  // Return TRUE if any logical processor reported an event.
  //
  if (*EventType != DtsEventNone) {
    return TRUE;
  }

  return FALSE;
}


/**
  Checks for a Package Thermal Event by reading MSR.

  @param[in]  PkgEventType     DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval     TRUE             Means this is a Package DTS Thermal event
  @retval     FALSE            Means this is not a Package DTS Thermal event.

**/
BOOLEAN
DigitalThermalSensorEventCheckPackageMsr (
  IN DTS_EVENT_TYPE *PkgEventType
  )
{
  MSR_REGISTER      MsrData;

  //
  // Clear event status
  //
  *PkgEventType = DtsEventNone;

  //
  // If Processor has already been flagged as Out-Of-Spec,
  // just return.
  //
  if (*PkgEventType != DtsEventOutOfSpec) {
    //
    // Read thermal status
    //
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);

    //
    // Check for Out-Of-Spec status.
    //
    if (MsrData.Qword & B_OUT_OF_SPEC_STATUS_LOG) {
      *PkgEventType = DtsEventOutOfSpec;

      //
      // Check thresholds.
      //
    } else if ((mDtsValue != DTS_OUT_OF_SPEC_ONLY) &&
               (MsrData.Qword & (B_THERMAL_THRESHOLD_1_STATUS_LOG | B_THERMAL_THRESHOLD_2_STATUS_LOG))
               ) {
      *PkgEventType = DtsEventThreshold;
    }
  }

  //
  // Return TRUE if processor reported an event.
  //
  if (*PkgEventType != DtsEventNone) {
    return TRUE;
  }

  return FALSE;

}


/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in]  Buffer    Pointer to DTS_EVENT_TYPE

**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN VOID *Buffer
  )
{
  MSR_REGISTER        MsrData;
  DTS_EVENT_TYPE      *EventType;

  //
  // Cast to enhance readability.
  //
  EventType = (DTS_EVENT_TYPE *) Buffer;

  //
  // If any processor has already been flagged as Out-Of-Spec,
  // just return.
  //
  if (*EventType != DtsEventOutOfSpec) {
    //
    // Read thermal status
    //
    MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

    //
    // Check for Out-Of-Spec status.
    //
    if (MsrData.Qword & B_OUT_OF_SPEC_STATUS_LOG) {
      *EventType = DtsEventOutOfSpec;

      //
      // Check thresholds.
      //
    } else if ((mDtsValue != DTS_OUT_OF_SPEC_ONLY) &&
               (MsrData.Qword & (B_THERMAL_THRESHOLD_1_STATUS_LOG | B_THERMAL_THRESHOLD_2_STATUS_LOG))
               ) {
      *EventType = DtsEventThreshold;
    }
  }
}


/**
  Set the Out Of Spec Interrupt in all cores
  This function must be AP safe.

  @param[in]  Buffer        Unused

**/
VOID
EFIAPI
DigitalThermalSensorSetOutOfSpecInterrupt (
  IN VOID *Buffer
  )
{
  MSR_REGISTER      MsrData;

  //
  // Enable Out Of Spec interrupt
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_INTERRUPT);
  MsrData.Qword |= (UINT64) OVERHEAT_INTERRUPT_ENABLE;
  AsmWriteMsr64 (EFI_MSR_IA32_THERM_INTERRUPT, MsrData.Qword);

  return;
}

/**
  Set the Out Of Spec Interrupt on the package

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Out Of Spec Interrupt programmed successfully

**/
EFI_STATUS
PackageDigitalThermalSensorSetOutOfSpecInterrupt (
  IN VOID *Buffer
  )
{
  MSR_REGISTER      MsrData;

  //
  // Enable Out Of Spec interrupt
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT);
  MsrData.Qword |= OVERHEAT_INTERRUPT_ENABLE;
  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT, MsrData.Qword);

  return EFI_SUCCESS;
}


/**
  Read the temperature and reconfigure the thresholds.
  This function must be AP safe.

  @param[in]  Buffer        Pointer to UINT8 to update with the current temperature

**/
VOID
EFIAPI
DigitalThermalSensorSetThreshold (
  IN VOID *Buffer
  )
{
  UINT8         ThresholdEntry;
  MSR_REGISTER  MsrData;
  UINT8         Temperature;

  //
  // Read the temperature
  //
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

  //
  // If Out-Of-Spec, return the critical shutdown temperature.
  //
  if (MsrData.Qword & B_OUT_OF_SPEC_STATUS) {
    *((UINT8 *) Buffer) = DTS_CRITICAL_TEMPERATURE;
    return;
  } else if (MsrData.Qword & B_READING_VALID) {
    //
    // Find the DTS temperature.
    //
    Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
    //
    // We only update the temperature if it is above the current temperature.
    //
    if (Temperature > *((UINT8 *) Buffer)) {
      *((UINT8 *) Buffer) = Temperature;
    }
    //
    // Compare the current temperature to the Digital Thermal Sensor Threshold Table until
    // a matching Value is found.
    //
    ThresholdEntry = 0;
    while ((Temperature > mDtsThresholdTable[ThresholdEntry][0]) && (ThresholdEntry < (mNoOfThresholdRanges - 1))) {
      ThresholdEntry++;
    }
    //
    // Update the threshold values
    //
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_INTERRUPT);
    //
    // Low temp is threshold #2
    //
    MsrData.Bytes.ThirdByte = mDtsThresholdTable[ThresholdEntry][1];
    //
    // High temp is threshold #1
    //
    MsrData.Bytes.SecondByte = mDtsThresholdTable[ThresholdEntry][2];
    DEBUG ((DEBUG_INFO, "CPU DTS Threshold #1 - %x\n", MsrData.Bytes.SecondByte));
    DEBUG ((DEBUG_INFO, "CPU DTS Threshold #2 - %x\n", MsrData.Bytes.ThirdByte));
    //
    // Enable interrupts
    //
    MsrData.Qword |= (UINT64) TH1_ENABLE;
    MsrData.Qword |= (UINT64) TH2_ENABLE;

    //
    // If the high temp is at TjMax (offset == 0)
    // We disable the int to avoid generating a large number of SMI because of TM1/TM2
    // causing many threshold crossings
    //
    if (MsrData.Bytes.SecondByte == 0x80) {
      MsrData.Qword &= (UINT64) ~TH1_ENABLE;
    }

    AsmWriteMsr64 (EFI_MSR_IA32_THERM_INTERRUPT, MsrData.Qword);
  }

  //
  //  Clear the threshold log bits
  //
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
  MsrData.Qword &= (UINT64) ~THERM_STATUS_THRESHOLD_LOG_MASK;
  AsmWriteMsr64 (MSR_IA32_THERM_STATUS, MsrData.Qword);

  return;
}

/**
  Read the temperature and reconfigure the thresholds on the package

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval    EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully

**/
EFI_STATUS
PackageDigitalThermalSensorSetThreshold (
  IN VOID *Buffer
  )
{
  UINT8         ThresholdEntry;
  MSR_REGISTER  MsrData;
  UINT8         Temperature;

  //
  // Read the temperature
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);

  //
  // If Out-Of-Spec, return the critical shutdown temperature.
  //
  if (MsrData.Qword & B_OUT_OF_SPEC_STATUS) {
    *((UINT8 *) Buffer) = DTS_CRITICAL_TEMPERATURE;
    return EFI_SUCCESS;
  } else if (MsrData.Qword & B_READING_VALID) {
    //
    // Find the DTS temperature.
    //
    Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
    //
    // We only update the temperature if it is above the current temperature.
    //
    if (Temperature > *((UINT8 *) Buffer)) {
      *((UINT8 *) Buffer) = Temperature;
    }
    //
    // Compare the current temperature to the Digital Thermal Sensor Threshold Table until
    // a matching Value is found.
    //
    ThresholdEntry = 0;
    while ((Temperature > mDtsThresholdTable[ThresholdEntry][0]) && (ThresholdEntry < (mNoOfThresholdRanges - 1))) {
      ThresholdEntry++;
    }
    //
    // Update the threshold values
    //
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT);
    //
    // Low temp is threshold #2
    //
    MsrData.Bytes.ThirdByte = mDtsThresholdTable[ThresholdEntry][1];
    //
    // High temp is threshold #1
    //
    MsrData.Bytes.SecondByte = mDtsThresholdTable[ThresholdEntry][2];

    //
    // Enable interrupts
    //
    MsrData.Qword |= TH1_ENABLE;
    MsrData.Qword |= TH2_ENABLE;

    //
    // If the high temp is at TjMax (offset == 0)
    // We disable the int to avoid generating a large number of SMI because of TM1/TM2
    // causing many threshold crossings
    //
    if (MsrData.Bytes.SecondByte == 0x80) {
      MsrData.Qword &= ~TH1_ENABLE;
    }

    AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT, MsrData.Qword);
  }

  //
  //  Clear the threshold log bits
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);
  MsrData.Qword &= ~THERM_STATUS_THRESHOLD_LOG_MASK;
  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS, MsrData.Qword);

  return EFI_SUCCESS;
}


/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Enable Local APIC to generate a SMI successfully

**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  IN VOID *Buffer
  )
{
  UINT32   ApicThermalValue;
  BOOLEAN  x2ApicEnabled;

  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_XAPIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);

  //
  // Configure the Local APIC to generate an SMI on Thermal events.  First,
  // Clear BIT16, BIT10-BIT8, BIT7-BIT0.  Then, set BIT9 (delivery mode).
  // Don't enable the interrupt if it's already enabled
  //
  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }

  if ((ApicThermalValue & (B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) != V_MODE_SMI) {
    ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | V_MODE_SMI;
    if (x2ApicEnabled) {
      AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
    } else {
      *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
    }
  }

  return;
}


/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Disable Local APIC to generate a SMI successfully

**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  IN VOID *Buffer
  )
{
  UINT32   ApicThermalValue;
  BOOLEAN  x2ApicEnabled;

  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_XAPIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);

  //
  // Disable Local APIC thermal entry
  //
  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }
  //
  // Following descriptions were from SSE BIOS
  // We set the interrupt mode at the same time as the interrupt is disabled to
  // avoid the "Received Illegal Vector" being set in the Error Status Register.
  //  and eax, 0FFFEF800h
  //  or  eax, 000010200h     ; Clear Mask, Set Delivery
  //
  ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | (B_INTERRUPT_MASK | V_MODE_SMI);
  if (x2ApicEnabled) {
    AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
  } else {
    *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
  }

  return;
}


/**
  Runs the specified procedure on one specific logical processors, passing in the
  parameter buffer to the procedure.

  @param[in, out] Procedure     The function to be run.
  @param[in]      Index         Indicate which logical processor should execute this procedure
  @param[in, out] Buffer        Pointer to a parameter buffer.

  @retval         EFI_SUCCESS   Function executed successfully.

**/
STATIC
EFI_STATUS
RunOnSpecificLogicalProcessor (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN UINTN                Index,
  IN OUT VOID             *Buffer
  )
{
  EFI_STATUS  Status;
  UINT8       RetryIteration;

  //
  // Run the procedure on one specific logical processor.
  //
  Status = EFI_NOT_READY;
  for (RetryIteration = 0; (RetryIteration < DTS_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS); RetryIteration++) {
    Status = gSmst->SmmStartupThisAp (Procedure, Index, Buffer);
    if (Status != EFI_SUCCESS) {
      //
      // SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
      //
      MicroSecondDelay (DTS_WAIT_PERIOD);
    }
  }

  return EFI_SUCCESS;
}


/**
  Digital Thermal Sensor (DTS) SMM driver function.

  @param[in] ImageHandle            Image handle for this driver image

  @retval    EFI_SUCCESS            Driver initialization completed successfully
  @retval    EFI_OUT_OF_RESOURCES   Error when allocating required memory buffer.

**/
EFI_STATUS
EFIAPI
InstallDigitalThermalSensor (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                Handle;
  MSR_REGISTER                              MsrData;
  CPU_GLOBAL_NVS_AREA_PROTOCOL              *CpuGlobalNvsAreaProtocol;
  EFI_HANDLE                                SxDispatchHandle;
  EFI_CPUID_REGISTER                        Cpuid06;
  EFI_SMM_SX_REGISTER_CONTEXT               SxDispatchContext;
  EFI_SMM_SX_DISPATCH2_PROTOCOL             *SxDispatchProtocol;
  VOID                                      *Hob;
  CPU_INIT_DATA_HOB                         *mCpuInitDataHob;
  CPU_CONFIG                                *mCpuConfig;

  Handle  = NULL;

  //
  // Get CPU Init Data Hob
  //
  Hob = GetFirstGuidHob (&gCpuInitDataHobGuid);
  if (Hob == NULL) {
    DEBUG ((DEBUG_ERROR, "CPU Data HOB not available\n"));
    return EFI_NOT_FOUND;
  }
  mCpuInitDataHob = (CPU_INIT_DATA_HOB *) ((UINTN) Hob + sizeof (EFI_HOB_GUID_TYPE));
  mCpuConfig = (CPU_CONFIG *) ((UINTN) mCpuInitDataHob->CpuConfig);

  //
  // Check if DTS disabled in setup.
  //
  if (mCpuConfig->EnableDts == CPU_FEATURE_DISABLE) {
    DEBUG ((DEBUG_WARN, "DTS not enabled/supported, so driver not loaded into SMM\n"));
    return EFI_SUCCESS;
  }

  //
  // Verify the code supports the number of processors present.
  //
  ASSERT_EFI_ERROR (gSmst->NumberOfCpus <= MAX_NUMBER_OF_THREADS_SUPPORTED);

  //
  // Get the ACPI Base Address
  //
  mAcpiBaseAddr = (UINT16)PcdGet16(PcdScAcpiIoPortBaseAddress);

  //
  // Initialize DTS setup value
  //
  mDtsValue = (UINT8) mCpuConfig->EnableDts;

  //
  // Locate CPU Global NVS area
  //
  Status = gBS->LocateProtocol (&gCpuGlobalNvsAreaProtocolGuid, NULL, (VOID **) &CpuGlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mCpuGlobalNvsAreaPtr = CpuGlobalNvsAreaProtocol->Area;

  //
  // CPU_ID 6, EAX bit 6 for the Package temperature MSR support
  //
  ZeroMem (&Cpuid06, sizeof (Cpuid06));
  AsmCpuid (6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);

  mIsPackageTempMsrAvailable                    = (((UINT8) (mCpuConfig->PackageDts) && ((Cpuid06.RegEax >> 6) & 0x01) ) ? 1: 0);
  DEBUG ((DEBUG_INFO, "Is Package DTS Supported: %x\n", mIsPackageTempMsrAvailable));
  mCpuGlobalNvsAreaPtr->IsPackageTempMSRAvailable  = mIsPackageTempMsrAvailable;

  //
  // Register a callback function to handle Digital Thermal Sensor SMIs.
  //
  if (mCpuConfig->EnableDts != DTS_OUT_OF_SPEC_ONLY) {
    Status = gSmst->SmiHandlerRegister (DtsSmiCallback, NULL, &Handle);
    ASSERT_EFI_ERROR (Status);
  } else {
    Status = gSmst->SmiHandlerRegister (DtsOutOfSpecSmiCallback, NULL, &Handle);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Locate the Sx Dispatch Protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, (VOID **) &SxDispatchProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Register the callback for S3 entry
  //
  SxDispatchContext.Type  = SxS3;
  SxDispatchContext.Phase = SxEntry;
  Status = SxDispatchProtocol->Register (
                                 SxDispatchProtocol,
                                 DtsS3EntryCallBack,
                                 &SxDispatchContext,
                                 &SxDispatchHandle
                                 );
  ASSERT_EFI_ERROR (Status);

  if (mCpuConfig->EnableDts != DTS_OUT_OF_SPEC_ONLY) {
    //
    // Get the TCC Activation Temperature and use it for TjMax.
    //
    MsrData.Qword         = AsmReadMsr64 (EFI_MSR_IA32_TEMPERATURE_TARGET);

    mDtsTjMax             = (MsrData.Bytes.ThirdByte);
    mDtsThresholdTable    = mDigitalThermalSensorThresholdTable;
    mNoOfThresholdRanges  = DTS_NUMBER_THRESHOLD_RANGES;
  }

  if (mIsPackageTempMsrAvailable) {
    //
    // Enable the DTS on package.
    //
    PackageDigitalThermalSensorEnable (NULL);
  } else {
    //
    // Enable the DTS on all logical processors.
    //
    RunOnAllLogicalProcessors (DigitalThermalSensorEnable, NULL);
  }

  //
  // Initialize Digital Thermal Sensor Function in POST
  //
  InitDigitalThermalSensor ();

  return EFI_SUCCESS;
}

