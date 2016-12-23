/** @file
  Defines and prototypes for the Digital Thermal Sensor SMM driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _POWER_MGMT_DTS_H_
#define _POWER_MGMT_DTS_H_

//
// Include files
//
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>
#include <CpuDataStruct.h>
#include <Protocol/CpuGlobalNvsArea.h>
#include <Private/CpuInitDataHob.h>
#include <Library/MmPciLib.h>
#include <Library/AslUpdateLib.h>
#include "PowerMgmtDtsLib.h"

#define MSR_IA32_THERM_STATUS                 0x0000019C
///
/// Stall period in microseconds
///
#define DTS_WAIT_PERIOD                       1
#define DTS_AP_SAFE_RETRY_LIMIT               3
///
/// Define module definitions
///
#define TJ_MAX                                110
#define DTS_CRITICAL_TEMPERATURE              255

//
// Generic definitions for DTS
//
#define DTS_OUT_OF_SPEC_ONLY                  2
#define DTS_OUT_OF_SPEC_OCCURRED              3

#define DTS_SAMPLE_RATE                       0x10
#define EFI_MSR_XAPIC_BASE                    0x1B
#define TH1_VALUE                             8
#define TH1_ENABLE                            (1 << 15)
#define TH2_VALUE                             16
#define TH2_ENABLE                            (1 << 23)
#define OFFSET_MASK                           (0x7F)
#define OVERHEAT_INTERRUPT_ENABLE             (1 << 4)

#define B_OUT_OF_SPEC_STATUS                  (1 << 4)
#define B_OUT_OF_SPEC_STATUS_LOG              (1 << 5)
#define B_THERMAL_THRESHOLD_1_STATUS          (1 << 6)
#define B_THERMAL_THRESHOLD_1_STATUS_LOG      (1 << 7)
#define B_THERMAL_THRESHOLD_2_STATUS          (1 << 8)
#define B_THERMAL_THRESHOLD_2_STATUS_LOG      (1 << 9)
#define B_READING_VALID                       (1 << 31)

#define EFI_MSR_IA32_TEMPERATURE_TARGET       0x1A2
#define EFI_MSR_EXT_XAPIC_LVT_THERM           0x833
#define EFI_MSR_MISC_PWR_MGMT                 0x1AA
#define B_LOCK_THERMAL_INT                    (1 << 22)

#define THERM_STATUS_LOG_MASK                 (B_THERMAL_THRESHOLD_2_STATUS_LOG | B_THERMAL_THRESHOLD_1_STATUS_LOG | B_OUT_OF_SPEC_STATUS_LOG)
#define THERM_STATUS_THRESHOLD_LOG_MASK       (B_THERMAL_THRESHOLD_2_STATUS_LOG | B_THERMAL_THRESHOLD_1_STATUS_LOG)

#define EFI_MSR_IA32_PACKAGE_THERM_STATUS     0x1B1
#define EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT  0x1B2

#define B_DTS_IO_TRAP                         (1 << 2)
#define B_SWGPE_CTRL                          (1 << 1)
#define DTS_IO_TRAP_REGISTER_LOW_DWORD        (0x00040001 + ICH_DTS_IO_TRAP_BASE_ADDRESS) ///< DigitalThermalSensor IO Trap High DWord value
#define DTS_IO_TRAP_REGISTER_HIGH_DWORD       0x000200F0                                  ///< DigitalThermalSensor IO Trap High DWord value
#define LOCAL_APIC_THERMAL_DEF                0xFEE00330
#define B_INTERRUPT_MASK                      (1 << 16)
#define B_DELIVERY_MODE                       (0x07 << 8)
#define V_MODE_SMI                            (0x02 << 8)
#define B_VECTOR                              (0xFF << 0)

#define DTS_NUMBER_THRESHOLD_RANGES           9     ///< How many ranges are in the threshold table
#define IO_TRAP_INIT_AP_DTS_FUNCTION          0x0A  ///< Enable AP DigitalThermalSensor function
#define IO_TRAP_INIT_DTS_FUNCTION_AFTER_S3    0x14  ///< Enable Digital Thermal Sensor function after resume from S3
#define IO_TRAP_DISABLE_UPDATE_DTS            0x1E  ///< Disable update DTS temperature and threshold value in every SMI
#define INIT_DTS_SCF_MIN                      0x10  ///< SCF Minimum value.
#define INIT_DTS_SCF_UNITY                    0x20  ///< SCF Unity Value.
#define INIT_DTS_SCF_MAX                      0x30  ///< SCF Maximum value.
#define UPDATE_DTS_EVERY_SMI                  TRUE  ///< Update DTS temperature and threshold value in every SMI
#define R_PCH_ACPI_PM1_CNT                    0x04
#define B_PCH_ACPI_PM1_CNT_SCI_EN             0x00000001
#define R_PCH_LPC_ACPI_BASE                   0x40
#define PCI_DEVICE_NUMBER_PCH_LPC             31
#define MAX_NUMBER_OF_THREADS_SUPPORTED       8     ///< Max number of threads supported by processor.


///
/// Enumerate a DTS event type
///
typedef enum {
  DtsEventNone,
  DtsEventThreshold,
  DtsEventOutOfSpec,
  DtsEventMax
} DTS_EVENT_TYPE;

//
// Function declarations
//
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
  );

/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @param[in]      SmmImageHandle        Image handle returned by the SMM driver.
  @param[in]      ContextData           Pointer to data buffer to be used for handler.
  @param[in, out] CommunicationBuffer   Pointer to the buffer that contains the communication Message
  @param[in, out] SourceSize            Size of the memory image to be used for handler.

  @retval         EFI_SUCCESS           Callback Function Executed

**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE  SmmImageHandle,
  IN CONST VOID  *ContextData,         OPTIONAL
  IN OUT VOID    *CommunicationBuffer, OPTIONAL
  IN OUT UINTN   *SourceSize           OPTIONAL
  );

/**
  Call from SMI handler to handle Package thermal temperature Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @retval    None

**/
VOID
PackageThermalDTS (
  VOID
  );

/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval   EFI_SUCCESS      Init Digital Thermal Sensor successfully

**/
EFI_STATUS
InitDigitalThermalSensor (
  VOID
  );

/**
  Initializes the Thermal Sensor Control MSR

  This function must be AP safe.

  @param[in]   Buffer        Unused.

**/
VOID
EFIAPI
DigitalThermalSensorEnable (
  IN VOID *Buffer
  );

/**
  Initializes the Package Thermal Sensor Control MSR

  @param[in]  Buffer        Unused.

  @retval     EFI_SUCCESS   The function completed successfully.

**/
EFI_STATUS
PackageDigitalThermalSensorEnable (
  IN VOID *Buffer
  );

/**
  Generates a _GPE._L02 SCI to an ACPI OS.

**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  );

/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in]  Buffer     Pointer to DTS_EVENT_TYPE

**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN VOID *Buffer
  );

/**
  Checks for a Package Thermal Event by reading MSR.

  @param[in] PkgEventType     DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval    TRUE             Means this is a Package DTS Thermal event.
  @retval    FALSE            Means this is not a Package DTS Thermal event.

**/
BOOLEAN
DigitalThermalSensorEventCheckPackageMsr (
  IN DTS_EVENT_TYPE *PkgEventType
  );

/**
  Checks for a Core Thermal Event on any processor

  @param[in]  EventType      DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval     TRUE           Means this is a DTS Thermal event
  @retval     FALSE          Means this is not a DTS Thermal event.

**/
BOOLEAN
DigitalThermalSensorEventCheck (
  IN DTS_EVENT_TYPE *EventType
  );

/**
  Read the temperature and reconfigure the thresholds.
  This function must be AP safe.

  @param[in]  Buffer        Pointer to UINT8 to update with the current temperature

  @retval     EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully.

**/
VOID
EFIAPI
DigitalThermalSensorSetThreshold (
  IN VOID *Buffer
  );

/**
  Read the temperature and reconfigure the thresholds on the package

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval    EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully.

**/
EFI_STATUS
PackageDigitalThermalSensorSetThreshold (
  IN VOID *Buffer
  );

/**
  Set the Out Of Spec Interrupt in all cores
  This function must be AP safe.

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Out Of Spec Interrupt programmed successfully.

**/
VOID
EFIAPI
DigitalThermalSensorSetOutOfSpecInterrupt (
  IN VOID *Buffer
  );

/**
  Set the Out Of Spec Interrupt on the package

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Out Of Spec Interrupt programmed successfully.

**/
EFI_STATUS
PackageDigitalThermalSensorSetOutOfSpecInterrupt (
  IN VOID *Buffer
  );

/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval    EFI_SUCCESS   Enable Local APIC to generate a SMI successfully.

**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  IN VOID *Buffer
  );

/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in]  Buffer        Unused

**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  IN VOID *Buffer
  );

/**
  Performs initialization of the threshold table.

  @retval  EFI_SUCCESS    Threshold tables initialized successfully.

**/
EFI_STATUS
InitThresholdTable (
  VOID
  );

/**
  This function executes DTS procedures for preparing to enter S3.

  @param[in]      Handle             Handle of the callback
  @param[in]      Context            The dispatch context
  @param[in, out] CommBuffer         Pointer to the buffer that contains the communication Message
  @param[in, out] CommBufferSize    The size of the CommBuffer.

  @retval         EFI_SUCCESS        DTS disabled

**/
EFI_STATUS
EFIAPI
DtsS3EntryCallBack (
  IN  EFI_HANDLE   DispatchHandle,
  IN CONST VOID    *Context         OPTIONAL,
  IN OUT VOID      *CommBuffer      OPTIONAL,
  IN OUT UINTN     *CommBufferSize  OPTIONAL
  );
#endif

