## @file
#  Library classes Description.
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
   # Entry point
   #
   PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
   PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
   DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
   UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
   UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
   DxeSmmDriverEntryPoint|IntelFrameworkPkg/Library/DxeSmmDriverEntryPoint/DxeSmmDriverEntryPoint.inf
   PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
   !if $(SOURCE_DEBUG_ENABLE) == TRUE
      PeCoffExtraActionLib|SourceLevelDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
      DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibSerialPort/DebugCommunicationLibSerialPort.inf
      PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
   !else
      PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
   !endif

   #
   # Common
   #
   SteppingLib|$(PLATFORM_SI_PACKAGE)/Library/SteppingLib/SteppingLib.inf
   SideBandLib|$(PLATFORM_SI_PACKAGE)/Library/SideBandLib/SideBandLib.inf
   GpioLib|$(PLATFORM_SI_PACKAGE)/Library/GpioLib/GpioLib.inf


   #
   # Basic
   #
   BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
   PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
   CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
   IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
   PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
   PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
   PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
   CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
   PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
   PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
   !if $(SSE2_ENABLE) == TRUE
      BaseMemoryLib|MdePkg/Library/BaseMemoryLibSse2/BaseMemoryLibSse2.inf
   !else
      BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
   !endif

   SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
   Tcg2PpVendorLib|SecurityPkg/Library/Tcg2PpVendorLibNull/Tcg2PpVendorLibNull.inf

   #
   # UEFI & PI
   #
   UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
   UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
   UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
   UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
   HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
   UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
   DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
   !if ($(LZMA_ENABLE) == FALSE)
   UefiDecompressLib|IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
   !else
   UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
   !endif
   PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
   PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
   DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
   DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
   UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
   UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
   GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
   PlatformBdsLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformBdsLib/PlatformBdsLib.inf
   NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
   DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf

   #
   # Framework
   #
   S3IoLib|MdePkg/Library/BaseS3IoLib/BaseS3IoLib.inf
   S3PciLib|MdePkg/Library/BaseS3PciLib/BaseS3PciLib.inf
   !if $(S3_ENABLE) == TRUE
      S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
   !else
      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
   !endif

   #
   # Generic Modules
   #
   CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
   OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
   LogoLib|$(PLATFORM_PACKAGE_COMMON)/Library/DxeLogoLib/DxeLogoLib.inf
   UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
   LegacyBootManagerLib|IntelFrameworkModulePkg/Library/LegacyBootManagerLib/LegacyBootManagerLib.inf
   SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
   SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
   IoApicLib|PcAtChipsetPkg/Library/BaseIoApicLib/BaseIoApicLib.inf
   CmosAccessLib|$(PLATFORM_PACKAGE_COMMON)/Library/BaseCmosAccessLib/BaseCmosAccessLib.inf
   BaseCmosAccessLib|$(PLATFORM_PACKAGE_COMMON)/Library/BaseCmosAccessLib/BaseCmosAccessLib.inf
   PostCodeLib|$(PLATFORM_PACKAGE_COMMON)/Library/BasePostCodeLibPort80Ex/BasePostCodeLibPort80Ex.inf
   DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
   !if $(USB_ENABLE) == TRUE
      !if $(USB_NATIVE_ENABLE) == TRUE
         UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
      !endif
   !endif
   !if $(SCSI_ENABLE) == TRUE
      UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
   !endif
   !if $(NETWORK_ENABLE) == TRUE
      NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
      IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
      UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
      TcpIoLib|MdeModulePkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
      DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
   !endif
   !if $(S3_ENABLE) == TRUE
      S3Lib|IntelFrameworkModulePkg/Library/PeiS3Lib/PeiS3Lib.inf
   !endif
   !if $(RECOVERY_ENABLE) == TRUE
      RecoveryLib|IntelFrameworkModulePkg/Library/PeiRecoveryLib/PeiRecoveryLib.inf
   !endif
   !if $(CAPSULE_ENABLE) == TRUE
      CapsuleLib|IntelFrameworkModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
   !else
      CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
   !endif

   #
   # ICH
   #
   SmbusLib|$(PLATFORM_PACKAGE_COMMON)/Library/SmbusLib/SmbusLib.inf
   ScAslUpdateLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/SampleCode/Library/AslUpdate/Dxe/ScAslUpdateLib.inf

   #
   # Platform
   #
   ResetSystemLib|$(PLATFORM_PACKAGE_COMMON)/Library/ResetSystemLib/ResetSystemLib.inf
   PlatformCmosLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformCmosLib/PlatformCmosLib.inf
   PlatformPostCodeMapLib|$(PLATFORM_PACKAGE_COMMON)/Library/BasePlatformPostCodeMapLib/BasePlatformPostCodeMapLib.inf
   TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/IntelScAcpiTimerLib/IntelScAcpiTimerLib.inf

   BaseIpcLib|$(PLATFORM_SI_PACKAGE)/Library/PmcIpcLib/BaseIpcLib.inf

   PeiPolicyInitLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiPolicyInitLib/PeiPolicyInitLib.inf
   PeiPolicyUpdateLib|$(PLATFORM_PACKAGE_COMMON)/Library/PeiPolicyUpdateLib/PeiPolicyUpdateLib.inf
   DxePolicyUpdateLib|$(PLATFORM_PACKAGE_COMMON)/Library/DxePolicyUpdateLib/DxePolicyUpdateLib.inf
   DxeSmbiosProcessorLib|$(PLATFORM_PACKAGE_COMMON)/Library/DxeSmbiosProcessorLib/DxeSmbiosProcessorLib.inf

   #
   # Misc
   #
   CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
   MonoStatusCodeLib|$(PLATFORM_PACKAGE_COMMON)/Console/MonoStatusCode/MonoStatusCode.inf
   PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
   PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
   RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf

   !if $(TARGET) == RELEASE
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
   !else
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
   !endif

   !if $(SPI_SERIAL_OUT) == TRUE
     SerialPortLib|$(PLATFORM_PACKAGE_COMMON)/Library/Emu/SerialPortLib/SerialPortLib.inf
   !else
     SerialPortLib|$(PLATFORM_PACKAGE_COMMON)/Library/BaseSerialPortLib/BaseSerialPortLibNoInit.inf
   !endif

   DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf

   PchSerialIoUartLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/PeiDxeSmmPchSerialIoUartLib/PeiDxeSmmPchSerialIoUartLib.inf
   PchSerialIoLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/PeiDxeSmmPchSerialIoLib/PeiDxeSmmPchSerialIoLib.inf
   SerialPortParameterLib|$(PLATFORM_PACKAGE_COMMON)/Library/BaseSerialPortParameterLibCmos/BaseSerialPortParameterLibCmos.inf

   BiosIdLib|$(PLATFORM_PACKAGE_COMMON)/Library/BiosIdLib/BiosIdLib.inf
   CpuIA32Lib|$(PLATFORM_PACKAGE_COMMON)/Library/CpuIA32Lib/CpuIA32Lib.inf
   StallSmmLib|$(PLATFORM_PACKAGE_COMMON)/Library/StallSmmLib/StallSmmLib.inf
   ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
   ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
   FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
   SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

   !if $(TPM12_ENABLE) == TRUE
     TpmCommLib|SecurityPkg/Library/TpmCommLib/TpmCommLib.inf
     Tpm12DeviceLib|SecurityPkg/Library/Tpm12DeviceLibDTpm/Tpm12DeviceLibDTpm.inf
     Tpm12CommandLib|SecurityPkg/Library/Tpm12CommandLib/Tpm12CommandLib.inf
   !endif
     TcgPpVendorLib|SecurityPkg/Library/TcgPpVendorLibNull/TcgPpVendorLibNull.inf

   !if ($(FTPM_ENABLE) == TRUE)
     Tpm2CommandLib|SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
     Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
     PttPtpLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/PeiDxePttPtpLib/PeiDxePttPtpLib.inf
     Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
   !endif
   TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf
   BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
   OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
   IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf

   #
   # DXE phase common
   #
   HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
   PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
   MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
   ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
   ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
   ScDxeRuntimePciLibPciExpress|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/DxeRuntimePciLibPciExpress/DxeRuntimePciLibPciExpress.inf
   TcgPhysicalPresenceLib|SecurityPkg/Library/DxeTcgPhysicalPresenceLib/DxeTcgPhysicalPresenceLib.inf

   !if $(SOURCE_DEBUG_ENABLE) == TRUE
     SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
   !endif

   LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
   EfiRegTableLib|$(PLATFORM_PACKAGE_COMMON)/Library/EfiRegTableLib/EfiRegTableLib.inf

   !if $(SECURE_BOOT_ENABLE) == TRUE
     BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
   !endif

   PlatformSecureDefaultsLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformSecureDefaultsLib/PlatformSecureDefaultsLib.inf
   SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf

   BasePlatformCmosLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformCmosLib/PlatformCmosLib.inf

   #
   # Reference code Libraries
   #

   #
   # Common
   #
   MmPciLib|$(PLATFORM_SI_PACKAGE)/Library/PeiDxeSmmMmPciLib/PeiDxeSmmMmPciLib.inf

   #
   # South Cluster
   #
   ScAslUpdateLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/SampleCode/Library/AslUpdate/Dxe/ScAslUpdateLib.inf
   ScPlatformLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/ScPlatformLib/ScPlatformLib.inf
   ScDxeRuntimePciLibPciExpress|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/DxeRuntimePciLibPciExpress/DxeRuntimePciLibPciExpress.inf
   ScHdaLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/DxeScHdaLib/DxeScHdaLib.inf
   DxeVtdLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/DxeVtdLib/DxeVtdLib.inf
   PeiDxeSmmScPciExpressHelpersLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/PeiDxeSmmScPciExpressHelpersLib/PeiDxeSmmScPciExpressHelpersLib.inf

   #
   # North Cluster
   #
   SmbiosMemoryLib|$(PLATFORM_SI_PACKAGE)/NorthCluster/Library/DxeSmbiosMemoryLib/DxeSmbiosMemoryLib.inf

   #
   # Txe
   #
   Heci2PowerManagementLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/BaseHeci2PowerManagementNullLib/BaseHeci2PowerManagementNullLib.inf
   !if $(SEC_ENABLE) == TRUE
     PeiSeCLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/PeiSecLib/PeiSeCLib.inf
     SeCLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/SeCLib/SeCLib.inf
   !endif

   FspWrapperApiLib|$(PLATFORM_PACKAGE_COMMON)/SampleCode/IntelFsp2WrapperPkg/Library/BaseFspWrapperApiLib/BaseFspWrapperApiLib.inf
   FspWrapperApiTestLib|IntelFsp2WrapperPkg/Library/BaseFspWrapperApiTestLibNull/BaseFspWrapperApiTestLibNull.inf
   FspWrapperPlatformResetLib|$(PLATFORM_PACKAGE_COMMON)/FspSupport/Library/DxeFspWrapperPlatformResetLib/DxeFspWrapperPlatformResetLib.inf

   BltLib|$(PLATFORM_PACKAGE_COMMON)/Library/FrameBufferBltLib/FrameBufferBltLib.inf

