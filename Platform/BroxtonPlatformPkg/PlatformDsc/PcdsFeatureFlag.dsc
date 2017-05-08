## @file
#  Platform Feature Pcd Description.
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
  # If PcdDxeIplSwitchToLongMode is TRUE, DxeIpl will load a 64-bit DxeCore and switch to long mode to hand over to DxeCore.
  #
  !if $(X64_CONFIG) == TRUE
    gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|FALSE
  !endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdBrowserGrayOutTextStatement|TRUE

  !if $(CAPSULE_RESET_ENABLE) == TRUE
    gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|FALSE
  !endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdFrameworkCompatibilitySupport|TRUE

  !if $(DATAHUB_STATUS_CODE_ENABLE) == TRUE
    gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseDataHub|TRUE
  !else
    gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdStatusCodeUseDataHub|FALSE
  !endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreImageLoaderSearchTeSectionFirst|FALSE

  !if $(TARGET) == RELEASE
    gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
    gPlatformModuleTokenSpaceGuid.PcdStatusCodeUseSerialPortPlatform|FALSE
  !endif

  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE

  !if $(ISA_SERIAL_STATUS_CODE_ENABLE) == TRUE
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseIsaSerial|TRUE
  !else
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseIsaSerial|FALSE
  !endif

  !if $(USB3_SERIAL_STATUS_CODE_ENABLE) == TRUE
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseUsbSerial|TRUE
  !else
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseUsbSerial|FALSE
  !endif

  !if $(RAM_SERIAL_STATUS_CODE_ENABLE) == TRUE
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseRam|TRUE
  !else
    gEfiSerialPortTokenSpaceGuid.PcdStatusCodeUseRam|FALSE
  !endif

  !if $(BXTI_PF_BUILD) == TRUE
    gPlatformModuleTokenSpaceGuid.PcdStatusCodeUseBeep|FALSE
  !endif

  !if $(VARIABLE_INFO_ENABLE) == TRUE
    gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|TRUE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdVariableCollectStatistics|FALSE
  !endif

  !if $(LZMA_ENABLE) == FALSE
    gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSupportUefiDecompress|TRUE
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSupportUefiDecompress|FALSE
  !endif

  # Optimize Driver init time in FastBoot Mode
  gEfiMdeModulePkgTokenSpaceGuid.PcdPs2KbdExtendedVerification|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPs2MouseExtendedVerification|FALSE

  # Disable Deprecated Function
  # bugbug benben: set this to FALSE to allow PcdSetXXX() to work in SecurityPkg. Check
  # new core to see if we can re-enable this
  gPlatformModuleTokenSpaceGuid.PcdDeprecatedFunctionRemove|FALSE

  ## Indicates if SMM Startup AP in a blocking fashion.
  #   TRUE  - SMM Startup AP in a blocking fashion.<BR>
  #   FALSE - SMM Startup AP in a non-blocking fashion.<BR>
  # @Prompt SMM Startup AP in a blocking fashion.
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmBlockStartupThisAp|FALSE