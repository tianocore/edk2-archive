#
# Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
# 
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
Target_Flag=Release
if [ "$1" == "Debug" ]; then
  Target_Flag=Debug
  shift
fi
if [ "$1" == "Release" ]; then
  Target_Flag=Release
  shift
fi

echo $Target_Flag

export WORKSPACE=`pwd`
export PACKAGES_PATH=$WORKSPACE:$WORKSPACE/Core:$WORKSPACE/Silicon/:$WORKSPACE/Platform:$WORKSPACE/Platform/BroxtonPlatformPkg:$WORKSPACE/Silicon/BroxtonSoC:$WORKSPACE/Platform/BroxtonPlatformPkg/Common

. edksetup.sh

make -C BaseTools

bash ./Platform/BroxtonPlatformPkg/BuildIFWI.sh APLI $Target_Flag

