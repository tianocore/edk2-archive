## @file
#  Dxe Driver Library Classes Description.
#
#  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
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

  !if $(PERFORMANCE_ENABLE) == TRUE
    PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
    TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformTscTimerLib/DxeTscTimerLib.inf
  !endif
  
  !if $(SOURCE_DEBUG_ENABLE) == TRUE
    DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
  !else
    DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  !endif