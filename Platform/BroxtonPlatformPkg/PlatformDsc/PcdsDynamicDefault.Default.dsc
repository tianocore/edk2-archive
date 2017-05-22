## @file
#  Platform Dynamic Pcd Description.
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

  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0

  !if $(FTPM_ENABLE) == TRUE
    gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{0x7b, 0x3a, 0xcd, 0x72, 0xA5, 0xFE, 0x5e, 0x4f, 0x91, 0x65, 0x4d, 0xd1, 0x21, 0x87, 0xbb, 0x13}
  !endif

  # This PCD should be set to TRUE to keep PlatformAuth Enabled during runtime
  gClientCommonModuleTokenSpaceGuid.PcdTpm2HierarchyControlPlatform|1

  ## This PCD defines the video horizontal resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|800
  ## This PCD defines the video vertical resolution.
  #  This PCD could be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|600
  ## This PCD defines the Console output column and the default value is 25 according to UEFI spec.
  #  This PCD could be set to 0 then console output could be at max column and max row.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|31
  ## This PCD defines the Console output row and the default value is 80 according to UEFI spec.
  #  This PCD could be set to 0 then console output could be at max column and max row.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|100

  ## The PCD is used to specify the video horizontal resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoHorizontalResolution|800
  ## The PCD is used to specify the video vertical resolution of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupVideoVerticalResolution|600
  ## The PCD is used to specify the console output column of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn|100
  ## The PCD is used to specify the console output column of text setup.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow|31

  ## The PCD is used to Enable/Disable Chunk Logic for Scsi device
  gPlatformModuleTokenSpaceGuid.PcdScsiChunk|0
  ## The PCD is used to access Ufs device Bar address
  gPlatformModuleTokenSpaceGuid.PcdUfsHcBar|0

  gPlatformModuleTokenSpaceGuid.PcdUfsInitStall|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase|0xfff60000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase|0xfff9e000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase|0xfffa0000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFastPS2Detection|FALSE
  
  ## Indicate whether a physical presence user exist.
  # When it is configured to Dynamic or DynamicEx, it can be set through detection using 
  # a platform-specific method (e.g. Button pressed) in a actual platform in early boot phase.<BR><BR>
  # @Prompt A physical presence user status
  gEfiSecurityPkgTokenSpaceGuid.PcdUserPhysicalPresence|TRUE

