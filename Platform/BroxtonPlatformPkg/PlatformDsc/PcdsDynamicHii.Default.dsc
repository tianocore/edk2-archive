## @file
#  Platform Dynamic Hii Pcd Description.
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

  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|L"Timeout"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"Timeout"
  gClientCommonModuleTokenSpaceGuid.PcdPlatformMemoryCheck|L"MemoryCheck"|gClientCommonModuleTokenSpaceGuid|0x0|0
  gEfiMdePkgTokenSpaceGuid.PcdHardwareErrorRecordLevel|L"HwErrRecSupport"|gEfiGlobalVariableGuid|0x0|1 # Variable: L"HwErrRecSupport"
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdBootState|L"BootState"|gEfiBootStateGuid|0x0|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|L"ConOutConfig"|gClientCommonModuleTokenSpaceGuid|0x0|100
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|L"ConOutConfig"|gClientCommonModuleTokenSpaceGuid|0x4|31

