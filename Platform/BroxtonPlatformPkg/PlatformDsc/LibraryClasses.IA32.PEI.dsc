## @file
#  IA32 PEI Library Classes Description.
#
#  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
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
  # UEFI & PI
  #
  S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf

  #
  # Common
  #
  PeiPlatformConfigUpdateLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiPlatformConfigUpdateLib/PeiPlatformConfigUpdateLib.inf


  #
  # PEI phase common
  #
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/PeiExtractGuidedSectionLib/PeiExtractGuidedSectionLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf
  PeiVariableCacheLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiVariableCacheLib/PeiVariableCacheLib.inf
  SeCUmaLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/Private/PeiSeCUma/SeCUma.inf
  Heci2PowerManagementLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/BaseHeci2PowerManagementNullLib/BaseHeci2PowerManagementNullLib.inf

!if $(FTPM_ENABLE) == TRUE
   Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterPei.inf
!endif
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  SerialPortLib|$(PLATFORM_PACKAGE_COMMON)/Library/BaseSerialPortLib/BaseSerialPortLib.inf
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SecPeiDebugAgentLib.inf 

!else
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf

!endif

  DebugLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiDebugLib/PeiDebugLib.inf

  FspCommonLib|IntelFsp2Pkg/Library/BaseFspCommonLib/BaseFspCommonLib.inf
  FspSwitchStackLib|IntelFsp2Pkg/Library/BaseFspSwitchStackLib/BaseFspSwitchStackLib.inf
# FSP platform sample
  FspWrapperPlatformLib|$(PLATFORM_PACKAGE_COMMON)/FspSupport/Library/BaseFspPlatformInfoLibSample/BaseFspWrapperPlatformLibSample.inf
  FspPlatformSecLib|$(PLATFORM_PACKAGE_COMMON)/SampleCode/IntelFsp2WrapperPkg/Library/SecPeiFspPlatformSecLibSample/SecPeiFspPlatformSecLibSample.inf
  FspWrapperHobProcessLib|$(PLATFORM_PACKAGE_COMMON)/FspSupport/Library/PeiFspHobProcessLib/PeiFspHobProcessLib.inf
  FspPolicyInitLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiFspPolicyInitLib/PeiFspPolicyInitLib.inf
  FspWrapperPlatformResetLib|$(PLATFORM_PACKAGE_COMMON)/FspSupport/Library/PeiFspWrapperPlatformResetLib/PeiFspWrapperPlatformResetLib.inf

  BasePlatformCmosLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformCmosLib/PlatformCmosLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/PeiTcg2PhysicalPresenceLib/PeiTcg2PhysicalPresenceLib.inf

