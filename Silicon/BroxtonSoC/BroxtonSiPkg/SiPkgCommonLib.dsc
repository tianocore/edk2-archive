## @file
#  Component description file for the Broxton RC both Pei and Dxe libraries DSC file.
#
#  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution. The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

#
# RC Common Library
#

#
# Common
#
  AslUpdateLib|$(PLATFORM_SI_PACKAGE)/Library/DxeAslUpdateLib/DxeAslUpdateLib.inf
  ConfigBlockLib|$(PLATFORM_SI_PACKAGE)/Library/BaseConfigBlockLib/BaseConfigBlockLib.inf
  BaseIpcLib|$(PLATFORM_SI_PACKAGE)/Library/PmcIpcLib/BaseIpcLib.inf
  GpioLib|$(PLATFORM_SI_PACKAGE)/Library/GpioLib/GpioLib.inf
  MmPciLib|$(PLATFORM_SI_PACKAGE)/Library/PeiDxeSmmMmPciLib/PeiDxeSmmMmPciLib.inf
  SideBandLib|$(PLATFORM_SI_PACKAGE)/Library/SideBandLib/SideBandLib.inf
  SteppingLib|$(PLATFORM_SI_PACKAGE)/Library/SteppingLib/SteppingLib.inf
  CseVariableStorageLib|$(PLATFORM_SI_PACKAGE)/Library/Private/BaseCseVariableStorageLib/BaseCseVariableStorageLib.inf
  VariableStorageSelectorLib|$(PLATFORM_SI_PACKAGE)/Library/BaseVariableStorageSelectorLib/BaseVariableStorageSelectorLib.inf
  VariableNvmStorageLib|$(PLATFORM_SI_PACKAGE)/Library/BaseVariableNvmStorageLib/BaseVariableNvmStorageLib.inf

#
# SC
#
  PeiDxeSmmScPciExpressHelpersLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/PeiDxeSmmScPciExpressHelpersLib/PeiDxeSmmScPciExpressHelpersLib.inf
  ScPlatformLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/ScPlatformLib/ScPlatformLib.inf
  UsbCommonLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/PeiDxeUsbCommonLib/PeiDxeUsbCommonLib.inf
  ScAslUpdateLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/SampleCode/Library/AslUpdate/Dxe/ScAslUpdateLib.inf
  ScHdaLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/DxeScHdaLib/DxeScHdaLib.inf
  DxeVtdLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/DxeVtdLib/DxeVtdLib.inf

#
# NC
#
  DxeSaPolicyLib|$(PLATFORM_SI_PACKAGE)/NorthCluster/Library/DxeSaPolicyLib/DxeSaPolicyLib.inf

#
# CPU
#
  CpuCommonLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/Private/PeiDxeSmmCpuCommonLib/PeiDxeSmmCpuCommonLib.inf
  CpuPlatformLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiDxeSmmCpuPlatformLib/PeiDxeSmmCpuPlatformLib.inf
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf

#
# TXE
#
  HeciInitLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/Private/PeiDxeHeciInitLib/PeiDxeHeciInitLib.inf
  SeCChipsetLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/PeiDxeSeCChipsetLib/PeiDxeSeCChipsetLib.inf
  Heci2PowerManagementLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/BaseHeci2PowerManagementNullLib/BaseHeci2PowerManagementNullLib.inf

