## @file
#  Component description file for the Broxton RC PEI libraries.
#
#  Copyright (c) 2004 - 2017, Intel Corporation. All rights reserved.<BR>
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
# RC Pei Library
#

#
# Common
#
  CseVariableStorageSelectorLib|$(PLATFORM_SI_PACKAGE)/Library/Private/PeiCseVariableStorageSelectorLib/PeiCseVariableStorageSelectorLib.inf
  BpdtLib|$(PLATFORM_SI_PACKAGE)/Library/BpdtLib/BpdtLib.inf
  
#
# SC
#
  PeiScPolicyLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/PeiScPolicyLib/PeiScPolicyLib.inf
  PeiSpiInitLib|$(PLATFORM_SI_PACKAGE)/SouthCluster/Library/PeiSpiInitLib/PeiSpiInitLib.inf

#
# CPU
#
  CpuPlatformLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiDxeSmmCpuPlatformLib/PeiDxeSmmCpuPlatformLib.inf
  CpuPolicyLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiCpuPolicyLibPreMem/PeiCpuPolicyLibPreMem.inf
  CpuPolicyLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiCpuPolicyLib/PeiCpuPolicyLib.inf
  MpServiceLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/Private/PeiMpServiceLib/PeiMpServiceLib.inf
  CpuS3Lib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/Private/PeiCpuS3Lib/PeiCpuS3Lib.inf


#
# TXE
#
  HeciMsgLib|$(PLATFORM_SI_PACKAGE)/Txe/Library/HeciMsgLib/PeiHeciMsgLib.inf

