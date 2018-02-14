## @file
#  Component description file for the Broxton RC DXE drivers.
#
#  Copyright (c) 2004 - 2018, Intel Corporation. All rights reserved.<BR>
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
# CPU
#
  !if $(PPM_ENABLE) == TRUE
    $(PLATFORM_SI_PACKAGE)/Cpu/PowerManagement/Dxe/PowerMgmtDxe.inf
    $(PLATFORM_SI_PACKAGE)/Cpu/PowerManagement/Smm/PowerMgmtSmm.inf
    $(PLATFORM_SI_PACKAGE)/Cpu/AcpiTables/CpuAcpiTables.inf
  !endif
    $(PLATFORM_SI_PACKAGE)/Cpu/SmmAccess/Dxe/SmmAccess.inf

#
# TXE
#
  !if $(SEC_ENABLE) == TRUE
    $(PLATFORM_SI_PACKAGE)/Txe/Heci/Dxe/Hecidrv.inf
  !endif

#
# HSTI
#
!if $(HSTI_ENABLE) == TRUE
$(PLATFORM_SI_PACKAGE)/Hsti/Dxe/HstiSiliconDxe.inf {
    <LibraryClasses>
      HstiLib | MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
      Tpm2CommandLib | SecurityPkg/Library/Tpm2CommandLib/Tpm2CommandLib.inf
  }
!endif

$(PLATFORM_SI_PACKAGE)/SouthCluster/Smbus/Dxe/PchSmbusDxe.inf {
    <PcdsPatchableInModule>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xFFFFFFFF
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
