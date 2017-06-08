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

##**********************************************************************
## Initial Setup
##**********************************************************************

Build_Flags=

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

## Build Flags
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/B" ]; then
      FabId=B
      Build_Flags="$Build_Flags /B"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/A" ]; then
      FabId=A
      Build_Flags="$Build_Flags /A"
      shift
    else
      break
    fi
  done

Target_Flag=Release
if [ "$1" == "Debug" ]; then
  Target_Flag=Debug
  shift
fi
if [ "$1" == "Release" ]; then
  Target_Flag=Release
  shift
fi

echo $Build_Flags
echo $Target_Flag

export WORKSPACE=`pwd`
export PACKAGES_PATH=$WORKSPACE:$WORKSPACE/Core:$WORKSPACE/Silicon/:$WORKSPACE/Platform:$WORKSPACE/Platform/BroxtonPlatformPkg:$WORKSPACE/Silicon/BroxtonSoC:$WORKSPACE/Platform/BroxtonPlatformPkg/Common

. edksetup.sh

make -C BaseTools

bash ./Platform/BroxtonPlatformPkg/BuildIFWI.sh $Build_Flags APLI $Target_Flag

