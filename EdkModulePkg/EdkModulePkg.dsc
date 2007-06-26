#/** @file
# EFI/Framework Reference Module Package for All Architectures
#
# This FPD file is used for Package Level build.
#
# Copyright (c) 2006 - 2007, Intel Corporation
#
#  All rights reserved. This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
# 
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = EdkModuleAll
  PLATFORM_GUID                  = DC967761-E6E9-4D1A-B02C-818102DB4CC6
  PLATFORM_VERSION               = 0.3
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = $(WORKSPACE)\Build/EdkModule
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT



################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

[LibraryClasses.IA32.DXE_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IA32]
  IoLib|${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

[LibraryClasses.IA32.DXE_SAL_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IA32.DXE_SMM_DRIVER]
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IA32.PEIM]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IA32.PEI_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IA32.DXE_RUNTIME_DRIVER]
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IA32.BASE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IA32.SEC]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IA32.UEFI_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IA32.DXE_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IA32.UEFI_APPLICATION]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.common.DXE_CORE]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeCoreHobLib/DxeCoreHobLib.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeCoreEntryPoint|${WORKSPACE}\MdePkg\Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  CustomDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreCustomDecompressLibFromHob/DxeCoreCustomDecompressLibFromHob.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf

[LibraryClasses.common]
  TimerLib|${WORKSPACE}\MdePkg\Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  PrintLib|${WORKSPACE}\MdePkg\Library/BasePrintLib/BasePrintLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/BaseUefiTianoDecompressLib/BaseUefiTianoDecompressLib.inf
  DebugLib|${WORKSPACE}\MdePkg\Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/BasePcdLibNull/BasePcdLibNull.inf
  CustomDecompressLib|${WORKSPACE}\EdkModulePkg\Library/BaseCustomDecompressLibNull/BaseCustomDecompressLibNull.inf
  PeCoffGetEntryPointLib|${WORKSPACE}\MdePkg\Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  SerialPortLib|${WORKSPACE}\EdkModulePkg\Library/EdkSerialPortLibNull/EdkSerialPortLibNull.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/BaseMemoryLib/BaseMemoryLib.inf
  BaseLib|${WORKSPACE}\MdePkg\Library/BaseLib/BaseLib.inf
  PeCoffLib|${WORKSPACE}\MdePkg\Library/BasePeCoffLib/BasePeCoffLib.inf
  PerformanceLib|${WORKSPACE}\MdePkg\Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PciIncompatibleDeviceSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkPciIncompatibleDeviceSupportLib/EdkPciIncompatibleDeviceSupportLib.inf
  OemHookStatusCodeLib|${WORKSPACE}\EdkModulePkg\Library/EdkOemHookStatusCodeLibNull/EdkOemHookStatusCodeLibNull.inf
  CacheMaintenanceLib|${WORKSPACE}\MdePkg\Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf

[LibraryClasses.common.PEIM]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PeiServicesTablePointerLib|${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  IoLib|${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  HobLib|${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  PeimEntryPoint|${WORKSPACE}\MdePkg\Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/PeiMemoryLib/PeiMemoryLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkPeCoffLoaderLib/EdkPeCoffLoaderLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf

[LibraryClasses.common.DXE_SAL_DRIVER]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  UefiDriverEntryPoint|${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  UefiRuntimeLib|${WORKSPACE}\EdkModulePkg\Library/EdkUefiRuntimeLib/EdkUefiRuntimeLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf

[LibraryClasses.common.PEI_CORE]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  PeiServicesTablePointerLib|${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  IoLib|${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  HobLib|${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  PeiServicesLib|${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PeiCoreEntryPoint|${WORKSPACE}\MdePkg\Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  UefiDriverEntryPoint|${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  UefiRuntimeLib|${WORKSPACE}\EdkModulePkg\Library/EdkUefiRuntimeLib/EdkUefiRuntimeLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf
  UefiDriverModelLib|${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  UefiDriverEntryPoint|${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf
  UefiDriverModelLib|${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf

[LibraryClasses.common.DXE_DRIVER]
  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  UefiDriverEntryPoint|${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf
  UefiDriverModelLib|${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  EdkUsbLib|${WORKSPACE}\EdkModulePkg\Library/EdkUsbLib/EdkUsbLib.inf
  HobLib|${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  DxeServicesTableLib|${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
#  PrintLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePrintLib/EdkDxePrintLib.inf
  EdkIfrSupportLib|${WORKSPACE}\EdkModulePkg\Library/EdkIfrSupportLib/EdkIfrSupportLib.inf
  UefiRuntimeServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  HiiLib|${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  UefiApplicationEntryPoint|${WORKSPACE}\MdePkg\Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
#  DebugLib|${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  DevicePathLib|${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreUefiDecompressLibFromHob/DxeCoreUefiDecompressLibFromHob.inf
  BaseMemoryLib|${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxePeCoffLoaderFromHobLib/EdkDxePeCoffLoaderFromHobLib.inf
  PcdLib|${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  EdkGraphicsLib|${WORKSPACE}\EdkModulePkg\Library/EdkGraphicsLib/EdkGraphicsLib.inf
  UefiLib|${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ReportStatusCodeLib|${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  TianoDecompressLib|${WORKSPACE}\EdkModulePkg\Library/DxeCoreTianoDecompressLibFromHob/DxeCoreTianoDecompressLibFromHob.inf
  EdkScsiLib|${WORKSPACE}\EdkModulePkg\Library/EdkScsiLib/EdkScsiLib.inf

[LibraryClasses.IPF.DXE_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IPF]
  IoLib|${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

[LibraryClasses.IPF.DXE_SAL_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkDxeSalLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxeRuntimeSalLib/EdkDxeRuntimeSalLib.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IPF.DXE_SMM_DRIVER]
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IPF.PEIM]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IPF.PEI_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IPF.DXE_RUNTIME_DRIVER]
  EdkDxeSalLib|${WORKSPACE}\EdkModulePkg\Library/EdkDxeRuntimeSalLib/EdkDxeRuntimeSalLib.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IPF.BASE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IPF.SEC]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.IPF.UEFI_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IPF.DXE_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.IPF.UEFI_APPLICATION]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.DXE_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.X64]
  IoLib|${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

[LibraryClasses.X64.DXE_SAL_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.PEIM]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.X64.PEI_CORE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.BASE]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.X64.SEC]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf

[LibraryClasses.X64.UEFI_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.DXE_DRIVER]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

[LibraryClasses.X64.UEFI_APPLICATION]
  TimerLib|${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  EdkFvbServiceLib|${WORKSPACE}\EdkModulePkg\Library/EdkFvbServiceLib/EdkFvbServiceLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFixedAtBuild.Ia32]
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdFSBClock|gEfiMdePkgTokenSpaceGuid|200000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaxPeiPcdCallBackNumberPerPcdEntry|gEfiEdkModulePkgTokenSpaceGuid|0x08
  PcdPciIncompatibleDeviceSupportMask|gEfiEdkModulePkgTokenSpaceGuid|0x0
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x06
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdStatusCodeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdVpdBaseAddress|gEfiEdkModulePkgTokenSpaceGuid|0

[PcdsFixedAtBuild.IPF]
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaxPeiPcdCallBackNumberPerPcdEntry|gEfiEdkModulePkgTokenSpaceGuid|0x08
  PcdPciIncompatibleDeviceSupportMask|gEfiEdkModulePkgTokenSpaceGuid|0x0
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x06
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdStatusCodeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdStatusCodeRuntimeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdVpdBaseAddress|gEfiEdkModulePkgTokenSpaceGuid|0


[PcdsFixedAtBuild.X64]
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdFSBClock|gEfiMdePkgTokenSpaceGuid|200000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaxPeiPcdCallBackNumberPerPcdEntry|gEfiEdkModulePkgTokenSpaceGuid|0x08
  PcdPciIncompatibleDeviceSupportMask|gEfiEdkModulePkgTokenSpaceGuid|0x0
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x06
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdStatusCodeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdStatusCodeRuntimeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdVpdBaseAddress|gEfiEdkModulePkgTokenSpaceGuid|0

[PcdsFixedAtBuild.EBC]
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaxPeiPcdCallBackNumberPerPcdEntry|gEfiEdkModulePkgTokenSpaceGuid|0x08
  PcdPciIncompatibleDeviceSupportMask|gEfiEdkModulePkgTokenSpaceGuid|0x0
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x06
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdStatusCodeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdStatusCodeRuntimeMemorySize|gEfiEdkModulePkgTokenSpaceGuid|4
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdVpdBaseAddress|gEfiEdkModulePkgTokenSpaceGuid|0

[PcdsFeatureFlag.IA32]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDevicePathSupportDevicePathFromText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDevicePathSupportDevicePathToText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDxeIplBuildShareCodeHobs|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportCustomDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportEfiDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportTianoDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSwitchToLongMode|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdDxePcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdNtEmulatorEnable|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPciBusHotplugDeviceSupport|gEfiGenericPlatformTokenSpaceGuid|TRUE
  PcdPciIsaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPciVgaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseCallbackOnSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseExEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseGetSizeEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdStatusCodeUseMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE

[PcdsFeatureFlag.IPF]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDevicePathSupportDevicePathFromText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDevicePathSupportDevicePathToText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDxeIplBuildShareCodeHobs|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportCustomDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportEfiDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportTianoDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxePcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdNtEmulatorEnable|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPciBusHotplugDeviceSupport|gEfiGenericPlatformTokenSpaceGuid|TRUE
  PcdPciIsaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPciVgaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseCallbackOnSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseExEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseGetSizeEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdStatusCodeReplayInDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseEfiSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseHardSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE

[PcdsFeatureFlag.X64]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDevicePathSupportDevicePathFromText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDevicePathSupportDevicePathToText|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDxeIplBuildShareCodeHobs|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportCustomDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportEfiDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportTianoDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxePcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdNtEmulatorEnable|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPciBusHotplugDeviceSupport|gEfiGenericPlatformTokenSpaceGuid|TRUE
  PcdPciIsaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPciVgaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseCallbackOnSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseExEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseGetSizeEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdStatusCodeReplayInDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseEfiSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseHardSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE

[PcdsFeatureFlag.EBC]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDevicePathSupportDevicePathFromText|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdDevicePathSupportDevicePathToText|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDxeIplBuildShareCodeHobs|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportCustomDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportEfiDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxeIplSupportTianoDecompress|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdDxePcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdPciBusHotplugDeviceSupport|gEfiGenericPlatformTokenSpaceGuid|TRUE
  PcdPciIsaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPciVgaEnable|gEfiGenericPlatformTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseCallbackOnSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseExEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseGetSizeEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseSetEnabled|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdPeiPcdDatabaseTraverseEnabled|gEfiEdkModulePkgTokenSpaceGuid|TRUE
  PcdStatusCodeReplayInDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeReplayInSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseDataHub|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseEfiSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseHardSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseOEM|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseRuntimeMemory|gEfiEdkModulePkgTokenSpaceGuid|FALSE
  PcdStatusCodeUseSerial|gEfiEdkModulePkgTokenSpaceGuid|FALSE


################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamic.common]
  PcdFlashNvStorageFtwSpareBase|gEfiGenericPlatformTokenSpaceGuid|0x0
  PcdFlashNvStorageVariableSize|gEfiGenericPlatformTokenSpaceGuid|0x0
  PcdFlashNvStorageVariableBase|gEfiGenericPlatformTokenSpaceGuid|0x0
  PcdFlashNvStorageFtwWorkingSize|gEfiGenericPlatformTokenSpaceGuid|0x0
  PcdFlashNvStorageFtwWorkingBase|gEfiGenericPlatformTokenSpaceGuid|0x0
  PcdFlashNvStorageFtwSpareSize|gEfiGenericPlatformTokenSpaceGuid|0x0

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################

[Components.Ia32]
  ${WORKSPACE}\EdkModulePkg\Universal/Network/Snp32_64/Dxe/SNP.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/RuntimeDxe/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/GuidedSectionExtraction/Crc32SectionExtract/Dxe/Crc32SectionExtract.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/DiskIo/Dxe/DiskIo.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/UnicodeCollation/English/Dxe/English.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMouse/Dxe/UsbMouse.inf
  ${WORKSPACE}\EdkModulePkg\Universal/EmuVariable/RuntimeDxe/EmuVariable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeDhcp4/Dxe/Dhcp4.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/ConSplitter/Dxe/ConSplitter.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/PciBus/Dxe/PciBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/Terminal/Dxe/Terminal.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Pei/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbKb/Dxe/UsbKb.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/HiiDataBase/Dxe/HiiDatabase.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Dxe/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi0/UsbCbi0.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Dxe/NullMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBus/Dxe/UsbBus.inf
  ${WORKSPACE}\EdkModulePkg\Application/HelloWorld/HelloWorld.inf
  ${WORKSPACE}\EdkModulePkg\Universal/WatchdogTimer/Dxe/WatchDogTimer.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Runtime/RuntimeDxe/Runtime.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/SetupBrowser/Dxe/SetupBrowser.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/IdeBus/Dxe/idebus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Security/SecurityStub/Dxe/SecurityStub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Uhci/Dxe/Uhci.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBot/Dxe/UsbBot.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DebugSupport/Dxe/DebugSupport.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/AtapiPassThru/Dxe/AtapiPassThru.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/Pei/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/GraphicsConsole/Dxe/GraphicsConsole.inf
  ${WORKSPACE}\EdkModulePkg\Universal/MonotonicCounter/RuntimeDxe/MonotonicCounter.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Ebc/Dxe/Ebc.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/Partition/Dxe/Partition.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DevicePath/Dxe/DevicePath.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/FaultTolerantWriteLite/Dxe/FtwLite.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Pei/BaseMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi1/UsbCbi1.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHub/Dxe/DataHub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMassStorage/Dxe/UsbMassStorage.inf
  ${WORKSPACE}\EdkModulePkg\Core/DxeIplPeim/DxeIpl.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/CirrusLogic/Dxe/CirrusLogic5430.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHubStdErr/Dxe/DataHubStdErr.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/DriverSample/DriverSample.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Ehci/Dxe/Ehci.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Debugger/Debugport/Dxe/DebugPort.inf
  ${WORKSPACE}\EdkModulePkg\Core/Pei/PeiMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/ConPlatform/Dxe/ConPlatform.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiDisk/Dxe/ScsiDisk.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiBus/Dxe/ScsiBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Pei/PeiStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Core/Dxe/DxeMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeBc/Dxe/BC.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Dxe/DxeStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Undi/RuntimeDxe/Undi.inf

[Components.IPF]
  ${WORKSPACE}\EdkModulePkg\Universal/Network/Snp32_64/Dxe/SNP.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/GuidedSectionExtraction/Crc32SectionExtract/Dxe/Crc32SectionExtract.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/DiskIo/Dxe/DiskIo.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/UnicodeCollation/English/Dxe/English.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMouse/Dxe/UsbMouse.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeDhcp4/Dxe/Dhcp4.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/ConSplitter/Dxe/ConSplitter.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/PciBus/Dxe/PciBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/Terminal/Dxe/Terminal.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Pei/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbKb/Dxe/UsbKb.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/HiiDataBase/Dxe/HiiDatabase.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Dxe/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi0/UsbCbi0.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Dxe/NullMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBus/Dxe/UsbBus.inf
  ${WORKSPACE}\EdkModulePkg\Application/HelloWorld/HelloWorld.inf
  ${WORKSPACE}\EdkModulePkg\Universal/WatchdogTimer/Dxe/WatchDogTimer.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Runtime/RuntimeDxe/Runtime.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/SetupBrowser/Dxe/SetupBrowser.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/IdeBus/Dxe/idebus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Security/SecurityStub/Dxe/SecurityStub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Uhci/Dxe/Uhci.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBot/Dxe/UsbBot.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DebugSupport/Dxe/DebugSupport.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/AtapiPassThru/Dxe/AtapiPassThru.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/Pei/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/GraphicsConsole/Dxe/GraphicsConsole.inf
  ${WORKSPACE}\EdkModulePkg\Universal/EmuVariable/RuntimeDxe/EmuVariableIpf.inf
  ${WORKSPACE}\EdkModulePkg\Universal/MonotonicCounter/RuntimeDxe/MonotonicCounter.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Ebc/Dxe/Ebc.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/RuntimeDxe/VariableIpf.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/Partition/Dxe/Partition.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DevicePath/Dxe/DevicePath.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/FaultTolerantWriteLite/Dxe/FtwLite.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Pei/BaseMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi1/UsbCbi1.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHub/Dxe/DataHub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMassStorage/Dxe/UsbMassStorage.inf
  ${WORKSPACE}\EdkModulePkg\Core/DxeIplPeim/DxeIpl.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/CirrusLogic/Dxe/CirrusLogic5430.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHubStdErr/Dxe/DataHubStdErr.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/DriverSample/DriverSample.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Ehci/Dxe/Ehci.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Debugger/Debugport/Dxe/DebugPort.inf
  ${WORKSPACE}\EdkModulePkg\Core/Pei/PeiMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/ConPlatform/Dxe/ConPlatform.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiDisk/Dxe/ScsiDisk.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiBus/Dxe/ScsiBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Pei/PeiStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Core/Dxe/DxeMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeBc/Dxe/BC.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Dxe/DxeStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Undi/RuntimeDxe/Undi.inf

[Components.X64]
  ${WORKSPACE}\EdkModulePkg\Universal/Network/Snp32_64/Dxe/SNP.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/RuntimeDxe/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/GuidedSectionExtraction/Crc32SectionExtract/Dxe/Crc32SectionExtract.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/DiskIo/Dxe/DiskIo.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/UnicodeCollation/English/Dxe/English.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMouse/Dxe/UsbMouse.inf
  ${WORKSPACE}\EdkModulePkg\Universal/EmuVariable/RuntimeDxe/EmuVariable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeDhcp4/Dxe/Dhcp4.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/ConSplitter/Dxe/ConSplitter.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/PciBus/Dxe/PciBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/Terminal/Dxe/Terminal.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Pei/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbKb/Dxe/UsbKb.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/HiiDataBase/Dxe/HiiDatabase.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Dxe/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi0/UsbCbi0.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Dxe/NullMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBus/Dxe/UsbBus.inf
  ${WORKSPACE}\EdkModulePkg\Application/HelloWorld/HelloWorld.inf
  ${WORKSPACE}\EdkModulePkg\Universal/WatchdogTimer/Dxe/WatchDogTimer.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Runtime/RuntimeDxe/Runtime.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/SetupBrowser/Dxe/SetupBrowser.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/IdeBus/Dxe/idebus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Security/SecurityStub/Dxe/SecurityStub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Uhci/Dxe/Uhci.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBot/Dxe/UsbBot.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DebugSupport/Dxe/DebugSupport.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/AtapiPassThru/Dxe/AtapiPassThru.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/Pei/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/GraphicsConsole/Dxe/GraphicsConsole.inf
  ${WORKSPACE}\EdkModulePkg\Universal/MonotonicCounter/RuntimeDxe/MonotonicCounter.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Ebc/Dxe/Ebc.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/Partition/Dxe/Partition.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DevicePath/Dxe/DevicePath.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/FaultTolerantWriteLite/Dxe/FtwLite.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Pei/BaseMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi1/UsbCbi1.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHub/Dxe/DataHub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMassStorage/Dxe/UsbMassStorage.inf
  ${WORKSPACE}\EdkModulePkg\Core/DxeIplPeim/DxeIpl.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/CirrusLogic/Dxe/CirrusLogic5430.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHubStdErr/Dxe/DataHubStdErr.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/DriverSample/DriverSample.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Ehci/Dxe/Ehci.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Debugger/Debugport/Dxe/DebugPort.inf
  ${WORKSPACE}\EdkModulePkg\Core/Pei/PeiMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/ConPlatform/Dxe/ConPlatform.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiDisk/Dxe/ScsiDisk.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiBus/Dxe/ScsiBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Pei/PeiStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Core/Dxe/DxeMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeBc/Dxe/BC.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Dxe/DxeStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Undi/RuntimeDxe/Undi.inf

[Components.EBC]
  ${WORKSPACE}\EdkModulePkg\Universal/Network/Snp32_64/Dxe/SNP.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/GuidedSectionExtraction/Crc32SectionExtract/Dxe/Crc32SectionExtract.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/DiskIo/Dxe/DiskIo.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/UnicodeCollation/English/Dxe/English.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMouse/Dxe/UsbMouse.inf
  ${WORKSPACE}\EdkModulePkg\Universal/EmuVariable/RuntimeDxe/EmuVariable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Network/PxeDhcp4/Dxe/Dhcp4.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/ConSplitter/Dxe/ConSplitter.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/Terminal/Dxe/Terminal.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Pei/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbKb/Dxe/UsbKb.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/HiiDataBase/Dxe/HiiDatabase.inf
  ${WORKSPACE}\EdkModulePkg\Universal/PCD/Dxe/Pcd.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi0/UsbCbi0.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Dxe/NullMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBus/Dxe/UsbBus.inf
  ${WORKSPACE}\EdkModulePkg\Application/HelloWorld/HelloWorld.inf {
    <LibraryClass>
      DebugLib|${WORKSPACE}\MdePkg\Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  }
  ${WORKSPACE}\EdkModulePkg\Universal/WatchdogTimer/Dxe/WatchDogTimer.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Runtime/RuntimeDxe/Runtime.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/SetupBrowser/Dxe/SetupBrowser.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/IdeBus/Dxe/idebus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Security/SecurityStub/Dxe/SecurityStub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Uhci/Dxe/Uhci.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbBot/Dxe/UsbBot.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/AtapiPassThru/Dxe/AtapiPassThru.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Variable/Pei/Variable.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Console/GraphicsConsole/Dxe/GraphicsConsole.inf
  ${WORKSPACE}\EdkModulePkg\Universal/MonotonicCounter/RuntimeDxe/MonotonicCounter.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/CirrusLogic/Dxe/CirrusLogic5430.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Disk/Partition/Dxe/Partition.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DevicePath/Dxe/DevicePath.inf
  ${WORKSPACE}\EdkModulePkg\Universal/FirmwareVolume/FaultTolerantWriteLite/Dxe/FtwLite.inf
  ${WORKSPACE}\EdkModulePkg\Universal/GenericMemoryTest/Pei/BaseMemoryTest.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbCbi/Dxe/Cbi1/UsbCbi1.inf
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHub/Dxe/DataHub.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Usb/UsbMassStorage/Dxe/UsbMassStorage.inf
  ${WORKSPACE}\EdkModulePkg\Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClass>
      EdkPeCoffLoaderLib|${WORKSPACE}\EdkModulePkg\Library/EdkPeCoffLoaderLib/EdkPeCoffLoaderLib.inf
  }
  ${WORKSPACE}\EdkModulePkg\Universal/DataHub/DataHubStdErr/Dxe/DataHubStdErr.inf
  ${WORKSPACE}\EdkModulePkg\Universal/UserInterface/DriverSample/DriverSample.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Ehci/Dxe/Ehci.inf
  ${WORKSPACE}\EdkModulePkg\Universal/Debugger/Debugport/Dxe/DebugPort.inf
  ${WORKSPACE}\EdkModulePkg\Core/Pei/PeiMain.inf
  ${WORKSPACE}\EdkModulePkg\Universal/ConPlatform/Dxe/ConPlatform.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiDisk/Dxe/ScsiDisk.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Scsi/ScsiBus/Dxe/ScsiBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Pei/PeiStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Core/Dxe/DxeMain.inf {
    <BuildOptions>
      *_VS2005PRO_EBC_CC_FLAGS                    = /WX- /Ob0
      *_WINDDK3790x1830_EBC_CC_FLAGS              = /WX- /Ob0
      *_ICC_EBC_CC_FLAGS                          = /WX- /Ob0
      *_MYTOOLS_EBC_CC_FLAGS                      = /WX- /Ob0
  }
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/PciBus/Dxe/PciBus.inf
  ${WORKSPACE}\EdkModulePkg\Universal/StatusCode/Dxe/DxeStatusCode.inf
  ${WORKSPACE}\EdkModulePkg\Bus/Pci/Undi/RuntimeDxe/Undi.inf

