## @file
#  Platform Components for IA32 Description.
#
#  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
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

  $(PLATFORM_SI_PACKAGE)/Cpu/SecCore/Vtf0SecCore.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      PlatformSecLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformSecLib/Vtf0PlatformSecLib.inf
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x00
      gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x0
  }

  $(PLATFORM_PACKAGE_COMMON)/Console/PlatformDebugPei/PlatformDebugPei.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/MdeModulePkg/Core/Pei/PeiMain.inf {
  !if $(TOOL_CHAIN_TAG) == GCC47
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0
      gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <BuildOptions>
      GCC:*_*_*_CC_FLAGS = -DMDEPKG_NDEBUG
  !else
    !if $(TARGET) == DEBUG
      <PcdsFixedAtBuild>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2E
    !endif
      <PcdsPatchableInModule>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
      <LibraryClasses>
        DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  !endif
  }

!if $(PERFORMANCE_ENABLE) == TRUE
  MdeModulePkg/Universal/ReportStatusCodeRouter/Pei/ReportStatusCodeRouterPei.inf
!endif

  $(PLATFORM_SI_PACKAGE)/VariableStorage/Pei/CseVariableStoragePei/CseVariableStoragePei.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }

  $(PLATFORM_SI_PACKAGE)/SampleCode/MdeModulePkg/Universal/Variable/Pei/VariablePei.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
  $(PLATFORM_PACKAGE_COMMON)/Console/MonoStatusCode/MonoStatusCode.inf {
  !if $(TARGET) == DEBUG
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2E
  !endif
  }

  $(PLATFORM_PACKAGE_COMMON)/PlatformSettings/PlatformPreMemPei/PlatformPreMemPei.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_NAME)/Board/MinnowBoard3/BoardInitPreMem/BoardInitPreMem.inf
      NULL|$(PLATFORM_NAME)/Board/LeafHill/BoardInitPreMem/BoardInitPreMem.inf
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
      CpuPolicyLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiCpuPolicyLibPreMem/PeiCpuPolicyLibPreMem.inf
    <BuildOptions>
      !if $(BOOT_GUARD_ENABLE) == TRUE
        *_*_IA32_CC_FLAGS = -DBOOT_GUARD_ENABLE=1
      !else
        *_*_IA32_CC_FLAGS = -DBOOT_GUARD_ENABLE=0
      !endif
  !if $(TOOL_CHAIN_TAG) == GCC47
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0
      gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <BuildOptions>
      GCC:*_*_*_CC_FLAGS = -DMDEPKG_NDEBUG
  !else
    !if $(TARGET) == DEBUG
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2E
      gPlatformModuleTokenSpaceGuid.PcdFlashFvIBBMBase|0xFEF30000
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046

    !endif
  !endif
  }

  $(PLATFORM_PACKAGE_COMMON)/PlatformSettings/PlatformPostMemPei/PlatformPostMemPei.inf {
     <LibraryClasses>
       NULL|$(PLATFORM_NAME)/Board/MinnowBoard3/BoardInitPostMem/BoardInitPostMem.inf
       NULL|$(PLATFORM_NAME)/Board/LeafHill/BoardInitPostMem/BoardInitPostMem.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x803805c6
  }

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/IntelFsp2WrapperPkg/FspmWrapperPeim/FspmWrapperPeim.inf{
  <PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    # this is just the FSP-M s base, need cleanup soon
    gIntelFsp2WrapperTokenSpaceGuid.PcdFspmBaseAddress|$(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE) + $(FSP_RAM_DATA_SIZE) + $(FSP_EMP_DATA_SIZE) + $(BLD_IBBM_SIZE)
    gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspBase|$(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE) + $(FSP_RAM_DATA_SIZE) + $(FSP_EMP_DATA_SIZE) + $(BLD_IBBM_SIZE)

    gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspSize|$(FSP_IBBM_SIZE)
    gIntelFsp2WrapperTokenSpaceGuid.PcdTemporaryRamBase|$(CAR_BASE_ADDRESS)

    gIntelFsp2WrapperTokenSpaceGuid.PcdFspStackBase|$(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE)
    gIntelFsp2WrapperTokenSpaceGuid.PcdFspStackSize|$(FSP_RAM_DATA_SIZE)
    gIntelFspPkgTokenSpaceGuid.PcdGlobalDataPointerAddress|0xFED00148
  }

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/IntelFsp2WrapperPkg/FspsWrapperPeim/FspsWrapperPeim.inf{
  <PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    # this is just the FSP-M s base, need cleanup soon
    gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspBase|$(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE) + $(FSP_RAM_DATA_SIZE) + $(FSP_EMP_DATA_SIZE) + $(BLD_IBBM_SIZE)

    gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspSize|$(FSP_IBBM_SIZE)

    gIntelFsp2WrapperTokenSpaceGuid.PcdTemporaryRamBase|$(CAR_BASE_ADDRESS)
    gIntelFspPkgTokenSpaceGuid.PcdGlobalDataPointerAddress|0xFED00148
  }

  #No use of SPI PPI for now
  $(PLATFORM_SI_PACKAGE)/SouthCluster/SmmControl/Pei/SmmControl.inf

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/MdeModulePkg/Universal/PCD/Pei/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
   !if $(TARGET) == DEBUG
      <PcdsFixedAtBuild>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x26
   !endif
  }

  UefiCpuPkg/Universal/Acpi/S3Resume2Pei/S3Resume2Pei.inf

  $(PLATFORM_PACKAGE_COMMON)/Compatibility/AcpiVariableHobOnSmramReserveHobThunk/AcpiVariableHobOnSmramReserveHobThunk.inf
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf

  MdeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
!if $(LZMA_ENABLE) == TRUE
    NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
!endif
  }

!if $(FTPM_ENABLE) == TRUE
  SecurityPkg/Tcg/Tcg2Pei/Tcg2Pei.inf {
    <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterPei.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE_COMMON)/Library/Tpm2DeviceLibPtp/Tpm2InstanceLibPtt.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterPei.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
      PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/PeiCryptLib.inf
  }
!endif

!if $(TPM12_ENABLE) == TRUE
  SecurityPkg/Tcg/TcgPei/TcgPei.inf
!endif

!if $(ACPI50_ENABLE) == TRUE
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTablePei/FirmwarePerformancePei.inf {
    <LibraryClasses>
      TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformTscTimerLib/PeiTscTimerLib.inf
  }
!endif

!if $(INTEL_FPDT_ENABLE) == TRUE
  $(PLATFORM_PACKAGE_COMMON)/FpdtPei/FpdtPei.inf
!endif

