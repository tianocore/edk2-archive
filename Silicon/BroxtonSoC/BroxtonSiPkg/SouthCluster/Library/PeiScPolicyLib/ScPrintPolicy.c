/** @file
  Print whole SC_POLICY_PPI and serial out.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiScPolicyLibrary.h"

/**
  Print SC_INTERRUPT_CONFIG and serial out

  @param[in] InterruptConfig        Pointer to Interrupt Configuration structure

**/
VOID
PrintInterruptConfig (
  IN CONST SC_INTERRUPT_CONFIG     *InterruptConfig
  )
{
  UINTN  Index;
  //
  // Print interrupt information
  //
  DEBUG ((DEBUG_INFO, "------------------ Interrupt Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " Interrupt assignment:\n"));
  DEBUG ((DEBUG_INFO, "  Dxx:Fx INTx IRQ\n"));
  for (Index = 0; Index < InterruptConfig->NumOfDevIntConfig; Index++) {
    DEBUG ((DEBUG_INFO, "  D%02d:F%d    %d %03d\n",
            InterruptConfig->DevIntConfig[Index].Device,
            InterruptConfig->DevIntConfig[Index].Function,
            InterruptConfig->DevIntConfig[Index].IntX,
            InterruptConfig->DevIntConfig[Index].Irq));
  }
  DEBUG ((DEBUG_INFO, " Direct Irq Table:\n"));
  for (Index = 0; Index < InterruptConfig->NumOfDirectIrqTable; Index++) {
    DEBUG ((DEBUG_INFO, "  D%02d:F%d    %d %03d\n",
            InterruptConfig->DirectIrqTable[Index].Port,
            InterruptConfig->DirectIrqTable[Index].PciCfgOffset,
            InterruptConfig->DirectIrqTable[Index].PciIrqNumber,
            InterruptConfig->DirectIrqTable[Index].IrqPin));
  }

  DEBUG ((DEBUG_INFO, " Legacy Interrupt Routing:\n"));
  for (Index = 0; Index < SC_MAX_PXRC_CONFIG; Index++) {
    DEBUG ((DEBUG_INFO, "PxRcRouting[%x] = %x \n", Index ,InterruptConfig->PxRcRouting[Index]));
  }
}


/**
  Print SC_USB_CONFIG and serial out.

  @param[in] UsbConfig         Pointer to a SC_USB_CONFIG that provides the platform setting

**/

VOID
PrintUsbConfig (
  IN CONST SC_USB_CONFIG     *UsbConfig
  )
{
  UINT32  Index;

  DEBUG ((DEBUG_INFO, "------------------ USB Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " DisableComplianceMode= %x\n", UsbConfig->DisableComplianceMode));
  DEBUG ((DEBUG_INFO, " UsbPerPortCtl        = %x\n", UsbConfig->UsbPerPortCtl));
  DEBUG ((DEBUG_INFO, " Mode                 = %x\n", UsbConfig->Usb30Settings.Mode));

  for (Index = 0; Index < GetScXhciMaxUsb2PortNum (); Index++) {
    DEBUG ((DEBUG_INFO, " PortUsb20[%d].Enabled= %x\n", Index, UsbConfig->PortUsb20[Index].Enable));
    DEBUG ((DEBUG_INFO, " PortUsb20[%d].OverCurrentPin= OC%x\n", Index, UsbConfig->PortUsb20[Index].OverCurrentPin));
  }
  for (Index = 0; Index < GetScXhciMaxUsb3PortNum (); Index++) {
    DEBUG ((DEBUG_INFO, " PortUsb30[%d] Enabled= %x\n", Index, UsbConfig->PortUsb30[Index].Enable));
    DEBUG ((DEBUG_INFO, " PortUsb30[%d].OverCurrentPin= OC%x\n", Index, UsbConfig->PortUsb30[Index].OverCurrentPin));
  }
  DEBUG ((DEBUG_INFO, " xDCI Enable = %x\n", UsbConfig->XdciConfig.Enable));

  for (Index = 0; Index < XHCI_MAX_HSIC_PORTS; Index++) {
    DEBUG ((DEBUG_INFO, " HsicPort[%d].Enable  = %x\n", Index, UsbConfig->HsicConfig.HsicPort[Index].Enable));
  }
  for (Index = 0; Index < XHCI_MAX_SSIC_PORTS; Index++) {
    DEBUG ((DEBUG_INFO, " SsicPort[%d].Enable  = %x\n", Index, UsbConfig->SsicConfig.SsicPort[Index].Enable));
    DEBUG ((DEBUG_INFO, " SsicPort[%d].Rate    = %x\n", Index, UsbConfig->SsicConfig.SsicPort[Index].Rate));
  }
  DEBUG ((DEBUG_INFO, " SSIC DlanePwrGating = %x\n", UsbConfig->SsicConfig.DlanePwrGating));

  return;
}


/**
  Print SC_PCIE_CONFIG and serial out.

  @param[in] PcieConfig         Pointer to a SC_PCIE_CONFIG that provides the platform setting

**/
VOID
PrintPcieConfig (
  IN CONST SC_PCIE_CONFIG   *PcieConfig
  )
{
  UINT32  i;

  DEBUG ((DEBUG_INFO, "------------------ PCIe Config ------------------\n"));
  for (i = 0; i < GetScMaxPciePortNum (); i++) {
    DEBUG ((DEBUG_INFO, " RootPort[%d] Enabled                           = %x\n", i, PcieConfig->RootPort[i].Enable));
    DEBUG ((DEBUG_INFO, " RootPort[%d] Hide                              = %x\n", i, PcieConfig->RootPort[i].Hide));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SlotImplemented                   = %x\n", i, PcieConfig->RootPort[i].SlotImplemented));
    DEBUG ((DEBUG_INFO, " RootPort[%d] HotPlug                           = %x\n", i, PcieConfig->RootPort[i].HotPlug));
    DEBUG ((DEBUG_INFO, " RootPort[%d] PmSci                             = %x\n", i, PcieConfig->RootPort[i].PmSci));
    DEBUG ((DEBUG_INFO, " RootPort[%d] ExtSync                           = %x\n", i, PcieConfig->RootPort[i].ExtSync));
    DEBUG ((DEBUG_INFO, " RootPort[%d] ClkReqSupported                   = %x\n", i, PcieConfig->RootPort[i].ClkReqSupported));
    DEBUG ((DEBUG_INFO, " RootPort[%d] ClkReqNumber                      = %x\n", i, PcieConfig->RootPort[i].ClkReqNumber));
    DEBUG ((DEBUG_INFO, " RootPort[%d] UnsupportedRequestReport          = %x\n", i, PcieConfig->RootPort[i].UnsupportedRequestReport));
    DEBUG ((DEBUG_INFO, " RootPort[%d] FatalErrorReport                  = %x\n", i, PcieConfig->RootPort[i].FatalErrorReport));
    DEBUG ((DEBUG_INFO, " RootPort[%d] NoFatalErrorReport                = %x\n", i, PcieConfig->RootPort[i].NoFatalErrorReport));
    DEBUG ((DEBUG_INFO, " RootPort[%d] CorrectableErrorReport            = %x\n", i, PcieConfig->RootPort[i].CorrectableErrorReport));
    DEBUG ((DEBUG_INFO, " RootPort[%d] PmeInterrupt                      = %x\n", i, PcieConfig->RootPort[i].PmeInterrupt));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SystemErrorOnFatalError           = %x\n", i, PcieConfig->RootPort[i].SystemErrorOnFatalError));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SystemErrorOnNonFatalError        = %x\n", i, PcieConfig->RootPort[i].SystemErrorOnNonFatalError));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SystemErrorOnCorrectableError     = %x\n", i, PcieConfig->RootPort[i].SystemErrorOnCorrectableError));
    DEBUG ((DEBUG_INFO, " RootPort[%d] AcsEnabled                        = %x\n", i, PcieConfig->RootPort[i].AcsEnabled));
    DEBUG ((DEBUG_INFO, " RootPort[%d] AdvancedErrorReporting            = %x\n", i, PcieConfig->RootPort[i].AdvancedErrorReporting));
    DEBUG ((DEBUG_INFO, " RootPort[%d] TransmitterHalfSwing              = %x\n", i, PcieConfig->RootPort[i].TransmitterHalfSwing));
    DEBUG ((DEBUG_INFO, " RootPort[%d] PcieSpeed                         = %x\n", i, PcieConfig->RootPort[i].PcieSpeed));
    DEBUG ((DEBUG_INFO, " RootPort[%d] PhysicalSlotNumber                = %x\n", i, PcieConfig->RootPort[i].PhysicalSlotNumber));
    DEBUG ((DEBUG_INFO, " RootPort[%d] CompletionTimeout                 = %x\n", i, PcieConfig->RootPort[i].CompletionTimeout));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SelectableDeemphasis              = %x\n", i, PcieConfig->RootPort[i].SelectableDeemphasis));
    DEBUG ((DEBUG_INFO, " RootPort[%d] Aspm                              = %x\n", i, PcieConfig->RootPort[i].Aspm));
    DEBUG ((DEBUG_INFO, " RootPort[%d] L1Substates                       = %x\n", i, PcieConfig->RootPort[i].L1Substates));
    DEBUG ((DEBUG_INFO, " RootPort[%d] LtrEnable                         = %x\n", i, PcieConfig->RootPort[i].LtrEnable));
    DEBUG ((DEBUG_INFO, " RootPort[%d] LtrConfigLock                     = %x\n", i, PcieConfig->RootPort[i].LtrConfigLock));
    DEBUG ((DEBUG_INFO, " RootPort[%d] LtrMaxSnoopLatency                = %x\n", i, PcieConfig->RootPort[i].LtrMaxSnoopLatency));
    DEBUG ((DEBUG_INFO, " RootPort[%d] LtrMaxNoSnoopLatency              = %x\n", i, PcieConfig->RootPort[i].LtrMaxNoSnoopLatency));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SnoopLatencyOverrideMode          = %x\n", i, PcieConfig->RootPort[i].SnoopLatencyOverrideMode));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SnoopLatencyOverrideMultiplier    = %x\n", i, PcieConfig->RootPort[i].SnoopLatencyOverrideMultiplier));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SnoopLatencyOverrideValue         = %x\n", i, PcieConfig->RootPort[i].SnoopLatencyOverrideValue));
    DEBUG ((DEBUG_INFO, " RootPort[%d] NonSnoopLatencyOverrideMode       = %x\n", i, PcieConfig->RootPort[i].NonSnoopLatencyOverrideMode));
    DEBUG ((DEBUG_INFO, " RootPort[%d] NonSnoopLatencyOverrideMultiplier = %x\n", i, PcieConfig->RootPort[i].NonSnoopLatencyOverrideMultiplier));
    DEBUG ((DEBUG_INFO, " RootPort[%d] NonSnoopLatencyOverrideValue      = %x\n", i, PcieConfig->RootPort[i].NonSnoopLatencyOverrideValue));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SlotPowerLimitScale               = %x\n", i, PcieConfig->RootPort[i].SlotPowerLimitScale));
    DEBUG ((DEBUG_INFO, " RootPort[%d] SlotPowerLimitValue               = %x\n", i, PcieConfig->RootPort[i].SlotPowerLimitValue));
  }
  DEBUG ((DEBUG_INFO, " EnablePort8xhDecode          = %x\n", PcieConfig->EnablePort8xhDecode));
  DEBUG ((DEBUG_INFO, " ScPciePort8xhDecodePortIndex = %x\n", PcieConfig->ScPciePort8xhDecodePortIndex));
  DEBUG ((DEBUG_INFO, " DisableRootPortClockGating   = %x\n", PcieConfig->DisableRootPortClockGating));
  DEBUG ((DEBUG_INFO, " EnablePeerMemoryWrite        = %x\n", PcieConfig->EnablePeerMemoryWrite));
  DEBUG ((DEBUG_INFO, " AspmSwSmiNumber              = %x\n", PcieConfig->AspmSwSmiNumber));
  DEBUG ((DEBUG_INFO, " ComplianceTestMode           = %x\n", PcieConfig->ComplianceTestMode));
}


/**
  Print SC_SATA_CONFIG and serial out.

  @param[in] SataConfig         Pointer to a SC_SATA_CONFIG that provides the platform setting

**/
VOID
PrintSataConfig (
  IN CONST SC_SATA_CONFIG   *SataConfig
  )
{
  UINT32  i;

  DEBUG ((DEBUG_INFO, "------------------ SATA Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " Enable = %x\n", SataConfig->Enable));
  DEBUG ((DEBUG_INFO, " SataMode = %x\n", SataConfig->SataMode));

  for (i = 0; i < SC_MAX_SATA_PORTS; i++) {
    DEBUG ((DEBUG_INFO, " PortSettings[%d] Enabled          = %x\n", i, SataConfig->PortSettings[i].Enable));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] HotPlug          = %x\n", i, SataConfig->PortSettings[i].HotPlug));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] InterlockSw      = %x\n", i, SataConfig->PortSettings[i].InterlockSw));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] External         = %x\n", i, SataConfig->PortSettings[i].External));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] SpinUp           = %x\n", i, SataConfig->PortSettings[i].SpinUp));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] SolidStateDrive  = %x\n", i, SataConfig->PortSettings[i].SolidStateDrive));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] DevSlp           = %x\n", i, SataConfig->PortSettings[i].DevSlp));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] EnableDitoConfig = %x\n", i, SataConfig->PortSettings[i].EnableDitoConfig));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] DmVal            = %x\n", i, SataConfig->PortSettings[i].DmVal));
    DEBUG ((DEBUG_INFO, " PortSettings[%d] DitoVal          = %x\n", i, SataConfig->PortSettings[i].DitoVal));
  }
  DEBUG ((DEBUG_INFO, " SpeedSupport    = %x\n", SataConfig->SpeedLimit));
  DEBUG ((DEBUG_INFO, " eSATASpeedLimit = %x\n", SataConfig->eSATASpeedLimit));
  DEBUG ((DEBUG_INFO, " TestMode        = %x\n", SataConfig->TestMode));
  DEBUG ((DEBUG_INFO, " SalpSupport     = %x\n", SataConfig->SalpSupport));
  DEBUG ((DEBUG_INFO, " PwrOptEnable    = %x\n", SataConfig->PwrOptEnable));
}


/**
  Print SC_IOAPIC_CONFIG and serial out.

  @param[in] IoApicConfig         Pointer to a SC_IOAPIC_CONFIG that provides the platform setting

**/
VOID
PrintIoApicConfig (
  IN CONST SC_IOAPIC_CONFIG   *IoApicConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ IOAPIC Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " BdfValid          = %x\n", IoApicConfig->BdfValid));
  DEBUG ((DEBUG_INFO, " BusNumber         = %x\n", IoApicConfig->BusNumber));
  DEBUG ((DEBUG_INFO, " DeviceNumber      = %x\n", IoApicConfig->DeviceNumber));
  DEBUG ((DEBUG_INFO, " FunctionNumber    = %x\n", IoApicConfig->FunctionNumber));
  DEBUG ((DEBUG_INFO, " IoApicId          = %x\n", IoApicConfig->IoApicId));
  DEBUG ((DEBUG_INFO, " ApicRangeSelect   = %x\n", IoApicConfig->ApicRangeSelect));
  DEBUG ((DEBUG_INFO, " IoApicEntry24_119 = %x\n", IoApicConfig->IoApicEntry24_119));
}


/**
  Print SC_HPET_CONFIG and serial out.

  @param[in] HpetConfig         Pointer to a SC_HPET_CONFIG that provides the platform setting

**/
VOID
PrintHpetConfig (
  IN CONST SC_HPET_CONFIG   *HpetConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ HPET Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " Enable         = %x\n", HpetConfig->Enable));
  DEBUG ((DEBUG_INFO, " BdfValid       = %x\n", HpetConfig->BdfValid));
  DEBUG ((DEBUG_INFO, " BusNumber      = %x\n", HpetConfig->BusNumber));
  DEBUG ((DEBUG_INFO, " DeviceNumber   = %x\n", HpetConfig->DeviceNumber));
  DEBUG ((DEBUG_INFO, " FunctionNumber = %x\n", HpetConfig->FunctionNumber));
  DEBUG ((DEBUG_INFO, " Base           = %x\n", HpetConfig->Base));
}


/**
  Print PCH_LOCK_DOWN_CONFIG and serial out.

  @param[in] LockDownConfig         Pointer to a PCH_LOCK_DOWN_CONFIG that provides the platform setting

**/
VOID
PrintLockDownConfig (
  IN CONST SC_LOCK_DOWN_CONFIG   *LockDownConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ Lock Down Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " GlobalSmi           = %x\n", LockDownConfig->GlobalSmi));
  DEBUG ((DEBUG_INFO, " BiosInterface       = %x\n", LockDownConfig->BiosInterface));
  DEBUG ((DEBUG_INFO, " RtcLock             = %x\n", LockDownConfig->RtcLock));
  DEBUG ((DEBUG_INFO, " BiosLock            = %x\n", LockDownConfig->BiosLock));
  DEBUG ((DEBUG_INFO, " SpiEiss             = %x\n", LockDownConfig->SpiEiss));
  DEBUG ((DEBUG_INFO, " BiosLockSwSmiNumbe  = %x\n", LockDownConfig->BiosLockSwSmiNumber));
  DEBUG ((DEBUG_INFO, " TcoTimerLock        = %x\n", LockDownConfig->TcoLock));
}

/**
  Print SC_SMBUS_CONFIG and serial out.

  @param[in] SmbusConfig         Pointer to a SC_SMBUS_CONFIG that provides the platform setting

**/
VOID
PrintSmbusConfig (
  IN CONST SC_SMBUS_CONFIG   *SmbusConfig
  )
{
  UINT32  i;

  DEBUG ((DEBUG_INFO, "------------------ SMBus Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " Enable                = %x\n", SmbusConfig->Enable));
  DEBUG ((DEBUG_INFO, " ArpEnable             = %x\n", SmbusConfig->ArpEnable));
  DEBUG ((DEBUG_INFO, " DynamicPowerGating    = %x\n", SmbusConfig->DynamicPowerGating));
  DEBUG ((DEBUG_INFO, " SmbusIoBase           = %x\n", SmbusConfig->SmbusIoBase));
  DEBUG ((DEBUG_INFO, " NumRsvdSmbusAddresses = %x\n", SmbusConfig->NumRsvdSmbusAddresses));
  DEBUG ((DEBUG_INFO, " RsvdSmbusAddressTable = {"));
  for (i = 0; i < SmbusConfig->NumRsvdSmbusAddresses; ++i) {
    DEBUG ((DEBUG_INFO, " %02xh", SmbusConfig->RsvdSmbusAddressTable[i]));
  }
  DEBUG ((DEBUG_INFO, " }\n"));
}


/**
  Print SC_HDAUDIO_CONFIG and serial out.

  @param[in] HdaConfig         Pointer to a SC_HDAUDIO_CONFIG that provides the platform setting

**/
VOID
PrintHdAudioConfig (
  IN CONST SC_HDAUDIO_CONFIG   *HdaConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ HD-Audio Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " HDA Enable                   = %x\n", HdaConfig->Enable));
  DEBUG ((DEBUG_INFO, " DSP Enable                   = %x\n", HdaConfig->DspEnable));
  DEBUG ((DEBUG_INFO, " Pme                          = %x\n", HdaConfig->Pme));
  DEBUG ((DEBUG_INFO, " I/O Buffer Ownership         = %x\n", HdaConfig->IoBufferOwnership));
  DEBUG ((DEBUG_INFO, " I/O Buffer Voltage           = %x\n", HdaConfig->IoBufferVoltage));
  DEBUG ((DEBUG_INFO, " VC Type                      = %x\n", HdaConfig->VcType));
  DEBUG ((DEBUG_INFO, " DSP Feature Mask             = %x\n", HdaConfig->DspFeatureMask));
  DEBUG ((DEBUG_INFO, " DSP PP Module Mask           = %x\n", HdaConfig->DspPpModuleMask));
  DEBUG ((DEBUG_INFO, " ResetWaitTimer               = %x\n", HdaConfig->ResetWaitTimer));
  DEBUG ((DEBUG_INFO, " VcType                       = %x\n", HdaConfig->VcType));
  DEBUG ((DEBUG_INFO, " HD-A Link Frequency          = %x\n", HdaConfig->HdAudioLinkFrequency));
  DEBUG ((DEBUG_INFO, " iDisp Link Frequency         = %x\n", HdaConfig->IDispLinkFrequency));
  DEBUG ((DEBUG_INFO, " iDisp Link T-Mode            = %x\n", HdaConfig->IDispLinkTmode));
  DEBUG ((DEBUG_INFO, " DSP Endpoint DMIC            = %x\n", HdaConfig->DspEndpointDmic));
  DEBUG ((DEBUG_INFO, " DSP Endpoint I2S SKP         = %x\n", HdaConfig->DspEndpointI2sSkp));
  DEBUG ((DEBUG_INFO, " DSP Endpoint I2S HP          = %x\n", HdaConfig->DspEndpointI2sHp));
  DEBUG ((DEBUG_INFO, " DSP Endpoint BT              = %x\n", HdaConfig->DspEndpointBluetooth));
  DEBUG ((DEBUG_INFO, " DSP Feature Mask             = %x\n", HdaConfig->DspFeatureMask));
  DEBUG ((DEBUG_INFO, " DSP PP Module Mask           = %x\n", HdaConfig->DspPpModuleMask));
  DEBUG ((DEBUG_INFO, " CSME Memory Transfers        = %x\n", HdaConfig->Mmt));
  DEBUG ((DEBUG_INFO, " Host Memory Transfers        = %x\n", HdaConfig->Hmt));
  DEBUG ((DEBUG_INFO, " BIOS Configuration Lock Down = %x\n", HdaConfig->BiosCfgLockDown));
  DEBUG ((DEBUG_INFO, " Power Gating                 = %x\n", HdaConfig->PwrGate));
  DEBUG ((DEBUG_INFO, " Clock Gating                 = %x\n", HdaConfig->ClkGate));
}


/**
  Print SC_PM_CONFIG and serial out.

  @param[in] PmConfig         Pointer to a SC_PM_CONFIG that provides the platform setting

**/
VOID
PrintPmConfig (
  IN CONST SC_PM_CONFIG   *PmConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ PM Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " WakeConfig PmeB0S5Dis               = %x\n", PmConfig->WakeConfig.PmeB0S5Dis));
  DEBUG ((DEBUG_INFO, " PciClockRun                         = %x\n", PmConfig->PciClockRun));
  DEBUG ((DEBUG_INFO, " Timer8254ClkGateEn                  = %x\n", PmConfig->Timer8254ClkGateEn));
  DEBUG ((DEBUG_INFO, " PwrBtnOverridePeriod                = %x\n", PmConfig->PwrBtnOverridePeriod));
  DEBUG ((DEBUG_INFO, " DisableNativePowerButton            = %x\n", PmConfig->DisableNativePowerButton));
  DEBUG ((DEBUG_INFO, " PowerButterDebounceMode             = %x\n", PmConfig->PowerButterDebounceMode));
}

/**
  Print SC_LPC_SIRQ_CONFIG and serial out.

  @param[in] SerialIrqConfig         Pointer to a SC_LPC_SIRQ_CONFIG that provides the platform setting

**/
VOID
PrintSerialIrqConfig (
  IN CONST SC_LPC_SIRQ_CONFIG   *SerialIrqConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ Serial IRQ Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " SirqEnable      = %x\n", SerialIrqConfig->SirqEnable));
  DEBUG ((DEBUG_INFO, " SirqMode        = %x\n", SerialIrqConfig->SirqMode));
  DEBUG ((DEBUG_INFO, " StartFramePulse = %x\n", SerialIrqConfig->StartFramePulse));
}


/**
  Print SC_GMM_CONFIG and serial out.

  @param[in] GmmConfig         Pointer to a SC_GMM_CONFIG that provides the platform setting

**/
VOID
PrintGmmConfig (
  IN CONST SC_GMM_CONFIG                *GmmConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ GMM Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " Enable                  = %x\n", GmmConfig->Enable));
  DEBUG ((DEBUG_INFO, " ClkGatingPgcbClkTrunk   = %x\n", GmmConfig->ClkGatingPgcbClkTrunk));
  DEBUG ((DEBUG_INFO, " ClkGatingSb             = %x\n", GmmConfig->ClkGatingSb));
  DEBUG ((DEBUG_INFO, " ClkGatingSbClkTrunk     = %x\n", GmmConfig->ClkGatingSbClkTrunk));
  DEBUG ((DEBUG_INFO, " ClkGatingSbClkPartition = %x\n", GmmConfig->ClkGatingSbClkPartition));
  DEBUG ((DEBUG_INFO, " ClkGatingCore           = %x\n", GmmConfig->ClkGatingCore));
  DEBUG ((DEBUG_INFO, " ClkGatingDma            = %x\n", GmmConfig->ClkGatingDma));
  DEBUG ((DEBUG_INFO, " ClkGatingRegAccess      = %x\n", GmmConfig->ClkGatingRegAccess));
  DEBUG ((DEBUG_INFO, " ClkGatingHost           = %x\n", GmmConfig->ClkGatingHost));
  DEBUG ((DEBUG_INFO, " ClkGatingPartition      = %x\n", GmmConfig->ClkGatingPartition));
  DEBUG ((DEBUG_INFO, " ClkGatingTrunk          = %x\n", GmmConfig->ClkGatingTrunk));
  DEBUG ((DEBUG_INFO, " SvPwrGatingHwAutoEnable = %x\n", GmmConfig->SvPwrGatingHwAutoEnable));
  DEBUG ((DEBUG_INFO, " SvPwrGatingD3HotEnable  = %x\n", GmmConfig->SvPwrGatingD3HotEnable));
  DEBUG ((DEBUG_INFO, " SvPwrGatingI3Enable     = %x\n", GmmConfig->SvPwrGatingI3Enable));
  DEBUG ((DEBUG_INFO, " SvPwrGatingPmcReqEnable = %x\n", GmmConfig->SvPwrGatingPmcReqEnable));

  return;
}


/**
  Print SC_LPSS_CONFIG and serial out.

  @param[in] LpssConfig         Pointer to a SC_LPSS_CONFIG that provides the platform setting

**/

VOID
PrintLpssConfig (
  IN CONST SC_LPSS_CONFIG               *LpssConfig
  )
{
  UINT32  Index;

  DEBUG ((DEBUG_INFO, "------------------ LPSS Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " I2c0Enable                  = %x\n", LpssConfig->I2c0Enable));
  DEBUG ((DEBUG_INFO, " I2c1Enable                  = %x\n", LpssConfig->I2c1Enable));
  DEBUG ((DEBUG_INFO, " I2c2Enable                  = %x\n", LpssConfig->I2c2Enable));
  DEBUG ((DEBUG_INFO, " I2c3Enable                  = %x\n", LpssConfig->I2c3Enable));
  DEBUG ((DEBUG_INFO, " I2c4Enable                  = %x\n", LpssConfig->I2c4Enable));
  DEBUG ((DEBUG_INFO, " I2c5Enable                  = %x\n", LpssConfig->I2c5Enable));
  DEBUG ((DEBUG_INFO, " I2c6Enable                  = %x\n", LpssConfig->I2c6Enable));
  DEBUG ((DEBUG_INFO, " I2c7Enable                  = %x\n", LpssConfig->I2c7Enable));
  DEBUG ((DEBUG_INFO, " Hsuart0Enable               = %x\n", LpssConfig->Hsuart0Enable));
  DEBUG ((DEBUG_INFO, " Hsuart1Enable               = %x\n", LpssConfig->Hsuart1Enable));
  DEBUG ((DEBUG_INFO, " Hsuart2Enable               = %x\n", LpssConfig->Hsuart2Enable));
  DEBUG ((DEBUG_INFO, " Hsuart3Enable               = %x\n", LpssConfig->Hsuart3Enable));
  DEBUG ((DEBUG_INFO, " Spi0Enable                  = %x\n", LpssConfig->Spi0Enable));
  DEBUG ((DEBUG_INFO, " Spi1Enable                  = %x\n", LpssConfig->Spi1Enable));
  DEBUG ((DEBUG_INFO, " Spi2Enable                  = %x\n", LpssConfig->Spi2Enable));
  DEBUG ((DEBUG_INFO, " Uart2KernelDebugBaseAddress = %x\n", LpssConfig->Uart2KernelDebugBaseAddress));

  for (Index = 0; Index < LPSS_I2C_DEVICE_NUM; Index++) {
    DEBUG ((DEBUG_INFO, " I2cClkGateCfg[%d]           = %x\n", Index, LpssConfig->I2cClkGateCfg[Index]));
  }

  for (Index = 0; Index < LPSS_HSUART_DEVICE_NUM; Index++) {
    DEBUG ((DEBUG_INFO, " HsuartClkGateCfg[%d]        = %x\n", Index, LpssConfig->HsuartClkGateCfg[Index]));
  }

  for (Index = 0; Index < LPSS_SPI_DEVICE_NUM; Index++) {
    DEBUG ((DEBUG_INFO, " SpiClkGateCfg[%d]           = %x\n", Index, LpssConfig->SpiClkGateCfg[Index]));
  }

  DEBUG ((DEBUG_INFO, " S0ixEnable                  = %x\n", LpssConfig->S0ixEnable));
  DEBUG ((DEBUG_INFO, " OsDbgEnable                 = %x\n", LpssConfig->OsDbgEnable));
}


/**
  Print SC_SCS_CONFIG and serial out.

  @param[in] ScsConfig         Pointer to a SC_SCS_CONFIG that provides the platform setting

**/
VOID
PrintScsConfig (
  IN CONST SC_SCS_CONFIG *ScsConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ SCS Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " SdcardEnable     = %x\n", ScsConfig->SdcardEnable));
  DEBUG ((DEBUG_INFO, " EmmcEnable       = %x\n", ScsConfig->EmmcEnable));
  DEBUG ((DEBUG_INFO, " UfsEnable        = %x\n", ScsConfig->UfsEnable));
  DEBUG ((DEBUG_INFO, " SdioEnable       = %x\n", ScsConfig->SdioEnable));
  DEBUG ((DEBUG_INFO, " EmmcHostMaxSpeed = %x\n", ScsConfig->EmmcHostMaxSpeed));
  DEBUG ((DEBUG_INFO, " GppLock          = %x\n", ScsConfig->GppLock));
  DEBUG ((DEBUG_INFO, " SccEmmcTraceLength = %x\n", ScsConfig->SccEmmcTraceLength));
  DEBUG ((DEBUG_INFO, " SdioTxCmdCntl        = %x\n", ScsConfig->SdioRegDllConfig.TxCmdCntl));
  DEBUG ((DEBUG_INFO, " SdioTxDataCntl1      = %x\n", ScsConfig->SdioRegDllConfig.TxDataCntl1));
  DEBUG ((DEBUG_INFO, " SdioTxDataCntl2      = %x\n", ScsConfig->SdioRegDllConfig.TxDataCntl2));
  DEBUG ((DEBUG_INFO, " SdioRxCmdDataCntl1   = %x\n", ScsConfig->SdioRegDllConfig.RxCmdDataCntl1));
  DEBUG ((DEBUG_INFO, " SdioRxStrobeCntl     = %x\n", ScsConfig->SdioRegDllConfig.RxCmdDataCntl2));
  DEBUG ((DEBUG_INFO, " SdcardTxCmdCntl      = %x\n", ScsConfig->SdcardRegDllConfig.TxCmdCntl));
  DEBUG ((DEBUG_INFO, " SdcardTxDataCntl1    = %x\n", ScsConfig->SdcardRegDllConfig.TxDataCntl1));
  DEBUG ((DEBUG_INFO, " SdcardTxDataCntl2    = %x\n", ScsConfig->SdcardRegDllConfig.TxDataCntl2));
  DEBUG ((DEBUG_INFO, " SdcardRxCmdDataCntl1 = %x\n", ScsConfig->SdcardRegDllConfig.RxCmdDataCntl1));
  DEBUG ((DEBUG_INFO, " SdcardRxStrobeCntl   = %x\n", ScsConfig->SdcardRegDllConfig.RxStrobeCntl));
  DEBUG ((DEBUG_INFO, " SdcardRxCmdDataCntl1 = %x\n", ScsConfig->SdcardRegDllConfig.RxCmdDataCntl2));
  DEBUG ((DEBUG_INFO, " EmmcTxCmdCntl        = %x\n", ScsConfig->EmmcRegDllConfig.TxCmdCntl));
  DEBUG ((DEBUG_INFO, " EmmcTxDataCntl1      = %x\n", ScsConfig->EmmcRegDllConfig.TxDataCntl1));
  DEBUG ((DEBUG_INFO, " EmmcTxDataCntl2      = %x\n", ScsConfig->EmmcRegDllConfig.TxDataCntl2));
  DEBUG ((DEBUG_INFO, " EmmcRxCmdDataCntl1   = %x\n", ScsConfig->EmmcRegDllConfig.RxCmdDataCntl1));
  DEBUG ((DEBUG_INFO, " EmmcRxStrobeCntl     = %x\n", ScsConfig->EmmcRegDllConfig.RxStrobeCntl));
  DEBUG ((DEBUG_INFO, " EmmcRxCmdDataCntl2   = %x\n", ScsConfig->EmmcRegDllConfig.RxCmdDataCntl2));
  DEBUG ((DEBUG_INFO, " EmmcMasterSwCntl     = %x\n", ScsConfig->EmmcRegDllConfig.MasterSwCntl));
}


/**
  Print SC_VTD_CONFIG and serial out.

  @param[in] VtdConfig         Pointer to a SC_VTD_CONFIG that provides the platform setting

**/

VOID
PrintVtdConfig (
  IN CONST SC_VTD_CONFIG                *VtdConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ Vtd Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " VtdEnable    = %x\n", VtdConfig->VtdEnable));

  return;
}


/**
  Print SC_GENERAL_CONFIG and serial out.

  @param[in] ScConfig         Pointer to a SC_GENERAL_CONFIG that provides the platform setting
**/
VOID
PrintGeneralConfig (
  IN CONST SC_GENERAL_CONFIG   *ScGeneralConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ General Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " SubSystemVendorId = %x\n", ScGeneralConfig->SubSystemVendorId));
  DEBUG ((DEBUG_INFO, " SubSystemId       = %x\n", ScGeneralConfig->SubSystemId));
  DEBUG ((DEBUG_INFO, " AcpiBase          = %x\n", ScGeneralConfig->AcpiBase));
  DEBUG ((DEBUG_INFO, " PmcBase           = %x\n", ScGeneralConfig->PmcBase));
  DEBUG ((DEBUG_INFO, " P2sbBase          = %x\n", ScGeneralConfig->P2sbBase));
  DEBUG ((DEBUG_INFO, " Crid              = %x\n", ScGeneralConfig->Crid));
  DEBUG ((DEBUG_INFO, " ResetSelect       = %x\n", ScGeneralConfig->ResetSelect));
}


/**
  Print SC_ISH_CONFIG and serial out.

  @param[in] IshConfig                  Pointer to a SC_ISH_CONFIG that provides the platform setting

**/
VOID
PrintIshConfig (
  IN CONST SC_ISH_CONFIG  *IshConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ ISH Config ------------------\n"));
  DEBUG ((DEBUG_INFO, "Enable = %x\n", IshConfig->Enable));
}


/**
  Print SC_FLASH_PROTECTION_CONFIG and serial out.

  @param[in] FlashProtectConfig  Pointer to a SC_FLASH_PROTECTION_CONFIG that provides the platform setting

**/
VOID
PrintFlashProtectionConfig (
  IN CONST SC_FLASH_PROTECTION_CONFIG   *FlashProtectConfig
  )
{
  UINT32 Index;

  DEBUG ((DEBUG_INFO, "------------------ SC Flash Protection Config ------------------\n"));
  for (Index = 0; Index < SC_FLASH_PROTECTED_RANGES; ++Index) {
    DEBUG ((DEBUG_INFO, " WriteProtectionEnable[%d] = %x\n", Index, FlashProtectConfig->ProtectRange[Index].WriteProtectionEnable));
    DEBUG ((DEBUG_INFO, " ReadProtectionEnable[%d]  = %x\n", Index, FlashProtectConfig->ProtectRange[Index].ReadProtectionEnable));
    DEBUG ((DEBUG_INFO, " ProtectedRangeLimit[%d]   = %x\n", Index, FlashProtectConfig->ProtectRange[Index].ProtectedRangeLimit));
    DEBUG ((DEBUG_INFO, " ProtectedRangeBase[%d]    = %x\n", Index, FlashProtectConfig->ProtectRange[Index].ProtectedRangeBase));
  }
}


/**
  Print SC_DCI_CONFIG and serial out.

  @param[in] IshConfig                  Pointer to a SC_DCI_CONFIG that provides the platform setting

**/
VOID
PrintDciConfig (
  IN CONST SC_DCI_CONFIG  *DciConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ DCI Config ------------------\n"));
  DEBUG ((DEBUG_INFO, "DciEn         = %x\n", DciConfig->DciEn));
  DEBUG ((DEBUG_INFO, "DciAutoDetect = %x\n", DciConfig->DciAutoDetect));
}


/**
  Print SC_P2SB_CONFIG and serial out.

  @param[in] IshConfig                  Pointer to a SC_DCI_CONFIG that provides the platform setting

**/
VOID
PrintP2sbConfig (
  IN CONST SC_P2SB_CONFIG  *P2sbConfig
  )
{
  DEBUG ((DEBUG_INFO, "------------------ P2SB Config ------------------\n"));
  DEBUG ((DEBUG_INFO, " P2sbUnhide     = %x\n", P2sbConfig->P2sbUnhide));
}


/**
  Print whole SC config blocks and serial out.

  @param[in] ScPolicyPpi    The RC Policy PPI instance

**/
VOID
ScPrintPolicyPpi (
  IN  SC_POLICY_PPI     *ScPolicyPpi
  )
{
  EFI_STATUS                 Status;
  SC_GENERAL_CONFIG          *ScGeneralConfig;
  SC_SATA_CONFIG             *SataConfig;
  SC_PCIE_CONFIG             *PcieRpConfig;
  SC_SMBUS_CONFIG            *SmbusConfig;
  SC_HPET_CONFIG             *HpetConfig;
  SC_IOAPIC_CONFIG           *IoApicConfig;
  SC_USB_CONFIG              *UsbConfig;
  SC_HDAUDIO_CONFIG          *HdaConfig;
  SC_GMM_CONFIG              *GmmConfig;
  SC_PM_CONFIG               *PmConfig;
  SC_LOCK_DOWN_CONFIG        *LockDownConfig;
  SC_LPC_SIRQ_CONFIG         *SerialIrqConfig;
  SC_LPSS_CONFIG             *LpssConfig;
  SC_SCS_CONFIG              *ScsConfig;
  SC_VTD_CONFIG              *VtdConfig;
  SC_ISH_CONFIG              *IshConfig;
  SC_FLASH_PROTECTION_CONFIG *FlashProtectionConfig;
  SC_DCI_CONFIG              *DciConfig;
  SC_P2SB_CONFIG             *P2sbConfig;
  SC_INTERRUPT_CONFIG        *InterruptConfig;

  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gScGeneralConfigGuid, (VOID *) &ScGeneralConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gSataConfigGuid, (VOID *) &SataConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gHpetConfigGuid, (VOID *) &HpetConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gSmbusConfigGuid, (VOID *) &SmbusConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gIoApicConfigGuid, (VOID *) &IoApicConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gHdAudioConfigGuid, (VOID *) &HdaConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gGmmConfigGuid, (VOID *) &GmmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gPmConfigGuid, (VOID *) &PmConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gSerialIrqConfigGuid, (VOID *) &SerialIrqConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gLpssConfigGuid, (VOID *) &LpssConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gScsConfigGuid, (VOID *) &ScsConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gVtdConfigGuid, (VOID *) &VtdConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gIshConfigGuid, (VOID *) &IshConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gFlashProtectionConfigGuid, (VOID *) &FlashProtectionConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gDciConfigGuid, (VOID *) &DciConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gP2sbConfigGuid, (VOID *) &P2sbConfig);
  ASSERT_EFI_ERROR (Status);
  Status = GetConfigBlock ((VOID *) ScPolicyPpi, &gInterruptConfigGuid, (VOID *) &InterruptConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "------------------------ SC Print Policy Start ------------------------\n"));
  PrintGeneralConfig (ScGeneralConfig);
  PrintSataConfig (SataConfig);
  PrintPcieConfig (PcieRpConfig);
  PrintHpetConfig (HpetConfig);
  PrintSmbusConfig (SmbusConfig);
  PrintIoApicConfig (IoApicConfig);
  PrintUsbConfig (UsbConfig);
  PrintHdAudioConfig (HdaConfig);
  PrintGmmConfig (GmmConfig);
  PrintPmConfig (PmConfig);
  PrintLockDownConfig (LockDownConfig);
  PrintSerialIrqConfig (SerialIrqConfig);
  PrintLpssConfig (LpssConfig);
  PrintScsConfig (ScsConfig);
  PrintVtdConfig (VtdConfig);
  PrintIshConfig (IshConfig);
  PrintFlashProtectionConfig (FlashProtectionConfig);
  PrintDciConfig (DciConfig);
  PrintP2sbConfig (P2sbConfig);
  PrintInterruptConfig (InterruptConfig);

  DEBUG ((DEBUG_INFO, "------------------------ SC Print Policy End ------------------------\n"));
}

