## @file
#  Component description file for the Broxton RC DXE libraries.
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
# RC Dxe Library
#

#
# Common
#
  !if $(SMM_VARIABLE_ENABLE) == TRUE
    CseVariableStorageSelectorLib|$(PLATFORM_SI_PACKAGE)/Library/Private/DxeSmmCseVariableStorageSelectorLib/SmmCseVariableStorageSelectorLib.inf
  !else
    CseVariableStorageSelectorLib|$(PLATFORM_SI_PACKAGE)/Library/Private/DxeSmmCseVariableStorageSelectorLib/DxeCseVariableStorageSelectorLib.inf
  !endif
  SmmHeciMsgLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/SmmHeciMsgLib/SmmHeciMsgLib.inf

#
# TXE
#
  HeciMsgLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/HeciMsgLib/DxeSmmHeciMsgLib.inf
  SeCLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/SeCLib/SeCLib.inf

#
# SMBus
#  
  ScSmbusCommonLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/Private/PeiDxeSmmScSmbusCommonLib/PeiDxeSmmScSmbusCommonLib.inf
