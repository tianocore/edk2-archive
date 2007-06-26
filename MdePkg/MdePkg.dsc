#/** @file
# EFI/Tiano MdePkg Package
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
  PLATFORM_NAME                  = MdePkgAll
  PLATFORM_GUID                  = 0FE7DD3E-0969-48C3-8CD2-DE9A190088E2
  PLATFORM_VERSION               = 0.3
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = $(WORKSPACE)\Build/Mde
  SUPPORTED_ARCHITECTURES        = IA32|IPF|X64|EBC
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT



################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdPatchableInModule.IA32]
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000

[PcdPatchableInModule.IPF]
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000

[PcdPatchableInModule.X64]
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000

[PcdPatchableInModule.EBC]
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000
  PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid|0x80000000

[PcdFixedAtBuild.IA32]
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdFSBClock|gEfiMdePkgTokenSpaceGuid|200000000
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdPciExpressBaseAddress|gEfiMdePkgTokenSpaceGuid|0xE0000000
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF

[PcdFixedAtBuild.IPF]
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdPciExpressBaseAddress|gEfiMdePkgTokenSpaceGuid|0xE0000000
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdIoBlockBaseAddressForIpf|gEfiMdePkgTokenSpaceGuid|0x0ffffc000000

[PcdFixedAtBuild.X64]
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdFSBClock|gEfiMdePkgTokenSpaceGuid|200000000
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdPciExpressBaseAddress|gEfiMdePkgTokenSpaceGuid|0xE0000000
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF

[PcdFixedAtBuild.EBC]
  PcdMaximumUnicodeStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdMaximumAsciiStringLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdSpinLockTimeout|gEfiMdePkgTokenSpaceGuid|10000000
  PcdMaximumLinkedListLength|gEfiMdePkgTokenSpaceGuid|1000000
  PcdPerformanceLibraryPropertyMask|gEfiMdePkgTokenSpaceGuid|0
  PcdPciExpressBaseAddress|gEfiMdePkgTokenSpaceGuid|0xE0000000
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdReportStatusCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x07
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdPostCodePropertyMask|gEfiMdePkgTokenSpaceGuid|0x00
  PcdUefiLibMaxPrintBufferSize|gEfiMdePkgTokenSpaceGuid|320
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF
  PcdDebugPropertyMask|gEfiMdePkgTokenSpaceGuid|0x0f
  PcdDebugClearMemoryValue|gEfiMdePkgTokenSpaceGuid|0xAF

[PcdFeatureFlag.IA32]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE

[PcdFeatureFlag.IPF]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE

[PcdFeatureFlag.X64]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE

[PcdFeatureFlag.EBC]
  PcdComponentNameDisable|gEfiMdePkgTokenSpaceGuid|FALSE
  PcdDriverDiagnosticsDisable|gEfiMdePkgTokenSpaceGuid|FALSE


################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################

[Components.IA32]
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLibMm7/PeiServicesTablePointerLibMm7.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibCf8/BasePciLibCf8.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryLib/PeiMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseDebugLibNull/BaseDebugLibNull.inf
  ${WORKSPACE}\MdePkg\Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  ${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  ${WORKSPACE}\MdePkg\Library/PeiSmbusLib/PeiSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLib/BaseMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseLib/BaseLib.inf
  ${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/PeimEntryPoint/PeimEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  ${WORKSPACE}\MdePkg\Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePrintLib/BasePrintLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffLib/BasePeCoffLib.inf
  ${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibSse2/BaseMemoryLibSse2.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxePostCodeLibReportStatusCode/PeiDxePostCodeLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/BasePciExpressLib/BasePciExpressLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmbusLib/DxeSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibMmx/BaseMemoryLibMmx.inf
  ${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmmDriverEntryPoint/DxeSmmDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  ${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreHobLib/DxeCoreHobLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  ${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciCf8Lib/BasePciCf8Lib.inf
  ${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibDebug/BasePostCodeLibDebug.inf
  ${WORKSPACE}\MdePkg\Library/BasePcdLibNull/BasePcdLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  ${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  ${WORKSPACE}\MdePkg\Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[Components.IPF]
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  ${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryLib/PeiMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseDebugLibNull/BaseDebugLibNull.inf
  ${WORKSPACE}\MdePkg\Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  ${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  ${WORKSPACE}\MdePkg\Library/PeiSmbusLib/PeiSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLib/BaseMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseLib/BaseLib.inf
  ${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  ${WORKSPACE}\MdePkg\Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePrintLib/BasePrintLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffLib/BasePeCoffLib.inf
  ${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxePostCodeLibReportStatusCode/PeiDxePostCodeLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/BasePciExpressLib/BasePciExpressLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmbusLib/DxeSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/PeimEntryPoint/PeimEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLibKr1/PeiServicesTablePointerLibKr1.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  ${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreHobLib/DxeCoreHobLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibCf8/BasePciLibCf8.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciCf8Lib/BasePciCf8Lib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibDebug/BasePostCodeLibDebug.inf
  ${WORKSPACE}\MdePkg\Library/BasePcdLibNull/BasePcdLibNull.inf
  ${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  ${WORKSPACE}\MdePkg\Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[Components.X64]
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLibMm7/PeiServicesTablePointerLibMm7.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibCf8/BasePciLibCf8.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryLib/PeiMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseDebugLibNull/BaseDebugLibNull.inf
  ${WORKSPACE}\MdePkg\Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  ${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  ${WORKSPACE}\MdePkg\Library/PeiSmbusLib/PeiSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLib/BaseMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseLib/BaseLib.inf
  ${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/PeimEntryPoint/PeimEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  ${WORKSPACE}\MdePkg\Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePrintLib/BasePrintLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffLib/BasePeCoffLib.inf
  ${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibSse2/BaseMemoryLibSse2.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxePostCodeLibReportStatusCode/PeiDxePostCodeLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/BasePciExpressLib/BasePciExpressLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmbusLib/DxeSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibMmx/BaseMemoryLibMmx.inf
  ${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmmDriverEntryPoint/DxeSmmDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  ${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreHobLib/DxeCoreHobLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  ${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciCf8Lib/BasePciCf8Lib.inf
  ${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibDebug/BasePostCodeLibDebug.inf
  ${WORKSPACE}\MdePkg\Library/BasePcdLibNull/BasePcdLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  ${WORKSPACE}\MdePkg\Library/SecPeiDxeTimerLibCpu/SecPeiDxeTimerLibCpu.inf
  ${WORKSPACE}\MdePkg\Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

[Components.EBC]
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf
  ${WORKSPACE}\MdePkg\Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiPcdLib/PeiPcdLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesLib/PeiServicesLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryLib/PeiMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseDebugLibNull/BaseDebugLibNull.inf
  ${WORKSPACE}\MdePkg\Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  ${WORKSPACE}\MdePkg\Library/UefiLib/UefiLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  ${WORKSPACE}\MdePkg\Library/DxeSmbusLib/DxeSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseMemoryLib/BaseMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  ${WORKSPACE}\MdePkg\Library/BaseLib/BaseLib.inf
  ${WORKSPACE}\MdePkg\Library/HiiLib/HiiLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeHobLib/DxeHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryLib/DxeMemoryLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  ${WORKSPACE}\MdePkg\Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePrintLib/BasePrintLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffLib/BasePeCoffLib.inf
  ${WORKSPACE}\MdePkg\Library/DxePcdLib/DxePcdLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeIoLibCpuIo/DxeIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/PeiResourcePublicationLib/PeiResourcePublicationLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciExpressLib/BasePciExpressLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiSmbusLib/PeiSmbusLib.inf
  ${WORKSPACE}\MdePkg\Library/PeimEntryPoint/PeimEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiHobLib/PeiHobLib.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ${WORKSPACE}\MdePkg\Library/PeiDxePostCodeLibReportStatusCode/PeiDxePostCodeLibReportStatusCode.inf
  ${WORKSPACE}\MdePkg\Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDriverModelLib/UefiDriverModelLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeCoreHobLib/DxeCoreHobLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciLibCf8/BasePciLibCf8.inf
  ${WORKSPACE}\MdePkg\Library/PeiIoLibCpuIo/PeiIoLibCpuIo.inf
  ${WORKSPACE}\MdePkg\Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDevicePathLib/UefiDevicePathLib.inf
  ${WORKSPACE}\MdePkg\Library/BasePciCf8Lib/BasePciCf8Lib.inf
  ${WORKSPACE}\MdePkg\Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  ${WORKSPACE}\MdePkg\Library/BasePostCodeLibDebug/BasePostCodeLibDebug.inf
  ${WORKSPACE}\MdePkg\Library/BasePcdLibNull/BasePcdLibNull.inf
  ${WORKSPACE}\MdePkg\Library/DxeMemoryAllocationLib/DxeMemoryAllocationLib.inf
  ${WORKSPACE}\MdePkg\Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  ${WORKSPACE}\MdePkg\Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf

