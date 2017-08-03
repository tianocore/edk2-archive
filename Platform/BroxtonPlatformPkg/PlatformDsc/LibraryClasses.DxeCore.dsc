## @file
#  Dxe Core Library Classes Description.
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

  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

  !if $(PERFORMANCE_ENABLE) == TRUE
     !if $(INTEL_FPDT_ENABLE) == TRUE
        PerformanceLib|$(PLATFORM_PACKAGE_COMMON)/Library/DxeCoreFpdtPerformanceLib/DxeCoreFpdtPerformanceLib.inf # RPPO-APL-0002
     !else
        PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
     !endif

     !if $(SOURCE_DEBUG_ENABLE) == TRUE
        TimerLib|$(PLATFORM_PACKAGE_COMMON)/Override/IA32FamilyCpuPkg/Library/CpuLocalApicTimerLib/CpuLocalApicTimerLib.inf
     !else
        TimerLib|$(PLATFORM_PACKAGE_COMMON)/Library/PlatformTscTimerLib/DxeTscTimerLib.inf
     !endif
  !endif
 
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
!else
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
!endif

