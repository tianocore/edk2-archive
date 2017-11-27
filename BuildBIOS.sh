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
## Function define
##**********************************************************************
function Usage () {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: BuildBios.sh Build_Flags [PlatformName]  Target_Flag"
  echo
  echo "       Build_Flags:                 /MN    MinnowBoard3 (default: MN)"
  echo "       Build_Flags:                 /BG    Benson Glacier Board"
  echo "       Build_Flags:                 /MX    MinnowBoard3 Next"
  echo "       Build_Flags:                 /A     Set FabId to A (default:  FAB_B)"
  echo "       Build_Flags:                 /B     Set FabId to B (default:  FAB_B)"
  echo "       PlatformName [optional]:     Broxton  "                   
  echo "       Target_Flag:                 Release, Debug       "
  echo
  echo "Press any key......"
  read
  exit 0
}

##**********************************************************************
## Initial Setup
##**********************************************************************

Build_Flags=

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

if [ "$1" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  Usage
fi


## Build Flags
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MN" ]; then
      BoardId=MN
      Build_Flags="$Build_Flags /MN"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/BG" ]; then
      BoardId=BG
      Build_Flags="$Build_Flags /BG"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MX" ]; then
      BoardId=MX
      Build_Flags="$Build_Flags /MX"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/B" ]; then
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

if [ "$1" == "Broxton" ]; then
       shift
 else
   echo "No parameter for board : assume Broxton"  
fi

Target_Flag=Debug

if [ "$1" == "Debug" ]; then
  Target_Flag=Debug
elif [ "$1" == "Release" ]; then
  Target_Flag=Release
else
  echo "Error - Invalid Target"
  echo "  Please review the Help screen"
  Usage
fi

echo $Build_Flags
echo $Target_Flag

export WORKSPACE=`pwd`
export PACKAGES_PATH=$WORKSPACE:$WORKSPACE/Core:$WORKSPACE/Silicon/:$WORKSPACE/Platform:$WORKSPACE/Platform/BroxtonPlatformPkg:$WORKSPACE/Silicon/BroxtonSoC:$WORKSPACE/Platform/BroxtonPlatformPkg/Common

. edksetup.sh BaseTools

make -C BaseTools

bash ./Platform/BroxtonPlatformPkg/BuildIFWI.sh $Build_Flags Broxton $Target_Flag

