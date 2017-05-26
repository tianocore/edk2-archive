/** @file
  PCH Pcie SMM Driver Entry.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScInitSmm.h"

GLOBAL_REMOVE_IF_UNREFERENCED SC_PCIE_DEVICE_OVERRIDE     *mDevAspmOverride;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                      mNumOfDevAspmOverride;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                       mPolicyRevision;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                       mPchBusNumber;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                       mTempRootPortBusNumMin;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                       mTempRootPortBusNumMax;
GLOBAL_REMOVE_IF_UNREFERENCED SC_PCIE_ROOT_PORT_CONFIG    mPcieRootPortConfig[PCIE_MAX_ROOT_PORTS];
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                      mLtrNonSupportRpBitMap;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                     mPciePmSwSmiExecuted = FALSE;

extern EFI_GUID gScDeviceTableHobGuid;

/**
  PCIE Link Active State Change Hotplug SMI call back function for all Root ports

  @param[in] DispatchHandle             Handle of this dispatch function
  @param[in] RpContext                  Rootport context, which contains RootPort Index,
                                        and RootPort PCI BDF.

  @retval    None

**/
VOID
EFIAPI
ScPcieLinkActiveStateChange (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  SC_PCIE_SMI_RP_CONTEXT            *RpContext
  )
{
  return;
}


/**
  PCIE Link Equalization Request SMI call back function for all Root ports

  @param[in] DispatchHandle             Handle of this dispatch function
  @param[in] RpContext                  Rootport context, which contains RootPort Index,
                                        and RootPort PCI BDF.

  @retval    None

**/
VOID
EFIAPI
ScPcieLinkEqHandlerFunction (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  SC_PCIE_SMI_RP_CONTEXT            *RpContext
  )
{
  return;
}


/**
  An IoTrap callback to config PCIE power management settings

  @param[in]  None

  @retval     None

**/
VOID
ScPciePmIoTrapSmiCallback (
  VOID
  )
{
  UINT8                                     PortIndex;
  UINTN                                     RpBase;
  UINTN                                     RpDevice;
  UINTN                                     RpFunction;
  BOOLEAN                                   L1SubstatesSupportedPerPort;
  UINT16                                    AspmVal;
  BOOLEAN                                   ClkreqPerPortSupported;
  BOOLEAN                                   ClkreqSupportedInAllEnabledPorts;
  BOOLEAN                                   L1SupportedInAllEnabledPorts;
  BOOLEAN                                   FirstRPToSetPm;
  BOOLEAN                                   LtrSupported;
  BOOLEAN                                   IsAnyRootPortEnabled;
  UINT8                                     MaxPciePortNum;

  AspmVal                                   = 0;
  L1SubstatesSupportedPerPort               = FALSE;
  ClkreqPerPortSupported                    = FALSE;
  ClkreqSupportedInAllEnabledPorts          = TRUE;
  L1SupportedInAllEnabledPorts              = TRUE;
  FirstRPToSetPm                            = TRUE;
  LtrSupported                              = TRUE;
  IsAnyRootPortEnabled                      = FALSE;
  MaxPciePortNum                            = GetScMaxPciePortNum ();

  for (PortIndex = 0; PortIndex < MaxPciePortNum; PortIndex++) {
    GetScPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_SC, (UINT32) RpDevice, (UINT32) RpFunction);

    if ((MmioRead16 (RpBase) != 0xFFFF) &&
        ((MmioRead16 (RpBase + R_PCH_PCIE_SLSTS) & B_PCIE_SLSTS_PDS) != 0)) {
      PcieCheckPmConfig (
        DEFAULT_PCI_BUS_NUMBER_SC,
        (UINT8) RpDevice,
        (UINT8) RpFunction,
        mNumOfDevAspmOverride,
        mDevAspmOverride,
        mTempRootPortBusNumMin,
        mTempRootPortBusNumMax,
        &mPcieRootPortConfig[PortIndex],
        &L1SubstatesSupportedPerPort,
        mPolicyRevision,
        &AspmVal,
        &ClkreqPerPortSupported,
        &LtrSupported
        );

      if ((AspmVal & V_PCIE_LCTL_ASPM_L1) != V_PCIE_LCTL_ASPM_L1) {
        L1SupportedInAllEnabledPorts = FALSE;
      }

      if (ClkreqPerPortSupported == FALSE) {
        ClkreqSupportedInAllEnabledPorts = FALSE;
      }

      if (!LtrSupported) {
        mLtrNonSupportRpBitMap |= 1<<PortIndex;
      }

      IsAnyRootPortEnabled  = TRUE;
    }
  }

  for (PortIndex = 0; PortIndex < MaxPciePortNum; PortIndex++) {
    GetScPcieRpDevFun (PortIndex, &RpDevice, &RpFunction);
    RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_SC, (UINT32) RpDevice, (UINT32) RpFunction);

    if (MmioRead16 (RpBase) != 0xFFFF) {
      PcieSetPm (
        DEFAULT_PCI_BUS_NUMBER_SC,
        (UINT8) RpDevice,
        (UINT8) RpFunction,
        mNumOfDevAspmOverride,
        mDevAspmOverride,
        mTempRootPortBusNumMin,
        mTempRootPortBusNumMax,
        &mPcieRootPortConfig[PortIndex],
        &L1SubstatesSupportedPerPort,
        mPolicyRevision,
        FirstRPToSetPm,
        L1SupportedInAllEnabledPorts,
        ClkreqSupportedInAllEnabledPorts,
        &LtrSupported,
        TRUE
        );

      FirstRPToSetPm = FALSE;
    }
  }
}


/**
  The SW SMI callback to config PCIE power management settings

  @param[in]      DispatchHandle  The handle of this callback, obtained when registering
  @param[in]      CallbackContext Pointer to the EFI_SMM_SW_REGISTER_CONTEXT
  @param[in, out] CommBuffer      Point to the CommBuffer structure
  @param[in, out] CommBufferSize  Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
ScPciePmSwSmiCallback (
  IN EFI_HANDLE                  DispatchHandle,
  IN EFI_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  if (DispatchContext->SwSmiInputValue == mPcieRpConfig->AspmSwSmiNumber) {
    if (mPciePmSwSmiExecuted == FALSE) {
      ScPciePmIoTrapSmiCallback ();
      mPciePmSwSmiExecuted = TRUE;
    }
  } else {
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
  }
}


/**
  This function clear the Io trap executed flag before enter S3

  @param[in]      Handle             Handle of the callback
  @param[in]      Context            The dispatch context
  @param[in, out] CommBuffer         Point to the CommBuffer structure
  @param[in, out] CommBufferSize     Point to the Size of CommBuffer structure

  @retval         EFI_SUCCESS        PCH register saved

**/
EFI_STATUS
EFIAPI
ScPcieS3EntryCallBack (
  IN EFI_HANDLE   Handle,
  IN CONST VOID   *Context OPTIONAL,
  IN OUT   VOID   *CommBuffer OPTIONAL,
  IN OUT   UINTN  *CommBufferSize OPTIONAL
  )
{
  mPciePmSwSmiExecuted = FALSE;
  return EFI_SUCCESS;
}


/**
  Register PCIE Hotplug SMI dispatch function to handle Hotplug enabling

  @param[in] ImageHandle          The image handle of this module
  @param[in] SystemTable          The EFI System Table

  @retval    EFI_SUCCESS          The function completes successfully

**/
EFI_STATUS
EFIAPI
InitializeScPcieSmm (
  IN      EFI_HANDLE                   ImageHandle,
  IN      EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HOB_GUID_TYPE*                    Hob;
  UINT32                                DevTableSize;
  EFI_HANDLE                            SwDispatchHandle;
  EFI_SMM_SW_REGISTER_CONTEXT           SwDispatchContext;
  EFI_HANDLE                            SxDispatchHandle;
  EFI_SMM_SX_REGISTER_CONTEXT           SxDispatchContext;
  UINT8                                 MaxPciePortNum;
  SI_POLICY_HOB                         *SiPolicyHob;

  DEBUG ((DEBUG_INFO, "InitializeScPcieSmm () Start\n"));

  MaxPciePortNum = GetScMaxPciePortNum ();

  //
  // Get Silicon Policy data HOB
  //
  Hob = GetFirstGuidHob (&gSiPolicyHobGuid);
  if (Hob == NULL) {
    ASSERT (Hob != NULL);
    return EFI_NOT_FOUND;
  }

  SiPolicyHob = GET_GUID_HOB_DATA (Hob);
  mPolicyRevision = mScPolicy->TableHeader.Header.Revision;
  mPchBusNumber   = DEFAULT_PCI_BUS_NUMBER_SC;
  mTempRootPortBusNumMin = SiPolicyHob->TempPciBusMin;
  mTempRootPortBusNumMax = SiPolicyHob->TempPciBusMax;

  ASSERT (sizeof (mPcieRootPortConfig) == sizeof (mPcieRpConfig->RootPort));
  CopyMem (
    mPcieRootPortConfig,
    &(mPcieRpConfig->RootPort),
    sizeof (mPcieRootPortConfig)
    );

  mDevAspmOverride                  = NULL;
  mNumOfDevAspmOverride             = 0;
  mLtrNonSupportRpBitMap            = 0;

  Hob = GetFirstGuidHob (&gScDeviceTableHobGuid);
  if (Hob != NULL) {
    DevTableSize = GET_GUID_HOB_DATA_SIZE (Hob);
    ASSERT ((DevTableSize % sizeof (SC_PCIE_DEVICE_OVERRIDE)) == 0);
    mNumOfDevAspmOverride = DevTableSize / sizeof (SC_PCIE_DEVICE_OVERRIDE);
    DEBUG ((DEBUG_INFO, "Found PcieDeviceTable HOB (%d entries)\n", mNumOfDevAspmOverride));
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      DevTableSize,
                      (VOID **) &mDevAspmOverride
                      );
    CopyMem (mDevAspmOverride, GET_GUID_HOB_DATA (Hob), DevTableSize);
  }

  //
  // Register PCIe ASPM SW SMI handler
  //
  SwDispatchHandle  = NULL;
  SwDispatchContext.SwSmiInputValue = mPcieRpConfig->AspmSwSmiNumber;
  Status = mSwDispatch->Register (
                          mSwDispatch,
                          (EFI_SMM_HANDLER_ENTRY_POINT2) ScPciePmSwSmiCallback,
                          &SwDispatchContext,
                          &SwDispatchHandle
                          );
  ASSERT_EFI_ERROR (Status);

  //
  // Register the callback for S3 entry
  //
  SxDispatchContext.Type  = SxS3;
  SxDispatchContext.Phase = SxEntry;
  Status = mSxDispatch->Register (
                          mSxDispatch,
                          ScPcieS3EntryCallBack,
                          &SxDispatchContext,
                          &SxDispatchHandle
                          );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "InitializeScPcieSmm () End\n"));

  return EFI_SUCCESS;
}

