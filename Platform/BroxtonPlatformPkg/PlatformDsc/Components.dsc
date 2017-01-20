## @file
#  Platform Components Description.
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

  #
  # EDK II Related Platform codes
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046
    <LibraryClasses>
  !if $(DXE_CRC32_SECTION_ENABLE) == TRUE
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
  !endif
  !if $(LZMA_ENABLE) == TRUE
      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  !endif
  }
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf
  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf

  $(PLATFORM_PACKAGE_COMMON)/Console/PlatformStatusCodeHandler/RuntimeDxe/PlatformStatusCodeHandlerRuntimeDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Console/PlatformStatusCodeHandler/RuntimeDxe/PlatformPort80HandlerRuntimeDxe.inf

  MdeModulePkg/Universal/ReportStatusCodeRouter/Smm/ReportStatusCodeRouterSmm.inf
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf # Use BaseDebugLibNull for resolve size issue
  !if $(SECURE_BOOT_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeImageVerificationLib/DxeImageVerificationLib.inf
  !endif
  !if $(USER_IDENTIFICATION_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeDeferImageLoadLib/DxeDeferImageLoadLib.inf
  !endif
  !if $(TPM12_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeTpmMeasureBootLib/DxeTpmMeasureBootLib.inf
  !endif
  !if $(FTPM_ENABLE) == TRUE
      NULL|SecurityPkg/Library/DxeTpm2MeasureBootLib/DxeTpm2MeasureBootLib.inf
  !endif
  }

    !if $(SMM_VARIABLE_ENABLE)
      $(PLATFORM_SI_PACKAGE)/VariableStorage/Dxe/CseVariableStorageSmmRuntimeDxe/CseVariableStorageSmmRuntimeDxe.inf {
        <PcdsPatchableInModule>
          gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
      }
    !else
      $(PLATFORM_SI_PACKAGE)/VariableStorage/Dxe/CseVariableStorageSmmRuntimeDxe/CseVariableStorageRuntimeDxe.inf {
        <PcdsPatchableInModule>
          gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
      }
    !endif



  $(PLATFORM_SI_PACKAGE)/SiInit/Dxe/SiInitDxe.inf
  MdeModulePkg/Universal/Metronome/Metronome.inf

  IntelFrameworkModulePkg/Universal/Acpi/AcpiS3SaveDxe/AcpiS3SaveDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xF0000043
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    <LibraryClasses>
!if $(TARGET) != RELEASE
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
    <BuildOptions>
      ICC:*_*_*_CC_FLAGS = /D MDEPKG_NDEBUG
      GCC:*_*_*_CC_FLAGS = -D MDEPKG_NDEBUG
  }

  IntelFrameworkModulePkg/Universal/BdsDxe/BdsDxe.inf {
    <LibraryClasses>
      OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
      IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
      BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE_COMMON)/Library/Tpm2DeviceLibPtp/Tpm2InstanceLibPtt.inf
  }

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/IntelFsp2WrapperPkg/FspNotifyDxe/FspNotifyDxe.inf

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf

  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

!if $(SMM_VARIABLE_ENABLE)
  $(PLATFORM_SI_PACKAGE)/SampleCode/MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmmRuntimeDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
    <LibraryClasses>
!if $(SECURE_BOOT_ENABLE) == FALSE
      TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
!endif
  }
  $(PLATFORM_SI_PACKAGE)/SampleCode/MdeModulePkg/Universal/Variable/RuntimeDxe/VariableSmm.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
    <LibraryClasses>
      VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
      PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
      AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
      AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
!endif
  }
!else
  $(PLATFORM_SI_PACKAGE)/SampleCode/MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000103
    <LibraryClasses>
      VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
      NULL|MdeModulePkg/Library/VarCheckUefiLib/VarCheckUefiLib.inf
      NULL|MdeModulePkg/Library/VarCheckHiiLib/VarCheckHiiLib.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
      PlatformSecureLib|SecurityPkg/Library/PlatformSecureLibNull/PlatformSecureLibNull.inf
      AuthVariableLib|SecurityPkg/Library/AuthVariableLib/AuthVariableLib.inf
!else
      AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
      TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
!endif
  }
!endif #SMM_VARIABLE_ENABLE

  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  PcAtChipsetPkg/PcatRealTimeClockRuntimeDxe/PcatRealTimeClockRuntimeDxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

  $(PLATFORM_PACKAGE_COMMON)/PlatformSettings/PlatformSetupDxe/PlatformSetupDxe.inf {
    <LibraryClasses>
      NULL|$(PLATFORM_NAME)/Board/MinnowBoard3/BoardInitDxe/BoardInitDxe.inf
      NULL|$(PLATFORM_NAME)/Board/LeafHill/BoardInitDxe/BoardInitDxe.inf
  }

!if $(DATAHUB_ENABLE) == TRUE
  IntelFrameworkModulePkg/Universal/DataHubDxe/DataHubDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|0
  }
!endif

  IntelFrameworkModulePkg/Universal/StatusCode/DatahubStatusCodeHandlerDxe/DatahubStatusCodeHandlerDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/SampleCode/MdeModulePkg/Universal/MemoryTest/NullMemoryTestDxe/NullMemoryTestDxe.inf

  PcAtChipsetPkg/HpetTimerDxe/HpetTimerDxe.inf

  $(PLATFORM_SI_PACKAGE)/SouthCluster/SmmControl/RuntimeDxe/SmmControl.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/Reset/RuntimeDxe/ScReset.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/ScSmiDispatcher/Smm/ScSmiDispatcher.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/ScInit/Smm/ScInitSmm.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/SampleCode/BiosWriteProtect/Smm/ScBiosWriteProtect.inf

  $(PLATFORM_SI_PACKAGE)/NorthCluster/PciHostBridge/Dxe/PciHostBridge.inf
  $(PLATFORM_SI_PACKAGE)/NorthCluster/SaInit/Dxe/SaInit.inf
  $(PLATFORM_SI_PACKAGE)/NorthCluster/AcpiTables/SaAcpiTables.inf

  $(PLATFORM_PACKAGE_COMMON)/DptfDxe/DptfDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/PnpDxe/PnpDxe.inf

!if $(SEC_ENABLE) == TRUE
  $(PLATFORM_PACKAGE_COMMON)/SeCPolicyInitDxe/SeCPolicyInitDxe.inf
!endif

!if $(FTPM_ENABLE) == TRUE
  SecurityPkg/Tcg/MemoryOverwriteControl/TcgMor.inf
  SecurityPkg/Tcg/MemoryOverwriteRequestControlLock/TcgMorLockSmm.inf

  SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf {
    <LibraryClasses>
      Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
      NULL|SecurityPkg/Library/Tpm2DeviceLibDTpm/Tpm2InstanceLibDTpm.inf
      NULL|$(PLATFORM_PACKAGE_COMMON)/Library/Tpm2DeviceLibPtp/Tpm2InstanceLibPtt.inf
      HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
      NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
  }

  $(PLATFORM_PACKAGE_COMMON)/SampleCode/SecurityPkg/Tcg/Tcg2Smm/Tcg2Smm.inf
!endif

!if $(TPM12_ENABLE) == TRUE
  SecurityPkg/Tcg/TcgDxe/TcgDxe.inf
  SecurityPkg/Tcg/TcgSmm/TcgSmm.inf
!endif

  #
  # EDK II Related Platform codes
  #
  $(PLATFORM_PACKAGE_COMMON)/PlatformSmm/PlatformSmm.inf
  $(PLATFORM_PACKAGE_COMMON)/PlatformSettings/PlatformInfoDxe/PlatformInfoDxe.inf

  $(PLATFORM_PACKAGE_COMMON)/PlatformSettings/PlatformDxe/PlatformDxe.inf

  $(PLATFORM_PACKAGE_COMMON)/SaveMemoryConfigDxe/SaveMemoryConfigDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Features/S3/SmramSaveInfoHandlerSmm/SmramSaveInfoHandlerSmm.inf

!if $(GOP_DRIVER_ENABLE) == TRUE
  $(PLATFORM_PACKAGE_COMMON)/Console/PlatformGopPolicyDxe/PlatformGopPolicyDxe.inf
!endif

  #
  # SMM
  #
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf

  UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf
  UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf
  $(PLATFORM_PACKAGE_COMMON)/Compatibility/SmmSwDispatch2OnSmmSwDispatchThunk/SmmSwDispatch2OnSmmSwDispatchThunk.inf
  $(PLATFORM_PACKAGE_COMMON)/Compatibility/SmmSxDispatch2OnSmmSxDispatchThunk/SmmSxDispatch2OnSmmSxDispatchThunk.inf

!if $(NVM_VARIABLE_ENABLE) == TRUE
  $(PLATFORM_SI_PACKAGE)/Txe/Heci/Smm/HeciSmm.inf
  $(PLATFORM_SI_PACKAGE)/Txe/Heci/Smm/HeciSmmRuntimeDxe.inf
!endif

  #
  # ACPI
  #
  MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/BootScriptExecutorDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Features/S3/BootScriptSaveDxe/BootScriptSaveDxe.inf
  IntelFrameworkModulePkg/Universal/Acpi/AcpiSupportDxe/AcpiSupportDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Acpi/AcpiTablesPCAT/PlatformSsdt/PlatformSsdt.inf

!if $(DTS_ENABLE) == TRUE

!endif

  $(PLATFORM_PACKAGE_COMMON)/Acpi/AcpiTablesPCAT/AcpiTables.inf
  $(PLATFORM_PACKAGE_COMMON)/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf

  #
  # PCI
  #
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

  #
  # LAN/Network
  #
!if $(NETWORK_ENABLE) == TRUE
  MdeModulePkg/Universal/Network/SnpDxe/SnpDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/DpcDxe/DpcDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/MnpDxe/MnpDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/ArpDxe/ArpDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/Dhcp4Dxe/Dhcp4Dxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/Ip4Dxe/Ip4Dxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/Mtftp4Dxe/Mtftp4Dxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/Tcp4Dxe/Tcp4Dxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }
  MdeModulePkg/Universal/Network/Udp4Dxe/Udp4Dxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  }

  !if $(NETWORK_IP6_ENABLE) == TRUE
     NetworkPkg/Ip6Dxe/Ip6Dxe.inf
     NetworkPkg/Dhcp6Dxe/Dhcp6Dxe.inf
     NetworkPkg/IpSecDxe/IpSecDxe.inf
     NetworkPkg/TcpDxe/TcpDxe.inf
     NetworkPkg/Udp6Dxe/Udp6Dxe.inf
     NetworkPkg/Mtftp6Dxe/Mtftp6Dxe.inf
  !endif
  !if $(NETWORK_IP6_ENABLE) == TRUE
     $(PLATFORM_PACKAGE_COMMON)/SampleCode/NetworkPkg/UefiPxeBcDxe/UefiPxeBcDxe.inf
  !else
     MdeModulePkg/Universal/Network/UefiPxeBcDxe/UefiPxeBcDxe.inf {
       <LibraryClasses>
         DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
       <PcdsPatchableInModule>
         gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
     }
  !endif
!endif
  #
  # ISA
  #
  $(PLATFORM_PACKAGE_COMMON)/Console/Wpce791/SiO791.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaBusDxe/IsaBusDxe.inf
  IntelFrameworkModulePkg/Bus/Isa/IsaIoDxe/IsaIoDxe.inf

  #
  # LPSS UART Serial
  #
  $(PLATFORM_PACKAGE_COMMON)/Console/LpssUartSerialDxe/LpssUartSerialDxe.inf

  #
  # SDIO
  #
  $(PLATFORM_SI_PACKAGE)/SouthCluster/Sdio/Dxe/MMC/MmcHostDxe/MmcHostDxe.inf

  $(PLATFORM_SI_PACKAGE)/SouthCluster/Sdio/Dxe/MMC/MmcMediaDeviceDxe/MmcMediaDeviceDxe.inf
  
  $(PLATFORM_SI_PACKAGE)/SouthCluster/Sdio/Dxe/SD/SdControllerDxe/SdControllerDxe.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/Sdio/Dxe/SD/SdMediaDeviceDxe/SdMediaDeviceDxe.inf
    

  !if $(ACPI50_ENABLE) == TRUE
    MdeModulePkg/Universal/SmmCommunicationBufferDxe/SmmCommunicationBufferDxe.inf
    MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf {
      <LibraryClasses>
      TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformTscTimerLib/DxeTscTimerLib.inf
    }
    MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf {
      <LibraryClasses>
      TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformTscTimerLib/DxeTscTimerLib.inf
    }
  !endif

  #
  # IDE/SCSI/AHCI/NVMe
  #
  IntelFrameworkModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  $(PLATFORM_SI_PACKAGE)/SouthCluster/SataController/Dxe/SataController.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf

  # This is used for the ScsiPassThru driver (repurposed for UFS in BXT)
  !if $(SCSI_ENABLE) == TRUE
    MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf {
      <PcdsPatchableInModule>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046 #0
      <PcdsFixedAtBuild>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    }
    MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf {
      <PcdsPatchableInModule>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000046 #0x80000007
      <PcdsFixedAtBuild>
        gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
    }
  !endif
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

  #
  # Console
  #
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  IntelFrameworkModulePkg/Universal/Console/VgaClassDxe/VgaClassDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/SampleCode/MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf #Use BaseDebugLibNull for resolve size issue
  }

  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Console/GraphicsOutputDxe/GraphicsOutputDxe.inf

  #
  # USB
  #
!if $(USB_ENABLE) == TRUE && $(USB_NATIVE_ENABLE) == TRUE
#  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
#  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
#  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
#  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
#  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
#  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
#  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf {
    <BuildOptions>
    !if $(SIMICS_ENABLE) == TRUE
      MSFT:*_*_*_CC_FLAGS  = $(C_PROJ_FLAGS) /GL- /D SIMICS_ENABLE
      INTEL:*_*_*_CC_FLAGS = $(C_PROJ_FLAGS) /GL- /D SIMICS_ENABLE
      GCC:*_*_*_CC_FLAGS   = $(C_PROJ_FLAGS) -O0  -DSIMICS_ENABLE
    !else
      MSFT:*_*_*_CC_FLAGS  = $(C_PROJ_FLAGS) /GL-
      INTEL:*_*_*_CC_FLAGS = $(C_PROJ_FLAGS) /GL-
      GCC:*_*_*_CC_FLAGS   = $(C_PROJ_FLAGS) -O0
    !endif
  }
!endif

  #
  # SMBIOS
  #
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  $(PLATFORM_PACKAGE_COMMON)/Features/Smbios/SmBiosMiscDxe/SmBiosMiscDxe.inf

  #
  # Legacy Modules
  #
  PcAtChipsetPkg/8259InterruptControllerDxe/8259.inf

  #
  # USB TypeC
  #
  $(PLATFORM_PACKAGE_COMMON)/Acpi/UsbTypeCDxe/UsbTypeCDxe.inf

